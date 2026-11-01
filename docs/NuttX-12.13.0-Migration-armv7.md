# NuttX 12.13.0 Migration: armv7-m, armv7-r and their QEMU boards

This document describes the second arch step of the NuttX 12.13.0
migration: the cores `armv7-m` and `armv7-r`, the QEMU-only chips `imx6`
and `tiva` with their boards `sabre-6quad` and `lm3s6965-ek`, and two
more QEMU boards that check the cores, `qemu-armv7r` and `mps2-an500`.
The general migration is described in
[NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md), the choice of
the chips in [NuttX-12.13.0-Arch-Survey.md](NuttX-12.13.0-Arch-Survey.md).

- Branch: `DEV-12.13.0-LABEL-ARMV7` (from `DEV-12.13.0-LABEL-RTL8730E`,
  111c4584).
- Source: NuttX 12.13.0 (b9b5d9af, tag `nuttx-12.13.0`).
- Verification: QEMU, boot, TASH and ostest (with `smp_call` on SMP).

## Decisions (user)

| Topic | Decision |
|---|---|
| Cores | `os/arch/arm/{src,include}/{armv7-m,armv7-r}` and `os/libs/libc/machine/arm/{armv7-m,armv7-r}` are taken from NuttX 12.13.0 |
| TizenRT-only parts of the cores | decided item by item ([TD-13](#td-13-samsung-only-files-of-the-cores) to [TD-16](#td-16-cache-api-memcpy-and-the-armv7-r-gic)) |
| Chips | `imx6` and `tiva` are overwritten with NuttX 12.13.0 (QEMU only); stm32, stm32l4, stm32h745, imxrt, esp32, s5j and bcm4390x are not changed |
| `build/configs/qemu` | renamed to `build/configs/lm3s6965-ek`; the old configurations stay, not updated |
| sabre-6quad | new `ostest` and `tash` (SMP, 4 CPUs); `hello` and `hello_smp` updated |
| `hello` entry | `preapp_start()` starts `CONFIG_USER_ENTRYPOINT` at its end, like the old `os_bringup()` ([TD-20](#td-20-application-entry-point)) |
| armv7-r | port `qemu-armv7r` and run it on the OpenVela QEMU; its Docker file is in `build/configs/qemu-armv7r` |
| Extra armv7-m check | `mps2-an500` (Cortex-M7, FPU) |
| QEMU boards | every board has `build/configs/<board>/qemu_run.sh` |
| lm3s6965-ek flash | NuttX optimization options restored, lm3s6965-ek built with `DEBUG_FULLOPT` ([TD-22](#td-22-optimization-level-options)) |

## Rules

The rules of the migration apply (see "Rules applied to the synced
files" and "License headers" of the migration document, and the rules
of [NuttX-12.13.0-Migration-rtl8730e.md](NuttX-12.13.0-Migration-rtl8730e.md)):

- Synced files: only the include paths (`#include <nuttx/...>` and
  `#include "nuttx/..."` → `tinyara/`), `sched/` → `kernel/` and
  `boards/` → `board/` in makefiles and Kconfig, and the third comment
  line of Kconfig files in the TizenRT form ("see kconfig-language at
  https://www.kernel.org/..."), as in all Kconfig files synced before.
  C files are formatted with `tools/cformatter.py` of
  `agent/261022_improve-cformatter-tool` (51e698d9).
- License: `tools/license/license_manager.py` of
  `agent/261021_license-manager` (90da78a4; the commit 12777249 named
  in HANDOFF.md is no longer on the branch).  A file that existed with a
  Samsung header keeps that header; an existing file without one gets
  the year of its first `git add` (`include/armv7-m/nvicpri.h` 2019,
  `imx6/Make.defs` and `sabre-6quad/src/Makefile` 2023); new files get
  2026; Kconfig files get none.
- Old files: NuttX-origin files without a NuttX 12.13.0 peer (path in
  the NuttX history or NuttX license header) are deleted; Samsung-only
  files as decided per item.
- The sync tool was checked first by re-syncing `armv7-a`, `qemu` and
  `mps`: the tree came out byte-identical.
- A fix needed only because of old TizenRT code is its own commit and
  has an entry below.

## Commits

| Commit | Label | Content |
|---|---|---|
| `62d310c4` | `[SYNC]` | armv7-m (src, include) |
| `da67a311` | `[SYNC]` | armv7-r (src, include) |
| `5611e3b3` | `[SYNC]` | libc `machine/arm/{armv7-m,armv7-r}` |
| `f639b731` | `[MODI_NX]` | libc `machine/arm/Kconfig` sources armv7-m and armv7-r again |
| `58d29314` | `[SYNC]` | imx6 (src, include) |
| `dcdb7645` | `[MODI_NX]` | `ARCH_CHIP_IMX6` Kconfig entry of NuttX ([TD-17](#td-17-kconfig-entry-of-imx6)) |
| `72edc6d7` | `[SYNC]` | `os/board/sabre-6quad` |
| `55b11254` | `[MODI_NX]` | sabre-6quad board makefile in the TizenRT form |
| `b3c69214` | `[MODI_NX]` | `ARCH_BOARD_SABRE_6QUAD` Kconfig entry of NuttX ([TD-18](#td-18-kconfig-entry-of-sabre-6quad)) |
| `e5f4a745` | `[MODI_TR]` | `preapp_start()` starts `CONFIG_USER_ENTRYPOINT` ([TD-20](#td-20-application-entry-point)) |
| `82f80554` | `[MIXED_S,TR]` | `build/configs/sabre-6quad` (`ostest`, `tash`, `hello`, `hello_smp`, `qemu_run.sh`) |
| `55917e30` | `[MOVE]` | `build/configs/qemu` → `build/configs/lm3s6965-ek` |
| `6fc462af` | `[MODI_TR]` | CI, `build_test.sh` and README paths of the renamed directory |
| `4ff4cfc7` | `[SYNC]` | tiva (src, include) |
| `a103fd34` | `[SYNC]` | `os/board/lm3s6965-ek` |
| `8e8bfd7b` | `[MODI_NX]` | lm3s6965-ek board makefile in the TizenRT form |
| `ab650d74` | `[MODI_NX]` | optimization level options of NuttX restored ([TD-22](#td-22-optimization-level-options)) |
| `8ebb9ebc` | `[MODI_NX]` | core directory selected by the architecture ([TD-23](#td-23-core-directory-selection)) |
| `1b2f0e07` | `[SYNC]` | `os/board/qemu-armv7r` |
| `a3d79294` | `[SYNC]` | `os/board/mps2-an500` |
| `22ff5b52` | `[MODI_NX]` | qemu-armv7r and mps2-an500 board makefiles in the TizenRT form |
| `915c7dd5` | `[MODI_NX]` | Kconfig entries of qemu-armv7r and mps2-an500 |
| `b8c9dba8` | `[MIXED_S,TR]` | `build/configs/lm3s6965-ek` (`ostest`, `tash`, `qemu_run.sh`) |
| `1fd81cb4` | `[MIXED_S,TR]` | `build/configs/qemu-armv7r` (`ostest`, `tash`, `qemu_run.sh`, `Dockerfile`) |
| `4d444db8` | `[MIXED_S,TR]` | `build/configs/mps2-an500` (`ostest`, `tash`, `qemu_run.sh`) |

## Synced code

codesim (`tizenrt-nuttx-similarity/core`, `python3 -m codesim dir`,
renames tinyara/TinyAra/TINYARA/Tinyara = nuttx, `.c`, `.h`, `.S`):

| TizenRT | NuttX 12.13.0 | Identical | Only in TizenRT | Only in NuttX |
|---|---|---|---|---|
| `os/arch/arm/src/armv7-m` | `arch/arm/src/armv7-m` | 43/43 | 8 kept files ([TD-13](#td-13-samsung-only-files-of-the-cores), [TD-14](#td-14-unblock-without-saving-and-switchcontext)) | - |
| `os/arch/arm/include/armv7-m` | `arch/arm/include/armv7-m` | 3/3 | - | - |
| `os/arch/arm/src/armv7-r` | `arch/arm/src/armv7-r` | 40/40 | 4 kept files ([TD-13](#td-13-samsung-only-files-of-the-cores)) | - |
| `os/arch/arm/include/armv7-r` | `arch/arm/include/armv7-r` | 3/3 | - | - |
| `os/libs/libc/machine/arm/armv7-m`, `armv7-r` | `libs/libc/machine/arm/...` | 9/9, 8/8 | - | - |
| `os/arch/arm/src/imx6`, `include/imx6` | `arch/arm/...` | 35/35, 2/2 | - | - |
| `os/arch/arm/src/tiva`, `include/tiva` | `arch/arm/...` | 190/190, 7/7 | `memory_region.h` (Samsung, memory leak checker) | - |
| `os/board/sabre-6quad` | `boards/arm/imx6/sabre-6quad` | 8/8 | - | - |
| `os/board/lm3s6965-ek` | `boards/arm/tiva/lm3s6965-ek` | 9/9 | - | `kernel/lm_userspace.c` (protected build, not taken) |
| `os/board/qemu-armv7r` | `boards/arm/qemu/qemu-armv7r` | 6/6 | - | `kernel/qemu_userspace.c` (not taken) |
| `os/board/mps2-an500` | `boards/arm/mps/mps2-an500` | 3/3 | - | `kernel/mps_userspace.c` (not taken) |

366 of 366 common files are token-identical.  The Kconfig and makefile
files of these directories and the board linker scripts
(`dramboot.ld`, `ld.script`, `memory.ld`, `nuttx.ld`, `flash.ld`) are
byte-identical to NuttX 12.13.0 after the license header and the path
renames, except the board `src/Makefile`s (TizenRT board makefile form,
as qemu-armv7a and mps2-an521).  No number is split at the exponent
sign (`ppnum.py`); a second cformatter run changes nothing.

## Boards

| Board | QEMU | CPU | Chip | Configs |
|---|---|---|---|---|
| [sabre-6quad](../build/configs/sabre-6quad/README.md) | `-M sabrelite -m 1024 -smp 4` | 4 × Cortex-A9 (armv7-a) | imx6 | `ostest`, `tash`, `hello_smp` (SMP 4), `hello` |
| [lm3s6965-ek](../build/configs/lm3s6965-ek/README.md) | `-M lm3s6965evb` | Cortex-M3 (armv7-m) | tiva | `ostest`, `tash` |
| [mps2-an500](../build/configs/mps2-an500/README.md) | `-M mps2-an500` | Cortex-M7 + FPU (armv7-m) | mps | `ostest`, `tash` |
| [qemu-armv7r](../build/configs/qemu-armv7r/README.md) | OpenVela QEMU `-M virt -cpu cortex-r5f` | Cortex-R5F (armv7-r) | qemu | `ostest`, `tash` |

The defconfigs are full `.config` files made with `olddefconfig` from
the configurations of the boards already ported (qemu-armv7a for
sabre-6quad and qemu-armv7r, mps2-an521 for lm3s6965-ek and
mps2-an500) and the chip and board values of the NuttX configurations
of each board (`smp`, `qemu-flat`, `nsh`).

QEMU images: `tizenrt/tizenrt:2.0.1-qemu` (upstream QEMU 8.2,
`tools/docker`) for all boards except qemu-armv7r, which runs in
`tizenrt/tizenrt:2.0.1-qemu-armv7r` (`build/configs/qemu-armv7r/Dockerfile`:
the OpenVela QEMU fork, QEMU 10.0.0 based, commit df9bdfe3, built
statically into `/opt/qemu-vela`).  In this environment the image was
built from a copy of that Dockerfile with only the proxy CA added for
`git fetch`.

## Results

Each configuration was built clean (`distclean`, `configure.sh`,
`make -j8`) in `tizenrt/tizenrt:2.0.1-qemu` and run with its
`qemu_run.sh`; TASH commands were typed on the console.  "Groups" is
the number of `user_main: ... test` lines of the ostest log (not the
same count as the 33/31 groups of the first migration report).

| Config | Result |
|---|---|
| sabre-6quad/ostest | 4 CPUs; `smp_call_test: Test success`; `ostest_main: Exiting with status 0` (39 groups, 412 s) |
| sabre-6quad/tash | `ps`: `CPU0 IDLE` .. `CPU3 IDLE`; `smp`: 8 barrier threads `completed with result=0`, running on CPU0..CPU3; `ostest` from TASH: status 0 with `smp_call` |
| sabre-6quad/hello | `TASH>>Hello, World!!` at boot (hello_main started by `preapp_start`); `hello` command; 1 CPU |
| sabre-6quad/hello_smp | as `hello` on 4 CPUs; `smp`: 8 threads `result=0` |
| lm3s6965-ek/ostest | 157 KB text (`-Os`); status 0 (37 groups, 722 s) |
| lm3s6965-ek/tash | 181 KB text; `free`: 45760 bytes free after boot; `ostest` from TASH: status 0 (37 groups) |
| mps2-an500/ostest | FPU test (`FPU#1: Succeeded`, `FPU#2`); status 0 (38 groups) |
| mps2-an500/tash | `ps`, `free`; `ostest` from TASH: status 0 (38 groups) |
| qemu-armv7r/ostest | OpenVela QEMU; FPU test; status 0 (39 groups) and QEMU powers off through semihosting (178 s) |
| qemu-armv7r/tash | `ps`, `free` (16 MB heap); `ostest` from TASH: status 0 (39 groups) |

Regression of the boards ported before (same procedure):

| Config | Result |
|---|---|
| qemu-armv7a/ostest | SMP 2: `smp_call_test: Test success`; status 0 (40 groups) |
| qemu-armv7a/tash | `ps`: `CPU0 IDLE`, `CPU1 IDLE`; `smp`: 8 threads `result=0`; `ostest` from TASH: status 0 |
| mps2-an521/ostest | status 0 (37 groups) |
| mps2-an521/tash | `ostest` from TASH: status 0 (37 groups) |
| rtl8730e/tash | build and Realtek post-build in `tizenrt/tizenrt:2.0.1` (sequential make): rc 0, `km0_km4_ap_image_all.bin` 1058998 bytes, as at `0b068d7b`; not run on hardware |

QEMU prints "Timer with period zero, disabling" when lm3s6965-ek boots
(QEMU timer model, no effect).

## TizenRT-only differences

One entry per item that exists only because of the old TizenRT code
(rule 3 of the rtl8730e document; numbering continues from TD-12).
"Callers" are files outside the core in the current tree.  The old
chips (stm32, stm32l4, stm32h745, imxrt, s5j, bcm4390x) are not
buildable since `common` was synced; `binary_manager` and
`kernel/debug` are TizenRT kernel parts that are not built.

### TD-13 Samsung-only files of the cores

- Old TizenRT: files with only a Samsung header and no NuttX peer:

  | File(s) | What it does | Callers |
  |---|---|---|
  | armv7-m `up_mpusavecontext.c`, `up_mpurestorecontext.c`, `mpu-reg.h`; armv7-r `arm_mpucontextsave.S`, `arm_mpucontextrestore.S` | save/restore the per-task MPU regions (`REG_RNUM` area of `xcp.regs`) at the context switch (protected build, loadable apps) | stm32 `stm32_vectors.S`, Make.defs of the old chips |
  | armv7-m `up_setupregions.c`; armv7-r `arm_setupregions.c` | `up_setup_regions()`: per-task MPU stack and user regions in the TCB | none |
  | armv7-m `up_schedyield.c`; armv7-r `arm_schedyield.c` | `up_schedyield()` on `g_readytorun` | Make.defs of the old chips |
  | armv7-m `up_busfault.c`, `up_usagefault.c` | fault decode print, `system_exception_location`, reboot reason (`SYSTEM_REBOOT_REASON`), PANIC | irq.c of the old chips |

- NuttX 12.13.0: no per-task MPU switch (`arm_mpu.c` sets fixed regions,
  `mpu_user_*`, `mpu_priv_*`); `sched_yield()` is in the kernel;
  `arm_busfault.c`/`arm_usagefault.c` decode and PANIC, without a reboot
  reason.  The files use old kernel symbols and would not compile.
- Decision (user): kept in place and not built (not in the NuttX
  Make.defs), as the same files of armv8-m and armv7-a.  To be designed
  again with the protected build, the loadable apps and the reboot
  reason.
- Commits: `62d310c4`, `da67a311` (kept).

### TD-14 Unblock without saving, and switchcontext

- Old TizenRT: armv7-m `up_unblocktask_withoutsavereg.c` (NuttX and
  Samsung header, made by TizenRT from `up_unblocktask.c`):
  `up_unblock_task_without_savereg()`, called by
  `kernel/binary_manager/binary_manager_recovery.c`.
  `up_switchcontext.S`: `up_switchcontext()` with the `SYS_switch_context`
  SVC.
- NuttX 12.13.0: neither; the context switch is in `common`
  (`arm_switchcontext.c`) and `arm_svcall.c`.
- Decision (user): `up_unblocktask_withoutsavereg.c` kept, not built (as
  in armv8-m and armv7-a; needed when the binary manager is ported);
  `up_switchcontext.S` (NuttX origin) deleted.
- Commit: `62d310c4`.

### TD-15 TizenRT code in the core files of NuttX origin

- Old TizenRT, replaced by the NuttX 12.13.0 files:
  - armv7-m `up_assert.c`, armv7-r `arm_assert.c`: crash dump with
    `BINMGR_RECOVERY` (recovery of the faulting app),
    `APP_BINARY_SEPARATION` (faulting app information),
    `BOARD_ASSERT_AUTORESET`/`BOARD_ASSERT_SYSTEM_HALT`,
    `SECURITY_LEVEL`, `DEBUG_WORKQUEUE`, `dump_stack()` and
    `dump_all_stack()` (used by `kernel/debug/sysdbg.c`); armv7-r also
    `MPU_STACKGUARD`, symbol lookup and frame-pointer unwind
    (`DEBUG_DISPLAY_SYMBOL`), `save_cpuctxt_regs()`.
  - armv7-m `up_memfault.c`, `up_hardfault.c`: reboot reason, an
    STM32H745 case, TizenRT prints.
  - armv7-m `up_doirq.c`: nested interrupts (`ARCH_NESTED_INTERRUPT`,
    `_STACKCHECK`) and the last three IRQ numbers (`g_irq_nums`).
  - armv7-m `up_mpu.c` (`APPS_RAM_REGION_SHAREABLE`), `up_svcall.c`
    (`SUPPORT_COMMON_BINARY` system calls).
  - `TASK_SCHED_HISTORY` hooks of the block/unblock/release/reprioritize
    and signal files (armv7-m and armv7-r).
  - armv7-r abort handlers (`arm_dataabort.c`, `arm_prefetchabort.c`,
    `arm_undefinedinsn.c`: verbose prints) and the data-abort stack
    (`ARCH_DABORTSTACK`, `arm_vectors.S`).
- NuttX 12.13.0: `sched/misc/assert.c` with `up_dump_register()`, the
  `dump_stack()` macro of `execinfo.h`, the unwinders (`UNWINDER_*`),
  `arm_doirq.c` (no nested interrupts on armv7-m), `arm_mpu.c`,
  `arm_svcall.c`, the common switch code.
- Decision (user): overwritten by the sync rule; the binary manager
  recovery, the reboot reason and the crash dump are to be implemented
  again as chip/board hooks when those features are ported.
- Commits: `62d310c4`, `da67a311`.

### TD-16 Cache API, memcpy and the armv7-r GIC

- Old TizenRT: armv7-m `arch_*dcache*.c` with
  `arch_clean_dcache_by_range_to_poc()` and
  `arch_clean_invalidate_dcache_by_range_to_poc()` (used by the imxrt
  USB host); armv7-r `cache.h` with `arch_enable_dcache()` etc. and
  `ARMV7R_DCACHE`/`ARMV7R_ICACHE` (used by `s5j_boot.c`); armv7-m
  `up_memcpy.S`; armv7-r `arm_gicv2.c`, `gic.h`, `mpcore.h` of NuttX 7.x
  with `ARCH_TRUSTZONE_BOTH`.
- NuttX 12.13.0: `arm_cache.c` (`up_clean_dcache()`,
  `up_flush_dcache()`, `up_invalidate_dcache()`, `up_enable_dcache()`,
  ...) with `ARCH_DCACHE`/`ARCH_ICACHE`; `arch_memcpy.S` of
  `libs/libc/machine/arm/armv7-m` (`ARMV7M_MEMCPY`); the armv7-r GICv2
  with the same function names.
- Decision (user): replaced by NuttX 12.13.0; imxrt and s5j use the
  `up_*` cache API when they are ported.
- Commits: `62d310c4`, `da67a311`, `5611e3b3`.

### TD-17 Kconfig entry of imx6

- Old TizenRT: `ARCH_CHIP_IMX6` in `os/arch/arm/Kconfig` with
  `ARMV7A_HAVE_L2CC_PL310` commented out and `select BOOT_RUNFROMSDRAM`.
- NuttX 12.13.0: selects `ARMV7A_HAVE_L2CC_PL310`, `depends on
  BOOT_RUNFROMSDRAM` (set by the board configuration).
- Decision: the NuttX entry.  The QEMU configurations leave
  `ARMV7A_L2CC_PL310` off (QEMU does not model the PL310 correctly;
  NuttX README).
- Commit: `dcdb7645`.

### TD-18 Kconfig entry of sabre-6quad

- Old TizenRT: `ARCH_BOARD_SABRE_6QUAD` depended on `ARCH_CHIP_IMX6`,
  selected `ARCH_BUTTONS` and `ARCH_HAVE_RAM_KERNEL_TEXT`, no LEDs, and
  the board Kconfig was not sourced.
- NuttX 12.13.0: depends on `ARCH_CHIP_IMX6_6QUAD`, selects
  `ARCH_HAVE_LEDS`, `ARCH_HAVE_BUTTONS`, `ARCH_HAVE_IRQBUTTONS`.
  `ARCH_HAVE_RAM_KERNEL_TEXT` is only used by `common/up_checkspace.c`,
  which is not built.
- Decision: the NuttX entry and the `source` line.
- Commit: `b3c69214`.

### TD-19 Old imx6 and sabre-6quad changes

- Old TizenRT (imx6 from NuttX 12.0.0, sabre-6quad port 07faccb1d):
  `up_internal.h`, the FIQ stack of `CONFIG_ARMV7A_DECODEFIQ` in
  `imx_irq.c`, the loadable-app MMU code of `imx_boot.c`, procfs
  mounted with `smartfs_mount()` in `imx_bringup.c`, an 8 MB `ddr3`
  region and `_stext_ram`/`_etext_ram` in `dramboot.ld`, the source list
  in `src/Make.defs`.
- NuttX 12.13.0: `arm_internal.h`, no FIQ decode, the NuttX MMU set-up,
  `nx_mount()` (and a tmpfs mount), the whole DDR (1 GB - 8 MB) in
  `dramboot.ld`, the source list in `src/Makefile`.
- Decision: overwritten (arch survey: no TizenRT feature of its own);
  `src/Make.defs` removed with the TizenRT board makefile.
- Commits: `58d29314`, `72edc6d7`, `55b11254`, `82f80554`.

### TD-20 Application entry point

- Old TizenRT: `os_bringup()` started "appinit" (`preapp_start`,
  priority `SCHED_PRIORITY_DEFAULT + 1`) and "appmain"
  (`CONFIG_USER_ENTRYPOINT`, for example `hello_main` with
  `CONFIG_ENTRY_HELLO`, priority `SCHED_PRIORITY_DEFAULT`, stack
  `CONFIG_USERMAIN_STACKSIZE`).
- NuttX 12.13.0: `nx_bringup()` starts only `CONFIG_INIT_ENTRYPOINT`;
  with `preapp_start` the application entry point was not started.
- Decision (user): `preapp_start()` creates "appmain" at its end when
  `CONFIG_USER_ENTRYPOINT` is set and `CONFIG_APP_BINARY_SEPARATION` is
  not; priority `SCHED_PRIORITY_DEFAULT`, stack `CONFIG_INIT_STACKSIZE`
  (`CONFIG_USERMAIN_STACKSIZE` no longer exists).  Configurations
  without `CONFIG_USER_ENTRYPOINT` (qemu-armv7a, mps2-an521 and
  rtl8730e `tash`) do not change.
- Result: sabre-6quad `hello` and `hello_smp` print
  `TASH>>Hello, World!!` at boot, as the 2023 port did.
- Commit: `e5f4a745`.

### TD-21 Old tiva and lm3s6965-ek changes

- Old TizenRT (tiva from NuttX 7.20, 2017): the flash driver changed to
  run on QEMU (3a3c238bd), the lm3s Ethernet driver update (83152fe1c),
  the flat file layout with `chip/`, cc3200; the board had `up_*.c`
  files and a `QEMU_SRAM`/`QEMU_SDRAM`/`QEMU_NONE` choice for the RAM of
  a patched QEMU 2.12 (`build/configs/qemu`, 1 MB and 16 MB RAM).
- NuttX 12.13.0: tiva in `common/`, `lm/`, `tm4c/`, `cc13xx/`,
  `hardware/`; `TIVA_WITH_QEMU` for the Ethernet driver on QEMU; the
  board as `lm_*.c`; upstream QEMU emulates the real 256 KB flash and
  64 KB SRAM.
- Decision: overwritten (QEMU only).  The new configurations use the
  real memory.  The old configurations, the QEMU 2.12 patches and the
  old linker scripts stay in `build/configs/lm3s6965-ek`, not updated
  (user decision).  The Ethernet driver and the flash driver are not
  used by the new configurations (no network, no MTD).
- Commits: `4ff4cfc7`, `a103fd34`, `55917e30`.

### TD-22 Optimization level options

- Old TizenRT / earlier migration: `os/Kconfig` taken from NuttX 12.13.0
  lost the "Optimization Level" choice (`DEBUG_NOOPT`,
  `DEBUG_CUSTOMOPT`, `DEBUG_FULLOPT`, `DEBUG_OPTLEVEL`) and
  `ARCH_HAVE_CUSTOMOPT`; every build was compiled without `-O`
  although `common/Toolchain.defs` adds `-Os` with `DEBUG_FULLOPT`.
- Effect: `lm3s6965-ek/tash` overflowed the 256 KB flash by 32 KB
  (`ostest` fitted with 3.5 KB left).  With `-Os` the image is 181 KB.
- Decision (user): restore the NuttX block in `os/Kconfig.debug` (after
  `FRAME_POINTER`) and set `CONFIG_DEBUG_FULLOPT=y` in the lm3s6965-ek
  configurations, as NuttX does.  The default is `DEBUG_NOOPT` with
  `DEBUG_SYMBOLS`; all other configurations have `DEBUG_SYMBOLS=y` and
  build as before.
- Commit: `ab650d74`.

### TD-23 Core directory selection

- Old TizenRT: `os/arch/arm/src/Makefile` chose `ARCH_SUBDIR` by CPU
  (Cortex-M3/M4/M7, Cortex-R4, Cortex-M33/M55, else `ARCH_ARMV7A`,
  else `arm`); Cortex-R5 fell back to `arm`.
- NuttX 12.13.0: by architecture (`ARCH_ARMV7A`, `ARCH_ARMV7R`,
  `ARCH_ARMV8R`, `ARCH_ARMV7M`, `ARCH_ARMV8M`, `ARCH_ARMV6M`, else
  `arm`).
- Decision (user, with the plan): the NuttX block.  All existing CPUs
  get the same directory as before.
- Commit: `8ebb9ebc`.

## Old lm3s6965-ek configurations (analysis, not applied)

The QEMU-only boards should also support their old configurations.
sabre-6quad `hello` and `hello_smp` are updated (`82f80554`).  The six
old lm3s6965-ek configurations are full `.config` files of the old
TizenRT kernel.  They were analysed (with two probe builds) but not
changed.  **Decision (user): the analysis is recorded here only; it is
applied later.**

| Config | Purpose | Memory it needs | Main features | Blockers |
|---|---|---|---|---|
| `tc_64k` | TizenRT kernel TCs (`tc_main`) | real board: 256 KB flash, 64 KB SRAM (`QEMU_NONE`) | kernel TCs, no TASH | B3 |
| `build_test` | CI build test (CircleCI `qemu_build_test`, `os/tools/build_test.sh`) | patched QEMU (`QEMU_SRAM`) | kernel, driver and procfs TCs, SmartFS on the RAM MTD, examples (eeprom, timer, smart), ramtest, TASH, Ethernet | B1, B2 (Ethernet), B3 |
| `smartfs` | SmartFS demo | patched QEMU (`QEMU_SRAM`) | hello, SmartFS on the tiva flash (`TIVA_FLASH`), TASH, Ethernet | B1, B2 (Ethernet), B4 |
| `tc_1m` | kernel and network TCs | patched QEMU 1 MB variant | kernel TCs, lwip network TCs, TASH | B1, B2, B3 |
| `tc_16m` | network and Wi-Fi manager TCs, iperf | patched QEMU (`QEMU_SRAM`) | lwip, DHCP client/server, Wi-Fi manager TCs, iperf | B1, B2 |
| `tc_16m_grpc` | gRPC/protobuf C++ examples | patched QEMU (`QEMU_SRAM`) | lwip, gRPC, protobuf, libcxx, `helloxx_main` | B1, B2 |

### B1 Memory of the QEMU board

- The real LM3S6965 has 256 KB of flash and 64 KB of SRAM.  The
  upstream QEMU machine `lm3s6965evb` has the same sizes, and its flash
  is read-only: QEMU does not emulate the flash controller, so the
  software cannot program it.
- The old TizenRT ran these configurations on a patched QEMU 2.12.0-rc1
  (patches in `build/configs/lm3s6965-ek`):
  `qemu-2.12.0-rc1_16m_ram_size.patch` gives 16 MB SRAM, 128 MB
  writable flash and a 512 MB SDRAM at 0x60000000;
  `qemu-2.12.0-rc1_1m_ram_size.patch` gives 1 MB SRAM and 32 MB flash.
  The old board option `QEMU_SRAM` (or `QEMU_SDRAM`) selected the
  linker script `scripts/flash-sram-qemu-memory-increased.ld` (128 MB
  flash, 16 MB SRAM) or `flash-sdram-qemu-memory-increased.ld`.  The
  images with the testcases, SmartFS, lwip or the C++/gRPC code do not
  fit in 256 KB/64 KB.
- Now: the image `tizenrt/tizenrt:2.0.1-qemu` has the upstream QEMU 8.2
  without the patch (64 KB/256 KB only), and the `QEMU_SRAM`/
  `QEMU_SDRAM`/`QEMU_NONE` choice is gone with the NuttX 12.13.0 board
  Kconfig (TD-21).  So every configuration except `tc_64k` can neither
  be linked with the new `scripts/ld.script` nor run on our QEMU.
- Ways to apply it later:
  1. port the patch to QEMU 8.2 (the `stellaris.c` code changed since
     2.12) and build it as its own image, like qemu-armv7r
     (`build/configs/lm3s6965-ek/Dockerfile`); select the bigger linker
     script and `CONFIG_RAM_SIZE` in `build/configs` (Make.defs and the
     defconfigs), without changing a NuttX file;
  2. cut the configurations down to the real 256 KB/64 KB (most of the
     testcases, SmartFS, the network and gRPC would go);
  3. drop the configurations.

### B2 Network

- The TizenRT network stack (`os/net`, lwip) does not build on this
  tree.  Probe (`lm3s6965-ek/tash` + `NET`, `NET_LWIP`, `TIVA_ETHERNET`):
  `os/include/netinet/in.h:66: lwip/inet.h: No such file or directory`
  and `os/include/sys/socket.h:75: lwip/sockets.h: No such file or
  directory` in `os/net/netmgr` (`netdev_mgr.c`, `netdev_lwnl.c`,
  `netdev_wifi.c`, ...).  The network is outside the NuttX 12.13.0
  migration so far (not synced).
- The Ethernet driver of the NuttX 12.13.0 tiva chip (`lm3s_ethernet.c`)
  is written for the NuttX network stack; the old TizenRT lwip changes
  of the driver (83152fe1c, 8fa74eac3) went with the tiva sync.
- To apply later: the lwip build on the new tree, then an lwip netdev
  for the lm3s Ethernet on the TizenRT side (no NuttX file changed).
  `tc_1m` could also be updated with its kernel TCs only, and
  `tc_16m_grpc` with the C++ examples only; `tc_16m` has nearly nothing
  without the network.

### B3 TizenRT testcases

- Probe (`lm3s6965-ek/tash` + the kernel TC options of `tc_64k`): the
  application-side TCs of `apps/examples/testcase/le_tc` compile.  The
  kernel-side test driver `os/drivers/os_api_test`
  (`CONFIG_DRIVERS_OS_API_TEST`, TizenRT only), which the kernel TCs
  use through ioctl, fails with 39 errors in `os_api_test_drv.c` and
  `kernel/test_{task,clock,signal,sched,timer,sem}.c`: the `TESTIOC_*`
  ioctl numbers are no longer constant expressions, `struct task_tcb_s`
  and the old kernel internals are gone.
- Old names and the NuttX 12.13.0 functions: `task_init` →
  `nxtask_init`, `task_activate` → `nxtask_activate`, `sched_gettcb` →
  `nxsched_get_tcb`, `sched_self` → `nxsched_self`, `sched_foreach` →
  `nxsched_foreach`, `sched_removeblocked` → `nxsched_remove_blocked`,
  `sched_releasetcb` → `nxsched_release_tcb`, `sem_tickwait` →
  `nxsem_tickwait`, `sig_findaction` → `nxsig_find_action`.
- To apply later: port `os_api_test` to these functions (TizenRT side,
  own commits and TD entries), then run the TCs and review the failures
  one by one; the TCs of TizenRT-only features (TASH heapinfo and
  stack monitor) need a check against the NuttX behaviour.  `tc_64k`
  must also fit in 256 KB/64 KB (built with `DEBUG_FULLOPT` as the new
  configurations).

### B4 SmartFS on the flash

- The old board mounted a 1 MB SmartFS at `/mnt` in
  `board_initialize()` on the flash of the patched QEMU, through the
  QEMU path of the old TizenRT `tiva_flash.c` (3a3c238bd,
  `CONFIG_QEMU_SRAM`), which wrote the writable flash directly.  Both
  went with the NuttX 12.13.0 sync (TD-21).  The NuttX tiva flash driver
  (`TIVA_FLASH`) programs the flash controller, which QEMU does not
  emulate.
- `build_test` uses the RAM MTD (`RAMMTD`), which the NuttX code still
  has; only the mount is missing (the NuttX `lm_bringup.c` does not
  create it).
- To apply later: SmartFS on the RAM MTD, created on the TizenRT side
  (content lost at each boot), or a TizenRT MTD for the writable flash
  of a patched QEMU (needs B1, option 1).

### How the probes were made

A seed from `lm3s6965-ek/tash/defconfig` with the extra options
(`mkseed.py`), `olddefconfig`, a temporary
`build/configs/lm3s6965-ek/_probe/defconfig` and `make -k -j8`, so
that all errors are listed.  The probe directory is not committed.

## Known gaps

- The other armv7-m and armv7-r chips (stm32, stm32l4, stm32h745,
  imxrt; s5j, bcm4390x) are not ported; they did not build since the
  `common` sync and still use the old core API (`up_*` files, the
  TizenRT-only items above).
- The TizenRT-only features of TD-13 to TD-16 (per-task MPU, binary
  manager recovery, reboot reason, crash dump, nested interrupts on
  armv7-m) are not available on the NuttX cores.
- Protected build: the `kernel/` user space blobs of lm3s6965-ek,
  qemu-armv7r and mps2-an500 are not taken.
- The old lm3s6965-ek configurations (`build_test`, `smartfs`, `tc_*`)
  are not updated (analysis and options in
  [Old lm3s6965-ek configurations](#old-lm3s6965-ek-configurations-analysis-not-applied));
  the CircleCI job and `os/tools/build_test.sh` still build
  `lm3s6965-ek/build_test`, which does not build.
- qemu-armv7r needs the OpenVela QEMU (not upstream); the image is built
  from source (gitlab.com is needed for the QEMU meson subprojects).
