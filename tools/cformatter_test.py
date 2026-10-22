#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Regression tests for cformatter.py.

Usage:
    python3 tools/cformatter_test.py
"""

import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import cformatter  # noqa: E402


def body(lines):
    """Wrap statement lines into a function definition."""
    return 'void f(void)\n{\n' + ''.join('\t%s\n' % ln for ln in lines) + \
        '}\n'


class FormatterTest(unittest.TestCase):

    def check(self, src, expected):
        out = cformatter.format_source(src)
        self.assertEqual(out, expected)
        # safety interlocks used before rewriting a file
        self.assertEqual(cformatter.token_signature(src),
                         cformatter.token_signature(out))
        self.assertEqual(cformatter.format_source(out), out)

    def check_stmts(self, pairs):
        self.check(body([p[0] for p in pairs]), body([p[1] for p in pairs]))

    def test_operator_spacing(self):
        # [M04] binary operators spaced, unary operators and casts not
        self.check_stmts([
            ('y = a|b;', 'y = a | b;'),
            ('y = a&~MASK;', 'y = a & ~MASK;'),
            ('y = x++ & 1;', 'y = x++ & 1;'),
            ('y = a[i]++ & 1;', 'y = a[i]++ & 1;'),
            ('y = x-- | 1;', 'y = x-- | 1;'),
            ('x = y + ++ z;', 'x = y + ++z;'),
            ('x = y - -- z;', 'x = y - --z;'),
            ('x = (uint8_t) ~a;', 'x = (uint8_t)~a;'),
            ('x = (uint32_t) &y;', 'x = (uint32_t)&y;'),
            ('x = (bool) y;', 'x = (bool)y;'),
            ('x = (my_type) y;', 'x = (my_type)y;'),
            ('x = (FAR char *) &buf[0];', 'x = (FAR char *)&buf[0];'),
            ('((void (*)(void)) 0xDEADBEEF)();',
             '((void (*)(void))0xDEADBEEF)();'),
            ('*end = 1;', '*end = 1;'),
            ('return -EINVAL;', 'return -EINVAL;'),
            ('f(-1, &x, *b);', 'f(-1, &x, *b);'),
            ('r = a ? -1 : +1;', 'r = a ? -1 : +1;'),
        ])

    def test_ambiguous_spacing_is_kept(self):
        # without type information these keep the author's spacing
        self.check_stmts([
            ('y = (x) & mask;', 'y = (x) & mask;'),
            ('y = (x)-1;', 'y = (x)-1;'),
            ('y = (my_type)&x;', 'y = (my_type)&x;'),
            ('FAR fd_set *readfds;', 'FAR fd_set *readfds;'),
            ('y = a * b;', 'y = a * b;'),
        ])

    def test_declarations(self):
        self.check_stmts([
            ('struct exlink * link;', 'struct exlink *link;'),
            ('void (*handler)(int);', 'void (*handler)(int);'),
            ('char * const p = NULL;', 'char *const p = NULL;'),
            ('int             ret;', 'int             ret;'),
        ])

    def test_separators(self):
        # [M03] / [M05]
        self.check_stmts([
            ('if(!value) {', 'if (!value) {'),
            ('}', '}'),
            ('x = sizeof (int);', 'x = sizeof(int);'),
            ('read( &link->left , &link->right );',
             'read(&link->left, &link->right);'),
            ('struct s v = { .a = 1,.b = 2 };',
             'struct s v = { .a = 1, .b = 2 };'),
            ('char n[3] = {0,};', 'char n[3] = {0,};'),
            ('if (/* c */ x) {', 'if (/* c */ x) {'),
            ('}', '}'),
        ])

    def test_trailing_comment_and_wrapping_are_kept(self):
        src = body([
            'int a = 0;\t\t/* aligned comment */',
            'entry = create("abc", NULL,',
            '               NULL);',
            'if (x)',
            '\treturn;',
        ])
        self.check(src, src)

    def test_braces(self):
        # [M06], [M07], [M08]
        src = ('int function(void) {\n'
               '\tif (a)\n'
               '\t{\n'
               '\t\tb = 1;\n'
               '\t}\n'
               '\telse\n'
               '\t{\n'
               '\t\tb = 2;\n'
               '\t}\n'
               '\tdo\n'
               '\t{\n'
               '\t\ti--;\n'
               '\t}\n'
               '\twhile (i);\n'
               '}\n'
               'struct people\n'
               '{\n'
               '\tint age;\n'
               '};\n')
        expected = ('int function(void)\n'
                    '{\n'
                    '\tif (a) {\n'
                    '\t\tb = 1;\n'
                    '\t} else {\n'
                    '\t\tb = 2;\n'
                    '\t}\n'
                    '\tdo {\n'
                    '\t\ti--;\n'
                    '\t} while (i);\n'
                    '}\n'
                    'struct people {\n'
                    '\tint age;\n'
                    '};\n')
        self.check(src, expected)

    def test_switch_and_labels(self):
        # [M02], [R03]
        src = body([
            'switch (state) {',
            '\t/* first */',
            '\tcase START:',
            '\t\tbreak;',
            'default:',
            '\t\tbreak;',
            '}',
            '\tout:',
            'return;',
        ])
        expected = ('void f(void)\n{\n'
                    '\tswitch (state) {\n'
                    '\t/* first */\n'
                    '\tcase START:\n'
                    '\t\tbreak;\n'
                    '\tdefault:\n'
                    '\t\tbreak;\n'
                    '\t}\n'
                    'out:\n'
                    '\treturn;\n'
                    '}\n')
        self.check(src, expected)

    def test_statements_are_not_joined(self):
        src = body(['stcb = x;', 'while (y) {', '\ty--;', '}'])
        self.check(src, src)

    def test_preprocessor_branches(self):
        src = ('void f(void)\n{\n'
               '#ifdef X\n'
               '\tif (y) {\n'
               '\t\tz = 1;\n'
               '\t} else\n'
               '#endif\n'
               '\t\tif (q) {\n'
               '\t\t\tr = 2;\n'
               '\t\t}\n'
               '\treturn;\n'
               '}\n')
        self.check(src, src)

    def test_consecutive_labels(self):
        src = body(['x = 1;']).replace(
            '}\n', 'success:\n\n'
            '\t/* Error exits */\n\n'
            'errout:\n'
            '\treturn;\n}\n')
        self.check(src, src)

    def test_brace_after_comment_line_is_not_joined(self):
        src = body(['if (x)', '\t/* comment */', '{', '\ty = 1;', '}'])
        expected = body(['if (x)', '\t/* comment */', '{', '\ty = 1;', '}'])
        self.check(src, expected)

    def test_struct_declarator_after_body(self):
        src = 'struct a {\n\tstruct b {\n\t\tint x;\n\t} *next;\n};\n'
        self.check(src, src)

    def test_case_block_styles(self):
        # both TizenRT styles of 'case' blocks are kept
        src = body([
            'switch (cmd) {',
            'case A:',
            '{',
            '\tx = 1;',
            '}',
            'break;',
            'case B: {',
            '\tx = 2;',
            '}',
            '\tbreak;',
            '}',
        ])
        self.check(src, src)

    def test_dangling_else_after_comment(self):
        src = body([
            'if (a) {',
            '\tx = 1;',
            '} else',
            '#endif',
            '\tif (b) {',
            '\t\ty = 1;',
            '\t}',
            '',
            '\t/* comment */',
            '',
            '\telse {',
            '\t\ty = 2;',
            '\t}',
        ]).replace('\t#endif', '#endif').replace('\t\n', '\n')
        self.check(src, src)

    def warnings_of(self, src):
        warnings = []
        cformatter.format_source(src, warnings)
        return warnings

    def test_m07_warnings(self):
        # a comment between '}' / header and 'else' / '{' cannot be moved
        # by the formatter: reported for a manual fix
        src = body(['if (a) {', '\tx = 1;', '}', '/* other */',
                    'else {', '\tx = 2;', '}'])
        self.assertEqual([w[0] for w in self.warnings_of(src)], [7])
        src = body(['if (a)', '\t/* comment */', '{', '\tx = 1;', '}'])
        self.assertEqual([w[0] for w in self.warnings_of(src)], [5])
        # conditional compilation between them is a normal pattern
        src = body(['if (a) {', '\tx = 1;', '} else', '#endif', '{',
                    '\tx = 2;', '}'])
        self.assertEqual(self.warnings_of(src), [])

    def test_extern_c_is_not_indented(self):
        src = ('#ifdef __cplusplus\n'
               'extern "C" {\n'
               '#endif\n'
               'void sem_initialize(void);\n'
               '#ifdef __cplusplus\n'
               '}\n'
               '#endif\n')
        self.check(src, src)

    def test_indent_off_region(self):
        src = ('/* *INDENT-OFF* */\n'
               'int  x=1 ;\n'
               '/* *INDENT-ON* */\n'
               'int y = 2;\n')
        self.check(src, src)

    def test_cplusplus_detection(self):
        toks = cformatter.tokenize('class Foo {\npublic:\n\tint x;\n};\n')
        self.assertTrue(cformatter.is_cplusplus(toks))
        toks = cformatter.tokenize('int class_id;\nint public_key;\n')
        self.assertFalse(cformatter.is_cplusplus(toks))


if __name__ == '__main__':
    unittest.main()
