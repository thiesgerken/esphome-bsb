#pragma once

#include "esphome/core/defines.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include "bsbDumpChannel.h"
  #include "bsbDumpFormat.h"
  #include "bsbPacket.h"

  #include "esphome/core/optional.h"

  #include <cstddef>
  #include <cstdint>

namespace esphome {
  namespace bsb {

    // Queries all parameters of a dump one after another, like BSB-LAN's `/JB`, and sends the
    // line of every parameter that decoded without error to the channel. Driven by the
    // component loop.
    class BsbParameterDump {
    public:
      BsbParameterDump( const DumpParameter* parameters,
                        size_t               parameter_count,
                        uint8_t              device_family,
                        uint8_t              source_address,
                        uint8_t              destination_address );

      DumpChannel& channel() { return channel_; }
      size_t       parameter_count() const { return parameter_count_; }
      bool         is_running() const { return running_; }

      // Advances the dump and returns the query telegram to send now, if any.
      optional< BsbPacket > poll( uint32_t now );
      void                  handle_packet( const BsbPacket& packet );

    private:
      const DumpParameter& current() const { return parameters_[next_index_]; }
      bool                 is_answer( const BsbPacket& packet ) const;

      void      start();
      BsbPacket query_current( uint32_t now );
      void      handle_answer( const BsbPacket& packet );
      void      skip_current( uint16_t error );
      void      advance();
      void      finish();

      // Like BSB-LAN's query(): three attempts, three seconds each.
      static constexpr uint8_t  QueryAttempts = 3;
      static constexpr uint32_t AnswerTimeout = 3000;
      // BSB-LAN's error number for a query without answer.
      static constexpr uint16_t QueryFailed = 261;

      DumpChannel          channel_;
      const DumpParameter* parameters_;
      const size_t         parameter_count_;
      const uint8_t        device_family_;
      const uint8_t        source_address_;
      const uint8_t        destination_address_;

      bool     running_         = false;
      size_t   next_index_      = 0;
      size_t   dumped_count_    = 0;
      uint8_t  attempts_        = 0;
      bool     awaiting_answer_ = false;
      uint32_t query_sent_at_   = 0;
    };

  } // namespace bsb
} // namespace esphome

#endif
