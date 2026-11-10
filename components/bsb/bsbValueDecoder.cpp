#include "bsbValueDecoder.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>

namespace esphome {
  namespace bsb {

    namespace {

      using Payload = std::vector< uint8_t >;

      constexpr const char* Disabled = "---"; // BSB-LAN's STR_DISABLED

      std::string format( const char* format, ... ) __attribute__( ( format( printf, 1, 2 ) ) );

      std::string format( const char* format, ... ) {
        char    buffer[48];
        va_list args;
        va_start( args, format );
        vsnprintf( buffer, sizeof( buffer ), format, args );
        va_end( args );
        return buffer;
      }

      DecodedValue value( std::string text ) { return { DecodedValue::NoError, std::move( text ) }; }

      DecodedValue length_mismatch() { return { DecodedValue::LengthMismatch, "" }; }

      // The first payload byte of an answer is 0 when the value is set and non-zero when the
      // parameter is disabled.
      bool is_enabled( const Payload& payload ) { return payload[0] == 0; }

      // Port of BSB-LAN's _printFIXPOINT(), including its rounding by adding 5 / 10^(precision + 1).
      // The mix of float and double operations is kept, it decides the last digit in edge cases.
      std::string format_fixpoint( float number, uint8_t precision ) {
        std::string text;
        if( number < 0 ) {
          text += '-';
          number *= -1.0;
        }
        float scale = 10.0;
        for( uint8_t i = 0; i < precision; i++ ) {
          scale *= 10.0;
        }
        number += 5.0 / scale;
        const int integer = ( int )number;
        if( precision == 0 ) {
          return text + format( "%d", integer );
        }
        scale /= 10.0;
        const int fraction = ( int )( number * scale - integer * scale );
        return text + format( "%d.%0*d", integer, precision, fraction );
      }

      DecodedValue decode_byte( const Payload& payload ) {
        if( payload.size() != 2 ) {
          return length_mismatch();
        }
        return value( is_enabled( payload ) ? format( "%d", payload[1] ) : Disabled );
      }

