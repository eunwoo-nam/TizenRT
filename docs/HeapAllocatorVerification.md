# Heap Allocator Verification

This page records how the heap allocator changes ported from TizenRT master onto the
NuttX 12.13.0 based `os/mm/mm_heap` were verified, and what the verification found.

## Contents
- [Scope](#scope)
- [Host Stress Test](#host-stress-test)
- [Baseline Comparison](#baseline-comparison)
- [Target Build and QEMU Test](#target-build-and-qemu-test)
- [Not Covered](#not-covered)
- [Pre-existing Issues Found](#pre-existing-issues-found)

## Scope

The base is `upstream/nuttx-12.13.0-migration` at `eaad4ca84`, where `os/mm/mm_heap` is the
NuttX 12.13.0 allocator. These five changes were ported from TizenRT master:

| # | Commit title | Kconfig |
|---|--------------|---------|
| 1 | `os/mm/mm_heap: Use memmove when realloc extends into the previous chunk` | always on |
| 2 | `os/mm/mm_heap: Keep a separate descending free list per bin` | always on |
| 3 | `os/mm/mm_heap: Search the free list directly in memalign` | always on |
| 4 | `os/mm: Add an option to allocate large chunks from the back` | `CONFIG_MM_ALLOC_LARGE_FROM_BACK` (default n), `CONFIG_MM_LARGE_ALLOC_THRESHOLD` (default 1048576) |
| 5 | `os/mm: Add an option to disable realloc neighbor extension` | `CONFIG_REALLOC_DISABLE_NEIGHBOR_EXTENSION` (default n) |

Not ported on purpose:
- `realloc(p, 0)` keeps the NuttX 12.13.0 behavior, which matches Linux.
- Double free detection is handled separately, together with use-after-free detection.

## Host Stress Test

### Method

The allocator sources (`mm_initialize.c`, `mm_malloc.c`, `mm_free.c`, `mm_realloc.c`,
`mm_memalign.c`, `mm_shrinkchunk.c`, `mm_foreach.c`, `mm_mallinfo.c`, `mm_checkcorruption.c`,
`mm_malloc_size.c`, `mm_heapmember.c`, `mm_memdump.c`) are built unmodified with host gcc
(x86_64, `-std=gnu11 -O1 -g -Wall -Wextra`) against small shim headers:
- no-op `mm_lock()`/`mm_unlock()`, no-op `sched_note_heap()`
- `CONFIG_MM_BACKTRACE=-1`, `CONFIG_MM_NODE_GUARDSIZE=0`, `CONFIG_MM_FREE_DELAYCOUNT_MAX=0`
- no mempool (`CONFIG_MM_HEAP_MEMPOOL_THRESHOLD=-1`), `CONFIG_MM_REGIONS=1`
- `CONFIG_DEBUG_MM=y`, `CONFIG_DEBUG_ASSERTIONS=y`

The test harness is not part of the source tree.

The test drives one 24 MB heap with 384 slots and a xorshift random generator:
- An empty slot gets `malloc` (70%) or `memalign` (30%).
- A used slot gets `free` (40%) or `realloc` (60%).
- Sizes: 60% 1 B to 256 B, 30% 257 B to 8 KB, 8.5% 8 KB to 264 KB, 1.2% 256 KB to 2.25 MB,
  0.3% 2 MB to 6 MB. The last bin (4 MB and up) is used.
- `memalign` alignment: 8 B to 4 KB (95%), 8 KB to 64 KB (5%).

After every operation the test checks:
- Every bin list: head size is 0, `blink` links are consistent, every node is in the bin that
  `mm_size2ndx()` gives, the bin is in descending order, and the listed nodes are exactly the
  free nodes found by `mm_foreach()`.
- `mm_checkcorruption()` passes.
- `mm_mallinfo()`: `fordblks`, `mxordblk` and `ordblks` match the heap walk, and `uordblks`
  and `aordblks` match the live slots.
- `mm_heapfree_largest()` equals the largest free node.

Each operation also checks:
- `malloc`: the result is aligned, and the chosen node is the smallest free node that fits
  (best fit). A failure is allowed only if no free node fits.
- `memalign`: the result is aligned and it fails only if no free node fits. For alignment above
  `MM_ALIGN`, the node is less than `MM_MIN_CHUNK` larger than needed and is cut from the
  smallest free node that fits.
- `realloc`: the old data is kept. With `CONFIG_REALLOC_DISABLE_NEIGHBOR_EXTENSION`, growth
  always moves to a chunk that does not overlap the old one.
- With `CONFIG_MM_ALLOC_LARGE_FROM_BACK`, a block at or above the threshold that leaves at
  least `MM_MIN_CHUNK` free ends where the free node it was cut from ended. Any other `malloc`
  block starts where the free node started.
- The data pattern of a slot is checked when it is freed or reallocated, and the patterns of
  all live slots are checked every 5000 operations.

Two fixed scenarios run before the random phase:
- Front extension: free the chunk in front of a block, then grow the block with `realloc` so
  that the data moves forward into an overlapping range.
- `memalign` hole: leave a single free hole of 3 x 4096 bytes and call `memalign(4096, 8192)`.
  The aligned chunk fits in the hole, but `size + 2 * alignment` does not.

### Variants

| Name | Extra flags |
|------|-------------|
| default | none |
| back4k | `CONFIG_MM_ALLOC_LARGE_FROM_BACK`, `CONFIG_MM_LARGE_ALLOC_THRESHOLD=4096` |
| back64 | `CONFIG_MM_ALLOC_LARGE_FROM_BACK`, `CONFIG_MM_LARGE_ALLOC_THRESHOLD=64` |
| nonb | `CONFIG_REALLOC_DISABLE_NEIGHBOR_EXTENSION` |
| all16 | `-O2`, `CONFIG_MM_DEFAULT_ALIGNMENT=16`, all three options above (threshold 4096) |
| asan | `-fsanitize=address,undefined` |
| all_asan | `-fsanitize=address,undefined`, all three options above (threshold 4096) |

### Results

All 30 runs on the final tree completed with no failed check. One line per variant is shown
below; the other seeds give similar numbers.

```
f_default seed=101: ops=300000 malloc=60087(fail 72) memalign=25872(fail 26, spurious 0) realloc=128451(fail 73, inplace 72677, front 11118, moved 44583) free=85861 back=0 largest_mismatch=0 lastbin_snap=228783
f_back4k seed=101: ops=300000 malloc=60278(fail 98) memalign=25869(fail 33, spurious 0) realloc=128106(fail 110, inplace 73334, front 11720, moved 42942) free=86016 back=14733 largest_mismatch=0 lastbin_snap=210195
f_back64 seed=101: ops=300000 malloc=60224(fail 80) memalign=26092(fail 45, spurious 0) realloc=127762(fail 98, inplace 73915, front 11508, moved 42241) free=86191 back=24316 largest_mismatch=0 lastbin_snap=230139
f_nonb seed=101: ops=300000 malloc=60378(fail 50) memalign=25848(fail 24, spurious 0) realloc=127883(fail 100, inplace 58529, front 0, moved 69254) free=86152 back=0 largest_mismatch=0 lastbin_snap=262787
f_all16 seed=101: ops=300000 malloc=60070(fail 42) memalign=26045(fail 22, spurious 0) realloc=128106(fail 111, inplace 59339, front 0, moved 68656) free=86051 back=14880 largest_mismatch=0 lastbin_snap=280246
f_asan seed=106: ops=30000 malloc=6087(fail 5) memalign=2627(fail 2, spurious 0) realloc=12839(fail 8, inplace 7264, front 1125, moved 4442) free=8707 back=0 largest_mismatch=0 lastbin_snap=25551
f_all_asan seed=106: ops=30000 malloc=6030(fail 2) memalign=2648(fail 1, spurious 0) realloc=12928(fail 10, inplace 5951, front 0, moved 6967) free=8675 back=1456 largest_mismatch=0 lastbin_snap=27200
```

| Variant | Seeds | Operations per run | Failed checks |
|---------|-------|--------------------|---------------|
| default, back4k, back64, nonb, all16 | 101 to 105 | 300000 | 0 / 25 runs |
| asan, all_asan | 106, 107 | 30000 | 0 / 4 runs, no sanitizer report |
| baseline (`eaad4ca84`) | 1 | 50000 | see [Baseline Comparison](#baseline-comparison) |

Counter meanings:
- `fail`: requests that returned NULL. Each was checked to have no fitting free node.
- `spurious`: `memalign` failures while a fitting free node existed. This is 0 in every run.
- `inplace`, `front`, `moved`: `realloc` that stayed in place, extended into the previous chunk,
  or moved to a new chunk. `front` is 0 with `CONFIG_REALLOC_DISABLE_NEIGHBOR_EXTENSION`.
- `back`: allocations placed at the end of a free node by `CONFIG_MM_ALLOC_LARGE_FROM_BACK`.
- `largest_mismatch`: checks where `mm_heapfree_largest()` was not the largest free node.
- `lastbin_snap`: free nodes of 4 MB or more seen in the heap walks, so the last bin is used.

Each commit was also tested on its own with seed 201 and 100000 operations. Commits 1 and 2
still have the old `memalign`, so its best fit check was off for them:

```
os/mm/mm_heap: Use memmove when realloc extends into the previous chunk [-DOLD_LISTS -DOLD_MEMALIGN]: ops=100000 malloc=20258(fail 31) memalign=8589(fail 14, spurious 0) realloc=42643(fail 26, inplace 24353, front 4163, moved 14101) free=28802 back=0 largest_mismatch=62171 lastbin_snap=72962
os/mm/mm_heap: Keep a separate descending free list per bin [-DOLD_MEMALIGN]: ops=100000 malloc=20191(fail 28) memalign=8614(fail 7, spurious 0) realloc=42707(fail 31, inplace 24221, front 4189, moved 14266) free=28770 back=0 largest_mismatch=0 lastbin_snap=84975
os/mm/mm_heap: Search the free list directly in memalign []: ops=100000 malloc=20251(fail 24) memalign=8592(fail 17, spurious 0) realloc=42635(fail 22, inplace 24122, front 3663, moved 14828) free=28802 back=0 largest_mismatch=0 lastbin_snap=85034
os/mm: Add an option to allocate large chunks from the back [-DCONFIG_MM_ALLOC_LARGE_FROM_BACK -DCONFIG_MM_LARGE_ALLOC_THRESHOLD=4096]: ops=100000 malloc=20287(fail 30) memalign=8618(fail 16, spurious 0) realloc=42508(fail 34, inplace 24242, front 4013, moved 14219) free=28859 back=4996 largest_mismatch=0 lastbin_snap=59718
```

## Baseline Comparison

The same test was run on the base `eaad4ca84`. The bin list check was changed for the chained,
ascending lists, `mm_heapfree_largest()` mismatches were counted instead of failing, and the
`memalign` best fit check was off. Three differences show up:

**1. realloc copies overlapping memory with memcpy.** With ASan, the front extension scenario
stops in `mm_realloc()`:

```
==3318197==ERROR: AddressSanitizer: memcpy-param-overlap: memory ranges [0x5fa71c30dd18,0x5fa71c30e100) and [0x5fa71c30df10, 0x5fa71c30e2f8) overlap
    #0 0x726d6defb16d in memcpy ../../../../src/libsanitizer/sanitizer_common/sanitizer_common_interceptors_memintrinsics.inc:115
    #1 0x5fa71c1d0f9e in memcpy /usr/include/x86_64-linux-gnu/bits/string_fortified.h:29
    #2 0x5fa71c1d0f9e in mm_realloc base/os/mm/mm_heap/mm_realloc.c:387
```

Line 387 is `memcpy(newmem, oldmem, oldsize - MM_ALLOCNODE_OVERHEAD);`. Commit 1 changes it to
`memmove`. On the final tree the same scenario passes under ASan (the asan and all_asan runs).

**2. `mm_heapfree_largest()` is often wrong.** It walks the chained list backward from the
last bin head, so it misses free nodes in the last bin. In 50000 operations it was wrong in
34259 checks:

```
base seed=1: ops=50000 malloc=10162(fail 13) memalign=4270(fail 7, spurious 0) realloc=21434(fail 16, inplace 12189, front 1986, moved 7243) free=14412 back=0 largest_mismatch=34259 lastbin_snap=42397
```

With commit 2 it returns the first (largest) node of the highest non-empty bin, and
`largest_mismatch` is 0 in every run.

**3. memalign fails when the heap has a fitting hole.** In the `memalign` hole scenario the
base returns NULL because it asks `malloc()` for `size + 2 * alignment`. With commit 3 it
returns a 4096-byte aligned block from the hole.

## Target Build and QEMU Test

The targets were built with `./tools/configure.sh` and `./dbuild.sh`
(docker image `tizenrt/tizenrt:2.0.1-qemu`) and run with `build/configs/<board>/qemu_run.sh`.
The builds had no warnings in `os/mm`.

| Target | Build | Mode | Options | Result |
|--------|-------|------|---------|--------|
| `qemu-armv7a/ostest` | Cortex-A7, SMP 2 CPUs | flat | defconfig | ostest exits with status 0, no assertion |
| `mps2-an521/ostest` | Cortex-M33 | flat | defconfig | ostest exits with status 0, no assertion |
| `qemu-armv7a/tash` | Cortex-A7, SMP 2 CPUs | flat | see below | `ostest` from TASH exits with status 0, no assertion |

Options changed for the `qemu-armv7a/tash` run (not committed):

```
CONFIG_MM_ALLOC_LARGE_FROM_BACK=y
CONFIG_MM_LARGE_ALLOC_THRESHOLD=4096
CONFIG_REALLOC_DISABLE_NEIGHBOR_EXTENSION=y
CONFIG_DEBUG_ASSERTIONS=y
CONFIG_DEBUG_MM=y
```

`CONFIG_DEBUG_MM=y` builds `mm_checkcorruption.c`.

End of the `qemu-armv7a/ostest` log:

```
Final memory usage:
VARIABLE  BEFORE   AFTER
======== ======== ========
arena      ff0000   ff0000
ordblks         2        8
mxordblk   fe62a0   fe4290
uordblks     9120     a3b0
fordblks   fe6ee0   fe5c50
user_main: Exiting
ostest_main: Exiting with status 0
```

End of the `mps2-an521/ostest` log. This board does not power off, so the runner stops the
container after `ostest_main` exits:

```
Final memory usage:
VARIABLE  BEFORE   AFTER
======== ======== ========
arena      3fc5b8   3fc5b8
ordblks         2        6
mxordblk   3f2918   3f2918
uordblks     9058     91c0
fordblks   3f3560   3f33f8
user_main: Exiting
ostest_main: Exiting with status 0
```

End of the `qemu-armv7a/tash` log with the options above:

```
Final memory usage:
VARIABLE  BEFORE   AFTER
======== ======== ========
arena      ff0000   ff0000
ordblks         4        7
mxordblk   fe29c0   fe2970
uordblks     c2c0     c460
fordblks   fe3d40   fe3ba0
user_main: Exiting
ostest_main: Exiting with status 0
```

## Not Covered

- Real boards. The rtl8730e defconfigs `loadable_ext_ddr`, `loadable_ext_psram`,
  `loadable_apps` and `loadable_ext_ddr_st7785` already set `CONFIG_MM_ALLOC_LARGE_FROM_BACK=y`,
  so the option is active again on them, but they were not built or run.
- Protected and loadable builds. Only flat builds were run.
- `CONFIG_MM_KASAN`, the heap mempool, `CONFIG_MM_BACKTRACE >= 0`,
  `CONFIG_MM_FREE_DELAYCOUNT_MAX > 0`, `CONFIG_MM_NODE_GUARDSIZE > 0`, multiple regions and
  `CONFIG_MM_SMALL`.
- A 32-bit host build. The 32-bit code paths were run only on QEMU.
- Concurrency. The host test is single-threaded; SMP was covered only by ostest on
  `qemu-armv7a`.
- `docs/TinyAra_Config_Variables.html` does not list the new options.

## Pre-existing Issues Found

These issues are in the base and were not changed by this work.

**realloc shrink leaves `mm_curused` too low.** When `realloc()` shrinks a block,
`mm_realloc()` subtracts `oldsize - newsize` from `heap->mm_curused` before it calls
`mm_shrinkchunk()`. If the next node is allocated and the tail is smaller than `MM_MIN_CHUNK`,
`mm_shrinkchunk()` does not split the node, so the node keeps its old size. `mm_free()` later
subtracts the full node size, so `mm_curused` drifts low and `mm_heapfree()` reports more free
memory than the heap has. NuttX master has the same code. The host test checks `mm_heapfree()`
only when it is built with `-DCHECK_HEAPFREE`, and that check fails on both the base and the
new code.

**`tc_umm_heap` does not build on this branch.** With `CONFIG_TC_KERNEL_UMM_HEAP=y`, the
`qemu-armv7a/tash` build first stops at the kernel test cases:

```
le_tc/kernel/kernel_tc_main.c:30:10: fatal error: tc_common.h: No such file or directory
```

`build/configs/qemu-armv7a/Make.defs` has no `-I.` in its include flags, while the rtl8730e
`Make.defs` has `ARCHINCLUDES = -I. ...`. With `make 'EXTRAFLAGS=-iquote .'` added to get past
this, the build stops in `le_tc/kernel/tc_umm_heap.c` with 12 errors, for example:

```
le_tc/kernel/tc_umm_heap.c:119:8: error: too many arguments to function 'mallinfo'
le_tc/kernel/tc_umm_heap.c:46:56: error: 'SIZEOF_MM_ALLOCNODE' undeclared (first use in this function)
```

The test still calls the old `mallinfo(&info)` form, while NuttX 12.13.0 has
`struct mallinfo mallinfo(void)`. It also uses allocator internals (`SIZEOF_MM_ALLOCNODE`,
`MM_ALIGN_UP`) that are no longer in a public header.
