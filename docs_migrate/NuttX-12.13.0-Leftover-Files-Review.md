# NuttX 12.13.0 sync 후 잔존 파일 리뷰

| 항목 | 내용 |
|---|---|
| 작성일 | 2026-11-02 |
| 갱신 | 2026-11-02 후속 결정 반영 (8장) |
| 대상 브랜치 | `DEV-12.13.0-LABEL` (base `379c33e74`, upstream/master 파생) |
| 리뷰 기준 트리 | `DEV-12.13.0-LABEL___NO_CHANGED` `485989868` — 마이그레이션 커밋이 추가/수정한 파일을 모두 뺀 트리(28842 files, [MOVE]로 위치만 바뀐 파일은 남김) |
| 범위 | `os/libs` (40), `os/include` (154). 다른 영역은 이후 섹션으로 추가 |
| 함께 볼 문서 | [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md) (후속 조치), [NuttX-12.13.0-Libxx-Newlib-Impure-Removal.md](NuttX-12.13.0-Libxx-Newlib-Impure-Removal.md), [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md) |

## 1. 목적

NuttX 12.13.0 sync는 NuttX 파일을 가져오고 TizenRT 빌드에 등록했지만, sync가 건드리지 않은 TizenRT 파일
중에는 NuttX 파일로 대체되어 지워졌어야 할 파일이 남아 있습니다. 예를 들어 `os/libs/libc/misc/lib_impure.c`가
NuttX에서 들어오면서 `os/libs/libxx/libxx_newlib_impure.c`는 쓸모가 없어졌지만 지워지지 않았습니다.

이 문서는 NO_CHANGED 트리에 남은 파일을 하나씩 확인해 삭제할 파일, 후속 조치(TD)가 필요한 파일,
그대로 둘 파일로 나눈 결과입니다.

## 2. 판정 기준

| 판정 | 기준 | 조치 |
|---|---|---|
| **삭제** | NuttX 12.13.0 파일로 대체되었거나 유일한 사용처가 sync로 교체되어, 빌드·소스 어디에서도 참조되지 않음 | 이번에 삭제 |
| **별도 판정 필요** | 삭제가 유력하지만 기능 폐기나 테스트(le_tc 등) 수정이 따르는 항목 | 삭제하지 않음. TD에 기록하고 따로 결정 |
| 유지 (TD-xx) | 파일은 필요하지만 sync로 Kconfig/Make.defs/헤더/구현 연결이 끊겼거나 API가 어긋남 | 파일 유지. TD 문서의 해당 항목에서 조치 |
| 유지 | 의도적 미sync 영역(net, bluetooth, wireless, pm, power, usb, lcd, video, input, sensors, audio), 사용 중인 Samsung 기능, 본문이 NuttX와 같은 파일 | 조치 없음 |

미sync 영역은 마이그레이션 결정([NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md), [HANDOFF.md](HANDOFF.md)의 Decisions)을 따릅니다.
"Samsung 파일은 유지하고 config로 끈다", "미sync 영역의 헤더/파일은 유지한다"는 결정이므로, Samsung 기능은
사용처가 없어도 바로 지우지 않고 TD나 별도 판정으로 넘겼습니다.

## 3. 확인 방법

| 확인 | 방법 |
|---|---|
| 남은 파일 목록 | `git ls-tree -r --name-only DEV-12.13.0-LABEL___NO_CHANGED -- os/libs os/include` |
| NuttX 대응 파일 | NuttX `nuttx-12.13.0` 태그 트리에서 같은 경로(`os/X` → `X`, `tinyara/` → `nuttx/`)와 같은 이름의 다른 경로 검색, NuttX 전체 이력(`git log --all -- include`) |
| 출처 | 파일 라이선스 헤더(Samsung / Gregory Nutt·ASF = NuttX 유래 / musl / NetBSD 등) |
| 빌드 연결 | 현재 HEAD와 base(`379c33e74`)의 Make.defs·Makefile·Kconfig에서 파일명·config 검색 |
| 참조 수 | `git grep -l '#include [<"]경로[>"]'` 결과 파일 수 (전체 / `os` 아래). 헤더는 sync 직후 HEAD(`cf549f849`) 기준 |

## 4. 요약

리뷰 당시 판정:

| 영역 | 파일 | 삭제 | 별도 판정 필요 | 유지 (TD 연계) | 유지 |
|---|---|---|---|---|---|
| os/libs | 40 | 4 | 8 | 7 | 21 |
| os/include | 154 | 1 | 1 | 10 | 142 |
| 합계 | 194 | 5 | 9 | 17 | 163 |

후속 결정(8장) 반영 후:

