# HANDOFF: NuttX 12.13.0 → TizenRT migration (QEMU armv7-a / armv8-m)

Branch: `claude/nuttx-tizenrt-migration-nqry9a` (TizenRT, NuttX, similarity repos).
Only the TizenRT repository gets commits. Never push to `master`.

This file is updated after every step (work commit → HANDOFF commit).

## Goal

Bring NuttX 12.13.0 (`/home/user/nuttx`, HEAD b9b5d9afbf5a = tag
`nuttx-12.13.0`) into TizenRT and keep the TizenRT build system.
Done means: `qemu-armv7a` (Cortex-A7, QEMU virt, PL011) and `mps2-an521`
(Cortex-M33, CMSDK UART) boot on QEMU and pass ostest. FLAT build only.

## Decisions (agreed with the user)

| Topic | Decision |
|---|---|
| armv8-m board | mps2-an521 |
| Formatter | `tools/cformatter.py` of TizenRT branch `agent/261022_improve-cformatter-tool`, used from a separate worktree and not merged. The fix 3463f629 (comments continued from a directive line) was pushed to that branch with the user's approval. |
| libs | NuttX layout `os/libs/{libc,libm,libxx,libbuiltin,libdsp,libnx}`. `lib/` is gone. |
| Rename | Only include paths: `#include <nuttx/..>` → `<tinyara/..>`, and makefile/Kconfig paths `sched` → `kernel`, `boards` → `board`. Header guards, `__NUTTX__` and comments stay. |
| Folders kept | `os/kernel` (= NuttX sched), `os/include/tinyara` (= include/nuttx) |
| Not synced | net (everything network: net/, drivers/net, drivers/wireless, include/{net,netinet,arpa,netpacket}, sys/socket.h, sys/sockio.h, sys/un.h, netdb.h, ifaddrs.h, nuttx/net, nuttx/usrsock, libc/net, libc/netdb, fs/socket), pm (drivers/power/pm, pm headers), wireless/, fs/nfs, crypto/, cmake/, openamp (openamp/, drivers/rpmsg, rptun, fs/rpmsgfs and every `*rpmsg*`/`*rptun*` file), tools/, dummy/, pass1/ |
| Drivers | Same-name dirs and dirs at other TizenRT paths are synced (moved to the NuttX layout first). Drivers TizenRT does not have are not added, except `note` and what QEMU needs (serial/pl011, serial_cmsdk, timers). |
| Old files | TizenRT files of NuttX origin that 12.13.0 dropped/renamed are deleted. Samsung files are kept and disabled by config. Headers/files of not-synced areas (bluetooth, pm, mipidsi, ...) are kept. |
| Samsung hooks | Overwritten (task manager, binary manager, heapinfo, ...). |
| Logging | NuttX code uses NuttX macros; `os/include/debug.h` maps the old TizenRT macros onto them (fac1eff5). |
| wqueue, fs/mtd | NuttX layout (`os/kernel/wqueue` + `os/libs/libc/wqueue`, `os/drivers/mtd`, `os/fs/driver`). |
| Docker | `tools/docker/Dockerfile`: `FROM tizenrt/tizenrt:2.0.1` + static qemu-system-arm 8.2, tag `tizenrt/tizenrt:2.0.1-qemu`, selected by `CONFIG_DOCKER_VERSION="2.0.1-qemu"`. gcc 10.3.1 of the base image. |
| ostest | Two configs per board: `ostest` (entry point `ostest_main`, poweroff) and `tash` (TASH + `ostest` command). Sync apps/examples/testcase/ostest with nuttx-apps 12.13.0. |
| Host tools | Keep TizenRT `os/tools`; change only what the build needs. |
| Sync scripts | Not committed (user decision). See "Tools" below to recreate them. |

Commit rules: author/committer `eunwoo-nam <eunwoo.nam@samsung.com>`, last
line `Co-Authored-By: Claude Opus 5.5` (no e-mail, no session link), push
after every commit, one commit per NuttX top-level directory (libs: per
library, drivers: per group).

