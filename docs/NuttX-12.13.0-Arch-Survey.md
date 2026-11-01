# NuttX 12.13.0 Migration: survey of the not-synced arch directories

The NuttX 12.13.0 migration synced only `os/arch/arm/{src,include}/
{common,armv7-a,armv8-m,qemu,mps}` (see
[NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md)).  This
survey is step 1 of deciding, chip by chip, whether the TizenRT code can
be **overwritten with the NuttX 12.13.0 code**.  It records where each
directory came from, what TizenRT changed after it took the code, and
which boards use it.  No code was changed for it.

Scope (user decision): the core directories `armv7-m` and `armv7-r` are
taken from NuttX 12.13.0 in any case; the chips `imx6`, `imxrt`,
`stm32`, `stm32l4`, `tiva`, `stm32h745` and `os/arch/xtensa` (`xtensa`,
`lx6`, `esp32`) are selected one by one.

## Status

Done on branch `DEV-12.13.0-LABEL-ARMV7` (see
[NuttX-12.13.0-Migration-armv7.md](NuttX-12.13.0-Migration-armv7.md)):
the cores `armv7-m` and `armv7-r` (with their libc parts) and the chips
`imx6` and `tiva` are taken from NuttX 12.13.0; `sabre-6quad`,
`lm3s6965-ek`, `mps2-an500` and `qemu-armv7r` run on QEMU.  The other
chips of the table below are not changed.

## Decision rule (user)

A chip directory can be overwritten with the NuttX 12.13.0 code when

1. it is only used by QEMU boards (no real product uses it), or
2. the comparison shows that TizenRT made no changes of its own: the
   board was only taken for testing.

Otherwise the TizenRT changes listed below would be lost and the chip
needs a closer review before it is taken.

## Result

| Chip (TizenRT) | NuttX 12.13.0 | Boards (configs) | QEMU only | Origin | TizenRT changes | Decision |
|---|---|---|---|---|---|---|
| `imx6` | `imx6` | `sabre-6quad` (`hello`, `hello_smp`) | **yes** (`qemu-system-arm -machine sabrelite`, port commit 07faccb1d) | NuttX 12.0.0 (37/37 files, avg 0.995) | none of its own: 17 files differ only by tree-wide TizenRT edits (`up_internal.h`, heap and FIQ stack, MMU for loadable apps) | **Overwrite** (rule 1 and 2) |
| `tiva` | `tiva` | `qemu` → `lm3s6965-ek` (`build_test`, `smartfs`, `tc_1m`, `tc_16m`, `tc_16m_grpc`, `tc_64k`) | **yes** (README: QEMU 2.12 target) | NuttX 7.20 (73/75 files, avg 0.97) | flash driver changed to run on QEMU, lm3s Ethernet driver update, `tiva_adc.c` added; the rest tree-wide | **Overwrite** (rule 1); redo the QEMU flash/Ethernet changes when the board is ported, if still needed |
| `stm32` | `stm32` | `stm32f407-disc1`, `stm32f429i-disco` (`hello`, `tc`) | no (ST Discovery kits) | NuttX 7.8 (172/173 files, 116 identical, avg 0.99) | few of its own: USB host CDC-ACM fixes (2019), Kconfig menu fix, CCM `xalloc_user_at`; the rest tree-wide (PM code removed, heap symbols, fault logs) | **Overwrite** (rule 2: test boards, `hello`/`tc` only); check the 3 USB host/CCM fixes against 12.13.0 |
| `stm32l4` | `stm32l4` | `stm32l4r9ai-disco` (`hello`, `pm`) | no (ST Discovery kit) | NuttX 8.1 for 159 files (avg 0.98) **plus 146 files that are not NuttX** (ST HAL, CMSIS) | PM for STM32L4 (Stop2, tick correction, wake-up clocks, EXTI, staycount: 18 commits), ramdump, console | **Review**: the NuttX part can be overwritten, but the ST HAL part is not NuttX code and the PM work would be lost |
| `stm32h745` | `stm32h7` (other name) | `stm32h745zi-nucleo` (`hello`) | no (ST Nucleo board) | **not from NuttX**: 3/113 files in common with NuttX 10.3.0 `stm32h7`; 72 ST HAL and 5 CMSIS files | the port itself (ST HAL based), baud rate, reset flag | **Not an overwrite**: NuttX `stm32h7` is a different implementation; taking it means a new port (only `hello` exists) |
| `imxrt` | `imxrt` | `imxrt1020-evk` (`loadable_elf_apps`), `imxrt1050-evk` (`hello`, `helloxx`, `loadable_elf_apps`, `tc`) | no (NXP EVK) | NuttX 7.28 (first import 76/76 files, avg 0.99) | large: 70 files added (i.MX RT1020 support, ADC, clock, USB, 42 NXP SDK headers), LPI2C/eDMA/clock config rewritten (similarity 0.03–0.17), 101 chip commits | **Review**: overwriting would drop most of the TizenRT work |
| `xtensa` (`xtensa`, `lx6`) | `xtensa/src/common`, `lx6` | `esp32_DevKitC`, `esp_wrover_kit` (`STDK`, `st_things`, `security`, `tash_wlan`, `hello_with_tash`, `tc`) | no (Espressif kits, Wi-Fi/SmartThings configs) | NuttX 7.26 (51/51 files, avg 0.95) | FPU enable, Wi-Fi interrupt level, pipe init, SPIRAM init order, type-collision and build fixes | **Review** together with `esp32` |
| `esp32` | `xtensa/src/esp32` | same as `xtensa` | no | NuttX 7.27 for 35 files (avg 0.96) **plus 75 files that are not NuttX** (Espressif/Cadence: RTC, I2S, LEDC, touch, DAC, ...) | I2C, GPIO, serial, SPI flash, LCD drivers, Wi-Fi interrupt; 32 chip commits | **Review**: the Espressif part is not NuttX code |