| 영역 | 파일 | 삭제 | 이동 | sync됨 | 별도 판정 필요 | 유지 (TD 연계) | 유지 |
|---|---|---|---|---|---|---|---|
| os/libs | 40 | 10 | 0 | 4 | 2 | 3 | 21 |
| os/include | 154 | 2 | 1 | 15 | 1 | 10 | 125 |
| 합계 | 194 | 12 | 1 | 19 | 3 | 13 | 146 |

삭제한 파일과 커밋:

| 커밋 | 삭제 파일 |
|---|---|
| `os/libs/libxx: Remove the files left over after the NuttX 12.13.0 sync [DEL_TR]` | `os/libs/libxx/libxx_newlib_impure.c`, `os/libs/libxx/libxx_cxa_guard.hxx` |
| `os/libs/libc: Remove the files left over after the NuttX 12.13.0 sync [DEL_TR]` | `os/libs/libc/machine/arm/setjmp.S`, `os/libs/libc/stdlib/__randname.c` |
| `os/include: Remove the header left over after the NuttX 12.13.0 sync [DEL_TR]` | `os/include/tinyara/endian.h` |
| `os, apps: Remove the system debug (sysdbg) feature [DEL_TR]` (`0818e112b`, 후속 결정) | `os/include/tinyara/debug/sysdbg.h` (기능 전체 삭제) |
| `os/libs/libm: Remove the Bessel functions left over after the NuttX 12.13.0 sync [DEL_TR]` (`119a92815`, 후속 결정) | `os/libs/libm/libm/lib_{j0,j0f,j1,j1f,jn,jnf}.c` |

후속 결정으로 이동·sync된 파일:

| 커밋 | 파일 |
|---|---|
| `os/drivers/audio: Move cJSON.h from os/include into the audio driver [MODI_TR]` (`e710928f0`) | `os/include/cJSON.h` → `os/drivers/audio/cJSON.h` |
| `os/libs/libc/machine/arm: Sync armv7-m and armv7-r with NuttX 12.13.0 [SYNC]` (`30917f838`, ARMV7 cherry-pick) | `os/libs/libc/machine/arm/armv7-{m,r}/{Make.defs,arch_elf.c}` |
| `os/include: Sync the USB headers with NuttX 12.13.0 [SYNC]` (`1c37bfac3`) | `os/include/tinyara/usb/*.h` 15개 |

`DEV-12.13.0-LABEL___NO_CHANGED`에도 같은 5개 파일을 지우는 커밋을 485989868 위에 추가했습니다.

## 5. os/libs (40)

NuttX 대응은 NuttX 12.13.0의 `libs/` 기준입니다.

