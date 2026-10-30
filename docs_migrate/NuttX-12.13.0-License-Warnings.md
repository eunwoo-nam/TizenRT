# NuttX 12.13.0 파일 라이선스 적용 워닝

NuttX 12.13.0에서 가져온 파일에 삼성 라이선스를 적용한 결과와 확인이 필요한 파일 목록입니다. 결과 리포트는 [NuttX-12.13.0-Migration-Report.md](NuttX-12.13.0-Migration-Report.md)에 있습니다.

## 적용 요약

| 대상 | 파일 수 | 헤더 연도 | 방법 |
|---|---|---|---|
| TizenRT에 있던 파일, master에 삼성 헤더 있음 | 1122 | master 헤더 그대로 | master 헤더 블록 복원 |
| TizenRT에 있던 파일, 삼성 헤더 없음, NuttX 라이선스 | 13 | git 최초 추가 연도 (2016: 1개, 2023: 10개, 2026: 2개) | `license_manager.py add` (연도 지정 없음) |
| 신규, NuttX Apache 라이선스 | 2413 | 2026 | `license_manager.py add --year 2026` (툴 미지원 확장자는 툴 함수) |
| 신규, 제3자 라이선스 | 131 | 2026 | 같음 (W1) |
| 신규, 라이선스 없는 데이터 헤더 | 9 | 2026 | 같음 (W2) |
| 그대로 둔 파일 | 280 | - | 아래 "그대로 둔 파일" |

툴은 `agent/261021_license-manager` 브랜치(12777249)의 `tools/license/license_manager.py`입니다. 툴이 지원하지 않는 확장자(.S, .ld, .defs, Makefile.*, 확장자 없는 C++ 헤더 등)는 툴의 `generate_license_header()`와 `add_license_to_content()`를 그대로 불러 같은 형식으로 넣었습니다.

## W1. NuttX Apache 라이선스가 아닌 파일에 삼성 헤더를 붙인 경우

NuttX 12.13.0의 이 파일들은 Apache-2.0(ASF)이 아닌 제3자 라이선스입니다. 삼성 헤더는 맨 위에 있고, 원래 라이선스는 그 아래에 그대로 있습니다. 배포 시 원래 라이선스 고지 의무를 확인해야 합니다.

### 신규 파일 (131개, 2026)

<details><summary>BSD-3-Clause (59)</summary>

- `os/arch/arm/src/armv8-m/arm_stackcheck.c`
- `os/arch/arm/src/armv8-m/arm_vectors.c`
- `os/drivers/lcd/ra8875.c`
- `os/drivers/lcd/ra8875.h`
- `os/drivers/mtd/hamming.c`
- `os/drivers/mtd/mtd_modeltab.c`
- `os/drivers/mtd/mtd_nandecc.c`
- `os/drivers/mtd/mtd_nandmodel.c`
- `os/drivers/mtd/mtd_onfi.c`
- `os/drivers/sensors/as726x.c`
- `os/drivers/sensors/lis3dh.c`
- `os/drivers/sensors/vl53l1x.c`
- `os/include/sys/epoll.h`
- `os/include/sys/queue.h`
- `os/include/tinyara/input/xbox-controller.h`
- `os/include/tinyara/lcd/ra8875.h`
- `os/include/tinyara/modem/u-blox.h`
- `os/include/tinyara/mtd/hamming.h`
- `os/include/tinyara/mtd/onfi.h`
- `os/include/tinyara/power/bq2429x.h`
- `os/include/tinyara/sensors/as726x.h`
- `os/include/tinyara/sensors/lis3dh.h`
- `os/include/tinyara/sensors/vl53l1x.h`
- `os/libs/libc/machine/arm/armv7-a/acle-compat.h`
- `os/libs/libc/machine/arm/armv7-a/arch_memchr.S`
- `os/libs/libc/machine/arm/armv7-a/arch_memcpy.S`
- `os/libs/libc/machine/arm/armv7-a/arch_memmove.S`
- `os/libs/libc/machine/arm/armv7-a/arch_strcmp.S`
- `os/libs/libc/machine/arm/armv7-a/arch_strlen.S`
- `os/libs/libc/machine/arm/armv8-m/acle-compat.h`
- `os/libs/libc/machine/arm/armv8-m/arch_memchr.S`
- `os/libs/libc/machine/arm/armv8-m/arch_memcpy.S`
- `os/libs/libc/machine/arm/armv8-m/arch_memmove.S`
- `os/libs/libc/machine/arm/armv8-m/arch_memset.S`
- `os/libs/libc/machine/arm/armv8-m/arch_strcmp.S`
- `os/libs/libc/machine/arm/armv8-m/arch_strlen.S`
- `os/libs/libc/misc/lib_utsname.c`
- `os/libs/libc/pthread/pthread_get_stackaddr_np.c`
- `os/libs/libc/search/hcreate.c`
- `os/libs/libc/stream/lib_dtoa_data.c`
- `os/libs/libc/stream/lib_dtoa_engine.c`
- `os/libs/libc/stream/lib_dtoa_engine.h`
- `os/libs/libc/stream/lib_libvsprintf.c`
- `os/libs/libc/stream/lib_ultoa_invert.c`
- `os/libs/libc/stream/lib_ultoa_invert.h`
- `os/libs/libc/wchar/lib_wcslcat.c`
- `os/libs/libm/libm/arm/armv8-m/arch_ceil.c`
- `os/libs/libm/libm/arm/armv8-m/arch_ceilf.c`
- `os/libs/libm/libm/arm/armv8-m/arch_floor.c`
- `os/libs/libm/libm/arm/armv8-m/arch_floorf.c`
- `os/libs/libm/libm/arm/armv8-m/arch_nearbyint.c`
- `os/libs/libm/libm/arm/armv8-m/arch_nearbyintf.c`
- `os/libs/libm/libm/arm/armv8-m/arch_rint.c`
- `os/libs/libm/libm/arm/armv8-m/arch_rintf.c`
- `os/libs/libm/libm/arm/armv8-m/arch_round.c`
- `os/libs/libm/libm/arm/armv8-m/arch_roundf.c`
- `os/libs/libm/libm/arm/armv8-m/arch_trunc.c`
- `os/libs/libm/libm/arm/armv8-m/arch_truncf.c`
- `os/libs/libm/newlib/include/tgmath.h`

</details>

<details><summary>BSD-2-Clause (23)</summary>

- `os/drivers/misc/optee_msg.h`
- `os/drivers/misc/optee_smc.h`
- `os/include/android/fdsan.h`
- `os/include/lzf.h`
- `os/include/regex.h`
- `os/include/sys/tree.h`
- `os/include/sys/utsname.h`
- `os/include/tinyara/list.h`
- `os/include/tinyara/list_type.h`
- `os/include/tinyara/tee.h`
- `os/libs/libc/lzf/lzf.h`
- `os/libs/libc/lzf/lzf_c.c`
- `os/libs/libc/lzf/lzf_d.c`
- `os/libs/libc/machine/arm/armv7-a/arch_memset.S`
- `os/libs/libc/misc/lib_fdsan.c`
- `os/libs/libc/misc/lib_fnmatch.c`
- `os/libs/libc/pthread/pthread_get_stacksize_np.c`
- `os/libs/libc/regex/regcomp.c`
- `os/libs/libc/regex/regerror.c`
- `os/libs/libc/regex/regexec.c`
- `os/libs/libc/regex/tre-mem.c`
- `os/libs/libc/regex/tre.h`
- `os/libs/libc/search/hcreate_r.c`

</details>

<details><summary>MIT (22)</summary>

