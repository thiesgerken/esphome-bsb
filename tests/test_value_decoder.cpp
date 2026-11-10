// Host-side tests for the BSB-LAN compatible value decoder and dump line format.
// Expected values follow BSB-LAN's printTelegram() decoders (include/print_telegram.h).

#include "bsbDumpFormat.h"
#include "bsbValueDecoder.h"

#include <cstdio>
#include <string>
#include <vector>

using namespace esphome::bsb;

namespace {

  int failures = 0;

  void expect_value( const char* name, const DecodedValue& actual, const std::string& expected ) {
    if( actual.error != DecodedValue::NoError || actual.text != expected ) {
      std::printf( "FAIL %s: expected \"%s\", got \"%s\" (error %u)\n", name, expected.c_str(), actual.text.c_str(), actual.error );
      failures++;
    }
  }

  void expect_error( const char* name, const DecodedValue& actual, uint16_t expected ) {
    if( actual.error != expected ) {
      std::printf( "FAIL %s: expected error %u, got error %u (\"%s\")\n", name, expected, actual.error, actual.text.c_str() );
      failures++;
    }
  }

  constexpr uint8_t LMS15       = 123;
  constexpr uint8_t OtherFamily = 85;

  DecodedValue decode( const ValueSpec& spec, const std::vector< uint8_t >& payload, uint8_t family = LMS15 ) {
    return decode_value( spec, family, false, payload );
  }

  constexpr uint16_t   OnOffValues[]   = { 0, 1 };
  constexpr EnumValues OnOff           = { OnOffValues, 2 };
  constexpr uint16_t   OnlyOffValues[] = { 0 };
  constexpr EnumValues OnlyOff         = { OnlyOffValues, 1 };
  constexpr uint16_t   ModeValues[]    = { 0, 1, 2, 3, 258 };
  constexpr EnumValues Modes           = { ModeValues, 5 };

  constexpr ValueSpec Byte          = { ValueFormat::Byte, 1.0f, 0, nullptr };
  constexpr ValueSpec Word          = { ValueFormat::Word, 1.0f, 0, nullptr };
  constexpr ValueSpec Hours         = { ValueFormat::DWord, 3600.0f, 0, nullptr };
  constexpr ValueSpec DWord         = { ValueFormat::DWord, 1.0f, 0, nullptr };
  constexpr ValueSpec SignedWord    = { ValueFormat::SignedWord, 1.0f, 0, nullptr };
  constexpr ValueSpec Temperature   = { ValueFormat::FixPoint, 64.0f, 1, nullptr };
  constexpr ValueSpec Fp02          = { ValueFormat::FixPoint, 50.0f, 2, nullptr };
  constexpr ValueSpec TempShort5    = { ValueFormat::FixPointByte, 2.0f, 1, nullptr };
  constexpr ValueSpec TempShort64   = { ValueFormat::FixPointByte, 64.0f, 5, nullptr };
  constexpr ValueSpec Pressure      = { ValueFormat::FixPointUnsignedByte, 10.0f, 1, nullptr };
  constexpr ValueSpec OnOffChoice   = { ValueFormat::Choice, 1.0f, 0, &OnOff };
  constexpr ValueSpec OnlyOffChoice = { ValueFormat::Choice, 1.0f, 0, &OnlyOff };
  constexpr ValueSpec ModeEnum      = { ValueFormat::Enum, 1.0f, 0, &Modes };
  constexpr ValueSpec EnumWithout   = { ValueFormat::Enum, 1.0f, 0, nullptr };
  constexpr ValueSpec DateTime      = { ValueFormat::DateTime, 1.0f, 0, nullptr };
  constexpr ValueSpec Year          = { ValueFormat::Year, 1.0f, 0, nullptr };
  constexpr ValueSpec DayMonth      = { ValueFormat::DayMonth, 1.0f, 0, nullptr };
  constexpr ValueSpec TimeOfDay     = { ValueFormat::TimeOfDay, 1.0f, 0, nullptr };
  constexpr ValueSpec HourMinutes   = { ValueFormat::HourMinutes, 1.0f, 0, nullptr };
  constexpr ValueSpec TimeProgram   = { ValueFormat::TimeProgram, 1.0f, 0, nullptr };
  constexpr ValueSpec String        = { ValueFormat::String, 1.0f, 0, nullptr };