Cores, taken as decided (the TizenRT-only items are to be handled under
the TizenRT-only rule of
[NuttX-12.13.0-Migration-rtl8730e.md](NuttX-12.13.0-Migration-rtl8730e.md)):

| Core | Used by | Origin | TizenRT changes that the import overwrites |
|---|---|---|---|
| `armv7-m` | `stm32`, `stm32l4`, `stm32h745`, `imxrt`, `tiva` | NuttX ≈7.18 (53/70 files, avg 0.91; 7.8: 46/70, 0.92) | 17 files added: MPU context save/restore and `up_setupregions.c` (protected build, loadable apps), `up_switchcontext.S`, `up_unblocktask_withoutsavereg.c`, `up_schedyield.c`, `up_busfault.c`, `up_trigger_irq.c`, D-cache by range; rewritten: `up_memfault.c` (0.29), `up_assert.c` (0.44), `up_doirq.c`, `up_hardfault.c` (0.58), `up_mpu.c` (0.69) — crash dump, assert, MPU |
| `armv7-r` | `s5j`, `bcm4390x` | NuttX ≈7.18 (48/55 files, avg 0.93) | 7 files added: `arm_gicv2.c`, `gic.h`, `mpcore.h`, MPU context save/restore, `arm_setupregions.c`, `arm_schedyield.c`; rewritten: `arm_assert.c` (0.41), `cache.h` (0.58), the abort handlers (0.71–0.73) |

Summary: **overwrite** `imx6`, `tiva` (QEMU only) and `stm32` (test
boards, no TizenRT feature of its own); **review** `stm32l4`, `imxrt`,
`xtensa`/`esp32`; `stm32h745` is **not NuttX code**.

## QEMU boards and their NuttX names

The two QEMU-only chips are in `build/configs` under board names that
differ from NuttX.  The NuttX `qemu` chip is a third, different thing.

