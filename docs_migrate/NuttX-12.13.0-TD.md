# NuttX 12.13.0 마이그레이션 TD (후속 조치 목록)

| 항목 | 내용 |
|---|---|
| 작성일 | 2026-11-02 |
| 대상 브랜치 | `DEV-12.13.0-LABEL` |
| 출처 | [NuttX-12.13.0-Leftover-Files-Review.md](NuttX-12.13.0-Leftover-Files-Review.md) (잔존 파일 리뷰: os/libs, os/include), [NuttX-12.13.0-Unsynced-Files-Review.md](NuttX-12.13.0-Unsynced-Files-Review.md) (미sync 파일 리뷰: NuttX include, libs) |
| 함께 볼 문서 | [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md)의 Known gaps (다른 보드, protected/kernel build, 옮겨진 NuttX 헤더 사용처, `NFILE_DESCRIPTORS` 사용처 등) |

잔존 파일 리뷰와 미sync 파일 리뷰에서 삭제만으로 끝나지 않는 항목, 빠뜨린 sync, sync 경계에서 생긴 문제를 모았습니다.
리뷰 영역이 늘어나면 `TD-<영역>-NN`으로 추가합니다.
Migration.md의 Known gaps에 이미 있는 내용은 여기에 다시 적지 않습니다.

종류:
- **별도 판정 필요**: 삭제가 유력하지만 기능 폐기나 테스트 수정이 따르므로 따로 결정할 항목. 결정 전에는 파일을 지우지 않습니다.
- **조치 필요**: 파일은 필요하지만 sync로 빌드 연결·구현·API가 어긋나 수정이 필요한 항목.

## 요약

| ID | 제목 | 대상 | 종류 | 상태 |
|---|---|---|---|---|
| TD-LIBS-01 | ELF cache 빌드 연결 소실 | `os/libs/libc/elf/libelf_cache.c` | 조치 필요 | 열림 |
| TD-LIBS-02 | hashmap 빌드 연결 소실 | `os/libs/libc/misc/lib_hashmap.c`, `os/include/tinyara/hashmap.h` | 조치 필요 | 열림 |
| TD-LIBS-03 | Samsung CRC16-CCITT API 존폐 | `os/libs/libc/misc/lib_crc16_ccitt.c` | 별도 판정 필요 | 열림 |
| TD-LIBS-04 | Bessel 함수 존폐 | `os/libs/libm/libm/lib_{j0,j0f,j1,j1f,jn,jnf}.c` | 별도 판정 필요 | 열림 |
| TD-LIBS-05 | zoneinfo ROMFS 빌드 깨짐, mk_opt_tzrule.py 존폐 | `os/libs/libc/zoneinfo/Makefile`, `mk_opt_tzrule.py` | 조치 필요 + 별도 판정 필요 | 열림 |
| TD-LIBS-06 | armv7-m/armv7-r/xtensa machine 미sync | `os/libs/libc/machine/arm/armv7-{m,r}/`, `libs/libc/machine/xtensa/` | 조치 필요 | 진행 중 (armv7-m/r: DEV-12.13.0-LABEL-ARMV7) |
| TD-LIBS-07 | ARM setjmp 빌드 조건 변경 | `CONFIG_ARCH_SETJMP_H`, setjmp 사용처 | 조치 필요 | 열림 |
| TD-LIBS-08 | libstdc++ 미sync로 `CONFIG_LIBSUPCXX` 빌드 불가 | NuttX `libs/libxx/libstdc++/`, `os/libs/libxx/Makefile` | 별도 판정 필요 (라이선스) | 열림 |
| TD-LIBS-09 | libc kbd/slcd 코덱의 헤더 미sync | `os/libs/libc/misc/lib_{kbd,slcd}{en,de}code.c` | 조치 필요 | 열림 |
| TD-LIBS-10 | `lib_crypt_r.c`의 `crypto/cryptodev.h` 미sync | `os/libs/libc/unistd/lib_crypt_r.c` | 조치 필요 (crypto sync 때) | 열림 |
| TD-INC-01 | Samsung gpio upper-half 구현 소실 | `os/include/tinyara/gpio.h`, `os/drivers/ioexpander/gpio.c` | 조치 필요 | 열림 |
| TD-INC-02 | 오디오 영역 부분 sync 불일치 (i2schar.c, libc/audio) | `os/include/tinyara/audio/{i2s,audio}.h`, `os/drivers/i2s/i2schar.c`, `os/libs/libc/audio/lib_buffer.c` | 조치 필요 | 열림 |
| TD-INC-03 | tools/nxfuse가 없어진 헤더 참조 | `tools/nxfuse/` | 조치 필요 | 열림 |
| TD-INC-04 | 미sync 영역의 미사용 NuttX 헤더 | `power/relay.h`, `video/rgbcolors.h`, `net/{gmii,cs89x0,encx24j600}.h` | 조치 필요 (영역 sync 때) | 열림 |
| TD-INC-05 | 구현 없는 iotdev 헤더 존폐 | `os/include/tinyara/iotdev.h` | 별도 판정 필요 | 열림 |
| TD-INC-06 | smartfs 완료 후 smart_procfs.h 재검토 | `os/include/tinyara/fs/smart_procfs.h` | 조치 필요 (smartfs 완료 후) | 열림 |
| TD-INC-07 | sync된 드라이버·헤더의 미sync 헤더 의존 | `uart_bth4/5`, `optee_socket.c`, `tinyara/coresight/coresight.h` | 조치 필요 | 열림 |
| TD-DOC-01 | 마이그레이션 문서의 sync 범위가 LABEL과 다름 | `docs_migrate/{NuttX-12.13.0-Migration.md,NuttX-12.13.0-Migration-Report.md,HANDOFF.md}` | 조치 필요 | 열림 |
| TD-BUILD-01 | QEMU 보드에 `MKDEP` 정의 없음 (`.depend` 실패) | `build/configs/{qemu-armv7a,mps2-an521}/Make.defs`, `os/tools/Config.mk` | 조치 필요 | 열림 |

