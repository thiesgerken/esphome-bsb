#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Port of the value decoders of BSB-LAN's printTelegram() (include/print_telegram.h). The
// decoded text has to match BSB-LAN byte for byte, so the arithmetic deliberately keeps
// BSB-LAN's integer and float types, including their truncation and rounding quirks.

namespace esphome {
  namespace bsb {

    enum class ValueFormat : uint8_t {
      Byte,                 // printBYTE()
      Word,                 // printWORD()
      DWord,                // printDWORD()
      SignedWord,           // printSINT()
      FixPoint,             // printFIXPOINT()
      FixPointByte,         // printFIXPOINT_BYTE()
      FixPointUnsignedByte, // printFIXPOINT_BYTE_US()
      Choice,               // printCHOICE()
      Enum,                 // VT_ENUM
      DateTime,             // printDateTime() for VT_DATETIME
      Year,                 // printDateTime() for VT_YEAR
      DayMonth,             // printDateTime() for VT_DAYMONTH and VT_VACATIONPROG
      TimeOfDay,            // printDateTime() for VT_TIME
      HourMinutes,          // printTime()
      TimeProgram,          // printTimeProg()
      String,               // VT_STRING
    };

    // The values BSB-LAN's printENUM() can find in a parameter's enum.
    struct EnumValues {
      const uint16_t* values;
      uint16_t        count;

      bool contains( uint16_t value ) const;
    };

    struct ValueSpec {
      ValueFormat       format;
      float             operand; // BSB-LAN's optbl operand: divisor, or factor for SignedWord
      uint8_t           precision;
      const EnumValues* enum_values; // nullptr if the definitions assign no enum
    };

    struct DecodedValue {
      // BSB-LAN's decodedTelegram.error numbers. Besides these, an error telegram's code is passed on.
      static constexpr uint16_t NoError          = 0;
      static constexpr uint16_t LengthMismatch   = 256;
      static constexpr uint16_t UnknownEnumValue = 258;
      static constexpr uint16_t MissingEnum      = 259;

      uint16_t    error;
      std::string text;
    };

    // Decodes the payload of an answer to a query. device_family is the controller's BSB-LAN
    // device family, which changes how FixPointByte payloads are read.
    DecodedValue
    decode_value( const ValueSpec& spec, uint8_t device_family, bool is_error_telegram, const std::vector< uint8_t >& payload );

  } // namespace bsb
} // namespace esphome