| 파일 (os/libs/) | 출처 | NuttX 12.13.0 대응 | 빌드 연결 / 참조 | 판정 | 근거 |
|---|---|---|---|---|---|
| `libxx/libxx_newlib_impure.c` | Samsung (2018, a41a69b9ca) | `libc/misc/lib_impure.c` (기능 대체) | 없음 (구 `CONFIG_LIBCXX_HAVE_LIBSUPCXX` Makefile 블록 제거됨) | **삭제** | NuttX lib_impure.c가 진짜 `_impure_ptr`를 항상 제공. 남기면 중복 심볼 위험. [상세 분석](NuttX-12.13.0-Libxx-Newlib-Impure-Removal.md) |
| `libxx/libxx_cxa_guard.hxx` | Samsung (2018) | `libxx/libminiabi/libxx_cxa_guard.cxx`가 `__guard` 직접 정의 | include 0 | **삭제** | base에서 include하던 구 `libxx_cxa_guard.cxx`, `libxx_cxa_atexit.cxx`가 NuttX libminiabi 소스로 교체됨 |
| `libc/machine/arm/setjmp.S` | NetBSD 유래 (TizenRT 첫 커밋) | `libc/machine/arm/arch_setjmp.S` | 없음 | **삭제** | base `lib/libc/machine/arm/Make.defs`가 무조건 `ASRCS += setjmp.S`. sync된 Make.defs는 `CONFIG_ARCH_SETJMP_H`일 때 arch_setjmp.S를 빌드. 빌드 조건 변화는 TD-LIBS-07 |
| `libc/stdlib/__randname.c` | musl 유래 (Samsung 2018) | 없음 (NuttX `lib_mktemp.c`가 이름을 직접 생성) | 호출 0 | **삭제** | 유일한 호출처인 구 `lib_mktemp.c`, `lib_mkstemp.c`가 NuttX 버전으로 교체, 선언하던 `lib/libc/lib_internal.h`도 없음 |
| `libc/misc/lib_crc16_ccitt.c` | Samsung (2023) | 같은 계열 함수: `crc16xmodem`, `crc16ccitt`, `crc16h1021` (`tinyara/crc16.h`) | 미빌드. le_tc `tc_libc_misc.c`가 `crc16_ccitt_*` 42곳 사용 | **별도 판정 필요** (TD-LIBS-03) | 선언하던 `os/include/crc16.h`가 sync로 삭제, misc/Make.defs 미등록 |
| `libm/libm/lib_j0.c` | musl/FreeBSD 유래 (Samsung 2017) | 없음 (NuttX libm/libm에 Bessel 없음, newlib libm에만 있음) | 미빌드. le_tc `tc_libc_math.c` 사용 | **삭제** (후속 결정, `119a92815`) | 선언하던 구 `tinyara/math.h`가 sync로 삭제, libm Make.defs 미등록 |
| `libm/libm/lib_j0f.c` | 〃 | 〃 | 〃 | **삭제** (후속 결정, `119a92815`) | 〃 |
| `libm/libm/lib_j1.c` | 〃 | 〃 | 〃 | **삭제** (후속 결정, `119a92815`) | 〃 |
| `libm/libm/lib_j1f.c` | 〃 | 〃 | 〃 | **삭제** (후속 결정, `119a92815`) | 〃 |
| `libm/libm/lib_jn.c` | 〃 | 〃 | 〃 | **삭제** (후속 결정, `119a92815`) | 〃 |
| `libm/libm/lib_jnf.c` | 〃 | 〃 | 〃 | **삭제** (후속 결정, `119a92815`) | 〃 |
| `libc/zoneinfo/mk_opt_tzrule.py` | Samsung (2020) | 없음 | `zoneinfo/Makefile`이 `CONFIG_LIBC_OPTIMIZE_ZONEINFO_RULES`일 때 호출 | **별도 판정 필요** (TD-LIBS-05) | 그 Kconfig 심볼이 sync로 없어져 켤 수 없음 |
| `libc/zoneinfo/Makefile` | Samsung (2020) | `libc/zoneinfo/Makefile` (동일 경로, 미sync) | libc Makefile이 `CONFIG_LIBC_ZONEINFO_ROMFS`일 때 호출 | 유지 (TD-LIBS-05) | 옛 경로 `$(TOPDIR)/../lib/libc/zoneinfo`, `zoneinfo.img`를 만드는데 sync된 `tzromfs.c`는 `romfs_zoneinfo.h`를 include → 켜면 빌드 불가 |
| `libc/elf/libelf_cache.c` | Samsung (2019) | 없음 | 미빌드 (base `os/binfmt/libelf/Make.defs`의 `CONFIG_ELF_CACHE_READ` 블록 소실) | 유지 (TD-LIBS-01) | Samsung ELF cache. Kconfig와 elf init/read/uninit 훅이 sync로 사라짐. rtl8721csm, imxrt1050 loadable defconfig는 `=y` |
| `libc/misc/lib_hashmap.c` | Samsung (2017) | 없음 | 미빌드 (`CONFIG_LIB_HASHMAP` Kconfig와 Make.defs 블록 소실) | 유지 (TD-LIBS-02) | `os/Kconfig`(SUPPORT_COMMON_BINARY), st_things가 여전히 `select LIB_HASHMAP`. 없어진 `lib_internal.h`를 include |
| `libc/machine/arm/armv7-m/Make.defs` | NuttX 유래 | `libc/machine/arm/armv7-m/Make.defs` (동일 경로, 미sync) | `machine/arm/Make.defs`가 `CONFIG_ARCH_ARMV7M`일 때 include | sync됨 (`30917f838`) | machine/arm은 armv7-a, armv8-m만 sync. NuttX판은 `arch_mem*.S` 등도 빌드 |
| `libc/machine/arm/armv7-m/arch_elf.c` | NuttX 유래 | 동일 경로 (미sync) | 위 Make.defs가 `CONFIG_LIBC_ARCH_ELF`일 때 빌드 | sync됨 (`30917f838`) | `up_relocate`/`up_relocateadd` 시그니처가 sync된 libc/elf(armv7-a판)와 다름 |
| `libc/machine/arm/armv7-r/Make.defs` | NuttX 유래 | 동일 경로 (미sync) | `CONFIG_ARCH_ARMV7R`일 때 include | sync됨 (`30917f838`) | armv7-m과 같음 |
| `libc/machine/arm/armv7-r/arch_elf.c` | NuttX 유래 | 동일 경로 (미sync) | 〃 | sync됨 (`30917f838`) | armv7-m과 같음 |
| `libc/aio/Make.defs` | NuttX 유래 | 동일 경로 | libc Makefile include | 유지 | 본문이 NuttX와 같고 라이선스 헤더만 다름 |
| `libc/inttypes/Make.defs` | NuttX 유래 | 동일 경로 | libc Makefile include | 유지 | 〃 |
| `libc/libgen/Make.defs` | NuttX 유래 | 동일 경로 | libc Makefile include | 유지 | 〃 |
| `libc/net/Make.defs` | NuttX 유래 | 동일 경로 (미sync) | libc Makefile include | 유지 | net 미sync 결정 영역, [MOVE]만 됨 |
| `libc/net/lib_addrconfig.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_addroute.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_delroute.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_etherntoa.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_htonl.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_htons.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_inetaddr.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_inetaton.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_inetntoa.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_inetntop.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/net/lib_inetpton.c` | NuttX 유래 | 동일 경로 (미sync) | net/Make.defs | 유지 | 〃 |
| `libc/netdb/Make.defs` | Samsung (2019) | 동일 경로 (미sync) | libc Makefile include | 유지 | 〃 |
| `libc/netdb/lib_freeaddrinfo.c` | Samsung (2019) | 동일 경로 (미sync) | netdb/Make.defs | 유지 | 〃 |
| `libc/netdb/lib_getaddrinfo.c` | Samsung (2019) | 동일 경로 (미sync) | netdb/Make.defs | 유지 | 〃 |
| `libc/netdb/lib_gethostbyname.c` | Samsung (2019) | 동일 경로 (미sync) | netdb/Make.defs | 유지 | 〃 |
| `libc/netdb/lib_getifaddr.c` | Samsung (2020) | 없음 | netdb/Make.defs | 유지 | 〃 |
| `libc/netdb/lib_getnameinfo.c` | Samsung (2019) | 동일 경로 (미sync) | netdb/Make.defs | 유지 | 〃 |

