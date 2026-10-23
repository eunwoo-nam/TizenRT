# NuttX 12.13.0 → TizenRT 마이그레이션 최종 결과 리포트

| 항목 | 내용 |
|---|---|
| 작성일 | 2026-10-23 |
| 브랜치 | `claude/nuttx-tizenrt-migration-nqry9a` (기준 master `1996f41e`) |
| 원본 | NuttX `nuttx-12.13.0` (b9b5d9afbf5a), nuttx-apps 12.13.0 (e4b84b2) |
| 대상 | TizenRT `os/`, `apps/examples/testcase/ostest`, `build/configs`, `tools/docker` |
| 검증 보드 | qemu-armv7a (Cortex-A7), mps2-an521 (Cortex-M33), FLAT 빌드 |
| 함께 볼 문서 | [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md) (구조·규칙·차이 목록), [qemu-armv7a README](../build/configs/qemu-armv7a/README.md), [mps2-an521 README](../build/configs/mps2-an521/README.md), [HANDOFF.md](../HANDOFF.md) |

## 1. 요약

NuttX 12.13.0 코드를 TizenRT에 가져왔습니다. TizenRT 빌드시스템, Kconfig·defconfig 방식, `os/kernel`과 `include/tinyara` 이름은 그대로 유지했습니다.
두 QEMU 보드는 4개 구성 모두 빌드되고 부팅되며, NuttX ostest를 통과합니다.

| 보드 / config | 빌드 | QEMU 실행 결과 |
|---|---|---|
| qemu-armv7a / `ostest` | 성공 | 33개 테스트 그룹 통과, `ostest_main: Exiting with status 0`, 이후 QEMU 자동 종료(poweroff) |
| qemu-armv7a / `tash` | 성공 | `TASH>>`에서 `ostest` 실행 → 33개 그룹 통과, `Exiting with status 0` |
| mps2-an521 / `ostest` | 성공 | 31개 그룹 통과, `Exiting with status 0` (보드에 poweroff가 없어 QEMU는 계속 실행) |
| mps2-an521 / `tash` | 성공 | `TASH>>`에서 `ostest` 실행 → 31개 그룹 통과, `Exiting with status 0` |

| 지표 | 값 |
|---|---|
| NuttX와 동일한 싱크 파일 (tizenrt-nuttx-similarity core) | **3858 / 3882** (나머지 24개는 의도적 수정, 6.1절) |
| cformatter | 싱크한 C 파일에 적용(실제로 바뀐 파일 2964개), 에러 0, [M07] 경고 274개(수동 수정 안 함) |
| 지수 숫자 분리 검사(`1e - 1`) | 0건 |
| 커밋 | 작업 커밋 42개(이 리포트 포함), 그 뒤마다 HANDOFF 커밋. 모두 브랜치에 푸쉬했고 master 푸쉬는 없음 |
| 변경 규모 (master 대비, `1268b92d` 기준) | 4633개 파일: 추가 2963, 수정 737, 삭제 668, 이동 265 |
| cformatter 툴 브랜치 수정 | `3463f629`, `51e698d9` (`agent/261022_improve-cformatter-tool`) |

## 2. 검증 결과

### 2.1 QEMU ostest

- 실행 환경: `tizenrt/tizenrt:2.0.1-qemu` 이미지 (Ubuntu 16.04.3, arm-none-eabi-gcc 10.3.1, QEMU 8.2.2).
- 판정 기준: NuttX CI(test_os.py)와 같이 `ostest_main: Exiting with status 0`가 출력되어야 합니다. 로그에 `ERROR`/`FAIL`/`ASSERT`/`PANIC`도 없어야 합니다.
- 테스트 그룹 수 차이(33 vs 31): mps2-an521은 NuttX의 nsh 설정을 따랐습니다. 그 설정에는 `ARCH_FPU`와 `PRIORITY_INHERITANCE`가 없어서 FPU 테스트와 우선순위 상속 테스트가 빠집니다.
- 실행 명령: 보드별 `build/configs/<board>/qemu_run.sh`를 씁니다. 자세한 내용은 8절에 있습니다.

최종 확인은 리포트 직전 커밋 `28d774bd`에서 했습니다. 4개 구성을 처음부터 다시 빌드(distclean → configure → make)하고 QEMU에서 실행했습니다.

| config | tinyara.bin | 테스트 그룹 | 결과 | QEMU 종료 |
|---|---|---|---|---|
| qemu-armv7a / `ostest` | 390,184 B | 33 | `Exiting with status 0`, ERROR/FAIL 0건 | poweroff로 스스로 종료(종료 코드 0) |
| qemu-armv7a / `tash` | 422,984 B | 33 | `Exiting with status 0`, ERROR/FAIL 0건 | 셸이라 계속 실행(timeout으로 종료) |
| mps2-an521 / `ostest` | 214,352 B | 31 | `Exiting with status 0`, ERROR/FAIL 0건 | poweroff 없음(timeout으로 종료) |
| mps2-an521 / `tash` | 268,272 B | 31 | `Exiting with status 0`, ERROR/FAIL 0건 | 셸이라 계속 실행(timeout으로 종료) |

