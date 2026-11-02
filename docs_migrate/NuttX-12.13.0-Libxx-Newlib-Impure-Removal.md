# libxx_newlib_impure.c 제거 분석 (NuttX 12.13.0 마이그레이션)

| 항목 | 내용 |
|---|---|
| 작성일 | 2026-11-02 |
| 대상 파일 | `os/libs/libxx/libxx_newlib_impure.c` (제거됨, 같은 커밋에서 `libxx_cxa_guard.hxx`도 제거) |
| 원본 추가 커밋 | `a41a69b9ca` "libcxx : add libsupc++ abi support" (2018-02-06, Manohara HK) |
| 제거 커밋 | `os/libs/libxx: Remove the files left over after the NuttX 12.13.0 sync [DEL_TR]` |
| 함께 볼 문서 | [NuttX-12.13.0-Leftover-Files-Review.md](NuttX-12.13.0-Leftover-Files-Review.md) (잔존 파일 리뷰 표), [NuttX-12.13.0-Migration.md](NuttX-12.13.0-Migration.md), [NuttX-12.13.0-Migration-Report.md](NuttX-12.13.0-Migration-Report.md) |

## 1. 요약

`libxx_newlib_impure.c`는 2018년에 toolchain의 `libsupc++.a`를 링크하기 위한
임시 우회책(dummy `_impure_ptr`)으로 추가되었으나, NuttX 12.13.0 마이그레이션
이후 빌드에 전혀 포함되지 않는 죽은 파일(dead code)이 되었습니다.
그 기능은 NuttX 12.13.0에서 싱크된 `os/libs/libc/misc/lib_impure.c`가 정식
구현으로 대체하고 있으므로 제거해도 안전하며, 유지 시 오히려 심볼 중복
충돌 위험만 있습니다.

## 2. 원본 커밋 분석 (a41a69b9ca, 2018)

당시 TizenRT는 자체 libc를 사용하므로 toolchain의 newlib `libc.a`에 있는
`_impure_ptr` 심볼이 존재하지 않았습니다. 이 때문에 toolchain의
`libsupc++.a`를 libc++ ABI로 링크할 때 아래와 같은 링크 에러가 발생했습니다.

```
.../lib/libsupc++.a(vterminate.o): In function `__gnu_cxx::__verbose_terminate_handler()':
vterminate.cc:(....): undefined reference to `_impure_ptr'
```

이를 해결하기 위해 커밋 `a41a69b9ca`가 3개 파일을 수정했습니다.

| 파일 | 변경 내용 | 12.13.0 마이그레이션 후 상태 |
|---|---|---|
| `external/libcxx/new.cxx` | `bad_array_new_length`, `bad_array_length`를 `#ifndef __GLIBCXX__` 가드로 감싸 libsupc++와의 심볼 중복 방지 | **유지됨** (여전히 유효) |
| `lib/libxx/Makefile` | `ifeq ($(CONFIG_LIBCXX_HAVE_LIBSUPCXX),y)` 조건으로 `CSRCS += libxx_newlib_impure.c` 추가 (빌드 연결) | **삭제됨** — 12.13.0 빌드 등록 커밋에서 Makefile이 NuttX 소스 목록 기준으로 바뀌며 제거 |
| `lib/libxx/libxx_newlib_impure.c` | dummy `void *_impure_ptr;` 정의 (신규 파일) | **파일만 남음** — 빌드 참조가 끊어진 고립 파일 |

## 3. 제거 근거

### 3.1 빌드에 전혀 포함되지 않음

