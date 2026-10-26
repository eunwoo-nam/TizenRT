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
| Formatter | `tools/cformatter.py` of TizenRT branch `agent/261022_improve-cformatter-tool`, used from a separate worktree and not merged. Fixes pushed to that branch: 3463f629 (comments continued from a directive line, approved by the user) and 51e698d9 (exponent sign of numbers: `1e-1` was formatted as `1e - 1`). |
| libs | NuttX layout `os/libs/{libc,libm,libxx,libbuiltin,libdsp,libnx}`. `lib/` is gone. |
| Rename | Only include paths: `#include <nuttx/..>` → `<tinyara/..>`, and makefile/Kconfig paths `sched` → `kernel`, `boards` → `board`. Header guards, `__NUTTX__` and comments stay. |
| Folders kept | `os/kernel` (= NuttX sched), `os/include/tinyara` (= include/nuttx) |
| Not synced | net (everything network: net/, drivers/net, drivers/wireless, include/{net,netinet,arpa,netpacket}, sys/socket.h, sys/sockio.h, sys/un.h, netdb.h, ifaddrs.h, nuttx/net, nuttx/usrsock, libc/net, libc/netdb, fs/socket), pm (drivers/power/pm, pm headers), wireless/, fs/nfs, crypto/, cmake/, openamp (openamp/, drivers/rpmsg, rptun, fs/rpmsgfs and every `*rpmsg*`/`*rptun*` file, except the 5 rpmsg headers below), tools/, dummy/, pass1/ |
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
| 0f89d2cf | re-format the 9 files with exponent numbers after the cformatter fix 51e698d9 |
| 002d1e26 | 5 rpmsg headers synced (rpmsgdev.h, rpmsgblk.h, syslog_rpmsg.h, rpmsg_port.h, fs/rpmsgfs/rpmsgfs.h), their include guards dropped; codesim 3867/20 of 3887; 4 QEMU configs pass |
| 90a584b6 | license ②: Samsung header added to 2548 new NuttX files (2026) and 13 existing NuttX-licensed files (git add year); docs/NuttX-12.13.0-License-Warnings.md; license sections in the migration notes and the report |
| 90ad849c | license ①: Samsung headers of 1122 existing files restored from master (same text/year, 665 same path + 457 moved) |
| ceb8595e | docs/NuttX-12.13.0-Migration-Report.md: final report in Korean (results rebuilt and run from 28d774bd, user decisions, own decisions with reasons, deviations); test group counts corrected to 33/31 (34/32 counted the argv[0] line) |
| 1268b92d | docs/NuttX-12.13.0-Migration.md (mapping, rules, exclusions, intentional differences, known gaps) |
| a989f610 | build/configs/mps2-an521: Make.defs, scripts/flash.ld, ostest, tash, qemu_run.sh, README.md. **Both pass on QEMU** (31 groups; no FPU/prio-inherit as in NuttX nsh; no board_power_off so QEMU keeps running) |
| 98ff0124 | build/configs/qemu-armv7a/tash (+README): TASH via `preapp_start`, `ostest` from TASH passes |
| 52b06bd1 | apps: TASH/FS_CMDS no longer depend on NFILE_DESCRIPTORS (removed in NuttX), fscmd.c guard dropped, local mount point handler type (foreach_mountpoint_t is OS internal now) |
| f4e15eed | build/configs/qemu-armv7a: Make.defs, scripts/dramboot.ld, ostest/defconfig, qemu_run.sh, README.md. **ostest passes on QEMU** (33 groups, `ostest_main: Exiting with status 0`, poweroff) |
| 0d962fae | apps/system: `CONFIG_LIB_HOMEDIR` → `CONFIG_LIBC_HOMEDIR`, system_file.c includes `<tinyara/crc32.h>` |
| 3af7c335 | apps/examples/testcase/ostest ← nuttx-apps 12.13.0 testing/ostest (entry point stays `ostest_main`, TizenRT Make.defs with the nuttx-apps conditions, fork.c/multiuser.c kept but not built) |
| 1612c7b6 | build system: tools/Config.mk, mkdeps/mkconfig/cfgdefine, *.defs, Makefile.unix, Directories/FlatLibs/LibTargets.mk, arch/arm/src/Makefile, drivers/fs makefiles, include guards for not-synced headers, include/queue.h, libc/mm bin dirs removed |

Every sync commit was verified with tizenrt-nuttx-similarity core: all
copied C files are token-identical to NuttX 12.13.0 (renames
tinyara=nuttx), other files byte-identical after the path rename.
The token check ignores whitespace, so it cannot see a number split by
the formatter (`1e - 1`); `ppnum.py` (see Tools) scans for that.

## In progress

