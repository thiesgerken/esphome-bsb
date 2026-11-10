// Excerpt of the controller specific BSB_LAN_custom_defs.h that BSB-LAN
// (https://github.com/fredlcore/BSB-LAN) generated for an LMS15.000A349 (device family 123,
// variant 4). BSB-LAN and this excerpt are licensed under CC BY-NC-SA 4.0
// (https://creativecommons.org/licenses/by-nc-sa/4.0/); esphome-bsb itself has no license file.
// Cut by tests/fixtures/trim_custom_defs.py: a selection of command table rows plus everything
// they reference, all lines verbatim.
// 0/LMS15.000A349/123/4/0.7/211980093 4/AGU2.550A109/137/109/1/2203010076 10/QAA75.910/349/133/231/101/324296
#define E_CAT_00_TEXT "Uhrzeit"
#define S516_0_TEXT "Standardwerte ZSP"
#define S642_0_TEXT "Urlaubsbeginn"
#define S648_0_TEXT "Betriebsniveau bei Urlaub"
#define S700_0_TEXT "Betriebsart"
#define S701_0_TEXT "Wärmer / Kälter"
#define S701P1_0_TEXT "Temporär wärmer"
#define S701P2_0_TEXT "Temporär kälter"
#define S710_0_TEXT "Komfortsollwert"
#define S714_0_TEXT "Raumtemperatur-Frostschutzsollwert"
#define S726_0_TEXT "Heizkennlinien-Adaption"
#define S790_0_TEXT "Einschaltzeitoptimierung maximale Vorverlegung"
#define S832_0_TEXT "Antrieb-Regelungsart Heizkreis"
#define S881_0_TEXT "Anlaufdrehzahl"
#define S1610_0_TEXT "Trinkwassertemperatur-Nennsollwert"
#define S1644_0_TEXT "Zeitpunkt für Legionellenfunktion"
#define S2479_0_TEXT "Abgasüberwachung Zeitkonst Modulationsfreigabe"
#define S6141_0_TEXT "Wasserdruck Minimum"
#define S6605_0_TEXT "LPB-Speisung Status"
#define S6650_0_TEXT "Aussentemperatur-Lieferant"
#define S7042_0_TEXT "Brennerstarts Wartungsintervall"
#define S7170_0_TEXT "Telefon Kundendienst"
#define S7400_0_TEXT "Funktionswert 1 H2/H21 Modul 2"
#define S8700_0_TEXT "Aussentemperatur"
#define S6700_0_TEXT "Fehlermeldung"
#define S10021_0_TEXT "Wartungsmeldung"
#define S10023_0_TEXT "Raumtemperatur Minimum"
#define E_CAT_01_TEXT "Zeitschaltprogramm 1"
#define E_CAT_02_TEXT "Zeitschaltprogramm 2"
#define E_CAT_03_TEXT "Zeitschaltprogramm 3"
#define E_CAT_04_TEXT "Zeitschaltprogramm 4"
#define E_CAT_05_TEXT "Zeitschaltprogramm 5"
#define E_CAT_06_TEXT "Ferienprogramme HK1"
#define E_CAT_07_TEXT "Ferienprogramme HK2"
#define E_CAT_08_TEXT "Ferienprogramme HK3"
#define E_CAT_09_TEXT "Heizkreis 1"
#define E_CAT_0A_TEXT "Kühlkreis 1"
#define E_CAT_0B_TEXT "Heizkreis 2"
#define E_CAT_0C_TEXT "Heizkreis 3"
#define E_CAT_0D_TEXT "Trinkwasser"
#define E_CAT_0E_TEXT "Verbraucherkreis 1"
#define E_CAT_0F_TEXT "Verbraucherkreis 2"
#define E_CAT_10_TEXT "Schwimmbadkreis"
#define E_CAT_11_TEXT "Schwimmbad"
#define E_CAT_12_TEXT "Vorregler/Zubringerpumpe"
#define E_CAT_13_TEXT "Kessel"
#define E_CAT_14_TEXT "Sitherm Pro"
#define E_CAT_15_TEXT "Kaskade"
#define E_CAT_16_TEXT "Zusatzerzeuger"
#define E_CAT_17_TEXT "Solar"
#define E_CAT_18_TEXT "Feststoffkessel"
#define E_CAT_19_TEXT "Pufferspeicher"
#define E_CAT_1A_TEXT "TWW-Speicher"
#define E_CAT_1B_TEXT "TWW-Durchlauferhitzer"
#define E_CAT_1C_TEXT "Allgemeine Funktionen"
#define E_CAT_1D_TEXT "Konfiguration"
#define E_CAT_1E_TEXT "LPB"
#define E_CAT_1F_TEXT "Modbus"
#define E_CAT_20_TEXT "Fehler"
#define E_CAT_21_TEXT "Wartung/Sonderbetrieb"
#define E_CAT_22_TEXT "Konfiguration Erweiterungsmodule"
#define E_CAT_23_TEXT "IO-Test"
#define E_CAT_24_TEXT "Status"
#define E_CAT_25_TEXT "Diagnose Kaskade"
#define E_CAT_26_TEXT "Diagnose Erzeuger"
#define E_CAT_27_TEXT "Diagnose Verbraucher"
#define E_CAT_28_TEXT "Feuerungsautomat"

