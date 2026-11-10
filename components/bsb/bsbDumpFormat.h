#pragma once

#include "bsbValueDecoder.h"

#include <cstdint>
#include <string>

// The output format of BSB-LAN's JSON backup (`/JB`), which BSB-LAN's `/JS` can restore.

namespace esphome {
  namespace bsb {

    // One parameter of the dump, generated from BSB-LAN's controller specific definitions.
    struct DumpParameter {
      float       number;   // BSB-LAN's parameter number ("ProgNr"), may have one decimal (701.1)
      uint32_t    field_id; // as queried and as expected in the answer
      ValueSpec   value;
      const char* category;
      const char* description;
    };

    // The dump is one JSON object, one parameter per line: header, lines joined by the
    // separator, footer.
    constexpr const char* DumpHeader        = "{\r\n";
    constexpr const char* DumpLineSeparator = ",\r\n";
    constexpr const char* DumpFooter        = "\r\n}\r\n";

    std::string format_dump_line( const DumpParameter& parameter, const std::string& value, uint8_t destination_address );

  } // namespace bsb
} // namespace esphome