  const std::vector< uint8_t > DateTimePayload = { 0x00, 124, 11, 10, 2, 13, 5, 9, 0x00 };

  void test_integers() {
    expect_value( "byte", decode( Byte, { 0x00, 0xC8 } ), "200" );
    expect_value( "byte disabled", decode( Byte, { 0x01, 0xC8 } ), "---" );
    expect_error( "byte length", decode( Byte, { 0x00, 0x01, 0x02 } ), DecodedValue::LengthMismatch );

    expect_value( "word", decode( Word, { 0x00, 0x01, 0x2C } ), "300" );
    expect_value( "word disabled", decode( Word, { 0x06, 0x01, 0x2C } ), "---" );
    expect_error( "word length", decode( Word, { 0x00, 0x01 } ), DecodedValue::LengthMismatch );

    expect_value( "dword hours", decode( Hours, { 0x00, 0x00, 0x01, 0x51, 0x80 } ), "24" );
    expect_value( "dword hours truncated", decode( Hours, { 0x00, 0x00, 0x00, 0x1C, 0x1F } ), "1" ); // 7199 s
    expect_value( "dword wraps to negative", decode( DWord, { 0x00, 0xFF, 0xFF, 0xFF, 0xFF } ), "-1" );
    expect_error( "dword length", decode( DWord, { 0x00, 0x01, 0x02 } ), DecodedValue::LengthMismatch );

    expect_value( "signed word", decode( SignedWord, { 0x00, 0xFF, 0xFE } ), "-2" );
    expect_value( "signed word positive", decode( SignedWord, { 0x00, 0x00, 0x2A } ), "42" );
  }

  void test_fixpoint() {
    expect_value( "temperature", decode( Temperature, { 0x00, 0x05, 0x60 } ), "21.5" );
    expect_value( "negative temperature", decode( Temperature, { 0x00, 0xFE, 0xA0 } ), "-5.5" );
    // BSB-LAN prints the sign before rounding, so tiny negative values become "-0.0".
    expect_value( "negative zero", decode( Temperature, { 0x00, 0xFF, 0xFF } ), "-0.0" );
    expect_value( "rounds half up", decode( Temperature, { 0x00, 0x00, 0x04 } ), "0.1" ); // 0.0625
    expect_value( "rounds down", decode( Temperature, { 0x00, 0x00, 0x03 } ), "0.0" );    // 0.046875
    expect_value( "temperature in five bytes", decode( Temperature, { 0x00, 0x12, 0x34, 0x05, 0x60 } ), "21.5" );
    expect_value( "temperature disabled", decode( Temperature, { 0x01, 0x05, 0x60 } ), "---" );
    expect_error( "temperature length", decode( Temperature, { 0x00, 0x05 } ), DecodedValue::LengthMismatch );
    expect_value( "two decimals", decode( Fp02, { 0x00, 0x00, 0x64 } ), "2.00" );

    expect_value( "signed byte", decode( TempShort5, { 0x00, 0xF6 } ), "-5.0" );
    expect_value( "signed byte in word", decode( TempShort5, { 0x00, 0x00, 0xF6 } ), "-5.0" );
    expect_error(
      "signed byte in word, other family", decode( TempShort5, { 0x00, 0x00, 0xF6 }, OtherFamily ), DecodedValue::LengthMismatch );
    expect_value( "five decimals", decode( TempShort64, { 0x00, 0x20 } ), "0.50000" );
    expect_value( "signed byte disabled", decode( TempShort5, { 0x01, 0xF6 } ), "---" );

    expect_value( "unsigned byte", decode( Pressure, { 0x00, 0x0F } ), "1.5" );
    expect_value( "unsigned byte high", decode( Pressure, { 0x00, 0xF6 } ), "24.6" );
    expect_error( "unsigned byte length", decode( Pressure, { 0x00, 0x00, 0x0F } ), DecodedValue::LengthMismatch );
  }

