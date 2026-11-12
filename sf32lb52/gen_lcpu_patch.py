#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Core Devices LLC
# SPDX-License-Identifier: Apache-2.0

"""Generate a self-contained C header from a vendor SF32LB5x LCPU patch file.

The vendor file is only parsed, never compiled. Any change to its layout or
install logic makes this script fail, so that it gets reviewed.
"""

import argparse
import re
import sys
from pathlib import Path

PATCH_LIST_TAG = 0x50544348
WORDS_PER_LINE = 4

INSTALL_BODY_52 = (
    "uint32_t entry[3] = {{{magic},{word1}, LCPU_PATCH_CODE_START_ADDR + 1}};"
    "memcpy((void *)LCPU_PATCH_BUF_START_ADDR, (void *)&entry, 12);"
    "#ifdef SOC_BF0_HCPU"
    "memset((void*)(LCPU_PATCH_CODE_START_ADDR_S),0,LCPU_PATCH_CODE_SIZE);"
    "memcpy((void*)(LCPU_PATCH_CODE_START_ADDR_S),g_lcpu_patch_bin,sizeof(g_lcpu_patch_bin));"
    "#else"
    "memset((void*)(LCPU_PATCH_CODE_START_ADDR),0,LCPU_PATCH_CODE_SIZE);"
    "memcpy((void*)(LCPU_PATCH_CODE_START_ADDR),g_lcpu_patch_bin,sizeof(g_lcpu_patch_bin));"
    "#endif"
    "HAL_PATCH_install();"
)

ENTRY_ADDR_BODY_52 = (
    "uint32_t *entry_addr = (uint32_t *)LCPU_PATCH_RECORD_ADDR;"
    "uint8_t rev_id = __HAL_SYSCFG_GET_REVID();"
    "if (rev_id >= HAL_CHIP_REV_ID_A4)"
    "entry_addr = (uint32_t *)g_lcpu_patch_list;"
    "return entry_addr;"
)

# The SF32LB57 places the code right after the header, without a separate code area
INSTALL_BODY_57 = (
    "uint32_t entry[3] = {{{magic}, {word1}, LCPU_PATCH_BUF_START_ADDR + 13}};"
    "memcpy((void *)LCPU_PATCH_BUF_START_ADDR, (void *)&entry, 12);"
    "#ifdef SOC_BF0_HCPU"
    "memset((void *)(LCPU_PATCH_BUF_START_ADDR + 12), 0, LCPU_PATCH_TOTAL_SIZE);"
    "memcpy((void *)(LCPU_PATCH_BUF_START_ADDR + 12), g_lcpu_patch_bin, sizeof(g_lcpu_patch_bin));"
    "#else"
    "memset((void *)(LCPU_PATCH_BUF_START_ADDR - 0x20000000 + 12), 0, LCPU_PATCH_TOTAL_SIZE);"
    "memcpy((void *)(LCPU_PATCH_BUF_START_ADDR - 0x20000000 + 12), g_lcpu_patch_bin, "
    "sizeof(g_lcpu_patch_bin));"
    "#endif"
    "HAL_PATCH_install();"
)

ENTRY_ADDR_BODY_57 = (
    "uint32_t *entry_addr = (uint32_t *)g_lcpu_patch_list;"
    "return entry_addr;"
)

SERIES = {
    "sf32lb52": {
        "install": "void lcpu_patch_install_rev_b()",
        "install_body": INSTALL_BODY_52,
        "entry_addr_body": ENTRY_ADDR_BODY_52,
        "array_decl": "static const unsigned int",
    },
    "sf32lb57": {
        "install": "void lcpu_patch_install()",
        "install_body": INSTALL_BODY_57,
        "entry_addr_body": ENTRY_ADDR_BODY_57,
        "array_decl": "const unsigned int",
    },
}


def fail(msg):
    sys.exit(f"error: {msg}")


def squash(text):
    return re.sub(r"\s+", "", text)