## TD-LIBS-01 ELF cache 빌드 연결 소실

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/elf/libelf_cache.c` (Samsung 2019) |
| 관련 config | `CONFIG_ELF_CACHE_READ`, `CONFIG_ELF_CACHE_BLOCK_SIZE`, `CONFIG_ELF_CACHE_BLOCKS_COUNT` |
| 사용 defconfig | `rtl8721csm/cpp_loadable`, `rtl8721csm/loadable_apps`, `imxrt1050-evk/loadable_elf_apps` (`=y`) |

현상: base에서는 `os/binfmt/libelf/`의 Kconfig가 위 config를 정의하고, Make.defs가 `CONFIG_ELF_CACHE_READ`일 때
`libelf_cache.c`를 빌드했으며, `libelf_init.c`(elf_cache_init), `libelf_read.c`(블록 크기 이하 읽기를
elf_cache_read로), `libelf_uninit.c`(elf_cache_uninit)에 훅이 있었습니다. libelf가 NuttX `libs/libc/elf`로
sync되면서 Kconfig·Make.defs 블록·훅·선언(libelf.h)이 모두 사라지고 파일만 [MOVE]로 남았습니다.

영향: 위 defconfig로 loadable app을 다시 켜면 ELF cache 없이 동작하고(속도 저하), 캐시 관련 config는 무시됩니다.

제안 조치: loadable app/binary manager를 포팅할 때 결정합니다.
- A. NuttX `elf_init.c`/`elf_read.c`/`elf_uninit.c`에 TizenRT 훅을 다시 넣고 Kconfig·Make.defs를 libs/libc/elf에 추가 (MODI_NX).
- B. ELF cache를 폐기하고 파일과 defconfig 항목 삭제.

## TD-LIBS-02 hashmap 빌드 연결 소실

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/misc/lib_hashmap.c`, `os/include/tinyara/hashmap.h` (Samsung 2017) |
| 관련 config | `CONFIG_LIB_HASHMAP` |
| 사용처 | `os/Kconfig`의 `SUPPORT_COMMON_BINARY`와 `framework/src/st_things/Kconfig`가 `select LIB_HASHMAP`, `things_data_manager.c` 등 |

현상: base `lib/libc/Kconfig`가 `LIB_HASHMAP`을 정의하고 `lib/libc/misc/Make.defs`가 빌드했습니다. libc Kconfig와
misc/Make.defs가 NuttX 기준으로 바뀌면서 심볼 정의와 빌드 블록이 사라졌고, 파일은 없어진 `lib_internal.h`를 include합니다.
`select`만 남아 있어 Kconfig가 정의되지 않은 심볼을 select합니다.

