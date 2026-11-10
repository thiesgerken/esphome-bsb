"""How BSB-LAN decodes each value type (VT_*) for display: the decoder printTelegram()
dispatches to, plus operand and precision from BSB-LAN's `optbl` (BSB_LAN_defs.h).

Only value types with a decoder ported to `bsbValueDecoder.cpp` are listed. The format
names match the C++ `ValueFormat` enum.
"""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class ValueType:
    format: str
    operand: float = 1.0  # divisor, or factor for SignedWord (BSB-LAN's optbl operand)
    precision: int = 0  # decimal places of fixpoint formats


def _family(value_format: str, types: dict[str, tuple[float, int]]) -> dict[str, ValueType]:
    return {name: ValueType(value_format, operand, precision) for name, (operand, precision) in types.items()}


def _plain(value_format: str, *names: str) -> dict[str, ValueType]:
    return {name: ValueType(value_format) for name in names}


VALUE_TYPES: dict[str, ValueType] = {
    # printBYTE()
    **_plain(
        "Byte",
        "VT_LITER", "VT_POWER_SHORT", "VT_POWER_SHORT_N", "VT_MONTHS", "VT_MONTHS_N", "VT_DAYS",
        "VT_GRADIENT_SHORT", "VT_HOURS_SHORT", "VT_HOURS_SHORT_N", "VT_MINUTES_SHORT", "VT_MINUTES_SHORT_N",
        "VT_SECONDS_SHORT", "VT_SECONDS_SHORT_N", "VT_PERCENT", "VT_PERCENT_NN", "VT_PERCENT1", "VT_BYTE",
        "VT_BYTE_N",
    ),
    # printWORD()
    **_family(
        "Word",
        {
            "VT_MONTHS_WORD": (1.0, 0), "VT_DAYS_WORD": (1.0, 0), "VT_HOURS_WORD": (1.0, 0),
            "VT_HOURS_WORD_N": (1.0, 0), "VT_MINUTES_WORD": (1.0, 0), "VT_MINUTES_WORD_N": (1.0, 0),
            "VT_MINUTES_WORD10": (0.1, 0), "VT_MSECONDS_WORD": (1.0, 0), "VT_MSECONDS_WORD_N": (1.0, 0),
            "VT_SECONDS_WORD": (1.0, 0), "VT_SECONDS_WORD_N": (1.0, 0), "VT_GRADIENT": (1.0, 0),
            "VT_UINT": (1.0, 0), "VT_UINT_N": (1.0, 0),
        },
    ),
    # printDWORD()
    **_family(
        "DWord",
        {
            "VT_MINUTES": (60.0, 0), "VT_HOURS": (3600.0, 0), "VT_HOURS_N": (3600.0, 0),
            "VT_SECONDS_DWORD": (1.0, 0), "VT_TEMP_DWORD": (64.0, 1), "VT_ENERGY": (1.0, 0),
            "VT_ENERGY_N": (1.0, 0), "VT_ENERGY10": (10.0, 1), "VT_ENERGY10_N": (10.0, 1),
            "VT_ENERGY_MWH": (100.0, 0), "VT_ENERGY_MWH_N": (100.0, 0), "VT_POWER": (10.0, 1),
            "VT_POWER100": (100.0, 2), "VT_CUBICMETER": (10.0, 1), "VT_CUBICMETER_N": (10.0, 1),
            "VT_UINT100": (100.0, 2), "VT_DWORD": (1.0, 0), "VT_DWORD_N": (1.0, 0), "VT_DWORD10": (10.0, 0),
        },
    ),
    # printSINT()
    **_plain("SignedWord", "VT_SINT", "VT_SINT_NN"),
    # printFIXPOINT_BYTE()
    **_family(
        "FixPointByte",
        {
            "VT_SECONDS_SHORT2": (2.0, 1), "VT_SECONDS_SHORT2_N": (2.0, 1), "VT_SECONDS_SHORT4": (4.0, 1),
            "VT_SECONDS_SHORT5": (5.0, 1), "VT_TEMP_SHORT64": (64.0, 5), "VT_TEMP_SHORT5": (2.0, 1),
            "VT_TEMP_SHORT5_N": (2.0, 1), "VT_TEMP_SHORT": (1.0, 0), "VT_TEMP_PER_MIN": (1.0, 0),
            "VT_VOLTAGE": (10.0, 1), "VT_VOLTAGE_N": (10.0, 1),
        },
    ),
    # printFIXPOINT_BYTE_US()
    **_family(
        "FixPointUnsignedByte",
        {
            "VT_BYTE5_N": (5.0, 0), "VT_BYTE10": (10.0, 1), "VT_BYTE10_N": (10.0, 1), "VT_LPM_SHORT": (10.0, 1),
            "VT_PRESSURE": (10.0, 1), "VT_PRESSURE_NN": (10.0, 1), "VT_PRESSURE50": (50.0, 2),
            "VT_PERCENT5": (2.0, 0), "VT_TEMP_SHORT5_US": (2.0, 1), "VT_TEMP_SHORT_US": (1.0, 0),
            "VT_TEMP_SHORT_US_N": (1.0, 0),
        },
    ),
    # printFIXPOINT()
    **_family(
        "FixPoint",
        {
            "VT_TEMP": (64.0, 1), "VT_TEMP_N": (64.0, 1), "VT_SECONDS_WORD2": (5.0, 0),
            "VT_SECONDS_WORD2_N": (5.0, 0), "VT_SECONDS_WORD4": (4.0, 0), "VT_SECONDS_WORD4_N": (4.0, 0),
            "VT_SECONDS_WORD5": (2.0, 0), "VT_SECONDS_WORD5_N": (2.0, 0), "VT_SECONDS_WORD16": (16.0, 0),
            "VT_TEMP_WORD": (1.0, 1), "VT_TEMP_WORD5_US": (2.0, 1), "VT_VOLTAGE_WORD": (10.0, 1),
            "VT_VOLTAGE_WORD1": (1.0, 1), "VT_CELMIN": (1.0, 1), "VT_CELMIN_N": (1.0, 1),
            "VT_LITERPERHOUR": (1.0, 0), "VT_LITERPERHOUR_N": (1.0, 0), "VT_LITERPERHOUR100": (100.0, 0),
            "VT_LITERPERHOUR100_N": (100.0, 0), "VT_LITERPERMIN": (10.0, 1), "VT_LITERPERMIN_N": (10.0, 1),
            "VT_PRESSURE_WORD": (10.0, 1), "VT_PRESSURE_WORD_NN": (10.0, 1), "VT_PRESSURE_1000": (1000.0, 1),
            "VT_POWER_W": (1.0, 1), "VT_POWER_W_N": (1.0, 1), "VT_POWER_WORD": (10.0, 1),
            "VT_POWER_WORD_N": (10.0, 1), "VT_POWER_WORD100": (100.0, 2), "VT_ENERGY_WORD": (10.0, 1),
            "VT_ENERGY_WORD_N": (10.0, 1), "VT_SPF": (100.0, 2), "VT_ENERGY_CONTENT": (10.0, 1),
            "VT_CURRENT": (100.0, 2), "VT_CURRENT1000": (1000.0, 2), "VT_PROPVAL": (16.0, 2),
            "VT_SPEED": (0.02, 0), "VT_SPEED2": (1.0, 0), "VT_SPEED2_N": (1.0, 0), "VT_FREQ": (20.0, 1),
            "VT_FREQ10": (10.0, 0), "VT_FP1": (10.0, 1), "VT_FP02": (50.0, 2), "VT_METER": (10.0, 0),
            "VT_PERCENT_WORD1": (1.0, 1), "VT_PERCENT_WORD": (2.0, 1), "VT_PERCENT_100": (100.0, 1),
            "VT_SINT1000": (1000.0, 3), "VT_UINT100_WORD": (100.0, 2), "VT_UINT100_WORD_N": (100.0, 2),
            "VT_UINT2_N": (5.0, 1), "VT_UINT5": (0.2, 0), "VT_UINT10": (10.0, 1), "VT_AMP": (100.0, 2),
        },
    ),
    # printCHOICE()
    **_plain("Choice", "VT_BINARY_ENUM", "VT_ONOFF", "VT_YESNO", "VT_CLOSEDOPEN", "VT_VOLTAGEONOFF"),
    # printENUM() via the VT_ENUM case
    **_plain("Enum", "VT_ENUM", "VT_WEEKDAY"),
    # printDateTime(), one format per value type it distinguishes
    "VT_DATETIME": ValueType("DateTime"),
    "VT_YEAR": ValueType("Year"),
    "VT_DAYMONTH": ValueType("DayMonth"),
    "VT_VACATIONPROG": ValueType("DayMonth"),
    "VT_TIME": ValueType("TimeOfDay"),
    # printTime()
    **_plain("HourMinutes", "VT_HOUR_MINUTES", "VT_HOUR_MINUTES_N"),
    # printTimeProg()
    "VT_TIMEPROG": ValueType("TimeProgram"),
    "VT_STRING": ValueType("String"),
}