| TizenRT config dir | TizenRT board (`CONFIG_ARCH_BOARD`) | Chip | Configs | NuttX 12.13.0 | NuttX QEMU configs | QEMU machine |
|---|---|---|---|---|---|---|
| `build/configs/qemu` | `lm3s6965-ek` (`os/board/lm3s6965-ek`) | `tiva` (LM3S6965, Cortex-M3) | `build_test`, `smartfs`, `tc_1m`, `tc_16m`, `tc_16m_grpc`, `tc_64k` | `boards/arm/tiva/lm3s6965-ek` + `arch/arm/src/tiva` | `qemu-flat`, `qemu-protected`, `qemu-kostest`, `qemu-nxflat` | `lm3s6965evb` |
| `build/configs/sabre-6quad` | `sabre-6quad` (`os/board/sabre-6quad`) | `imx6` (i.MX6 Quad, Cortex-A9) | `hello`, `hello_smp` | `boards/arm/imx6/sabre-6quad` + `arch/arm/src/imx6` | none by name; the README has "Debugging with QEMU" | `sabrelite` |
| `build/configs/qemu-armv7a` | `qemu-armv7a` | `qemu` (QEMU `virt`, Cortex-A7) | `ostest`, `tash` | `boards/arm/qemu/qemu-armv7a` + `arch/arm/src/qemu` | all | `virt` |

- TizenRT `build/configs/qemu` is the TI LM3S6965 evaluation board run
  on QEMU (port commit f31971989 "port lm3s6965-ek board from NuttX",
  QEMU 2.12 with the RAM size patches of that directory).  Its NuttX
  counterpart is the board `lm3s6965-ek` of the `tiva` chip, not the
  NuttX `qemu` chip.
- The NuttX `qemu` chip (`arch/arm/src/qemu`) is made for the QEMU
  `virt` machine; its boards are `qemu-armv7a` (already taken by the
  migration) and `qemu-armv7r` (Cortex-R5, not in TizenRT).
- Taking `tiva` therefore means `arch/arm/src/tiva` and
  `boards/arm/tiva/lm3s6965-ek`; whether `build/configs/qemu` is renamed
  to `lm3s6965-ek` is decided with that port.

## QEMU per core and Armv7-R

Core (common) arch directories:

- In TizenRT: arm `armv7-a`, `armv8-m`, `armv7-m`, `armv7-r`; xtensa
  `xtensa` (NuttX `common`) and `lx6`.  `armv7-a`, `armv8-m` are synced;
  `armv7-m`, `armv7-r` will be taken (user decision).
- Only in NuttX 12.13.0: `armv6-m`, `armv8-r`, `arm` (ARM7/9), xtensa
  `lx7`.  None of them has a NuttX board that runs on our QEMU (`armv8-r`
  uses the Arm FVP, `fvp-v8r-aarch32`).

A QEMU board per core (review only, nothing ported).  "Our QEMU" is the
QEMU 8.2 of `tizenrt/tizenrt:2.0.1-qemu` (`tools/docker/Dockerfile`),
checked with `-M help` / `-cpu help`:

| Core | NuttX 12.13.0 QEMU board | Our QEMU 8.2 | Result |
|---|---|---|---|
| `armv7-a` | `qemu/qemu-armv7a` (`virt`, Cortex-A7) | yes | done (SMP too); `imx6/sabre-6quad` (`sabrelite`) also possible |
| `armv8-m` | `mps/mps2-an521` (Cortex-M33) | yes | done; `mps/mps3-an547` (Cortex-M55) also possible |
| `armv7-m` | `mps/mps2-an500` (Cortex-M7), `tiva/lm3s6965-ek` (Cortex-M3, `lm3s6965evb`) | yes, both | done: both ported (`DEV-12.13.0-LABEL-ARMV7`) |
| `armv7-r` | `qemu/qemu-armv7r` (`virt`, Cortex-R5) | **no** | done on the OpenVela QEMU fork (`build/configs/qemu-armv7r/Dockerfile`) |
| xtensa `lx6` | `esp32/esp32-devkitc:qemu_openeth` | **no** (no `qemu-system-xtensa`) | needs the Espressif QEMU fork, an esptool flash image (MCUboot format) and the `esp32` chip, which is under review |

Taking `armv7-m` (or `armv7-r`) breaks the TizenRT chips on that core
(`stm32`, `stm32l4`, `stm32h745`, `imxrt`, `tiva`; `s5j`, `bcm4390x`)
until each is adapted, as `amebad` and `armino` on `armv8-m` today.

### Armv7-R on QEMU

