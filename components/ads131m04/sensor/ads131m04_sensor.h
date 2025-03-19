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
  //void set_gain(ADS131M04Gain gain) { this->gain_ = gain; }
  ADS131M04Gain get_gain() { return this->gain_; }
  void set_channel(uint8_t channel);

protected:
  /** Gain setting (string). */
  ADS131M04Gain gain_ = ADS131M04Gain::ADS131M04_GAIN_1; // Initialize with a default gain
  uint8_t channel_; // Add channel_ member variable
};

}  // namespace ads131m04
}  // namespace esphome
