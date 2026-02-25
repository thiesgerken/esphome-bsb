#pragma once

#include <map>
#include <string>

#include "bsbPacketSend.h"

#include "esphome/components/sensor/sensor.h"

#ifdef USE_BINARY_SENSOR
  #include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
  #include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome {
  namespace bsb {

    enum SensorType { Sensor, TextSensor, BinarySensor };

    enum class BsbSensorValueType { UInt8, Int8, Int16, Int32, Temperature, RoomTemperature, DateTime };

    class BsbSensorBase {
    public:
      virtual SensorType get_type() = 0;
      virtual void       publish()  = 0;

      void           set_field_id( const uint32_t field_id ) { this->field_id_ = field_id; }
      const uint32_t get_field_id() const { return field_id_; }

      void           set_update_interval( const uint32_t update_interval_ms ) { update_interval_ms_ = update_interval_ms; }
      const uint32_t get_update_interval() const { return update_interval_ms_; }

      void set_retry_interval( const uint32_t retry_interval_ms ) { retry_interval_ms_ = retry_interval_ms; }
      void set_retry_count( uint8_t retry_count ) { retry_count_ = retry_count; }

      void                     set_value_type( const int value_type ) { this->value_type_ = ( BsbSensorValueType )value_type; }
      const BsbSensorValueType get_value_type() const { return this->value_type_; }

      const bool is_ready( const uint32_t timestamp ) {
        if( sent_get_ >= 5 ) {
          if( !logged_exhaustion_ ) {
            ESP_LOGW( TAG, "BsbSensor Get %08X: retries exhausted, waiting %.0fs before retry", get_field_id(), retry_interval_ms_ / 1000. );
            logged_exhaustion_ = true;
            retry_start_timestamp_ = timestamp;
          }
          if( timestamp >= ( retry_start_timestamp_ + retry_interval_ms_ ) ) {
            ESP_LOGI( TAG, "BsbSensor Get %08X: retrying after wait", get_field_id() );
            sent_get_ = 0;
            logged_exhaustion_ = false;
            return true;
          }
          return false;
        }

        return timestamp >= next_update_timestamp_;
      }

      void schedule_next_regular_update( const uint32_t timestamp ) {
        sent_get_              = 0;
        logged_exhaustion_     = false;
        next_update_timestamp_ = timestamp + update_interval_ms_;
      }

      const BsbPacket createPackageGet( uint8_t source_address, uint8_t destination_address ) {
        ++sent_get_;

        return BsbPacketGet( source_address, destination_address, get_field_id() );
      }

    protected:
      uint32_t           field_id_;
      BsbSensorValueType value_type_ = BsbSensorValueType::Temperature;

      uint32_t update_interval_ms_;
      uint32_t retry_interval_ms_;
      uint8_t  retry_count_;

    private:
      uint32_t next_update_timestamp_ = 0;
      uint32_t retry_start_timestamp_ = 0;
      uint16_t sent_get_              = 0;
      bool     logged_exhaustion_     = false;
    };

    class BsbSensor
        : public BsbSensorBase
        , public sensor::Sensor {
    public:
      SensorType get_type() override { return SensorType::Sensor; }
      void       publish() override { publish_state( value_ ); }

      void set_enable_byte( const uint8_t enable_byte ) { this->enable_byte_ = enable_byte; }

      void set_value( float value ) { this->value_ = value * factor_ / divisor_; }

      void        set_divisor( const float divisor ) { this->divisor_ = divisor; }
      const float get_divisor() const { return this->divisor_; }

      void        set_factor( const float factor ) { this->factor_ = factor; }
      const float get_factor() const { return this->factor_; }

    protected:
      float   divisor_     = 1.;
      float   factor_      = 1.;
      uint8_t enable_byte_ = 0x01;

      float value_;
    };

#ifdef USE_TEXT_SENSOR
    class BsbTextSensor
        : public BsbSensorBase
        , public text_sensor::TextSensor {
    public:
      SensorType get_type() override { return SensorType::TextSensor; }
      void       publish() override { publish_state( value_ ); }

      void set_value( const std::string value ) { this->value_ = value; }

      void set_value_int( int8_t value ) {
        auto it = value_to_option_.find(value);
        if (it != value_to_option_.end()) {
          this->value_ = it->second;
        } else {
          this->value_ = std::to_string(value);
        }
      }

      void add_option_mapping( int8_t value, const std::string& option ) {
        value_to_option_[value] = option;
      }

      bool has_enum_mapping() const { return !value_to_option_.empty(); }

    protected:
      std::string value_;
      std::map<int8_t, std::string> value_to_option_;
    };
#endif

#ifdef USE_BINARY_SENSOR
    class BsbBinarySensor
        : public BsbSensorBase
        , public binary_sensor::BinarySensor {
    public:
      SensorType get_type() override { return SensorType::BinarySensor; }
      void       publish() override { publish_state( value_ ); }

      void set_enable_byte( const uint8_t enable_byte ) { this->enable_byte_ = enable_byte; }

      // BSB VT_ONOFF uses 0x00 for off and either 0x01 or 0xFF for on,
      // so any non-zero value is treated as on.
      void set_value( uint8_t value ) {
        value_ = (value != off_value_);
      }

      void          set_on_value( const uint8_t on_value ) { this->on_value_ = on_value; }
      const uint8_t get_on_value() const { return this->on_value_; }

      void          set_off_value( const uint8_t off_value ) { this->off_value_ = off_value; }
      const uint8_t get_off_value() const { return this->off_value_; }

    protected:
      uint8_t on_value_    = 1;
      uint8_t off_value_   = 0;
      uint8_t enable_byte_ = 0x01;

      bool value_;
    };
#endif

  } // namespace bsb
} // namespace esphome
