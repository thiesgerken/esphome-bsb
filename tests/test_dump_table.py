"""Tests for the BSB-LAN definitions parser and the dump table codegen.

Run from the repository root: `python3 -m unittest discover tests`.
"""

from pathlib import Path
import sys
import types
import unittest

# Import the codegen modules without the package's __init__.py, which needs ESPHome.
_package = types.ModuleType("bsb")
_package.__path__ = [str(Path(__file__).parents[1] / "components" / "bsb")]
sys.modules["bsb"] = _package

from bsb.bsb_lan_defs import CustomDefinitions, DefinitionsError, float32  # noqa: E402
from bsb.dump_selection import select_dump_entries  # noqa: E402
from bsb.dump_table import build_dump_rows, render_dump_table  # noqa: E402

DEFINITIONS = r"""
// 0/LMS15/123/4
#define E_CAT_00_TEXT "Uhrzeit"
#define E_CAT_01_TEXT "Heizkreis 1"
#define E_CAT_02_TEXT "Nur AGU"
#define E_CAT_03_TEXT "Trinkwasser"
#define S700_0_TEXT "Betriebsart"
#define S701P1_0_TEXT "Tempor\303\244r w\303\244rmer"
#define E700_0_00_TEXT "Schutzbetrieb"
#define E700_0_01_TEXT "Automatik"
#define E1600_0_00_TEXT "Aus"
#define E1600_0_100_TEXT E700_0_01_TEXT

typedef enum{
CAT_0,
CAT_1,
CAT_2,
CAT_3,
CAT_USER_DEFINED,
CAT_UNKNOWN
}category_t;

const float ENUM_CAT_NR[] PROGMEM_LATEST = {
0, 6,
700, 799,
800, 899,
1600, 1699,
10000, 11999,
20000, 20899
};
const char ENUM_CAT[] PROGMEM_LATEST = {
"\x00 " E_CAT_00_TEXT "\0"
"\x01 " E_CAT_01_TEXT "\0"
"\x02\x89\x6D " E_CAT_02_TEXT "\0" // restricted to another device
"\x03 " E_CAT_03_TEXT "\0"
"\x04 " ENUM_CAT_40_TEXT
};

const char E700_0[] PROGMEM_LATEST = {
"\x00 " E700_0_00_TEXT "\0"
"\x01 " E700_0_01_TEXT
};
#define E701P1_0 E700_0
const char E1600_0[] PROGMEM_LATEST = {
"\x00 " E1600_0_00_TEXT "\0"
"\x01\x00 " E1600_0_100_TEXT
};

const char S700_0[] PROGMEM = S700_0_TEXT;
const char S701P1_0[] PROGMEM = S701P1_0_TEXT;
const char S_GENERIC[] PROGMEM = "Generisch";
const char S_RO[] PROGMEM = "Nur lesen";

PROGMEM_LATE const cmd_t cmdtbl[]={
{0x053D000B,  VT_DATETIME,        0,     S_GENERIC, 0,                  NULL,         DEFAULT_FLAG, 255, 255},
{0x2D3D0574,  VT_ENUM,            700,   S700_0,    sizeof(E700_0),     E700_0,       DEFAULT_FLAG, 123, 4},
{0x2D3D0574,  VT_ENUM,            700,   S_GENERIC, sizeof(E700_0),     E700_0,       DEFAULT_FLAG, 255, 255},
{0x053D1A91,  VT_ENUM,            701.1, S701P1_0,  sizeof(E701P1_0),   E701P1_0,     DEFAULT_FLAG, 123, 4},
{0x053D0001,  VT_TEMP,            702,   S_RO,      0,                  NULL,         FL_RONLY,     123, 4},
{0x053D0002,  VT_TEMP,            703,   S_GENERIC, 0,                  NULL,         FL_OEM+FL_NOSWAP_QUR, 123, 4},
{0x053D0003,  VT_TEMP,            704,   S_GENERIC, 0,                  NULL,         DEFAULT_FLAG+FL_WONLY, 123, 4},
{0x053D0004,  VT_TEMP,            705,   S_GENERIC, 0,                  NULL,         FL_NO_CMD,    123, 4},
{0x053D0005,  VT_TEMP,            706,   S_GENERIC, 0,                  NULL,         DEFAULT_FLAG, 137, 109},
{0x053D0006,  VT_TEMP,            850,   S_GENERIC, 0,                  NULL,         DEFAULT_FLAG, 123, 4},
{0x313D0571,  VT_ENUM,            1600,  S_GENERIC, sizeof(E1600_0),    E1600_0,      DEFAULT_FLAG, 123, 4},
{0x313D0572,  VT_ONOFF,           1601,  S_GENERIC, sizeof(ENUM_ONOFF), ENUM_ONOFF,   DEFAULT_FLAG, 123, 4},
{0x053D058C,  VT_TEMP,            10023, S_GENERIC, 0,                  NULL,         DEFAULT_FLAG, 123, 4},
"""


def dump_rows(definitions=DEFINITIONS):
    return build_dump_rows(CustomDefinitions(definitions), 123, 4)


