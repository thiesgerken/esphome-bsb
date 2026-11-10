"""Parser for BSB-LAN's controller specific `BSB_LAN_custom_defs.h`.

Self-contained (no ESPHome imports), so it can be tested on its own.

BSB-LAN stores parameter numbers as `float` and compares them with float semantics, so
all parameter numbers here are rounded to float32.
"""

from __future__ import annotations

from dataclasses import dataclass
import re
import struct

FL_RONLY = 1
FL_WONLY = 2
FL_NO_CMD = 4
FL_OEM = 8
FL_SPECIAL_INF = 16
FL_EEPROM = 32
FL_QINF_ONLY = 64
FL_SW_CTL_RONLY = 128
FL_NOSWAP_QUR = 256
FL_FORCE_INF = 512

FLAG_VALUES = {
    "FL_WRITEABLE": 0,
    "FL_RONLY": FL_RONLY,
    "FL_WONLY": FL_WONLY,
    "FL_NO_CMD": FL_NO_CMD,
    "FL_OEM": FL_OEM,
    "FL_SPECIAL_INF": FL_SPECIAL_INF,
    "FL_EEPROM": FL_EEPROM,
    "FL_QINF_ONLY": FL_QINF_ONLY,
    "FL_SW_CTL_RONLY": FL_SW_CTL_RONLY,
    "FL_NOSWAP_QUR": FL_NOSWAP_QUR,
    "FL_FORCE_INF": FL_FORCE_INF,
    # BSB-LAN's DEFAULT_FLAG comes from BSB_LAN_config.h. 0 makes everything writable that
    # the definitions don't mark read-only, which is the selection the dump reproduces.
    "DEFAULT_FLAG": 0,
}

# Symbols the custom definitions reference but BSB-LAN defines elsewhere: generic enums
# from BSB_LAN_defs.h (values only) and category names from its German localization
# (LANG_DE.h), the language the custom definitions are written in.
GENERIC_ENUM_VALUES = {
    "ENUM_ONOFF": (0, 1),
    "ENUM_YESNO": (0, 1),
    "ENUM_CLOSEDOPEN": (0, 1),
    "ENUM_VOLTAGEONOFF": (0, 1),
    "ENUM_WEEKDAY": (1, 2, 3, 4, 5, 6, 7),
}
GENERIC_TEXTS = {
    "ENUM_CAT_40_TEXT": "Benutzerdefiniert",
    "ENUM_CAT_41_TEXT": "LMU64",
    "ENUM_CAT_42_TEXT": "RVD/RVP",
    "ENUM_CAT_43_TEXT": "PPS-Bus",
    "ENUM_CAT_44_TEXT": "One Wire, DHT & MAX! Sensors",
    "ENUM_CAT_45_TEXT": "unbekannte Kategorie",
}


class DefinitionsError(Exception):
    pass


def float32(value: float) -> float:
    return struct.unpack("<f", struct.pack("<f", value))[0]


# --- tokenizer -------------------------------------------------------------------------

_TOKEN_RE = re.compile(
    r"""
      (?P<ws>\s+)
    | (?P<comment>//[^\n]*|/\*.*?\*/)
    | (?P<string>"(?:[^"\\\n]|\\.)*")
    | (?P<number>0[xX][0-9a-fA-F]+[uUlL]*|\d+\.\d*(?:[eE][+-]?\d+)?[fF]?|\d+[uUlL]*)
    | (?P<ident>[A-Za-z_]\w*)
    | (?P<punct>[{}\[\](),;=+\-*/#&|<>.])
    """,
    re.VERBOSE | re.DOTALL,
)


@dataclass(frozen=True)
class Token:
    kind: str
    text: str


def _tokenize(text: str) -> list[Token]:
    tokens = []
    position = 0
    while position < len(text):
        match = _TOKEN_RE.match(text, position)
        if match is None:
            line = text.count("\n", 0, position) + 1
            raise DefinitionsError(f"line {line}: unexpected character {text[position]!r}")
        position = match.end()
        if match.lastgroup not in ("ws", "comment"):
            tokens.append(Token(match.lastgroup, match.group()))
    return tokens


