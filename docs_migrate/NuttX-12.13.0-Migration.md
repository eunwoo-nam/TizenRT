# NuttX 12.13.0 Migration

TizenRT `os/` was forked from NuttX 7.x in 2016.  This migration brings
the NuttX 12.13.0 code (tag `nuttx-12.13.0`, release notes commit
b9b5d9afbf5a) and the OS test of nuttx-apps 12.13.0 (e4b84b2) into
TizenRT.  The TizenRT build system, Kconfig and defconfig style, the
`os/kernel` and `os/include/tinyara` names and the TizenRT apps,
framework and external code are kept.

The branch is based on upstream/master `e2d34db96` (rebased on
2026-11-03).  Upstream removed the task manager framework (45d6bb67f)
and libtuv (75ee1ec9b), so the task manager entries of the TizenRT
blocks in `os/Kconfig`, `os/Kconfig.debug`, `os/drivers/Makefile` and
`os/include/debug.h` are removed too.  Upstream changes to files that
this migration replaces with NuttX files are not carried over (see the
rules below): the mutex self-lock check of `sem_wait.c` (5ab57d70d), the
mutex flag of the heap semaphore in `mm_sem.c` (2ba78eace), the order of
the preference callback cleanup in `task_exit.c` (72ed0957a), the errno
of the signal set, `sigprocmask()` and `sigsuspend()` error paths
(9b21f619f) and the failed free log of `umm_free.c` (e2d34db96).

The result is verified on QEMU boards, FLAT build only.  The first two
boards are from the first step; the other three, the SMP configuration
of qemu-armv7a and the armv7-m/armv7-r cores come from
`DEV-12.13.0-LABEL-ARMV7` (follow-up decisions of 2026-11-02).  Results
of DEV-12.13.0-LABEL `3b1582dc0` (clean build, then ostest on QEMU; in
`tash` the `ostest` command is typed at the prompt; a test group is a
`user_main: ... test` line):

| Board | CPU | Configs | Result |
|---|---|---|---|
| [qemu-armv7a](../build/configs/qemu-armv7a/README.md) | Cortex-A7 ×2 (SMP), `virt`, GICv2, PL011 | `ostest`, `tash` | 39 test groups pass (with `smp_call`), `ostest_main: Exiting with status 0` (QEMU 8.2) |
| [mps2-an521](../build/configs/mps2-an521/README.md) | Cortex-M33, MPS2 AN521, CMSDK UART | `ostest`, `tash` | 36 test groups pass, status 0 (QEMU 8.2) |
| [mps2-an500](../build/configs/mps2-an500/README.md) | Cortex-M7, MPS2 AN500, CMSDK UART | `ostest`, `tash` | 37 test groups pass, status 0 (QEMU 8.2) |
| [lm3s6965-ek](../build/configs/lm3s6965-ek/README.md) | Cortex-M3, TI Stellaris LM3S6965 (tiva) | `ostest`, `tash` | 36 test groups pass, status 0 in 1 of 6 runs; in the others the round-robin test fails depending on the host speed and ostest stops there (TD-TEST-01). The binary of `3b1582dc0` equals the passing one except the build time (QEMU 8.2) |
| [qemu-armv7r](../build/configs/qemu-armv7r/README.md) | Cortex-R5F, `virt` of the OpenVela QEMU | `ostest`, `tash` | 38 test groups pass, status 0 (OpenVela QEMU, image from `build/configs/qemu-armv7r/Dockerfile`) |

## Layout

| NuttX 12.13.0 | TizenRT | Notes |
|---|---|---|
| `sched/` | `os/kernel/` | `sched/wqueue` → `os/kernel/wqueue` (was `os/wqueue`) |
| `include/nuttx/` | `os/include/tinyara/` | |
| `include/` | `os/include/` | `debug.h` ends with a TizenRT macro block |
| `libs/libc` | `os/libs/libc` | was `lib/libc`; `binfmt/libelf` → `libs/libc/elf`, `binfmt/libbuiltin` → `libs/libc/builtin`, user work queue → `libs/libc/wqueue` |
| `libs/{libm,libxx,libbuiltin,libdsp}` | `os/libs/...` | `lib/libc/math` → `libs/libm/libm`, `lib/libxx` → `libs/libxx` |
| `mm/`, `fs/`, `binfmt/`, `syscall/` | same under `os/` | `fs/driver/block` → `fs/driver`, `fs/driver/mtd` → `drivers/mtd` |
| `drivers/*` | `os/drivers/*` | driver files at the root of `os/drivers` moved to the NuttX subdirectories first; the areas in "Not synced" keep the TizenRT drivers |
| `arch/arm/{src,include}/{common,armv7-a,armv7-m,armv7-r,armv8-m,qemu,mps,tiva}` | `os/arch/arm/...` | armv7-m, armv7-r and tiva from `DEV-12.13.0-LABEL-ARMV7` |
| `boards/boardctl.c` | `os/board/boardctl.c` | was `os/arch/boardctl.c` |
| `boards/arm/{qemu/qemu-armv7a,qemu/qemu-armv7r,mps/mps2-an500,mps/mps2-an521,tiva/lm3s6965-ek}` | `os/board/<board>/{Kconfig,include,src}` + `build/configs/<board>/{Make.defs,scripts,<config>/defconfig}` | `build/configs/qemu` was renamed to `lm3s6965-ek` |
| nuttx-apps `testing/ostest` | `apps/examples/testcase/ostest` | |