## 6. os/include (154)

NuttX 대응은 NuttX 12.13.0의 `include/` 기준이고 `tinyara/`는 `nuttx/`로 바꿔 찾았습니다.
"동일 경로"는 NuttX에 같은 파일이 있지만 해당 영역을 sync하지 않아 TizenRT 파일이 그대로 남은 경우입니다.

### 네트워크 (미sync 결정 영역) (48)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `arpa/inet.h` | NuttX 유래 | arpa/inet.h (동일 경로) | 244/42 | 유지 | net 미sync 결정 영역 |
| `ifaddrs.h` | Samsung | ifaddrs.h (동일 경로) | 28/11 | 유지 | net 미sync 결정 영역 |
| `net/ethernet.h` | NuttX 유래 | net/ethernet.h (동일 경로) | 13/9 | 유지 | net 미sync 결정 영역 |
| `net/if.h` | NuttX 유래 | net/if.h (동일 경로) | 175/64 | 유지 | net 미sync 결정 영역 |
| `net/route.h` | NuttX 유래 | net/route.h (동일 경로) | 7/5 | 유지 | net 미sync 결정 영역 |
| `netdb.h` | NuttX 유래 | netdb.h (동일 경로) | 149/15 | 유지 | net 미sync 결정 영역 |
| `netinet/arp.h` | NuttX 유래 | netinet/arp.h (동일 경로) | 1/1 | 유지 | net 미sync 결정 영역 |
| `netinet/ether.h` | NuttX 유래 | netinet/ether.h (동일 경로) | 5/2 | 유지 | net 미sync 결정 영역 |
| `netinet/in.h` | NuttX 유래 | netinet/in.h (동일 경로) | 249/31 | 유지 | net 미sync 결정 영역 |
| `netinet/ip.h` | NuttX 유래 | netinet/ip.h (동일 경로) | 5/2 | 유지 | net 미sync 결정 영역 |
| `netinet/ip6.h` | NuttX 유래 | netinet/ip6.h (동일 경로) | 2/1 | 유지 | net 미sync 결정 영역 |
| `netinet/tcp.h` | Samsung | netinet/tcp.h (동일 경로) | 7/0 | 유지 | net 미sync 결정 영역 |
| `netpacket/packet.h` | NuttX 유래 | netpacket/packet.h (동일 경로) | 8/0 | 유지 | net 미sync 결정 영역 |
| `sys/sock_internal.h` | NuttX 유래 | - | 2/1 | 유지 | net 미sync 결정 영역 |
| `sys/socket.h` | NuttX 유래 | sys/socket.h (동일 경로) | 244/43 | 유지 | net 미sync 결정 영역 |
| `sys/sockio.h` | NuttX 유래 | sys/sockio.h (동일 경로) | 5/2 | 유지 | net 미sync 결정 영역 |
| `sys/un.h` | NuttX 유래 | sys/un.h (동일 경로) | 19/6 | 유지 | net 미sync 결정 영역 |
| `tinyara/lwnl/lwnl.h` | Samsung | - | 48/35 | 유지 | net 미sync 결정 영역 |
| `tinyara/lwnl/rtk_drv.h` | Samsung | - | 0/0 | 유지 | wireless(lwnl) 미sync 영역. base부터 참조 0 |
| `tinyara/lwnl/slsi_drv.h` | Samsung | - | 1/1 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/cs89x0.h` | NuttX 유래 | - | 0/0 | 유지 (TD-INC-04) | net 미sync 결정 영역. base부터 참조 0 |
| `tinyara/net/dns.h` | NuttX 유래 | nuttx/net/dns.h (동일 경로) | 2/0 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/enc28j60.h` | NuttX 유래 | nuttx/net/enc28j60.h (동일 경로) | 2/2 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/encx24j600.h` | NuttX 유래 | nuttx/net/encx24j600.h (동일 경로) | 0/0 | 유지 (TD-INC-04) | net 미sync 결정 영역. base부터 참조 0 |
| `tinyara/net/ethernet.h` | NuttX 유래 | nuttx/net/ethernet.h (동일 경로) | 4/3 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/gmii.h` | NuttX 유래 | nuttx/net/gmii.h (동일 경로) | 0/0 | 유지 (TD-INC-04) | net 미sync 결정 영역. base부터 참조 0 |
| `tinyara/net/if/ble.h` | Samsung | - | 48/44 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/if/ethernet.h` | Samsung | - | 6/6 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/if/wifi.h` | Samsung | - | 45/25 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/ioctl.h` | NuttX 유래 | nuttx/net/ioctl.h (동일 경로) | 5/5 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/ip.h` | NuttX 유래 | nuttx/net/ip.h (동일 경로) | 6/3 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/loopback.h` | NuttX 유래 | nuttx/net/loopback.h (동일 경로) | 1/1 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/mii.h` | NuttX 유래 | nuttx/net/mii.h (동일 경로) | 6/6 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/net.h` | NuttX 유래 | nuttx/net/net.h (동일 경로) | 18/16 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/netdev.h` | NuttX 유래 | nuttx/net/netdev.h (동일 경로) | 8/8 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/netlog.h` | Samsung | - | 39/15 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/phy.h` | NuttX 유래 | nuttx/net/phy.h (동일 경로) | 4/4 | 유지 | net 미sync 결정 영역 |
| `tinyara/net/telnet.h` | NuttX 유래 | nuttx/net/telnet.h (동일 경로) | 4/3 | 유지 | net 미sync 결정 영역 |
| `tinyara/netmgr/netctl.h` | Samsung | - | 12/7 | 유지 | net 미sync 결정 영역 |
| `tinyara/netmgr/netdev_mgr.h` | Samsung | - | 25/25 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/rtk/dlist.h` | Realtek | - | 0/0 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/rtk/wifi_constants.h` | Realtek | - | 3/2 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/rtk/wifi_structures.h` | Realtek | - | 3/2 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/slsi/slsi_wifi_api.h` | Samsung | - | 11/6 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/slsi/slsi_wifi_nvmap.h` | Samsung | - | 1/1 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi/wifi_manager.h` | Samsung | - | 2/0 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi_csi/wifi_csi.h` | NuttX 유래 | - | 5/4 | 유지 | net 미sync 결정 영역 |
| `tinyara/wifi_csi/wifi_csi_struct.h` | Samsung | - | 6/0 | 유지 | net 미sync 결정 영역 |

### Bluetooth / BLE (미sync 결정 영역) (15)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/ble/ble_handler.h` | Samsung | - | 1/0 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/ble/ble_log.h` | Samsung | - | 0/0 | 유지 | ble 미sync 영역. ble_manager.h가 상대 경로로 include |
| `tinyara/ble/ble_manager.h` | Samsung | - | 12/10 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bluetooth.h` | NuttX 유래 | nuttx/net/bluetooth.h | 10/8 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_buf.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_buf.h | 6/6 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_core.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_core.h | 12/12 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_driver.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_driver.h | 2/2 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_gatt.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_gatt.h | 3/3 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_hci.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_hci.h | 16/15 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_ioctl.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_ioctl.h | 1/1 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_null.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_null.h | 2/2 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/bt_uart.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_uart.h | 0/0 | 유지 | bluetooth 미sync 영역. 참조 0 (sync된 uart_bth4.c는 NuttX 경로 tinyara/wireless/bluetooth/bt_uart.h를 include) |
| `tinyara/bluetooth/bt_uuid.h` | NuttX 유래 | nuttx/wireless/bluetooth/bt_uuid.h | 4/4 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/conn.h` | - | - | 4/4 | 유지 | bluetooth 미sync 결정 영역 |
| `tinyara/bluetooth/iob/iob.h` | NuttX 유래 | nuttx/mm/iob.h (sync됨, 별도 사본) | 27/27 | 유지 | bluetooth 미sync 영역의 구 IOB 사본. NuttX tinyara/mm/iob.h와 별개로 bluetooth 코드가 사용 |

### 오디오 (10)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/audio/adam110.h` | ABOV/Samsung | - | 2/2 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/alc1019.h` | Samsung | - | 3/3 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/alc5658.h` | NuttX 유래 | - | 2/2 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/audio.h` | NuttX 유래 | nuttx/audio/audio.h (동일 경로) | 32/25 | 유지 (TD-INC-02) | TizenRT audio 드라이버가 사용. sync된 i2schar.c도 include |
| `tinyara/audio/audio_null.h` | NuttX 유래 | nuttx/audio/audio_null.h (동일 경로) | 1/1 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/cx20921.h` | NuttX 유래 | - | 1/1 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/i2s.h` | NuttX 유래 | nuttx/audio/i2s.h (동일 경로) | 36/36 | 유지 (TD-INC-02) | TizenRT codec 드라이버가 사용. sync된 drivers/i2s/i2schar.c도 이 구버전 헤더를 include |
| `tinyara/audio/ndp120.h` | Samsung | - | 3/3 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/syu645b.h` | Samsung | - | 2/2 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |
| `tinyara/audio/tas5749.h` | NuttX 유래 | - | 1/1 | 유지 | audio 드라이버(TizenRT codec) 미sync 영역 |