#define E648_0_00_TEXT "Frost"
#define E648_0_01_TEXT "Reduziert"
#define E648_0_02_TEXT "Nenn"
#define E700_0_00_TEXT "Schutzbetrieb"
#define E700_0_01_TEXT "Automatik"
#define E700_0_03_TEXT "Komfort"
#define E701_0_00_TEXT "Kälter"
#define E701_0_01_TEXT "…"
#define E701_0_02_TEXT "Wärmer"
#define E780_0_00_TEXT "Aus"
#define E832_0_00_TEXT "2-Punkt"
#define E832_0_01_TEXT "3-Punkt"
#define E1600_0_01_TEXT "Ein"
#define E1600_0_02_TEXT "Eco"

#define E700_0_02_TEXT E648_0_01_TEXT
#define E1600_0_00_TEXT E780_0_00_TEXT

typedef enum{
CAT_0,
CAT_1,
CAT_2,
CAT_3,
CAT_4,
CAT_5,
CAT_6,
CAT_7,
CAT_8,
CAT_9,
CAT_10,
CAT_11,
CAT_12,
CAT_13,
CAT_14,
CAT_15,
CAT_16,
CAT_17,
CAT_18,
CAT_19,
CAT_20,
CAT_21,
CAT_22,
CAT_23,
CAT_24,
CAT_25,
CAT_26,
CAT_27,
CAT_28,
CAT_29,
CAT_30,
CAT_31,
CAT_32,
CAT_33,
CAT_34,
CAT_35,
CAT_36,
CAT_37,
CAT_38,
CAT_39,
CAT_40,
CAT_USER_DEFINED,
CAT_PPS,
CAT_USERSENSORS,
CAT_UNKNOWN
}category_t;