def parse_array(src, name, decl):
    m = re.search(
        rf"(?:^|\n){decl} {name}\[\]\s*=\s*\{{(.*?)\}};", src, re.DOTALL
    )
    if not m:
        fail(f"array {name} not found")

    words = []
    for tok in m.group(1).split(","):
        tok = tok.strip()
        if not tok:
            continue
        if not re.fullmatch(r"0x[0-9A-Fa-f]{1,8}", tok):
            fail(f"unexpected token in {name}: {tok!r}")
        words.append(int(tok, 16))

    if not words:
        fail(f"array {name} is empty")

    return words


def parse_body(src, signature):
    m = re.search(rf"{re.escape(signature)}\s*\{{(.*?)\n\}}", src, re.DOTALL)
    if not m:
        fail(f"function {signature} not found")

    return squash(m.group(1))


def parse_install(src, series):
    install = series["install"]
    body = parse_body(src, install)
    m = re.match(r"uint32_tentry\[3\]=\{(0x[0-9A-Fa-f]+),(0x[0-9A-Fa-f]+),", body)
    if not m:
        fail(f"unexpected patch header in {install}")

    magic, word1 = m.groups()
    if body != squash(series["install_body"].format(magic=magic, word1=word1)):
        fail(f"{install} changed")

    return int(magic, 16), int(word1, 16)


def parse_entries(words):
    if words[0] != PATCH_LIST_TAG:
        fail(f"bad patch list tag 0x{words[0]:08X}")

    size = words[1]
    payload = words[2:]
    if size % 8 != 0 or size != len(payload) * 4:
        fail(f"patch list size 0x{size:X} does not match {len(payload)} words")

    return [payload[i : i + 2] for i in range(0, len(payload), 2)]


def fmt_words(words, indent="  "):
    lines = []
    for i in range(0, len(words), WORDS_PER_LINE):
        chunk = words[i : i + WORDS_PER_LINE]
        lines.append(indent + ", ".join(f"0x{w:08X}" for w in chunk) + ",")
    return "\n".join(lines)


def generate(src, src_name, prefix):
    series = SERIES[prefix]
    copyright_m = re.search(r"SPDX-FileCopyrightText:\s*(.+)", src)
    license_m = re.search(r"SPDX-License-Identifier:\s*(.+)", src)
    if not copyright_m or not license_m:
        fail("SPDX header not found")

    if parse_body(src, "uint32_t *HAL_PATCH_GetEntryAddr(void)") != squash(
        series["entry_addr_body"]
    ):
        fail("HAL_PATCH_GetEntryAddr() changed")

    magic, word1 = parse_install(src, series)
    entries = parse_entries(parse_array(src, "g_lcpu_patch_list", series["array_decl"]))
    code = parse_array(src, "g_lcpu_patch_bin", series["array_decl"])

    entry_lines = "\n".join(f"  {{0x{a:08X}, 0x{d:08X}}}," for a, d in entries)

    return f"""\
/* SPDX-FileCopyrightText: {copyright_m.group(1).strip()} */
/* SPDX-License-Identifier: {license_m.group(1).strip()} */

/* Generated by gen_lcpu_patch.py from {src_name}, do not edit. */

#pragma once

#include <stdint.h>

/* Header words before the code address (+1, Thumb), at the start of the patch buffer */
static const uint32_t {prefix}_lcpu_patch_hdr[] = {{0x{magic:08X}, 0x{word1:08X}}};

/* Patch controller entries: break address, replacement word */
static const uint32_t {prefix}_lcpu_patch_entries[][2] = {{
{entry_lines}
}};

/* Patch code, placed right after the header */
static const uint32_t {prefix}_lcpu_patch_code[] = {{
{fmt_words(code)}
}};
"""


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="vendor LCPU patch C file")
    parser.add_argument("output", type=Path, help="generated header")
    parser.add_argument(
        "--series", choices=sorted(SERIES), default="sf32lb52", help="SoC series"
    )
    args = parser.parse_args()

    out = generate(args.input.read_text(), args.input.name, args.series)
    args.output.write_text(out)


if __name__ == "__main__":
    main()
