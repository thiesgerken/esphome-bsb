"""Builds the parameter table of the dump from BSB-LAN definitions and renders it as C++.

Self-contained (no ESPHome imports), so it can be tested on its own.
"""

from __future__ import annotations

from dataclasses import dataclass

from .bsb_lan_defs import (
    FL_NOSWAP_QUR,
    FL_QINF_ONLY,
    FL_SPECIAL_INF,
    CustomDefinitions,
    DefinitionsError,
    float32,
    select_dump_entries,
)
from .bsb_value_types import VALUE_TYPES, ValueType

ENUM_FORMATS = ("Choice", "Enum")


@dataclass(frozen=True)
class DumpRow:
    number: float
    field_id: int
    value_type: ValueType
    enum_values: tuple[int, ...] | None
    category: bytes
    description: bytes


def _query_field_id(field_id: int, flags: int) -> int:
    """The field ID to query with and to expect in the answer.

    Queries swap the first two bytes of the field ID on the wire. For FL_NOSWAP_QUR
    parameters BSB-LAN swaps them in advance, so they arrive unswapped and the answer
    carries the swapped ID.
    """
    if flags & FL_NOSWAP_QUR:
        return (field_id & 0xFF000000) >> 8 | (field_id & 0x00FF0000) << 8 | (field_id & 0x0000FFFF)
    return field_id


def build_dump_rows(definitions: CustomDefinitions, device_family: int, device_variant: int) -> list[DumpRow]:
    rows = []
    for entry in select_dump_entries(definitions, device_family, device_variant):
        command = entry.command
        where = f"parameter {entry.parameter:g}"
        value_type = VALUE_TYPES.get(command.value_type)
        if value_type is None:
            raise DefinitionsError(f"{where}: value type {command.value_type} is not supported")
        if command.flags & FL_QINF_ONLY:
            raise DefinitionsError(f"{where}: FL_QINF_ONLY (query via QINF) is not supported")
        if command.flags & FL_SPECIAL_INF:
            raise DefinitionsError(f"{where}: FL_SPECIAL_INF is not supported")
        uses_enum = value_type.format in ENUM_FORMATS and command.enum_symbol is not None
        rows.append(
            DumpRow(
                number=entry.parameter,
                field_id=_query_field_id(command.field_id, command.flags),
                value_type=value_type,
                enum_values=definitions.enum_values(command.enum_symbol) if uses_enum else None,
                category=entry.category,
                description=definitions.text(command.description_symbol),
            )
        )
    return rows


def _float_literal(value: float) -> str:
    """Shortest decimal that yields the same float32."""
    target = float32(value)
    digits = next(digits for digits in range(1, 10) if float32(float(f"{target:.{digits}g}")) == target)
    return repr(float(f"{target:.{digits}g}")) + "f"


def _string_literal(data: bytes) -> str:
    """C string literal; non-ASCII bytes as three digit octal escapes, which never merge with
    following characters (unlike hex escapes)."""
    out = []
    for byte in data:
        char = chr(byte)
        if char in '"\\':
            out.append("\\" + char)
        elif 0x20 <= byte < 0x7F:
            out.append(char)
        else:
            out.append(f"\\{byte:03o}")
    return '"' + "".join(out) + '"'


def render_dump_table(name: str, rows: list[DumpRow]) -> str:
    """C++ definitions of `<name>_parameters`, an array of esphome::bsb::DumpParameter."""
    enums = list(dict.fromkeys(row.enum_values for row in rows if row.enum_values is not None))
    lines = []
    for index, values in enumerate(enums):
        if values:
            lines.append(f"static constexpr uint16_t {name}_enum_values_{index}[] = {{{', '.join(map(str, values))}}};")
    if enums:
        enum_items = ", ".join(
            f"{{{name}_enum_values_{index}, {len(values)}}}" if values else "{nullptr, 0}" for index, values in enumerate(enums)
        )
        lines.append(f"static constexpr esphome::bsb::EnumValues {name}_enums[] = {{{enum_items}}};")

    lines.append(f"static constexpr esphome::bsb::DumpParameter {name}_parameters[] = {{")
    for row in rows:
        enum_ref = f"&{name}_enums[{enums.index(row.enum_values)}]" if row.enum_values is not None else "nullptr"
        value_spec = (
            f"{{esphome::bsb::ValueFormat::{row.value_type.format}, {_float_literal(row.value_type.operand)}, "
            f"{row.value_type.precision}, {enum_ref}}}"
        )
        lines.append(
            f"  {{{_float_literal(row.number)}, 0x{row.field_id:08X}, {value_spec}, "
            f"{_string_literal(row.category)}, {_string_literal(row.description)}}},"
        )
    lines.append("};")
    return "\n".join(lines)