## Done (TizenRT commits, oldest first)

| Commit | What |
|---|---|
| a284ab61 | tools/docker: build image with QEMU |
| e967f697 | os/include sync |
| fac1eff5 | debug.h: TizenRT macros → NuttX macros |
| 05aac056 | move lib/libc, lib/libxx, math, binfmt/libelf, libbuiltin, os/wqueue to NuttX layout |
| 321873aa d3dea6bb e968b113 c1009927 1752c7e9 ac0ffe94 | os/libs/{libc,libm,libxx,libbuiltin,libdsp,libnx} |
| df041e48 | os/kernel (sched, incl. wqueue) |
| 8cd029e9 | os/mm |
| 7bd79912 | move fs/driver/block, fs/driver/mtd to NuttX layout |
| e6069c5a | os/fs |
| 1f7c44eb | move driver root files to NuttX subdirs |
| 398c1cea 6431efc9 c6e4ff5e d76e990d 2f730c83 cd97210f 16d7b9cf 7df82b2e | drivers: core, mtd, serial+timers, bus, media, usb, power, dummy |
| 5886fcb6 a26e5be4 6bbb2883 39bce27f e0c988ed | binfmt, audio, syscall, graphics, video |
| 706971a3 | os/arch/arm: common, armv7-a, armv8-m, qemu, mps |
| c80d1840 8e5f6d3e | os/board: boardctl.c move, qemu-armv7a + mps2-an521 + Board.mk/Makefile |
| 422cae36 | Kconfig integration (os/Kconfig, Kconfig.debug, arch, arm, board, adapted NuttX Kconfigs, apps_entry) |

Every sync commit was verified with tizenrt-nuttx-similarity core: all
copied C files are token-identical to NuttX 12.13.0 (renames
tinyara=nuttx), other files byte-identical after the path rename.

## In progress

Build system integration (Phase 3), not committed yet:
- `os/tools/Config.mk` ← NuttX 12.13.0 Config.mk with TizenRT changes
  (BOARD_DIR = os/board/<board>, no board common dir, drivers/dummy as
  platform dir, fixed GCC prefixes `-D`/`-I`/`-isystem `, flock in ARCHIVE,
  MOVEOBJ).
