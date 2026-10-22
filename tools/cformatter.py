#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cformatter.py -- Style formatter for TizenRT C sources.

Automatically applies the mechanical C rules of
docs/CodingStyleGuide.md:

  [M01] tab-only indentation (tab width 4)
  [M02] 'case'/'default' at the same indent level as 'switch'
  [R03] goto labels start at column 0
  [M03] keyword spacing rules
  [M04] operator spacing rules (no space after casts, right-bound '*')
  [M05] separator spacing rules
  [M06] function opening brace starts the next line
  [M07] control opening brace stays at the end of the line; the
         closing brace stands alone (exceptions: '} else', '} while')
  [M08] struct/enum/union opening brace stays on the same line
  [R05] no trailing whitespace
  [R06] file ends with a newline
  [R09] '#' of a preprocessor directive at the first column

Rules that need semantic understanding are intentionally NOT
automated: [R01], [R07], [M09], [M11], [R08]-[R12], [M12],
[R13]-[R16].

The formatter only rewrites whitespace between tokens.  Before any
output is written, a safety check verifies that the token stream
(identifiers, literals, comments, operators) is unchanged and that
formatting is idempotent.

Usage:
    python3 cformatter.py [--check] [--diff] [--backup] [-v] TARGET...

TARGET is a .c/.h file or a directory (searched recursively).
"""

import argparse
import difflib
import os
import re
import sys

# --------------------------------------------------------------------------
# Rule tables (docs/CodingStyleGuide.md, C section)

# [M03] keywords that take a space after them
KEYWORDS_SPACE_AFTER = {
    'if', 'else', 'for', 'while', 'switch', 'case', 'default', 'do',
    'return', 'goto', 'break', 'continue', 'typedef', 'extern',
    'static', 'register', 'volatile', 'const', 'inline', 'auto',
}

# [M03] function-like keywords: no space after
KEYWORDS_NO_SPACE_AFTER = {
    'sizeof', 'typeof', 'alignof', '_Alignof', '__attribute__',
}

TYPE_KEYWORDS = {
    'void', 'char', 'short', 'int', 'long', 'float', 'double',
    'signed', 'unsigned', 'struct', 'union', 'enum', 'const',
    'volatile', 'register', 'extern', 'static', 'inline', 'auto',
    'restrict', '_Bool', '_Atomic', 'FAR', 'typedef',
}

BINARY_OPS = {
    '=', '+', '-', '*', '/', '%', '|', '&', '^', '<', '>',
    '<<', '>>', '<=', '>=', '==', '!=', '&&', '||',
    '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=',
}

OP_TOKENS = [
    '<<=', '>>=', '...', '->', '++', '--', '<<', '>>', '<=', '>=',
    '==', '!=', '&&', '||', '+=', '-=', '*=', '/=', '%=', '&=',
    '|=', '^=', '=', '+', '-', '*', '/', '%', '&', '|', '^', '~',
    '!', '<', '>', '.', '?', ':', ',', ';', '(', ')', '[', ']',
    '{', '}',
]

C_SUFFIX = ('.c', '.h')


class Tok(object):
    __slots__ = ('kind', 'text', 'nl_before')

    def __init__(self, kind, text, nl_before=0):
        self.kind = kind
        self.text = text
        self.nl_before = nl_before

    def __repr__(self):
        return 'Tok(%s, %r)' % (self.kind, self.text[:40])


_IDENT_RE = re.compile(r'[A-Za-z_]\w*')
_NUM_RE = re.compile(r'\.?\d[\w.]*')


def tokenize(text):
    """Split *text* into tokens.  Whitespace is dropped; every token
    remembers how many newlines preceded it in the original source.
    Preprocessor directives (including backslash continuations) are
    kept as one verbatim token ([R09], untouched content)."""
    toks = []
    i, n = 0, len(text)
    state = {'nl': 0, 'line_start': True}

    def emit(kind, txt):
        toks.append(Tok(kind, txt, state['nl']))
        state['nl'] = 0

    while i < n:
        c = text[i]
        if c == '\n':
            state['nl'] += 1
            state['line_start'] = True
            i += 1
            continue
        if c in ' \t\r':
            i += 1
            continue
        if c == '\\' and i + 1 < n and text[i + 1] == '\n':
            # line splice outside a directive: plain whitespace
            i += 2
            continue
        if state['line_start'] and c == '#':
            j = i
            while True:
                k = text.find('\n', j)
                if k == -1:
                    j = n
                    break
                if k > 0 and text[k - 1] == '\\':
                    j = k + 1
                else:
                    j = k
                    break
            emit('PREPROC', text[i:j].rstrip(' \t\r'))
            state['line_start'] = False
            i = j
            continue
        if text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j == -1 else j + 2
            emit('COMMENT', text[i:j])
            state['nl'] = 0
            state['line_start'] = False
            i = j
            continue
        if text.startswith('//', i):
            j = text.find('\n', i)
            j = n if j == -1 else j
            emit('COMMENT', text[i:j].rstrip(' \t\r'))
            state['line_start'] = False
            i = j
            continue
        if c == '"' or c == "'":
            j = i + 1
            while j < n and text[j] != c and text[j] != '\n':
                if text[j] == '\\':
                    j += 1
                j += 1
            if j < n and text[j] == c:
                j += 1
            emit('STR' if c == '"' else 'CHAR', text[i:j])
            state['line_start'] = False
            i = j
            continue
        m = _IDENT_RE.match(text, i)
        if m:
            emit('IDENT', m.group(0))
            state['line_start'] = False
            i = m.end()
            continue
        m = _NUM_RE.match(text, i)
        if m:
            emit('NUM', m.group(0))
            state['line_start'] = False
            i = m.end()
            continue
        for op in OP_TOKENS:
            if text.startswith(op, i):
                emit('OP', op)
                state['line_start'] = False
                i += len(op)
                break
        else:
            emit('OP', c)
            state['line_start'] = False
            i += 1
    return toks


# --------------------------------------------------------------------------
# Analysis passes


def match_pairs(toks, opening, closing):
    """Map index of each unmatched *opening* to its matching *closing*."""
    stack, match = [], {}
    for idx, t in enumerate(toks):
        if t.kind == 'OP':
            if t.text == opening:
                stack.append(idx)
            elif t.text == closing and stack:
                match[stack.pop()] = idx
    return match


def sig_back(toks, i):
    """Index of the nearest significant token before position *i*,
    skipping comments, preprocessor tokens and balanced '(...)'
    groups (which makes '__attribute__((...))' transparent)."""
    j = i - 1
    while j >= 0:
        t = toks[j]
        if t.kind in ('COMMENT', 'PREPROC'):
            j -= 1
            continue
        if t.kind == 'IDENT' and t.text == '__attribute__':
            j -= 1
            continue
        if t.kind == 'OP' and t.text == ')':
            depth = 0
            while j >= 0:
                tt = toks[j]
                if tt.kind == 'OP':
                    if tt.text == ')':
                        depth += 1
                    elif tt.text == '(':
                        depth -= 1
                        if depth == 0:
                            break
                j -= 1
            j -= 1
            continue
        return j
    return -1


def prev_sig(toks, i):
    """Index of the nearest significant (non-comment) token before *i*."""
    return sig_back(toks, i)


def prev_tok(toks, i):
    """Index of the token directly before *i*, skipping comments only."""
    j = i - 1
    while j >= 0 and toks[j].kind == 'COMMENT':
        j -= 1
    return j


_ASM_GUARD_RE = re.compile(
    r'#\s*(?:ifdef\s+__ASSEMBL|if\s+.*__ASSEMBL)\w*__')


def asm_region_indices(toks):
    """Token indices inside '#ifdef __ASSEMBLY__'-style blocks.
    Assembly code must never be reformatted as C, so those regions
    keep their original line structure (only spacing is normalized)."""
    regions = set()
    n = len(toks)
    for i, t in enumerate(toks):
        if t.kind != 'PREPROC' or not _ASM_GUARD_RE.match(t.text):
            continue
        depth = 0
        j = i + 1
        while j < n:
            tt = toks[j]
            if tt.kind == 'PREPROC':
                txt = tt.text.lstrip()
                if txt.startswith(('#if', '#ifdef', '#ifndef')):
                    depth += 1
                elif txt.startswith('#endif'):
                    if depth == 0:
                        break
                    depth -= 1
                elif txt.startswith('#else') and depth == 0:
                    break
            regions.add(j)
            j += 1
    return regions


def classify_braces(toks, bmatch):
    """Classify every '{' token as one of:
       'func'  -- function body definition      -> [M06] next line
       'ctrl'  -- control statement block       -> [M07] end of line
       'type'  -- struct/enum/union definition  -> [M08] same line
       'init'  -- initializer / statement-expr  -> keep original breaks
       'block' -- anything else (treated like 'ctrl')
    """
    bclass = {}
    for idx, t in enumerate(toks):
        if not (t.kind == 'OP' and t.text == '{'):
            continue
        j = prev_tok(toks, idx)
        prev = toks[j] if j >= 0 else None
        if prev is None:
            bclass[idx] = 'block'
            continue
        if prev.kind == 'PREPROC':
            bclass[idx] = 'ctrl'
            continue
        if prev.kind == 'OP':
            if prev.text in ('=', '(', ',', '[', '{', '?', ':', '||',
                            '&&'):
                bclass[idx] = 'init'
            elif prev.text in (';', '}'):
                bclass[idx] = 'ctrl'
            elif prev.text == ')':
                k = sig_back(toks, idx)
                bt = toks[k] if k >= 0 else None
                if bt is None:
                    bclass[idx] = 'init'
                elif bt.kind != 'IDENT':
                    bclass[idx] = 'init'
                elif bt.text in ('struct', 'union', 'enum'):
                    bclass[idx] = 'type'
                elif bt.text in ('if', 'while', 'for', 'switch'):
                    bclass[idx] = 'ctrl'
                elif bt.text in KEYWORDS_NO_SPACE_AFTER:
                    bclass[idx] = 'ctrl'
                else:
                    bclass[idx] = 'func'
            else:
                bclass[idx] = 'init'
            continue
        if prev.kind == 'IDENT':
            if prev.text in ('do', 'else'):
                bclass[idx] = 'ctrl'
            elif prev.text in ('struct', 'union', 'enum'):
                bclass[idx] = 'type'
            elif idx >= 2 and toks[idx - 2].kind == 'IDENT' and \
                    toks[idx - 2].text in ('struct', 'union', 'enum'):
                bclass[idx] = 'type'
            elif prev.text in KEYWORDS_NO_SPACE_AFTER:
                bclass[idx] = 'ctrl'
            else:
                bclass[idx] = 'block'
            continue
        bclass[idx] = 'init'
    return bclass


def find_casts(toks, pmatch):
    """Set of ')' token indices that close a cast expression.
    [M04]: no space after a cast operator."""
    casts = set()
    for open_, close in pmatch.items():
        inner = [t for t in toks[open_ + 1:close] if t.kind != 'COMMENT']
        if not inner:
            continue
        if any(t.kind in ('NUM', 'STR', 'CHAR') for t in inner):
            continue
        if any(t.kind == 'OP' and t.text != '*' for t in inner):
            continue
        idents = [t for t in inner if t.kind == 'IDENT']
        if not idents:
            continue
        first = idents[0].text
        if not (first in TYPE_KEYWORDS or first.endswith('_t')
                or first.endswith('_s')):
            continue
        # token before '(' decides function call vs cast
        j = open_ - 1
        while j >= 0 and toks[j].kind == 'COMMENT':
            j -= 1
        if j < 0:
            casts.add(close)
            continue
        bt = toks[j]
        if bt.kind == 'IDENT':
            # plain identifier means a function call: foo(x)
            if bt.text in ('return', 'case', 'goto'):
                casts.add(close)
            continue
        if bt.kind == 'OP' and bt.text in (')', ']'):
            continue  # call through expression: (*fp)(x), arr[i](x)
        casts.add(close)
    return casts


def decl_pointer_indices(toks):
    """Indices of '*' tokens used in pointer declarations.
    [M04]: the '*' binds to the declarator (right-bound).
    Heuristic: the '*' follows a type keyword, a struct/union/enum
    tag, a typedef-looking name (*_t/*_s) or another declarator '*'."""
    decls = set()
    for idx, t in enumerate(toks):
        if not (t.kind == 'OP' and t.text == '*'):
            continue
        prev = toks[idx - 1] if idx > 0 else None
        if prev is None:
            continue
        if prev.kind == 'IDENT':
            if prev.text in TYPE_KEYWORDS:
                decls.add(idx)
            elif prev.text.endswith('_t') or prev.text.endswith('_s'):
                decls.add(idx)
            elif idx >= 2 and toks[idx - 2].kind == 'IDENT' and \
                    toks[idx - 2].text in ('struct', 'union', 'enum'):
                decls.add(idx)
        elif prev.kind == 'OP' and prev.text == '*' and (idx - 1) in decls:
            decls.add(idx)
    return decls


# --------------------------------------------------------------------------
# Whitespace rules ([M03] keywords, [M04] operators, [M05] separators)


def _is_unary_pos(toks, pi):
    """True when toks[pi] is used as a unary operator."""
    k = pi - 1
    while k >= 0 and toks[k].kind == 'COMMENT':
        k -= 1
    if k < 0:
        return True
    q = toks[k]
    if q.kind == 'IDENT':
        return q.text in KEYWORDS_SPACE_AFTER or \
            q.text in KEYWORDS_NO_SPACE_AFTER or \
            q.text in ('struct', 'union', 'enum')
    if q.kind in ('NUM', 'STR', 'CHAR'):
        return False
    if q.kind == 'OP':
        return q.text not in (')', ']', '}')
    return True


def base_gap(toks, pi, i, casts, decls):
    """Minimal separator ('none' or 'space') between the significant
    token *pi* and toks[i].  Newline decisions are made elsewhere."""
    if pi < 0:
        return 'space'
    p, t = toks[pi], toks[i]
    pt = p.text if p.kind == 'OP' else None
    tt = t.text if t.kind == 'OP' else None

    # ---- rules driven by the current token -----------------------------
    if tt == '{':
        return 'space'        # func braces forced to 'nl' later ([M06])
    if tt == '}':
        return 'none'
    if tt in (')', ']', ';', ',', '->'):
        return 'none'
    if tt == '.':
        return 'none'
    if tt == '[':
        if p.kind in ('IDENT', 'NUM', 'STR', 'CHAR'):
            return 'none'
        if pt in (')', ']'):
            return 'none'
        return 'space'
    if t.kind == 'NUM' and pt == '.':
        return 'space'               # never merge '. 0' into '.0'
    if tt == ':':
        return 'space'        # ternary / bitfield (labels handled later)
    if tt in ('++', '--'):
        if p.kind in ('IDENT', 'NUM', 'STR', 'CHAR'):
            return 'none'     # postfix
        return 'space'        # prefix: falls through to p-side rules
    if tt == '(':
        if p.kind == 'IDENT':
            if p.text in KEYWORDS_SPACE_AFTER:
                return 'space'          # [M03] if ( ...
            return 'none'               # [M05] call: name(
        if p.kind in ('NUM', 'STR', 'CHAR'):
            return 'space'
        if pt in (')', ']'):
            return 'none'
        # any other previous token: fall through to the p-side rules

    # ---- rules driven by the previous token ---------------------------
    if p.kind == 'IDENT':
        if p.text in KEYWORDS_NO_SPACE_AFTER:
            # [M03] function-like keywords: 'sizeof(' but never
            # merge two identifiers together
            return 'none' if tt == '(' else 'space'
        if p.text in KEYWORDS_SPACE_AFTER:
            return 'space'              # [M03] if / return / ...
        if tt in (')', ']', ';', '.', ',', '->', '++', '--', '(', '['):
            return 'none'
        return 'space'
    if p.kind in ('NUM', 'STR', 'CHAR'):
        if tt in (')', ']', ';', ',', '->', '++', '--'):
            return 'none'
        return 'space'
    if pt in ('*', '&', '+', '-'):
        # unary operators take no trailing space ([M04]);
        # binary ones are spaced on both sides
        if pt == '*' and pi in decls:
            return 'none'               # [M04] right-bound declarator '*'
        if pi - 1 in casts:
            return 'none'               # unary op on a cast result
        return 'none' if _is_unary_pos(toks, pi) else 'space'
    if pt == ',':
        return 'space'                  # [M05] space after comma
    if pt in ('(', '[', '!', '~', '.', '->'):
        return 'none'                   # [M05] nothing inside parens
    if pt in BINARY_OPS:
        return 'space'                  # [M04] binary operators
    if pt in ('?', ':'):
        return 'space'
    if pt == '{':
        return 'none'                  # first initializer element
    if pt == '}':
        if tt in (',', ';', '.', ')', ']', '->', '(', '[', '++', '--'):
            return 'none'
        return 'space'                 # [M05] '} else' / '} var'
    if pt == ']':
        if tt in ('(', '[', '.', '->', '++', '--'):
            return 'none'
        return 'space'
    if pt == ')':
        if pi in casts and (t.kind in ('IDENT', 'NUM', 'STR', 'CHAR')
                             or tt in ('&', '*', '-', '+', '~', '!',
                                       '++', '--')):
            return 'none'               # [M04] no space after cast
        if tt in (';', ',', '.', '->', ')', ']', '(', '[', '++', '--'):
            return 'none'
        return 'space'
    if pt == ';':
        if tt in (';', ')'):
            return 'none'               # 'for (;;)'
        return 'space'
    if pt in ('++', '--'):
        # prefix already returned 'none'; postfix needs a separator
        return 'space'
    return 'space'


# --------------------------------------------------------------------------
# Line-break decisions ([M02], [M06], [M07], [M08], [R03], [R09])


def compute_layout(toks, bclass, bmatch, pmatch, casts, decls):
    """Compute for every token index *i*:
       gap[i]   -- 'none' | 'space' | 'nl' separator before toks[i]
       blank    -- indices that keep a blank line before them
       col0     -- indices rendered at column 0 ([R03] labels, [R09] '#')
       case_head-- 'case'/'default' keyword indices ([M02])
    """
    n = len(toks)
    gap = ['space'] * n
    blank = set()
    col0 = set()
    case_head = set()
    label_head = set()

    # initializer interiors keep the original line breaks
    init_region = set()
    for open_, close in bmatch.items():
        if bclass.get(open_) == 'init':
            init_region.update(range(open_ + 1, close))
    init_close = {c: o for o, c in bmatch.items()
                  if bclass.get(o) == 'init'}

    # '}' indices closing a do-block, for '} while' attachment
    do_close = set()
    for idx, t in enumerate(toks):
        if t.kind == 'IDENT' and t.text == 'do':
            j = idx + 1
            while j < n and toks[j].kind == 'COMMENT':
                j += 1
            if j in bmatch:
                do_close.add(bmatch[j])

    # case/default labels and goto labels
    case_colon = set()
    for idx, t in enumerate(toks):
        if t.kind != 'IDENT':
            continue
        if t.text in ('case', 'default'):
            d, j = 0, idx + 1
            while j < n:
                tt = toks[j]
                if tt.kind == 'OP':
                    if tt.text == '(':
                        d += 1
                    elif tt.text == ')':
                        d -= 1
                    elif tt.text == ':' and d == 0:
                        case_head.add(idx)
                        case_colon.add(j)
                        break
                    elif tt.text in (';', '{', '}') and d == 0:
                        break
                j += 1
        elif idx + 1 < n and toks[idx + 1].kind == 'OP' \
                and toks[idx + 1].text == ':':
            j = prev_sig(toks, idx)
            pv = toks[j] if j >= 0 else None
            if pv is None or (pv.kind == 'OP'
                              and pv.text in (';', '{', '}', ':')) \
                    or pv.kind == 'PREPROC':
                k = idx + 2
                while k < n and toks[k].kind == 'COMMENT':
                    k += 1
                if k < n and toks[k].kind == 'IDENT':
                    label_head.add(idx)
    label_colon = {h + 1 for h in label_head}

    # switch bodies, for 'case' indentation ([M02])
    switch_brace = set()
    for idx in bclass:
        if bclass[idx] == 'ctrl':
            k = sig_back(toks, idx)
            if k >= 0 and toks[k].kind == 'IDENT' \
                    and toks[k].text == 'switch':
                switch_brace.add(idx)

    paren_depth = [0] * n
    d = 0
    for i, t in enumerate(toks):
        paren_depth[i] = d
        if t.kind == 'OP' and t.text == '(':
            d += 1
        elif t.kind == 'OP' and t.text == ')':
            d = max(0, d - 1)

    asm_region = asm_region_indices(toks)

    return _gap_loop(toks, gap, blank, col0, case_head, case_colon,
                     label_head, label_colon, init_region, init_close,
                     do_close, switch_brace, paren_depth, bmatch,
                     asm_region, bclass, casts, decls)


def _gap_loop(toks, gap, blank, col0, case_head, case_colon,
              label_head, label_colon, init_region, init_close,
              do_close, switch_brace, paren_depth, bmatch,
              asm_region, bclass, casts, decls):
    n = len(toks)
    for i, t in enumerate(toks):
        if i == 0:
            if t.kind == 'PREPROC':
                col0.add(i)
            continue

        pi = prev_tok(toks, i)
        im = toks[i - 1]

        # assembly guarded regions keep their line structure
        if i in asm_region:
            if im.kind == 'COMMENT' and im.text.startswith('//'):
                gap[i] = 'nl'
            elif t.nl_before >= 1:
                gap[i] = 'nl'
            else:
                gap[i] = base_gap(toks, pi, i, casts, decls)
            if gap[i] == 'nl' and t.nl_before >= 2:
                blank.add(i)
            continue
        preserve = (i in init_region or i in init_close) and \
            t.nl_before >= 1
        after_comment = im.kind == 'COMMENT'

        # ---- forced newlines ([M06], [M07], [M08], [R09]) ---------------
        force = False
        if t.kind == 'PREPROC':
            force = True
            col0.add(i)
        elif im.kind == 'PREPROC':
            force = True
        elif t.kind == 'OP' and t.text == '}':
            force = i not in init_close
        elif t.kind == 'OP' and t.text == '{' \
                and bclass.get(i) == 'func':
            force = True                       # [M06]
        elif pi >= 0 and toks[pi].kind == 'OP' and toks[pi].text == '{' \
                and bclass.get(pi) in ('func', 'ctrl', 'type', 'block'):
            force = True
        elif t.kind == 'IDENT' and i in case_head:
            force = True
        elif t.kind == 'IDENT' and i in label_head:
            force = True
            col0.add(i)

        # ';' ending a statement starts a new line
        if not force and pi >= 0 and toks[pi].kind == 'OP' \
                and toks[pi].text == ';' and paren_depth[pi] == 0 \
                and pi not in init_region:
            if not (t.kind == 'IDENT' and t.text in ('else', 'while')):
                force = True

        # a non-initializer '}' is followed by a new statement line
        # ([M07]), except the attached tokens '};' '},' ') else' ...
        if not force and pi >= 0 and toks[pi].kind == 'OP' \
                and toks[pi].text == '}' and pi not in init_close:
            if t.kind == 'IDENT':
                if t.text in ('else', 'while'):
                    pass        # '} else' / '} while' attach ([M07])
                else:
                    open_ = bmatch.get(pi)
                    if not (open_ is not None
                            and bclass.get(open_) == 'type'):
                        force = True    # '} var' only for type braces
            elif not (t.kind == 'OP' and t.text in
                      (',', ';', '.', '->', ')', ']', '(', '[')):
                force = True

        if after_comment and im.text.startswith('//'):
            gap[i] = 'nl'
        elif force:
            gap[i] = 'nl'
        elif preserve:
            gap[i] = 'nl'
        elif t.kind == 'COMMENT':
            gap[i] = 'nl' if t.nl_before >= 1 else 'space'
        elif after_comment:
            gap[i] = 'nl' if t.nl_before >= 1 else 'space'
        else:
            gap[i] = base_gap(toks, pi, i, casts, decls)

        if gap[i] == 'nl' and t.nl_before >= 2:
            blank.add(i)

    # ':' right after case/goto labels: no space before, newline after
    for i in case_colon | label_colon:
        gap[i] = 'none'
        if i + 1 < n:
            gap[i + 1] = 'nl'
            if toks[i + 1].nl_before >= 2:
                blank.add(i + 1)

    # do-while: '} while' stays on one line ([M07])
    for i, t in enumerate(toks):
        if t.kind == 'IDENT' and t.text == 'while':
            pi = prev_tok(toks, i)
            if pi in do_close:
                gap[i] = 'space'
                blank.discard(i)

    return gap, blank, col0, case_head


# --------------------------------------------------------------------------
# Emission ([M01] tabs, [M02] case indent, [R03] label column 0)


def emit(toks, gap, blank, col0, case_head, bclass, switch_brace):
    lines = []
    cur = []
    depth = 0
    stack = []          # one entry per open '{': True for switch bodies

    def close_line():
        lines.append(''.join(cur).rstrip(' \t'))   # [R05]
        del cur[:]

    started = False
    for i, t in enumerate(toks):
        if gap[i] == 'nl' and started:
            close_line()
            started = False
        if i in blank and lines and lines[-1] != '' and started is False:
            lines.append('')

        if t.kind == 'OP' and t.text == '}':
            if stack:
                stack.pop()
            depth = max(0, depth - 1)

        if not started:
            if i in col0:
                indent = ''
            elif t.kind == 'IDENT' and i in case_head and stack \
                    and stack[-1]:
                indent = '\t' * max(0, depth - 1)    # [M02]
            else:
                indent = '\t' * depth                 # [M01]
            cur.append(indent)
            started = True
        elif gap[i] == 'space':
            cur.append(' ')

        cur.append(t.text)

        if t.kind == 'OP' and t.text == '{':
            depth += 1
            stack.append(i in switch_brace)

    if started:
        close_line()
    while lines and lines[-1] == '':
        lines.pop()
    if not lines:
        return ''
    return '\n'.join(lines) + '\n'                   # [R06]


def format_source(text):
    """One formatting pass over *text*; returns the formatted source."""
    if not text.strip():
        return text
    toks = tokenize(text)
    if not toks:
        return text
    pmatch = match_pairs(toks, '(', ')')
    bmatch = match_pairs(toks, '{', '}')
    bclass = classify_braces(toks, bmatch)
    casts = find_casts(toks, pmatch)
    decls = decl_pointer_indices(toks)
    gap, blank, col0, case_head = compute_layout(
        toks, bclass, bmatch, pmatch, casts, decls)

    switch_brace = set()
    for idx in bclass:
        if bclass[idx] == 'ctrl':
            k = sig_back(toks, idx)
            if k >= 0 and toks[k].kind == 'IDENT' \
                    and toks[k].text == 'switch':
                switch_brace.add(idx)

    return emit(toks, gap, blank, col0, case_head, bclass, switch_brace)


def token_signature(text):
    """Token stream of *text* without whitespace information."""
    return [(t.kind, t.text) for t in tokenize(text)]


# --------------------------------------------------------------------------
# File processing and command line interface


class Result(object):
    def __init__(self, path, status, message=''):
        self.path = path
        self.status = status
        self.message = message


def format_file(path, check=False, backup=False, show_diff=False):
    """Format one file.  Returns a Result with one of:
       'unchanged', 'formatted', 'would-format', 'error'."""
    try:
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            original = f.read()
    except OSError as e:
        return Result(path, 'error', str(e))

    try:
        formatted = format_source(original)
    except Exception as e:                       # noqa: BLE001
        return Result(path, 'error', 'formatter failed: %s' % e)

    # safety interlock 1: token stream must be unchanged
    if token_signature(original) != token_signature(formatted):
        return Result(path, 'error',
                      'token stream changed; file left untouched')

    # safety interlock 2: formatting must be idempotent
    try:
        twice = format_source(formatted)
    except Exception as e:                       # noqa: BLE001
        return Result(path, 'error', 'second pass failed: %s' % e)
    if twice != formatted:
        return Result(path, 'error',
                      'formatting is not idempotent; file left untouched')

    if formatted == original:
        return Result(path, 'unchanged')

    if check:
        return Result(path, 'would-format')
    if show_diff:
        diff = difflib.unified_diff(
            original.splitlines(True), formatted.splitlines(True),
            fromfile=path, tofile=path + ' (formatted)')
        return Result(path, 'would-format', ''.join(diff))
    if backup:
        with open(path + '.orig', 'w', encoding='utf-8') as f:
            f.write(original)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(formatted)
    return Result(path, 'formatted')


def collect_targets(targets):
    files = []
    for tgt in targets:
        if os.path.isdir(tgt):
            for root, dirs, names in os.walk(tgt):
                dirs[:] = [d for d in sorted(dirs)
                           if d not in ('.git', '.orig')]
                for name in sorted(names):
                    if name.endswith(C_SUFFIX) and not \
                            name.endswith('.orig'):
                        files.append(os.path.join(root, name))
        elif os.path.isfile(tgt):
            files.append(tgt)
        else:
            print('error: no such file or directory: %s' % tgt,
                  file=sys.stderr)
    return files


def main(argv=None):
    parser = argparse.ArgumentParser(
        prog='cformatter.py',
        description='TizenRT C formatter (docs/CodingStyleGuide.md)')
    parser.add_argument('targets', nargs='+', metavar='TARGET',
                        help='.c/.h file or directory')
    parser.add_argument('--check', action='store_true',
                        help='report files needing formatting; '
                             'exit 1 when found (for CI)')
    parser.add_argument('--diff', action='store_true',
                        help='print a unified diff instead of writing')
    parser.add_argument('--backup', action='store_true',
                        help='keep a .orig copy before rewriting')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='list every processed file')
    args = parser.parse_args(argv)

    files = collect_targets(args.targets)
    if not files:
        print('no C files found', file=sys.stderr)
        return 2

    counts = {}
    failed = 0
    for path in files:
        res = format_file(path, check=args.check,
                          show_diff=args.diff, backup=args.backup)
        counts[res.status] = counts.get(res.status, 0) + 1
        if res.status == 'error':
            failed += 1
            print('%s: %s' % (res.path, res.message), file=sys.stderr)
        elif args.verbose or args.check or args.diff:
            if res.status == 'would-format':
                print(res.message if args.diff else res.path)
            elif args.verbose:
                print('%s: %s' % (res.path, res.status))

    if not (args.check or args.diff or args.verbose):
        summary = ', '.join('%d %s' % (v, k)
                            for k, v in sorted(counts.items()))
        print('%d file(s): %s' % (len(files), summary or 'nothing'))

    if args.check:
        return 1 if counts.get('would-format') else 0
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