### 디스플레이 / 영상 / 입력 / 센서 (미sync 영역) (19)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/input/ist415.h` | NuttX 유래 | - | 4/4 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/input/touchscreen.h` | NuttX 유래 | nuttx/input/touchscreen.h (동일 경로) | 5/3 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/ili9341.h` | NuttX 유래 | nuttx/lcd/ili9341.h (동일 경로) | 4/4 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/lcd.h` | NuttX 유래 | nuttx/lcd/lcd.h (동일 경로) | 8/8 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/lcd_dev.h` | Samsung | nuttx/lcd/lcd_dev.h (동일 경로) | 10/7 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/mipi_lcd.h` | Samsung | - | 6/6 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/st7701.h` | Samsung | - | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/st7701sn.h` | Samsung | - | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/st7785.h` | Samsung | - | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/lcd/st7789.h` | Samsung | nuttx/lcd/st7789.h (동일 경로) | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/mipidsi/mipi_display.h` | NuttX 유래 | nuttx/video/mipi_display.h | 4/4 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/mipidsi/mipi_dsi.h` | NuttX 유래 | nuttx/video/mipi_dsi.h | 5/5 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/sensors/ppd42ns.h` | Samsung | - | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/sensors/qencoder.h` | NuttX 유래 | nuttx/sensors/qencoder.h (동일 경로) | 2/2 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/video/fb.h` | NuttX 유래 | nuttx/video/fb.h (동일 경로) | 15/15 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `tinyara/video/rgbcolors.h` | NuttX 유래 | nuttx/video/rgbcolors.h (동일 경로) | 0/0 | 유지 (TD-INC-04) | video 미sync 영역. base부터 참조 0 |
| `video/video.h` | Sony(Spresense) 유래 | nuttx/video/video.h | 7/6 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `video/video_controls.h` | Sony(Spresense) 유래 | sys/video_controls.h | 1/1 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |
| `video/video_halif.h` | Sony(Spresense) 유래 | - | 5/5 | 유지 | lcd/video/mipidsi/input/sensors 미sync 영역 |

