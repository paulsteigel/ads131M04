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

  void set_multiplexer(ADS131M04Multiplexer multiplexer) { this->multiplexer_ = multiplexer; }
  void set_gain(ADS131M04Gain gain) { this->gain_ = gain; }
  void set_temperature_mode(bool temp) { this->temperature_mode_ = temp; }

  float sample() override;

  void dump_config() override;

 protected:
  ADS131M04Multiplexer multiplexer_{ADS131M04_MULTIPLEXER_P0_NG};
  ADS131M04Gain gain_{ADS131M04_GAIN_6P144};
  bool temperature_mode_;
};

}  // namespace ads131m04
}  // namespace esphome
