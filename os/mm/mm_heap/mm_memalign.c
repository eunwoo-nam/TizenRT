/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * mm/mm_heap/mm_memalign.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <tinyara/config.h>

#include <assert.h>
#include <debug.h>

#include <tinyara/mm/mm.h>
#include <tinyara/mm/kasan.h>
#include <tinyara/sched_note.h>

#include "mm_heap/mm.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: memalign_from_freelist
 *
 * Description:
 *   Take an aligned chunk directly from the best fitting free node that can
 *   hold it, without allocating 2 * alignment extra bytes first.
 *
 * Input Parameters:
 *   heap      - The selected heap
 *   alignment - The alignment, a power of two not less than MM_MIN_CHUNK
 *   size      - The chunk size, including MM_ALLOCNODE_OVERHEAD
 *
 * Returned Value:
 *   The aligned memory, or NULL if no free node can hold the chunk.
 *
 ****************************************************************************/

static FAR void *memalign_from_freelist(FAR struct mm_heap_s *heap,
                                        size_t alignment, size_t size)
{
	FAR struct mm_freenode_s *node;
	FAR struct mm_allocnode_s *newnode;
	FAR struct mm_allocnode_s *next;
	uintptr_t alignedchunk;
	size_t precedingsize;
	size_t nodesize;
	size_t mask = alignment - 1;
	FAR void *ret;
	int ndx;

	mm_free_delaylist(heap);

	DEBUGVERIFY(mm_lock(heap));

	/* Each bin is sorted in descending order.  Start at the smallest node
	 * of the bin that is large enough, and go back to the larger ones until
	 * a node can hold the chunk at an aligned address.
	 */

	for (ndx = mm_size2ndx(size); ndx < MM_NNODES; ndx++) {
		node = heap->mm_nodelist[ndx].flink;
		if (node == NULL || MM_SIZEOF_NODE(node) < size) {
			continue;
		}

		while (node->flink && MM_SIZEOF_NODE(node->flink) >= size) {
			node = node->flink;
		}

		for (; node != &heap->mm_nodelist[ndx]; node = node->blink) {
			alignedchunk  = ((uintptr_t)node + MM_SIZEOF_ALLOCNODE + mask) &
			                ~mask;
			precedingsize = alignedchunk - MM_SIZEOF_ALLOCNODE -
			                (uintptr_t)node;

			/* The space before the chunk must be empty or large enough to
			 * become a free node; if not, use the next alignment point.
			 */

			if (precedingsize > 0 && precedingsize < MM_MIN_CHUNK) {
				alignedchunk  += alignment;
				precedingsize += alignment;
			}

			if (precedingsize + size <= MM_SIZEOF_NODE(node)) {
				goto found;
			}
		}
	}

	mm_unlock(heap);
	return NULL;

found:

	/* Remove the node.  There must be a predecessor, but there may not be a
	 * successor node.
	 */

	DEBUGASSERT(node->blink->flink == node);
	node->blink->flink = node->flink;
	if (node->flink) {
		node->flink->blink = node->blink;
	}

	nodesize = MM_SIZEOF_NODE(node);
	next = (FAR struct mm_allocnode_s *)((FAR char *)node + nodesize);
	DEBUGASSERT(MM_NODE_IS_ALLOC(next) && MM_PREVNODE_IS_FREE(next) &&
	            next->preceding == nodesize);

	if (precedingsize > 0) {
		/* Return the space before the chunk to the free list */

		newnode = (FAR struct mm_allocnode_s *)
		  ((FAR char *)node + precedingsize);
		node->size = precedingsize | (node->size & MM_MASK_BIT);
		newnode->size = (nodesize - precedingsize) | MM_ALLOC_BIT |
		                MM_PREVFREE_BIT;
		newnode->preceding = precedingsize;
		mm_addfreechunk(heap, node);
	} else {
		newnode = (FAR struct mm_allocnode_s *)node;
		newnode->size |= MM_ALLOC_BIT;
	}

	next->size &= ~MM_PREVFREE_BIT;

	/* Return the space after the chunk to the free list */

	if (MM_SIZEOF_NODE(newnode) > size) {
		mm_shrinkchunk(heap, newnode, size);
	}

	/* Update heap statistics */

	nodesize = MM_SIZEOF_NODE(newnode);
	heap->mm_curused += nodesize;
	if (heap->mm_curused > heap->mm_maxused) {
		heap->mm_maxused = heap->mm_curused;
	}

	sched_note_heap(NOTE_HEAP_ALLOC, heap, (FAR void *)alignedchunk,
	                nodesize, heap->mm_curused);

	mm_unlock(heap);

	MM_ADD_BACKTRACE(heap, newnode);

	ret = kasan_unpoison((FAR void *)alignedchunk,
	                     nodesize - MM_ALLOCNODE_OVERHEAD);
#ifdef CONFIG_MM_FILL_ALLOCATIONS
	memset(ret, MM_ALLOC_MAGIC, size - MM_ALLOCNODE_OVERHEAD);
#endif

	DEBUGASSERT(((uintptr_t)ret & mask) == 0);
	return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mm_memalign
 *
 * Description:
 *   memalign first looks for a free node that can hold the aligned chunk.
 *   If there is none, it requests more than enough space from malloc, finds
 *   a region within that chunk that meets the alignment request and then
 *   frees any leading or trailing space.
 *
 *   The alignment argument must be a power of two. 16-byte alignment is
 *   guaranteed by normal malloc calls.
 *
 ****************************************************************************/

FAR void *mm_memalign(FAR struct mm_heap_s *heap, size_t alignment,
                      size_t size)
{
	FAR struct mm_allocnode_s *node;
	uintptr_t rawchunk;
	uintptr_t alignedchunk;
	size_t mask;
	size_t allocsize;
	size_t newsize;
	size_t chunksize;
	FAR void *ret;

	/* Make sure that alignment is less than half max size_t */

	if (alignment >= (SIZE_MAX / 2)) {
		return NULL;
	}

	/* Make sure that alignment is a power of 2 */

	if ((alignment & -alignment) != alignment) {
		return NULL;
	}

#ifdef CONFIG_MM_HEAP_MEMPOOL
	if (heap->mm_mpool) {
		node = mempool_multiple_memalign(heap->mm_mpool, alignment, size);
		if (node != NULL) {
			return node;
		}
	}
#endif

	/* If this requested alinement's less than or equal to the natural
	 * alignment of malloc, then just let malloc do the work.
	 */

	if (alignment <= MM_ALIGN) {
		FAR void *ptr = mm_malloc(heap, size);
		DEBUGASSERT(ptr == NULL || ((uintptr_t)ptr) % alignment == 0);
		return ptr;
	} else if (alignment < MM_MIN_CHUNK) {
		alignment = MM_MIN_CHUNK;
	}

	mask = alignment - 1;

	/* Adjust the size to account for (1) the size of the allocated node and
	 * (2) to make sure that it is aligned with MM_ALIGN and its size is at
	 * least MM_MIN_CHUNK.
	 *
	 * Notice that we increase the allocation size by twice the requested
	 * alignment.  We do this so that there will be at least two valid
	 * alignment points within the allocated memory.
	 *
	 * NOTE:  These are sizes given to malloc and not chunk sizes. They do
	 * not include MM_SIZEOF_ALLOCNODE.
	 */

	if (size < MM_MIN_CHUNK - MM_ALLOCNODE_OVERHEAD) {
		size = MM_MIN_CHUNK - MM_ALLOCNODE_OVERHEAD;
	}

	newsize = MM_ALIGN_UP(size);         /* Make multiples of our granule size */
	allocsize = newsize + 2 * alignment; /* Add double full alignment size */

	if (newsize < size || allocsize < newsize) {
		/* Integer overflow */

		return NULL;
	}

	/* First try to take the aligned chunk directly from a free node.  The
	 * chunk size includes MM_ALLOCNODE_OVERHEAD.
	 */

	chunksize = MM_ALIGN_UP(size + MM_ALLOCNODE_OVERHEAD);
	if (chunksize > size) {
		ret = memalign_from_freelist(heap, alignment, chunksize);
		if (ret != NULL) {
			return ret;
		}
	}

	/* Otherwise malloc more than enough space */

	rawchunk = (uintptr_t)mm_malloc(heap, allocsize);
	if (rawchunk == 0) {
		return NULL;
	}

	kasan_poison((FAR void *)rawchunk,
	             mm_malloc_size(heap, (FAR void *)rawchunk));

	rawchunk = (uintptr_t)kasan_clear_tag((FAR void *)rawchunk);

	/* We need to hold the MM mutex while we muck with the chunks and
	 * nodelist.
	 */

	DEBUGVERIFY(mm_lock(heap));

	/* Get the node associated with the allocation and the next node after
	 * the allocation.
	 */

	node = (FAR struct mm_allocnode_s *)(rawchunk - MM_SIZEOF_ALLOCNODE);
	heap->mm_curused -= MM_SIZEOF_NODE(node);

	/* Find the aligned subregion */

	alignedchunk = (rawchunk + mask) & ~mask;

	/* Check if there is free space at the beginning of the aligned chunk */

	if (alignedchunk != rawchunk) {
		FAR struct mm_allocnode_s *newnode;
		FAR struct mm_allocnode_s *next;
		size_t precedingsize;
		size_t newnodesize;

		/* Get the node the next node after the allocation. */

		next = (FAR struct mm_allocnode_s *)
		  ((FAR char *)node + MM_SIZEOF_NODE(node));

		newnode = (FAR struct mm_allocnode_s *)
		  (alignedchunk - MM_SIZEOF_ALLOCNODE);

		/* Preceding size is full size of the new 'node,' including
		 * MM_SIZEOF_ALLOCNODE
		 */

		precedingsize = (uintptr_t)newnode - (uintptr_t)node;

		/* If we were unlucky, then the alignedchunk can lie in such a position
		 * that precedingsize < SIZEOF_NODE_FREENODE.  We can't let that happen
		 * because we are going to cast 'node' to struct mm_freenode_s below.
		 * This is why we allocated memory large enough to support two
		 * alignment points.  In this case, we will simply use the second
		 * alignment point.
		 */

		if (precedingsize < MM_MIN_CHUNK) {
			alignedchunk += alignment;
			newnode       = (FAR struct mm_allocnode_s *)
			                (alignedchunk - MM_SIZEOF_ALLOCNODE);
			precedingsize = (uintptr_t)newnode - (uintptr_t)node;
		}

		/* If the previous node is free, merge node and previous node, then
		 * set up the node size.
		 */

		if (MM_PREVNODE_IS_FREE(node)) {
			FAR struct mm_freenode_s *prev =
			  (FAR struct mm_freenode_s *)((FAR char *)node - node->preceding);

			/* Remove the node.  There must be a predecessor, but there may
			 * not be a successor node.
			 */

			DEBUGASSERT(prev->blink);
			prev->blink->flink = prev->flink;
			if (prev->flink) {
				prev->flink->blink = prev->blink;
			}

			precedingsize += MM_SIZEOF_NODE(prev);
			node = (FAR struct mm_allocnode_s *)prev;
		}

		node->size = precedingsize;

		/* Set up the size of the new node */

		newnodesize = (uintptr_t)next - (uintptr_t)newnode;
		newnode->size = newnodesize | MM_ALLOC_BIT | MM_PREVFREE_BIT;
		newnode->preceding = precedingsize;

		/* Clear the previous free bit of the next node */

		next->size &= ~MM_PREVFREE_BIT;

		/* Convert the newnode chunk size back into malloc-compatible size by
		 * subtracting the header size MM_ALLOCNODE_OVERHEAD.
		 */

		allocsize = newnodesize - MM_ALLOCNODE_OVERHEAD;

		/* Add the original, newly freed node to the free nodelist */

		mm_addfreechunk(heap, (FAR struct mm_freenode_s *)node);

		/* Replace the original node with the newlay realloaced,
		 * aligned node
		 */

		node = newnode;
	}

	/* Check if there is free space at the end of the aligned chunk. Convert
	 * malloc-compatible chunk size to include MM_ALLOCNODE_OVERHEAD as needed
	 * for mm_shrinkchunk.
	 */

	size = MM_ALIGN_UP(size + MM_ALLOCNODE_OVERHEAD);

	if (allocsize > size) {
		/* Shrink the chunk by that much -- remember, mm_shrinkchunk wants
		 * internal chunk sizes that include MM_ALLOCNODE_OVERHEAD.
		 */

		mm_shrinkchunk(heap, node, size);
	}

	/* Update heap statistics */

	size = MM_SIZEOF_NODE(node);
	heap->mm_curused += size;
	if (heap->mm_curused > heap->mm_maxused) {
		heap->mm_maxused = heap->mm_curused;
	}

	sched_note_heap(NOTE_HEAP_ALLOC, heap, (FAR void *)alignedchunk, size,
	                heap->mm_curused);

	mm_unlock(heap);

	MM_ADD_BACKTRACE(heap, node);

	alignedchunk = (uintptr_t)kasan_unpoison((FAR const void *)alignedchunk,
	                                         size - MM_ALLOCNODE_OVERHEAD);
	DEBUGASSERT(alignedchunk % alignment == 0);
	minfo("Aligned %" PRIxPTR " to %" PRIxPTR ", size %zu\n",
	      rawchunk, alignedchunk, size);
	return (FAR void *)alignedchunk;
}