### USB (15, 후속 결정으로 sync됨)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/usb/cdc.h` | NuttX 유래 | nuttx/usb/cdc.h (동일 경로) | 5/5 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/cdcacm.h` | NuttX 유래 | nuttx/usb/cdcacm.h (동일 경로) | 9/8 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/composite.h` | NuttX 유래 | nuttx/usb/composite.h (동일 경로) | 4/4 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/ehci.h` | NuttX 유래 | nuttx/usb/ehci.h (동일 경로) | 1/1 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/hub.h` | NuttX 유래 | nuttx/usb/hub.h (동일 경로) | 2/2 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/ohci.h` | NuttX 유래 | nuttx/usb/ohci.h (동일 경로) | 1/1 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/pl2303.h` | NuttX 유래 | nuttx/usb/pl2303.h (동일 경로) | 2/2 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/storage.h` | NuttX 유래 | nuttx/usb/storage.h (동일 경로) | 4/4 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usb.h` | NuttX 유래 | nuttx/usb/usb.h (동일 경로) | 31/31 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbdev.h` | NuttX 유래 | nuttx/usb/usbdev.h (동일 경로) | 21/21 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbdev_trace.h` | NuttX 유래 | nuttx/usb/usbdev_trace.h (동일 경로) | 26/26 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbhost.h` | NuttX 유래 | nuttx/usb/usbhost.h (동일 경로) | 29/29 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbhost_devaddr.h` | NuttX 유래 | nuttx/usb/usbhost_devaddr.h (동일 경로) | 10/10 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbhost_trace.h` | NuttX 유래 | nuttx/usb/usbhost_trace.h (동일 경로) | 7/7 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |
| `tinyara/usb/usbmsc.h` | NuttX 유래 | nuttx/usb/usbmsc.h (동일 경로) | 2/2 | sync됨 (`1c37bfac3`) | 후속 결정으로 usb sync |