const float ENUM_CAT_NR[] PROGMEM_LATEST = {
0, 6,
501, 516,
521, 536,
541, 556,
561, 576,
601, 616,
642, 648,
652, 658,
662, 668,
700, 900,
901, 932,
1000, 1200,
1300, 1500,
1600, 1680,
1859, 1880,
1909, 1930,
1959, 1980,
2055, 2080,
2110, 2150,
2203, 2682,
2700, 2753,
3510, 3590,
3690, 3690,
3810, 3887,
4101, 4184,
4720, 4813,
5010, 5151,
5420, 5550,
5570, 5587,
5700, 6359,
6600, 6650,
6651, 6654,
6704, 6999,
7040, 7254,
7300, 7507,
7700, 7952,
8000, 8023,
8100, 8151,
8304, 8570,
8700, 9058,
9500, 9652,
10000, 11999,
15000, 15099,
20000, 20899
};
const char ENUM_CAT[] PROGMEM_LATEST = {
"\x00 " E_CAT_00_TEXT "\0"
"\x01 " E_CAT_01_TEXT "\0" // Zeitschaltprogramm 1
"\x02 " E_CAT_02_TEXT "\0" // Zeitschaltprogramm 2
"\x03 " E_CAT_03_TEXT "\0" // Zeitschaltprogramm 3
"\x04 " E_CAT_04_TEXT "\0" // Zeitschaltprogramm 4
"\x05 " E_CAT_05_TEXT "\0" // Zeitschaltprogramm 5
"\x06 " E_CAT_06_TEXT "\0" // Ferienprogramme HK1
"\x07 " E_CAT_07_TEXT "\0" // Ferienprogramme HK2
"\x08 " E_CAT_08_TEXT "\0" // Ferienprogramme HK3
"\x09 " E_CAT_09_TEXT "\0" // Heizkreis 1
"\x0A " E_CAT_0A_TEXT "\0" // Kühlkreis 1
"\x0B " E_CAT_0B_TEXT "\0" // Heizkreis 2
"\x0C " E_CAT_0C_TEXT "\0" // Heizkreis 3
"\x0D " E_CAT_0D_TEXT "\0" // Trinkwasser
"\x0E " E_CAT_0E_TEXT "\0" // Verbraucherkreis 1
"\x0F " E_CAT_0F_TEXT "\0" // Verbraucherkreis 2
"\x10 " E_CAT_10_TEXT "\0" // Schwimmbadkreis
"\x11 " E_CAT_11_TEXT "\0" // Schwimmbad
"\x12 " E_CAT_12_TEXT "\0" // Vorregler/Zubringerpumpe
"\x13 " E_CAT_13_TEXT "\0" // Kessel
"\x14 " E_CAT_14_TEXT "\0" // Sitherm Pro
"\x15 " E_CAT_15_TEXT "\0" // Kaskade
"\x16 " E_CAT_16_TEXT "\0" // Zusatzerzeuger
"\x17 " E_CAT_17_TEXT "\0" // Solar
"\x18 " E_CAT_18_TEXT "\0" // Feststoffkessel
"\x19 " E_CAT_19_TEXT "\0" // Pufferspeicher
"\x1A " E_CAT_1A_TEXT "\0" // TWW-Speicher
"\x1B " E_CAT_1B_TEXT "\0" // TWW-Durchlauferhitzer
"\x1C " E_CAT_1C_TEXT "\0" // Allgemeine Funktionen
"\x1D " E_CAT_1D_TEXT "\0" // Konfiguration
"\x1E " E_CAT_1E_TEXT "\0" // LPB
"\x1F " E_CAT_1F_TEXT "\0" // Modbus
"\x20 " E_CAT_20_TEXT "\0" // Fehler
"\x21 " E_CAT_21_TEXT "\0" // Wartung/Sonderbetrieb
"\x22 " E_CAT_22_TEXT "\0" // Konfiguration Erweiterungsmodule
"\x23 " E_CAT_23_TEXT "\0" // IO-Test
"\x24 " E_CAT_24_TEXT "\0" // Status
"\x25 " E_CAT_25_TEXT "\0" // Diagnose Kaskade
"\x26 " E_CAT_26_TEXT "\0" // Diagnose Erzeuger
"\x27 " E_CAT_27_TEXT "\0" // Diagnose Verbraucher
"\x28 " E_CAT_28_TEXT "\0" // Feuerungsautomat
"\x29 " ENUM_CAT_40_TEXT "\0"
"\x2A " ENUM_CAT_43_TEXT "\0"
"\x2B " ENUM_CAT_44_TEXT "\0"
"\x45 " ENUM_CAT_45_TEXT
};

// 648 - Betriebsniveau bei Urlaub
const char E648_0[] PROGMEM_LATEST = {
"\x00 " E648_0_00_TEXT "\0" // Frost
"\x01 " E648_0_01_TEXT "\0" // Reduziert
"\x02 " E648_0_02_TEXT // Nenn
};

// 700 - Betriebsart
const char E700_0[] PROGMEM_LATEST = {
"\x00 " E700_0_00_TEXT "\0" // Schutzbetrieb
"\x01 " E700_0_01_TEXT "\0" // Automatik
"\x02 " E700_0_02_TEXT "\0" // Reduziert
"\x03 " E700_0_03_TEXT // Komfort
};

// 701 - Wärmer / Kälter
const char E701_0[] PROGMEM_LATEST = {
"\x00 " E701_0_00_TEXT "\0" // Kälter
"\x01 " E701_0_01_TEXT "\0" // …
"\x02 " E701_0_02_TEXT // Wärmer
};