Directories and files were moved with `git mv` before the sync commits,
so `git log --follow` shows their TizenRT history.

## Rules applied to the synced files

- Only include paths are renamed: `#include <nuttx/...>` →
  `<tinyara/...>`; in makefiles and Kconfig `sched/` → `kernel/` and
  `boards/` → `board/`.  Header guards, `__NUTTX__` and comments keep
  their NuttX names.
- C files are formatted with `tools/cformatter.py` of branch
  `agent/261022_improve-cformatter-tool`, which changes only whitespace.
  Two formatter bugs were fixed on that branch during the migration:
  3463f629 (block comments continued from a directive line) and
  51e698d9 (the sign of an exponent, `1e-1`, was spaced as an operator).
- NuttX code uses the NuttX logging macros.  `os/include/debug.h` maps
  the TizenRT macros (`dbg`, `lldbg`, `vdbg`, `sdbg`, the
  binary manager family, ...) onto them.
- TizenRT changes inside synced NuttX files (task manager, binary
  manager, heapinfo hooks, ...) are overwritten.  Samsung-only files are
  kept and switched off by configuration.
- TizenRT files of NuttX origin that 12.13.0 removed or renamed (old
  `up_*.c` of the synced arch directories, ...) are deleted.

## Not synced

| Area | Reason |
|---|---|
| net: `net/`, `drivers/net`, `drivers/wireless`, `include/{net,netinet,arpa,netpacket}`, `sys/socket.h`, `sys/sockio.h`, `sys/un.h`, `netdb.h`, `ifaddrs.h`, `include/nuttx/{net,usrsock}`, `libs/libc/{net,netdb}`, `fs/socket` | TizenRT keeps its own network stack |
| pm: `drivers/power/pm`, pm headers | TizenRT keeps `os/pm` |
| `wireless/` | not used |
| `fs/nfs` | NFSv3 client on the NuttX network stack |
| openamp: the top-level `openamp/` (OpenAMP library download and patches) and the `*rpmsg*` files of the areas that are not synced (net, usrsock, wireless, sensors, power; clk, reset, virtio and vhost do not exist in TizenRT) | `drivers/rpmsg`, `drivers/rptun`, `fs/rpmsgfs` and the other rpmsg files are synced with their options off (decision 2026-11-02); the options cannot be used without `openamp/` |
| audio: `audio/`, `drivers/audio`, `include/nuttx/audio` | TizenRT keeps its audio drivers and headers (decision 2026-11-02) |
| graphics: `graphics/`, `libs/libnx`, `include/nuttx/nx` | not taken in the rewritten history |
| video: `video/`, `drivers/video`, `include/nuttx/video`, `include/sys/video*` | TizenRT keeps its video driver and headers |
| driver areas `input`, `lcd`, `sensors`, `power` (except `pm`, which is listed above) | TizenRT keeps its drivers (8dd98470f) |
| drivers TizenRT does not have | not added, except `note`, `rpmsg`, `rptun`, `usbmisc`, `usbmonitor` and what the QEMU boards need (`serial/uart_pl011`, `serial/serial_cmsdk`, `timers`) |
| `crypto/`, `cmake/` | out of scope |
| `tools/`, `dummy/`, `pass1/` | NuttX build system; TizenRT `os/tools` is kept |
| other arch and board directories | only the QEMU boards in the table above are ported |

The rpmsg and rptun area is synced since the follow-up decisions of
2026-11-02 (`drivers/rpmsg`, `drivers/rptun`, `fs/rpmsgfs`,
`include/nuttx/{rpmsg,rptun}` and the rpmsg files of the synced driver
directories).  Before, only five rpmsg headers that synced files include
unconditionally were synced.  `net/rpmsgdrv.h`,
`usrsock/usrsock_rpmsg.h` and `netpacket/rpmsg.h` belong to the network
area and are still not synced.