- TizenRT never ran Armv7-R on QEMU.  In the whole TizenRT history the
  QEMU commits are for `lm3s6965-ek` (`build/configs/qemu`, from 2017) and
  `sabre-6quad` (2023) only.  The TizenRT `armv7-r` core (Cortex-R4, since
  4fc385da0, 2016) is used by hardware boards: `s5j` (`artik053`,
  `artik053s`, `artik055s`, `sidk_s5jt200`) and `bcm4390x` (`cy4390x`).
- The Cortex-R5 code in TizenRT comes from this migration: the NuttX
  `qemu` chip serves both `qemu-armv7a` and `qemu-armv7r`, and it was
  synced as a whole for `qemu-armv7a` ([SYNC] a8b4c678).  It has
  `ARCH_CHIP_QEMU_CORTEXR5` in `os/arch/arm/src/qemu/Kconfig` and the
  `CONFIG_ARCH_ARMV7R` paths of `Make.defs` (`include armv7-r/Make.defs`)
  and `qemu_boot.c`; it is the same as in NuttX 12.13.0 and is not used
  (`# CONFIG_ARCH_CHIP_QEMU_CORTEXR5 is not set` in `qemu-armv7a`).
- To use it, three things are missing:
  1. the NuttX 12.13.0 `armv7-r` core: the old TizenRT `armv7-r/Kconfig`
     has `ARMV7R_HAVE_GICv2` but no `ARMV7R_HAVE_PTM`, and there is no
     `arm_timer.c` (generic timer) as in NuttX;
  2. a `qemu-armv7r` board (`os/board`, `build/configs`) from NuttX
     `boards/arm/qemu/qemu-armv7r` (`nsh`, `pnsh`);
  3. a QEMU that runs Cortex-R on `virt`.
- QEMU: `tools/docker/Dockerfile` builds the upstream QEMU 8.2 (Ubuntu
  24.04 source package, `--target-list=arm-softmmu --static`).  Upstream
  QEMU has the `cortex-r5f` CPU, but only inside SoC models such as the
  ZynqMP RPU; its `virt` machine accepts only A-profile CPUs:
  `qemu-system-arm -M virt -cpu cortex-r5f` gives `mach-virt: CPU type
  cortex-r5f-arm-cpu not supported`.  This is not a QEMU version problem:
  the NuttX 12.13.0 documentation of `qemu-armv7r` says "The upstream
  QEMU does not support ARM Cortex-R virtual machine" and uses the
  OpenVela QEMU fork (source `open-vela/platform_external_qemu`, prebuilt
  `open-vela/prebuilts_qemu_linux-x86_64`).
- Done (user decision): `build/configs/qemu-armv7r/Dockerfile` builds
  the fork from source (QEMU 10.0.0 based, commit df9bdfe3, static
  `arm-softmmu`) into `/opt/qemu-vela` of the image
  `tizenrt/tizenrt:2.0.1-qemu-armv7r`, beside the upstream QEMU 8.2;
  `tools/docker` is unchanged.  The NuttX 12.13.0 `armv7-r` core and the
  `qemu-armv7r` board are taken, and `os/arch/arm/src/Makefile` selects
  the core directory by architecture (Cortex-R5 was not known).

## Numbers

Average similarity is the mean codesim token similarity of the common
files (1.0 = same code apart from style and the `tinyara`/`nuttx`
names).  "TizenRT-only" are files with no NuttX counterpart, "NuttX-only"
the reverse.