#define E701P1_0 E701_0
#define E701P2_0 E701_0

// 832 - Antrieb-Regelungsart Heizkreis
const char E832_0[] PROGMEM_LATEST = {
"\x00 " E832_0_00_TEXT "\0" // 2-Punkt
"\x01 " E832_0_01_TEXT // 3-Punkt
};

#define E1000_0 E700_0
#define E1300_0 E700_0
// 1600 - Betriebsart
const char E1600_0[] PROGMEM_LATEST = {
"\x00 " E1600_0_00_TEXT "\0" // Aus
"\x01 " E1600_0_01_TEXT "\0" // Ein
"\x02 " E1600_0_02_TEXT // Eco
};

const char E_BA_HK[] PROGMEM_LATEST = {
"\x00 Schutzbetrieb" "\0"
"\x01 Automatik" "\0"
"\x02 Reduziert" "\0"
"\x03 Komfort"
};

const char E_BA_TW[] PROGMEM_LATEST = {
"\x00 Aus" "\0"
"\x01 Ein" "\0"
"\x02 Eco"
};

const char E_BA_TEMP[] PROGMEM_LATEST = {
"\x01 Reduziert\0"
"\x02 Komfort"
};

const char S516_0[] PROGMEM = S516_0_TEXT;  // Standardwerte ZSP
const char S642_0[] PROGMEM = S642_0_TEXT;  // Urlaubbeginn
const char S648_0[] PROGMEM = S648_0_TEXT;  // Betriebsniveau bei Urlaub
const char S700_0[] PROGMEM = S700_0_TEXT;  // Betriebsart
const char S701_0[] PROGMEM = S701_0_TEXT;  // Wärmer / Kälter
const char S701P1_0[] PROGMEM = S701P1_0_TEXT;  // Temporär wärmer
const char S701P2_0[] PROGMEM = S701P2_0_TEXT;  // Temporär kälter
const char S710_0[] PROGMEM = S710_0_TEXT;  // Komfortsollwert
const char S714_0[] PROGMEM = S714_0_TEXT;  // Raumtemperatur-Frostschutzsollwert
const char S726_0[] PROGMEM = S726_0_TEXT;  // Heizkennlinien-Adaption
const char S790_0[] PROGMEM = S790_0_TEXT;  // Einschaltzeitoptimierung maximale Vorverlegung
const char S832_0[] PROGMEM = S832_0_TEXT;  // Antrieb-Regelungsart Heizkreis
const char S881_0[] PROGMEM = S881_0_TEXT;  // Anlaufdrehzahl
#define S1000_0 S700_0                       // Betriebsart
#define S1300_0 S700_0                       // Betriebsart
#define S1600_0 S700_0                       // Betriebsart
const char S1610_0[] PROGMEM = S1610_0_TEXT;  // Trinkwassertemperatur-Nennsollwert
const char S1644_0[] PROGMEM = S1644_0_TEXT;  // Zeitpunkt für Legionellenfunktion
const char S2479_0[] PROGMEM = S2479_0_TEXT;  // Abgasüberwachung Zeitkonst Modulationsfreigabe
const char S6141_0[] PROGMEM = S6141_0_TEXT;  // Wasserdruck Minimum
const char S6605_0[] PROGMEM = S6605_0_TEXT;  // LPB-Speisung Status
const char S6650_0[] PROGMEM = S6650_0_TEXT;  // Aussentemperatur-Lieferant
const char S7042_0[] PROGMEM = S7042_0_TEXT;  // Brennerstarts Wartungsintervall
const char S7170_0[] PROGMEM = S7170_0_TEXT;  // Telefon Kundendienst
const char S7400_0[] PROGMEM = S7400_0_TEXT;  // Funktionswert 1 H2/H21 Modul 2
const char S8700_0[] PROGMEM = S8700_0_TEXT;  // Aussentemperatur
const char S6700_0[] PROGMEM = S6700_0_TEXT;  // Fehlermeldung
const char S10021_0[] PROGMEM = S10021_0_TEXT;  // Wartungsmeldung
const char S10023_0[] PROGMEM = S10023_0_TEXT;  // Raumtemperatur Minimum
#define S10046_0 S714_0                       // Raumtemperatur-Frostschutzsollwert
const char S_WD_1[] PROGMEM = "Montag";
const char S0[] PROGMEM = "Aktuelles Datum / Aktuelle Uhrzeit";
const char S1[] PROGMEM = "Uhrzeit";
const char S2[] PROGMEM = "Datum 1";
const char S3[] PROGMEM = "Datum 2";
const char S10000[] PROGMEM = "Raumtemperatur Heizkreis 1";
const char S10019[] PROGMEM = "Trinkwasser Push";
const char S_BATHK1[] PROGMEM = "Heizbetrieb Heizkreis 1 (Temporär)";
const char S_BA_TEXT[] PROGMEM = "Betriebsart";
const char STR_GI[] PROGMEM = "Geräte-Identifikation";
const char STR_GF[] PROGMEM = "Geräte-Familie";

