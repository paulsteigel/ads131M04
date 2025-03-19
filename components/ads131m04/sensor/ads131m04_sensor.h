#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
#include "../ads131m04.h"

namespace esphome {
namespace ads131m04 {

class ADS131M04Sensor : public PollingComponent,
                      public sensor::Sensor,
                      public voltage_sampler::VoltageSampler,
                      public Parented<ADS131M04> {
 public:
  void update() override;
  void dump_config() override;

  void set_channel(uint8_t channel) { this->channel_ = channel; }
  void set_gain(ADS131M04Gain gain) { this->gain_ = gain; }

  float sample() override;

 protected:
  uint8_t channel_{0};  // Channel 0-3
  ADS131M04Gain gain_{ADS131M04_GAIN_1};  // Default to unity gain
};

}  // namespace ads131m04
}  // namespace esphome