- `os/libs/libc/stdio/lib_fgetwc.c`
- `os/libs/libc/stdio/lib_fputwc.c`
- `os/libs/libc/stdio/lib_fputws.c`
- `os/libs/libc/stdio/lib_getwc.c`
- `os/libs/libc/stdio/lib_putwc.c`
- `os/libs/libc/stdio/lib_putwchar.c`
- `os/libs/libc/stdio/lib_ungetwc.c`
- `os/libs/libc/wchar/lib_wcscat.c`
- `os/libs/libc/wchar/lib_wcschr.c`
- `os/libs/libc/wchar/lib_wcscpy.c`
- `os/libs/libc/wchar/lib_wcscspn.c`
- `os/libs/libc/wchar/lib_wcsncat.c`
- `os/libs/libc/wchar/lib_wcsncmp.c`
- `os/libs/libc/wchar/lib_wcsncpy.c`
- `os/libs/libc/wchar/lib_wcspbrk.c`
- `os/libs/libc/wchar/lib_wcsrchr.c`
- `os/libs/libc/wchar/lib_wcsspn.c`
- `os/libs/libc/wchar/lib_wcsstr.c`
- `os/libs/libc/wchar/lib_wcstok.c`
- `os/libs/libc/wchar/lib_wcswcs.c`
- `os/libs/libc/wchar/lib_wcswidth.c`
- `os/libs/libc/wchar/lib_wcwidth.c`

</details>

<details><summary>BSD (17)</summary>

- `os/libs/libc/string/lib_bsdmemccpy.c`
- `os/libs/libc/string/lib_bsdmemchr.c`
- `os/libs/libc/string/lib_bsdmemcmp.c`
- `os/libs/libc/string/lib_bsdmemcpy.c`
- `os/libs/libc/string/lib_bsdmemrchr.c`
- `os/libs/libc/string/lib_bsdmemset.c`
- `os/libs/libc/string/lib_bsdstpcpy.c`
- `os/libs/libc/string/lib_bsdstpncpy.c`
- `os/libs/libc/string/lib_bsdstrcat.c`
- `os/libs/libc/string/lib_bsdstrchr.c`
- `os/libs/libc/string/lib_bsdstrchrnul.c`
- `os/libs/libc/string/lib_bsdstrcmp.c`
- `os/libs/libc/string/lib_bsdstrcpy.c`
- `os/libs/libc/string/lib_bsdstrlen.c`
- `os/libs/libc/string/lib_bsdstrncmp.c`
- `os/libs/libc/string/lib_bsdstrncpy.c`
- `os/libs/libc/string/lib_bsdstrrchr.c`

</details>

<details><summary>other (7)</summary>

- `os/include/tinyara/input/x11_keysym.h`
- `os/include/tinyara/input/x11_keysymdef.h`
- `os/include/tinyara/lcd/ili9488.h`
- `os/libs/libc/string/lib_memmem.c`
- `os/libs/libxx/libminiabi/libxx_dynamic_cast.cxx`
- `os/libs/libxx/libminiabi/libxx_typeinfo.cxx`
- `os/libs/libxx/libminiabi/libxx_typeinfo.h`

</details>

<details><summary>BSD-3-Clause-UC (1)</summary>

- `os/libs/libc/search/hash_func.c`

</details>

<details><summary>ISC (1)</summary>

- `os/libs/libc/string/lib_timingsafe_bcmp.c`

</details>

<details><summary>LicenseRef-NuttX-PublicDomain (1)</summary>

- `os/include/search.h`

</details>

### TizenRT에 있던 파일 (53개, master 헤더 복원)

<details><summary>BSD-2-Clause (21)</summary>

- `os/libs/libc/stdlib/lib_mbtowc.c`
- `os/libs/libc/string/lib_strcoll.c`
- `os/libs/libc/string/lib_strxfrm.c`
- `os/libs/libc/wchar/lib_btowc.c`
- `os/libs/libc/wchar/lib_mbrlen.c`
- `os/libs/libc/wchar/lib_mbrtowc.c`
- `os/libs/libc/wchar/lib_mbsrtowcs.c`
- `os/libs/libc/wchar/lib_wcslen.c`
- `os/libs/libc/wchar/lib_wcstod.c`
- `os/libs/libc/wchar/lib_wcstof.c`
- `os/libs/libc/wchar/lib_wcstol.c`
- `os/libs/libc/wchar/lib_wcstold.c`
- `os/libs/libc/wchar/lib_wcstoll.c`
- `os/libs/libc/wchar/lib_wcstoul.c`
- `os/libs/libc/wchar/lib_wcstoull.c`
- `os/libs/libc/wchar/lib_wcsxfrm.c`
- `os/libs/libc/wchar/lib_wmemchr.c`
- `os/libs/libc/wchar/lib_wmemcmp.c`
- `os/libs/libc/wchar/lib_wmemcpy.c`
- `os/libs/libc/wchar/lib_wmemmove.c`
- `os/libs/libc/wchar/lib_wmemset.c`

</details>

<details><summary>BSD-3-Clause (20)</summary>

- `os/arch/arm/src/armv8-m/dwt.h`
- `os/arch/arm/src/armv8-m/itm.h`
- `os/arch/arm/src/armv8-m/tpi.h`
- `os/drivers/mtd/mtd_nand.c`
- `os/drivers/mtd/mtd_nandscheme.c`
- `os/include/tinyara/mtd/nand.h`
- `os/include/tinyara/mtd/nand_config.h`
- `os/include/tinyara/mtd/nand_ecc.h`
- `os/include/tinyara/mtd/nand_model.h`
- `os/include/tinyara/mtd/nand_raw.h`
- `os/include/tinyara/mtd/nand_scheme.h`
- `os/include/tinyara/power/relay.h`
- `os/libs/libc/stdlib/lib_bsearch.c`
- `os/libs/libc/stdlib/lib_div.c`
- `os/libs/libc/wchar/lib_wcslcpy.c`
- `os/libs/libm/libm/__cos.c`
- `os/libs/libm/libm/__sin.c`
- `os/libs/libm/libm/lib_copysignf.c`
- `os/libs/libm/libm/lib_gamma.c`
- `os/libs/libm/libm/lib_lgamma.c`

</details>

<details><summary>MIT (5)</summary>

- `os/libs/libc/stdlib/lib_strtold.c`
- `os/libs/libc/string/lib_strstr.c`
- `os/libs/libm/libm/lib_trunc.c`
- `os/libs/libm/libm/lib_truncf.c`
- `os/libs/libm/libm/lib_truncl.c`

</details>

<details><summary>TwistedSNMP (4)</summary>

- `os/libs/libc/wctype/lib_iswctype.c`
- `os/libs/libc/wctype/lib_towlower.c`
- `os/libs/libc/wctype/lib_towupper.c`
- `os/libs/libc/wctype/lib_wctype.c`

</details>

<details><summary>BSD-3-Clause-UC (1)</summary>

- `os/libs/libc/stdlib/lib_wctomb.c`

</details>

<details><summary>Zlib (1)</summary>

- `os/libs/libc/string/lib_vikmemcpy.c`

</details>

<details><summary>other (1)</summary>

- `os/libs/libc/time/lib_strptime.c`

</details>

`other`는 SPDX 식별자 없이 copyright나 permission 문구만 있는 파일입니다.

## W2. 원래 라이선스가 없는데 삼성 헤더를 붙인 경우

NuttX 12.13.0에서 라이선스 문구가 없는 파일입니다. 신규 파일은 locale 변환표와 X11 keysym 같은 데이터 헤더이고, 기존 파일은 master 헤더를 복원했습니다. 원 출처(musl, X.Org 등)의 라이선스를 확인해야 합니다.

| 구분 | 파일 |
|---|---|
| 신규 (2026) | `os/include/tinyara/input/x11_xf86keysym.h` |
| 신규 (2026) | `os/libs/libc/locale/big5.h` |
| 신규 (2026) | `os/libs/libc/locale/codepages.h` |
| 신규 (2026) | `os/libs/libc/locale/gb18030.h` |
| 신규 (2026) | `os/libs/libc/locale/hkscs.h` |
| 신규 (2026) | `os/libs/libc/locale/jis0208.h` |
| 신규 (2026) | `os/libs/libc/locale/ksc.h` |
| 신규 (2026) | `os/libs/libc/locale/legacychars.h` |
| 신규 (2026) | `os/libs/libc/locale/revjis.h` |
| TizenRT에 있던 파일 (2017, 복원) | `os/libs/libm/libm/lib_scalbn.c` |
| TizenRT에 있던 파일 (2017, 복원) | `os/libs/libm/libm/lib_scalbnf.c` |
| TizenRT에 있던 파일 (2017, 복원) | `os/libs/libm/libm/lib_scalbnl.c` |