PROGMEM_LATE const cmd_t cmdtbl[]={
{0x053D000B,  VT_DATETIME,        0,     S0,       0,                  NULL,         DEFAULT_FLAG, 255, 255},
{0x053D000B,  VT_TIME,            1,     S1,       0,                  NULL,         DEFAULT_FLAG, 255, 255},
{0x053D000B,  VT_DAYMONTH,        2,     S2,       0,                  NULL,         DEFAULT_FLAG, 255, 255},
{0x053D000B,  VT_YEAR,            3,     S3,       0,                  NULL,         DEFAULT_FLAG, 255, 255},
{0x053D0A8C,  VT_TIMEPROG,        501,   S_WD_1,   0,                  NULL,         DEFAULT_FLAG, 123, 4},
{0x053D05B2,  VT_YESNO,           516,   S516_0,   sizeof(ENUM_YESNO), ENUM_YESNO,   DEFAULT_FLAG, 123, 4}, // Standardwerte ZSP
{0x053D09C4,  VT_VACATIONPROG,    642,   S642_0,   0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Urlaubbeginn
{0x2D3D04C2,  VT_ENUM,            648,   S648_0,   sizeof(E648_0),     E648_0,       DEFAULT_FLAG, 123, 4}, // Betriebsniveau bei Urlaub
{0x2D3D0574,  VT_ENUM,            700,   S700_0,   sizeof(E700_0),     E700_0,       DEFAULT_FLAG, 123, 4}, // Betriebsart
{0x2D3D0574,  VT_ENUM,            700,  S_BA_TEXT,sizeof(E_BA_HK),    E_BA_HK,      DEFAULT_FLAG, 255, 255},
{0x053D1A83,  VT_ENUM,            701,   S701_0,   sizeof(E701_0),     E701_0,       DEFAULT_FLAG, 123, 4}, // Wärmer / Kälter
{0x053D1A91,  VT_ENUM,            701.1, S701P1_0, sizeof(E701P1_0),   E701P1_0,     DEFAULT_FLAG, 123, 4}, // Temporär wärmer
{0x053D1A92,  VT_ENUM,            701.2, S701P2_0, sizeof(E701P2_0),   E701P2_0,     DEFAULT_FLAG, 123, 4}, // Temporär kälter
{0x2D3D058E,  VT_TEMP,            710,   S710_0,   0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Komfortsollwert
{0x2D3D060B,  VT_ONOFF,           726,   S726_0,   sizeof(ENUM_ONOFF), ENUM_ONOFF,   FL_OEM,       123, 4}, // Heizkennlinien-Adaption
{0x2D3D0607,  VT_MINUTES,         790,   S790_0,   0,                  NULL,         FL_OEM,       123, 4}, // Einschaltzeitoptimierung maximale Vorverlegung
{0x213D0654,  VT_BINARY_ENUM,     832,   S832_0,   sizeof(E832_0),     E832_0,       FL_OEM,       123, 4}, // Antrieb-Regelungsart Heizkreis
{0x053D10D7,  VT_PERCENT_NN,      881,   S881_0,   0,                  NULL,         FL_OEM,       123, 4}, // Anlaufdrehzahl
{0x2E3D0574,  VT_ENUM,            1000,  S1000_0,  sizeof(E1000_0),    E1000_0,      DEFAULT_FLAG, 123, 4}, // Betriebsart
{0x2E3D0574,  VT_ENUM,            1000,  S_BA_TEXT,sizeof(E_BA_HK),    E_BA_HK,      DEFAULT_FLAG, 255, 255},
{0x2F3D0574,  VT_ENUM,            1300,  S1300_0,  sizeof(E1300_0),    E1300_0,      FL_OEM,       123, 4}, // Betriebsart
{0x2F3D0574,  VT_ENUM,            1300,  S_BA_TEXT,sizeof(E_BA_HK),    E_BA_HK,      DEFAULT_FLAG, 255, 255},
{0x313D0571,  VT_ENUM,            1600,  S1600_0,  sizeof(E1600_0),    E1600_0,      DEFAULT_FLAG, 123, 4}, // Betriebsart
{0x313D0571,  VT_ENUM,            1600,  S_BA_TEXT,sizeof(E_BA_TW),    E_BA_TW,      DEFAULT_FLAG, 255, 255},
{0x313D06B9,  VT_TEMP,            1610,  S1610_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Trinkwassertemperatur-Nennsollwert
{0x313D075A,  VT_HOUR_MINUTES_N,  1644,  S1644_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Zeitpunkt für Legionellenfunktion
{0x053D17B3,  VT_SECONDS_SHORT5,  2479,  S2479_0,  0,                  NULL,         FL_OEM,       123, 4}, // Abgasüberwachung Zeitkonst Modulationsfreigabe
{0x053D05D9,  VT_PRESSURE,        6141,  S6141_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Wasserdruck Minimum
{0x053D0001,  VT_STRING,          6224,  STR_GI,   0,                  NULL,         FL_RONLY, 255, 255},
{0x053D0002,  VT_UINT,            6225,  STR_GF,   0,                  NULL,         FL_RONLY, 255, 255},
{0x053D0072,  VT_ONOFF,           6605,  S6605_0,  sizeof(ENUM_ONOFF), ENUM_ONOFF,   FL_OEM+FL_RONLY, 123, 4}, // LPB-Speisung Status
{0x053D009F,  VT_LPBADDR,         6650,  S6650_0,  0,                  NULL,         FL_OEM+FL_RONLY, 123, 4}, // Aussentemperatur-Lieferant
{0x053D009A,  VT_UNKNOWN,         6700,  S6700_0,  0,                  NULL,         FL_RONLY,     123, 4}, // Fehlermeldung
{0x053D0C69,  VT_UINT,            7042,  S7042_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Brennerstarts Wartungsintervall
{0x053D06E8,  VT_STRING,          7170,  S7170_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Telefon Kundendienst
{0x053D0E79,  VT_SINT_NN,         7400,  S7400_0,  0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Funktionswert 1 H2/H21 Modul 2
{0x053D0521,  VT_TEMP,            8700,  S8700_0,  0,                  NULL,         FL_RONLY,     123, 4}, // Aussentemperatur
{0x2D3D0215,  VT_TEMP,            10000, S10000,   0,                  NULL,         DEFAULT_FLAG+FL_WONLY+FL_SPECIAL_INF, 255, 255},
{0x2D3D021C,  VT_TEMP,            10000, S10000,   0,                  NULL,         DEFAULT_FLAG+FL_WONLY+FL_SPECIAL_INF, 59, 255},
{0x313D0573,  VT_ONOFF,           10019, S10019,   sizeof(ENUM_ONOFF), ENUM_ONOFF,   DEFAULT_FLAG+FL_WONLY, 255, 255},
{0x053D009C,  VT_UNKNOWN,         10021, S10021_0, 0,                  NULL,         FL_RONLY,     123, 4}, // Wartungsmeldung
{0x053D058C,  VT_TEMP,            10023, S10023_0, 0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Raumtemperatur Minimum
{0x2D3D0592,  VT_TEMP,            10046, S10046_0, 0,                  NULL,         DEFAULT_FLAG, 123, 4}, // Raumtemperatur-Frostschutzsollwert
{0x2D3D0572,  VT_ENUM,            10110, S_BATHK1, sizeof(E_BA_TEMP),  E_BA_TEMP,    DEFAULT_FLAG+FL_WONLY, 255, 255},