영향: `SUPPORT_COMMON_BINARY`나 st_things를 켜면 `hashmap_*` 링크 에러.

제안 조치: libc Kconfig에 `LIB_HASHMAP`을 TizenRT 확장으로 복원, `misc/Make.defs`에 `CSRCS += lib_hashmap.c` 추가,
include를 `libc.h`(또는 필요한 헤더)로 수정 (MODI_NX + MODI_TR).

## TD-LIBS-03 Samsung CRC16-CCITT API 존폐 (별도 판정 필요)

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/misc/lib_crc16_ccitt.c` (Samsung 2023) |
| 함수 | `crc16_ccitt_part`, `crc16_ccitt_rev_part`, `crc16_ccitt_xmodem`, `crc16_ccitt_false`, `crc16_ccitt_aug`, `crc16_ccitt_kermit` |
| 사용처 | `apps/examples/testcase/le_tc/kernel/tc_libc_misc.c` (42곳) |

현상: 선언하던 TizenRT `os/include/crc16.h`가 sync로 삭제되고(NuttX `tinyara/crc16.h`로 대체), `misc/Make.defs`에도
등록되지 않아 빌드되지 않습니다. NuttX 12.13.0에 같은 계산을 하는 함수가 있습니다.

| Samsung 함수 | NuttX 12.13.0 대체 |
|---|---|
| `crc16_ccitt_xmodem(src, len)` | `crc16xmodem(src, len)` |
| `crc16_ccitt_false(src, len)` | `crc16xmodempart(src, len, 0xffff)` |
| `crc16_ccitt_aug(src, len)` | `crc16xmodempart(src, len, 0x1d0f)` |
| `crc16_ccitt_kermit(src, len)` | `crc16ccitt(src, len)` |
| `crc16_ccitt_part`, `crc16_ccitt_rev_part` | `crc16xmodempart`, `crc16ccittpart` |

선택지:
- A. 삭제: le_tc 테스트를 NuttX 함수로 바꾸고 파일 삭제 (DEL_TR + MODI_TR). 대체 함수의 값이 같은지 테스트로 확인.
- B. 유지: `tinyara/crc16.h`에 TizenRT 확장 선언을 추가하고 Make.defs에 등록.

## TD-LIBS-04 Bessel 함수 존폐 (별도 판정 필요)

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libm/libm/lib_j0.c`, `lib_j0f.c`, `lib_j1.c`, `lib_j1f.c`, `lib_jn.c`, `lib_jnf.c` (musl/FreeBSD 유래, Samsung 2017) |
| 사용처 | `apps/examples/testcase/le_tc/kernel/tc_libc_math.c` (j0, j0f, j1, j1f, jn, jnf 테스트) |

현상: NuttX 12.13.0 `libs/libm/libm`에는 Bessel 함수가 없습니다(newlib libm에만 있음). 선언하던 구 `tinyara/math.h`가
sync로 삭제되고 libm Make.defs에도 등록되지 않아 빌드되지 않습니다.

선택지:
- A. 삭제: 6개 파일과 le_tc 테스트 삭제 (필요하면 `CONFIG_LIBM_NEWLIB`로 제공).
- B. 유지: libm/libm/Make.defs에 등록하고 `math.h`에 TizenRT 확장으로 선언 추가.