## W3. 라이선스 없이 그대로 둔 신규 Kconfig (89개)

NuttX의 Kconfig에는 라이선스 문구가 없습니다. 사용자 결정에 따라 삼성 헤더도 넣지 않았습니다. TizenRT에 있던 Kconfig도 같은 방식입니다.

<details><summary>목록</summary>

- `os/arch/arm/src/mps/Kconfig`
- `os/arch/arm/src/qemu/Kconfig`
- `os/binfmt/libnxflat/Kconfig`
- `os/board/mps2-an521/Kconfig`
- `os/board/qemu-armv7a/Kconfig`
- `os/drivers/bch/Kconfig`
- `os/drivers/crypto/Kconfig`
- `os/drivers/dummy/Kconfig`
- `os/drivers/i2s/Kconfig`
- `os/drivers/ioexpander/Kconfig`
- `os/drivers/loop/Kconfig`
- `os/drivers/misc/Kconfig`
- `os/drivers/note/Kconfig`
- `os/drivers/power/battery/Kconfig`
- `os/drivers/power/relay/Kconfig`
- `os/drivers/power/supply/Kconfig`
- `os/drivers/serial/Kconfig-16550`
- `os/drivers/serial/Kconfig-cmsdk`
- `os/drivers/serial/Kconfig-pci`
- `os/drivers/serial/Kconfig-pl011`
- `os/drivers/serial/Kconfig-sci`
- `os/drivers/serial/Kconfig-usart`
- `os/drivers/serial/Kconfig-xlnxps`
- `os/drivers/timers/Kconfig`
- `os/drivers/video/mipidsi/Kconfig`
- `os/drivers/video/vnc/Kconfig`
- `os/fs/binfs/Kconfig`
- `os/fs/cromfs/Kconfig`
- `os/fs/event/Kconfig`
- `os/fs/fat/Kconfig`
- `os/fs/hostfs/Kconfig`
- `os/fs/mmap/Kconfig`
- `os/fs/mnemofs/Kconfig`
- `os/fs/nxffs/Kconfig`
- `os/fs/partition/Kconfig`
- `os/fs/shm/Kconfig`
- `os/fs/spiffs/Kconfig`
- `os/fs/unionfs/Kconfig`
- `os/fs/userfs/Kconfig`
- `os/fs/v9fs/Kconfig`
- `os/fs/vfs/Kconfig`
- `os/fs/zipfs/Kconfig`
- `os/graphics/Kconfig`
- `os/libs/libbuiltin/Kconfig`
- `os/libs/libc/audio/Kconfig`
- `os/libs/libc/audio/libsrc/Kconfig`
- `os/libs/libc/dlfcn/Kconfig`
- `os/libs/libc/fdt/Kconfig`
- `os/libs/libc/gdbstub/Kconfig`
- `os/libs/libc/gnssutils/Kconfig`
- `os/libs/libc/grp/Kconfig`
- `os/libs/libc/hex2bin/Kconfig`
- `os/libs/libc/locale/Kconfig`
- `os/libs/libc/lzf/Kconfig`
- `os/libs/libc/machine/Kconfig`
- `os/libs/libc/machine/arm/Kconfig`
- `os/libs/libc/machine/arm/armv7-a/Kconfig`
- `os/libs/libc/machine/arm/armv8-m/Kconfig`
- `os/libs/libc/misc/Kconfig`
- `os/libs/libc/pthread/Kconfig`
- `os/libs/libc/pwd/Kconfig`
- `os/libs/libc/queue/Kconfig`
- `os/libs/libc/regex/Kconfig`
- `os/libs/libc/stdio/Kconfig`
- `os/libs/libc/stdlib/Kconfig`
- `os/libs/libc/stream/Kconfig`
- `os/libs/libc/string/Kconfig`
- `os/libs/libc/symtab/Kconfig`
- `os/libs/libc/time/Kconfig`
- `os/libs/libc/tls/Kconfig`
- `os/libs/libc/unistd/Kconfig`
- `os/libs/libc/userfs/Kconfig`
- `os/libs/libdsp/Kconfig`
- `os/libs/libm/Kconfig`
- `os/libs/libm/libm/arm/Kconfig`
- `os/libs/libm/libm/arm/armv7-m/Kconfig`
- `os/libs/libm/libm/arm/armv8-m/Kconfig`
- `os/libs/libm/libmcs/Kconfig`
- `os/libs/libm/newlib/Kconfig`
- `os/libs/libnx/Kconfig`
- `os/libs/libnx/nx/Kconfig`
- `os/libs/libnx/nxfonts/Kconfig`
- `os/libs/libnx/nxglib/Kconfig`
- `os/libs/libnx/nxmu/Kconfig`
- `os/libs/libnx/nxtk/Kconfig`
- `os/mm/iob/Kconfig`
- `os/mm/kasan/Kconfig`
- `os/video/Kconfig`
- `os/video/videomode/Kconfig`

</details>

## W4. `license_manager.py check` 연도 불일치 (501개)

툴의 `get_creation_year()`는 현재 경로에 대해 `git log --diff-filter=A`의 가장 오래된 연도를 생성 연도로 봅니다. `--follow`는 쓰지 않습니다(rename 유사도 오판을 피하려는 설계). 그래서 경로나 이름이 바뀐 파일은 바뀐 커밋의 연도를 생성 연도로 봅니다.

이번 작업은 사용자 결정대로 TizenRT에 있던 파일에는 master 헤더 연도, 신규 파일에는 2026을 썼습니다. 그래서 아래 파일은 `check`에서 YEAR_MISMATCH로 나옵니다.

> **주의**: 이 파일들에 `license_manager.py fix`를 돌리면 헤더 연도가 툴 연도로 바뀝니다. 툴을 `fix`로 일괄 적용하지 마세요.

확인 방법: TizenRT 전체 히스토리(12658 커밋, 2016-08-30부터)에서 `license_manager.py check`(연도 지정 없음)를 툴 지원 확장자 대상 3615개에 돌린 결과입니다. Missing은 0개입니다.

| 사유 | 파일 수 | 설명 |
|---|---|---|
| `moved` | 416 | 이번 마이그레이션의 이동 커밋으로 경로가 바뀜 |
| `readded` | 27 | TizenRT에서 삭제됐던 경로에 NuttX 파일이 다시 추가됨 |
| `history-rename+moved` | 20 | TizenRT 과거 히스토리에서 바뀌었고 이번에도 이동 |
| `master-mismatch` | 17 | master에서도 헤더 연도와 git 추가 연도가 다름 |
| `moved+renamed` | 14 | 이번 마이그레이션의 이동 커밋으로 경로와 이름이 바뀜 |
| `history-rename` | 5 | TizenRT 과거 히스토리에서 경로나 이름이 바뀜 |
| `master-mismatch+moved` | 2 | master에서도 다르고 이번에 이동 |

열 설명: 헤더 = 파일의 삼성 헤더 연도, 툴 = 툴이 계산한 연도(현재 경로), master 툴 = master에서 이전 경로로 계산한 툴 연도, follow = master에서 `git log --follow`로 본 최초 추가 연도.