The other NuttX files that include a header of a not-synced area
unconditionally get an include guard with the option of that area, for
example:

```c
#if defined(CONFIG_PM)
#  include <tinyara/power/pm.h>
#endif
```

## Build system

TizenRT keeps its build flow (`tools/configure.sh <board>/<config>`,
`dbuild.sh`, `build/output/{bin,libraries}`, full `.config` defconfigs,
the TizenRT makefiles and `os/tools/mkdeps.c`) and the makefiles select
the NuttX 12.13.0 sources: the source lists, the search paths and the
selection conditions follow the NuttX 12.13.0 layout (`os/kernel`,
`os/board`, `os/drivers/mtd`, `os/fs/driver`, `os/libs`).

- `os/tools/Config.mk`: the TizenRT Config.mk plus what the NuttX
  toolchain descriptions need (DEFINE_PREFIX, INCDIR_PREFIX,
  INCSYSDIR_PREFIX, CONVERT_PATH, the `__NuttX__`/NDEBUG ARCHDEFINES)
  and a COMPILE that takes the extra flags and the per-file
  `<file>_CFLAGS` of the caller.
- `os/tools/{D,Rust,Swift,Zig}.defs`: from NuttX, included by the arch
  Toolchain.defs.  `mkconfig.c` (NuttX sanity checks + TizenRT board
  macros) and `cfgdefine.c` (string options used as identifiers such
  as `CONFIG_INIT_ENTRYPOINT`) keep the NuttX 12.13.0 generation.
- The module Make.defs and makefiles (kernel, fs, drivers, binfmt,
  syscall, arch/arm, board, libs) keep the TizenRT rules and take the
  source selection of the NuttX makefiles.  libc and mm create the
  bin and kbin object directories themselves (the mm/bin, mm/kbin and
  mm/ubin helper makefiles are gone) and libc builds the gnu-elf.ld
  ELF module linker script with PREPROCESS (`CONFIG_LIBC_ELF`).
- `os/arch/arm/src/Makefile`: the NuttX 12.13.0 arm sources into
  libarch, libkarch and libuarch, linking `build/output/bin/tinyara`
  from `build/output/libraries`.  Boards whose linker script uses
  configuration macros opt in with `PREPROCESS_LDSCRIPT = y` in their
  build/configs Make.defs (qemu-armv7a); the script is preprocessed
  with the existing PREPROCESS macro and rebuilt when
  `include/tinyara/config.h` changes.
- `os/Makefile.unix`, `Directories.mk`, `FlatLibs.mk`,
  `KernelLibs.mk`, `ProtectedLibs.mk`, `LibTargets.mk`: `libs/`
  instead of `lib/`, libsched from `kernel`, libboards, libm,
  libbuiltin, libxx, libdsp.  libwque is gone (kernel and libc have
  the work queue).
- Kconfig: the NuttX Kconfig files with the TizenRT-only symbols merged
  into `os/Kconfig`, `os/Kconfig.debug`, `os/arch/Kconfig`,
  `os/arch/arm/Kconfig` and `os/board/Kconfig`.
- `tools/docker/Dockerfile`: `tizenrt/tizenrt:2.0.1` plus a static
  `qemu-system-arm` 8.2, tag `tizenrt/tizenrt:2.0.1-qemu`, selected by
  `CONFIG_DOCKER_VERSION="2.0.1-qemu"`.

## Entry point

NuttX 12.13.0 starts only `CONFIG_INIT_ENTRYPOINT`.  The TizenRT
pre-application start-up of the old `os_bringup` is gone, so a TASH
configuration uses `CONFIG_INIT_ENTRYPOINT="preapp_start"`
(`apps/system/init`), which starts TASH and registers the builtin
commands.  NuttX renamed `USER_ENTRYPOINT` to `INIT_ENTRYPOINT` (and
the bool `INIT_ENTRYPOINT` to `INIT_ENTRY`), so the application entry
list (`apps/apps_entry/Kconfig` and the application Kconfig files)
sets `INIT_ENTRYPOINT`.  `os/kernel/Kconfig` keeps the symbol without
the prompt and the `"main"` default: it is read before the apps
Kconfig, so its default would win over the application defaults.
The QEMU configurations select `ENTRY_MANUAL` with the entry point
name.

## Files that differ from NuttX on purpose

