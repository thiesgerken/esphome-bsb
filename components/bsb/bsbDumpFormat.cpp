#include "bsbDumpFormat.h"

#include <cstdio>

namespace esphome {
  namespace bsb {

    std::string format_dump_line( const DumpParameter& parameter, const std::string& value, uint8_t destination_address ) {
      // Same format string and arguments as BSB-LAN, which prints the type as a constant 1 and
      // escapes nothing.
      constexpr const char* Format = "  \"%g - %s - %s\":{\"parameter\":\"%g\", \"value\":\"%s\", \"type\":\"%d\", \"destination\":%d}";
      const double          number = parameter.number;

      const int length =
        snprintf( nullptr, 0, Format, number, parameter.category, parameter.description, number, value.c_str(), 1, destination_address );
      std::string line( length, '\0' );
      snprintf(
        &line[0], length + 1, Format, number, parameter.category, parameter.description, number, value.c_str(), 1, destination_address );
      return line;
    }

  } // namespace bsb
} // namespace esphome