<details><summary>moved (416)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/board/boardctl.c` | `os/arch/boardctl.c` | 2017 | 2026 | 2017 | 2017 |
| `os/drivers/crypto/dev_urandom.c` | `os/drivers/dev_urandom.c` | 2018 | 2026 | 2018 | 2018 |
| `os/drivers/i2s/i2schar.c` | `os/drivers/audio/i2schar.c` | 2017 | 2026 | 2017 | 2017 |
| `os/drivers/ioexpander/Make.defs` | `os/drivers/gpio/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/ioexpander/gpio.c` | `os/drivers/gpio/gpio.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/loop/loop.c` | `os/drivers/loop.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/misc/dev_null.c` | `os/drivers/dev_null.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/misc/dev_zero.c` | `os/drivers/dev_zero.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/misc/ramdisk.c` | `os/fs/driver/block/ramdisk.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/misc/rwbuffer.c` | `os/drivers/rwbuffer.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/dhara.c` | `os/fs/driver/mtd/dhara.c` | 2025 | 2026 | 2025 | 2025 |
| `os/drivers/mtd/ftl.c` | `os/fs/driver/mtd/ftl.c` | 2017 | 2026 | 2017 | 2017 |
| `os/drivers/mtd/m25px.c` | `os/fs/driver/mtd/m25px/m25px.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/mtd_config.c` | `os/fs/driver/mtd/mtd_config.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/mtd_nand.c` | `os/fs/driver/mtd/mtd_nand.c` | 2025 | 2026 | 2025 | 2025 |
| `os/drivers/mtd/mtd_nandscheme.c` | `os/fs/driver/mtd/mtd_nandscheme.c` | 2025 | 2026 | 2025 | 2025 |
| `os/drivers/mtd/mtd_partition.c` | `os/fs/driver/mtd/mtd_partition.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/mtd_progmem.c` | `os/fs/driver/mtd/mtd_progmem.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/rammtd.c` | `os/fs/driver/mtd/rammtd/rammtd.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/smart.c` | `os/fs/driver/mtd/smart.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/mtd/w25.c` | `os/fs/driver/mtd/w25.c` | 2018 | 2026 | 2018 | 2018 |
| `os/drivers/mtd/w25n.c` | `os/fs/driver/mtd/w25n.c` | 2025 | 2026 | 2025 | 2025 |
| `os/drivers/timers/pwm.c` | `os/drivers/pwm.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/timers/timer.c` | `os/drivers/timer.c` | 2016 | 2026 | 2016 | 2016 |
| `os/drivers/timers/watchdog.c` | `os/drivers/watchdog.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/driver.h` | `os/fs/driver/block/driver.h` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_blockproxy.c` | `os/fs/driver/block/fs_blockproxy.c` | 2017 | 2026 | 2017 | 2017 |
| `os/fs/driver/fs_closeblockdriver.c` | `os/fs/driver/block/fs_closeblockdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_findblockdriver.c` | `os/fs/driver/block/fs_findblockdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_openblockdriver.c` | `os/fs/driver/block/fs_openblockdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_registerblockdriver.c` | `os/fs/driver/block/fs_registerblockdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_registerdriver.c` | `os/fs/driver/block/fs_registerdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_registermtddriver.c` | `os/fs/driver/mtd/fs_registermtddriver.c` | 2025 | 2026 | 2025 | 2016 |
| `os/fs/driver/fs_unregisterblockdriver.c` | `os/fs/driver/block/fs_unregisterblockdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_unregisterdriver.c` | `os/fs/driver/block/fs_unregisterdriver.c` | 2016 | 2026 | 2016 | 2016 |
| `os/fs/driver/fs_unregistermtddriver.c` | `os/fs/driver/mtd/fs_unregistermtddriver.c` | 2025 | 2026 | 2025 | 2016 |
| `os/libs/libc/Makefile` | `lib/libc/Makefile` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/Make.defs` | `lib/libc/aio/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/aio.h` | `lib/libc/aio/aio.h` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/aio_error.c` | `lib/libc/aio/aio_error.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/aio_return.c` | `lib/libc/aio/aio_return.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/aio_suspend.c` | `lib/libc/aio/aio_suspend.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/aio/lio_listio.c` | `lib/libc/aio/lio_listio.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/audio/Make.defs` | `lib/libc/audio/Make.defs` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/audio/lib_buffer.c` | `lib/libc/audio/lib_buffer.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/builtin/Make.defs` | `os/binfmt/libbuiltin/Make.defs` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/dirent/Make.defs` | `lib/libc/dirent/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/dirent/lib_readdirr.c` | `lib/libc/dirent/lib_readdirr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/dirent/lib_telldir.c` | `lib/libc/dirent/lib_telldir.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/elf/Make.defs` | `os/binfmt/libelf/Make.defs` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/fixedmath/Make.defs` | `lib/libc/fixedmath/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/fixedmath/lib_b16atan2.c` | `lib/libc/fixedmath/lib_b16atan2.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/fixedmath/lib_b16cos.c` | `lib/libc/fixedmath/lib_b16cos.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/fixedmath/lib_b16sin.c` | `lib/libc/fixedmath/lib_b16sin.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/fixedmath/lib_fixedmath.c` | `lib/libc/fixedmath/lib_fixedmath.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/inttypes/Make.defs` | `lib/libc/inttypes/Make.defs` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/inttypes/lib_imaxabs.c` | `lib/libc/inttypes/lib_imaxabs.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/inttypes/lib_imaxdiv.c` | `lib/libc/inttypes/lib_imaxdiv.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/inttypes/lib_strtoimax.c` | `lib/libc/inttypes/lib_strtoimax.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/inttypes/lib_strtoumax.c` | `lib/libc/inttypes/lib_strtoumax.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/libgen/Make.defs` | `lib/libc/libgen/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/libgen/lib_basename.c` | `lib/libc/libgen/lib_basename.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/libgen/lib_dirname.c` | `lib/libc/libgen/lib_dirname.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/locale/Make.defs` | `lib/libc/locale/Make.defs` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/locale/lib_localeconv.c` | `lib/libc/locale/lib_localeconv.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/locale/lib_setlocale.c` | `lib/libc/locale/lib_setlocale.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/machine/Make.defs` | `lib/libc/machine/Make.defs` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/machine/arm/Make.defs` | `lib/libc/machine/arm/Make.defs` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/machine/arm/armv7-a/Make.defs` | `lib/libc/machine/arm/armv7-a/Make.defs` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libc/machine/arm/armv7-a/arch_elf.c` | `lib/libc/machine/arm/armv7-a/arch_elf.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libc/misc/Make.defs` | `lib/libc/misc/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/misc/lib_crc16.c` | `lib/libc/misc/lib_crc16.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/misc/lib_crc32.c` | `lib/libc/misc/lib_crc32.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/misc/lib_crc8.c` | `lib/libc/misc/lib_crc8.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/misc/lib_dumpbuffer.c` | `lib/libc/misc/lib_dumpbuffer.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/misc/lib_envpath.c` | `lib/libc/misc/lib_envpath.c` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/misc/lib_xorshift128.c` | `lib/libc/misc/lib_xorshift128.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/pthread/Make.defs` | `lib/libc/pthread/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/pthread/pthread_attr_getaffinity.c` | `lib/libc/pthread/pthread_attr_getaffinity.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libc/pthread/pthread_attr_setaffinity.c` | `lib/libc/pthread/pthread_attr_setaffinity.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libc/pthread/pthread_mutexattr_getprotocol.c` | `lib/libc/pthread/pthread_mutexattr_getprotocol.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_mutexattr_getrobust.c` | `lib/libc/pthread/pthread_mutexattr_getrobust.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_mutexattr_setprotocol.c` | `lib/libc/pthread/pthread_mutexattr_setprotocol.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_mutexattr_setrobust.c` | `lib/libc/pthread/pthread_mutexattr_setrobust.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_rwlock.c` | `lib/libc/pthread/pthread_rwlock.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_rwlock_rdlock.c` | `lib/libc/pthread/pthread_rwlock_rdlock.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_rwlock_wrlock.c` | `lib/libc/pthread/pthread_rwlock_wrlock.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/queue/Make.defs` | `lib/libc/queue/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/sched/Make.defs` | `lib/libc/sched/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/sched/sched_getprioritymax.c` | `lib/libc/sched/sched_getprioritymax.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/sched/sched_getprioritymin.c` | `lib/libc/sched/sched_getprioritymin.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/sched/task_startup.c` | `lib/libc/sched/task_startup.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/semaphore/Make.defs` | `lib/libc/semaphore/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/semaphore/sem_getprotocol.c` | `lib/libc/semaphore/sem_getprotocol.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/semaphore/sem_getvalue.c` | `lib/libc/semaphore/sem_getvalue.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/semaphore/sem_init.c` | `lib/libc/semaphore/sem_init.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/semaphore/sem_setprotocol.c` | `lib/libc/semaphore/sem_setprotocol.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/signal/Make.defs` | `lib/libc/signal/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_addset.c` | `lib/libc/signal/sig_addset.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_delset.c` | `lib/libc/signal/sig_delset.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_emptyset.c` | `lib/libc/signal/sig_emptyset.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_fillset.c` | `lib/libc/signal/sig_fillset.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_hold.c` | `lib/libc/signal/sig_hold.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_ignore.c` | `lib/libc/signal/sig_ignore.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/signal/sig_ismember.c` | `lib/libc/signal/sig_ismember.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_pause.c` | `lib/libc/signal/sig_pause.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/signal/sig_psignal.c` | `lib/libc/signal/sig_psignal.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/signal/sig_raise.c` | `lib/libc/signal/sig_raise.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/signal/sig_relse.c` | `lib/libc/signal/sig_relse.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/signal/sig_set.c` | `lib/libc/signal/sig_set.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/stdio/Make.defs` | `lib/libc/stdio/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_asprintf.c` | `lib/libc/stdio/lib_asprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_clearerr.c` | `lib/libc/stdio/lib_clearerr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_dprintf.c` | `lib/libc/stdio/lib_dprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fclose.c` | `lib/libc/stdio/lib_fclose.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_feof.c` | `lib/libc/stdio/lib_feof.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_ferror.c` | `lib/libc/stdio/lib_ferror.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fflush.c` | `lib/libc/stdio/lib_fflush.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fgetc.c` | `lib/libc/stdio/lib_fgetc.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fgetpos.c` | `lib/libc/stdio/lib_fgetpos.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fgets.c` | `lib/libc/stdio/lib_fgets.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fileno.c` | `lib/libc/stdio/lib_fileno.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fopen.c` | `lib/libc/stdio/lib_fopen.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fprintf.c` | `lib/libc/stdio/lib_fprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fputc.c` | `lib/libc/stdio/lib_fputc.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fputs.c` | `lib/libc/stdio/lib_fputs.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fread.c` | `lib/libc/stdio/lib_fread.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_freopen.c` | `lib/libc/stdio/lib_freopen.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/stdio/lib_fseek.c` | `lib/libc/stdio/lib_fseek.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fsetpos.c` | `lib/libc/stdio/lib_fsetpos.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_ftell.c` | `lib/libc/stdio/lib_ftell.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_fwrite.c` | `lib/libc/stdio/lib_fwrite.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_gets.c` | `lib/libc/stdio/lib_gets.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_gets_s.c` | `lib/libc/stdio/lib_gets_s.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_libfflush.c` | `lib/libc/stdio/lib_libfflush.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_libfgets.c` | `lib/libc/stdio/lib_libfgets.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_libflushall.c` | `lib/libc/stdio/lib_libflushall.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_libfwrite.c` | `lib/libc/stdio/lib_libfwrite.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_perror.c` | `lib/libc/stdio/lib_perror.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_printf.c` | `lib/libc/stdio/lib_printf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_puts.c` | `lib/libc/stdio/lib_puts.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_remove.c` | `lib/libc/stdio/lib_remove.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/stdio/lib_setbuf.c` | `lib/libc/stdio/lib_setbuf.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libc/stdio/lib_setvbuf.c` | `lib/libc/stdio/lib_setvbuf.c` | 2017 | 2026 | 2017 | 2017 |
| `os/libs/libc/stdio/lib_snprintf.c` | `lib/libc/stdio/lib_snprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_sprintf.c` | `lib/libc/stdio/lib_sprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_sscanf.c` | `lib/libc/stdio/lib_sscanf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_tempnam.c` | `lib/libc/stdio/lib_tempnam.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_tmpnam.c` | `lib/libc/stdio/lib_tmpnam.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_ungetc.c` | `lib/libc/stdio/lib_ungetc.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_vasprintf.c` | `lib/libc/stdio/lib_vasprintf.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/stdio/lib_vdprintf.c` | `lib/libc/stdio/lib_vdprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_vfprintf.c` | `lib/libc/stdio/lib_vfprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_vprintf.c` | `lib/libc/stdio/lib_vprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_vsnprintf.c` | `lib/libc/stdio/lib_vsnprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdio/lib_vsprintf.c` | `lib/libc/stdio/lib_vsprintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/Make.defs` | `lib/libc/stdlib/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_abort.c` | `lib/libc/stdlib/lib_abort.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_abs.c` | `lib/libc/stdlib/lib_abs.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_bsearch.c` | `lib/libc/stdlib/lib_bsearch.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_checkbase.c` | `lib/libc/stdlib/lib_checkbase.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_div.c` | `lib/libc/stdlib/lib_div.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_itoa.c` | `lib/libc/stdlib/lib_itoa.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_labs.c` | `lib/libc/stdlib/lib_labs.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_ldiv.c` | `lib/libc/stdlib/lib_ldiv.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_llabs.c` | `lib/libc/stdlib/lib_llabs.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_lldiv.c` | `lib/libc/stdlib/lib_lldiv.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_mbtowc.c` | `lib/libc/stdlib/lib_mbtowc.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/stdlib/lib_mkstemp.c` | `lib/libc/stdlib/lib_mkstemp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_mktemp.c` | `lib/libc/stdlib/lib_mktemp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_qsort.c` | `lib/libc/stdlib/lib_qsort.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_rand.c` | `lib/libc/stdlib/lib_rand.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_strtol.c` | `lib/libc/stdlib/lib_strtol.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_strtold.c` | `lib/libc/stdlib/lib_strtold.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/stdlib/lib_strtoll.c` | `lib/libc/stdlib/lib_strtoll.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_strtoul.c` | `lib/libc/stdlib/lib_strtoul.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_strtoull.c` | `lib/libc/stdlib/lib_strtoull.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/stdlib/lib_wctomb.c` | `lib/libc/stdlib/lib_wctomb.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/Make.defs` | `lib/libc/string/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_fls.c` | `lib/libc/string/lib_fls.c` | 2025 | 2026 | 2025 | 2016 |
| `os/libs/libc/string/lib_isbasedigit.c` | `lib/libc/string/lib_isbasedigit.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memccpy.c` | `lib/libc/string/lib_memccpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memchr.c` | `lib/libc/string/lib_memchr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memcmp.c` | `lib/libc/string/lib_memcmp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memcpy.c` | `lib/libc/string/lib_memcpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memmove.c` | `lib/libc/string/lib_memmove.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_memset.c` | `lib/libc/string/lib_memset.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_skipspace.c` | `lib/libc/string/lib_skipspace.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_stpcpy.c` | `lib/libc/string/lib_stpcpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_stpncpy.c` | `lib/libc/string/lib_stpncpy.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/lib_strcasecmp.c` | `lib/libc/string/lib_strcasecmp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strcasestr.c` | `lib/libc/string/lib_strcasestr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strcat.c` | `lib/libc/string/lib_strcat.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strchr.c` | `lib/libc/string/lib_strchr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strcmp.c` | `lib/libc/string/lib_strcmp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strcoll.c` | `lib/libc/string/lib_strcoll.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/lib_strcpy.c` | `lib/libc/string/lib_strcpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strcspn.c` | `lib/libc/string/lib_strcspn.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strdup.c` | `lib/libc/string/lib_strdup.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strerror.c` | `lib/libc/string/lib_strerror.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strerrorr.c` | `lib/libc/string/lib_strerrorr.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/lib_strlcpy.c` | `lib/libc/string/lib_strlcpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strlen.c` | `lib/libc/string/lib_strlen.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strncasecmp.c` | `lib/libc/string/lib_strncasecmp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strncat.c` | `lib/libc/string/lib_strncat.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strncmp.c` | `lib/libc/string/lib_strncmp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strncpy.c` | `lib/libc/string/lib_strncpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strndup.c` | `lib/libc/string/lib_strndup.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strnlen.c` | `lib/libc/string/lib_strnlen.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strpbrk.c` | `lib/libc/string/lib_strpbrk.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strrchr.c` | `lib/libc/string/lib_strrchr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strsignal.c` | `lib/libc/string/lib_strsignal.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/lib_strspn.c` | `lib/libc/string/lib_strspn.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strstr.c` | `lib/libc/string/lib_strstr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strtok.c` | `lib/libc/string/lib_strtok.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strtokr.c` | `lib/libc/string/lib_strtokr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/string/lib_strxfrm.c` | `lib/libc/string/lib_strxfrm.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/string/lib_vikmemcpy.c` | `lib/libc/string/lib_vikmemcpy.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/symtab/Make.defs` | `lib/libc/symtab/Make.defs` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/symtab/symtab_findbyname.c` | `lib/libc/symtab/symtab_findbyname.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/symtab/symtab_findbyvalue.c` | `lib/libc/symtab/symtab_findbyvalue.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/symtab/symtab_sortbyname.c` | `lib/libc/symtab/symtab_sortbyname.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/syslog/Make.defs` | `lib/libc/syslog/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/syslog/lib_setlogmask.c` | `lib/libc/syslog/lib_setlogmask.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/syslog/lib_syslog.c` | `lib/libc/syslog/lib_syslog.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/syslog/syslog.h` | `lib/libc/syslog/syslog.h` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/termios/Make.defs` | `lib/libc/termios/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/termios/lib_isatty.c` | `lib/libc/termios/lib_isatty.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/termios/lib_tcflush.c` | `lib/libc/termios/lib_tcflush.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/termios/lib_tcgetattr.c` | `lib/libc/termios/lib_tcgetattr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/termios/lib_tcsetattr.c` | `lib/libc/termios/lib_tcsetattr.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/Make.defs` | `lib/libc/time/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_calendar2utc.c` | `lib/libc/time/lib_calendar2utc.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_daysbeforemonth.c` | `lib/libc/time/lib_daysbeforemonth.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_difftime.c` | `lib/libc/time/lib_difftime.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_gmtime.c` | `lib/libc/time/lib_gmtime.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_gmtimer.c` | `lib/libc/time/lib_gmtimer.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_isleapyear.c` | `lib/libc/time/lib_isleapyear.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_localtime.c` | `lib/libc/time/lib_localtime.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_strftime.c` | `lib/libc/time/lib_strftime.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_strptime.c` | `lib/libc/time/lib_strptime.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/time/lib_time.c` | `lib/libc/time/lib_time.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/Make.defs` | `lib/libc/unistd/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_access.c` | `lib/libc/unistd/lib_access.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_chdir.c` | `lib/libc/unistd/lib_chdir.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_execl.c` | `lib/libc/unistd/lib_execl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_getcwd.c` | `lib/libc/unistd/lib_getcwd.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_getopt.c` | `lib/libc/unistd/lib_getopt.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_getoptargp.c` | `lib/libc/unistd/lib_getoptargp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_getoptindp.c` | `lib/libc/unistd/lib_getoptindp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_getoptoptp.c` | `lib/libc/unistd/lib_getoptoptp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_sleep.c` | `lib/libc/unistd/lib_sleep.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/unistd/lib_swab.c` | `lib/libc/unistd/lib_swab.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/unistd/lib_truncate.c` | `lib/libc/unistd/lib_truncate.c` | 2020 | 2026 | 2020 | 2016 |
| `os/libs/libc/unistd/lib_usleep.c` | `lib/libc/unistd/lib_usleep.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libc/wchar/Make.defs` | `lib/libc/wchar/Make.defs` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_btowc.c` | `lib/libc/wchar/lib_btowc.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_mbrlen.c` | `lib/libc/wchar/lib_mbrlen.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_mbrtowc.c` | `lib/libc/wchar/lib_mbrtowc.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_mbsnrtowcs.c` | `lib/libc/wchar/lib_mbsnrtowcs.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_mbsrtowcs.c` | `lib/libc/wchar/lib_mbsrtowcs.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_swprintf.c` | `lib/libc/wchar/lib_swprintf.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcrtomb.c` | `lib/libc/wchar/lib_wcrtomb.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcscmp.c` | `lib/libc/wchar/lib_wcscmp.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcscoll.c` | `lib/libc/wchar/lib_wcscoll.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcsftime.c` | `lib/libc/wchar/lib_wcsftime.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcslcpy.c` | `lib/libc/wchar/lib_wcslcpy.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcslen.c` | `lib/libc/wchar/lib_wcslen.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcsnrtombs.c` | `lib/libc/wchar/lib_wcsnrtombs.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstod.c` | `lib/libc/wchar/lib_wcstod.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstof.c` | `lib/libc/wchar/lib_wcstof.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstol.c` | `lib/libc/wchar/lib_wcstol.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstold.c` | `lib/libc/wchar/lib_wcstold.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstoll.c` | `lib/libc/wchar/lib_wcstoll.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstoul.c` | `lib/libc/wchar/lib_wcstoul.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcstoull.c` | `lib/libc/wchar/lib_wcstoull.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wcsxfrm.c` | `lib/libc/wchar/lib_wcsxfrm.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wctob.c` | `lib/libc/wchar/lib_wctob.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wmemchr.c` | `lib/libc/wchar/lib_wmemchr.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wmemcmp.c` | `lib/libc/wchar/lib_wmemcmp.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wmemcpy.c` | `lib/libc/wchar/lib_wmemcpy.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wmemmove.c` | `lib/libc/wchar/lib_wmemmove.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wchar/lib_wmemset.c` | `lib/libc/wchar/lib_wmemset.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wctype/Make.defs` | `lib/libc/wctype/Make.defs` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libc/wctype/lib_iswctype.c` | `lib/libc/wctype/lib_iswctype.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libc/wctype/lib_towlower.c` | `lib/libc/wctype/lib_towlower.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libc/wctype/lib_towupper.c` | `lib/libc/wctype/lib_towupper.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libc/wctype/lib_wctype.c` | `lib/libc/wctype/lib_wctype.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libc/zoneinfo/Makefile` | `lib/libc/zoneinfo/Makefile` | 2020 | 2026 | 2020 | 2020 |
| `os/libs/libm/libm/Make.defs` | `lib/libc/math/Make.defs` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/__cos.c` | `lib/libc/math/__cos.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libm/libm/__sin.c` | `lib/libc/math/__sin.c` | 2018 | 2026 | 2018 | 2016 |
| `os/libs/libm/libm/lib_acos.c` | `lib/libc/math/lib_acos.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_acosf.c` | `lib/libc/math/lib_acosf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_acosh.c` | `lib/libc/math/lib_acosh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_acoshf.c` | `lib/libc/math/lib_acoshf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_acoshl.c` | `lib/libc/math/lib_acoshl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_acosl.c` | `lib/libc/math/lib_acosl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asin.c` | `lib/libc/math/lib_asin.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asinf.c` | `lib/libc/math/lib_asinf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asinh.c` | `lib/libc/math/lib_asinh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asinhf.c` | `lib/libc/math/lib_asinhf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asinhl.c` | `lib/libc/math/lib_asinhl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_asinl.c` | `lib/libc/math/lib_asinl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atan.c` | `lib/libc/math/lib_atan.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atan2.c` | `lib/libc/math/lib_atan2.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atan2f.c` | `lib/libc/math/lib_atan2f.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atan2l.c` | `lib/libc/math/lib_atan2l.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atanf.c` | `lib/libc/math/lib_atanf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atanh.c` | `lib/libc/math/lib_atanh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atanhf.c` | `lib/libc/math/lib_atanhf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atanhl.c` | `lib/libc/math/lib_atanhl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_atanl.c` | `lib/libc/math/lib_atanl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_cbrt.c` | `lib/libc/math/lib_cbrt.c` | 2017 | 2026 | 2017 | 2017 |
| `os/libs/libm/libm/lib_ceil.c` | `lib/libc/math/lib_ceil.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_ceilf.c` | `lib/libc/math/lib_ceilf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_ceill.c` | `lib/libc/math/lib_ceill.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_copysign.c` | `lib/libc/math/lib_copysign.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_copysignf.c` | `lib/libc/math/lib_copysignf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_copysignl.c` | `lib/libc/math/lib_copysignl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_cos.c` | `lib/libc/math/lib_cos.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_cosf.c` | `lib/libc/math/lib_cosf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_cosh.c` | `lib/libc/math/lib_cosh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_coshf.c` | `lib/libc/math/lib_coshf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_coshl.c` | `lib/libc/math/lib_coshl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_cosl.c` | `lib/libc/math/lib_cosl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_erf.c` | `lib/libc/math/lib_erf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_erfc.c` | `lib/libc/math/lib_erfc.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_erfcf.c` | `lib/libc/math/lib_erfcf.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_erfcl.c` | `lib/libc/math/lib_erfcl.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_erff.c` | `lib/libc/math/lib_erff.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_erfl.c` | `lib/libc/math/lib_erfl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_exp.c` | `lib/libc/math/lib_exp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_expf.c` | `lib/libc/math/lib_expf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_expl.c` | `lib/libc/math/lib_expl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_expm1.c` | `lib/libc/math/lib_expm1.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_expm1f.c` | `lib/libc/math/lib_expm1f.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_expm1l.c` | `lib/libc/math/lib_expm1l.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_fabs.c` | `lib/libc/math/lib_fabs.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_fabsf.c` | `lib/libc/math/lib_fabsf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_fabsl.c` | `lib/libc/math/lib_fabsl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_floor.c` | `lib/libc/math/lib_floor.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_floorf.c` | `lib/libc/math/lib_floorf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_floorl.c` | `lib/libc/math/lib_floorl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_fmax.c` | `lib/libc/math/lib_fmax.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fmaxf.c` | `lib/libc/math/lib_fmaxf.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fmaxl.c` | `lib/libc/math/lib_fmaxl.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fmin.c` | `lib/libc/math/lib_fmin.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fminf.c` | `lib/libc/math/lib_fminf.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fminl.c` | `lib/libc/math/lib_fminl.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_fmod.c` | `lib/libc/math/lib_fmod.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_fmodf.c` | `lib/libc/math/lib_fmodf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_fmodl.c` | `lib/libc/math/lib_fmodl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_frexp.c` | `lib/libc/math/lib_frexp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_frexpf.c` | `lib/libc/math/lib_frexpf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_frexpl.c` | `lib/libc/math/lib_frexpl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_gamma.c` | `lib/libc/math/lib_gamma.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libm/libm/lib_ldexp.c` | `lib/libc/math/lib_ldexp.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_ldexpf.c` | `lib/libc/math/lib_ldexpf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_ldexpl.c` | `lib/libc/math/lib_ldexpl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_lgamma.c` | `lib/libc/math/lib_lgamma.c` | 2018 | 2026 | 2018 | 2018 |
| `os/libs/libm/libm/lib_libexpi.c` | `lib/libc/math/lib_libexpi.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_libsqrtapprox.c` | `lib/libc/math/lib_libsqrtapprox.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_llround.c` | `lib/libc/math/lib_llround.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_llroundf.c` | `lib/libc/math/lib_llroundf.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_llroundl.c` | `lib/libc/math/lib_llroundl.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_log.c` | `lib/libc/math/lib_log.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log10.c` | `lib/libc/math/lib_log10.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log10f.c` | `lib/libc/math/lib_log10f.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log10l.c` | `lib/libc/math/lib_log10l.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log2.c` | `lib/libc/math/lib_log2.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log2f.c` | `lib/libc/math/lib_log2f.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_log2l.c` | `lib/libc/math/lib_log2l.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_logf.c` | `lib/libc/math/lib_logf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_logl.c` | `lib/libc/math/lib_logl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_lround.c` | `lib/libc/math/lib_lround.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_lroundf.c` | `lib/libc/math/lib_lroundf.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_lroundl.c` | `lib/libc/math/lib_lroundl.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_modf.c` | `lib/libc/math/lib_modf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_modff.c` | `lib/libc/math/lib_modff.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_modfl.c` | `lib/libc/math/lib_modfl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_nan.c` | `lib/libc/math/lib_nan.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_nanf.c` | `lib/libc/math/lib_nanf.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_nanl.c` | `lib/libc/math/lib_nanl.c` | 2023 | 2026 | 2023 | 2023 |
| `os/libs/libm/libm/lib_pow.c` | `lib/libc/math/lib_pow.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_powf.c` | `lib/libc/math/lib_powf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_powl.c` | `lib/libc/math/lib_powl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_rint.c` | `lib/libc/math/lib_rint.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_rintf.c` | `lib/libc/math/lib_rintf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_rintl.c` | `lib/libc/math/lib_rintl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_round.c` | `lib/libc/math/lib_round.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_roundf.c` | `lib/libc/math/lib_roundf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_roundl.c` | `lib/libc/math/lib_roundl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_scalbn.c` | `lib/libc/math/lib_scalbn.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_scalbnf.c` | `lib/libc/math/lib_scalbnf.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_scalbnl.c` | `lib/libc/math/lib_scalbnl.c` | 2017 | 2026 | 2017 | 2016 |
| `os/libs/libm/libm/lib_sin.c` | `lib/libc/math/lib_sin.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sinf.c` | `lib/libc/math/lib_sinf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sinh.c` | `lib/libc/math/lib_sinh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sinhf.c` | `lib/libc/math/lib_sinhf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sinhl.c` | `lib/libc/math/lib_sinhl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sinl.c` | `lib/libc/math/lib_sinl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sqrt.c` | `lib/libc/math/lib_sqrt.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sqrtf.c` | `lib/libc/math/lib_sqrtf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_sqrtl.c` | `lib/libc/math/lib_sqrtl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tan.c` | `lib/libc/math/lib_tan.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tanf.c` | `lib/libc/math/lib_tanf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tanh.c` | `lib/libc/math/lib_tanh.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tanhf.c` | `lib/libc/math/lib_tanhf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tanhl.c` | `lib/libc/math/lib_tanhl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_tanl.c` | `lib/libc/math/lib_tanl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_trunc.c` | `lib/libc/math/lib_trunc.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_truncf.c` | `lib/libc/math/lib_truncf.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/lib_truncl.c` | `lib/libc/math/lib_truncl.c` | 2016 | 2026 | 2016 | 2016 |
| `os/libs/libm/libm/libm.h` | `lib/libc/math/libm.h` | 2017 | 2026 | 2017 | 2017 |
| `os/libs/libxx/Makefile` | `lib/libxx/Makefile` | 2016 | 2026 | 2016 | 2016 |