## TD-LIBS-05 zoneinfo ROMFS 빌드 깨짐, mk_opt_tzrule.py 존폐

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/zoneinfo/Makefile` (Samsung 2020, 미sync), `os/libs/libc/zoneinfo/mk_opt_tzrule.py` (Samsung 2020, 별도 판정 필요) |
| 관련 config | `CONFIG_LIBC_ZONEINFO_ROMFS` (있음), 구 `CONFIG_LIBC_DOWNLOAD_ZONEINFO`, `CONFIG_LIBC_OPTIMIZE_ZONEINFO_RULES` (sync로 사라짐) |

현상: `tzromfs.c`는 NuttX로 sync되어 `romfs_zoneinfo.h`를 include하지만, Makefile은 TizenRT 버전 그대로라
- `ZONEINFO_PATH = $(TOPDIR)/../lib/libc/zoneinfo`(없어진 `lib/` 경로)를 쓰고,
- `zoneinfo.img`만 만들어 `build/output/bin`에 복사하며 `romfs_zoneinfo.h`는 만들지 않습니다.
- `mk_opt_tzrule.py`를 부르는 `CONFIG_LIBC_OPTIMIZE_ZONEINFO_RULES`는 Kconfig에서 사라져 켤 수 없습니다.

영향: `CONFIG_LIBC_ZONEINFO_ROMFS=y`로 빌드할 수 없습니다(현재 켠 defconfig는 없음).

제안 조치:
- Makefile을 NuttX 12.13.0 `libs/libc/zoneinfo/Makefile` 기준으로 맞추고(`romfs_zoneinfo.h` 생성), TizenRT가 필요로 하던
  `zoneinfo.img`의 `build/output/bin` 복사가 아직 필요한지 확인 (MIXED_S,TR).
- `mk_opt_tzrule.py`: 규칙 최적화 기능을 유지하면 Kconfig 심볼을 TizenRT 확장으로 복원, 아니면 삭제 (별도 판정).

## TD-LIBS-06 armv7-m/armv7-r/xtensa machine 미sync

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/machine/arm/armv7-m/{Make.defs,arch_elf.c}`, `armv7-r/{Make.defs,arch_elf.c}` (NuttX 유래, 미sync), NuttX `libs/libc/machine/arm/armv7-{m,r}/` 21개, `libs/libc/machine/xtensa/` 13개 |
| 관련 커밋 | `2906710bc` (machine/arm/Kconfig에서 armv7-m, armv7-r 등의 `source` 제거) |
| 진행 | `DEV-12.13.0-LABEL-ARMV7`(cf549f849 기반)의 `5611e3b35` "os/libs/libc/machine/arm: Sync armv7-m and armv7-r with NuttX 12.13.0", `f639b7316` (Kconfig `source` 복원) |

현상: machine/arm은 QEMU 보드에 필요한 armv7-a, armv8-m만 sync했습니다. armv7-m/armv7-r에는 TizenRT 구버전이 남아 있고,
`arch_elf.c`의 `up_relocate`/`up_relocateadd`는 `arch_data` 인자가 없어 sync된 `tinyara/elf.h`(`..., uintptr_t addr, FAR void *arch_data)`)와
시그니처가 다릅니다. NuttX판의 `arch_mem*.S`, `arch_str*.S`, `Kconfig`, `acle-compat.h`도 없습니다.
TizenRT에 esp32 보드(xtensa)도 있지만 `libc/machine/xtensa`는 sync하지 않았습니다.

영향: armv7-m(Cortex-M3/M4/M7)이나 armv7-r 보드에서 `CONFIG_LIBC_ARCH_ELF`를 켜면 컴파일 에러. xtensa 보드는 machine 최적화·ELF 지원 없음.

제안 조치: armv7-m/armv7-r은 ARMV7 브랜치 작업을 LABEL에 합칠 때 닫음. xtensa는 esp32 보드를 포팅할 때 sync하고
machine/Kconfig의 `source`를 복원 (SYNC + MODI_NX).

## TD-LIBS-07 ARM setjmp 빌드 조건 변경

| 항목 | 내용 |
|---|---|
| 대상 | `CONFIG_ARCH_SETJMP_H`(`os/Kconfig`), `os/libs/libc/machine/arm/Make.defs` |
| 사용처 | `apps/examples/setjmp`(`EXAMPLES_SETJMP_TEST`), `apps/examples/testcase/ostest/setjmp.c`, `external/iotjs`(jerryscript) |

현상: base에서는 ARM이면 항상 `setjmp.S`를 빌드했습니다. 지금은 NuttX처럼 `CONFIG_ARCH_SETJMP_H=y`일 때만 `arch_setjmp.S`를
빌드하고, QEMU defconfig 4개는 모두 `# CONFIG_ARCH_SETJMP_H is not set`입니다(TizenRT setjmp.S는 이번 리뷰에서 삭제).
ostest의 setjmp.c는 이 config로 막혀 있지만 `EXAMPLES_SETJMP_TEST`와 iotjs는 막혀 있지 않습니다.

영향: 이 config 없이 setjmp 사용처를 켜면 툴체인 libc가 링크되지 않는 한 `setjmp`/`longjmp` 링크 에러.

제안 조치: setjmp 사용처 Kconfig에 `depends on ARCH_SETJMP_H`(또는 `select`)를 추가하거나, TizenRT 정책으로 ARM 보드 defconfig에서
`CONFIG_ARCH_SETJMP_H=y`를 켜는 것 중에서 결정 (MODI_TR).