`tizenrt-nuttx-similarity` core compares every synced file with its
NuttX file (renames tinyara=nuttx, TinyAra=NuttX, TINYARA=NUTTX):
**3863 of 3887 files are identical**.  The 24 others are:

| File | Change |
|---|---|
| `os/include/debug.h` | TizenRT macro block at the end |
| `os/include/.gitignore` | ignores the generated `apps` link |
| `os/include/tinyara/fs/ioctl.h` | the `PMIOC_*` commands of the TizenRT pm driver; `_PMBASE` is 0x4800, as 0x2b00 is `_RPMSGBASE` in NuttX (decision 2026-11-04) |
| `os/include/tinyara/power/regulator.h` | `power/pm.h` include guard |
| `os/kernel/sched/sched_cpuload_{oneshot,period}.c` | `power/pm.h` include guard |
| `os/drivers/drivers_initialize.c` | include guards (crypto, net, pm, usrsock) |
| `os/drivers/serial/serial.c`, `os/drivers/timers/watchdog.c` | `power/pm.h` include guard |
| `os/drivers/Kconfig`, `os/drivers/Makefile` | not-synced drivers dropped, TizenRT drivers added |
| `os/drivers/power/{Kconfig,Make.defs}` | `pm` dropped |
| `os/fs/{Kconfig,Makefile}` | `nfs` (and `socket` in the Makefile) dropped |
| `os/libs/libc/Kconfig` | `net`, `netdb` dropped |
| `os/libs/libc/machine/Kconfig`, `os/libs/libc/machine/arm/Kconfig` | arch directories that are not synced dropped |
| `os/arch/arm/src/Makefile` | links `build/output/bin/tinyara` from `build/output/libraries` |
| `apps/examples/testcase/ostest/ostest_main.c` | entry point `ostest_main()` (`main()` with CONFIG_BUILD_KERNEL) |
| `os/arch/arm/src/common/Toolchain.defs` | the D/Rust/Swift/Zig toolchain includes removed (the `.defs` are not used; decision 2026-11-02) |
| `os/fs/procfs/fs_procfsversion.c` | `CONFIG_BASE_DEFCONFIG` removed from the `/proc/version` line (decision 2026-11-02) |
| `os/kernel/Kconfig` | `INIT_ENTRYPOINT` without the prompt and the `"main"` default; the application entry list sets it (decision 2026-11-04) |

A token comparison ignores whitespace, so the synced C files were also
scanned for numbers split at the exponent sign; none is left.

## TizenRT changes outside the synced files

- `os/include/tinyara/pm/pm.h`: includes `<tinyara/queue.h>` (NuttX
  has no `<queue.h>`) and declares the domain watchdog as
  `FAR struct wdog_s *`, the type of the removed `WDOG_ID`, so that
  `os/drivers/pm/pm.c` builds in every configuration, as in TizenRT
  upstream (decision 2026-11-04).
- `apps/system/utils`: `CONFIG_LIBC_HOMEDIR` (renamed in NuttX),
  `fscmd.c` without the `CONFIG_NFILE_DESCRIPTORS` guard and with its own
  mount point callback type; `apps/shell` and the `FS_CMDS` option no
  longer depend on `NFILE_DESCRIPTORS`.
- `apps/system/system_file/system_file.c`: includes `<tinyara/crc32.h>`.
- `apps/examples/testcase/ostest`: `Make.defs` in the TizenRT form with
  the conditions of the nuttx-apps Makefile; `fork.c` and `multiuser.c`
  (TizenRT additions) are kept but not built.
- Follow-up decisions of 2026-11-02:
  - `os/include/cJSON.h` moved to `os/drivers/audio/cJSON.h`, the only
    user being the syu645b codec driver.
  - The sysdbg feature (`CONFIG_DEBUG_SYSTEM`, `/dev/sysdbg`) is removed.
  - The TizenRT Bessel functions of `os/libs/libm/libm` are removed.
    Use `CONFIG_LIBM_TOOLCHAIN` (default; the toolchain newlib
    `libm.a`, statically linked), which has `j0`..`ynf`.

## License headers

Samsung Electronics Apache 2.0 headers are applied with
`tools/license/license_manager.py` of branch
`agent/261021_license-manager` (12777249), used from a separate
checkout as the formatter:

| Files | Count | Header |
|---|---|---|
| TizenRT files that had a Samsung header on master | 1122 | the master header, same text and year, on top of the NuttX header |
| TizenRT files without a Samsung header but with a NuttX license | 13 | `license_manager.py add` (year of the first git add: 2023 ×10, 2016 ×1, 2026 ×2) |
| New files with a NuttX Apache or a third-party license, and 9 data headers without a license | 2553 | `license_manager.py add --year 2026`; for types the tool does not know (.S, .ld, .defs, Makefile.*, C++ headers without extension) the tool's own functions |
| Kconfig, .gitignore, .csv, .patch, docs, defconfig | - | unchanged |