def _unescape_c_string(literal: str) -> bytes:
    """Bytes of a C string literal (including its quotes), as UTF-8 source encoding."""
    body = literal[1:-1]
    out = bytearray()
    i = 0
    simple_escapes = {"n": 10, "t": 9, "r": 13, "a": 7, "b": 8, "f": 12, "v": 11, "\\": 92, '"': 34, "'": 39, "?": 63}
    while i < len(body):
        char = body[i]
        if char != "\\":
            out += char.encode("utf-8")
            i += 1
            continue
        escape = body[i + 1]
        if escape == "x":
            digits = re.match(r"[0-9a-fA-F]+", body[i + 2 :]).group()
            value = int(digits, 16)
            if value > 0xFF:
                raise DefinitionsError(f"hex escape out of range in {literal}")
            out.append(value)
            i += 2 + len(digits)
        elif escape in "01234567":
            digits = re.match(r"[0-7]{1,3}", body[i + 1 :]).group()
            out.append(int(digits, 8))
            i += 1 + len(digits)
        elif escape in simple_escapes:
            out.append(simple_escapes[escape])
            i += 2
        else:
            raise DefinitionsError(f"unsupported escape \\{escape} in {literal}")
    return bytes(out)


def _parse_number(text: str) -> float | int:
    stripped = text.rstrip("uUlLfF") if not text.lower().startswith("0x") else text.rstrip("uUlL")
    if stripped.lower().startswith("0x"):
        return int(stripped, 16)
    if "." in stripped or "e" in stripped.lower():
        return float(stripped)
    return int(stripped)


def _split_top_level(tokens: list[Token], separator: str) -> list[list[Token]]:
    parts: list[list[Token]] = [[]]
    depth = 0
    for token in tokens:
        if token.kind == "punct" and token.text in "{([":
            depth += 1
        elif token.kind == "punct" and token.text in "})]":
            depth -= 1
        if depth == 0 and token.kind == "punct" and token.text == separator:
            parts.append([])
        else:
            parts[-1].append(token)
    return parts


def _strip_braces(tokens: list[Token]) -> list[Token]:
    if tokens and tokens[0].text == "{" and tokens[-1].text == "}":
        return tokens[1:-1]
    return tokens


# --- definitions model -----------------------------------------------------------------


@dataclass(frozen=True)
class Command:
    """One row of BSB-LAN's command table (`cmdtbl`)."""

    field_id: int
    value_type: str
    parameter: float
    description_symbol: str
    enum_symbol: str | None
    flags: int
    device_family: int
    device_variant: int

    @property
    def is_writable(self) -> bool:
        return not self.flags & (FL_RONLY | FL_WONLY)


@dataclass(frozen=True)
class Category:
    name: bytes
    device_family: int | None
    device_variant: int | None


