# NuttX 12.13.0 Migration

TizenRT `os/` was forked from NuttX 7.x in 2016.  This migration brings
the NuttX 12.13.0 code (tag `nuttx-12.13.0`, release notes commit
b9b5d9afbf5a) and the OS test of nuttx-apps 12.13.0 (e4b84b2) into
TizenRT.  The TizenRT build system, Kconfig and defconfig style, the
`os/kernel` and `os/include/tinyara` names and the TizenRT apps,
framework and external code are kept.

The result is verified on two QEMU boards, FLAT build only:

| Board | CPU | Configs | Result on QEMU 8.2 |
|---|---|---|---|
| [qemu-armv7a](../build/configs/qemu-armv7a/README.md) | Cortex-A7, `virt`, GICv2, PL011 | `ostest`, `tash` | 33 test groups pass, `ostest_main: Exiting with status 0` |
| [mps2-an521](../build/configs/mps2-an521/README.md) | Cortex-M33, MPS2 AN521, CMSDK UART | `ostest`, `tash` | 31 test groups pass, `ostest_main: Exiting with status 0` |

## Layout

| NuttX 12.13.0 | TizenRT | Notes |
|---|---|---|
| `sched/` | `os/kernel/` | `sched/wqueue` → `os/kernel/wqueue` (was `os/wqueue`) |
| `include/nuttx/` | `os/include/tinyara/` | |
| `include/` | `os/include/` | `debug.h` ends with a TizenRT macro block |
| `libs/libc` | `os/libs/libc` | was `lib/libc`; `binfmt/libelf` → `libs/libc/elf`, `binfmt/libbuiltin` → `libs/libc/builtin`, user work queue → `libs/libc/wqueue` |
| `libs/{libm,libxx,libbuiltin,libdsp,libnx}` | `os/libs/...` | `lib/libc/math` → `libs/libm/libm`, `lib/libxx` → `libs/libxx` |
| `mm/`, `fs/`, `binfmt/`, `audio/`, `syscall/` | same under `os/` | `fs/driver/block` → `fs/driver`, `fs/driver/mtd` → `drivers/mtd` |
| `graphics/`, `video/` | `os/graphics`, `os/video` | new |
| `drivers/*` | `os/drivers/*` | driver files at the root of `os/drivers` moved to the NuttX subdirectories first |
| `arch/arm/{src,include}/{common,armv7-a,armv8-m,qemu,mps}` | `os/arch/arm/...` | |
| `boards/boardctl.c` | `os/board/boardctl.c` | was `os/arch/boardctl.c` |
| `boards/arm/{qemu/qemu-armv7a,mps/mps2-an521}` | `os/board/<board>/{Kconfig,include,src}` + `build/configs/<board>/{Make.defs,scripts,<config>/defconfig}` | |
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
  binary/task manager families, ...) onto them.
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
| openamp: `openamp/`, `drivers/rpmsg`, `drivers/rptun`, `fs/rpmsgfs`, every `*rpmsg*`/`*rptun*` file, except 5 headers (below) | not used |
| `crypto/`, `cmake/` | out of scope |
| `tools/`, `dummy/`, `pass1/` | NuttX build system; TizenRT `os/tools` is kept |
| drivers TizenRT does not have | not added, except `note` and what the QEMU boards need (`serial/uart_pl011`, `serial/serial_cmsdk`, `timers`) |
| other arch and board directories | only the two QEMU boards are ported |

Five rpmsg headers that synced files include unconditionally are synced
although openamp is not: `include/nuttx/drivers/rpmsgdev.h`,
`rpmsgblk.h`, `include/nuttx/syslog/syslog_rpmsg.h`,
`include/nuttx/rpmsg/rpmsg_port.h` and `fs/rpmsgfs/rpmsgfs.h`.  They only
declare APIs under `CONFIG_` options and include headers that are synced,
so `syslog_initialize.c`, `qemu_boot.c`, `qemu_bringup.c` and
`fs_initialize.c` stay as in NuttX.  `net/rpmsgdrv.h` and
`usrsock/usrsock_rpmsg.h` need the NuttX network headers and are not
synced.

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
commands.  `CONFIG_USER_ENTRYPOINT` is no longer used by the OS.

## Files that differ from NuttX on purpose

`tizenrt-nuttx-similarity` core compares every synced file with its
NuttX file (renames tinyara=nuttx, TinyAra=NuttX, TINYARA=NUTTX):
**3867 of 3887 files are identical**.  The 20 others are:

| File | Change |
|---|---|
| `os/include/debug.h` | TizenRT macro block at the end |
| `os/include/.gitignore` | ignores the generated `apps` link |
| `os/include/tinyara/power/regulator.h` | `power/pm.h` include guard |
| `os/kernel/sched/sched_cpuload_{oneshot,period}.c` | `power/pm.h` include guard |
| `os/drivers/drivers_initialize.c` | include guards (crypto, net, pm, usrsock) |
| `os/drivers/serial/serial.c`, `os/drivers/timers/watchdog.c` | `power/pm.h` include guard |
| `os/drivers/Kconfig`, `os/drivers/Makefile` | not-synced drivers dropped, TizenRT drivers added |
| `os/drivers/power/{Kconfig,Make.defs}` | `pm` dropped |
| `os/fs/{Kconfig,Makefile}` | `nfs`, `rpmsgfs` (and `socket` in the Makefile) dropped |
| `os/libs/libc/Kconfig` | `net`, `netdb` dropped |
| `os/libs/libc/machine/Kconfig`, `os/libs/libc/machine/arm/Kconfig` | arch directories that are not synced dropped |
| `os/arch/arm/src/Makefile` | links `build/output/bin/tinyara` from `build/output/libraries` |
| `apps/examples/testcase/ostest/ostest_main.c` | entry point `ostest_main()` (`main()` with CONFIG_BUILD_KERNEL) |

A token comparison ignores whitespace, so the synced C files were also
scanned for numbers split at the exponent sign; none is left.

## TizenRT changes outside the synced files

- `os/include/queue.h`: compatibility header for the 85 TizenRT files
  that include `<queue.h>` (NuttX has `<tinyara/queue.h>` only).
- `os/drivers/pm/Make.defs`: the TizenRT `/dev/pm` driver is built only
  with `CONFIG_PM`.
- `apps/system/utils`: `CONFIG_LIBC_HOMEDIR` (renamed in NuttX),
  `fscmd.c` without the `CONFIG_NFILE_DESCRIPTORS` guard and with its own
  mount point callback type; `apps/shell` and the `FS_CMDS` option no
  longer depend on `NFILE_DESCRIPTORS`.
- `apps/system/system_file/system_file.c`: includes `<tinyara/crc32.h>`.
- `apps/examples/testcase/ostest`: `Make.defs` in the TizenRT form with
  the conditions of the nuttx-apps Makefile; `fork.c` and `multiuser.c`
  (TizenRT additions) are kept but not built.

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
- 90 files still include `<queue.h>` (list them with
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
  `drivers/power/pm`, which are not synced.
- mps2-an521 has no `board_power_off()` in NuttX 12.13.0, so its ostest
  configuration cannot power QEMU off.
- The apps preconfig regenerates `apps/examples/SoundPlayer/Kconfig`
  and `Kconfig_ENTRY` (behavior of TizenRT `apps/tools/mkkconfig.sh`,
  not of this migration).