`tash` 구성은 부팅 후 `TASH>>` 프롬프트에 `ostest`를 입력해 실행했습니다. 그래서 콘솔 입력(UART RX)도 함께 확인됩니다.

> 정정: 커밋 메시지(`f4e15eed`, `98ff0124`, `a989f610`)와 진행 중 알림에는 그룹 수를 34/32로 적었습니다. 이 수는 `user_main: argv[0]="ostest"` 줄까지 센 값이라 1씩 많았습니다. 실제 테스트 그룹은 33/31입니다. 통과 여부에는 영향이 없습니다.

실행된 테스트 그룹(qemu-armv7a 기준): argument, getopt, libc, setvbuf, /dev/null, FPU, task_restart, waitpid, wqueue, mutex, timed mutex, cancel, robust, semaphore, timed semaphore, condition variable, pthread_exit, pthread_rwlock, pthread_rwlock_cancel, timed wait, timed message queue, sigprocmask, message queue, signal handler, nested signal handler, POSIX timer, spinlock, wdog, round-robin, barrier, priority inheritance, scheduler lock, vfork.

### 2.2 코드 동일성 (tizenrt-nuttx-similarity core)

싱크 단위마다 `codesim.engine.tokens_identical`로 비교했습니다. rename 규칙은 tinyara=nuttx, TinyAra=NuttX, TINYARA=NUTTX입니다. C 파일은 토큰 단위로, 그 밖의 파일은 경로 rename 후 바이트 단위로 비교했습니다.

| 단위 | 동일 | 수정 |
|---|---|---|
| include | 725 | 3 |
| libs/libc | 942 | 4 |
| libs/libm, libxx, libbuiltin, libdsp, libnx | 181, 35, 10, 19, 164 | 0 |
| kernel (sched) | 313 | 2 |
| mm | 134 | 0 |
| fs | 267 | 3 |
| binfmt, audio, syscall, graphics, video | 32, 5, 13, 86, 10 | 0 |
| drivers (core, mtd, serial+timers, bus, media, usb, power) | 93, 51, 51, 78, 293, 57, 26 | 4, 0, 2, 0, 0, 0, 2 |
| arch | 207 | 2 |
| board | 15 | 1 |
| apps ostest | 51 | 1 |
| **합계** | **3858** | **24** |

토큰 비교는 공백을 보지 않습니다. 그래서 숫자가 지수 부호에서 쪼개진 경우(`1e - 1`)는 이 비교로 잡히지 않습니다. 이 경우만 `ppnum.py`로 따로 검사했고 0건입니다.

### 2.3 포매터 (cformatter)

- 싱크한 .c/.h를 모두 `agent/261022_improve-cformatter-tool` 브랜치의 `tools/cformatter.py`로 포맷했습니다(실제로 바뀐 파일 2964개). 이 브랜치는 별도 worktree로 쓰고 마이그레이션 브랜치에는 머지하지 않았습니다.
- 진행 중 포매터 버그 2개를 찾아 툴 브랜치에서 고쳤습니다. 둘 다 회귀 테스트를 추가했습니다.
  - `3463f629`: 지시문 줄에서 시작한 블록 주석이 다음 줄로 이어지면 `*/`가 `* /`로 바뀌어 주석이 닫히지 않았습니다. 이 수정은 사용자 승인을 받고 푸쉬했습니다.
  - `51e698d9`: 지수 부호를 이항 연산자로 보고 `1e-1`을 `1e - 1`로 바꿔 컴파일이 깨졌습니다("exponent has no digits"). 영향받은 9개 파일을 다시 포맷했습니다(`0f89d2cf`).
- [M07] 경고 274개는 주석 위치 때문에 포매터가 `}`와 `else`를 붙이지 못한 곳입니다. 손으로 고치지 않았습니다. 이유는 4.2절에 있습니다.

### 2.4 빌드 경고

NuttX에서 가져온 코드는 경고 없이 빌드됩니다. 남은 경고는 손대지 않은 TizenRT apps 코드에서만 나옵니다.

- `apps/examples/testcase/tc_main.c`: 사용하지 않는 변수 `pid`
- `apps/system/utils/fscmd.c`: printf 형식 경고. `uint32_t`/`off_t` 크기가 바뀌어 생겼습니다.

## 3. 사용자 결정사항

### 3.1 처음 요구사항 (원칙)

