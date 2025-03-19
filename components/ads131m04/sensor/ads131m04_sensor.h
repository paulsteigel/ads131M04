#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

#include "esphome/components/sensor/sensor.h"

#include "../ads131m04.h"

namespace esphome {
namespace ads131m04 {

class ADS131M04Sensor : public PollingComponent,
                         public sensor::Sensor,
                         public Parented<ADS131M04> {
public: 
  //void set_multiplexer(ADS1118Multiplexer multiplexer) { this->multiplexer_ = multiplexer; }  
  void update() override;
  float sample(); // Removed 'override'
  void dump_config() override;

protected:
  /** Sensor number (1-4). */
  uint8_t sensor_num_;
  /** Multiplexer setting (string). */
  std::string multiplexer_;
  /** Gain setting (string). */
  std::string gain_;
};

}  // namespace ads131m04
}  // namespace esphome