</details>

<details><summary>readded (27)</summary>

| 파일 | 헤더 | 툴 | master에서 마지막 삭제 |
|---|---|---|---|
| `os/arch/arm/src/armv7-a/arm_addrenv_shm.c` | 2026 | 2023 | f3f45fdb3 2026-03-18 |
| `os/arch/arm/src/armv7-a/arm_cpustart.c` | 2026 | 2023 | ff8a142cd 2025-11-24 |
| `os/audio/pcm_decode.c` | 2026 | 2017 | c8b54a141 2018-03-21 |
| `os/drivers/usbhost/hid_parser.c` | 2026 | 2016 | 3139923bf 2017-08-07 |
| `os/drivers/usbhost/usbhost_hidkbd.c` | 2026 | 2016 | 3139923bf 2017-08-07 |
| `os/drivers/usbhost/usbhost_hidmouse.c` | 2026 | 2016 | 3139923bf 2017-08-07 |
| `os/fs/mount/fs_automount.c` | 2026 | 2016 | 70e974271 2017-08-07 |
| `os/fs/vfs/fs_sendfile.c` | 2026 | 2016 | d80337aee 2017-03-13 |
| `os/include/spawn.h` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/include/sys/shm.h` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/include/tinyara/audio/pcm.h` | 2026 | 2017 | c8b54a141 2018-03-21 |
| `os/include/tinyara/can.h` | 2026 | 2016 | e2ba99244 2026-05-28 |
| `os/include/tinyara/sensors/sensor.h` | 2026 | 2017 | 2a178beb0 2017-07-24 |
| `os/include/tinyara/spawn.h` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/include/tinyara/usb/hid.h` | 2026 | 2016 | 3139923bf 2017-08-07 |
| `os/include/tinyara/usb/hid_parser.h` | 2026 | 2016 | 3139923bf 2017-08-07 |
| `os/kernel/task/spawn.h` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/kernel/task/task_posixspawn.c` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/kernel/task/task_spawn.c` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/kernel/task/task_spawnparms.c` | 2026 | 2016 | 9ea8e1243 2018-06-18 |
| `os/mm/shm/Make.defs` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/shm/shm.h` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/shm/shmat.c` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/shm/shmctl.c` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/shm/shmdt.c` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/shm/shmget.c` | 2026 | 2016 | f3f45fdb3 2026-03-18 |
| `os/mm/umm_heap/umm_heap.h` | 2026 | 2019 | 9f6ac64f4 2020-05-15 |

</details>

<details><summary>history-rename+moved (20)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/libs/libc/pthread/pthread_barrierdestroy.c` | `lib/libc/pthread/pthread_barrierdestroy.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_barrierinit.c` | `lib/libc/pthread/pthread_barrierinit.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_barrierwait.c` | `lib/libc/pthread/pthread_barrierwait.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_conddestroy.c` | `lib/libc/pthread/pthread_conddestroy.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_condinit.c` | `lib/libc/pthread/pthread_condinit.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_once.c` | `lib/libc/pthread/pthread_once.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_setcancelstate.c` | `lib/libc/pthread/pthread_setcancelstate.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_setcanceltype.c` | `lib/libc/pthread/pthread_setcanceltype.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_testcancel.c` | `lib/libc/pthread/pthread_testcancel.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/pthread/pthread_yield.c` | `lib/libc/pthread/pthread_yield.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/sched/task_setcanceltype.c` | `lib/libc/sched/task_setcanceltype.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/sched/task_testcancel.c` | `lib/libc/sched/task_testcancel.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/time/lib_asctime.c` | `lib/libc/time/lib_asctime.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/time/lib_asctimer.c` | `lib/libc/time/lib_asctimer.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/time/lib_ctime.c` | `lib/libc/time/lib_ctime.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/time/lib_ctimer.c` | `lib/libc/time/lib_ctimer.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/time/lib_dayofweek.c` | `lib/libc/time/lib_dayofweek.c` | 2016 | 2026 | 2017 | 2016 |
| `os/libs/libc/wqueue/Make.defs` | `os/wqueue/uwqueue/Make.defs` | 2016 | 2026 | 2019 | 2016 |
| `os/libs/libc/wqueue/work_cancel.c` | `os/wqueue/work_cancel.c` | 2016 | 2026 | 2019 | 2016 |
| `os/libs/libc/wqueue/work_queue.c` | `os/wqueue/work_queue.c` | 2016 | 2026 | 2019 | 2016 |