| # | 원칙 |
|---|---|
| 1 | TizenRT 빌드시스템, Kconfig, defconfig 방식을 유지합니다. docker 변경이 필요하면 Dockerfile로 합니다. |
| 2 | NuttX에 있는 것만 덮어씁니다. 관련 없는 TizenRT 코드는 남기고 config로 끕니다. TizenRT에 없는 NuttX 기능은 모두 가져오되 Crypto와 cmake는 제외합니다. TizenRT에 없는 드라이버는 가져오지 않지만, mtd 같은 핵심 드라이버는 예외입니다. |
| 3 | armv7-a, armv8-m 두 arch와 그 QEMU 보드, 그리고 QEMU 구동에 필요한 것만 포팅합니다. |
| 4 | `os/kernel` 이름을 유지합니다. |
| 5 | `include/tinyara` 이름을 유지합니다. |
| 6 | 싱크하는 파일 안의 삼성 코드(task manager, eventloop 등)는 덮어씁니다. |
| 7 | NuttX 코드는 NuttX 로그 매크로를 쓰고, `debug.h`에서 삼성 매크로를 NuttX 매크로로 연결합니다. |
| 8 | net과 pm은 싱크하지 않습니다. |
| 9 | fs/mtd는 NuttX 구조를 따릅니다. |
| 10 | wqueue는 NuttX 구조를 따릅니다. |
| 11 | NuttX 1-depth 폴더 단위로 커밋합니다(libs는 2-depth, drivers는 묶음). 서브에이전트를 활용하고 단계 계획을 잘 세웁니다. |
| 12 | 애매한 것은 물어봅니다. |
| - | 코딩 규칙은 TizenRT `agent/261022_improve-cformatter-tool` 브랜치의 cformatter를 따릅니다. 동일성은 tizenrt-nuttx-similarity core로 검증합니다. |
| - | 검증은 QEMU에서 armv7-a와 armv8-m을 부팅하고, nuttx-apps 12.13.0에서 싱크한 ostest를 돌리는 것입니다. |

### 3.2 진행 중 질의응답으로 정한 사항

| 항목 | 결정 |
|---|---|
| armv8-m 보드 | mps2-an521 |
| cformatter 사용 방식 | 별도 체크아웃(worktree)에서 실행하고 머지하지 않습니다. |
| cformatter 버그(주석) | 툴 브랜치에 수정을 푸쉬합니다. |
| libs 위치 | NuttX 구조로 `os/libs/*`를 새로 만듭니다. |
| rename 범위 | include 경로만 바꿉니다(`<nuttx/..>`→`<tinyara/..>`, Makefile/Kconfig 경로 `sched`→`kernel`, `boards`→`board`). |
| ostest 실행 방식 | config 2개: 진입점으로 실행 후 poweroff(`ostest`), TASH에서 실행(`tash`) |
| 호스트 툴 | TizenRT `os/tools`를 유지하고 최소한만 바꿉니다. |
| 빌드 모드 | FLAT만 |
| 핵심 드라이버 | note와 QEMU 최소 부팅에 필요한 것만 가져옵니다. openamp는 전혀 가져오지 않습니다. |
| net | 관련된 것을 모두 제외합니다(net/, drivers/net·wireless, net 헤더, libc/net·netdb, fs/socket). |
| pm | 제외합니다(TizenRT `os/pm` 유지). |
| wireless/ | 가져오지 않습니다. |
| fs/nfs | 제외합니다(NuttX net 위의 NFSv3 클라이언트라는 설명을 듣고 결정). |
| NuttX 유래 구 파일 | 삭제합니다. |
| 싱크 스크립트 | 커밋하지 않습니다. |
| Docker | `tizenrt/tizenrt` 최신(2.0.1)을 기준으로 필요한 것만 추가합니다. |
| 제외 영역 헤더를 무조건 include하는 NuttX 코드 | 헤더를 가져오지 않고 include를 모두 config로 가드합니다. |

### 3.3 커밋·푸시 규칙

| 항목 | 결정 |
|---|---|
| 작성자 | author/committer `eunwoo-nam <eunwoo.nam@samsung.com>` |
| trailer | `Co-Authored-By: Claude Opus 5.5` (이메일 없음), `Claude-Session` 줄 없음 |
| 푸쉬 | 커밋할 때마다 브랜치에 푸쉬하고, master에는 절대 푸쉬하지 않습니다. |
| 진행 기록 | 단계마다 작업 커밋 → HANDOFF.md 커밋 → 푸쉬 → 브랜치와 커밋을 알림 → 계속 진행 |

## 4. Claude가 스스로 판단한 사항과 근거

사용자 결정이 정하지 않은 세부 사항은 아래처럼 판단했습니다.

### 4.1 구조 매핑

