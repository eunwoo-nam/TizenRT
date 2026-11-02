# NuttX 12.13.0 미sync 파일 리뷰

| 항목 | 내용 |
|---|---|
| 작성일 | 2026-11-02 |
| 갱신 | 2026-11-02 후속 결정 반영 (8장) |
| 대상 브랜치 | `DEV-12.13.0-LABEL` (`cf549f849` 기준) |
| 리뷰 기준 트리 | `~/project/nuttx/nuttx-12.13.0___NO_SYNCED` 브랜치 `nuttx-12.13.0___NO_SYNCED` `9779f82204` — NuttX 12.13.0(`b9b5d9afbf`)에서 LABEL이 가져간 파일 2880개를 뺀 트리(22691 files) |
| 범위 | `include/`, `libs/` (884) |
| 함께 볼 문서 | [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md), [NuttX-12.13.0-Leftover-Files-Review.md](NuttX-12.13.0-Leftover-Files-Review.md) (반대 방향 리뷰), [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md) |

## 1. 목적

[잔존 파일 리뷰](NuttX-12.13.0-Leftover-Files-Review.md)가 TizenRT 쪽에서 "지워졌어야 하는데 남은 파일"을 봤다면,
이 문서는 반대로 NuttX 쪽에서 "TizenRT로 sync되지 않은 파일"을 봅니다. 의도적으로 제외한 파일, 다른 형태로 이미
반영된 파일, 빠뜨린 파일(덜 sync)을 나누고, 빠뜨린 파일과 sync 경계에서 생긴 문제는 TD로 넘깁니다.

## 2. 판정 기준

| 판정 | 기준 |
|---|---|
| 제외 결정 | 마이그레이션 결정으로 sync하지 않는 영역 ([Migration.md](NuttX-12.13.0-Migration.md) "Not synced") |
| 의도된 제외 | 원래 히스토리에서는 sync했지만 재작성 히스토리(LABEL)에서 TizenRT 쪽을 유지하기로 한 영역 (사용자 판정). 문서가 아직 sync로 적고 있어 TD-DOC-01 |
| 제외 | TizenRT에 해당 arch가 없어 필요 없는 파일 |
| 부분 반영 / 반영됨 | TizenRT 파일에 내용이 반영되어 NuttX 파일 자체는 필요 없음 |
| TD | 해당 보드 포팅 때 sync하거나 따로 조치가 필요한 파일 |
| **누락** | TizenRT 빌드가 참조하는데 sync되지 않은 파일. 이번에는 sync하지 않고 TD로 기록 (사용자 결정) |

## 3. 확인 방법

| 확인 | 방법 |
|---|---|
| sync된 파일 | NuttX `b9b5d9afbf` 전체 트리 − NO_SYNCED 트리 = 2880개 (NO_SYNCED 커밋 메시지의 분류: SYNC 2715, 그대로 복사 162, crypto 3) |
| 미sync 파일 분류 | 경로 규칙으로 884개를 전부 분류 (미분류 0) |
| TizenRT 대응 | `include/nuttx/` → `os/include/tinyara/`, 그 밖은 `os/` 아래 같은 경로 |
| 빌드 참조 | TizenRT Makefile·Make.defs·Kconfig에서 경로·config 검색 |
| sync 경계 의존성 | sync된 NuttX 소스 2536개(`.c .h .S .cxx`)의 `#include`를 따라가 NO_SYNCED에 있는 헤더를 찾고, TizenRT 쪽 파일에서 그 include를 감싼 `#if` 가드와 헤더 존재 여부 확인 |

## 4. 요약

| 분류 | 파일 | 판정 | 근거 |
|---|---|---|---|
| CMake 파일 (`CMakeLists.txt`, `*.cmake`) | 95 | 제외 결정 | cmake 범위 밖 (Migration.md Not synced) |
| 네트워크 | 141 | 제외 결정 | TizenRT 네트워크 스택 유지 |
| openamp (`*rpmsg*`, `*rptun*`) | 19 | 제외 결정 → **14개 sync됨** (후속 결정) | rpmsg/rptun sync, net·usrsock·wireless의 5개는 제외 유지 (8장) |
| crypto (`include/crypto`, `include/nuttx/crypto`) | 25 | 제외 결정 | 범위 밖. `tea.h`만 sync |
| wireless (`include/nuttx/wireless`) | 39 | 제외 결정 | 사용 안 함 |
| pm (`include/nuttx/power/pm.h`) | 1 | 제외 결정 | TizenRT `os/pm` 유지 |
| drivers 영역 헤더 (`audio`, `input`, `lcd`, `sensors`, `usb`, `video`, `power`, `uorb.h`, `sys/video*`) | 262 | 의도된 제외 → **usb 31개 sync됨** (후속 결정) | 재작성 히스토리에서 TizenRT 드라이버 유지 (8dd98470f). usb는 후속 결정으로 sync (8장). 문서 불일치는 TD-DOC-01 |
| graphics (`libs/libnx`, `include/nuttx/nx`) | 173 | 의도된 제외 | 재작성 히스토리에서 제외. 문서 불일치는 TD-DOC-01 |
| libc/machine: TizenRT에 없는 arch | 81 | 제외 | arm/arm, armv6-m, armv8-r, arm64, risc-v, sim, sparc, x86, x86_64, renesas |
| libc/machine: TizenRT에 있는 arch (armv7-m, armv7-r, xtensa) | 34 | TD → **armv7-m/r 21개 sync됨** (후속 결정) | TD-LIBS-06 (xtensa 13개 남음) |
| 라이브러리 Makefile | 6 | 부분 반영 | TizenRT 빌드 시스템 Makefile에 NuttX 소스 목록만 반영 |
| TizenRT에 본문이 같은 파일이 있음 | 4 | 반영됨 | 라이선스 헤더만 다름 |
| `libs/libc/zoneinfo/Makefile` | 1 | TD | TD-LIBS-05 |
| `libs/libxx/libstdc++` | 3 | **누락** | TD-LIBS-08 |
| 합계 | 884 | | |

조치가 필요한 결과:

- **누락 1건**: `libs/libxx/libstdc++` 3개. TizenRT `os/libs/libxx/Makefile`이 `CONFIG_LIBSUPCXX`일 때
  `libstdc++/libsupc++.defs`를 include하므로 그 옵션을 켜면 빌드되지 않습니다. `c++config.h`는 GPLv3 + GCC Runtime
  Library Exception이라 라이선스 검토가 필요합니다 → TD-LIBS-08.
- **sync 경계 문제**: sync된 파일 중 일부가 TizenRT에 없는 NuttX 헤더를 가드 없이 include합니다(6장).
  `CONFIG_AUDIO=y`인 defconfig가 19개라 `os/libs/libc/audio/lib_buffer.c`가 가장 영향이 큽니다 → TD-INC-02.
- **문서 불일치**: Migration.md·Migration-Report·HANDOFF는 drivers 영역(audio, input, lcd, sensors, usb, video, power)과
  graphics(`os/libs/libnx`, `os/graphics`), `os/video`, `os/audio`를 sync했다고 적지만 LABEL에는 이 파일들이 없거나
  (`os/audio`는) TizenRT 파일 그대로입니다 → TD-DOC-01.

## 5. 조치·확인 대상 파일 (48)

armv7-m, armv7-r의 `libc/machine`은 `DEV-12.13.0-LABEL-ARMV7` 브랜치에서 sync했고, 후속 결정으로 LABEL에 cherry-pick했습니다
(`30917f838` "os/libs/libc/machine/arm: Sync armv7-m and armv7-r with NuttX 12.13.0", `11f775a4a` Kconfig source 복원).
아래 표의 armv7-m/armv7-r 21개는 이제 sync되어 있습니다.
xtensa는 TizenRT의 esp32 보드를 포팅할 때 필요합니다.

