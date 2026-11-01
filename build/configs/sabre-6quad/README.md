# sabre-6quad

TizenRT on the QEMU `sabrelite` machine: NXP i.MX6 Quad (4 × Cortex-A9,
GICv2, SCU), UART1 console.  The chip and the board come from NuttX
12.13.0 (`arch/arm/src/imx6`, `boards/arm/imx6/sabre-6quad`;
`os/arch/arm/src/imx6`, `os/board/sabre-6quad`).  FLAT build only.  The
board is used only on QEMU.

## Configurations

| Config | CPUs | What it does |
|---|---|---|
| `ostest` | 4 (SMP) | Runs the NuttX OS test (`apps/examples/testcase/ostest`) as the entry point (`CONFIG_INIT_ENTRYPOINT="ostest_main"`), including the SMP `smp_call` test. |
| `tash` | 4 (SMP) | Starts TASH (`CONFIG_INIT_ENTRYPOINT="preapp_start"`, `apps/system/init`) with the system and FS commands, the `ostest` command and the `smp` command (`apps/examples/smp`). |
| `hello` | 1 | TASH and the hello example: `preapp_start` starts TASH and then the application entry point `hello_main` (`CONFIG_ENTRY_HELLO`, `CONFIG_USER_ENTRYPOINT`), as the old TizenRT `os_bringup` did; `hello` is also a TASH command. |
| `hello_smp` | 4 (SMP) | `hello` with SMP and the `smp` command. |

With SMP, `CONFIG_SMP_NCPUS=4` and `CONFIG_NCPUS=4`.  At boot CPU0
releases the other cores from reset through the i.MX6 SRC
(`imx_cpu_enable()` in `imx_cpuboot.c`); `up_cpu_start()` is the SGI
version of `armv7-a/arm_cpustart.c`.

The PL310 L2 cache controller (`CONFIG_ARMV7A_L2CC_PL310`) is off: QEMU
does not model its registers correctly and the debug assertions would
stop the system (NuttX sabre-6quad README, "Debugging with QEMU").
There is no `board_power_off()`, so the `ostest` configuration does not
power QEMU off at the end.

## Build

The `tizenrt/tizenrt:2.0.1-qemu` docker image adds QEMU to the TizenRT
build image (see `tools/docker`).  `CONFIG_DOCKER_VERSION="2.0.1-qemu"`
makes `dbuild.sh` use it.

```bash
docker build -t tizenrt/tizenrt:2.0.1-qemu tools/docker
cd os
./tools/configure.sh sabre-6quad/ostest    # or tash, hello, hello_smp
./dbuild.sh build
```

## Run

```bash
docker run --rm -it -v $PWD:/root/tizenrt tizenrt/tizenrt:2.0.1-qemu \
	/root/tizenrt/build/configs/sabre-6quad/qemu_run.sh
```

`qemu_run.sh` runs:

```bash
qemu-system-arm -M sabrelite -m 1024 -smp 4 -nographic \
	-chardev stdio,id=con,mux=on -serial chardev:con \
	-mon chardev=con,mode=readline -kernel build/output/bin/tinyara
```

`sabrelite` always has 4 CPUs (`-smp 4` also for `hello`).

The `ostest` configuration passes when the log ends with

```
smp_call_test: Test success
...
user_main: Exiting
ostest_main: Exiting with status 0
```

With the `tash` configuration, type `ostest` at the `TASH>>` prompt; the
same lines show the result.  `smp` runs the SMP example (barrier threads
on all CPUs) and `ps` lists `CPU0 IDLE` to `CPU3 IDLE`.  Quit QEMU with
`Ctrl-a x`.