| 판단 | 근거 |
|---|---|
| 싱크 전에 구조 이동 커밋(`git mv`만)을 따로 만들었습니다: `lib/libc`→`os/libs/libc`, `lib/libc/math`→`os/libs/libm/libm`, `binfmt/libelf`→`libs/libc/elf`(`libelf_*`→`elf_*`), `binfmt/libbuiltin`→`libs/libc/builtin`, `os/wqueue`→`kernel/wqueue`+`libs/libc/wqueue`, `fs/driver/{block,mtd}`→`fs/driver`, `drivers/mtd`, 드라이버 루트 파일→NuttX 하위 폴더, `arch/boardctl.c`→`board/boardctl.c` | 각 파일이 NuttX 12.13.0에 있는 위치로 옮겨야 원칙 2("NuttX에 있는 것을 덮어쓰기")를 같은 파일 단위로 적용할 수 있습니다. 이동과 내용 변경을 분리하면 `git log --follow`로 TizenRT 히스토리가 남습니다. |
| 보드 소스는 `os/board/<board>`에, defconfig·Make.defs·링커 스크립트는 `build/configs/<board>`에 두었습니다. | TizenRT 보드 구성 방식입니다(원칙 1). |
| `graphics`, `video`, `libs/{libbuiltin,libdsp,libnx}`를 새로 추가하고 기본은 꺼 두었습니다. | 원칙 2("TizenRT에 없는 NuttX 기능은 모두")에 해당하고 드라이버가 아닙니다. 기본 off라 기존 동작에 영향이 없습니다. |
| arch는 `common`, `armv7-a`, `armv8-m`, `qemu`, `mps`만 가져왔습니다. libc `machine`도 arm 공통, armv7-a, armv8-m만 가져왔습니다. 다른 arch·칩(armv7-m, armv7-r, amebad, imxrt, s5j, stm32 등)은 손대지 않았습니다. | 원칙 3(두 arch만 포팅). |
| "QEMU에 필요한 드라이버"는 `serial`(uart_pl011, serial_cmsdk), `timers`(arch_timer/alarm), `drivers/dummy`로 판단했습니다. | 두 보드의 콘솔 UART와 시스템 타이머에 필요합니다. `drivers/dummy`는 보드에 drivers 폴더가 없을 때 NuttX `Config.mk`와 `drivers/Kconfig`가 가리키는 platform 디렉터리입니다. |
| drivers 커밋은 core, mtd, serial+timers, bus(i2c/spi/i2s/analog/ioexpander), media(input/lcd/video/audio/sensors), usb, power, dummy 8묶음으로 나눴습니다. | 원칙 11의 "drivers는 묶음"을 기능 영역별로 나눈 것입니다. |

### 4.2 싱크·검증 방식

| 판단 | 근거 |
|---|---|
| 싱크는 스크립트(`nxsync.py`)가 단위별로 했습니다: 복사, include 경로 rename, cformatter, codesim 검증. | 모든 단위에 같은 규칙을 기계적으로 적용하고, 결과를 단위마다 "동일 N / 수정 M"으로 검증할 수 있습니다. |
| 서브에이전트는 계획 단계의 읽기 전용 조사 3건(NuttX 12.13.0 구조, TizenRT 빌드시스템, codesim·cformatter 사용법)에만 썼습니다. 싱크와 커밋은 메인 세션에서 직접 했습니다. | 싱크가 스크립트로 결정적이라 나눠서 얻을 이득이 작았습니다. 여러 에이전트가 같은 트리를 동시에 고치면 충돌이 날 수 있고, 커밋 순서와 작성자 규칙도 한곳에서 지키는 편이 안전했습니다. **원칙 11의 "서브에이전트 활용"과 다른 점입니다.** |
| 구 파일 판정 기준: 12.13.0에 짝이 없는 TizenRT 파일 중 (a) NuttX git 히스토리에 그 경로가 있었거나 (b) 알려진 NuttX 이름이면서 NuttX/ASF 라이선스 헤더가 있으면 NuttX 유래로 보고 삭제했습니다. 삼성 헤더 파일은 유지했고, 애매하거나 소스가 아닌 파일은 검토했습니다. | "NuttX 유래 구 파일 삭제" 결정을 객관적으로 적용하려면 기준이 필요했습니다. 유지한 삼성 파일 목록은 각 싱크 커밋 메시지에 적었습니다. |
| cformatter는 .c/.h에만 적용했습니다. .S, Makefile, Kconfig, 링커 스크립트는 포맷하지 않았습니다(경로 rename만 적용). | 포매터 대상이 C 코드입니다. |
| cformatter [M07] 경고 274곳은 손으로 고치지 않았습니다. | 고치려면 토큰 순서를 바꿔야 합니다(주석 이동). 그러면 NuttX와의 토큰 동일성이 깨지고, 다음 싱크 때 비교가 어려워집니다. |
| 두 번째 cformatter 버그(`51e698d9`)는 먼저 묻지 않고 툴 브랜치에 푸쉬한 뒤 알렸습니다. | 첫 번째 버그 때 "툴 브랜치에 수정 푸쉬"를 승인받았고, 같은 종류의 툴 버그이면서 빌드를 막는 문제였습니다. |
| codesim 외에 지수 숫자 분리 검사(`ppnum.py`)를 추가했습니다. | codesim은 공백을 무시하므로 `1e - 1`과 `1e-1`을 같다고 봅니다. 컴파일러에는 다른 코드입니다. |
| `debug.h` 호환부: `dbg`/`lldbg`→`_err`, `wdbg`→`_warn`, `vdbg`/`llvdbg`→`_info`, 서브시스템 매크로는 `<sub>err/warn/info`로 연결했습니다. NuttX에 없는 삼성 서브시스템(binary manager, task manager, eventloop 등)은 자기 `CONFIG_DEBUG_<SUB>_<LEVEL>` 옵션을 유지하고 `_err/_warn/_info`로 연결했습니다. | 원칙 7의 구체화입니다. 서브시스템별로 로그를 켜고 끄던 TizenRT 동작을 유지합니다. |