</details>

<details><summary>master-mismatch (17)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/arch/arm/include/armv8-m/irq.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/include/elf.h` |  | 2019 | 2016 | 2016 | 2016 |
| `os/arch/arm/src/armv8-m/dwt.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/etm.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/exc_return.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/itm.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/itm_syslog.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/mpu.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/nvic.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/psr.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/ram_vectors.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/arch/arm/src/armv8-m/tpi.h` |  | 2019 | 2020 | 2020 | 2016 |
| `os/drivers/lcd/Make.defs` |  | 2019 | 2016 | 2016 | 2016 |
| `os/drivers/lcd/ili9341.c` |  | 2019 | 2016 | 2016 | 2016 |
| `os/include/elf32.h` |  | 2019 | 2016 | 2016 | 2016 |
| `os/include/tinyara/lcd/ili9341.h` |  | 2018 | 2016 | 2016 | 2016 |
| `os/kernel/task/task_cancelpt.c` |  | 2016 | 2017 | 2017 | 2017 |

</details>

<details><summary>moved+renamed (14)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/libs/libc/builtin/lib_builtin_getname.c` | `os/binfmt/libbuiltin/libbuiltin_getname.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/builtin/lib_builtin_isavail.c` | `os/binfmt/libbuiltin/libbuiltin_isavail.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf.h` | `os/binfmt/libelf/libelf.h` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/elf/elf_addrenv.c` | `os/binfmt/libelf/libelf_addrenv.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_bind.c` | `os/binfmt/libelf/libelf_bind.c` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/elf/elf_init.c` | `os/binfmt/libelf/libelf_init.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_iobuffer.c` | `os/binfmt/libelf/libelf_iobuffer.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_load.c` | `os/binfmt/libelf/libelf_load.c` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/elf/elf_read.c` | `os/binfmt/libelf/libelf_read.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_sections.c` | `os/binfmt/libelf/libelf_sections.c` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/elf/elf_symbols.c` | `os/binfmt/libelf/libelf_symbols.c` | 2019 | 2026 | 2019 | 2019 |
| `os/libs/libc/elf/elf_uninit.c` | `os/binfmt/libelf/libelf_uninit.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_unload.c` | `os/binfmt/libelf/libelf_unload.c` | 2019 | 2026 | 2019 | 2016 |
| `os/libs/libc/elf/elf_verify.c` | `os/binfmt/libelf/libelf_verify.c` | 2019 | 2026 | 2019 | 2016 |