- `os/tools/mkconfig.c`: NuttX 12.13.0 sanity section (CONFIG_y/m,
  RAM_END/RAM_VEND/FLASH_END with #ifndef) + TizenRT board block
  (crash dump, FLASH_START_ADDR, KMM_REGIONS, FLASH_END_ADDR, MPU guard).
  The old NuttX-7.x derived macros are removed.
- `os/Makefile.unix`: LIB_DIR=libs, headers copied from
  include/tinyara/lib (math/float/stdarg/setjmp/stdbit), drivers/platform
  link, BINDIR for kconfig.
- `os/Directories.mk`, `FlatLibs.mk`, `LibTargets.mk`: libsched (kernel),
  libboards, libm, libbuiltin, libxx, libnx, libgraphics, libvideo,
  libdsp; libwque removed; binfmt always built.
- `os/arch/arm/src/Makefile` (NuttX) adapted: includes $(TOPDIR)/Make.defs,
  links $(OUTBIN_DIR)/tinyara, libraries from $(LIBRARIES_DIR),
  System.map in $(OUTBIN_DIR).
- `os/drivers/Makefile`, `os/drivers/power/Make.defs`, `os/fs/Makefile`:
  drop includes of not-synced dirs, add TizenRT-only drivers.
- `build/configs/qemu-armv7a/{Make.defs,scripts/dramboot.ld,ostest/defconfig}`.

## Next steps

1. Commit the build system step, then build `qemu-armv7a/ostest` in docker:
   `cd os && ./tools/configure.sh qemu-armv7a/ostest && make` (inside
   `tizenrt/tizenrt:2.0.1-qemu`) and fix the errors per area.
2. Run QEMU:
   `qemu-system-arm -cpu cortex-a7 -nographic -machine virt,virtualization=off,gic-version=2 -net none -chardev stdio,id=con,mux=on -serial chardev:con -mon chardev=con,mode=readline -kernel build/output/bin/tinyara`
   Pass: `ostest_main: Exiting with status 0`.
3. Sync ostest with nuttx-apps 12.13.0 (`testing/ostest`), keep
   `ostest_main` and the TASH registration. Add the `tash` config.
4. mps2-an521: build/configs/mps2-an521 (Make.defs, flash.ld, ostest,
   tash), run with `qemu-system-arm -M mps2-an521 -nographic ... -kernel`.
   QEMU 8.2 → default CMSDK IRQs (swap only for QEMU >= 9.2).
5. Docs: build/configs/<board>/README.md, docs/NuttX-12.13.0-Migration.md.
6. Final codesim report over all synced files.

## Environment (recreate after a new session)

```bash
S=<scratchpad>
# docker daemon (not running by default)
dockerd > $S/dockerd.log 2>&1 &
# Docker Hub rate-limits anonymous pulls: use the GCR mirror
docker pull mirror.gcr.io/tizenrt/tizenrt:2.0.1 && docker tag mirror.gcr.io/tizenrt/tizenrt:2.0.1 tizenrt/tizenrt:2.0.1
docker pull mirror.gcr.io/library/ubuntu:24.04 && docker tag mirror.gcr.io/library/ubuntu:24.04 ubuntu:24.04
docker build --network host -t tizenrt/tizenrt:2.0.1-qemu tools/docker
# references
git clone --depth 1 -b nuttx-12.13.0 https://github.com/apache/nuttx-apps $S/ref/apps-12.13.0
git clone --depth 1 -b nuttx-7.8 https://github.com/apache/nuttx $S/ref/nuttx-7.8
git clone --filter=blob:none --no-checkout https://github.com/apache/nuttx $S/ref/nuttx-hist
git -C $S/ref/nuttx-hist log --name-only --format= HEAD | sort -u > $S/ref/nuttx_all_paths.txt
# formatter
git fetch origin agent/261022_improve-cformatter-tool
git worktree add --detach $S/cformatter FETCH_HEAD
```

## Tools (scratchpad, not committed)

- `nxsync.py sync|verify|stale UNIT`: units = the mapping table above.
  For each file: copy, rename `#include <nuttx/` → `<tinyara/` (C/asm/ld),
  rename `$(TOPDIR)/sched` → `kernel`, `$(TOPDIR)/boards` → `board`,
  `source "sched/` → `kernel/` (build files), keep symlinks, skip
  CMakeLists/*.cmake and `*rpmsg*`/`*rptun*`, run cformatter on .c/.h,
  then verify with `codesim.engine.tokens_identical(a, b,
  renames=[(tinyara,nuttx),(TinyAra,NuttX),(TINYARA,NUTTX),(Tinyara,Nuttx)])`.
  `stale` lists TizenRT files under the target dirs without a 12.13.0
  peer: nuttx-origin (mapped path in the NuttX history, or known name +
  NuttX/ASF header) → delete; samsung → keep; ambiguous/non-source → review.
- `adapt.py`: drops Kconfig `source` / makefile `include` lines of
  not-synced dirs in drivers, drivers/power, fs, libs/libc, adds the
  TizenRT-only driver hooks (from the old drivers/Kconfig and Makefile).
- `kmerge.py OLD_KCONFIG KNOWN_SYMBOLS`: prints the entries of an old
  Kconfig that define symbols not in KNOWN_SYMBOLS (used for the TizenRT
  sections of os/Kconfig, Kconfig.debug, arch, arm, board Kconfig).
- `gen_dbgcompat.py`: generated the TizenRT macro block of debug.h.
- `cpush.sh`: commit with the user's identity and push the branch.
