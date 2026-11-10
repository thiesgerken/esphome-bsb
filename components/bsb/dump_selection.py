"""Port of the parameter selection of BSB-LAN's JSON backup endpoint (`/JB`): which
parameters it queries for a device, in which order, and under which category.

Self-contained (no ESPHome imports), so it can be tested on its own. Parameter numbers are
float32 like in BSB-LAN, which makes the traversal match exactly.
"""

from __future__ import annotations

from dataclasses import dataclass

from .bsb_lan_defs import FL_NO_CMD, Command, CustomDefinitions, DefinitionsError, float32

CMD_UNKNOWN = 0x00000000

DEV_ALL = 255

# First virtual (ESP-internal) parameter number in BSB-LAN. Its parameters are not
# controller parameters and BSB-LAN never backs them up.
BSP_INTERNAL = 20000


def _roundf_tenths(value: float) -> int:
    """C `roundf(value * 10)` on floats (rounds halfway cases away from zero)."""
    scaled = float32(value * 10)
    return int(scaled + 0.5) if scaled >= 0 else -int(-scaled + 0.5)


@dataclass(frozen=True)
class DumpEntry:
    parameter: float
    command: Command
    category: bytes


class _CommandTable:
    """Port of BSB-LAN's findLine() and get_next_prognr() for one device."""

    def __init__(self, commands: list[Command], device_family: int, device_variant: int) -> None:
        self._commands = commands
        self._family = device_family
        self._variant = device_variant

    def find(self, parameter: float) -> int | None:
        rows = self._commands
        wanted = _roundf_tenths(parameter)
        i = next((j for j in range(len(rows)) if _roundf_tenths(rows[j].parameter) == wanted), None)
        if i is None:
            return None
        found = False
        best = 0
        while i < len(rows) and rows[i].parameter == parameter:
            row = rows[i]
            family_matches = row.device_family in (self._family, DEV_ALL)
            variant_matches = row.device_variant in (self._variant, DEV_ALL)
            if family_matches and variant_matches:
                exact = row.device_family == self._family and row.device_variant == self._variant
                if exact or (not found and row.device_family != self._family) or row.device_family == self._family:
                    if row.flags & FL_NO_CMD:
                        i = self._last_row_with_same_field_id(i)
                        found = False
                    else:
                        found = True
                        best = i
                        if exact:
                            break
            i += 1
        return best if found else None

    def _last_row_with_same_field_id(self, i: int) -> int:
        field_id = self._commands[i].field_id
        while i + 1 < len(self._commands) and self._commands[i + 1].field_id == field_id:
            i += 1
        return i

    def next_parameter(self, parameter: float) -> float | None:
        start = self.find(parameter)
        if start is None:
            return float(int(parameter) + 1)
        current = self._commands[start].parameter
        for row in self._commands[start + 1 :]:
            if row.parameter != current:
                return row.parameter
        return None


def select_dump_entries(definitions: CustomDefinitions, device_family: int, device_variant: int) -> list[DumpEntry]:
    """The parameters BSB-LAN's `/JB` queries, in its order (before dropping failed queries)."""
    if any(command.parameter >= BSP_INTERNAL for command in definitions.commands):
        raise DefinitionsError(f"parameters >= {BSP_INTERNAL} are BSB-LAN virtual parameters and not supported")
    if not any((command.device_family, command.device_variant) == (device_family, device_variant) for command in definitions.commands):
        raise DefinitionsError(f"no parameters for device family {device_family}, variant {device_variant}")
    table = _CommandTable(definitions.commands, device_family, device_variant)
    categories = [
        (definitions.category(number, device_family, device_variant), *definitions.category_ranges[number])
        for number in range(definitions.unknown_category)
    ]
    valid_categories = [(name, low, high) for name, low, high in categories if name is not None]

    def category_of(command: Command) -> bytes:
        # BSB-LAN labels a parameter with the first valid category containing it
        # (get_cmdtbl_category()), not with the category being iterated.
        return next(name for name, low, high in valid_categories if low <= command.parameter < float32(high + 1))

    entries = []
    for name, low, high in categories[1:]:  # BSB-LAN skips category 0 (date/time)
        if name is None:
            continue
        parameter: float | None = low
        while parameter is not None and low <= parameter <= high:
            row = table.find(parameter)
            if row is not None:
                command = definitions.commands[row]
                if command.field_id != CMD_UNKNOWN and command.is_writable:
                    entries.append(DumpEntry(parameter, command, category_of(command)))
            parameter = table.next_parameter(parameter)
    return entries