### 전원 / PM (미sync 결정 영역) (5)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/pm/pm.h` | NuttX 유래 | nuttx/power/pm.h | 49/43 | 유지 | pm·power 미sync 결정 영역 |
| `tinyara/power/battery_charger.h` | NuttX 유래 | nuttx/power/battery_charger.h (동일 경로) | 1/1 | 유지 | pm·power 미sync 결정 영역 |
| `tinyara/power/battery_gauge.h` | NuttX 유래 | nuttx/power/battery_gauge.h (동일 경로) | 1/1 | 유지 | pm·power 미sync 결정 영역 |
| `tinyara/power/battery_ioctl.h` | NuttX 유래 | nuttx/power/battery_ioctl.h (동일 경로) | 2/2 | 유지 | pm·power 미sync 결정 영역 |
| `tinyara/power/relay.h` | NuttX 유래 | nuttx/power/relay.h (동일 경로) | 0/0 | 유지 (TD-INC-04) | power 미sync 영역. base부터 참조 0 |

### libc 보조 / 유틸 헤더 (7)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `cJSON.h` | cJSON 유래 | - | 25/2 | 이동 (후속 결정, `e710928f0`) | 사용처는 os/drivers/audio의 cJSON.c, syu645b.c 둘뿐(다른 사용처는 external `<json/cJSON.h>`). `os/drivers/audio/cJSON.h`로 옮김 |
| `tinyara/endian.h` | musl 유래 | endian.h, sys/endian.h (sync됨) | 0/0 | **삭제** | base 유일 사용처 lib/libc/math/libm.h가 NuttX libm으로 교체, endian.h·sys/endian.h sync됨. 참조 0 |
| `tinyara/features.h` | musl 유래 | - | 1/0 | 유지 | external/include/grpc port_platform.h가 사용 (삭제한 endian.h도 include했음) |
| `tinyara/fs/fs_utils.h` | NuttX 유래 | - | 9/0 | 유지 | TizenRT fs 유틸 API. apps 9개(fscmd, tash_script, fs 테스트 등)가 사용 |
| `tinyara/fs/smart_procfs.h` | NuttX 유래 | - | 0/0 | 유지 (TD-INC-06) | smartfs 작업 미완료로 유지(사용자 결정). base 사용처 smart.c·smartfs_procfs.c는 NuttX 버전(tinyara/mtd/smart.h)으로 교체, 현재 참조 0 |
| `tinyara/hashmap.h` | Samsung | - | 3/1 | 유지 (TD-LIBS-02) | lib_hashmap.c 헤더. LIB_HASHMAP 빌드 연결 복원 필요 |
| `uuid/uuid.h` | libuuid 유래 | uuid.h (sync됨, BSD API로 다름) | 3/0 | 유지 | libuuid API. iotivity/st_things port 파일이 사용 (NuttX uuid.h는 BSD API라 대체 불가) |

### 기타 (.gitignore) (2)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/.gitignore` | - | nuttx/.gitignore (동일 경로) | 0/0 | 유지 | NuttX include/nuttx/.gitignore와 내용 동일(/config.h, /version.h) |
| `tinyara/mm/.gitignore` | - | - | 0/0 | 유지 | os/mm 빌드가 생성하는 heapinfo_internal.h 무시 |

### Samsung 커널·시스템·보안 기능 (33)

