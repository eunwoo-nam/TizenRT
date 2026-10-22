# How to use the formatter

This document explains how to format C sources with the coding rules of
[CodingStyleGuide](../docs/CodingStyleGuide.md).

## Prerequisites

The C formatter requires **python3 only** (standard library, no pip
packages). There is no longer any dependency on GNU `indent`, `astyle`
or `perl`.

The C++ formatter (`cxxformatter.sh`) still requires `clang-format`.

## C formatter

### Usage

```bash
./tools/cformatter.sh os/kernel/sched/sched_addreadytorun.c   # one file
./tools/cformatter.sh os/kernel                               # a directory (recursive)
./tools/cformatter.sh --check os/kernel                        # report only (CI-friendly, exit 1 on violations)
./tools/cformatter.sh --diff os/kernel                         # print a unified diff, do not write
./tools/cformatter.sh --backup os/kernel                        # keep a *.orig copy before rewriting
./tools/cformatter.sh -v os/kernel                             # verbose per-file status
```

`cformatter.sh` is a thin wrapper; the implementation lives in
`tools/cformatter.py` and can be invoked directly:

```bash
python3 tools/cformatter.py [--check] [--diff] [--backup] [-v] TARGET...
```

### Applied rules

The formatter rewrites only whitespace between tokens. It applies the
mechanical rules of the C coding style guide:

| Rule | Description |
| ---- | ----------- |
| [M01] | tab-only indentation, tab width 4 |
| [M02] | `case`/`default` at the same indent level as `switch` |
| [M03] | keyword spacing (`if (`, `sizeof(`) |
| [M04] | operator spacing, no space after casts, right-bound `*` declarators |
| [M05] | separator spacing (`,`, `;`, parentheses, brackets) |
| [M06] | function opening brace starts the next line |
| [M07] | control opening brace at end of line, closing brace on its own line, `} else` / `} while` attached |
| [M08] | `struct`/`enum`/`union` opening brace on the same line |
| [R03] | goto labels at column 0 |
| [R05] | no trailing whitespace |
| [R06] | newline at end of file |
| [R09] | `#` of preprocessor directives at the first column |

### Safety guarantees

Before a file is rewritten, the formatter verifies that:

1. the token stream (identifiers, literals, comments, operators) is
   unchanged, so the code semantics cannot be altered;
2. formatting is idempotent (running it twice produces no further change).

If either check fails the file is left untouched and an error is
reported. Regions guarded by `__ASSEMBLY__`/`__ASSEMBLER__` keep their
original line structure, since assembly must not be reformatted as C.

### Limitations (rules not automated)

Rules that need semantic understanding are intentionally out of scope:
comment style [R01], code organization [R07], header split [M09],
`inline` placement [R02], prototype parameter names [M11], `do-while`
wrapping of macros [R08], macro hygiene [R09]/[R10], tag/variable split
[R11], one declaration per line [R12], naming [M12]/[R13]-[R16].
Please fix those manually.

Statement sequences are always joined onto single lines (there is no
line-length limit in the C rules), and initializer lists keep the
author's original line breaks.
