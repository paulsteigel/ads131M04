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
  void set_channel(uint8_t channel);
  id set_voltage_offset(float offset);
  void set_voltage_scale(float scale);
  void set_current_offset(float offset);
  void set_current_scale(float scale);

protected:
  float calculateRMS(const int32_t* samples, int numSamples, float offset, float scale);
  //ADS131M04Gain gain_ = ADS131M04Gain::ADS131M04_GAIN_1; // Initialize with a default gain
  ADS131M04Gain gain_{ADS131M04_GAIN_1};
  uint8_t channel_; // Add channel_ member variable
  float voltage_offset_ = 0.0f;
  float voltage_scale_ = 1.0f;
  float current_offset_ = 0.0f;
  float current_scale_ = 1.0f;
};

}  // namespace ads131m04
}  // namespace esphome
