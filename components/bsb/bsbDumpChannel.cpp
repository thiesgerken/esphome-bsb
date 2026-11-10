#include "bsbDumpChannel.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include "esphome/core/log.h"

namespace esphome {
  namespace bsb {

    static const char* const TAG = "bsb.dump";

    DumpChannel::DumpChannel() : lines_( xQueueCreate( Capacity, sizeof( std::string* ) ) ) {}

    bool DumpChannel::request_dump() {
      if( state_ != State::Idle ) {
        return false;
      }
      // Reset before the loop can see the request; while idle nobody else touches the flag.
      cancelled_     = false;
      State expected = State::Idle;
      return state_.compare_exchange_strong( expected, State::Requested );
    }

    bool DumpChannel::receive_line( std::string& line ) {
      std::string* item = nullptr;
      xQueueReceive( lines_, &item, portMAX_DELAY );
      if( item == nullptr ) {
        state_ = State::Idle;
        return false;
      }
      line = std::move( *item );
      delete item;
      return true;
    }

    bool DumpChannel::take_request() {
      State expected = State::Requested;
      return state_.compare_exchange_strong( expected, State::Running );
    }

    bool DumpChannel::can_send_line() const { return uxQueueSpacesAvailable( lines_ ) > 1; }

    void DumpChannel::send_line( std::string line ) {
      auto* item = new std::string( std::move( line ) );
      if( xQueueSend( lines_, &item, 0 ) != pdTRUE ) {
        ESP_LOGE( TAG, "Dump line dropped, send_line() called without room" );
        delete item;
      }
    }

    void DumpChannel::send_end() {
      std::string* end = nullptr;
      xQueueSend( lines_, &end, 0 );
    }

  } // namespace bsb
} // namespace esphome

#endif
