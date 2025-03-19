#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/sensor/sensor.h"
#include "../ads131m04.h"

namespace esphome {
namespace ads131m04 {

struct ACValues {
  float voltage;
  float current;
};

class ADS131M04Sensor : public PollingComponent,
                         public sensor::Sensor,
                         public Parented<ADS131M04> {
public: 
  void update() override;
  ACValues sample(); // Modified return type
  void dump_config() override;
  void set_gain(ADS131M04Gain gain) { this->gain_ = gain; }
  //ADS131M04Gain get_gain() { return this->gain_; }
  void set_channel(uint8_t channel);

protected:
  //ADS131M04Gain gain_ = ADS131M04Gain::ADS131M04_GAIN_1; // Initialize with a default gain
  ADS131M04Gain gain_{ADS131M04_GAIN_1};
  uint8_t channel_; // Add channel_ member variable
};

}  // namespace ads131m04
}  // namespace esphome