## TD-LIBS-08 libstdc++ 미sync로 `CONFIG_LIBSUPCXX` 빌드 불가 (별도 판정 필요)

| 항목 | 내용 |
|---|---|
| 대상 | NuttX `libs/libxx/libstdc++/{.gitignore,c++config.h,libsupc++.defs}` (미sync), `os/libs/libxx/Makefile`, `os/libs/libxx/Kconfig`(`LIBSUPCXX`) |
| 관련 config | `CONFIG_LIBSUPCXX` (C++ 저수준 라이브러리 선택지 중 하나, 기본값은 `LIBSUPCXX_TOOLCHAIN`, 켠 defconfig 없음) |

현상: libxx sync는 원래 히스토리와 재작성 히스토리 모두 `libstdc++/` 디렉터리를 가져오지 않았습니다. 그런데 sync한 libxx
Makefile은 `CONFIG_LIBSUPCXX`일 때 `include libstdc++/libsupc++.defs`를 하고, Kconfig에도 `LIBSUPCXX` 선택지가 있습니다.
`libsupc++.defs`(ASF)는 gcc 소스를 내려받아 libsupc++를 빌드하고, `c++config.h`는 GPLv3 + GCC Runtime Library Exception 파일입니다.

영향: menuconfig에서 `LIBSUPCXX`를 고르면 makefile include 실패로 빌드 불가.

선택지:
- A. 3개 파일을 sync (c++config.h 라이선스 검토 후, [SYNC]).
- B. `LIBSUPCXX` 선택지를 Kconfig에서 막고 toolchain libsupc++(`LIBSUPCXX_TOOLCHAIN`)만 지원 (MODI_NX).