class SelectionTest(unittest.TestCase):
    def setUp(self):
        self.rows = dump_rows()
        self.by_number = {row.number: row for row in self.rows}

    def test_selects_writable_parameters_of_the_device_in_order(self):
        # 0: date/time category, 702: read-only, 704: write-only, 705: no command,
        # 706: other device, 850: category of another device.
        self.assertEqual([row.number for row in self.rows], [700, float32(701.1), 703, 1600, 1601, 10023])

    def test_prefers_rows_of_the_device_family(self):
        self.assertEqual(self.by_number[700].description, b"Betriebsart")

    def test_resolves_texts_and_categories(self):
        self.assertEqual(self.by_number[float32(701.1)].description, "Temporär wärmer".encode())
        self.assertEqual(self.by_number[float32(701.1)].category, b"Heizkreis 1")
        self.assertEqual(self.by_number[10023].category, b"Benutzerdefiniert")

    def test_resolves_enum_values_including_aliases_and_two_byte_values(self):
        self.assertEqual(self.by_number[float32(701.1)].enum_values, (0, 1))
        self.assertEqual(self.by_number[1600].enum_values, (0, 0x100))
        self.assertEqual(self.by_number[1601].enum_values, (0, 1))
        self.assertIsNone(self.by_number[703].enum_values)

    def test_swaps_field_id_for_noswap_parameters(self):
        self.assertEqual(self.by_number[703].field_id, 0x3D050002)
        self.assertEqual(self.by_number[700].field_id, 0x2D3D0574)

    def test_rejects_unsupported_value_types(self):
        unsupported = DEFINITIONS.replace("VT_TEMP,            850", "VT_CUSTOM_BYTE,     850")
        unsupported = unsupported.replace('"\\x02\\x89\\x6D "', '"\\x02 "')
        with self.assertRaisesRegex(DefinitionsError, "VT_CUSTOM_BYTE"):
            dump_rows(unsupported)

    def test_rejects_qinf_parameters(self):
        with self.assertRaisesRegex(DefinitionsError, "FL_QINF_ONLY"):
            dump_rows(DEFINITIONS.replace("FL_OEM+FL_NOSWAP_QUR", "FL_QINF_ONLY"))

    def test_rejects_unknown_device(self):
        with self.assertRaisesRegex(DefinitionsError, "device family 99"):
            build_dump_rows(CustomDefinitions(DEFINITIONS), 99, 4)

    def test_rejects_undefined_symbols(self):
        with self.assertRaisesRegex(DefinitionsError, "S_MISSING"):
            dump_rows(DEFINITIONS.replace("S700_0,    sizeof", "S_MISSING, sizeof"))


class RenderTest(unittest.TestCase):
    def test_renders_parameters_with_escaped_utf8(self):
        table = render_dump_table("dump", dump_rows())
        self.assertIn("static constexpr uint16_t dump_enum_values_0[] = {0, 1};", table)
        self.assertIn("static constexpr uint16_t dump_enum_values_1[] = {0, 256};", table)
        self.assertIn(
            '  {701.1f, 0x053D1A91, {esphome::bsb::ValueFormat::Enum, 1.0f, 0, &dump_enums[0]}, '
            '"Heizkreis 1", "Tempor\\303\\244r w\\303\\244rmer"},',
            table,
        )
        self.assertIn('  {10023.0f, 0x053D058C, {esphome::bsb::ValueFormat::FixPoint, 64.0f, 1, nullptr}, ', table)


class LmsDefinitionsTest(unittest.TestCase):
    """Against an excerpt of the real definitions of an LMS15 (family 123, variant 4)."""

    @classmethod
    def setUpClass(cls):
        text = (Path(__file__).parent / "fixtures" / "BSB_LAN_custom_defs.h").read_text(encoding="utf-8")
        cls.definitions = CustomDefinitions(text)
        cls.rows = build_dump_rows(cls.definitions, 123, 4)
        cls.by_number = {row.number: row for row in cls.rows}

    def test_selects_writable_parameters_of_the_device_in_order(self):
        self.assertEqual(len(self.definitions.commands), 44)
        self.assertEqual(sum(command.is_writable for command in self.definitions.commands), 33)
        # Minus date/time (0-3) and the generic duplicates of 700, 1000, 1300 and 1600.
        self.assertEqual(
            [row.number for row in self.rows],
            [501, 516, 642, 648, 700, 701, float32(701.1), float32(701.2), 710, 726, 790, 832, 881, 1000, 1300,
             1600, 1610, 1644, 2479, 6141, 7042, 7170, 7400, 10023, 10046],
        )

    def test_prefers_rows_of_the_device_family(self):
        entries = {entry.parameter: entry for entry in select_dump_entries(self.definitions, 123, 4)}
        for number in (700, 1000, 1300, 1600):
            self.assertEqual(entries[number].command.device_family, 123)

    def test_known_parameters(self):
        self.assertEqual(self.by_number[501].value_type.format, "TimeProgram")
        self.assertEqual(self.by_number[501].category, b"Zeitschaltprogramm 1")
        self.assertEqual(self.by_number[700].description, b"Betriebsart")
        self.assertEqual(self.by_number[700].enum_values, (0, 1, 2, 3))
        self.assertEqual(self.by_number[710].description, b"Komfortsollwert")
        self.assertEqual(self.by_number[710].value_type.format, "FixPoint")
        self.assertEqual(self.by_number[float32(701.1)].description, "Temporär wärmer".encode())
        self.assertEqual(self.by_number[float32(701.1)].enum_values, (0, 1, 2))
        self.assertEqual(self.by_number[10023].category, b"Benutzerdefiniert")
        self.assertEqual(self.by_number[10046].description, b"Raumtemperatur-Frostschutzsollwert")

    def test_covers_every_value_format_outside_the_date_time_category(self):
        # DateTime, Year and TimeOfDay only occur in the skipped date/time category (0-3).
        self.assertEqual(
            {row.value_type.format for row in self.rows},
            {"Byte", "Word", "DWord", "SignedWord", "FixPointByte", "FixPointUnsignedByte", "FixPoint", "Choice", "Enum",
             "DayMonth", "HourMinutes", "TimeProgram", "String"},
        )


if __name__ == "__main__":
    unittest.main()
