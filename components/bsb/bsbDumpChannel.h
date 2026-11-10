#pragma once

#include "esphome/core/defines.h"
#ifdef USE_BSB_PARAMETER_DUMP

  #include <atomic>
  #include <string>

  #include <freertos/FreeRTOS.h>
  #include <freertos/queue.h>

namespace esphome {
  namespace bsb {

    // Hands the lines of a parameter dump from the component loop, which owns the bus, to the
    // HTTP handler, which runs in the web server task. One dump at a time.
    class DumpChannel {
    public:
      DumpChannel();

      // Web server task: start a dump; false if one is already in progress.
      bool request_dump();
      // Web server task: blocks until the next line; false at the end of the dump, which
      // frees the channel for the next request.
      bool receive_line( std::string& line );
      // Web server task: ask the loop to stop early. Keep receiving until the end.
      void cancel() { cancelled_ = true; }

      // Component loop: true once for each requested dump.
      bool take_request();
      bool is_cancelled() const { return cancelled_; }
      // Component loop: whether send_line() has room, so the next parameter can be queried.
      bool can_send_line() const;
      void send_line( std::string line );
      void send_end();

    private:
      enum class State : uint8_t { Idle, Requested, Running };

      std::atomic< State > state_{ State::Idle };
      std::atomic< bool >  cancelled_{ false };
      // Items are owned std::string*, nullptr marks the end of the dump.
      QueueHandle_t lines_;

      // One slot is kept free for the end marker, so ending never has to wait for the receiver.
      static constexpr UBaseType_t Capacity = 4;
    };

  } // namespace bsb
} // namespace esphome

#endif