| 파일 (NuttX 12.13.0) | TizenRT 대응 | 판정 | 근거 |
|---|---|---|---|
| `libs/libxx/libstdc++/.gitignore` | 없음 | **누락** (TD-LIBS-08) | TizenRT에 디렉터리 없음 |
| `libs/libxx/libstdc++/c++config.h` | 없음 | **누락** (TD-LIBS-08) | TizenRT에 없음. GPLv3 + GCC Runtime Library Exception |
| `libs/libxx/libstdc++/libsupc++.defs` | 없음 | **누락** (TD-LIBS-08) | TizenRT에 없음. libxx/Makefile이 include |
| `libs/libc/zoneinfo/Makefile` | `os/libs/libc/zoneinfo/Makefile` | TD-LIBS-05 | TizenRT Makefile이 옛 경로·zoneinfo.img 방식, sync된 tzromfs.c와 불일치 |
| `libs/libbuiltin/Makefile` | `os/libs/libbuiltin/Makefile` | 부분 반영 | TizenRT 빌드 시스템 Makefile (os/libs/libbuiltin/Makefile)에 NuttX 소스 목록 반영 |
| `libs/libc/Makefile` | `os/libs/libc/Makefile` | 부분 반영 | 〃 (Make.defs include 목록, kbin/bin 처리는 TizenRT 방식) |
| `libs/libc/elf/Makefile` | 없음 | 부분 반영 | TizenRT는 별도 Makefile 없이 `os/libs/libc/Makefile`이 gnu-elf.ld를 생성 (ea6cac3af) |
| `libs/libdsp/Makefile` | `os/libs/libdsp/Makefile` | 부분 반영 | 〃 |
| `libs/libm/Makefile` | `os/libs/libm/Makefile` | 부분 반영 | 〃 |
| `libs/libxx/Makefile` | `os/libs/libxx/Makefile` | 부분 반영 | 〃. `CONFIG_LIBSUPCXX`일 때 없는 `libstdc++/libsupc++.defs`를 include → TD-LIBS-08 |
| `include/nuttx/.gitignore` | `os/include/tinyara/.gitignore` | 반영됨 | `os/include/tinyara/.gitignore`와 내용 동일 |
| `libs/libc/aio/Make.defs` | `os/libs/libc/aio/Make.defs` | 반영됨 | 본문 동일, 라이선스 헤더만 다름 (1차 리뷰 "유지") |
| `libs/libc/inttypes/Make.defs` | `os/libs/libc/inttypes/Make.defs` | 반영됨 | 〃 |
| `libs/libc/libgen/Make.defs` | `os/libs/libc/libgen/Make.defs` | 반영됨 | 〃 |
| `libs/libc/machine/arm/armv7-m/Kconfig` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/Make.defs` | `os/libs/libc/machine/arm/armv7-m/Make.defs` | sync됨 (`30917f838`) | TizenRT 구버전 유지 (up_relocate 시그니처 불일치) |
| `libs/libc/machine/arm/armv7-m/acle-compat.h` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_elf.c` | `os/libs/libc/machine/arm/armv7-m/arch_elf.c` | sync됨 (`30917f838`) | TizenRT 구버전 유지 (up_relocate 시그니처 불일치) |
| `libs/libc/machine/arm/armv7-m/arch_memchr.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_memcpy.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_memmove.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_memset.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_strcmp.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_strcpy.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-m/arch_strlen.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/Kconfig` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/Make.defs` | `os/libs/libc/machine/arm/armv7-r/Make.defs` | sync됨 (`30917f838`) | TizenRT 구버전 유지 (up_relocate 시그니처 불일치) |
| `libs/libc/machine/arm/armv7-r/acle-compat.h` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_elf.c` | `os/libs/libc/machine/arm/armv7-r/arch_elf.c` | sync됨 (`30917f838`) | TizenRT 구버전 유지 (up_relocate 시그니처 불일치) |
| `libs/libc/machine/arm/armv7-r/arch_memchr.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_memcpy.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_memmove.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_memset.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_strcmp.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/arm/armv7-r/arch_strlen.S` | 없음 | sync됨 (`30917f838`) | TizenRT에 없음 |
| `libs/libc/machine/xtensa/Kconfig` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/Make.defs` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_elf.c` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_mcount.c` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_memcpy.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_memmove.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_memset.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_setjmp.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_strcmp.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_strcpy.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_strlen.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/arch_strncpy.S` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |
| `libs/libc/machine/xtensa/xtensa_asm.h` | 없음 | TD-LIBS-06 | TizenRT에 없음 (esp32 보드용) |

## 6. sync 경계 의존성: sync된 파일이 include하는 미sync 헤더 (44)

"TizenRT에"가 **없음**이고 가드도 **없음**인 경우, 그 파일이 빌드되면 컴파일 에러가 납니다. 이 파일들은 모두
Make.defs의 config 조건(`CONFIG_AUDIO`, `CONFIG_LIBC_KBDCODEC`, `CONFIG_LIBC_SLCDCODEC`, `CONFIG_CRYPTO`,
`CONFIG_UART_BTH4/5`, optee 등)으로 빌드 여부가 정해지며, QEMU 구성에서는 꺼져 있어 지금 빌드는 통과합니다.
"구버전 있음"은 미sync 영역의 TizenRT 헤더를 쓰는 경우로, QEMU 구성에서 빌드되는 파일(`fs_fstat.c`, `nx_start.c`,
`assert.c` 등)은 이 상태로 빌드·실행이 확인되었습니다.