| Unit | NuttX at import | First TizenRT version vs import | Now vs import | Now vs 7.8 | Now vs 11.0.0 |
|---|---|---|---|---|---|
| `armv7-m` | 7.18 | 43/50, 17 identical, 0.93, +7 | 53/70, 26 identical, 0.91, +17 | 46/70, 25 identical, 0.92, +24 | 31/70, 1 identical, 0.72, +39 |
| `armv7-r` | 7.18 | 49/55, 22 identical, 0.94, +6 | 48/55, 20 identical, 0.93, +7 | not in 7.8 | 31/55, 7 identical, 0.86, +24 |
| `imx6` | 12.1.0 (best match 12.0.0: 37/37, 20 identical, 0.995) | 37/37, 29 identical, 0.99 (vs 11.0.0), +0 | 37/37, 19 identical, 0.995, +0 | not in 7.8 | 37/37, 20 identical, 0.99, +0 |
| `imxrt` | 7.28 | 76/76, 54 identical, 0.99, +0 | 71/141, 27 identical, 0.80, +70 | not in 7.8 | 84/141, 2 identical, 0.74, +57 |
| `stm32` | 7.27 | 169/178, 33 identical, 0.88, +9 | 163/173, 28 identical, 0.87, +10 | **172/173, 116 identical, 0.99, +1** | 154/173, 0 identical, 0.81, +19 |
| `stm32l4` | 8.1 | 165/309, 95 identical, 0.98, +144 | 159/305, 85 identical, 0.98, +146 | not in 7.8 | 157/305, 36 identical, 0.95, +148 |
| `tiva` | 7.20 | 78/79, 48 identical, 0.99, +1 | 73/75, 41 identical, 0.97, +2 | 72/75, 41 identical, 0.95, +3 | 52/75, 0 identical, 0.73, +23 |
| `stm32h745` | 10.3.0 (`stm32h7`) | 3/111, 0 identical, 0.58, +108 | 3/113, 0 identical, 0.58, +110 | not in 7.8 | 3/113, 0 identical, 0.58, +110 |
| `xtensa` | 7.26 | 51/51, 18 identical, 0.95, +0 | 51/51, 16 identical, 0.95, +0 | not in 7.8 | 45/51, 5 identical, 0.78, +6 |
| `esp32` | 7.27 | 35/91, 22 identical, 0.98, +56 | 35/110, 21 identical, 0.96, +75 | not in 7.8 | 40/110, 4 identical, 0.64, +70 |

Cells: common/TizenRT files, identical files, average similarity,
`+n` TizenRT-only files.  `stm32` matches NuttX 7.8 better than 7.27,
although it was added to TizenRT in 2018: TizenRT took an older copy.

NuttX releases that first had the chips that 7.8 does not have:
`armv7-r`, `imx6`, `stm32l4` 7.16; `xtensa` (`esp32`, `lx6`) 7.19;
`imxrt` 7.25; `stm32h7` 7.26.

File origin by license header (current TizenRT, `src` only):

| Chip | Files | NuttX header | ST | NXP | Espressif/Cadence | ARM CMSIS | other |
|---|---|---|---|---|---|---|---|
| `armv7-m` | 65 | 57 | | | | | 8 |
| `armv7-r` | 53 | 49 | | | | | 4 |
| `imx6` | 35 | 35 | | | | | |
| `imxrt` | 136 | 92 | | 42 | | | 2 |
| `stm32` | 164 | 164 | | | | | |
| `stm32l4` | 297 | 162 | 98 | | | 5 | 32 |
| `tiva` | 68 | 65 | | | | | 3 |
| `stm32h745` | 108 | 15 | 72 | | | 5 | 16 |
| `xtensa/src/xtensa` | 45 | 45 | | | | | |
| `xtensa/src/esp32` | 89 | 34 | | | 46 | | 9 |

## TizenRT history

From the Samsung/TizenRT history (master 379c33e7, 12618 commits).
"Chip commits" change only the chip and its boards; "tree-wide" commits
also change two or more other areas (API renames, PM or heap clean-ups,
license and style).

| Unit | Taken into TizenRT | Commits since | Chip | Tree-wide |
|---|---|---|---|---|
| `armv7-m`, `armv7-r` | 4fc385da0, 2016-12-23 "The first commit of Tizen RT 1.0 M1" | 152 / 79 | 94 / 43 | 58 / 36 |
| `imx6` | 07faccb1d, 2023-06-06 "os/board/sabre-6quad: To support qemu_armv7a" (ported from NuttX for QEMU) | 8 | 2 | 6 |
| `imxrt` | 4e380dfd8, 2019-02-06 "add nxp_demo application for iMXRT in NXP" | 146 | 101 | 45 |
| `stm32` | 4eb6eb990, 2018-12-13 "arch/arm: port stm32 chip" | 36 | 13 | 23 |
| `stm32l4` | b918193e7, 2019-10-29 "Add stm32l4r9 discovery board bsp (#3954)" | 57 | 27 | 30 |
| `tiva` | f31971989, 2017-03-21 "port lm3s6965-ek board from NuttX" | 45 | 20 | 25 |
| `stm32h745` | 43e88f3b9, 2022-08-12 "os/arch : arch support for stm32h745" | 14 | 5 | 9 |
| `xtensa` | aac492d8a, 2018-11-07 "arch: Add Xtensa architecture code." | 27 | 17 | 10 |
| `esp32` | 1c2f2ac84, 2019-01-18 "arch/xtensa: Add Espressif ESP32 soc/chip code." | 49 | 32 | 17 |

