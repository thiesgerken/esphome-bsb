#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "bsbPacket.h"
#include "bsbPacketSend.h"

#include "esphome/components/select/select.h"

namespace esphome {
  namespace bsb {
    extern const char* const TAG;

    class BsbSelect : public select::Select {
    public:
      void set_field_id( const uint32_t field_id ) { this->field_id_ = field_id; }
      const uint32_t get_field_id() const { return field_id_; }

      void set_enable_byte( const uint8_t enable_byte ) { this->enable_byte_ = enable_byte; }

      void set_update_interval( const uint32_t val ) { update_interval_ms_ = val; }
      const uint32_t get_update_interval() const { return update_interval_ms_; }

      void set_retry_interval( const uint32_t val ) { retry_interval_ms_ = val; }
      void set_retry_count( uint8_t val ) { retry_count_ = val; }

      void add_option_mapping( int8_t value, const std::string& option ) {
        value_to_option_[value] = option;
        option_to_value_[option] = value;
      }

      void set_value( const float value ) {
        int8_t int_value = static_cast<int8_t>(value);
        auto it = value_to_option_.find(int_value);
        if (it != value_to_option_.end()) {
          publish_state(it->second);
        } else {
          ESP_LOGW(TAG, "BsbSelect %08X: unknown value %d", get_field_id(), int_value);
        }
      }

      void publish() {
        if (this->has_state()) {
          publish_state(this->current_option());
        }
      }

      bool is_ready_to_update( const uint32_t timestamp ) {
        if( sent_get_ >= 5 ) {
          ESP_LOGE( TAG, "BsbSelect Get %08X: retries exhausted, next try in %fs ", get_field_id(), retry_interval_ms_ / 1000. );

          if( timestamp >= ( next_update_timestamp_ + retry_interval_ms_ ) ) {
            ESP_LOGE( TAG, "BsbSelect Get %08X: retrying", get_field_id() );
            sent_get_ = 0;
            return true;
          }
        }
        return ( sent_get_ < 5 ) && ( timestamp >= next_update_timestamp_ );
      }

      bool is_ready_to_set( const uint32_t timestamp ) {
        if( sent_set_ >= 5 ) {
          ESP_LOGE( TAG, "BsbSelect Set %08X: retries exhausted, next try in %fs ", get_field_id(), retry_interval_ms_ / 1000. );

          if( timestamp >= ( next_update_timestamp_ + retry_interval_ms_ ) ) {
            ESP_LOGE( TAG, "BsbSelect Set %08X: retrying", get_field_id() );
            sent_set_ = 0;
            return true;
          }
        }
        return ( sent_set_ < 5 ) && dirty_;
      }

      void schedule_next_regular_update( const uint32_t timestamp ) {
        sent_get_              = 0;
        next_update_timestamp_ = timestamp + update_interval_ms_;
      }

      void schedule_next_update( const uint32_t timestamp, const uint32_t interval ) {
        sent_get_              = 0;
        next_update_timestamp_ = timestamp + interval;
      }

      void reset_dirty() {
        sent_set_ = 0;
        dirty_    = false;
      }

      const BsbPacket createPackageSet( uint8_t source_address, uint8_t destination_address ) {
        ++sent_set_;
        return BsbPacketSetInt8(
          source_address, destination_address, get_field_id(), value_to_send_, enable_byte_ );
      }

      const BsbPacket createPackageGet( uint8_t source_address, uint8_t destination_address ) {
        ++sent_get_;
        return BsbPacketGet( source_address, destination_address, get_field_id() );
      }

    protected:
      void control( const std::string& value ) override {
        auto it = option_to_value_.find(value);
        if (it != option_to_value_.end()) {
          value_to_send_ = it->second;
          dirty_ = true;
          publish_state(value);
        } else {
          ESP_LOGW(TAG, "BsbSelect %08X: unknown option '%s'", get_field_id(), value.c_str());
        }
      }

      uint32_t field_id_ = 0;
      uint8_t enable_byte_ = 0x01;

      uint32_t update_interval_ms_;
      uint32_t retry_interval_ms_;
      uint8_t retry_count_;

      uint32_t next_update_timestamp_ = 0;

      uint16_t sent_set_ = 0;
      uint16_t sent_get_ = 0;
      bool dirty_ = false;

      int8_t value_to_send_ = 0;

      std::map<int8_t, std::string> value_to_option_;
      std::map<std::string, int8_t> option_to_value_;
    };

  } // namespace bsb
} // namespace esphome