| 미sync 헤더 (NuttX) | TizenRT에 | include한 sync 파일 (TizenRT) | 가드 | 판정 |
|---|---|---|---|---|
| `include/arpa/inet.h` | 구버전 있음 | `os/binfmt/libnxflat/libnxflat_bind.c`<br>`os/binfmt/libnxflat/libnxflat_init.c`<br>`os/binfmt/libnxflat/libnxflat_load.c`<br>`os/binfmt/libnxflat/libnxflat_read.c`<br>`os/binfmt/libnxflat/libnxflat_verify.c`<br>`os/binfmt/nxflat.c`<br>`os/fs/userfs/fs_userfs.c`<br>`os/fs/v9fs/socket_9p.c`<br>`os/libs/libc/string/lib_strstr.c`<br>`os/libs/libc/userfs/lib_userfs.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/crypto/cryptodev.h` | **없음** | `os/libs/libc/unistd/lib_crypt_r.c` | **없음** | TD-LIBS-10 |
| `include/net/if.h` | 구버전 있음 | `os/include/sys/boardctl.h` | `#ifdef CONFIG_BOARDCTL_MACADDR` | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/netinet/in.h` | 구버전 있음 | `os/fs/userfs/fs_userfs.c`<br>`os/libs/libc/userfs/lib_userfs.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/netpacket/rpmsg.h` | **없음** | `os/drivers/misc/optee_socket.c` | **없음** | TD-INC-07 |
| `include/nuttx/audio/audio.h` | 구버전 있음 | `os/drivers/i2s/i2schar.c`<br>`os/libs/libc/audio/lib_buffer.c` | **없음** | TD-INC-02 |
| `include/nuttx/audio/i2s.h` | 구버전 있음 | `os/drivers/i2s/i2schar.c` | **없음** | TD-INC-02 |
| `include/nuttx/crypto/crypto.h` | **없음** | `os/drivers/bch/bchlib_cache.c`<br>`os/drivers/drivers_initialize.c` | `#if defined(CONFIG_BCH_ENCRYPTION)`<br>`#if defined(CONFIG_CRYPTO)` | 가드 안, 해당 config 미사용 |
| `include/nuttx/input/ioctl.h` | **없음** | `os/include/sys/ioctl.h` | `#ifdef CONFIG_INPUT` | 가드 안, 해당 config 미사용 |
| `include/nuttx/input/kbd_codec.h` | **없음** | `os/libs/libc/misc/lib_kbddecode.c`<br>`os/libs/libc/misc/lib_kbdencode.c` | **없음** | TD-LIBS-09 |
| `include/nuttx/input/uinput.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_UINPUT_TOUCH) \|\| defined(CONFIG_UINPUT_BUTTONS) \|\|` | 가드 안, 해당 config 미사용 |
| `include/nuttx/lcd/slcd_codec.h` | **없음** | `os/libs/libc/misc/lib_slcddecode.c`<br>`os/libs/libc/misc/lib_slcdencode.c` | **없음** | TD-LIBS-09 |
| `include/nuttx/net/ioctl.h` | 구버전 있음 | `os/include/sys/ioctl.h` | `#ifdef CONFIG_NET` | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/nuttx/net/loopback.h` | 구버전 있음 | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_NET_LOOPBACK)` | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/nuttx/net/net.h` | 구버전 있음 | `os/drivers/misc/optee_socket.c`<br>`os/fs/userfs/fs_userfs.c`<br>`os/fs/v9fs/socket_9p.c`<br>`os/fs/vfs/fs_fstat.c`<br>`os/fs/vfs/fs_poll.c`<br>`os/fs/vfs/fs_sendfile.c`<br>`os/kernel/group/group_leave.c`<br>`os/kernel/group/group_setupidlefiles.c`<br>`os/kernel/init/nx_start.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/nuttx/net/netdev.h` | 구버전 있음 | `os/include/sys/boardctl.h` | `#ifdef CONFIG_BOARDCTL_MACADDR` | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/nuttx/net/rpmsgdrv.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_NET_RPMSG_DRV_SERVER)` | 가드 안, 해당 config 미사용 |
| `include/nuttx/net/telnet.h` | 구버전 있음 | `os/drivers/drivers_initialize.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/nuttx/net/tun.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_NET_TUN)` | 가드 안, 해당 config 미사용 |
| `include/nuttx/nx/nx.h` | **없음** | `os/include/sys/boardctl.h` | `#ifdef CONFIG_VNCSERVER` | 가드 안, 해당 config 미사용 |
| `include/nuttx/nx/nxmu.h` | **없음** | `os/board/boardctl.c` | `#ifdef CONFIG_NX` | 가드 안, 해당 config 미사용 |
| `include/nuttx/nx/nxterm.h` | **없음** | `os/include/sys/boardctl.h` | `#ifdef CONFIG_NXTERM` | 가드 안, 해당 config 미사용 |
| `include/nuttx/power/pm.h` | **없음** | `os/drivers/drivers_initialize.c`<br>`os/drivers/serial/serial.c`<br>`os/drivers/timers/watchdog.c`<br>`os/include/tinyara/coresight/coresight.h`<br>`os/include/sys/boardctl.h`<br>`os/kernel/sched/sched_cpuload_oneshot.c`<br>`os/kernel/sched/sched_cpuload_period.c` | `#if defined(CONFIG_PM)`<br>`#ifdef CONFIG_PM`<br>**없음** | TD-INC-07 (coresight.h), 그 외 Migration.md Known gaps |
| `include/nuttx/power/regulator.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_REGULATOR_RPMSG)` | 가드 안, 해당 config 미사용 |
| `include/nuttx/sensors/sensor.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_USENSOR) \|\| defined(CONFIG_SENSORS_RPMSG) \|\|` | 가드 안, 해당 config 미사용 |
| `include/nuttx/usb/adb.h` | **없음** | `os/board/boardctl.c` | `#ifdef CONFIG_BOARDCTL_USBDEVCTRL` | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/audio.h` | **없음** | `os/libs/libc/audio/lib_buffer.c` | **없음** | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/cdcacm.h` | 구버전 있음 | `os/board/boardctl.c`<br>`os/drivers/syslog/syslog_channel.c` | `#ifdef CONFIG_BOARDCTL_USBDEVCTRL`<br>`#ifdef CONFIG_SYSLOG_CDCACM` | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/composite.h` | 구버전 있음 | `os/board/boardctl.c` | `#ifdef CONFIG_BOARDCTL_USBDEVCTRL` | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/pl2303.h` | 구버전 있음 | `os/board/boardctl.c` | `#ifdef CONFIG_BOARDCTL_USBDEVCTRL` | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/usbdev_trace.h` | 구버전 있음 | `os/kernel/misc/assert.c` | **없음** | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/usbhost.h` | 구버전 있음 | `os/drivers/drivers_initialize.c` | **없음** | sync됨 (`1c37bfac3`) |
| `include/nuttx/usb/usbmsc.h` | 구버전 있음 | `os/board/boardctl.c` | `#ifdef CONFIG_BOARDCTL_USBDEVCTRL` | sync됨 (`1c37bfac3`) |
| `include/nuttx/usrsock/usrsock_rpmsg.h` | **없음** | `os/drivers/drivers_initialize.c` | `#if defined(CONFIG_NET_USRSOCK_RPMSG_SERVER)` | 가드 안, 해당 config 미사용 |
| `include/nuttx/video/vnc.h` | **없음** | `os/board/boardctl.c` | `#ifdef CONFIG_VNCSERVER` | 가드 안, 해당 config 미사용 |
| `include/nuttx/wireless/bluetooth/bt_driver.h` | **없음** | `os/drivers/serial/uart_bth4.c`<br>`os/drivers/serial/uart_bth5.c`<br>`os/include/tinyara/serial/uart_bth4.h`<br>`os/include/tinyara/serial/uart_bth5.h` | **없음** | TD-INC-07 |
| `include/nuttx/wireless/bluetooth/bt_hci.h` | **없음** | `os/drivers/serial/uart_bth4.c` | **없음** | TD-INC-07 |
| `include/nuttx/wireless/bluetooth/bt_slip.h` | **없음** | `os/drivers/serial/uart_bth5.c` | **없음** | TD-INC-07 |
| `include/nuttx/wireless/bluetooth/bt_uart.h` | **없음** | `os/drivers/serial/uart_bth4.c` | **없음** | TD-INC-07 |
| `include/nuttx/wireless/ieee802154/ieee802154_mac.h` | **없음** | `os/include/sys/ioctl.h` | `#ifdef CONFIG_WIRELESS_IEEE802154` | 가드 안, 해당 config 미사용 |
| `include/nuttx/wireless/ioctl.h` | **없음** | `os/include/sys/ioctl.h` | `#ifdef CONFIG_DRIVERS_WIRELESS` | 가드 안, 해당 config 미사용 |
| `include/nuttx/wireless/wireless.h` | **없음** | `os/include/sys/ioctl.h` | `#ifdef CONFIG_NET && #ifdef CONFIG_NETDEV_WIRELESS_IOCTL` | 가드 안, 해당 config 미사용 |
| `include/sys/socket.h` | 구버전 있음 | `os/drivers/misc/optee_socket.c`<br>`os/fs/userfs/fs_userfs.c`<br>`os/include/tinyara/can.h`<br>`os/include/tinyara/fs/nfs.h`<br>`os/include/tinyara/fs/userfs.h`<br>`os/libs/libc/userfs/lib_userfs.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |
| `include/sys/un.h` | 구버전 있음 | `os/drivers/misc/optee_socket.c` | **없음** | TizenRT 구버전 헤더 사용 (미sync 영역) |

## 7. 제외 영역 상세

대량으로 제외한 영역은 디렉터리별 파일 수와 접은 파일 목록으로 정리했습니다.

### CMake 파일 (`CMakeLists.txt`, `*.cmake`) (95)

| 디렉터리 | 파일 |
|---|---|
| `libs` | 1 |
| `libs/libbuiltin` | 3 |
| `libs/libc` | 70 |
| `libs/libdsp` | 1 |
| `libs/libm` | 8 |
| `libs/libnx` | 6 |
| `libs/libxx` | 6 |

<details><summary>파일 목록</summary>

- `libs/CMakeLists.txt`
- `libs/libbuiltin/CMakeLists.txt`
- `libs/libbuiltin/compiler-rt/CMakeLists.txt`
- `libs/libbuiltin/libgcc/CMakeLists.txt`
- `libs/libc/CMakeLists.txt`
- `libs/libc/aio/CMakeLists.txt`
- `libs/libc/assert/CMakeLists.txt`
- `libs/libc/audio/CMakeLists.txt`
- `libs/libc/audio/libsrc/CMakeLists.txt`
- `libs/libc/builtin/CMakeLists.txt`
- `libs/libc/ctype/CMakeLists.txt`
- `libs/libc/dirent/CMakeLists.txt`
- `libs/libc/dlfcn/CMakeLists.txt`
- `libs/libc/elf/CMakeLists.txt`
- `libs/libc/errno/CMakeLists.txt`
- `libs/libc/eventfd/CMakeLists.txt`
- `libs/libc/fdt/CMakeLists.txt`
- `libs/libc/fixedmath/CMakeLists.txt`
- `libs/libc/gdbstub/CMakeLists.txt`
- `libs/libc/gnssutils/CMakeLists.txt`
- `libs/libc/grp/CMakeLists.txt`
- `libs/libc/hex2bin/CMakeLists.txt`
- `libs/libc/inttypes/CMakeLists.txt`
- `libs/libc/libgen/CMakeLists.txt`
- `libs/libc/locale/CMakeLists.txt`
- `libs/libc/lzf/CMakeLists.txt`
- `libs/libc/machine/CMakeLists.txt`
- `libs/libc/machine/arm/CMakeLists.txt`
- `libs/libc/machine/arm/arm/CMakeLists.txt`
- `libs/libc/machine/arm/armv6-m/CMakeLists.txt`
- `libs/libc/machine/arm/armv7-a/CMakeLists.txt`
- `libs/libc/machine/arm/armv7-m/CMakeLists.txt`
- `libs/libc/machine/arm/armv7-r/CMakeLists.txt`
- `libs/libc/machine/arm/armv8-m/CMakeLists.txt`
- `libs/libc/machine/arm/armv8-r/CMakeLists.txt`
- `libs/libc/machine/arm64/CMakeLists.txt`
- `libs/libc/machine/renesas/CMakeLists.txt`
- `libs/libc/machine/risc-v/CMakeLists.txt`
- `libs/libc/machine/sim/CMakeLists.txt`
- `libs/libc/machine/sparc/CMakeLists.txt`
- `libs/libc/machine/tricore/CMakeLists.txt`
- `libs/libc/machine/x86/CMakeLists.txt`
- `libs/libc/machine/x86_64/CMakeLists.txt`
- `libs/libc/machine/xtensa/CMakeLists.txt`
- `libs/libc/misc/CMakeLists.txt`
- `libs/libc/net/CMakeLists.txt`
- `libs/libc/netdb/CMakeLists.txt`
- `libs/libc/obstack/CMakeLists.txt`
- `libs/libc/pthread/CMakeLists.txt`
- `libs/libc/pwd/CMakeLists.txt`
- `libs/libc/queue/CMakeLists.txt`
- `libs/libc/regex/CMakeLists.txt`
- `libs/libc/sched/CMakeLists.txt`
- `libs/libc/search/CMakeLists.txt`
- `libs/libc/semaphore/CMakeLists.txt`
- `libs/libc/signal/CMakeLists.txt`
- `libs/libc/spawn/CMakeLists.txt`
- `libs/libc/stdio/CMakeLists.txt`
- `libs/libc/stdlib/CMakeLists.txt`
- `libs/libc/stream/CMakeLists.txt`
- `libs/libc/string/CMakeLists.txt`
- `libs/libc/symtab/CMakeLists.txt`
- `libs/libc/syslog/CMakeLists.txt`
- `libs/libc/termios/CMakeLists.txt`
- `libs/libc/time/CMakeLists.txt`
- `libs/libc/tls/CMakeLists.txt`
- `libs/libc/uio/CMakeLists.txt`
- `libs/libc/unistd/CMakeLists.txt`
- `libs/libc/userfs/CMakeLists.txt`
- `libs/libc/uuid/CMakeLists.txt`
- `libs/libc/wchar/CMakeLists.txt`
- `libs/libc/wctype/CMakeLists.txt`
- `libs/libc/wqueue/CMakeLists.txt`
- `libs/libc/zoneinfo/CMakeLists.txt`
- `libs/libdsp/CMakeLists.txt`
- `libs/libm/CMakeLists.txt`
- `libs/libm/libm/CMakeLists.txt`
- `libs/libm/libm/arm/CMakeLists.txt`
- `libs/libm/libm/arm/armv7-m/CMakeLists.txt`
- `libs/libm/libm/arm/armv8-m/CMakeLists.txt`
- `libs/libm/libmcs/CMakeLists.txt`
- `libs/libm/newlib/CMakeLists.txt`
- `libs/libm/openlibm/CMakeLists.txt`
- `libs/libnx/CMakeLists.txt`
- `libs/libnx/nx/CMakeLists.txt`
- `libs/libnx/nxfonts/CMakeLists.txt`
- `libs/libnx/nxglib/CMakeLists.txt`
- `libs/libnx/nxmu/CMakeLists.txt`
- `libs/libnx/nxtk/CMakeLists.txt`
- `libs/libxx/CMakeLists.txt`
- `libs/libxx/libcxx/CMakeLists.txt`
- `libs/libxx/libcxxabi/CMakeLists.txt`
- `libs/libxx/libminiabi/CMakeLists.txt`
- `libs/libxx/libstdc++/CMakeLists.txt`
- `libs/libxx/uClibc++/CMakeLists.txt`

</details>

### 네트워크 (141)

| 디렉터리 | 파일 |
|---|---|
| `include` | 2 |
| `include/arpa` | 1 |
| `include/net` | 4 |
| `include/netinet` | 9 |
| `include/netpacket` | 6 |
| `include/nuttx/net` | 56 |
| `include/sys` | 3 |
| `libs/libc/net` | 25 |
| `libs/libc/netdb` | 35 |

<details><summary>파일 목록</summary>

- `include/arpa/inet.h`
- `include/ifaddrs.h`
- `include/net/ethernet.h`
- `include/net/if.h`
- `include/net/if_arp.h`
- `include/net/route.h`
- `include/netdb.h`
- `include/netinet/arp.h`
- `include/netinet/ether.h`
- `include/netinet/icmp6.h`
- `include/netinet/if_ether.h`
- `include/netinet/in.h`
- `include/netinet/ip.h`
- `include/netinet/ip6.h`
- `include/netinet/tcp.h`
- `include/netinet/udp.h`
- `include/netpacket/bluetooth.h`
- `include/netpacket/ieee802154.h`
- `include/netpacket/if_addr.h`
- `include/netpacket/netlink.h`
- `include/netpacket/packet.h`
- `include/netpacket/vm_sockets.h`
- `include/nuttx/net/bluetooth.h`
- `include/nuttx/net/can.h`
- `include/nuttx/net/dns.h`
- `include/nuttx/net/e1000.h`
- `include/nuttx/net/enc28j60.h`
- `include/nuttx/net/encx24j600.h`
- `include/nuttx/net/ethernet.h`
- `include/nuttx/net/ftmac100.h`
- `include/nuttx/net/gmii.h`
- `include/nuttx/net/icmp.h`
- `include/nuttx/net/icmpv6.h`
- `include/nuttx/net/ieee802154.h`
- `include/nuttx/net/if_bridge.h`
- `include/nuttx/net/igb.h`
- `include/nuttx/net/igc.h`
- `include/nuttx/net/igmp.h`
- `include/nuttx/net/ioctl.h`
- `include/nuttx/net/ip.h`
- `include/nuttx/net/ipopt.h`
- `include/nuttx/net/ipv6ext.h`
- `include/nuttx/net/ksz9477.h`
- `include/nuttx/net/lan91c111.h`
- `include/nuttx/net/lan9250.h`
- `include/nuttx/net/loopback.h`
- `include/nuttx/net/mdio.h`
- `include/nuttx/net/mii.h`
- `include/nuttx/net/mld.h`
- `include/nuttx/net/neighbor.h`
- `include/nuttx/net/net.h`
- `include/nuttx/net/netconfig.h`
- `include/nuttx/net/netdev.h`
- `include/nuttx/net/netdev_lowerhalf.h`
- `include/nuttx/net/netfilter/ip6_tables.h`
- `include/nuttx/net/netfilter/ip_tables.h`
- `include/nuttx/net/netfilter/netfilter.h`
- `include/nuttx/net/netfilter/nf_nat.h`
- `include/nuttx/net/netfilter/x_tables.h`
- `include/nuttx/net/netlink.h`
- `include/nuttx/net/netstats.h`
- `include/nuttx/net/oa_tc6.h`
- `include/nuttx/net/phy.h`
- `include/nuttx/net/pkt.h`
- `include/nuttx/net/radiodev.h`
- `include/nuttx/net/rfc6775.h`
- `include/nuttx/net/sixlowpan.h`
- `include/nuttx/net/slip.h`
- `include/nuttx/net/sms.h`
- `include/nuttx/net/snoop.h`
- `include/nuttx/net/tcp.h`
- `include/nuttx/net/telnet.h`
- `include/nuttx/net/tun.h`
- `include/nuttx/net/udp.h`
- `include/nuttx/net/usrsock.h`
- `include/nuttx/net/vlan.h`
- `include/nuttx/net/w5500.h`
- `include/nuttx/net/wifi_sim.h`
- `include/sys/socket.h`
- `include/sys/sockio.h`
- `include/sys/un.h`
- `libs/libc/net/Kconfig`
- `libs/libc/net/Make.defs`
- `libs/libc/net/lib_accept.c`
- `libs/libc/net/lib_addrconfig.c`
- `libs/libc/net/lib_addroute.c`
- `libs/libc/net/lib_base64.c`
- `libs/libc/net/lib_delroute.c`
- `libs/libc/net/lib_etheraton.c`
- `libs/libc/net/lib_etherntoa.c`
- `libs/libc/net/lib_freeifaddrs.c`
- `libs/libc/net/lib_freenameindex.c`
- `libs/libc/net/lib_getifaddrs.c`
- `libs/libc/net/lib_htonl.c`
- `libs/libc/net/lib_htonq.c`
- `libs/libc/net/lib_htons.c`
- `libs/libc/net/lib_indextoname.c`
- `libs/libc/net/lib_inetaddr.c`
- `libs/libc/net/lib_inetaton.c`
- `libs/libc/net/lib_inetnetwork.c`
- `libs/libc/net/lib_inetntoa.c`
- `libs/libc/net/lib_inetntop.c`
- `libs/libc/net/lib_inetpton.c`
- `libs/libc/net/lib_loopback.c`
- `libs/libc/net/lib_nameindex.c`
- `libs/libc/net/lib_nametoindex.c`
- `libs/libc/netdb/Kconfig`
- `libs/libc/netdb/Make.defs`
- `libs/libc/netdb/lib_dn.c`
- `libs/libc/netdb/lib_dns.h`
- `libs/libc/netdb/lib_dnsaddserver.c`
- `libs/libc/netdb/lib_dnsbind.c`
- `libs/libc/netdb/lib_dnscache.c`
- `libs/libc/netdb/lib_dnsdefaultserver.c`
- `libs/libc/netdb/lib_dnsdelserver.c`
- `libs/libc/netdb/lib_dnsforeach.c`
- `libs/libc/netdb/lib_dnsinit.c`
- `libs/libc/netdb/lib_dnsnotify.c`
- `libs/libc/netdb/lib_dnsquery.c`
- `libs/libc/netdb/lib_dnsqueryfamily.c`
- `libs/libc/netdb/lib_freeaddrinfo.c`
- `libs/libc/netdb/lib_gaistrerror.c`
- `libs/libc/netdb/lib_getaddrinfo.c`
- `libs/libc/netdb/lib_gethostbyaddr.c`
- `libs/libc/netdb/lib_gethostbyaddrr.c`
- `libs/libc/netdb/lib_gethostbyname.c`
- `libs/libc/netdb/lib_gethostbyname2.c`
- `libs/libc/netdb/lib_gethostbyname2r.c`
- `libs/libc/netdb/lib_gethostbynamer.c`
- `libs/libc/netdb/lib_gethostentbynamer.c`
- `libs/libc/netdb/lib_getnameinfo.c`
- `libs/libc/netdb/lib_getservbyname.c`
- `libs/libc/netdb/lib_getservbynamer.c`
- `libs/libc/netdb/lib_getservbyport.c`
- `libs/libc/netdb/lib_getservbyportr.c`
- `libs/libc/netdb/lib_netdb.c`
- `libs/libc/netdb/lib_netdb.h`
- `libs/libc/netdb/lib_parsehostfile.c`
- `libs/libc/netdb/lib_proto.c`
- `libs/libc/netdb/lib_protor.c`
- `libs/libc/netdb/lib_rexec.c`

</details>

### openamp (`*rpmsg*`, `*rptun*`) (19)

후속 결정(2026-11-02)으로 `include/nuttx/{ioexpander,rpmsg,rptun,serial,timers}`의 14개는 sync했습니다(`85dd59748`).
`include/netpacket`, `include/nuttx/net`, `include/nuttx/usrsock`, `include/nuttx/wireless`의 5개는 net·usrsock·wireless 영역이라 계속 제외입니다.

| 디렉터리 | 파일 |
|---|---|
| `include/netpacket` | 1 |
| `include/nuttx/ioexpander` | 1 |
| `include/nuttx/net` | 2 |
| `include/nuttx/rpmsg` | 6 |
| `include/nuttx/rptun` | 4 |
| `include/nuttx/serial` | 2 |
| `include/nuttx/timers` | 1 |
| `include/nuttx/usrsock` | 1 |
| `include/nuttx/wireless` | 1 |

<details><summary>파일 목록</summary>

- `include/netpacket/rpmsg.h`
- `include/nuttx/ioexpander/ioe_rpmsg.h`
- `include/nuttx/net/rpmsg.h`
- `include/nuttx/net/rpmsgdrv.h`
- `include/nuttx/rpmsg/rpmsg.h`
- `include/nuttx/rpmsg/rpmsg_ping.h`
- `include/nuttx/rpmsg/rpmsg_router.h`
- `include/nuttx/rpmsg/rpmsg_virtio.h`
- `include/nuttx/rpmsg/rpmsg_virtio_ivshmem.h`
- `include/nuttx/rpmsg/rpmsg_virtio_lite.h`
- `include/nuttx/rptun/openamp.h`
- `include/nuttx/rptun/rptun.h`
- `include/nuttx/rptun/rptun_bmp.h`
- `include/nuttx/rptun/rptun_ivshmem.h`
- `include/nuttx/serial/uart_rpmsg.h`
- `include/nuttx/serial/uart_rpmsg_raw.h`
- `include/nuttx/timers/rpmsg_rtc.h`
- `include/nuttx/usrsock/usrsock_rpmsg.h`
- `include/nuttx/wireless/bluetooth/bt_rpmsghci.h`

</details>

### crypto (`include/crypto`, `include/nuttx/crypto`) (25)

| 디렉터리 | 파일 |
|---|---|
| `include/crypto` | 22 |
| `include/nuttx/crypto` | 3 |

<details><summary>파일 목록</summary>

- `include/crypto/aes.h`
- `include/crypto/blf.h`
- `include/crypto/bn.h`
- `include/crypto/cast.h`
- `include/crypto/chachapoly.h`
- `include/crypto/cmac.h`
- `include/crypto/cryptodev.h`
- `include/crypto/cryptosoft.h`
- `include/crypto/curve25519.h`
- `include/crypto/ecc.h`
- `include/crypto/gmac.h`
- `include/crypto/hmac.h`
- `include/crypto/idgen.h`
- `include/crypto/key_wrap.h`
- `include/crypto/md5.h`
- `include/crypto/poly1305.h`
- `include/crypto/rijndael.h`
- `include/crypto/rmd160.h`
- `include/crypto/sha1.h`
- `include/crypto/sha2.h`
- `include/crypto/siphash.h`
- `include/crypto/xform.h`
- `include/nuttx/crypto/blake2s.h`
- `include/nuttx/crypto/crypto.h`
- `include/nuttx/crypto/se05x.h`

</details>

### wireless (`include/nuttx/wireless`) (39)

| 디렉터리 | 파일 |
|---|---|
| `include/nuttx/wireless` | 39 |

<details><summary>파일 목록</summary>

- `include/nuttx/wireless/bluetooth/bt_bridge.h`
- `include/nuttx/wireless/bluetooth/bt_buf.h`
- `include/nuttx/wireless/bluetooth/bt_core.h`
- `include/nuttx/wireless/bluetooth/bt_driver.h`
- `include/nuttx/wireless/bluetooth/bt_gatt.h`
- `include/nuttx/wireless/bluetooth/bt_hci.h`
- `include/nuttx/wireless/bluetooth/bt_ioctl.h`
- `include/nuttx/wireless/bluetooth/bt_null.h`
- `include/nuttx/wireless/bluetooth/bt_slip.h`
- `include/nuttx/wireless/bluetooth/bt_uart.h`
- `include/nuttx/wireless/bluetooth/bt_uart_shim.h`
- `include/nuttx/wireless/bluetooth/bt_uuid.h`
- `include/nuttx/wireless/cc1101.h`
- `include/nuttx/wireless/cellular/cellular.h`
- `include/nuttx/wireless/gs2200m.h`
- `include/nuttx/wireless/ieee80211/bcmf_board.h`
- `include/nuttx/wireless/ieee80211/bcmf_gpio.h`
- `include/nuttx/wireless/ieee80211/bcmf_gspi.h`
- `include/nuttx/wireless/ieee80211/bcmf_netdev.h`
- `include/nuttx/wireless/ieee80211/bcmf_sdio.h`
- `include/nuttx/wireless/ieee80211/ieee80211.h`
- `include/nuttx/wireless/ieee80211/ieee80211_cypto.h`
- `include/nuttx/wireless/ieee802154/at86rf23x.h`
- `include/nuttx/wireless/ieee802154/ieee802154_device.h`
- `include/nuttx/wireless/ieee802154/ieee802154_loopback.h`
- `include/nuttx/wireless/ieee802154/ieee802154_mac.h`
- `include/nuttx/wireless/ieee802154/ieee802154_radio.h`
- `include/nuttx/wireless/ieee802154/mrf24j40.h`
- `include/nuttx/wireless/ieee802154/xbee.h`
- `include/nuttx/wireless/ioctl.h`
- `include/nuttx/wireless/lpwan/rn2xx3.h`
- `include/nuttx/wireless/lpwan/sx126x.h`
- `include/nuttx/wireless/lpwan/sx127x.h`
- `include/nuttx/wireless/lte/lte.h`
- `include/nuttx/wireless/lte/lte_ioctl.h`
- `include/nuttx/wireless/nrf24l01.h`
- `include/nuttx/wireless/pktradio.h`
- `include/nuttx/wireless/spirit.h`
- `include/nuttx/wireless/wireless.h`

</details>

### pm (`include/nuttx/power/pm.h`) (1)

| 디렉터리 | 파일 |
|---|---|
| `include/nuttx/power` | 1 |

<details><summary>파일 목록</summary>

- `include/nuttx/power/pm.h`

</details>

### drivers 영역 헤더 (`audio`, `input`, `lcd`, `sensors`, `usb`, `video`, `power`, `uorb.h`, `sys/video*`) (262)

후속 결정(2026-11-02)으로 `include/nuttx/usb`의 31개는 sync했습니다(`1c37bfac3`). 나머지 231개는 계속 제외입니다.

| 디렉터리 | 파일 |
|---|---|
| `include/nuttx` | 1 |
| `include/nuttx/audio` | 17 |
| `include/nuttx/input` | 30 |
| `include/nuttx/lcd` | 39 |
| `include/nuttx/power` | 17 |
| `include/nuttx/sensors` | 104 |
| `include/nuttx/usb` | 31 |
| `include/nuttx/video` | 21 |
| `include/sys` | 2 |

<details><summary>파일 목록</summary>

- `include/nuttx/audio/audio.h`
- `include/nuttx/audio/audio_comp.h`
- `include/nuttx/audio/audio_dma.h`
- `include/nuttx/audio/audio_fake.h`
- `include/nuttx/audio/audio_i2s.h`
- `include/nuttx/audio/audio_null.h`
- `include/nuttx/audio/cs4344.h`
- `include/nuttx/audio/cs43l22.h`
- `include/nuttx/audio/es8311.h`
- `include/nuttx/audio/es8388.h`
- `include/nuttx/audio/i2s.h`
- `include/nuttx/audio/pcm.h`
- `include/nuttx/audio/tone.h`
- `include/nuttx/audio/vs1053.h`
- `include/nuttx/audio/wm8776.h`
- `include/nuttx/audio/wm8904.h`
- `include/nuttx/audio/wm8994.h`
- `include/nuttx/input/ads7843e.h`
- `include/nuttx/input/ajoystick.h`
- `include/nuttx/input/aw86225.h`
- `include/nuttx/input/buttons.h`
- `include/nuttx/input/cypress_mbr3108.h`
- `include/nuttx/input/djoystick.h`
- `include/nuttx/input/ff.h`
- `include/nuttx/input/ff_dummy.h`
- `include/nuttx/input/ft5x06.h`
- `include/nuttx/input/goldfish_events.h`
- `include/nuttx/input/gt9xx.h`
- `include/nuttx/input/ioctl.h`
- `include/nuttx/input/kbd_codec.h`
- `include/nuttx/input/keyboard.h`
- `include/nuttx/input/kmatrix.h`
- `include/nuttx/input/max11802.h`
- `include/nuttx/input/mouse.h`
- `include/nuttx/input/mxt.h`
- `include/nuttx/input/nunchuck.h`
- `include/nuttx/input/sbutton.h`
- `include/nuttx/input/spq10kbd.h`
- `include/nuttx/input/stmpe811.h`
- `include/nuttx/input/touchscreen.h`
- `include/nuttx/input/tsc2007.h`
- `include/nuttx/input/uinput.h`
- `include/nuttx/input/virtio-input-event-codes.h`
- `include/nuttx/input/x11_keysym.h`
- `include/nuttx/input/x11_keysymdef.h`
- `include/nuttx/input/x11_xf86keysym.h`
- `include/nuttx/input/xbox-controller.h`
- `include/nuttx/lcd/apa102.h`
- `include/nuttx/lcd/ft80x.h`
- `include/nuttx/lcd/gc9a01.h`
- `include/nuttx/lcd/hd4478ou.h`
- `include/nuttx/lcd/ht16k33.h`
- `include/nuttx/lcd/ili9225.h`
- `include/nuttx/lcd/ili9325.h`
- `include/nuttx/lcd/ili9340.h`
- `include/nuttx/lcd/ili9341.h`
- `include/nuttx/lcd/ili9488.h`
- `include/nuttx/lcd/jd9851.h`
- `include/nuttx/lcd/lcd.h`
- `include/nuttx/lcd/lcd_dev.h`
- `include/nuttx/lcd/lcd_ioctl.h`
- `include/nuttx/lcd/lcddrv_spiif.h`
- `include/nuttx/lcd/lpm013m091a.h`
- `include/nuttx/lcd/max7219.h`
- `include/nuttx/lcd/memlcd.h`
- `include/nuttx/lcd/mio283qt2.h`
- `include/nuttx/lcd/mio283qt9a.h`
- `include/nuttx/lcd/p14201.h`
- `include/nuttx/lcd/pcd8544.h`
- `include/nuttx/lcd/pcf8574_lcd_backpack.h`
- `include/nuttx/lcd/ra8875.h`
- `include/nuttx/lcd/slcd_codec.h`
- `include/nuttx/lcd/slcd_ioctl.h`
- `include/nuttx/lcd/ssd1289.h`
- `include/nuttx/lcd/ssd1306.h`
- `include/nuttx/lcd/ssd1351.h`
- `include/nuttx/lcd/ssd1680.h`
- `include/nuttx/lcd/st7032.h`
- `include/nuttx/lcd/st7565.h`
- `include/nuttx/lcd/st7567.h`
- `include/nuttx/lcd/st7735.h`
- `include/nuttx/lcd/st7789.h`
- `include/nuttx/lcd/st7796.h`
- `include/nuttx/lcd/tda19988.h`
- `include/nuttx/lcd/ug-2864ambag01.h`
- `include/nuttx/lcd/ug-9664hswag01.h`
- `include/nuttx/power/act8945a.h`
- `include/nuttx/power/axp202.h`
- `include/nuttx/power/battery_charger.h`
- `include/nuttx/power/battery_gauge.h`
- `include/nuttx/power/battery_ioctl.h`
- `include/nuttx/power/battery_monitor.h`
- `include/nuttx/power/bq2429x.h`
- `include/nuttx/power/bq27426.h`
- `include/nuttx/power/bq769x0.h`
- `include/nuttx/power/consumer.h`
- `include/nuttx/power/mcp73871.h`
- `include/nuttx/power/pm_runtime.h`
- `include/nuttx/power/power_ioctl.h`
- `include/nuttx/power/powerled.h`
- `include/nuttx/power/regulator.h`
- `include/nuttx/power/relay.h`
- `include/nuttx/power/smps.h`
- `include/nuttx/sensors/adt7320.h`
- `include/nuttx/sensors/adxl345.h`
- `include/nuttx/sensors/adxl362.h`
- `include/nuttx/sensors/adxl372.h`
- `include/nuttx/sensors/aht10.h`
- `include/nuttx/sensors/ak09912.h`
- `include/nuttx/sensors/amg88xx.h`
- `include/nuttx/sensors/apds9922.h`
- `include/nuttx/sensors/apds9930.h`
- `include/nuttx/sensors/apds9960.h`
- `include/nuttx/sensors/as5048a.h`
- `include/nuttx/sensors/as5048b.h`
- `include/nuttx/sensors/as726x.h`
- `include/nuttx/sensors/bh1721fvc.h`
- `include/nuttx/sensors/bh1745nuc.h`
- `include/nuttx/sensors/bh1749nuc.h`
- `include/nuttx/sensors/bh1750fvi.h`
- `include/nuttx/sensors/bm1383glv.h`
- `include/nuttx/sensors/bm1422gmv.h`
- `include/nuttx/sensors/bme680.h`
- `include/nuttx/sensors/bme688.h`
- `include/nuttx/sensors/bmg160.h`
- `include/nuttx/sensors/bmi088.h`
- `include/nuttx/sensors/bmi160.h`
- `include/nuttx/sensors/bmi270.h`
- `include/nuttx/sensors/bmm150.h`
- `include/nuttx/sensors/bmp180.h`
- `include/nuttx/sensors/bmp280.h`
- `include/nuttx/sensors/cluster_driver.h`
- `include/nuttx/sensors/cxd5602pwbimu.h`
- `include/nuttx/sensors/cxd5610_gnss.h`
- `include/nuttx/sensors/dhtxx.h`
- `include/nuttx/sensors/ds18b20.h`
- `include/nuttx/sensors/fakesensor.h`
- `include/nuttx/sensors/fxos8700cq.h`
- `include/nuttx/sensors/gnss.h`
- `include/nuttx/sensors/goldfish_gnss.h`
- `include/nuttx/sensors/goldfish_sensor.h`
- `include/nuttx/sensors/hall3ph.h`
- `include/nuttx/sensors/hc_sr04.h`
- `include/nuttx/sensors/hdc1008.h`
- `include/nuttx/sensors/hts221.h`
- `include/nuttx/sensors/hyt271.h`
- `include/nuttx/sensors/ina219.h`
- `include/nuttx/sensors/ina226.h`
- `include/nuttx/sensors/ina3221.h`
- `include/nuttx/sensors/ioctl.h`
- `include/nuttx/sensors/isl29023.h`
- `include/nuttx/sensors/kx022.h`
- `include/nuttx/sensors/kxtj9.h`
- `include/nuttx/sensors/l3gd20.h`
- `include/nuttx/sensors/l86xxx.h`
- `include/nuttx/sensors/lis2dh.h`
- `include/nuttx/sensors/lis2mdl.h`
- `include/nuttx/sensors/lis331dl.h`
- `include/nuttx/sensors/lis3dh.h`
- `include/nuttx/sensors/lis3dsh.h`
- `include/nuttx/sensors/lis3mdl.h`
- `include/nuttx/sensors/lm75.h`
- `include/nuttx/sensors/lm92.h`
- `include/nuttx/sensors/lps25h.h`
- `include/nuttx/sensors/lsm303agr.h`
- `include/nuttx/sensors/lsm330.h`
- `include/nuttx/sensors/lsm6dsl.h`
- `include/nuttx/sensors/lsm6dso32.h`
- `include/nuttx/sensors/lsm9ds1.h`
- `include/nuttx/sensors/lt1pa01.h`
- `include/nuttx/sensors/ltc4151.h`
- `include/nuttx/sensors/ltr308.h`
- `include/nuttx/sensors/max31855.h`
- `include/nuttx/sensors/max31865.h`
- `include/nuttx/sensors/max44009.h`
- `include/nuttx/sensors/max6675.h`
- `include/nuttx/sensors/mb7040.h`
- `include/nuttx/sensors/mcp9600.h`
- `include/nuttx/sensors/mcp9844.h`
- `include/nuttx/sensors/mlx90393.h`
- `include/nuttx/sensors/mlx90614.h`
- `include/nuttx/sensors/mpl115a.h`
- `include/nuttx/sensors/mpu60x0.h`
- `include/nuttx/sensors/mpu9250.h`
- `include/nuttx/sensors/ms56xx.h`
- `include/nuttx/sensors/ms58xx.h`
- `include/nuttx/sensors/msa301.h`
- `include/nuttx/sensors/msxxxx_crc4.h`
- `include/nuttx/sensors/nau7802.h`
- `include/nuttx/sensors/qencoder.h`
- `include/nuttx/sensors/qmi8658.h`
- `include/nuttx/sensors/rpr0521rs.h`
- `include/nuttx/sensors/scd30.h`
- `include/nuttx/sensors/scd41.h`
- `include/nuttx/sensors/sensor.h`
- `include/nuttx/sensors/sgp30.h`
- `include/nuttx/sensors/sht21.h`
- `include/nuttx/sensors/sht3x.h`
- `include/nuttx/sensors/sht4x.h`
- `include/nuttx/sensors/sps30.h`
- `include/nuttx/sensors/t67xx.h`
- `include/nuttx/sensors/tmp112.h`
- `include/nuttx/sensors/veml6070.h`
- `include/nuttx/sensors/vl53l1x.h`
- `include/nuttx/sensors/wtgahrs2.h`
- `include/nuttx/sensors/xen1210.h`
- `include/nuttx/sensors/zerocross.h`
- `include/nuttx/uorb.h`
- `include/nuttx/usb/adb.h`
- `include/nuttx/usb/audio.h`
- `include/nuttx/usb/cdc.h`
- `include/nuttx/usb/cdcacm.h`
- `include/nuttx/usb/cdcecm.h`
- `include/nuttx/usb/cdcncm.h`
- `include/nuttx/usb/composite.h`
- `include/nuttx/usb/dfu.h`
- `include/nuttx/usb/ehci.h`
- `include/nuttx/usb/fusb301.h`
- `include/nuttx/usb/fusb302.h`
- `include/nuttx/usb/fusb303.h`
- `include/nuttx/usb/hid.h`
- `include/nuttx/usb/hid_parser.h`
- `include/nuttx/usb/hub.h`
- `include/nuttx/usb/max3421e.h`
- `include/nuttx/usb/mtp.h`
- `include/nuttx/usb/ohci.h`
- `include/nuttx/usb/pl2303.h`
- `include/nuttx/usb/rndis.h`
- `include/nuttx/usb/storage.h`
- `include/nuttx/usb/stusb4500.h`
- `include/nuttx/usb/usb.h`
- `include/nuttx/usb/usbdev.h`
- `include/nuttx/usb/usbdev_trace.h`
- `include/nuttx/usb/usbhost.h`
- `include/nuttx/usb/usbhost_devaddr.h`
- `include/nuttx/usb/usbhost_trace.h`
- `include/nuttx/usb/usbmonitor.h`
- `include/nuttx/usb/usbmsc.h`
- `include/nuttx/usb/xhci_pci.h`
- `include/nuttx/video/edid.h`
- `include/nuttx/video/fb.h`
- `include/nuttx/video/goldfish_camera.h`
- `include/nuttx/video/goldfish_fb.h`
- `include/nuttx/video/goldfish_gpu_fb.h`
- `include/nuttx/video/imgdata.h`
- `include/nuttx/video/imgsensor.h`
- `include/nuttx/video/isx012.h`
- `include/nuttx/video/isx019.h`
- `include/nuttx/video/max7456.h`
- `include/nuttx/video/mipi_display.h`
- `include/nuttx/video/mipi_dsi.h`
- `include/nuttx/video/ov2640.h`
- `include/nuttx/video/rfb.h`
- `include/nuttx/video/rgbcolors.h`
- `include/nuttx/video/v4l2_cap.h`
- `include/nuttx/video/v4l2_m2m.h`
- `include/nuttx/video/vesagtf.h`
- `include/nuttx/video/video.h`
- `include/nuttx/video/videomode.h`
- `include/nuttx/video/vnc.h`
- `include/sys/video_controls.h`
- `include/sys/videoio.h`

</details>

### graphics (`libs/libnx`, `include/nuttx/nx`) (173)

| 디렉터리 | 파일 |
|---|---|
| `include/nuttx/nx` | 9 |
| `libs/libnx` | 3 |
| `libs/libnx/nx` | 6 |
| `libs/libnx/nxfonts` | 52 |
| `libs/libnx/nxglib` | 29 |
| `libs/libnx/nxmu` | 34 |
| `libs/libnx/nxtk` | 40 |

<details><summary>파일 목록</summary>

- `include/nuttx/nx/nx.h`
- `include/nuttx/nx/nxbe.h`
- `include/nuttx/nx/nxcursor.h`
- `include/nuttx/nx/nxfonts.h`
- `include/nuttx/nx/nxglib.h`
- `include/nuttx/nx/nxmu.h`
- `include/nuttx/nx/nxterm.h`
- `include/nuttx/nx/nxtk.h`
- `include/nuttx/nx/nxtypes.h`
- `libs/libnx/Kconfig`
- `libs/libnx/Makefile`
- `libs/libnx/nx/Kconfig`
- `libs/libnx/nx/Make.defs`
- `libs/libnx/nx/nx_drawcircle.c`
- `libs/libnx/nx/nx_drawline.c`
- `libs/libnx/nx/nx_fillcircle.c`
- `libs/libnx/nx/nx_ishidden.c`
- `libs/libnx/nxcontext.h`
- `libs/libnx/nxfonts/.gitignore`
- `libs/libnx/nxfonts/Kconfig`
- `libs/libnx/nxfonts/Make.defs`
- `libs/libnx/nxfonts/Makefile.sources`
- `libs/libnx/nxfonts/nxfonts.h`
- `libs/libnx/nxfonts/nxfonts_bitmaps.c`
- `libs/libnx/nxfonts/nxfonts_cache.c`
- `libs/libnx/nxfonts/nxfonts_convert.c`
- `libs/libnx/nxfonts/nxfonts_getfont.c`
- `libs/libnx/nxfonts/nxfonts_mono5x8.h`
- `libs/libnx/nxfonts/nxfonts_pixel-lcd-machine.h`
- `libs/libnx/nxfonts/nxfonts_pixel-unicode.h`
- `libs/libnx/nxfonts/nxfonts_sans17x22.h`
- `libs/libnx/nxfonts/nxfonts_sans17x23b.h`
- `libs/libnx/nxfonts/nxfonts_sans20x26.h`
- `libs/libnx/nxfonts/nxfonts_sans20x27b.h`
- `libs/libnx/nxfonts/nxfonts_sans22x29.h`
- `libs/libnx/nxfonts/nxfonts_sans22x29b.h`
- `libs/libnx/nxfonts/nxfonts_sans23x27.h`
- `libs/libnx/nxfonts/nxfonts_sans28x37.h`
- `libs/libnx/nxfonts/nxfonts_sans28x37b.h`
- `libs/libnx/nxfonts/nxfonts_sans39x48.h`
- `libs/libnx/nxfonts/nxfonts_sans40x49b.h`
- `libs/libnx/nxfonts/nxfonts_serif22x28b.h`
- `libs/libnx/nxfonts/nxfonts_serif22x29.h`
- `libs/libnx/nxfonts/nxfonts_serif27x38b.h`
- `libs/libnx/nxfonts/nxfonts_serif29x37.h`
- `libs/libnx/nxfonts/nxfonts_serif38x48.h`
- `libs/libnx/nxfonts/nxfonts_serif38x49b.h`
- `libs/libnx/nxfonts/nxfonts_tom-thumb-4x6.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-10x20.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-4x6.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-5x7.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-5x8.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x10.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x12.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x13.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x13b.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x13o.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-6x9.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-7x13.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-7x13b.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-7x13o.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-7x14.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-7x14b.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-8x13.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-8x13b.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-8x13o.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-9x15.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-9x15b.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-9x18.h`
- `libs/libnx/nxfonts/nxfonts_x11-misc-fixed-9x18b.h`
- `libs/libnx/nxglib/Kconfig`
- `libs/libnx/nxglib/Make.defs`
- `libs/libnx/nxglib/nxgl_area2rect.c`
- `libs/libnx/nxglib/nxgl_rect2area.c`
- `libs/libnx/nxglib/nxglib_circlepts.c`
- `libs/libnx/nxglib/nxglib_circletraps.c`
- `libs/libnx/nxglib/nxglib_colorcmp.c`
- `libs/libnx/nxglib/nxglib_colorcopy.c`
- `libs/libnx/nxglib/nxglib_intersecting.c`
- `libs/libnx/nxglib/nxglib_nonintersecting.c`
- `libs/libnx/nxglib/nxglib_nullrect.c`
- `libs/libnx/nxglib/nxglib_rectadd.c`
- `libs/libnx/nxglib/nxglib_rectcopy.c`
- `libs/libnx/nxglib/nxglib_rectinside.c`
- `libs/libnx/nxglib/nxglib_rectintersect.c`
- `libs/libnx/nxglib/nxglib_rectoffset.c`
- `libs/libnx/nxglib/nxglib_rectoverlap.c`
- `libs/libnx/nxglib/nxglib_rectsize.c`
- `libs/libnx/nxglib/nxglib_rectunion.c`
- `libs/libnx/nxglib/nxglib_rgb2yuv.c`
- `libs/libnx/nxglib/nxglib_rgbblend.c`
- `libs/libnx/nxglib/nxglib_runcopy.c`
- `libs/libnx/nxglib/nxglib_runoffset.c`
- `libs/libnx/nxglib/nxglib_splitline.c`
- `libs/libnx/nxglib/nxglib_trapcopy.c`
- `libs/libnx/nxglib/nxglib_trapoffset.c`
- `libs/libnx/nxglib/nxglib_vectoradd.c`
- `libs/libnx/nxglib/nxglib_vectsubtract.c`
- `libs/libnx/nxglib/nxglib_yuv2rgb.c`
- `libs/libnx/nxmu/Kconfig`
- `libs/libnx/nxmu/Make.defs`
- `libs/libnx/nxmu/nx_bitmap.c`
- `libs/libnx/nxmu/nx_block.c`
- `libs/libnx/nxmu/nx_closewindow.c`
- `libs/libnx/nxmu/nx_connect.c`
- `libs/libnx/nxmu/nx_constructwindow.c`
- `libs/libnx/nxmu/nx_cursor.c`
- `libs/libnx/nxmu/nx_disconnect.c`
- `libs/libnx/nxmu/nx_eventhandler.c`
- `libs/libnx/nxmu/nx_eventnotify.c`
- `libs/libnx/nxmu/nx_fill.c`
- `libs/libnx/nxmu/nx_filltrapezoid.c`
- `libs/libnx/nxmu/nx_getposition.c`
- `libs/libnx/nxmu/nx_getrectangle.c`
- `libs/libnx/nxmu/nx_kbdchin.c`
- `libs/libnx/nxmu/nx_kbdin.c`
- `libs/libnx/nxmu/nx_lower.c`
- `libs/libnx/nxmu/nx_modal.c`
- `libs/libnx/nxmu/nx_mousein.c`
- `libs/libnx/nxmu/nx_move.c`
- `libs/libnx/nxmu/nx_openwindow.c`
- `libs/libnx/nxmu/nx_raise.c`
- `libs/libnx/nxmu/nx_redrawreq.c`
- `libs/libnx/nxmu/nx_releasebkgd.c`
- `libs/libnx/nxmu/nx_requestbkgd.c`
- `libs/libnx/nxmu/nx_setbgcolor.c`
- `libs/libnx/nxmu/nx_setpixel.c`
- `libs/libnx/nxmu/nx_setposition.c`
- `libs/libnx/nxmu/nx_setsize.c`
- `libs/libnx/nxmu/nx_setvisibility.c`
- `libs/libnx/nxmu/nx_synch.c`
- `libs/libnx/nxmu/nxmu_sendserver.c`
- `libs/libnx/nxmu/nxmu_sendwindow.c`
- `libs/libnx/nxtk/Kconfig`
- `libs/libnx/nxtk/Make.defs`
- `libs/libnx/nxtk/nxtk.h`
- `libs/libnx/nxtk/nxtk_bitmaptoolbar.c`
- `libs/libnx/nxtk/nxtk_bitmapwindow.c`
- `libs/libnx/nxtk/nxtk_block.c`
- `libs/libnx/nxtk/nxtk_closetoolbar.c`
- `libs/libnx/nxtk/nxtk_closewindow.c`
- `libs/libnx/nxtk/nxtk_containerclip.c`
- `libs/libnx/nxtk/nxtk_drawcircletoolbar.c`
- `libs/libnx/nxtk/nxtk_drawcirclewindow.c`
- `libs/libnx/nxtk/nxtk_drawframe.c`
- `libs/libnx/nxtk/nxtk_drawlinetoolbar.c`
- `libs/libnx/nxtk/nxtk_drawlinewindow.c`
- `libs/libnx/nxtk/nxtk_events.c`
- `libs/libnx/nxtk/nxtk_fillcircletoolbar.c`
- `libs/libnx/nxtk/nxtk_fillcirclewindow.c`
- `libs/libnx/nxtk/nxtk_filltoolbar.c`
- `libs/libnx/nxtk/nxtk_filltraptoolbar.c`
- `libs/libnx/nxtk/nxtk_filltrapwindow.c`
- `libs/libnx/nxtk/nxtk_fillwindow.c`
- `libs/libnx/nxtk/nxtk_getposition.c`
- `libs/libnx/nxtk/nxtk_gettoolbar.c`
- `libs/libnx/nxtk/nxtk_getwindow.c`
- `libs/libnx/nxtk/nxtk_ishidden.c`
- `libs/libnx/nxtk/nxtk_lower.c`
- `libs/libnx/nxtk/nxtk_modal.c`
- `libs/libnx/nxtk/nxtk_movetoolbar.c`
- `libs/libnx/nxtk/nxtk_movewindow.c`
- `libs/libnx/nxtk/nxtk_opentoolbar.c`
- `libs/libnx/nxtk/nxtk_openwindow.c`
- `libs/libnx/nxtk/nxtk_raise.c`
- `libs/libnx/nxtk/nxtk_setposition.c`
- `libs/libnx/nxtk/nxtk_setsize.c`
- `libs/libnx/nxtk/nxtk_setsubwindows.c`
- `libs/libnx/nxtk/nxtk_setvisibility.c`
- `libs/libnx/nxtk/nxtk_subwindowclip.c`
- `libs/libnx/nxtk/nxtk_subwindowmove.c`
- `libs/libnx/nxtk/nxtk_synch.c`
- `libs/libnx/nxtk/nxtk_toolbarbounds.c`

</details>

### libc/machine: TizenRT에 없는 arch (81)

| 디렉터리 | 파일 |
|---|---|
| `libs/libc/machine/arm/arm` | 3 |
| `libs/libc/machine/arm/armv6-m` | 3 |
| `libs/libc/machine/arm/armv8-r` | 10 |
| `libs/libc/machine/arm64` | 18 |
| `libs/libc/machine/renesas` | 5 |
| `libs/libc/machine/risc-v` | 8 |
| `libs/libc/machine/sim` | 10 |
| `libs/libc/machine/sparc` | 3 |
| `libs/libc/machine/x86` | 3 |
| `libs/libc/machine/x86_64` | 18 |

<details><summary>파일 목록</summary>

- `libs/libc/machine/arm/arm/Kconfig`
- `libs/libc/machine/arm/arm/Make.defs`
- `libs/libc/machine/arm/arm/arch_elf.c`
- `libs/libc/machine/arm/armv6-m/Kconfig`
- `libs/libc/machine/arm/armv6-m/Make.defs`
- `libs/libc/machine/arm/armv6-m/arch_elf.c`
- `libs/libc/machine/arm/armv8-r/Kconfig`
- `libs/libc/machine/arm/armv8-r/Make.defs`
- `libs/libc/machine/arm/armv8-r/acle-compat.h`
- `libs/libc/machine/arm/armv8-r/arch_elf.c`
- `libs/libc/machine/arm/armv8-r/arch_memchr.S`
- `libs/libc/machine/arm/armv8-r/arch_memcpy.S`
- `libs/libc/machine/arm/armv8-r/arch_memmove.S`
- `libs/libc/machine/arm/armv8-r/arch_memset.S`
- `libs/libc/machine/arm/armv8-r/arch_strcmp.S`
- `libs/libc/machine/arm/armv8-r/arch_strlen.S`
- `libs/libc/machine/arm64/Kconfig`
- `libs/libc/machine/arm64/Make.defs`
- `libs/libc/machine/arm64/arch_elf.c`
- `libs/libc/machine/arm64/arch_mcount.c`
- `libs/libc/machine/arm64/arch_memchr.S`
- `libs/libc/machine/arm64/arch_memcmp.S`
- `libs/libc/machine/arm64/arch_memcpy.S`
- `libs/libc/machine/arm64/arch_memmove.S`
- `libs/libc/machine/arm64/arch_memset.S`
- `libs/libc/machine/arm64/arch_setjmp.S`
- `libs/libc/machine/arm64/arch_strchr.S`
- `libs/libc/machine/arm64/arch_strchrnul.S`
- `libs/libc/machine/arm64/arch_strcmp.S`
- `libs/libc/machine/arm64/arch_strcpy.S`
- `libs/libc/machine/arm64/arch_strlen.S`
- `libs/libc/machine/arm64/arch_strncmp.S`
- `libs/libc/machine/arm64/arch_strnlen.S`
- `libs/libc/machine/arm64/arch_strrchr.S`
- `libs/libc/machine/renesas/Kconfig`
- `libs/libc/machine/renesas/Make.defs`
- `libs/libc/machine/renesas/rx/Kconfig`
- `libs/libc/machine/renesas/rx/Make.defs`
- `libs/libc/machine/renesas/rx/arch_setjmp.S`
- `libs/libc/machine/risc-v/Kconfig`
- `libs/libc/machine/risc-v/Make.defs`
- `libs/libc/machine/risc-v/arch_elf.c`
- `libs/libc/machine/risc-v/arch_memcpy.S`
- `libs/libc/machine/risc-v/arch_memset.S`
- `libs/libc/machine/risc-v/arch_setjmp.S`
- `libs/libc/machine/risc-v/arch_strcmp.S`
- `libs/libc/machine/risc-v/asm.h`
- `libs/libc/machine/sim/Kconfig`
- `libs/libc/machine/sim/Make.defs`
- `libs/libc/machine/sim/arch_elf.c`
- `libs/libc/machine/sim/arch_elf64.c`
- `libs/libc/machine/sim/arch_setjmp_arm.S`
- `libs/libc/machine/sim/arch_setjmp_arm64.S`
- `libs/libc/machine/sim/arch_setjmp_x86.S`
- `libs/libc/machine/sim/arch_setjmp_x86.asm`
- `libs/libc/machine/sim/arch_setjmp_x86_64.S`
- `libs/libc/machine/sim/arch_setjmp_x86_64.asm`
- `libs/libc/machine/sparc/Kconfig`
- `libs/libc/machine/sparc/Make.defs`
- `libs/libc/machine/sparc/arch_elf.c`
- `libs/libc/machine/x86/Kconfig`
- `libs/libc/machine/x86/Make.defs`
- `libs/libc/machine/x86/arch_elf.c`
- `libs/libc/machine/x86_64/Kconfig`
- `libs/libc/machine/x86_64/Make.defs`
- `libs/libc/machine/x86_64/arch_elf64.c`
- `libs/libc/machine/x86_64/arch_memcmp.S`
- `libs/libc/machine/x86_64/arch_memmove.S`
- `libs/libc/machine/x86_64/arch_memset_avx2.S`
- `libs/libc/machine/x86_64/arch_memset_sse2.S`
- `libs/libc/machine/x86_64/arch_setjmp_x86_64.S`
- `libs/libc/machine/x86_64/arch_stpcpy.S`
- `libs/libc/machine/x86_64/arch_stpncpy.S`
- `libs/libc/machine/x86_64/arch_strcat.S`
- `libs/libc/machine/x86_64/arch_strcmp.S`
- `libs/libc/machine/x86_64/arch_strcpy.S`
- `libs/libc/machine/x86_64/arch_strlen.S`
- `libs/libc/machine/x86_64/arch_strncmp.S`
- `libs/libc/machine/x86_64/arch_strncpy.S`
- `libs/libc/machine/x86_64/cache.h`
- `libs/libc/machine/x86_64/mcount.S`

</details>

## 8. 후속 결정 반영 (2026-11-02)

리뷰 뒤 사용자 결정으로 sync한 파일입니다. 결정 전체는 [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md)의 "후속 결정"을 봅니다.
include/, libs/ 밖의 파일(드라이버, fs, arch, 보드)도 함께 sync했으므로 같이 적습니다.

| 영역 | 이 리뷰 범위 (include/, libs/) | 그 밖에 함께 sync | 커밋 |
|---|---|---|---|
| rpmsg/rptun (openamp) | `include/nuttx/{rpmsg,rptun}` 10, `ioexpander/ioe_rpmsg.h`, `serial/uart_rpmsg{,_raw}.h`, `timers/rpmsg_rtc.h` (14) | `drivers/rpmsg` 19, `drivers/rptun` 5, `fs/rpmsgfs` 5, sync된 드라이버 폴더의 rpmsg 파일 19 | `85dd59748`..`b8e765991` |
| usb | `include/nuttx/usb` 31 | `drivers/usbdev` 28, `drivers/usbhost` 29, `drivers/usbmisc` 6, `drivers/usbmonitor` 3 | `1c37bfac3`..`536d8ae05` |
| armv7-m/armv7-r | `libs/libc/machine/arm/armv7-{m,r}` 21 | `arch/arm/{src,include}/armv7-{m,r}`, tiva, 보드 lm3s6965-ek·qemu-armv7r·mps2-an500 | `d9450e6f6`..`1d3c8e7f5` (ARMV7 cherry-pick) |

계속 제외하는 것:
- `openamp/`
- 미sync 영역의 rpmsg 파일: net, usrsock, wireless, sensors, power, 그리고 TizenRT에 폴더가 없는 clk, reset, virtio, vhost (TD-RPMSG-01)
- audio를 포함한 drivers 영역 헤더 231개
- libc/machine xtensa 13개 (TD-LIBS-06)

6장에서 바뀐 점:
- `include/nuttx/usb/*`를 include하는 sync 파일(boardctl.c, syslog_channel.c, assert.c, drivers_initialize.c, lib_buffer.c)은 이제 NuttX 12.13.0 헤더를 씁니다.
- `include/netpacket/rpmsg.h`(optee_socket.c, TD-INC-07), `include/nuttx/net/rpmsgdrv.h`, `include/nuttx/usrsock/usrsock_rpmsg.h`는 그대로입니다.

`~/project/nuttx/nuttx-12.13.0___NO_SYNCED` 브랜치는 이 결정들을 반영해 다시 만들었습니다. 이 문서의 4~7장 수치는 리뷰 당시(`9779f82204`) 기준입니다.
