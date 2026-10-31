# NuttX 12.13.0 Migration: rtl8730e

This document describes the port of rtl8730e (Realtek AmebaSmart, the
Cortex-A32 application processor) to the NuttX 12.13.0 code.  It also
keeps the reviews made during the port and the log of the differences
that exist only because of the old TizenRT code.  The general migration
is described in [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md).

- Branch: `DEV-12.13.0-LABEL-RTL8730E` (from `DEV-12.13.0-LABEL`).
- Configuration: `rtl8730e/tash`.  The other rtl8730e configurations
  (`flat_apps`, `loadable_*`, `flat_dev_*`) are not updated.
- Scope (the one of the QEMU ports): boot, TASH and the `ostest`
  command.  Single core, UART4 console, TrustZone.
- Not in the scope: SMP (planned next, see [SMP](#smp)), `CONFIG_PM`,
  network, Wi-Fi, BLE, SE, flash partitions, MTD/FTL, SmartFS, the
  other peripherals, the TizenRT kernel extras (binary manager, log
  dump, heapinfo, memory leak checker), the socket/VFS glue and the KM4
  log path.
- Verification: build and Realtek post-build images only.  **The port
  has not been run on hardware.**

## Rules

1. **Code taken from NuttX 12.13.0 / nuttx-apps is not changed.**  This
   covers every file of the `[SYNC]` commits and the files changed by
   `[MODI_NX]` commits (for example `os/arch/arm/src/armv7-a`,
   `os/arch/arm/src/common`, `os/kernel`, `os/include`, the taken
   Kconfig and makefiles).  If a change looks necessary, the user is
   asked first.
2. **rtl8730e adapts to the NuttX HAL**, not the other way round.  The
   chip (`os/arch/arm/src/amebasmart`) implements the hooks that the
   NuttX armv7-a code calls, like the NuttX chips (`qemu`, `imx6`).
   Chip Kconfig selects go into `os/arch/arm/src/amebasmart/Kconfig`.
3. **A fix that is needed only because of the old TizenRT code is its
   own commit**, never mixed with other work.  This covers the
   adaptations that the old TizenRT made inside the code that NuttX now
   replaces (for example its Cortex-A32 changes in the old armv7-a
   files), and the TizenRT-only APIs and options that NuttX 12.13.0
   does not have (for example `up_cpu_pause_all()`, `CPU_HOTPLUG`,
   `kregionx`).  The analysis of each item (old TizenRT code, NuttX
   12.13.0 code, evidence, decision, commit) is written in
   [TizenRT-only differences](#tizenrt-only-differences).  This rule
   applies from the SMP work on; the items found before it are listed
   with the commit that handled them.

Files that may be changed: `os/arch/arm/src/amebasmart/**`,
`os/arch/arm/include/amebasmart/**`, `os/board/rtl8730e/**`,
`build/configs/rtl8730e/**`, the documents.

## Commits

| Commit | Label | Content |
|---|---|---|
| `246f38d0` | `[MODI_TR]` | amebasmart chip on the NuttX armv7-a code |
| `3a59a158` | `[MODI_TR]` | rtl8730e board and Realtek SDK on the NuttX 12.13.0 kernel |
| `0c893140` | `[MODI_TR]` | `-mcpu=cortex-a32`; NuttX start-up code in PSRAM (linker script) |
| `cb50553c` | `[MODI_TR]` | `rtl8730e/tash` configuration |
| `324a81cd`, `14e73d5d` | `[DOC]` | migration document, HANDOFF |

## Build

```bash
cd os
./tools/configure.sh rtl8730e/tash   # refuses a tree that is already built
make                                  # sequential top-level make, like dbuild.sh
```

A top-level `make -j` starts the Realtek post-build before the link.
The build runs in the `tizenrt/tizenrt:2.0.1` image (gcc 10.3.1).
Result: `build/output/bin/tinyara.axf` and the post-build images,
`km0_km4_ap_image_all.bin` among them.

The defconfig is a full `.config`, as for the QEMU configurations.  It
was made from the OS and application options of `qemu-armv7a/tash` and
the chip and board options of `rtl8730e/flat_apps`, then run through
`olddefconfig`.  Main values: `CONFIG_RAM_START=0x60100000`,
`CONFIG_RAM_SIZE=7340032` (PSRAM), `CONFIG_UART4_SERIAL_CONSOLE=y`,
`CONFIG_AMEBASMART_TRUSTZONE=y`, `CONFIG_ARCH_IRQPRIO=y`,
`CONFIG_ARCH_INTERRUPTSTACK=2048`, `CONFIG_USEC_PER_TICK=1000`, no SMP.

## NuttX HAL hooks

| Hook (caller) | amebasmart |
|---|---|
| `arm_boot()` (`armv7-a/arm_head.S`) | `amebasmart_boot.c`: maps the I/O and flash regions (`amebasmart_setupmappings()`) and the vectors, FPU, VBAR = `_vector_start`, then the Realtek `app_start()` (SoC set-up); returns to `arm_head.S`, which calls `nx_start()` |
| `up_irqinitialize()` | `amebasmart_irq.c`: NuttX GICv2 (`arm_gic0_initialize()`, `arm_gic_initialize()`) and the per-CPU interrupt stacks |
| `up_timer_initialize()` | `amebasmart_timerisr.c`: the generic **virtual** timer of the chip (CNTV, PPI 27), one tick per `CONFIG_USEC_PER_TICK`, ISR calls `nxsched_process_timer()` (see [System tick](#system-tick)) |
| `arm_serialinit()`, `up_putc()`, `up_lowputc()` | `amebasmart_serial.c` with the NuttX 12.13 `uart_ops_s` (`ioctl(struct file *, ...)`); CR/LF comes from `CONFIG_SYSLOG_CRLF` |
| `up_allocate_heap()` (weak in NuttX) | `amebasmart_allocateheap.c`: the PSRAM from `_psram_heap_start` (end of the image) to the IDLE stack |
| `board_late_initialize()` | `rtl8730e_boot.c` (was the TizenRT `board_initialize()`); needs `CONFIG_BOARD_LATE_INITIALIZE` |
| `board_app_initialize(uintptr_t)` | `rtl8730e_boot.c`, returns `OK` |

Kconfig (`amebasmart/Kconfig`): `ARCH_CHIP_RTL8730E` selects
`ARCH_ARMV7A` (the TizenRT `ARCH_CORTEXA32` entry in the taken
`os/arch/arm/Kconfig` does not), `ARCH_HAVE_MULTICPU` and
`ARMV7A_HAVE_GICv2`; the UARTs select `UARTn_SERIALDRIVER` (TizenRT had
`ARCH_HAVE_UARTn`); `AMEBASMART_WIFI` is no longer selected by the
chip.

## Memory layout

`hardware/amebasmart_memorymap.h` provides what the NuttX `arm_head.S`
needs: `NUTTX_TEXT_*` and `NUTTX_RAM_*` (renamed from `TIZENRT_*`),
`PGTABLE_BASE_*` and `IDLE_STACK_BASE`.

| Item | Address |
|---|---|
| PSRAM (`CONFIG_RAM_*`) | 0x60100000 - 0x607fffff |
| `_vector_start` | 0x60100000 |
| `__start` (`arm_head.S`) | 0x60100060 (PSRAM) |
| page table | 0x607fc000 (end of the PSRAM) |
| IDLE stack (`g_idle_topstack`) | below the page table, top 0x607fc000 |
| heap | `_psram_heap_start` .. IDLE stack |
| text | XIP flash, except the code below |

`arm_head.S` runs with only the PSRAM mapped; the flash is mapped by
`arm_boot()`.  `build/configs/rtl8730e/scripts/rlx8730e_img2.ld`
therefore keeps `*arm_head.o (.text*)` and `*arm_mmu.o (.text*)` in the
PSRAM `.code` section (`EXCLUDE_FILE` on the flash text) and defines
`_stext`/`_etext`.  NuttX puts `__start` in `.text` (the old TizenRT
used `.start`).

## Realtek SDK adaptations

| Item | Change |
|---|---|
| Types | NuttX `sys/types.h` has no `u8`/`u16`/`u32`/`__kernel_size_t`: `basic_types.h` and `osdep_service.h` define them |
| Lists | `dlist.h` includes `<tinyara/list.h>` first and drops its `list_add_tail`, `list_entry`, `list_first_entry` macros |
| Atomics | NuttX `atomic_t` (`int32_t`) clashes with the Realtek `atomic_t` struct: the Realtek type is `osdep_atomic_t` (`ATOMIC_T`) |
| Headers | `<tinyara/queue.h>`, `<time.h>` (no `tinyara/time.h`), `<arch/barriers.h>` (`UP_DSB`/`UP_ISB` for `ARM_DSB`/`ARM_ISB`) |
| OS wrapper | `kthread_create()` (`pid < 0` check), `nxsched_get_tcb()`, `nxsched_set_param()`; `vsyslog()` returns `void` |
| Heap | `os_heap_init()` (TizenRT `kregionx_*`) removed from `ameba_app_start.c`; the heap comes from `up_allocate_heap()` |
| Options | Wi-Fi, lwip, inic, kv, vfs2.0 only with `CONFIG_AMEBASMART_WIFI`; FTL with `CONFIG_AMEBASMART_FTL`; KM4 log task with `CONFIG_AMEBASMART_BLE`; `i2s_api.c` with `CONFIG_AMEBASMART_I2S`; flash partitions with `CONFIG_FLASH_PARTITION` |
| Assert | `assert_param()` calls `__assert()` |

## Reviews

### Atomics, spinlocks and barriers

Result: correct for the single-core `rtl8730e/tash`; nothing to change.

- NuttX atomics (`<tinyara/atomic.h>`): `CONFIG_LIBC_ARCH_ATOMIC` is not
  set, so the gcc `<stdatomic.h>` is used, as on qemu-armv7a.  With
  `-mcpu=cortex-a32` gcc inlines LDREX/STREX (LDREXD/STREXD for 64
  bit).  Without `-mcpu` the build failed ("Thumb-1 hard-float VFP
  ABI"), so this flag is required.  The `nx_atomic_*` fallbacks of
  `libs/libc/machine/arch_atomic.c` are built but not used.
- Test-and-set: `CONFIG_ARCH_HAVE_TESTSET=y` without a custom one, so
  `up_testset()` is the inline LDREXB/STREXB + DMB of
  `os/arch/arm/include/spinlock.h`.  `CONFIG_SPINLOCK` is off (single
  core).  The LOGUART spinlock of `amebasmart_serial.c` is only built
  with `CONFIG_SMP`.
- `CONFIG_ARCH_HAVE_FETCHADD=y` (selected by the TizenRT
  `ARCH_CHIP_AMEBASMART` entry) builds the NuttX `arm_fetchadd.S`.
  NuttX 12.13.0 has no user of it.
- Memory: the PSRAM is mapped cacheable (`MMU_MEMFLAGS`) and
  `arm_head.S` enables the D-cache when SMP is off, so the exclusive
  accesses work.
- Realtek `ATOMIC_T` (`osdep_atomic_t`, same `volatile int counter`
  layout as before): its functions use `enter_critical_section()`.  No
  Realtek source outside `osdep_service_atomic.c` and no prebuilt
  library (`lib_wifi_common.a`, `lib_wpa_lite.a`, `lib_btgap.a`, checked
  with `nm -u`) uses them, so there is no ABI change.
  `osdep_service.h` redefines `atomic_read()`/`atomic_set()` after the
  NuttX headers.  A wrong expansion would not compile (struct against
  `int32_t`), and the build has no "redefined" warning.  Like the list
  macros, this depends on the include order; check it again when Wi-Fi
  or SMP add files.
- Barriers: the Realtek `ARM_DSB`/`ARM_ISB` became `UP_DSB`/`UP_ISB`.

### System tick

NuttX 12.13.0 has a timer layer in which the chip writes no tick ISR,
and which also attaches the IRQ (qemu-armv7a uses it):

1. `armv7-a/arm_timer.c`: the ARM generic timer as a
   `oneshot_lowerhalf_s`.  `arm_timer_initialize()` calls `irq_attach()`
   and `up_enable_irq()`; the ISR disables the compare
   (`UINT64_MAX`) and calls `oneshot_process_callback()`.
2. `drivers/timers/arch_alarm.c` (and `oneshot.c` for the count/tick
   conversion): `up_alarm_set_lowerhalf()` installs the callback, which
   calls `nxsched_process_timer()` for each elapsed tick and re-arms
   `now + 1`.  It also provides `up_timer_gettime()`,
   `up_timer_gettick()`, `up_udelay()`/`up_ndelay()` and the tickless
   API.
3. `kernel/sched/sched_processtick.c`: `nxsched_process_timer()`
   (system time, round robin, `wd_timer()`).

The chip code is one line (`qemu/qemu_timer.c`:
`up_alarm_set_lowerhalf(arm_timer_initialize(0))`), with
`ARMV7A_HAVE_PTM` (selects `ONESHOT`) and `CONFIG_ALARM_ARCH=y`.

rtl8730e does not use this layer.  `amebasmart_timerisr.c` attaches its
own ISR to the virtual timer (CNTV, PPI 27), computes the elapsed ticks,
calls `nxsched_process_timer()` for each and sets the next compare
value.  This is the same work as the `arch_alarm.c` callback.
`arm_timer.c` cannot be used as it is: it is fixed to the physical
timer (CNTP, IRQ 30, or 29 with `CONFIG_ARCH_TRUSTZONE_SECURE`), and
`arm_timer_initialize(0)` expects the boot loader to have set CNTFRQ
(the Realtek code uses a fixed 50 MHz).  Whether the non-secure side can
use CNTP and what CNTFRQ holds cannot be checked without hardware.

**Decision (user): keep the chip ISR.**  If tickless mode or the
oneshot-based `up_udelay()` is needed later, amebasmart can provide a
CNTV `oneshot_lowerhalf_s` (same shape as `arm_timer.c`) and call
`up_alarm_set_lowerhalf()`, without changing a NuttX file.

### Cortex-A32 and the old TizenRT armv7-a code

The NuttX armv7-a code knows Cortex-A5/A7/A8/A9/A15/A17, not
Cortex-A32.  The old TizenRT armv7-a code (base commit `9455814169ce`)
had these Cortex-A32 changes:

| Old TizenRT | NuttX 12.13.0 | Effect on rtl8730e |
|---|---|---|
| `mpcore.h`: `MPCORE_ICC_OFFSET` 0x2000 for `CORTEXA7 \|\| CORTEXA32` | 0x2000 only for `CORTEXA7`, else 0x0100 | **Bug in the single-core port**: the GIC CPU interface is taken at 0xA0100100 instead of 0xA0102000, so interrupts do not work on hardware.  Not seen by the build.  See [TD-1](#td-1-gic-cpu-interface-offset) |
| `arm_scu.c` `arm_enable_smp()`: no SCU writes and no ACTLR write for Cortex-A32 | SCU writes and ACTLR.SMP for every core | SMP only.  See [TD-2](#td-2-scu-writes-in-arm_enable_smp) |
| `arm_head.S`, `arm_cpuhead.S`: no TTBR1 write, no NOPs, no `SCTLR_Z`, does not clear `SCTLR_SW/RR/HA` | does all of them | No effect: TTBR1 is not used (TTBCR.N=0), and on ARMv8 AArch32 `SCTLR.Z` is RES1 and `SW`/`RR`/`HA` are RES0 |
| `Toolchain.defs`: `-mcpu=cortex-a32` | no Cortex-A32 | Done in `build/configs/rtl8730e/Make.defs` (`0c893140`) |
| `arm_signal_dispatch.c`, `arm_signal_handler.S`: SP alignment for Cortex-A32 | files do not exist (NuttX signal code) | None |
| `include/syscall.h`: `CORTEXA9 \|\| CORTEXA32` | selects by architecture | None |

Evidence:

- Realtek GIC addresses (`os/board/rtl8730e/src/component/soc/amebad2/fwlib/ap_core/sheipa.h`):
  `GIC_DIST_BASE 0xA0101000`, `GIC_CPU_BASE 0xA0102000`.  amebasmart:
  `CHIP_MPCORE_VBASE = AMEBASMART_GIC_VBASE = 0xA0100000` (32 KB
  GIC-400: 0x0000-0x0fff reserved, 0x1000 distributor, 0x2000 CPU
  interface).
- `armv7-a/mpcore.h` defines the offsets with `#ifndef`, so a chip may
  override them before the include (qemu `chip.h` sets
  `MPCORE_ICC_OFFSET 0x10000`).
- The Realtek FreeRTOS start-up code
  (`fwlib/ap_core/startup.S`, "write to ACTLR") sets ACTLR bit 6 (SMP)
  and bit 0 on each core, then enables the MMU and the caches.  It does
  not write SCU registers.

## TizenRT-only differences

One entry per item that exists only because of the old TizenRT code
(rule 3).

### TD-1 GIC CPU interface offset

- Old TizenRT: `armv7-a/mpcore.h` gave 0x2000 to Cortex-A32 too.
- NuttX 12.13.0: 0x2000 only for Cortex-A7; Cortex-A32 gets 0x0100.
- Decision: amebasmart `chip.h` defines `MPCORE_ICC_OFFSET 0x2000`
  before the NuttX header (no NuttX file changed).
- Commit: own `[MODI_TR]` commit, next on the branch.

### TD-2 SCU writes in arm_enable_smp

- Old TizenRT: `arm_scu.c` skipped, for Cortex-A32, the SCU invalidate
  and SCU enable writes and the ACTLR write; CPUn set `SCTLR.C` itself.
  Cortex-A32 has no memory-mapped SCU.
- NuttX 12.13.0: `arm_enable_smp()` writes `SCU_INVALIDATE` and
  `SCU_CTRL` at `CHIP_MPCORE_VBASE + 0x0`/`0xc` (on rtl8730e:
  0xA0100000/0xA010000C, the reserved start of the GIC-400), sets
  ACTLR.SMP and `SCTLR` C/I/M; CPUn waits for its IDLE task and calls
  `up_update_task()`.  `arm_head.S`/`arm_cpuhead.S` leave the caches
  off with `CONFIG_SMP` until this call.
- Whether the GIC-400 ignores the writes to its reserved area cannot be
  checked without hardware.  The ACTLR write is done by the Realtek
  start-up code too.
- Decision (user): amebasmart gets its own `amebasmart_enable_smp(cpu)`:
  the NuttX `arm_enable_smp()` without the SCU writes.  The chip boot
  code calls it instead of `arm_enable_smp()`.  The NuttX `arm_scu.c`
  stays unchanged (built, not called by amebasmart).
- Commit: own `[MODI_TR]` commit in the SMP work.

### TD-3 CPU hot-plug

- Old TizenRT: `CONFIG_CPU_HOTPLUG` (selected with `SMP && PM`),
  `up_cpu_up()`, `up_cpu_down()`, `up_cpu_die()` in
  `amebasmart_smp.c`, `arm_cpuoff.c`.  `rtl8730e/loadable_ext_*` set
  `CONFIG_CPU_HOTPLUG=y`.
- NuttX 12.13.0: no `CPU_HOTPLUG` option and no such API.
- Decision: not used without PM.  In the SMP work the amebasmart
  hot-plug functions go under `CONFIG_CPU_HOTPLUG` (never defined in
  NuttX 12.13.0), in their own commit.  To be ported with PM.

### TD-4 Pausing the other CPU during XIP flash writes

- Old TizenRT: `FLASH_Write_Lock()` in `ameba_flash_ram.c` calls
  `up_cpu_pause_all()`/`up_cpu_resume_all()` with SMP so that the other
  core does not fetch from the flash while it is programmed.  The old
  linker script kept `arm_cpupause.o`, `arm_testset.o` and
  `spinlock.o` in PSRAM for this.
- NuttX 12.13.0: no CPU pause API (`nxsched_smp_call*()` exists).
- `rtl8730e/tash`: the code is only built with `ARM_CORE_CA32 &&
  CONFIG_XIP_FLASH`, and `CONFIG_XIP_FLASH` is not defined, so nothing
  is needed now.
- To do with MTD/FTL on SMP: an amebasmart pause/resume (for example a
  `nxsched_smp_call_async()` handler that spins in PSRAM with the
  interrupts off).

### TD-5 Heap regions

- Old TizenRT: `os_heap_init()` in `ameba_app_start.c` with
  `kregionx_*`.
- NuttX 12.13.0: no `kregionx`; the heap comes from `up_allocate_heap()`.
- Done in `3a59a158` (before rule 3): `os_heap_init()` removed,
  `amebasmart_allocateheap.c` added.

### TD-6 TizenRT GIC additions

- Old TizenRT: `up_irq_is_pending()`, `up_set_pending_irq()`,
  `up_clear_pending_irq()`, `arm_gic_freq_switch()`,
  `arm_gic_freq_restore()` in its armv7-a GIC code.
- NuttX 12.13.0: not there.
- `rtl8730e/tash`: only called by Realtek code that is not linked
  (`osdep_service_misc.c`, `ameba_rom_patch.c`).  To be provided by
  amebasmart when that code is needed.

### TD-7 Kconfig entries

- Old TizenRT: `ARCH_CORTEXA32` implied the armv7-a code; UARTs selected
  `ARCH_HAVE_UARTn`.
- NuttX 12.13.0: `ARCH_ARMV7A` must be selected; UARTs select
  `UARTn_SERIALDRIVER`.
- Done in `246f38d0` (before rule 3), in `amebasmart/Kconfig`.

## SMP

Planned next on this branch.  The analysis so far:

- Options of `rtl8730e/loadable_ext_*`: `CONFIG_SMP=y`,
  `CONFIG_SMP_NCPUS=2`, `CONFIG_SPINLOCK=y`, `CONFIG_IRQCOUNT=y`,
  `CONFIG_CPU_HOTPLUG=y`.  In NuttX 12.13.0 `SMP` selects `SPINLOCK`
  and `IRQCOUNT` and needs `ARCH_HAVE_MULTICPU`, `ARCH_HAVE_TESTSET`
  and `ARCH_INTERRUPTSTACK != 0`.  `CPU_HOTPLUG` does not exist
  ([TD-3](#td-3-cpu-hot-plug)).
- SMP tests ("smptc"): the ostest `smp_call` test (`smp_call.c`, built
  with `CONFIG_SMP`, run with `CONFIG_BUILD_FLAT`) and the TASH `smp`
  command (`apps/examples/smp`, `CONFIG_TESTING_SMP`, as in
  `rtl8730e/flat_apps`).
- qemu-armv7a first: the NuttX qemu chip already has the SMP code
  (`qemu_cpuboot.c`: `arm_cpu_boot()` → `nx_idle_trampoline()`;
  `qemu_boot.c`: `up_cpu_start()` = PSCI `CPU_ON` at `__start`).  Only
  the defconfigs and `qemu_run.sh` (`-smp`) change.
- CPU1 start on rtl8730e:

  | Step | Old TizenRT | NuttX 12.13.0 |
  |---|---|---|
  | start | `up_cpu_on()` → `up_cpu_up()` | `up_cpu_start()` (weak SGI version in `arm_cpustart.c`; a chip overrides it, like qemu) |
  | power | `cpu_core_poweron()` (`rtk_core1_power_on()`) | same, from amebasmart |
  | entry | Realtek `psci_cpu_on(1, __cpu1_start)` with the `psci_affinity_info()` wait | same; `__cpu1_start` is in the NuttX `arm_cpuhead.S` |
  | C entry | `arm_cpu_boot()`: GIC, VBAR, IRQ on, then WFI until SGI1 | `arm_cpu_boot()` must end with `nx_idle_trampoline()` (qemu pattern) |
  | first task | SGI1 → `arm_start_handler()` restores the IDLE context | `nx_idle_trampoline()` |

- PSCI: the Realtek `psci.c`/`smccc_call.S` stay (also used by PM).  The
  NuttX `CONFIG_ARM_PSCI` stays off because `arm_cpu_psci.c` defines
  `psci_cpu_on()` too.
- Linker: `__cpu1_start` runs with the MMU off, so `arm_cpuhead.o` goes
  to the PSRAM `.code` section like `arm_head.o`.  `g_cpu1_idlestack`
  is in `.noinit`, which is in PSRAM.
- Coherency: [TD-2](#td-2-scu-writes-in-arm_enable_smp).  XIP flash
  writes: [TD-4](#td-4-pausing-the-other-cpu-during-xip-flash-writes).

## Known gaps

- Not run on hardware.
- SMP not done yet (see above).
- PM, network, Wi-Fi, BLE, SE, flash partitions, MTD/FTL, SmartFS, the
  other peripherals, the KM4 log task (it uses the BLE OS layer) and the
  KM4 IPC log print (Wi-Fi inic API) are not ported.
- [TD-4](#td-4-pausing-the-other-cpu-during-xip-flash-writes) and
  [TD-6](#td-6-tizenrt-gic-additions) are open.
- The other rtl8730e configurations and `rlx8730e_img2_ddr.ld` are not
  updated.
- Remaining warnings come from code that was already there: `fscmd.c`
  format warnings and unused variables in the Realtek code.  Implicit
  declarations remain only for Realtek ROM functions and for the
  TD-6 functions, which are not linked.