Chip commits that carry TizenRT work (the rest are build, typo and
style fixes):

- `imx6`: FIQ stack by `CONFIG_ARMV7A_DECODEFIQ` (22e40700b).
- `tiva`: flash driver for QEMU (3a3c238bd), lm3s Ethernet driver
  (83152fe1c), Ethernet dependencies (8fa74eac3).
- `stm32`: USB host CDC-ACM spurious interrupt (9cd670a49), USBHOST_CDCACM
  menu (19ba9e119), USB host chip code (ba0f5880b), CCM `xalloc_user_at`
  (b2fc00c3f).
- `stm32l4`: PM — Stop2 clock reconfiguration (4d0f1ca0f), time
  correction during sleep (1b3b0181a), tick disable (9f615bc77), wake-up
  state (675100d54), staycount (d7f5fd721), EXTI PC13 (f598fc228),
  console in idle (252fc037f); heap region check (8d83a457e); ramdump
  (0968ce649); low-level serial (2a87f9ad1).
- `stm32h745`: baud rate from config (040eac208), reset flag (99b930291),
  support files (6305aeff0).
- `imxrt`: 101 commits (drivers, i.MX RT1020, clocks, USB, MPU, loadable
  apps); see `git log -- os/arch/arm/src/imxrt`.
- `xtensa`: FPU (829411c66), Wi-Fi CPU interrupt level (b2f6d020a), pipe
  init (efd60dc74), SPIRAM init order (bae3430d5), type collision
  (16319238b), `abort_mode` (bfb1736ed).
- `esp32`: I2C (8c3a324b2, 34cf8c43a, 4a0bac682, a7b7a2360, 346edec33),
  GPIO (f8a055f44, 57af5a620, 2bb49996f), serial (ab4c65d41), CPU
  interrupts (d1de74769), OS tick (0fa0b1f32), SPI flash (b3204e36c),
  LCD (0267b7a40), partition lock (3a64837ec).

## Method

- NuttX: apache/nuttx tags `nuttx-7.8`, `nuttx-11.0.0` and the release
  current when TizenRT took each directory (`nuttx-7.18`, `7.20`,
  `7.26`, `7.27`, `7.28`, `8.1`, `10.3.0`, `12.1.0`), `arch/` of each
  tag; for `imx6` also `12.0.0` and `12.2.0`.
- TizenRT: the current files of `DEV-12.13.0-LABEL-RTL8730E` (these
  directories are unchanged from TizenRT master) and the files of the
  commit that first added each directory.
- Comparison: the file comparison of codesim
  (`tizenrt-nuttx-similarity/core`, `compare.compare_files`, renames
  `tinyara`→`nuttx`), per directory pair (`stm32h745`↔`stm32h7`,
  `xtensa/src/xtensa`↔`xtensa/src/common`).  Files are paired by path,
  then by the NuttX renames `up_`↔`arm_`/`xtensa_` and `chip/`↔`hardware/`,
  then by a unique file name.  `.c`, `.h` and `.S` files; Kconfig and
  makefiles are not compared.
- `up_*` functions renamed to `arm_*` in NuttX 10 and later count as
  changes, so the 11.0.0 numbers of the cores are lower than the real
  difference of the code.
- Spot checks by hand (`diff -w`): `imx6/imx_enet.c` against 12.0.0
  differs only by the include names (`up_internal.h`); against 11.0.0 it
  also has the newer NuttX descriptor and buffer layout, so imx6 was taken
  from NuttX 12.0 or later.  `stm32/stm32_idle.c` against 7.8 lost the
  chip PM idle code (TizenRT PM clean-up).
- Product use is judged from the board configurations in
  `build/configs` (QEMU README and port commits); whether a board is used
  in a shipped product is not visible in the tree.