Only the header is added: each file is the previous file with the header
in front of it, codesim results and cformatter are unchanged, and the four
QEMU configurations still pass.

`license_manager.py check` takes the year of the first `git add` of the
current path (no `--follow`), so 501 files are reported as YEAR_MISMATCH
although their header follows the rules above: files moved by this
migration (430), renamed earlier in the TizenRT history (25), already
different on master (19) and new NuttX files on paths TizenRT had
deleted (27).  Do not run `license_manager.py fix` on them.  The list,
the third-party licensed files and the files without a license are in
[NuttX-12.13.0-License-Warnings.md](NuttX-12.13.0-License-Warnings.md).

## Known gaps

- Only qemu-armv7a and mps2-an521 are ported.  The other TizenRT boards
  and chips still use the old arch code and the old driver interfaces.
- Protected and kernel builds are not updated (`ProtectedLibs.mk`,
  `KernelLibs.mk`, the userspace linker scripts).
- TizenRT code that is not built for the QEMU boards still includes
  headers that NuttX 12.13.0 moved or removed: `tinyara/i2c.h` (now
  `tinyara/i2c/i2c_master.h`), `tinyara/fs/mtd.h` (now
  `tinyara/mtd/mtd.h`), `crc8.h`/`crc16.h`/`crc32.h` (now
  `tinyara/crc*.h`), `tinyara/time.h`, `tinyara/math.h`,
  `tinyara/fs/dirent.h`, `tinyara/configdata.h`, `tinyara/float.h`.
- 75 files still include `<queue.h>` (list them with
  `git grep -l -E '#\s*include\s*[<"]queue\.h[">]' -- os/ apps/`);
  `os/include/queue.h` is dropped, so they must move to
  `<tinyara/queue.h>` before their subsystems are enabled.
- `include/crypto` and `include/nuttx/crypto` are not synced (crypto
  is out of scope), except `nuttx/crypto/tea.h`, which the synced libc
  TEA cipher includes.  `blake2s.h`, `crypto.h` and `se05x.h` will be
  needed when the crypto implementation is migrated.
- `CONFIG_NFILE_DESCRIPTORS` and `CONFIG_NFILE_STREAMS` (removed in
  NuttX) are still used by le_tc, ftpc, libcoap, lwm2m and the vi
  Kconfig.
- A board with `CONFIG_PM` needs the NuttX `power/pm.h` and
  `drivers/power/pm`, which are not synced, and the TizenRT `os/pm`
  sources still use the old wdog API (`WDOG_ID`, `wd_create()`).
- The 94 defconfigs of the boards that are not ported still use the
  old entry point options: `CONFIG_INIT_ENTRYPOINT=y` (the old bool),
  `CONFIG_USER_ENTRYPOINT` and `CONFIG_USERMAIN_STACKSIZE`.  Port them
  to `CONFIG_INIT_ENTRY=y`, `CONFIG_INIT_ENTRYPOINT` and
  `CONFIG_INIT_STACKSIZE` with their boards.
- NuttX `nx_bringup` starts one task.  A configuration that starts
  TASH (`preapp_start`) and a user application (`appmain`) together,
  as the old `os_bringup` did, is not possible now, and
  `SYSTEM_PREAPP_STACKSIZE` is not used (`preapp_start` runs with
  `INIT_STACKSIZE`).
- Only `CONFIG_LIBM_TOOLCHAIN` and `CONFIG_LIBM_NONE` build: the
  TizenRT `os/libs/libm/Makefile` and the NuttX libm, newlib, openlibm
  and libmcs `Make.defs` mix `context:` and `context::` rules (TD-LIBS-11
  of [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md)).
- The rpmsg options need the OpenAMP library of `openamp/`, which is not
  synced (TD-RPMSG-01).  The TizenRT USB host drivers `rtl8723d.c` and
  `uvc/` are no longer wired after the USB sync (TD-USB-01).
- The follow-up list of the reviews is
  [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md).
- mps2-an521 has no `board_power_off()` in NuttX 12.13.0, so its ostest
  configuration cannot power QEMU off.
- The apps preconfig regenerates `apps/examples/SoundPlayer/Kconfig`
  and `Kconfig_ENTRY` (behavior of TizenRT `apps/tools/mkkconfig.sh`,
  not of this migration).