### 4.3 빌드시스템

| 판단 | 근거 |
|---|---|
| `os/tools/Config.mk`는 NuttX 12.13.0 것을 기반으로 하고 TizenRT 부분만 바꿨습니다: `BOARD_DIR`, platform 디렉터리, GCC 접두사, ARCHIVE 락, MOVEOBJ. | 싱크한 NuttX Makefile·Make.defs가 NuttX의 매크로 집합(CATFILE, DOWNLOAD, ARCHIVE_ADD, .ddc/.dds 규칙, INCDIR_PREFIX 등)을 씁니다. 일부만 옮기면 깨지기 쉽습니다. "os/tools 최소 변경" 결정에 비해 큰 변경이라 근거를 남깁니다. |
| `mkdeps.c`를 NuttX 버전으로 교체했습니다. | 기존 버전이 `clock/clock.c`처럼 하위 폴더 경로가 붙은 소스를 처리하지 못해 빌드가 멈췄습니다. |
| `mkconfig.c`는 NuttX sanity 검사에 TizenRT 보드 매크로 블록을 더했습니다. `cfgdefine.c`는 `INIT_ENTRYPOINT` 등의 dequote 목록을 추가했습니다. | NuttX 코드가 `CONFIG_RAM_END` 등을 기대하고, `int CONFIG_INIT_ENTRYPOINT(...)` 같은 식별자 사용이 컴파일되게 하려면 필요합니다. 기존 TizenRT 보드 매크로도 유지됩니다. |
| `tools/{D,Rust,Swift,Zig}.defs`를 NuttX에서 복사했습니다. | arch `Toolchain.defs`가 무조건 include합니다. |
| `libs/libc/{bin,kbin,ubin}`, `mm/{bin,kbin,ubin}`을 삭제했습니다. | NuttX Makefile이 `context`에서 만들고 `distclean`에서 지웁니다. 추적 파일로 두면 distclean 후 트리가 바뀝니다. |
| Kconfig: NuttX 최상위·arch·board Kconfig를 기준으로 하고 TizenRT에만 있는 심볼을 병합했습니다. `DEBUG`/`DEBUG_VERBOSE`/`LIB_BOARDCTL`은 별칭으로 남기고, `APP_BINARY_SEPARATION`은 `BUILD_PROTECTED` 옵션으로 옮겼습니다. | 싱크한 NuttX Kconfig가 NuttX 상위 옵션에 의존합니다. 별칭을 두면 기존 TizenRT 코드와 defconfig가 그대로 동작합니다. |
| `INIT_ENTRYPOINT`는 NuttX(문자열)를 따르고 `apps/apps_entry/Kconfig`는 `if INIT_ENTRY`로만 바꿨습니다. | 진입점을 실제로 시작하는 `nx_bringup.c`가 NuttX 코드입니다. apps 쪽은 최소 수정으로 맞췄습니다. |
| include 가드의 조건은 헤더별로 정했습니다: `power/pm.h`→`CONFIG_PM`, `syslog_rpmsg.h`→`CONFIG_SYSLOG_RPMSG(_SERVER)`, `net/tun.h`→`CONFIG_NET_TUN` 등. | 가드는 사용자 결정이고, 조건은 그 헤더를 쓰는 기능의 NuttX 옵션으로 정했습니다. 그 기능이 켜질 때만 헤더가 필요합니다. |
| libc의 TEA 암호 함수(`lib_tea_*`)를 빌드에서 뺐습니다. | `crypto/tea.h`가 싱크 대상이 아니고(crypto 제외), 이 함수를 쓰는 코드가 없습니다. |
| `os/include/queue.h` 호환 헤더(`<tinyara/queue.h>`로 연결)를 추가했습니다. | NuttX는 `<queue.h>`를 없앴지만 TizenRT 파일 85개가 이 헤더를 include합니다. QEMU 빌드에 들어가는 TizenRT 헤더도 그중 하나입니다. 85개 파일을 고치는 대신 1개 헤더로 해결했습니다. |
| 나머지 옮겨진 헤더(`tinyara/i2c.h`, `tinyara/fs/mtd.h`, `crc32.h` 등)는 호환 헤더를 만들지 않았습니다. 다른 보드용 코드에서만 쓰여 7절에 기록만 했습니다. | 원칙 3(두 보드 범위)에 따라 QEMU 빌드에 들어가지 않는 코드는 고치지 않았습니다. |
| TizenRT `/dev/pm` 드라이버(`drivers/pm`)는 `CONFIG_PM`일 때만 빌드되게 바꿨습니다. | 무조건 빌드되던 TizenRT 코드가 새 헤더와 맞지 않았습니다. 원칙 2("TizenRT 코드는 남기고 config로 끔")를 적용했습니다. |
| Docker: QEMU 8.2.2를 Ubuntu 24.04 소스 패키지에서 정적으로 빌드하는 multi-stage Dockerfile을 만들었습니다. `arm-softmmu`만 빌드하고 문서와 펌웨어 blob은 뺐습니다. 툴체인은 베이스 이미지의 gcc 10.3.1을 유지했습니다. | 베이스 이미지(Ubuntu 16.04.3)에는 두 머신을 지원하는 QEMU가 없습니다. 정적 빌드여야 오래된 베이스에서도 돌아갑니다. gcc 10.3.1로 빌드와 테스트가 통과해서 툴체인 교체(Arm GNU 13.2)는 필요 없었습니다. |

