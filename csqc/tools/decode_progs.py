#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""decode_progs.py — декод progs-файла (QC bytecode).

Разбирает форматы:
  * v7-extended 16-bit (FTE `fte_5768`, secondaryversion == 0x21b1461) — наш csprogs.dat;
  * v6-classic (как его читает ezquake PR1) — для сравнения с классическим PR1.

Layout (общий для обоих форматов, little-endian):
  header     dprograms_t  — 23 int (v7) или 15 int (v6);
  statements dstatement16 — 8 байт: op u16, a/b/c u16 (знаковые при прыжках);
  globaldefs/fielddefs   — 8 байт: type u16, ofs u16, s_name int (offset в строковую область);
  functions  dfunction_t  — 36 байт (parm_size[8] в хвосте);
  strings    ofs_strings + numstrings байт: s_name = относительный offset.

Имена опкодов — из таблицы pr_opcodes[].opname (fteqw/engine/qclib/qcc_pr_comp.c,
порядок записей = номер опкода формата fte_5768). Таблица вшита ниже (OPCODE_NAMES);
при наличии fteqw можно перечитать свежую через --opcode-table.

Пример:
  python3 decode_progs.py [csprogs.dat] [--opcode-table /path/qcc_pr_comp.c] [--json]
"""

import argparse
import collections
import json
import re
import struct
import sys

# Максимальный номер «классического» опкода (набор QW PR1, ezquake pr_comp.h):
# всё, что больше — расширение FTE (v7/ext).
CLASSIC_MAX = 65

# Имена опкодов формата fte_5768 (индекс == номер опкода). Источник:
# fteqw/engine/qclib/qcc_pr_comp.c, таблица pr_opcodes[], поле opname.
# (таблица вставлена ниже)
OPCODE_NAMES = {
    0: 'DONE',
    1: 'MUL_F',
    2: 'MUL_V',
    3: 'MUL_FV',
    4: 'MUL_VF',
    5: 'DIV_F',
    6: 'ADD_F',
    7: 'ADD_V',
    8: 'SUB_F',
    9: 'SUB_V',
    10: 'EQ_F',
    11: 'EQ_V',
    12: 'EQ_S',
    13: 'EQ_E',
    14: 'EQ_FNC',
    15: 'NE_F',
    16: 'NE_V',
    17: 'NE_S',
    18: 'NE_E',
    19: 'NE_FNC',
    20: 'LE_F',
    21: 'GE_F',
    22: 'LT_F',
    23: 'GT_F',
    24: 'LOADF_F',
    25: 'LOADF_V',
    26: 'LOADF_S',
    27: 'LOADF_ENT',
    28: 'LOADF_FLD',
    29: 'LOADF_FNC',
    30: 'FLDADDRESS',
    31: 'STORE_F',
    32: 'STORE_V',
    33: 'STORE_S',
    34: 'STORE_ENT',
    35: 'STORE_FLD',
    36: 'STORE_FNC',
    37: 'STOREP_F',
    38: 'STOREP_V',
    39: 'STOREP_S',
    40: 'STOREP_ENT',
    41: 'STOREP_FLD',
    42: 'STOREP_FNC',
    43: 'RETURN',
    44: 'NOT_F',
    45: 'NOT_V',
    46: 'NOT_S',
    47: 'NOT_ENT',
    48: 'NOT_FNC',
    49: 'IF',
    50: 'IFNOT',
    51: 'CALL0',
    52: 'CALL1',
    53: 'CALL2',
    54: 'CALL3',
    55: 'CALL4',
    56: 'CALL5',
    57: 'CALL6',
    58: 'CALL7',
    59: 'CALL8',
    60: 'STATE',
    61: 'GOTO',
    62: 'AND_F',
    63: 'OR_F',
    64: 'BITAND',
    65: 'BITOR',
    66: 'MULSTORE_F',
    67: 'MULSTORE_VF',
    68: 'MULSTOREP_F',
    69: 'MULSTOREP_VF',
    70: 'DIVSTORE_F',
    71: 'DIVSTOREP_F',
    72: 'ADDSTORE_F',
    73: 'ADDSTORE_V',
    74: 'ADDSTOREP_F',
    75: 'ADDSTOREP_V',
    76: 'SUBSTORE_F',
    77: 'SUBSTORE_V',
    78: 'SUBSTOREP_F',
    79: 'SUBSTOREP_V',
    80: 'FETCH_GBL_F',
    81: 'FETCH_GBL_V',
    82: 'FETCH_GBL_S',
    83: 'FETCH_GBL_E',
    84: 'FETCH_GBL_FNC',
    85: 'CSTATE',
    86: 'CWSTATE',
    87: 'THINKTIME',
    88: 'BITSETSTORE_F',
    89: 'BITSETSTOREP_F',
    90: 'BITCLRSTORE_F',
    91: 'BITCLRSTOREP_F',
    92: 'RAND0',
    93: 'RAND1',
    94: 'RAND2',
    95: 'RANDV0',
    96: 'RANDV1',
    97: 'RANDV2',
    98: 'SWITCH_F',
    99: 'SWITCH_V',
    100: 'SWITCH_S',
    101: 'SWITCH_E',
    102: 'SWITCH_FNC',
    103: 'CASE',
    104: 'CASERANGE',
    105: 'CALL1H',
    106: 'CALL2H',
    107: 'CALL3H',
    108: 'CALL4H',
    109: 'CALL5H',
    110: 'CALL6H',
    111: 'CALL7H',
    112: 'CALL8H',
    113: 'STORE_I',
    114: 'STORE_IF',
    115: 'STORE_FI',
    116: 'ADD_I',
    117: 'ADD_FI',
    118: 'ADD_IF',
    119: 'SUB_I',
    120: 'SUB_FI',
    121: 'SUB_IF',
    122: 'CONV_IF',
    123: 'CONV_FI',
    124: 'CONVP_IF',
    125: 'CONVP_FI',
    126: 'LOADF_I',
    127: 'STOREP_I',
    128: 'STOREP_IF',
    129: 'STOREP_FI',
    130: 'BITAND_I',
    131: 'BITOR_I',
    132: 'MUL_I',
    133: 'DIV_I',
    134: 'EQ_I',
    135: 'NE_I',
    136: 'IFNOTS',
    137: 'IFS',
    138: 'NOT_I',
    139: 'DIV_VF',
    140: 'BITXOR_I',
    141: 'RSHIFT_I',
    142: 'LSHIFT_I',
    143: 'GLOBALADDRESS',
    144: 'ADD_PIW',
    145: 'LOADA_F',
    146: 'LOADA_V',
    147: 'LOADA_S',
    148: 'LOADA_ENT',
    149: 'LOADA_FLD',
    150: 'LOADA_FNC',
    151: 'LOADA_I',
    152: 'STORE_P',
    153: 'LOADF_P',
    154: 'LOADP_F',
    155: 'LOADP_V',
    156: 'LOADP_S',
    157: 'LOADP_ENT',
    158: 'LOADP_FLD',
    159: 'LOADP_FNC',
    160: 'LOADP_I',
    161: 'LE_I',
    162: 'GE_I',
    163: 'LT_I',
    164: 'GT_I',
    165: 'LE_IF',
    166: 'GE_IF',
    167: 'LT_IF',
    168: 'GT_IF',
    169: 'LE_FI',
    170: 'GE_FI',
    171: 'LT_FI',
    172: 'GT_FI',
    173: 'EQ_IF',
    174: 'EQ_FI',
    175: 'ADD_SF',
    176: 'SUB_S',
    177: 'STOREP_C',
    178: 'LOADP_C',
    179: 'MUL_IF',
    180: 'MUL_FI',
    181: 'MUL_VI',
    182: 'MUL_IV',
    183: 'DIV_IF',
    184: 'DIV_FI',
    185: 'BITAND_IF',
    186: 'BITOR_IF',
    187: 'BITAND_FI',
    188: 'BITOR_FI',
    189: 'AND_I',
    190: 'OR_I',
    191: 'AND_IF',
    192: 'OR_IF',
    193: 'AND_FI',
    194: 'OR_FI',
    195: 'NE_IF',
    196: 'NE_FI',
    197: 'GSTOREP_I',
    198: 'GSTOREP_F',
    199: 'GSTOREP_ENT',
    200: 'GSTOREP_FLD',
    201: 'GSTOREP_S',
    202: 'GSTOREP_FNC',
    203: 'GSTOREP_V',
    204: 'GADDRESS',
    205: 'GLOAD_I',
    206: 'GLOAD_F',
    207: 'GLOAD_FLD',
    208: 'GLOAD_ENT',
    209: 'GLOAD_S',
    210: 'GLOAD_FNC',
    211: 'BOUNDCHECK',
    212: 'UNUSED',
    213: 'PUSH',
    214: 'POP',
    215: 'SWITCH_I',
    216: 'GLOAD_V',
    217: 'IF_F',
    218: 'IFNOT_F',
    219: 'STOREF_V',
    220: 'STOREF_F',
    221: 'STOREF_S',
    222: 'STOREF_I',
    223: 'STOREP_I8',
    224: 'LOADP_U8',
    225: 'LE_U',
    226: 'LT_U',
    227: 'DIV_U',
    228: 'RSHIFT_U',
    229: 'ADD_I64',
    230: 'SUB_I64',
    231: 'MUL_I64',
    232: 'DIV_I64',
    233: 'BITAND_I64',
    234: 'BITOR_I64',
    235: 'BITXOR_I64',
    236: 'LSHIFT_I64I',
    237: 'RSHIFT_I64I',
    238: 'LE_I64',
    239: 'LT_I64',
    240: 'EQ_I64',
    241: 'NE_I64',
    242: 'LE_U64',
    243: 'LT_U64',
    244: 'DIV_U64',
    245: 'RSHIFT_U64I',
    246: 'STORE_I64',
    247: 'STOREP_I64',
    248: 'STOREF_I64',
    249: 'LOADF_I64',
    250: 'LOADA_I64',
    251: 'LOADP_I64',
    252: 'CONV_UI64',
    253: 'CONV_II64',
    254: 'CONV_I64I',
    255: 'CONV_FD',
    256: 'CONV_DF',
    257: 'CONV_I64F',
    258: 'CONV_FI64',
    259: 'CONV_I64D',
    260: 'CONV_DI64',
    261: 'ADD_D',
    262: 'SUB_D',
    263: 'MUL_D',
    264: 'DIV_D',
    265: 'LE_D',
    266: 'LT_D',
    267: 'EQ_D',
    268: 'NE_D',
    269: 'STOREP_I16',
    270: 'LOADP_I16',
    271: 'LOADP_U16',
    272: 'LOADP_I8',
    273: 'BITEXTEND_I',
    274: 'BITEXTEND_U',
    275: 'BITCOPY_I',
    276: 'CONV_UF',
    277: 'CONV_FU',
    278: 'CONV_U64D',
    279: 'CONV_DU64',
    280: 'CONV_U64F',
    281: 'CONV_FU64',
    282: 'OP_EMULATED',
    283: 'BITSET_I',
    284: 'BITSETP_I',
    285: 'BITCLR_I',
    286: 'MULSTORE_I',
    287: 'DIVSTORE_I',
    288: 'ADDSTORE_I',
    289: 'SUBSTORE_I',
    290: 'MULSTOREP_I',
    291: 'DIVSTOREP_I',
    292: 'ADDSTOREP_I',
    293: 'SUBSTOREP_I',
    294: 'MULSTORE_IF',
    295: 'MULSTOREP_IF',
    296: 'DIVSTORE_IF',
    297: 'DIVSTOREP_IF',
    298: 'ADDSTORE_IF',
    299: 'ADDSTOREP_IF',
    300: 'SUBSTORE_IF',
    301: 'SUBSTOREP_IF',
    302: 'MULSTORE_FI',
    303: 'MULSTOREP_FI',
    304: 'DIVSTORE_FI',
    305: 'DIVSTOREP_FI',
    306: 'ADDSTORE_FI',
    307: 'ADDSTOREP_FI',
    308: 'SUBSTORE_FI',
    309: 'SUBSTOREP_FI',
    310: 'MULSTORE_VI',
    311: 'MULSTOREP_VI',
    312: 'LOADA_STRUCT',
    313: 'LOADP_P',
    314: 'STOREP_P',
    315: 'BITNOT_F',
    316: 'BITNOT_I',
    317: 'EQ_P',
    318: 'NE_P',
    319: 'LE_P',
    320: 'GE_P',
    321: 'LT_P',
    322: 'GT_P',
    323: 'ANDSTORE_F',
    324: 'BITCLR_F',
    325: 'BITCLR_I',
    326: 'BITCLR_V',
    327: 'ADD_SI',
    328: 'ADD_IS',
    329: 'ADD_PF',
    330: 'ADD_FP',
    331: 'ADD_PI',
    332: 'ADD_IP',
    333: 'ADD_PU',
    334: 'ADD_UP',
    335: 'SUB_SI',
    336: 'SUB_PF',
    337: 'SUB_PI',
    338: 'SUB_PU',
    339: 'SUB_PP',
    340: 'MOD_F',
    341: 'MOD_I',
    342: 'MOD_FI',
    343: 'MOD_IF',
    344: 'MOD_V',
    345: 'BITXOR_F',
    346: 'RSHIFT_F',
    347: 'LSHIFT_F',
    348: 'RSHIFT_IF',
    349: 'LSHIFT_IF',
    350: 'RSHIFT_FI',
    351: 'LSHIFT_FI',
    352: 'AND_ANY',
    353: 'OR_ANY',
    354: 'ADD_EI',
    355: 'ADD_EF',
    356: 'SUB_EI',
    357: 'SUB_EF',
    358: 'BITAND_V',
    359: 'BITOR_V',
    360: 'BITNOT_V',
    361: 'BITXOR_V',
    362: 'POW_F',
    363: 'POW_I',
    364: 'POW_FI',
    365: 'POW_IF',
    366: 'CROSS_V',
    367: 'EQ_FLD',
    368: 'NE_FLD',
    369: 'SPACESHIP_F',
    370: 'SPACESHIP_S',
    371: 'ADD_U',
    372: 'SUB_U',
    373: 'MUL_U',
    374: 'MOD_U',
    375: 'BITAND_U',
    376: 'BITOR_U',
    377: 'BITXOR_U',
    378: 'BITNOT_U',
    379: 'BITCLR_U',
    380: 'LSHIFT_U',
    381: 'GE_U',
    382: 'GT_U',
    383: 'EQ_U',
    384: 'NE_U',
    385: 'BITNOT_I64',
    386: 'BITCLR_I64',
    387: 'GE_I64',
    388: 'GT_I64',
    389: 'ADD_U64',
    390: 'SUB_U64',
    391: 'MUL_U64',
    392: 'MOD_U64',
    393: 'BITAND_U64',
    394: 'BITOR_U64',
    395: 'BITXOR_U64',
    396: 'BITNOT_U64',
    397: 'BITCLR_U64',
    398: 'LSHIFT_U64I',
    399: 'GE_U64',
    400: 'GT_U64',
    401: 'EQ_U64',
    402: 'NE_U64',
    403: 'BITAND_D',
    404: 'BITOR_D',
    405: 'BITXOR_D',
    406: 'BITNOT_D',
    407: 'BITCLR_D',
    408: 'LSHIFT_DI',
    409: 'RSHIFT_DI',
    410: 'GE_D',
    411: 'GT_D',
    412: 'WSTATE',
}



def opname(n):
    return OPCODE_NAMES.get(n, 'OP_%d' % n)


def is_classic(n):
    return n <= CLASSIC_MAX


def parse_opcode_table(path):
    """Перечитать имена опкодов из qcc_pr_comp.c (порядок записей == номер)."""
    src = open(path, 'rb').read().decode('latin1')
    m = re.search(r'QCC_opcode_t\s+pr_opcodes\[\]\s*=\s*\{(.*?)\n\};', src, re.S)
    if not m:
        raise ValueError('pr_opcodes table not found in %s' % path)
    body = re.sub(r'//[^\n]*', '', m.group(1))
    body = re.sub(r'/\*.*?\*/', '', body, flags=re.S)
    names = {}
    for i, em in enumerate(re.finditer(r'\{\s*\d+\s*,\s*"([^"]*)"\s*,\s*"([A-Z0-9_]+)"', body)):
        names[i] = em.group(2)
    return names


class Progs:
    def __init__(self, data):
        self.d = data
        h = struct.unpack('<23i', data[:92])
        if h[0] not in (6, 7):
            raise ValueError('unsupported progs version %r (need 6 or 7)' % h[0])
        self.version = h[0]
        if h[0] == 7:
            (self.crc, self.ofs_statements, self.numstatements,
             self.ofs_globaldefs, self.numglobaldefs,
             self.ofs_fielddefs, self.numfielddefs,
             self.ofs_functions, self.numfunctions,
             self.ofs_strings, self.numstrings,
             self.ofs_globals, self.numglobals, self.entityfields,
             self.ofsfiles, self.ofslinenums,
             self.ofsbodylessfuncs, self.numbodylessfuncs,
             self.ofs_types, self.numtypes, self.blockscompressed,
             self.secondaryversion) = h[1:23]
        else:
            h6 = struct.unpack('<15i', data[:60])
            (self.crc, self.ofs_statements, self.numstatements,
             self.ofs_globaldefs, self.numglobaldefs,
             self.ofs_fielddefs, self.numfielddefs,
             self.ofs_functions, self.numfunctions,
             self.ofs_strings, self.numstrings,
             self.ofs_globals, self.numglobals, self.entityfields) = h6[1:15]
            self.ofsfiles = self.ofslinenums = self.ofsbodylessfuncs = 0
            self.numbodylessfuncs = self.ofs_types = self.numtypes = 0
            self.blockscompressed = 0
            self.secondaryversion = 0
        self.sz = len(data)
        self.strbase = self.ofs_strings

    def statement(self, i):
        o = self.ofs_statements + i * 8
        op, a, b, c = struct.unpack_from('<HHHH', self.d, o)
        return op, _s16(a), _s16(b), _s16(c)

    def function(self, i):
        o = self.ofs_functions + i * 36
        fs, parm_start, locals_, prof, s_name, s_file, numparms = struct.unpack_from('<7i', self.d, o)
        return dict(index=i, first_statement=fs, parm_start=parm_start, locals=locals_,
                    s_name=s_name, s_file=s_file, numparms=numparms)

    def def_(self, i, kind):
        o = (self.ofs_globaldefs if kind == 'global' else self.ofs_fielddefs) + i * 8
        typ, ofs, name = struct.unpack_from('<HHi', self.d, o)
        return typ, ofs, name

    def string(self, ofs):
        """Строка по относительному offset в строковой области."""
        p = self.strbase + ofs
        if ofs < 0 or p >= self.sz:
            return '<bad-string-%d>' % ofs
        e = self.d.find(b'\x00', p)
        if e < 0:
            e = self.sz
        return self.d[p:e].decode('utf-8', 'replace')


def _s16(v):
    return v - 0x10000 if v & 0x8000 else v


def analyze(progs):
    stmts = []
    for i in range(progs.numstatements):
        op, a, b, c = progs.statement(i)
        stmts.append((op, a, b, c))

    funcs = [progs.function(i) for i in range(progs.numfunctions)]
    code_funcs = sorted([f for f in funcs if f['first_statement'] >= 0],
                        key=lambda f: f['first_statement'])

    # Диапазоны statements по функциям: [fs, fs_next).
    ranges = []
    for k, f in enumerate(code_funcs):
        start = f['first_statement']
        end = code_funcs[k + 1]['first_statement'] if k + 1 < len(code_funcs) else progs.numstatements
        ranges.append((f, start, end))

    # statements, покрытые телами функций (для оценки «хвоста»).
    covered = set()
    for f, s, e in ranges:
        covered.update(range(s, e))

    hist = collections.Counter(s[0] for s in stmts)
    ext = {n for n in hist if not is_classic(n) and n != 0}

    per_func = []
    for f, s, e in ranges:
        ops = collections.Counter(stmts[i][0] for i in range(s, e))
        name = progs.string(f['s_name'])
        per_func.append(dict(fn=f, start=s, end=e, name=name, ops=ops))

    builtins = [(f['index'], -f['first_statement'], f['numparms'])
                for f in funcs if f['first_statement'] < 0]

    return dict(stmts=stmts, hist=hist, ext=ext, code_funcs=code_funcs,
                ranges=ranges, covered=covered, per_func=per_func, builtins=builtins)


def _per_func_pairs(an):
    return [(r['fn'], r['start'], r['end'], r['name'], r['ops']) for r in an['per_func']]


def fmt_report(progs, an, use_json):
    h = progs
    header = [
        'version=%d crc=%d size=%d' % (h.version, h.crc, h.sz),
        'statements=%d  functions=%d  globaldefs=%d  fielddefs=%d  globals=%d  entityfields=%d'
        % (h.numstatements, h.numfunctions, h.numglobaldefs, h.numfielddefs, h.numglobals, h.entityfields),
    ]
    if h.version == 7:
        header.append('secondaryversion=0x%08x  numtypes=%d  numbodylessfuncs=%d  blockscompressed=%d'
                      % (h.secondaryversion, h.numtypes, h.numbodylessfuncs, h.blockscompressed))

    classic = [(n, c) for n, c in an['hist'].items() if is_classic(n)]
    extl = [(n, c) for n, c in an['hist'].items() if not is_classic(n) and n != 0]

    hist_rows = ['--- global opcode histogram (statements=%d) ---' % h.numstatements]
    for n, c in sorted(extl, key=lambda t: (-t[1], t[0])):
        hist_rows.append('  %4d %-16s %6d  EXT' % (n, opname(n), c))
    for n, c in sorted(classic, key=lambda t: (-t[1], t[0])):
        hist_rows.append('  %4d %-16s %6d  classic' % (n, opname(n), c))
    if not classic and not extl:
        hist_rows.append('  (no statements)')

    fn_rows = ['', '--- functions with code (first_statement >= 0): %d ---' % len(an['ranges'])]
    for f, s, e, name, ops in _per_func_pairs(an):
        ops_sorted = ', '.join('%s(%d)' % (opname(n), c) for n, c in sorted(ops.items(), key=lambda t: t[0]))
        fn_rows.append('  #%d %-22s stmts %d..%d (%d)  parm_start=%d locals=%d numparms=%d'
                       % (f['index'], name or '<unnamed>', s, e, e - s, f['parm_start'], f['locals'], f['numparms']))
        fn_rows.append('      ' + (ops_sorted if ops_sorted else '(no statements)'))

    ext_rows = ['', '--- extended opcodes present (num > %d), candidates to implement/rewrite ---' % CLASSIC_MAX]
    ext_where = collections.defaultdict(list)
    for f, s, e, name, ops in _per_func_pairs(an):
        for n, c in ops.items():
            if not is_classic(n):
                ext_where[n].append((name or '<unnamed>', c))
    for n in sorted(ext_where):
        where = ', '.join('%s x%d' % (nm, c) for nm, c in ext_where[n])
        ext_rows.append('  %4d %-16s total %d  <- %s' % (n, opname(n), an['hist'][n], where))
    if not ext_where:
        ext_rows.append('  (none)')

    bi = collections.Counter(b for _, b, _ in an['builtins'])
    bi_rows = ['', '--- builtin wrappers (first_statement = -N): %d funcs, %d unique numbers ---'
               % (len(an['builtins']), len(bi))]
    for b in sorted(bi):
        bi_rows.append('  builtin %d  (wrappers: %d)' % (b, bi[b]))

    def_rows = ['', '--- defs sample: first 8 globaldefs / first 8 fielddefs ---']
    for kind, cnt in (('global', h.numglobaldefs), ('field', h.numfielddefs)):
        for i in range(min(8, cnt)):
            typ, ofs, nm = progs.def_(i, kind)
            def_rows.append('  %sdef #%d  type=%d ofs=%d name=%r'
                            % (kind, i, typ, ofs, progs.string(nm)))

    uncovered = h.numstatements - len(an['covered'])

    if use_json:
        return json.dumps(dict(
            header=header,
            hist=dict((opname(k), v) for k, v in sorted(an['hist'].items())),
            functions=[dict(index=f['index'], name=nm, start=s, end=e,
                            ops=dict((opname(k), v) for k, v in sorted(ops.items())))
                       for f, s, e, nm, ops in _per_func_pairs(an)],
            builtins=sorted(set(b for _, b, _ in an['builtins'])),
            uncovered_statements=uncovered,
        ), indent=1, sort_keys=True)

    return ('\n'.join(header) + '\n' + '\n'.join(hist_rows) + '\n' + '\n'.join(fn_rows)
            + '\n' + '\n'.join(ext_rows) + '\n' + '\n'.join(bi_rows) + '\n' + '\n'.join(def_rows)
            + '\nuncovered statements: %d' % uncovered)


def main():
    ap = argparse.ArgumentParser(description='Decode progs.dat (v6-classic / v7-extended 16-bit)')
    ap.add_argument('dat', nargs='?', default='csprogs.dat')
    ap.add_argument('--opcode-table', default=None,
                    help='path to fteqw/engine/qclib/qcc_pr_comp.c to re-read opcode names')
    ap.add_argument('--json', action='store_true')
    args = ap.parse_args()

    global OPCODE_NAMES
    if args.opcode_table:
        OPCODE_NAMES = parse_opcode_table(args.opcode_table)

    data = open(args.dat, 'rb').read()
    progs = Progs(data)
    an = analyze(progs)
    sys.stdout.write(fmt_report(progs, an, args.json) + '\n')


if __name__ == '__main__':
    main()
