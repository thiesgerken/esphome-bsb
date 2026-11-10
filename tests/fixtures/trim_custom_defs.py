"""Cuts the test fixture `BSB_LAN_custom_defs.h` out of a full BSB-LAN generated one.

Keeps the category tables and the command table rows of the parameters below, plus every
`#define` and array they reference, as verbatim lines in their original order.

    python3 tests/fixtures/trim_custom_defs.py path/to/BSB_LAN_custom_defs.h > tests/fixtures/BSB_LAN_custom_defs.h
"""

from __future__ import annotations

from dataclasses import dataclass
import re
import sys

KEPT_PARAMETERS = {
    # date/time category, which the dump skips
    "0", "1", "2", "3",
    # time and holiday programs
    "501", "516", "642", "648",
    # generic duplicates (255/255) next to the device rows, fractional parameters
    "700", "701", "701.1", "701.2", "1000", "1300", "1600",
    # writable rows of every value format the dump decodes
    "710", "726", "790", "832", "881", "1610", "1644", "2479", "6141", "7042", "7170", "7400",
    # read-only rows, some with value types the dump can't decode
    "6224", "6225", "6605", "6650", "6700", "8700", "10021",
    # user defined category: write-only rows (one of another device family) and writable rows
    "10000", "10019", "10023", "10046", "10110",
}

HEADER = """\
// Excerpt of the controller specific BSB_LAN_custom_defs.h that BSB-LAN
// (https://github.com/fredlcore/BSB-LAN) generated for an LMS15.000A349 (device family 123,
// variant 4). BSB-LAN and this excerpt are licensed under CC BY-NC-SA 4.0
// (https://creativecommons.org/licenses/by-nc-sa/4.0/); esphome-bsb itself has no license file.
// Cut by tests/fixtures/trim_custom_defs.py: a selection of command table rows plus everything
// they reference, all lines verbatim.
"""

COMMAND_TABLE_START = "PROGMEM_LATE const cmd_t cmdtbl[]={"
ALWAYS_KEPT = {"category_t", "ENUM_CAT_NR", "ENUM_CAT"}


@dataclass
class Block:
    name: str | None
    lines: list[str]

    def references(self) -> set[str]:
        code = "\n".join(line.split("//", 1)[0] for line in self.lines)
        return set(re.findall(r"[A-Za-z_]\w*", code))


def split_blocks(lines: list[str]) -> list[Block]:
    """Definitions before the command table; blank lines become nameless blocks."""
    blocks = []
    comment: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("//"):
            comment.append(line)
            i += 1
            continue
        if not line:
            name, end = None, i + 1
        elif line.startswith("#define "):
            name, end = line.split()[1], i + 1
        elif line == "typedef enum{":
            name, end = "category_t", lines.index("}category_t;", i) + 1
        elif line.startswith("const ") and "[]" in line:
            name = line.split("[]")[0].split()[-1]
            end = lines.index("};", i) + 1 if line.endswith("{") else i + 1
        else:
            raise ValueError(f"line {i + 1}: unexpected {line!r}")
        blocks.append(Block(name, comment + lines[i:end]))
        comment = []
        i = end
    return blocks


def parameter_of(row: str) -> str:
    return row.split(",")[2].strip()


def referenced_names(blocks: list[Block], rows: list[str]) -> set[str]:
    """Names of the blocks the category tables and the rows need, directly or indirectly."""
    by_name = {block.name: block for block in blocks if block.name}
    needed: set[str] = set()
    pending = ALWAYS_KEPT | Block(None, rows).references()
    while pending:
        name = pending.pop()
        if name in by_name and name not in needed:
            needed.add(name)
            pending |= by_name[name].references()
    return needed


def trim(text: str) -> str:
    lines = text.splitlines()
    table_start = lines.index(COMMAND_TABLE_START)
    rows = [row for row in lines[table_start + 1 :] if row and parameter_of(row) in KEPT_PARAMETERS]
    if missing := KEPT_PARAMETERS - {parameter_of(row) for row in rows}:
        raise ValueError(f"parameters not in the command table: {sorted(missing)}")
    blocks = split_blocks(lines[1:table_start])
    needed = referenced_names(blocks, rows)

    out = HEADER.splitlines() + [lines[0]]
    for block in blocks:
        if block.name is None and out[-1] != "":
            out.append("")
        elif block.name in needed:
            out += block.lines
    return "\n".join(out + [COMMAND_TABLE_START] + rows) + "\n"


if __name__ == "__main__":
    with open(sys.argv[1], encoding="utf-8") as source:
        sys.stdout.write(trim(source.read()))