## TD-LIBS-09 libc kbd/slcd 코덱의 헤더 미sync

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/misc/lib_kbdencode.c`, `lib_kbddecode.c`, `lib_slcdencode.c`, `lib_slcddecode.c` (NuttX, sync됨), NuttX `include/nuttx/input/kbd_codec.h`, `include/nuttx/lcd/slcd_codec.h` (미sync) |
| 관련 config | `CONFIG_LIBC_KBDCODEC`, `CONFIG_LIBC_SLCDCODEC` (켠 defconfig 없음) |

현상: libc는 sync했지만, 이 코덱들이 가드 없이 include하는 헤더는 input, lcd 영역이라 sync하지 않았습니다(재작성 히스토리에서
TizenRT 드라이버 유지). TizenRT에는 같은 이름의 헤더가 없습니다.

영향: 위 config를 켜면 컴파일 에러.

제안 조치: 두 헤더는 TizenRT 헤더와 겹치지 않으므로 libc 보조 헤더로 sync하거나([SYNC]), 그 전까지 두 config를 막음.

## TD-LIBS-10 `lib_crypt_r.c`의 `crypto/cryptodev.h` 미sync

| 항목 | 내용 |
|---|---|
| 대상 | `os/libs/libc/unistd/lib_crypt_r.c` (NuttX, sync됨), NuttX `include/crypto/cryptodev.h` (미sync) |
| 관련 config | `CONFIG_CRYPTO` (unistd/Make.defs가 `lib_crypt.c`, `lib_crypt_r.c`를 빌드, 켠 defconfig 없음) |

현상: crypto는 범위 밖이라 `include/crypto`를 sync하지 않았는데, sync된 `lib_crypt_r.c`가 `<crypto/cryptodev.h>`를 가드 없이 include합니다.
Migration.md Known gaps의 crypto 항목은 `blake2s.h`, `crypto.h`, `se05x.h`만 적고 있습니다.

제안 조치: crypto를 sync할 때 함께 처리. 그 전에 `CONFIG_CRYPTO`를 켜야 하면 `lib_crypt*.c`를 빌드에서 빼는 가드 필요.

## TD-INC-01 Samsung gpio upper-half 구현 소실

| 항목 | 내용 |
|---|---|
| 대상 | `os/include/tinyara/gpio.h` (Samsung 2016), `os/drivers/ioexpander/gpio.c` |
| 관련 커밋 | `2eb8a4823` ([MOVE] `os/drivers/gpio/gpio.c` → `os/drivers/ioexpander/gpio.c`), `f6f00c7e9` (ioexpander sync), `8dd98470f` ("The GPIO option is dropped ... the TizenRT GPIO driver is not kept in this tree" — 재작성 때 의도적으로 뺌) |
| 사용처 | `tinyara/gpio.h` include 27개: os/arch(amebad, amebasmart, armino, imxrt, s5j, esp32 lowerhalf) 7, os/board(artik05x, esp32_devkit, imxrt1020/1050, rtl8730e) 10, os/drivers/audio 2, framework/iotbus 2, apps 6 |

현상: base `os/drivers/gpio/gpio.c`는 Samsung의 gpio upper-half(`gpio_register()`, `struct gpio_lowerhalf_s`)였습니다.
[MOVE] 커밋이 이 파일을 NuttX 위치인 `ioexpander/gpio.c`로 옮긴 뒤 ioexpander sync가 NuttX `gpio.c`
(`gpio_pin_register()`, `struct gpio_dev_s`)로 덮어써서, `tinyara/gpio.h`가 선언한 API의 구현이 없어졌습니다.
재작성 히스토리의 `8dd98470f`가 GPIO 옵션을 빼면서 "TizenRT GPIO 드라이버는 이 트리에 두지 않는다"고 적은 의도적 결정이지만,
`tinyara/gpio.h`와 그 사용처는 그대로 남아 있습니다.

영향: 위 chip/board의 gpio lowerhalf와 iotbus gpio(`/dev/gpioN` ioctl)를 쓰는 보드는 링크·동작 불가. QEMU 보드는 해당 없음.

제안 조치 (다른 보드 포팅 전에 결정):
- A. Samsung upper-half를 base에서 복원해 NuttX `gpio.c`와 다른 이름/경로로 두고 config로 선택.
- B. chip lowerhalf와 iotbus를 NuttX `ioexpander/gpio.h` API로 포팅하고 `tinyara/gpio.h`를 정리.

## TD-INC-02 오디오 영역 부분 sync 불일치 (i2schar.c, libc/audio)

| 항목 | 내용 |
|---|---|
| 대상 | `os/include/tinyara/audio/i2s.h`, `audio.h` (NuttX 유래, TizenRT 구버전), `os/drivers/i2s/i2schar.c`, `os/libs/libc/audio/lib_buffer.c` (NuttX 12.13.0) |
| 관련 config | `CONFIG_AUDIO_I2SCHAR` (켠 defconfig 없음), `CONFIG_AUDIO` (켠 defconfig 19개: artik053/avs_test, artik055s/audio, cy4390x 7개, imxrt1050-evk 3개, rtl8730e 7개) |

현상: drivers/i2s와 libc/audio는 NuttX 12.13.0으로 sync했지만, 오디오 헤더는 TizenRT codec 드라이버(alc5658, tas5749, ndp120 등)가 쓰는 구버전 그대로입니다.
- sync된 `i2schar.c`는 `I2S_RXCHANNELS()`/`I2S_TXCHANNELS()`를 쓰는데 TizenRT `i2s.h`에는 이 매크로가 없습니다.
- sync된 `lib_buffer.c`(`CONFIG_AUDIO`일 때 빌드)는 TizenRT에 없는 `<tinyara/usb/audio.h>`를 가드 없이 include하고, NuttX `audio.h` API를 전제로 합니다.
  TizenRT `audio.h`는 migration에서 빠진 `<queue.h>`도 include합니다(Migration.md Known gaps).

영향: `CONFIG_AUDIO_I2SCHAR=y`이면 컴파일 에러. `CONFIG_AUDIO=y`인 19개 구성(오디오 보드)은 libc 빌드부터 실패.

제안 조치: 오디오 영역(drivers/audio, audio 헤더, `os/audio`)을 sync할지, TizenRT 오디오를 유지하고 sync된 `i2schar.c`/`lib_buffer.c`를
TizenRT 헤더에 맞출지 결정 (오디오 보드 포팅 전). 그 전까지 `AUDIO_I2SCHAR`는 미지원으로 막음.

## TD-INC-03 tools/nxfuse가 없어진 헤더 참조

| 항목 | 내용 |
|---|---|
| 대상 | `tools/nxfuse/mknxfuse.sh`, `tools/nxfuse/src/{nxfuse,filemtd,tinyara_services}.c` |

현상: 호스트용 SmartFS FUSE 도구가 sync로 옮겨지거나 사라진 `tinyara/fs/mtd.h`(→ `tinyara/mtd/mtd.h`), `tinyara/fs/smart.h`,
`tinyara/fs/dirent.h`를 symlink·include하고, `tinyara/fs/smart_procfs.h`도 symlink합니다.

제안 조치: smartfs 작업이 끝난 뒤 새 헤더 경로로 고치고 빌드 확인 (MODI_TR). TD-INC-06과 함께 처리.

## TD-INC-04 미sync 영역의 미사용 NuttX 헤더

| 항목 | 내용 |
|---|---|
| 대상 | `os/include/tinyara/power/relay.h`, `tinyara/video/rgbcolors.h`, `tinyara/net/gmii.h`, `tinyara/net/cs89x0.h`, `tinyara/net/encx24j600.h` |

현상: NuttX 유래 헤더로 base부터 include하는 곳이 없습니다. 미sync 결정 영역(power, video, net)이라 이번에는 유지했습니다.

제안 조치: 해당 영역을 sync할 때 NuttX 12.13.0 파일로 바꾸거나(같은 경로가 NuttX에 있음) 삭제.

## TD-INC-05 구현 없는 iotdev 헤더 존폐 (별도 판정 필요)

| 항목 | 내용 |
|---|---|
| 대상 | `os/include/tinyara/iotdev.h` (Samsung 2017) |

현상: `id_drv_register()`, `id_post_evt()`를 선언하지만 base부터 구현이 없습니다. 유일하게 include하던 TizenRT
`tinyara/serial/serial.h`가 NuttX 버전으로 바뀌어 지금은 include하는 곳도 없습니다. `os/drivers/iotdev/`에는 `iotbus_sig.c`만 있고 이 헤더를 쓰지 않습니다.

선택지:
- A. 삭제 (DEL_TR).
- B. iotdev 이벤트 기능을 살릴 계획이 있으면 유지하고 구현·serial 훅을 새로 작성.

## TD-INC-06 smartfs 완료 후 smart_procfs.h 재검토

| 항목 | 내용 |
|---|---|
| 대상 | `os/include/tinyara/fs/smart_procfs.h` (NuttX 유래) |

현상: base에서 include하던 `os/fs/driver/mtd/smart.c`, `os/fs/smartfs/smartfs_procfs.c`가 NuttX 버전으로 바뀌어
(`struct mtd_smart_procfs_data_s` 등은 `tinyara/mtd/smart.h`) 지금은 include하는 곳이 없습니다(tools/nxfuse symlink만).
smartfs 작업이 끝나지 않아 유지합니다(사용자 결정).

제안 조치: smartfs 작업이 끝나면 사용처를 다시 확인해 삭제 여부를 결정하고, TD-INC-03의 nxfuse 경로도 함께 정리.

## TD-INC-07 sync된 드라이버·헤더의 미sync 헤더 의존

| 항목 | 내용 |
|---|---|
| 대상 | 아래 표의 sync된 파일 (모두 NuttX 12.13.0) |
| 출처 | [NuttX-12.13.0-Unsynced-Files-Review.md](NuttX-12.13.0-Unsynced-Files-Review.md) 6장 |

현상: sync된 파일이 미sync 영역(bluetooth/wireless, openamp, pm)의 헤더를 가드 없이 include하는데 TizenRT에는 그 헤더가 없습니다.

| sync된 파일 | 없는 헤더 | 빌드 조건 |
|---|---|---|
| `os/drivers/serial/uart_bth4.c`, `os/include/tinyara/serial/uart_bth4.h` | `tinyara/wireless/bluetooth/{bt_driver,bt_hci,bt_uart}.h` | `CONFIG_UART_BTH4` |
| `os/drivers/serial/uart_bth5.c`, `os/include/tinyara/serial/uart_bth5.h` | `tinyara/wireless/bluetooth/{bt_driver,bt_slip}.h` | `CONFIG_UART_BTH5` |
| `os/drivers/misc/optee_socket.c` | `netpacket/rpmsg.h` | `CONFIG_DEV_OPTEE_NONE`가 아니고 `CONFIG_DEV_OPTEE_SMC`도 아닐 때 |
| `os/include/tinyara/coresight/coresight.h` | `tinyara/power/pm.h` | coresight 드라이버 미sync라 include하는 곳 없음 |

TizenRT bluetooth 헤더는 `tinyara/bluetooth/`에 있어 NuttX 경로(`tinyara/wireless/bluetooth/`)와 다릅니다. 위 config를 켠 defconfig는 없습니다.

영향: 해당 config를 켜면 컴파일 에러. 지금 빌드에는 영향 없음.

제안 조치: 각 영역(bluetooth, openamp, pm, coresight)을 sync할 때 함께 처리. 그 전까지 `UART_BTH4/5`, optee socket을 Kconfig에서 막는 것을 검토.

## TD-DOC-01 마이그레이션 문서의 sync 범위가 LABEL과 다름

| 항목 | 내용 |
|---|---|
| 대상 | `docs_migrate/NuttX-12.13.0-Migration.md`, `docs_migrate/NuttX-12.13.0-Migration-Report.md`, `docs_migrate/HANDOFF.md` |
| 판정 | 재작성 히스토리의 범위 축소는 의도된 제외 (사용자 판정, 2026-11-02) |

현상: 세 문서는 원래 히스토리(`claude/nuttx-tizenrt-migration-nqry9a`, 예: libnx `ac0ffe942b`)를 기준으로 작성되어, 재작성된
`DEV-12.13.0-LABEL`이 가져오지 않은 영역을 sync했다고 적고 있습니다.

| 문서 | 위치 | LABEL과 다른 내용 |
|---|---|---|
| Migration.md | Layout 표 | `libs/{...,libnx}` → `os/libs/...`, `audio/` → `os/audio`, `graphics/`, `video/` → `os/graphics`, `os/video` (new) |
| Migration.md | Not synced 표 | 재작성에서 TizenRT를 유지한 drivers 영역(audio, input, lcd, sensors, usb, video, power의 pm 외 부분, `8dd98470f`)과 graphics/libnx/video/audio가 없음 |
| Migration-Report.md | 동기화 수치 표, 결정 표 | libnx 164, graphics 86, video 10, audio 5, drivers media/usb/power sync 수치와 "graphics, video, libnx를 새로 추가" |
| HANDOFF.md | Decisions, Done 표 | `os/libs/{...,libnx}`, 원래 히스토리의 libnx·graphics·video·audio·drivers media 커밋 |

제안 조치: Migration.md의 Layout·Not synced 표를 LABEL 기준으로 고치고(재작성에서 제외한 영역과 이유 추가),
Migration-Report와 HANDOFF에는 "원래 히스토리 기준이며 LABEL에서는 제외"라는 주석을 추가 (DOC).

## TD-BUILD-01 QEMU 보드에 `MKDEP` 정의 없음 (`.depend` 실패)

| 항목 | 내용 |
|---|---|
| 대상 | `build/configs/qemu-armv7a/Make.defs`, `build/configs/mps2-an521/Make.defs`, `os/tools/Config.mk` |
| 발견 | 잔존 파일 리뷰 커밋의 빌드 검증(2026-11-02, QEMU 4개 구성) |

현상: 각 디렉터리 Makefile의 `.depend` 규칙은 `$(MKDEP) $(DEPPATH) ...`를 실행하는데, QEMU 보드에서는 `MKDEP`가 정의되지
않아 명령이 `--dep-path ...`로 시작합니다. 빌드 로그에 `/bin/sh: 1: dep-path: not found`와 `[.depend] Error 127 (ignored)`가
22번 나옵니다(os/kernel, os/board, os/fs, os/drivers, apps, external, framework 등). make가 에러를 무시하므로 빌드와 QEMU 실행은 통과합니다.
다른 TizenRT 보드는 자기 `Make.defs`에서 `MKDEP`를 정의하고(예: artik053), 커밋 재구성(261028) 전 빌드 커밋 `2c33f8282`는
`os/tools/Config.mk`에서 `MKDEP ?= .../tools/mkdeps`를 정의했지만, 재구성에서 NuttX mkdeps 대체를 빼면서 이 정의도 빠졌습니다.
(10/27 빌드 로그에는 이 에러가 없음)

영향: `Make.dep`이 만들어지지 않아 헤더만 바꾼 증분 빌드에서 다시 컴파일되지 않을 수 있습니다. clean 빌드는 영향 없음.

제안 조치: QEMU 보드 `Make.defs`에 다른 TizenRT 보드처럼 `MKDEP = $(TOPDIR)/tools/mkdeps$(HOSTEXEEXT)`를 추가하거나,
`os/tools/Config.mk`에 `MKDEP ?=` 기본값을 둠 (MODI_TR). 수정 후 빌드 로그에 `dep-path` 에러가 없는지, `Make.dep`이 생성되는지 확인.