### 4.4 apps와 테스트 구성

| 판단 | 근거 |
|---|---|
| ostest는 nuttx-apps 12.13.0 파일과 Kconfig를 그대로 쓰고, Make.defs만 TizenRT 형식(CSRCS/VPATH)으로 바꿔 nuttx-apps Makefile 조건을 옮겼습니다. 기존 TizenRT의 테스트별 `TESTING_OSTEST_*` 옵션은 없어졌습니다. | 싱크한 `ostest_main.c`가 OS 옵션(`CONFIG_DISABLE_PTHREAD` 등)으로 테스트를 고르므로 빌드 조건도 같아야 합니다. TizenRT apps 빌드 방식은 유지했습니다(원칙 1). |
| `ostest_main.c`의 진입점 이름만 `ostest_main`으로 바꿨습니다(`CONFIG_BUILD_KERNEL`이면 `main`). | TASH 등록과 `CONFIG_INIT_ENTRYPOINT="ostest_main"`이 이 이름을 씁니다. NuttX와의 차이는 이 한 곳뿐입니다. |
| `fork.c`, `multiuser.c`(TizenRT 추가, 12.13.0에 없음)는 파일만 두고 빌드하지 않습니다. | 삼성 파일은 유지합니다(원칙 2). NuttX `ostest_main.c`가 호출하지 않아서 빌드해도 쓰이지 않습니다. |
| apps 최소 수정: `CONFIG_LIB_HOMEDIR`→`CONFIG_LIBC_HOMEDIR`, `system_file.c`에 `<tinyara/crc32.h>` 추가, TASH·`FS_CMDS`의 `NFILE_DESCRIPTORS` 의존 제거, `fscmd.c`의 `CONFIG_NFILE_DESCRIPTORS > 0` 가드 제거와 로컬 콜백 타입 추가. | QEMU 구성에서 항상 빌드되는 TizenRT 코드가 NuttX 12.13.0의 옵션·헤더 변경(옵션 이름 변경, 헤더 이동, fd 동적 할당으로 옵션 삭제, `foreach_mountpoint_t` 내부화) 때문에 깨지거나 통째로 컴파일에서 빠졌습니다. |
| defconfig의 OS 옵션은 NuttX 보드의 nsh defconfig 값을 따랐습니다. ostest 검증에 필요 없는 TizenRT apps와 기능은 껐습니다: GPIO, IOTBUS, SECURITY_API, SECURITY_LEVEL, OS API test, kernel TC, SYSTEM_INFORMATION, SYSTEM_CMDS(ps, free, date, kill 등). DEBUG_WARN/INFO도 껐습니다. 형식은 `kconfig-conf --olddefconfig`로 만든 TizenRT식 full .config입니다. | NuttX에서 검증된 보드 설정을 기준으로 해야 문제가 NuttX 코드에 있는지 TizenRT 통합에 있는지 나눠 볼 수 있습니다. 끈 apps는 QEMU 보드에 없는 하드웨어를 쓰거나 이번 범위 밖입니다. 원칙 2("config로 끔")를 적용했고, 켜서 동작을 확인하는 일은 남은 과제로 둡니다. |
| 두 보드 모두 `CONFIG_SCHED_WAITPID=y`를 추가했습니다. | 이 옵션이 없으면 `TESTING_OSTEST_WAITRESULT`가 꺼집니다. 그러면 ostest가 결과를 기다리지 않고 바로 `status 0`을 출력해 판정이 무의미해집니다. qemu-armv7a의 poweroff 옵션도 이 옵션에 의존합니다. |
| `tash` 구성의 진입점은 `preapp_start`(apps/system/init)입니다. | NuttX `nx_bringup`은 `CONFIG_INIT_ENTRYPOINT` 하나만 시작합니다. 예전 TizenRT `os_bringup`이 부르던 `preapp_start`가 TASH를 시작하고 builtin 명령을 등록합니다. |
| `tash` 구성에서 `CONFIG_SECURITY_LEVEL_CMDS`를 껐습니다. | TASH를 켜면 기본값(y)으로 켜지는 삼성 기능인데 NuttX에 없는 `prctl(PR_SET_SECURITY_LEVEL)`을 씁니다. 원칙 2를 적용했습니다. |
| mps2-an521 `ostest`는 poweroff 없이 구성했습니다. | NuttX 12.13.0 mps2-an521 보드에 `board_power_off()`가 없습니다. NuttX에 없는 보드 코드를 새로 쓰지 않았습니다. **"진입점 실행 후 poweroff" 결정과 다른 점입니다.** 결과 문구는 그대로 출력됩니다. |
| mps2-an521 CMSDK UART0 IRQ는 NuttX 값(TX 48, RX 49, OV 63)을 그대로 썼습니다. | QEMU 8.2.2에서 출력(ostest 로그)과 입력(TASH 명령)이 모두 동작하는 것을 확인했습니다. |
| mps2-an521에서 FPU와 우선순위 상속을 켜지 않았습니다. | NuttX 보드 설정을 따랐습니다. 그래서 테스트 그룹이 qemu-armv7a보다 2개 적습니다. |