| 파일 (os/include/) | 출처 | NuttX 12.13.0 대응 (include/) | 참조 전체/os | 판정 | 근거 |
|---|---|---|---|---|---|
| `tinyara/binary_manager.h` | Samsung | - | 25/15 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/binfmt/compression/compress_read.h` | Samsung | - | 5/5 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/compression.h` | Samsung | - | 7/4 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/cpuload.h` | Samsung | - | 2/1 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/debug/sysdbg.h` | Samsung | - | 19/18 | **삭제** (후속 결정, `0818e112b`) | sysdbg 기능 폐기. sync로 커널 훅과 빌드 연결이 이미 끊겨 있었음 |
| `tinyara/gpio.h` | Samsung | nuttx/ioexpander/gpio.h (sync됨, API 다름) | 27/19 | 유지 (TD-INC-01) | Samsung gpio upper-half(base drivers/gpio/gpio.c)가 [MOVE] 후 NuttX ioexpander/gpio.c로 덮어써져 gpio_register 구현 소실. chip lowerhalf 등이 사용 |
| `tinyara/iotbus_pinmap.h` | - | - | 2/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/iotbus_sig.h` | Samsung | - | 6/3 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/iotdev.h` | Samsung | - | 0/0 | **별도 판정 필요** (TD-INC-05) | 구현(id_drv_register, id_post_evt)이 base부터 없음. 유일 사용처 구 tinyara/serial/serial.h가 NuttX로 교체, 참조 0 |
| `tinyara/log_dump/log_dump.h` | Samsung | - | 3/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/log_dump/log_dump_internal.h` | Samsung | - | 2/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/lzma/7zTypes.h` | Samsung | - | 0/0 | 유지 | Samsung lzma 기능. external/lzma가 "7zTypes.h"로 include |
| `tinyara/lzma/LzmaLib.h` | Samsung | - | 2/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/mem_protect_test.h` | Samsung | - | 3/1 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/mm/heap_regioninfo.h` | Samsung | - | 14/14 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/mminfo.h` | Samsung | - | 6/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/mmu.h` | Samsung | - | 2/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/mpu.h` | Samsung | - | 7/7 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/os_api_test_drv.h` | Samsung | - | 32/11 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/otp.h` | Samsung | - | 2/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/preference.h` | Samsung | - | 14/7 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/prodconfig.h` | Samsung | - | 14/13 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/reboot_reason.h` | Samsung | - | 28/26 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/seclink.h` | Samsung | - | 27/7 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/seclink_drv.h` | Samsung | - | 24/13 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/security_hal.h` | Samsung | - | 36/16 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/security_level.h` | Samsung | - | 18/17 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/signature.h` | Samsung | - | 7/7 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/silent_reboot.h` | Samsung | - | 5/5 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/spi/ospi.h` | Samsung | - | 5/5 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/ss_slot_index.h` | Samsung | - | 4/2 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/task_manager_drv.h` | Samsung | - | 3/1 | 유지 | Samsung 기능, 사용 중 |
| `tinyara/tz_context.h` | Arm 유래 | - | 4/4 | 유지 | Samsung 기능, 사용 중 |

## 7. 남은 리뷰 영역

NO_CHANGED 트리에서 아직 보지 않은 os 하위 영역과 파일 수입니다. 리뷰가 끝나면 이 문서에 섹션을 추가합니다.

| 영역 | 파일 | 비고 |
|---|---|---|
| os/board | 8385 | 대부분 다른 보드(미포팅) |
| os/arch | 1533 | arm common/armv7-a/armv8-m 외 chip 코드 |
| os/drivers | 511 | sync 안 한 드라이버 + Samsung 드라이버 |
| os/net | 364 | net 미sync 결정 영역 |
| os/tools | 73 | |
| os/kernel | 57 | binary_manager, debug, preference 등 Samsung 기능 + NuttX sched 잔존 후보(pthread, sched, signal, wdog …) |
| os/se | 23 | |
| os/pm | 19 | pm 미sync 결정 영역 |
| os/mm | 16 | heapinfo 등 Samsung 기능 + kmm/umm getheap 등 잔존 후보 |
| os/fs | 11 | |
| os/binfmt | 7 | |
| 그 밖의 os 파일 | 23 | audio, compression, syscall, userspace, 최상위 Makefile 등 |

## 8. 후속 결정 반영 (2026-11-02)

리뷰 뒤 사용자 결정으로 바뀐 판정입니다. 결정 전체와 TD 변화는 [NuttX-12.13.0-TD.md](NuttX-12.13.0-TD.md)의 "후속 결정"을 봅니다.

| 대상 | 리뷰 판정 | 바뀐 판정 | 커밋 |
|---|---|---|---|
| `os/include/cJSON.h` | 유지 | 이동: `os/drivers/audio/cJSON.h` | `e710928f0` |
| `os/include/tinyara/debug/sysdbg.h` | 유지 (Samsung 기능) | 삭제: sysdbg 기능 전체 폐기 (`os/kernel/debug/sysdbg.c`, `apps/system/sysdbg`, le_tc 포함) | `0818e112b` |
| `os/libs/libm/libm/lib_{j0,j0f,j1,j1f,jn,jnf}.c` | 별도 판정 필요 (TD-LIBS-04) | 삭제: Bessel 함수는 `CONFIG_LIBM_TOOLCHAIN`의 툴체인 `libm.a`가 제공 | `119a92815` |
| `os/libs/libc/machine/arm/armv7-{m,r}/{Make.defs,arch_elf.c}` | 유지 (TD-LIBS-06) | sync됨: `DEV-12.13.0-LABEL-ARMV7`의 armv7-m/r 커밋 cherry-pick | `30917f838` |
| `os/include/tinyara/usb/*.h` 15개 | 유지 (usb 미sync 영역) | sync됨: usb 드라이버와 헤더 sync | `1c37bfac3` |

audio 영역 헤더(오디오 10개)는 계속 미sync이고 판정은 그대로입니다(TD-INC-02).
`DEV-12.13.0-LABEL___NO_CHANGED`는 이 결정들을 반영해 다시 만들었습니다. 이 문서의 3~6장 표와 "리뷰 기준 트리"는 리뷰 당시 기준입니다.