전체 코드베이스에서 `libxx_newlib_impure.c`를 참조하는 Makefile/Make.defs가
없습니다. 빌드 등록 커밋 `9867a7cf9`("os/libs/libxx: Register the NuttX 12.13.0
sources in the TizenRT build")가 `os/libs/libxx/Makefile`에서 해당 블록을
제거했습니다.

```diff
-# Support newlib impure pointer
-ifeq ($(CONFIG_LIBCXX_HAVE_LIBSUPCXX),y)
-CSRCS += libxx_newlib_impure.c
+ifeq ($(CONFIG_ETL),y)
+include etl/Make.defs
 endif
```

커밋 재구성(261028) 전 원래 싱크 커밋 `4a754acbfe`("os/libs/libxx: Sync with
NuttX 12.13.0", `agents/261026_nuttx_12.13.0` 브랜치)의 메시지에도 이 파일이
"빌드되지 않는 Samsung 로컬 파일"로 기록되어 있습니다:

> Kept (Samsung, not built by the NuttX makefiles): libxx_cxa_guard.hxx,
> libxx_newlib_impure.c.

### 3.2 조건이었던 config 심볼이 소멸함

빌드 조건이었던 `CONFIG_LIBCXX_HAVE_LIBSUPCXX`가 현재 트리의 어떤
Kconfig에도 존재하지 않습니다 (레거시 `build/configs/*/Make.defs`에만
잔존). 현재 libsupc++ 관련 선택지는 `os/libs/libxx/Kconfig`의
`LIBMINIABI` / `LIBCXXABI` / `LIBSUPCXX` / `LIBSUPCXX_TOOLCHAIN`으로
대체되었습니다.

### 3.3 기능이 libc의 정식 구현으로 대체됨

NuttX 12.13.0에서 싱크된 `os/libs/libc/misc/lib_impure.c`가
`os/libs/libc/misc/Make.defs`에서 조건 없이 항상 컴파일되며, dummy가 아닌
정식 타입의 `_impure_ptr`을 정의합니다.

```c
static struct _reent __ATTRIBUTE_IMPURE_DATA__
impure_data = _REENT_INIT(impure_data);

struct _reent *__ATTRIBUTE_IMPURE_PTR__
_impure_ptr = &impure_data;

struct _reent *__ATTRIBUTE_IMPURE_PTR__
_global_impure_ptr = &impure_data;
```

- `__has_include(<reent.h>)` 가드로 newlib 툴체인에서만 활성화되므로
  newlib이 아닌 환경에서도 안전합니다.
- toolchain의 `libsupc++.a`를 사용하는 현재 경로(`CONFIG_LIBSUPCXX_TOOLCHAIN`,
  `os/arch/arm/src/common/Toolchain.defs`)에서 필요한 `_impure_ptr` 심볼을
  libc가 제공하므로 원래 문제가 발생하지 않습니다.
- 즉, libxx가 아닌 libc 레벨에서 모든 외부 라이브러리에 대해 해결된
  상태입니다.

### 3.4 유지 시 충돌 위험

파일을 다시 빌드에 포함시키면 `void *` 타입 dummy(이 파일)와
`struct _reent *` 타입 정의(`lib_impure.c`)가 동시에 링크되어 duplicate
symbol 링크 에러가 발생합니다.

## 4. 검증

- 전체 트리 grep: `libxx_newlib_impure`를 참조하는 Makefile/Make.defs/Kconfig 없음 (삭제 전/후 모두 확인).
- 파일이 어떤 `CSRCS`에도 포함되지 않았으므로 삭제는 빌드 결과에 영향을 주지 않음.
- `_impure_ptr`은 `os/libs/libc/misc/lib_impure.c`가 제공하므로 toolchain
  라이브러리(libsupc++.a 등) 링크에 영향 없음.

## 5. 결론

`libxx_newlib_impure.c`는 2018년 `CONFIG_LIBCXX_HAVE_LIBSUPCXX` 구조에서만
유효한 우회책이었고, 12.13.0 마이그레이션에서 (1) 빌드 연결 제거,
(2) 의존 config 심볼 소멸, (3) upstream `lib_impure.c` 등장으로 세 겹으로
대체가 완료된 파일입니다. 어떤 빌드 시스템 수정 없이 파일만 제거했습니다.

위 인용에 함께 나온 `libxx_cxa_guard.hxx`도 같은 커밋에서 제거했습니다. 이 헤더를
include하던 구 `libxx_cxa_guard.cxx`, `libxx_cxa_atexit.cxx`가 NuttX
`libminiabi/` 소스로 바뀌었고, `libminiabi/libxx_cxa_guard.cxx`가 `__guard`
타입을 직접 정의하므로 include하는 곳이 없습니다. 근거는
[NuttX-12.13.0-Leftover-Files-Review.md](NuttX-12.13.0-Leftover-Files-Review.md)의
os/libs 표에 있습니다.