### 4.5 진행 과정

| 판단 | 근거 |
|---|---|
| 첫 지수 숫자 수정 커밋(`dea1d23a`)에 관련 없는 bin 디렉터리 삭제 12개가 섞여 들어갔습니다. 푸쉬 직후, 알리기 전에 `--force-with-lease`로 다시 만들었습니다(`0f89d2cf`). | 커밋 내용과 메시지가 맞지 않았습니다. 이 세션에서 만든 브랜치이고 아무도 그 커밋을 가져가지 않은 시점이었습니다. master는 건드리지 않았습니다. |
| 빌드할 때 TizenRT apps preconfig가 `apps/examples/SoundPlayer/Kconfig`와 `Kconfig_ENTRY`를 다시 생성합니다. 매번 되돌리고 커밋하지 않았습니다. | TizenRT `apps/tools/mkkconfig.sh`의 원래 동작이고 이번 작업과 관계가 없습니다. |
| Docker Hub pull 제한은 `mirror.gcr.io`에서 받아 태그를 바꾸는 방식으로 우회했습니다. | 작업 환경 문제라 레포에는 반영하지 않았습니다. HANDOFF.md에 재현 방법만 적었습니다. |

## 5. 사용자 결정과 다르게 진행된 점

| 결정 | 실제 | 이유 |
|---|---|---|
| 원칙 11: 서브에이전트 활용 | 계획 단계 조사에만 사용했고 싱크는 스크립트로 메인 세션에서 했습니다. | 4.2절 |
| ostest 구성: 진입점 실행 후 poweroff | mps2-an521은 poweroff 없음 | NuttX 보드에 `board_power_off()` 없음(4.4절) |
| 호스트 툴 최소 변경 | `Config.mk`, `mkdeps.c`는 NuttX 버전 기반으로 교체 | NuttX Makefile이 의존하는 매크로와 도구(4.3절) |
| 커밋마다 푸쉬 | 한 번 force push로 커밋을 다시 만듦 | 4.5절 |

## 6. 결과물

### 6.1 NuttX와 의도적으로 다른 파일 24개

| 파일 | 변경 |
|---|---|
| `os/include/debug.h` | 끝에 TizenRT 매크로 블록 |
| `os/include/.gitignore` | 생성되는 `apps` 링크 무시 |
| `os/include/tinyara/power/regulator.h`, `os/kernel/sched/sched_cpuload_{oneshot,period}.c`, `os/drivers/serial/serial.c`, `os/drivers/timers/watchdog.c` | `power/pm.h` include 가드 |
| `os/drivers/drivers_initialize.c` | crypto, rpmsg, net, pm, usrsock include 가드 |
| `os/drivers/syslog/syslog_initialize.c`, `os/arch/arm/src/qemu/qemu_boot.c` | `syslog_rpmsg.h` include 가드 |
| `os/fs/fs_initialize.c` | `rpmsgfs.h` include 가드 |
| `os/board/qemu-armv7a/src/qemu_bringup.c` | `rpmsg_port.h` include 가드 |
| `os/drivers/{Kconfig,Makefile}`, `os/drivers/power/{Kconfig,Make.defs}`, `os/fs/{Kconfig,Makefile}`, `os/libs/libc/Kconfig`, `os/libs/libc/machine/Kconfig`, `os/libs/libc/machine/arm/Kconfig` | 싱크하지 않은 디렉터리 제외, TizenRT 전용 드라이버 추가 |
| `os/libs/libc/misc/Make.defs` | TEA 제외 |
| `os/arch/arm/src/Makefile` | `build/output/bin/tinyara`로 링크 |
| `apps/examples/testcase/ostest/ostest_main.c` | 진입점 `ostest_main` |

