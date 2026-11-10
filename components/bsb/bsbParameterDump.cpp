#include "bsbParameterDump.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include "bsbPacketSend.h"

  #include "esphome/core/log.h"

namespace esphome {
  namespace bsb {

    static const char* const TAG = "bsb.dump";

    BsbParameterDump::BsbParameterDump(
      const DumpParameter* parameters, size_t parameter_count, uint8_t device_family, uint8_t source_address, uint8_t destination_address )
        : parameters_( parameters )
        , parameter_count_( parameter_count )
        , device_family_( device_family )
        , source_address_( source_address )
        , destination_address_( destination_address ) {}

    optional< BsbPacket > BsbParameterDump::poll( uint32_t now ) {
      if( !running_ ) {
        if( !channel_.take_request() ) {
          return {};
        }
        start();
      }

      if( channel_.is_cancelled() ) {
        ESP_LOGI( TAG, "Parameter dump cancelled" );
        finish();
        return {};
      }

      if( awaiting_answer_ ) {
        if( now - query_sent_at_ < AnswerTimeout ) {
          return {};
        }
        if( attempts_ < QueryAttempts ) {
          return query_current( now );
        }
        skip_current( QueryFailed );
      }

      if( next_index_ == parameter_count_ ) {
        ESP_LOGI( TAG, "Parameter dump finished, %u of %u parameters dumped", ( unsigned )dumped_count_, ( unsigned )parameter_count_ );
        finish();
        return {};
      }
      if( !channel_.can_send_line() ) {
        return {}; // the HTTP client is behind, wait before producing the next line
      }
      return query_current( now );
    }

    void BsbParameterDump::handle_packet( const BsbPacket& packet ) {
      if( running_ && awaiting_answer_ && is_answer( packet ) ) {
        handle_answer( packet );
      }
    }

    // Like BSB-LAN, any telegram to us with the queried field ID counts as the answer.
    bool BsbParameterDump::is_answer( const BsbPacket& packet ) const {
      return packet.destinationAddress == source_address_ && packet.fieldId == current().field_id;
    }

    void BsbParameterDump::start() {
      ESP_LOGI( TAG, "Parameter dump started, querying %u parameters", ( unsigned )parameter_count_ );
      running_         = true;
      next_index_      = 0;
      dumped_count_    = 0;
      attempts_        = 0;
      awaiting_answer_ = false;
    }

    BsbPacket BsbParameterDump::query_current( uint32_t now ) {
      attempts_++;
      awaiting_answer_ = true;
      query_sent_at_   = now;
      return BsbPacketGet( source_address_, destination_address_, current().field_id );
    }

    void BsbParameterDump::handle_answer( const BsbPacket& packet ) {
      const DumpParameter& parameter = current();
      const DecodedValue   decoded =
        decode_value( parameter.value, device_family_, packet.command == BsbPacket::Command::Error, packet.payload );
      if( decoded.error != DecodedValue::NoError ) {
        skip_current( decoded.error );
        return;
      }
      channel_.send_line( format_dump_line( parameter, decoded.text, destination_address_ ) );
      dumped_count_++;
      advance();
    }

    void BsbParameterDump::skip_current( uint16_t error ) {
      ESP_LOGD( TAG, "Parameter %g skipped, error %u", current().number, error );
      advance();
    }

    void BsbParameterDump::advance() {
      next_index_++;
      attempts_        = 0;
      awaiting_answer_ = false;
    }

    void BsbParameterDump::finish() {
      channel_.send_end();
      running_ = false;
    }

  } // namespace bsb
} // namespace esphome

#endif