      DecodedValue decode_word( const Payload& payload, float divisor ) {
        if( payload.size() != 3 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        const int32_t number = ( ( int32_t( payload[1] ) << 8 ) + int32_t( payload[2] ) ) / divisor;
        return value( format( "%ld", ( long )number ) );
      }

      DecodedValue decode_dword( const Payload& payload, float divisor ) {
        if( payload.size() != 5 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        // BSB-LAN assembles the bytes in a 32 bit long, so values from 0x80000000 on are negative.
        const int32_t raw =
          int32_t( uint32_t( payload[1] ) << 24 | uint32_t( payload[2] ) << 16 | uint32_t( payload[3] ) << 8 | payload[4] );
        const int32_t number = raw / divisor;
        return value( format( "%ld", ( long )number ) );
      }

      DecodedValue decode_signed_word( const Payload& payload, float factor ) {
        if( payload.size() != 3 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        // BSB-LAN multiplies the unsigned word and stores the result in an int16_t, which wraps.
        const int16_t number = int16_t( int32_t( ( ( payload[1] << 8 ) + payload[2] ) * factor ) );
        return value( format( "%d", number ) );
      }

      DecodedValue decode_fixpoint( const Payload& payload, float divisor, uint8_t precision ) {
        if( payload.size() != 3 && payload.size() != 5 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        const size_t high   = payload.size() == 3 ? 1 : 3;
        const float  number = float( int16_t( payload[high] << 8 ) + int16_t( payload[high + 1] ) ) / divisor;
        return value( format_fixpoint( number, precision ) );
      }

      DecodedValue decode_fixpoint_byte( const Payload& payload, float divisor, uint8_t precision, uint8_t device_family ) {
        // BSB-LAN accepts a two byte value from these device families and reads its last byte.
        const bool word_family = device_family == 107 || device_family == 123 || device_family == 163;
        if( payload.size() != 2 && !( payload.size() == 3 && word_family ) ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        const float number = float( int8_t( payload[payload.size() - 1] ) ) / divisor;
        return value( format_fixpoint( number, precision ) );
      }

      DecodedValue decode_fixpoint_unsigned_byte( const Payload& payload, float divisor, uint8_t precision ) {
        if( payload.size() != 2 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        return value( format_fixpoint( float( payload[1] ) / divisor, precision ) );
      }

      DecodedValue decode_choice( const Payload& payload, const EnumValues* enum_values ) {
        if( payload.size() != 2 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        const uint16_t choice = payload[1] & 1;
        if( enum_values != nullptr && !enum_values->contains( choice ) ) {
          return { DecodedValue::UnknownEnumValue, "" };
        }
        return value( format( "%u", choice ) );
      }

      DecodedValue decode_enum( const Payload& payload, const EnumValues* enum_values ) {
        if( payload.size() != 2 && payload.size() != 3 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( "65535" ); // BSB-LAN's marker for a disabled enum, not "---"
        }
        if( enum_values == nullptr ) {
          return { DecodedValue::MissingEnum, "" };
        }
        const uint16_t number = payload.size() == 2 ? payload[1] : uint16_t( payload[1] << 8 | payload[2] );
        if( !enum_values->contains( number ) ) {
          return { DecodedValue::UnknownEnumValue, "" };
        }
        return value( format( "%u", number ) );
      }

      DecodedValue decode_date_time( const Payload& payload, ValueFormat date_time_format ) {
        if( payload.size() != 9 ) {
          return length_mismatch();
        }
        if( !is_enabled( payload ) ) {
          return value( Disabled );
        }
        const int year = payload[1] + 1900;
        switch( date_time_format ) {
          case ValueFormat::DateTime:
            return value( format( "%02d.%02d.%d %02d:%02d:%02d", payload[3], payload[2], year, payload[5], payload[6], payload[7] ) );
          case ValueFormat::Year:
            return value( format( "%d", year ) );
          case ValueFormat::DayMonth:
            return value( format( "%02d.%02d.", payload[3], payload[2] ) );
          case ValueFormat::TimeOfDay:
            return value( format( "%02d:%02d:%02d", payload[5], payload[6], payload[7] ) );
          default:
            return length_mismatch(); // unreachable, decode_value() only passes the date formats
        }
      }

      DecodedValue decode_hour_minutes( const Payload& payload ) {
        if( payload.size() != 3 ) {
          return length_mismatch();
        }
        return value( is_enabled( payload ) ? format( "%02d:%02d", payload[1], payload[2] ) : "--:--" );
      }

      // Three switching periods of four bytes (on hour, on minute, off hour, off minute); an
      // hour of 24 or more marks an unused period.
      DecodedValue decode_time_program( const Payload& payload ) {
        if( payload.size() != 12 ) {
          return length_mismatch();
        }
        std::string text;
        for( size_t period = 0; period < 3; period++ ) {
          const uint8_t* bytes = &payload[period * 4];
          if( period > 0 ) {
            text += ' ';
          }
          text += bytes[0] < 24 ? format( "%02d:%02d-%02d:%02d", bytes[0], bytes[1], bytes[2], bytes[3] ) : "##:##-##:##";
        }
        return value( text );
      }

      DecodedValue decode_string( const Payload& payload ) {
        if( payload[0] == 0 ) {
          return value( "-" );
        }
        std::string text( payload.begin(), std::find( payload.begin(), payload.end(), 0 ) );
        text.erase( std::remove( text.begin(), text.end(), '\'' ), text.end() );
        return value( text );
      }

    } // namespace

    bool EnumValues::contains( uint16_t value ) const { return std::find( values, values + count, value ) != values + count; }

    DecodedValue decode_value( const ValueSpec& spec, uint8_t device_family, bool is_error_telegram, const Payload& payload ) {
      // BSB-LAN decodes nothing without payload and then reports the parameter without error
      // and with an empty value.
      if( payload.empty() ) {
        return value( "" );
      }
      if( is_error_telegram ) {
        return { payload[0], "" };
      }
      switch( spec.format ) {
        case ValueFormat::Byte:
          return decode_byte( payload );
        case ValueFormat::Word:
          return decode_word( payload, spec.operand );
        case ValueFormat::DWord:
          return decode_dword( payload, spec.operand );
        case ValueFormat::SignedWord:
          return decode_signed_word( payload, spec.operand );
        case ValueFormat::FixPoint:
          return decode_fixpoint( payload, spec.operand, spec.precision );
        case ValueFormat::FixPointByte:
          return decode_fixpoint_byte( payload, spec.operand, spec.precision, device_family );
        case ValueFormat::FixPointUnsignedByte:
          return decode_fixpoint_unsigned_byte( payload, spec.operand, spec.precision );
        case ValueFormat::Choice:
          return decode_choice( payload, spec.enum_values );
        case ValueFormat::Enum:
          return decode_enum( payload, spec.enum_values );
        case ValueFormat::DateTime:
        case ValueFormat::Year:
        case ValueFormat::DayMonth:
        case ValueFormat::TimeOfDay:
          return decode_date_time( payload, spec.format );
        case ValueFormat::HourMinutes:
          return decode_hour_minutes( payload );
        case ValueFormat::TimeProgram:
          return decode_time_program( payload );
        case ValueFormat::String:
          return decode_string( payload );
      }
      return length_mismatch(); // unreachable, all formats are handled above
    }

  } // namespace bsb
} // namespace esphome