- `qemu-armv7a/ostest` is done (f4e15eed); `./dbuild.sh build` uses the
  2.0.1-qemu image.  The defconfig is a full .config made by
  `kc_test.sh` from `seed.config` (nuttx nsh defconfig values + ostest
  entry point + `CONFIG_SCHED_WAITPID=y` for WAITRESULT/POWEROFF + the
  TizenRT apps switched off); copy os/.config to the defconfig.
- qemu-armv7a `tash` is done (98ff0124): entry point `preapp_start`
  (apps/system/init starts TASH and registers the builtin commands; NuttX
  nx_bringup only starts INIT_ENTRYPOINT), `CONFIG_SECURITY_LEVEL_CMDS`
  off.  To change a defconfig: use the committed defconfig as the seed
  of `kc_test.sh` (olddefconfig) and copy os/.config back.
- mps2-an521 is done (a989f610).  Seeds: NuttX nsh defconfig OS values +
  `CONFIG_SCHED_WAITPID=y` + the TizenRT `# ... is not set` lines of the
  qemu-armv7a seed.  CMSDK UART0 TX 48 / RX 49 work with QEMU 8.2 (input
  checked with TASH).
- Final verification (`nxsync.py verify` for every unit): 3858 of 3882
  synced files identical to NuttX 12.13.0 / nuttx-apps; the 24 others are
  the intentional changes listed in docs/NuttX-12.13.0-Migration.md.
  `ppnum.py`: no split exponent numbers.
- Final rerun from 28d774bd (distclean → configure → make → QEMU): all
  four configurations pass (qemu-armv7a 33 groups, mps2-an521 31 groups).
  The final report is docs/NuttX-12.13.0-Migration-Report.md.
- All planned steps are done.  Remaining work is in "Known gaps" of the
  migration doc (other boards, protected/kernel build, moved headers,
  NFILE_DESCRIPTORS users, CONFIG_PM boards).
- Note: the apps preconfig regenerates apps/examples/SoundPlayer/Kconfig
  and Kconfig_ENTRY (pre-existing TizenRT behavior); `git checkout` them.
- Known gap (not in the QEMU builds, rule 3): TizenRT code of other
  boards/drivers still includes headers NuttX 12.13.0 moved or removed:
  `tinyara/i2c.h` (48 files, now `tinyara/i2c/i2c_master.h`),
  `tinyara/fs/mtd.h` (41, now `tinyara/mtd/mtd.h`), `tinyara/time.h` (12),
  `tinyara/math.h` (11), `crc32.h`/`crc16.h`/`crc8.h` (15, now
  `tinyara/crc*.h`), `tinyara/fs/dirent.h` (7), `tinyara/configdata.h` (7),
  `tinyara/float.h` (3).  Only `queue.h` got a compat header (85 files include `<queue.h>`).
  `CONFIG_NFILE_DESCRIPTORS`/`CONFIG_NFILE_STREAMS` (removed in NuttX) are
  still used by le_tc, ftpc, libcoap, lwm2m examples and the vi Kconfig.

## License step (done)

User decisions (license):
- Tool: `tools/license/license_manager.py` of branch
  `agent/261021_license-manager` (12777249), used from a worktree
  ($S/license), not merged.
- Existing TizenRT files follow the license they had on master (Samsung
  header restored with its year). Existing files without a Samsung header
  but with a NuttX license get a 2026 header; Kconfig/.gitignore stay.
- New files: 2026 header with the tool; also for types the tool does not
  support if they have a NuttX license; third-party licensed files too.
  Files without a license: Kconfig stays, the 9 data .h get a header.
- Non-NuttX-Apache files and files without a license are listed as
  warnings in a separate .md.  Never run `fix` (see W4 below).
- Two work commits (① restore, ② add), each followed by a HANDOFF commit.

How the tool handles moved/renamed files: `get_creation_year()` uses
`git log --diff-filter=A` on the current path without `--follow`, so a
moved or renamed file gets the year of the move commit (2026 for the 457
files moved by this migration).  `check` reports such files as
YEAR_MISMATCH and `fix` would rewrite their year.  The clone was shallow;
`git fetch --unshallow --filter=blob:none origin` gives the full history
the tool needs.

State: ① 90ad849c, ② 90a584b6, both pushed.  Results:
- lic.py verify: every file is the previous file + header (bad 0).
- `license_manager.py check --year 2026` on the new files: all OK.
- `license_manager.py check` (no year, full history, 3615 files):
  Missing 0, YEAR_MISMATCH 501 = W4 of the warnings doc (moved 430,
  history rename 25, master mismatch 19, re-added path 27).
- nxsync verify 3858/24 unchanged (build files compared without the
  leading Samsung block), cformatter unchanged, 4 QEMU configs pass
  (33/31 groups); qemu-armv7a/ostest rechecked after the last change.
