############################################################################
# drivers/Makefile
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(TOPDIR)/Make.defs

CSRCS = drivers_initialize.c

# Include support for various drivers.  Each Make.defs file will add its
# files to the source file list, add its DEPPATH info, and will add
# the appropriate paths to the VPATH variable

include analog/Make.defs
include audio/Make.defs
include bch/Make.defs
include crypto/Make.defs
include i2c/Make.defs
include i2s/Make.defs
include input/Make.defs
include ioexpander/Make.defs
include lcd/Make.defs
include loop/Make.defs
include misc/Make.defs
include mtd/Make.defs
include net/Make.defs
include note/Make.defs
include pipes/Make.defs
include power/Make.defs
include sensors/Make.defs
include serial/Make.defs
include spi/Make.defs
include syslog/Make.defs
include timers/Make.defs
include usbdev/Make.defs
include usbhost/Make.defs
include video/Make.defs
include wireless/Make.defs

# TizenRT drivers that NuttX does not have

include bluetooth/Make.defs
include compression/Make.defs
include cpuload/Make.defs
include iotdev/Make.defs
include lwnl/Make.defs
include memory/Make.defs
include mipidsi/Make.defs
include os_api_test/Make.defs
include otp/Make.defs
include pm/Make.defs
include prodconfig/Make.defs
include seclink/Make.defs
include silent_reboot/Make.defs
include task_manager/Make.defs
include wifi_csi/Make.defs

ifeq ($(CONFIG_VIRTKEY),y)
  CSRCS += virtkey.c
endif

ifeq ($(CONFIG_SPECIFIC_DRIVERS),y)
-include platform/Make.defs
endif

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS)
OBJS = $(AOBJS) $(COBJS)

BIN = libdrivers$(LIBEXT)

all: $(BIN)
.PHONY: context depend clean distclean

$(AOBJS): %$(OBJEXT): %.S
	$(call ASSEMBLE, $<, $@)

$(COBJS): %$(OBJEXT): %.c
	$(call COMPILE, $<, $@)

$(BIN): $(OBJS)
	$(call ARCHIVE, $@, $(OBJS))

context::

makedepfile: $(CSRCS:.c=.ddc) $(ASRCS:.S=.dds)
	$(call CATFILE, Make.dep, $^)
	$(call DELFILE, $^)

.depend: Makefile $(SRCS) $(TOPDIR)$(DELIM).config
	$(Q) $(MAKE) makedepfile
	$(Q) touch $@

depend: .depend

clean:
	$(call DELFILE, $(BIN))
	$(call CLEAN)

distclean:: clean
	$(call DELFILE, $(CSRCS:.c=.ddc) $(ASRCS:.S=.dds))
	$(call DELFILE, Make.dep)
	$(call DELFILE, .depend)

-include Make.dep