  void test_enums() {
    expect_value( "choice uses lowest bit", decode( OnOffChoice, { 0x00, 0xFF } ), "1" );
    expect_value( "choice off", decode( OnOffChoice, { 0x00, 0x00 } ), "0" );
    expect_value( "choice disabled", decode( OnOffChoice, { 0x01, 0x01 } ), "---" );
    expect_error( "choice not in enum", decode( OnlyOffChoice, { 0x00, 0x01 } ), DecodedValue::UnknownEnumValue );

    expect_value( "enum", decode( ModeEnum, { 0x00, 0x03 } ), "3" );
    expect_value( "enum in word", decode( ModeEnum, { 0x00, 0x01, 0x02 } ), "258" );
    expect_value( "enum disabled", decode( ModeEnum, { 0x01, 0x00 } ), "65535" );
    expect_error( "enum unknown value", decode( ModeEnum, { 0x00, 0x07 } ), DecodedValue::UnknownEnumValue );
    expect_error( "enum without values", decode( EnumWithout, { 0x00, 0x01 } ), DecodedValue::MissingEnum );
    expect_error( "enum length", decode( ModeEnum, { 0x00, 0x00, 0x00, 0x01 } ), DecodedValue::LengthMismatch );
  }

  void test_times() {
    expect_value( "date time", decode( DateTime, DateTimePayload ), "10.11.2024 13:05:09" );
    expect_value( "year", decode( Year, DateTimePayload ), "2024" );
    expect_value( "day month", decode( DayMonth, DateTimePayload ), "10.11." );
    expect_value( "time of day", decode( TimeOfDay, DateTimePayload ), "13:05:09" );
    expect_value( "date disabled", decode( DayMonth, { 0x01, 124, 11, 10, 2, 13, 5, 9, 0x00 } ), "---" );
    expect_error( "date length", decode( DateTime, { 0x00, 124, 11, 10 } ), DecodedValue::LengthMismatch );

    expect_value( "hour minutes", decode( HourMinutes, { 0x00, 0x06, 0x05 } ), "06:05" );
    expect_value( "hour minutes disabled", decode( HourMinutes, { 0x01, 0x06, 0x05 } ), "--:--" );

    expect_value(
      "time program", decode( TimeProgram, { 6, 0, 22, 0, 0x18, 0, 0x18, 0, 0x80, 0, 0, 0 } ), "06:00-22:00 ##:##-##:## ##:##-##:##" );
    expect_value( "time program three periods",
                  decode( TimeProgram, { 5, 30, 8, 0, 11, 45, 13, 15, 17, 0, 23, 59 } ),
                  "05:30-08:00 11:45-13:15 17:00-23:59" );
    expect_error(
      "time program length", decode( TimeProgram, { 6, 0, 22, 0, 0x18, 0, 0x18, 0, 0x80, 0, 0 } ), DecodedValue::LengthMismatch );
  }

  void test_strings_and_errors() {
    expect_value( "string drops apostrophes", decode( String, { 'A', 'B', '\'', 'C', 0x00, 'X' } ), "ABC" );
    expect_value( "string empty", decode( String, { 0x00, 'X' } ), "-" );

    expect_error( "error telegram", decode_value( Temperature, LMS15, true, { 0x07 } ), 7 );
    expect_value( "error telegram with code 0", decode_value( Temperature, LMS15, true, { 0x00 } ), "" );
    expect_value( "no payload", decode( Temperature, {} ), "" );
  }

  void test_dump_line() {
    const DumpParameter parameter = { 701.1f, 0x053D1A91, ModeEnum, "Heizkreis 1", "Tempor\303\244r w\303\244rmer" };
    const std::string   expected  = "  \"701.1 - Heizkreis 1 - Tempor\303\244r w\303\244rmer\":"
                                    "{\"parameter\":\"701.1\", \"value\":\"1\", \"type\":\"1\", \"destination\":0}";
    const std::string   actual    = format_dump_line( parameter, "1", 0 );
    if( actual != expected ) {
      std::printf( "FAIL dump line:\n  expected %s\n  got      %s\n", expected.c_str(), actual.c_str() );
      failures++;
    }

    const DumpParameter integer = { 10023.0f, 0x053D058C, Temperature, "Benutzerdefiniert", "Raumtemperatur Minimum" };
    const std::string   line    = format_dump_line( integer, "15.0", 0 );
    if( line.find( "\"10023 - Benutzerdefiniert" ) == std::string::npos || line.find( "\"parameter\":\"10023\"" ) == std::string::npos ) {
      std::printf( "FAIL dump line integer number: %s\n", line.c_str() );
      failures++;
    }
  }

} // namespace

int
main() {
  test_integers();
  test_fixpoint();
  test_enums();
  test_times();
  test_strings_and_errors();
  test_dump_line();
  if( failures == 0 ) {
    std::printf( "All value decoder tests passed\n" );
  }
  return failures == 0 ? 0 : 1;
}