class CustomDefinitions:
    """The parts of a `BSB_LAN_custom_defs.h` the parameter dump needs."""

    def __init__(self, text: str) -> None:
        self._macros: dict[str, list[Token]] = {}
        self._arrays: dict[str, list[Token]] = {}
        self._enums: dict[str, list[str]] = {}
        self._parse(text)
        self.commands = self._parse_command_table()
        self.category_ranges = self._parse_category_ranges()
        self.unknown_category = self._enums["category_t"].index("CAT_UNKNOWN")
        if len(self.category_ranges) < self.unknown_category:
            raise DefinitionsError("ENUM_CAT_NR has fewer ranges than category_t has categories")

    # -- raw parsing --

    def _parse(self, text: str) -> None:
        body_lines = []
        for line in text.splitlines():
            stripped = line.strip()
            if stripped.startswith("#"):
                self._parse_directive(stripped)
                body_lines.append("")
            else:
                body_lines.append(line)
        for statement in _split_top_level(_tokenize("\n".join(body_lines)), ";"):
            self._parse_statement(statement)

    def _parse_directive(self, line: str) -> None:
        tokens = _tokenize(line)
        if len(tokens) >= 3 and tokens[1].text == "define":
            self._macros[tokens[2].text] = tokens[3:]
        else:
            raise DefinitionsError(f"unsupported preprocessor directive: {line}")

    def _parse_statement(self, tokens: list[Token]) -> None:
        if not tokens:
            return
        texts = [token.text for token in tokens]
        if texts[:2] == ["typedef", "enum"]:
            members = _split_top_level(_strip_braces(tokens[2:-1]), ",")
            self._enums[texts[-1]] = [member[0].text for member in members if member]
        elif "=" in texts and "[" in texts:
            name = tokens[texts.index("[") - 1].text
            initializer = tokens[texts.index("=") + 1 :]
            unclosed = texts.count("{") - texts.count("}")
            if unclosed and name == "cmdtbl":
                # The custom definitions end inside cmdtbl, BSB_LAN_defs.h appends its generic
                # rows and closes it.
                initializer += [Token("punct", "}")] * unclosed
            elif unclosed:
                raise DefinitionsError(f"unterminated initializer of {name}")
            self._arrays[name] = initializer
        else:
            raise DefinitionsError(f"unsupported statement: {' '.join(texts[:8])} ...")

    # -- symbol resolution --

    def _expand(self, tokens: list[Token], seen: frozenset[str] = frozenset()) -> list[Token]:
        expanded = []
        for token in tokens:
            if token.kind == "ident" and token.text in self._macros:
                if token.text in seen:
                    raise DefinitionsError(f"recursive macro {token.text}")
                expanded += self._expand(self._macros[token.text], seen | {token.text})
            else:
                expanded.append(token)
        return expanded

    def _string_bytes(self, tokens: list[Token]) -> bytes:
        """Concatenated bytes of adjacent string literals and string macros."""
        out = b""
        for token in self._expand(tokens):
            if token.kind == "string":
                out += _unescape_c_string(token.text)
            elif token.kind == "ident" and token.text in GENERIC_TEXTS:
                out += GENERIC_TEXTS[token.text].encode("utf-8")
            else:
                raise DefinitionsError(f"undefined string symbol {token.text}")
        return out

    def _resolve_array_name(self, symbol: str) -> str:
        expansion = self._expand([Token("ident", symbol)])
        if len(expansion) != 1 or expansion[0].kind != "ident":
            raise DefinitionsError(f"{symbol} does not name an array")
        return expansion[0].text

    def char_array(self, symbol: str) -> bytes:
        """Contents of a `const char NAME[]`, including the terminating NUL (like `sizeof`)."""
        name = self._resolve_array_name(symbol)
        if name not in self._arrays:
            raise DefinitionsError(f"undefined array {symbol}")
        return self._string_bytes(_strip_braces(self._arrays[name])) + b"\0"

    def text(self, symbol: str) -> bytes:
        """A description string as BSB-LAN prints it (up to the first NUL)."""
        return self.char_array(symbol).split(b"\0", 1)[0]

    def enum_values(self, symbol: str) -> tuple[int, ...]:
        """Values BSB-LAN's printENUM() finds in the enum, scanned exactly like it does."""
        name = self._resolve_array_name(symbol)
        if name in GENERIC_ENUM_VALUES:
            return GENERIC_ENUM_VALUES[name]
        return _scan_enum_values(self.char_array(name))

    # -- tables --

    def _parse_command_table(self) -> list[Command]:
        rows = _split_top_level(_strip_braces(self._arrays["cmdtbl"]), ",")
        return [self._parse_command(_strip_braces(row)) for row in rows if row]

    def _parse_command(self, tokens: list[Token]) -> Command:
        fields = _split_top_level(tokens, ",")
        if len(fields) != 9:
            raise DefinitionsError(f"command row with {len(fields)} fields: {' '.join(t.text for t in tokens)}")
        field_id, value_type, parameter, description, enum_size, enum, flags, family, variant = fields
        enum_symbol = None if enum[0].text == "NULL" else enum[0].text
        size_symbol = enum_size[2].text if enum_size[0].text == "sizeof" else None
        if size_symbol != enum_symbol:
            raise DefinitionsError(f"enum size and enum differ: {size_symbol} vs {enum_symbol}")
        return Command(
            field_id=self._evaluate_int(field_id),
            value_type=value_type[0].text,
            parameter=float32(self._evaluate_number(parameter)),
            description_symbol=description[0].text,
            enum_symbol=enum_symbol,
            flags=self._evaluate_int(flags),
            device_family=self._evaluate_int(family),
            device_variant=self._evaluate_int(variant),
        )

    def _evaluate_number(self, tokens: list[Token]) -> float | int:
        """Sum of numbers and flag constants (`FL_OEM+FL_RONLY`)."""
        total = 0
        for term in _split_top_level(self._expand(tokens), "+"):
            if len(term) != 1:
                raise DefinitionsError(f"unsupported expression {' '.join(t.text for t in tokens)}")
            token = term[0]
            if token.kind == "number":
                total += _parse_number(token.text)
            elif token.text in FLAG_VALUES:
                total += FLAG_VALUES[token.text]
            else:
                raise DefinitionsError(f"unknown constant {token.text}")
        return total

    def _evaluate_int(self, tokens: list[Token]) -> int:
        value = self._evaluate_number(tokens)
        if not isinstance(value, int):
            raise DefinitionsError(f"expected an integer, got {value}")
        return value

    def _parse_category_ranges(self) -> list[tuple[float, float]]:
        numbers = [float32(self._evaluate_number([token])) for token in _strip_braces(self._arrays["ENUM_CAT_NR"]) if token.text != ","]
        if len(numbers) % 2:
            raise DefinitionsError("ENUM_CAT_NR has an odd number of entries")
        return list(zip(numbers[0::2], numbers[1::2], strict=True))

    def category(self, number: int, device_family: int, device_variant: int) -> bytes | None:
        """Name of a category valid for the device, like BSB-LAN's printKat() (None on error 258/263)."""
        found = _scan_categories(self.char_array("ENUM_CAT")).get(number)
        if found is None:
            return None
        if found.device_family is not None and (found.device_family, found.device_variant) != (device_family, device_variant):
            return None
        return found.name