</details>

<details><summary>history-rename (5)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/include/sys/uio.h` |  | 2016 | 2018 | 2018 | 2016 |
| `os/include/tinyara/cancelpt.h` |  | 2016 | 2017 | 2017 | 2016 |
| `os/kernel/mqueue/mq_getattr.c` |  | 2016 | 2019 | 2019 | 2016 |
| `os/kernel/mqueue/mq_setattr.c` |  | 2016 | 2019 | 2019 | 2016 |
| `os/kernel/semaphore/sem_tickwait.c` |  | 2016 | 2017 | 2017 | 2016 |

</details>

<details><summary>master-mismatch+moved (2)</summary>

| 파일 | 이전 경로 | 헤더 | 툴 | master 툴 | follow |
|---|---|---|---|---|---|
| `os/drivers/timers/rtc.c` | `os/drivers/rtc.c` | 2017 | 2026 | 2016 | 2016 |
| `os/libs/libc/uio/Make.defs` | `lib/libc/uio/Make.defs` | 2018 | 2026 | 2017 | 2016 |

</details>

## 참고: 그대로 둔 파일

| 사유 | 파일 수 |
|---|---|
| 신규 Kconfig (라이선스 없음) | 88 |
| 이미 삼성 헤더 있음 | 59 |
| TizenRT에 있던 파일, 삼성 헤더 없음, Kconfig (라이선스 없음) | 48 |
| 신규 .patch (라이선스 없음) | 32 |
| 신규 .gitignore (라이선스 없음) | 19 |
| TizenRT에 있던 파일, 삼성 헤더 없음, .gitignore (라이선스 없음) | 9 |
| 신규 defconfig (라이선스 관련 문구 있음) | 4 |
| 신규 .png (라이선스 없음) | 4 |
| 신규 .md (라이선스 없음) | 3 |
| 신규 .txt (라이선스 없음) | 2 |
| TizenRT에 있던 파일, 삼성 헤더 없음, .csv (라이선스 없음) | 2 |
| 신규 .patch (GTDGmbH) | 2 |
| TizenRT에 있던 파일, 삼성 헤더 없음, Kconfig (라이선스 관련 문구 있음) | 1 |
| 신규 Kconfig (라이선스 관련 문구 있음) | 1 |
| 신규 .md (라이선스 관련 문구 있음) | 1 |
| 신규 TECH_SPEC (라이선스 없음) | 1 |
| 신규 TODO (라이선스 없음) | 1 |
| 심볼릭 링크 | 1 |
| 신규 .csv (라이선스 없음) | 1 |
| 신규 Dockerfile (라이선스 없음) | 1 |

- 이미 삼성 헤더 있음: TizenRT 빌드 파일과 이번에 새로 쓴 TizenRT 파일(qemu_run.sh 등).
- "라이선스 관련 문구 있음"은 라이선스 헤더가 아니라 본문에 license 같은 단어가 있는 파일입니다(os/Kconfig의 License Setup 메뉴, defconfig의 옵션 이름 등).
- `.csv`는 주석 문법이 없어 헤더를 넣지 않았습니다(syscall.csv 등은 빌드 도구가 그대로 읽음).
- `.patch`, `.md`, `.txt`, `.png`, defconfig, Dockerfile, 심볼릭 링크는 대상이 아닙니다.