- User decisions (2nd round): the 11 existing files without a Samsung
  header use the git add year (2023, sched_yield.c 2016); the 27 new
  files on deleted paths keep 2026 (W4).

`$S/nxsync/lic.py classify|restore|add|verify` (not committed): groups
A (restore, 1122), B (existing + NuttX license, 13), C1 (new NuttX
Apache, 2408), C2 (new third-party, 131), C3 (new data .h, 9), skip 280;
report in $S/nxsync/reports/lic.json; LIC_PRE=<commit before the license
commits> (9951064d) when re-run after a commit.  `lic_w4.py` explains the
tool's YEAR_MISMATCH list ($S/lic_mismatch.txt) and `lic_doc.py` writes
the warnings doc.  dockerd dies with the worker: restart it with
`(dockerd > $S/dockerd.log 2>&1 &)` before building.

## rpmsg headers (user decision, after the license step)

The 5 rpmsg headers that synced files included unconditionally are synced
(nxsync unit `rpmsg-headers`): include/nuttx/drivers/rpmsgdev.h,
rpmsgblk.h, include/nuttx/syslog/syslog_rpmsg.h,
include/nuttx/rpmsg/rpmsg_port.h, fs/rpmsgfs/rpmsgfs.h (only CONFIG_
guarded declarations, only synced headers included).  Their include
guards are gone: syslog_initialize.c, qemu_boot.c, qemu_bringup.c and
fs_initialize.c are identical to NuttX again; drivers_initialize.c keeps
the crypto/net/pm/usrsock guards (net/rpmsgdrv.h and
usrsock/usrsock_rpmsg.h need the NuttX net headers, not synced).
codesim: 3867 identical / 20 modified of 3887.  New headers have the
Samsung 2026 header (license_manager.py add --year 2026).

## Review branches (for review only, not for merging)

| Branch | Commit | Content |
|---|---|---|
| `claude/nuttx-tizenrt-migration-nqry9a-review-nodoc` | 739198c3 | the migration history without the documentation commits: 42 work commits replayed on master, 16 HANDOFF/docs commits dropped, the 7 migration documents stripped from the 5 mixed commits (tools/docker, board configs, license ②); author/dates kept |
| `claude/nuttx-tizenrt-migration-nqry9a-review-no-nuttx` | 966fd483 | d8488305 + 1 commit: without the 3886 files copied from NuttX/nuttx-apps; 30007 files left |
| `claude/nuttx-tizenrt-migration-nqry9a-review-changed-only` | fd1d4263 | d46ece4e + 1 commit: only the 3886 files copied from NuttX/nuttx-apps (3879 sync files + mkdeps.c, D/Rust/Swift/Zig.defs, dramboot.ld, flash.ld) |
| `claude/nuttx-tizenrt-migration-nqry9a-review-tizenrt-changes` | 85bb171a | d46ece4e + 1 commit: only the 31 TizenRT files changed/added for NuttX (build system, Kconfig, apps, queue.h, board configs, Dockerfile) |
| `claude/nuttx-tizenrt-migration-nqry9a-review-nuttx-adapted` | b932ee85 | d46ece4e + 1 commit: only the 24 NuttX files adapted for TizenRT (codesim "modified" list) |

Not in any of the last three: the 46 TizenRT files only moved to the
NuttX layout (dhara, libelf_cache.c, libc net/netdb, libm j0..jn, ...).
Recreate: `$S/review/mkbranch.sh <branch> <remove-list>` (temporary index,
no checkout; LEASE=<old sha> for a forced update); lists in $S/review.

## Next steps

None planned.  Possible follow-ups (ask the user first): port another
board, protected build, compat headers or include fixes for the moved
NuttX headers, a `board_power_off()` for mps2-an521.

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
- `ppnum.py [--fix] FILES`: lists (fixes) numbers split at the exponent
  sign (`1e - 1`).
- `build.sh <board>/<cfg> [make args]`: run inside docker
  (`docker run --rm -v /home/user/TizenRT:/root/tizenrt -v $S/build.sh:/root/build.sh tizenrt/tizenrt:2.0.1-qemu bash /root/build.sh qemu-armv7a/ostest -j8`);
  distclean, configure, make.
- `kmerge.py OLD_KCONFIG KNOWN_SYMBOLS`: prints the entries of an old
  Kconfig that define symbols not in KNOWN_SYMBOLS (used for the TizenRT
  sections of os/Kconfig, Kconfig.debug, arch, arm, board Kconfig).
- `gen_dbgcompat.py`: generated the TizenRT macro block of debug.h.
- `cpush.sh`: commit with the user's identity and push the branch.