def _byte_at(data: bytes, index: int) -> int | None:
    return data[index] if index < len(data) else None


def _skip_text(data: bytes, index: int) -> int:
    while index < len(data) and data[index] != 0:
        index += 1
    return index + 1


def _scan_enum_values(data: bytes) -> tuple[int, ...]:
    """Port of the scan loop in BSB-LAN's printENUM(): entries are `\\xVV text\\0` or
    `\\xHH\\xLL text\\0`. Returns every value a lookup can find."""
    space = ord(" ")
    values = []
    value = 0
    index = 0
    while index < len(data):
        if _byte_at(data, index + 2) == space:
            value = (data[index] << 8) | data[index + 1]
            index += 1
        elif _byte_at(data, index + 1) == space:
            value = data[index]
        index += 2
        if index < len(data):
            values.append(value)
        index = _skip_text(data, index)
    return tuple(dict.fromkeys(values))


def _scan_categories(data: bytes) -> dict[int, Category]:
    """Port of the scan loop in BSB-LAN's printKat(): entries are `\\xNN name\\0` or
    `\\xNN\\xFF\\xVV name\\0` (category restricted to a device family/variant)."""
    space = ord(" ")
    categories: dict[int, Category] = {}
    number = 0
    family = variant = None
    index = 0
    while index < len(data):
        if _byte_at(data, index + 3) == space:
            number, family, variant = data[index], data[index + 1], data[index + 2]
            index += 2
        elif _byte_at(data, index + 1) == space:
            number, family, variant = data[index], None, None
        index += 2
        if index < len(data):
            name = data[index:].split(b"\0", 1)[0]
            categories.setdefault(number, Category(name, family, variant))
        index = _skip_text(data, index)
    return categories