### 6.2 커밋 목록 (작업 커밋, 오래된 순)

| 단계 | 커밋 |
|---|---|
| 준비 | `a284ab61` tools/docker (QEMU 이미지) |
| include | `e967f697` os/include 싱크, `fac1eff5` debug.h 호환 |
| libs | `05aac056` 구조 이동, `321873aa` libc, `d3dea6bb` libm, `e968b113` libxx, `c1009927` libbuiltin, `1752c7e9` libdsp, `ac0ffe94` libnx |
| kernel, mm | `df041e48` kernel(sched), `8cd029e9` mm |
| fs | `7bd79912` 구조 이동, `e6069c5a` fs |
| drivers | `1f7c44eb` 구조 이동, `398c1cea` core, `6431efc9` mtd, `c6e4ff5e` serial+timers, `d76e990d` bus, `2f730c83` media, `cd97210f` usb, `16d7b9cf` power, `7df82b2e` dummy |
| 기타 os | `5886fcb6` binfmt, `a26e5be4` audio, `6bbb2883` syscall, `39bce27f` graphics, `e0c988ed` video |
| arch, board | `706971a3` arch/arm, `c80d1840` boardctl 이동, `8e5f6d3e` 보드 2개 |
| 통합 | `422cae36` Kconfig, `0f89d2cf` 지수 숫자 재포맷, `1612c7b6` 빌드시스템 |
| apps | `3af7c335` ostest 싱크, `0d962fae` apps/system, `52b06bd1` TASH·fscmd |
| 보드 config | `f4e15eed` qemu-armv7a ostest, `98ff0124` qemu-armv7a tash, `a989f610` mps2-an521 ostest·tash |
| 문서 | `1268b92d` 마이그레이션 문서, 이 리포트 |

각 작업 커밋 다음에 HANDOFF.md 갱신 커밋이 있습니다. 싱크 커밋 메시지에는 원본 커밋, 제외 항목, 삭제·유지 파일, codesim 결과를 적었습니다.

## 7. 남은 과제

자세한 내용은 [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md)의 Known gaps에 있습니다.

- qemu-armv7a와 mps2-an521만 포팅했습니다. 다른 TizenRT 보드와 칩은 예전 arch 코드와 예전 드라이버 인터페이스를 그대로 써서 지금은 빌드되지 않습니다.
- protected/kernel 빌드(`ProtectedLibs.mk`, `KernelLibs.mk`, userspace 링커 스크립트)는 갱신하지 않았습니다.
- 덮어쓴 삼성 훅에 의존하던 기능은 새 커널에 다시 포팅하기 전까지 꺼져 있습니다: binary manager, task manager, heapinfo, preference, messaging, 삼성 syscall.
- 다른 보드용 TizenRT 코드가 NuttX가 옮기거나 없앤 헤더를 아직 include합니다: `tinyara/i2c.h`, `tinyara/fs/mtd.h`, `crc*.h`, `tinyara/time.h`, `tinyara/math.h`, `tinyara/fs/dirent.h`, `tinyara/configdata.h`, `tinyara/float.h`.
- `CONFIG_NFILE_DESCRIPTORS`/`CONFIG_NFILE_STREAMS`를 쓰는 le_tc, ftpc, libcoap, lwm2m, vi가 남아 있습니다.
- `CONFIG_PM` 보드에는 NuttX `power/pm.h`와 `drivers/power/pm`이 필요한데 싱크하지 않았습니다.
- mps2-an521은 poweroff가 없어서 자동 테스트에 timeout이 필요합니다.
- QEMU 구성에서 끈 TizenRT apps(SYSTEM_CMDS의 ps/free/date/kill, SYSTEM_INFORMATION, 보안 API 등)는 NuttX 12.13.0 위에서 켜서 확인하지 않았습니다.
- cformatter [M07] 경고 274곳은 수동 정리 대상입니다.

## 8. 재현 방법

```bash
# 이미지 (tools/docker)
docker build -t tizenrt/tizenrt:2.0.1-qemu tools/docker

# 빌드
cd os
./tools/configure.sh qemu-armv7a/ostest     # qemu-armv7a/tash, mps2-an521/ostest, mps2-an521/tash
./dbuild.sh build

# 실행 (tash는 TASH>> 에서 ostest 입력, 종료는 Ctrl-a x)
cd ..
docker run --rm -it -v $PWD:/root/tizenrt tizenrt/tizenrt:2.0.1-qemu \
	/root/tizenrt/build/configs/qemu-armv7a/qemu_run.sh
```

다음 싱크에 쓸 도구(nxsync.py, adapt.py, kmerge.py, ppnum.py 등)는 커밋하지 않았습니다(사용자 결정). 동작과 재구성 방법은 [HANDOFF.md](../HANDOFF.md)의 Tools와 Environment에 있습니다.
