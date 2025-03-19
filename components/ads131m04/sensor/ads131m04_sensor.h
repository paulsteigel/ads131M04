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
  void set_gain(ADS131M04Gain gain) { this->gain_ = gain; }
  ADS131M04Gain get_gain() { return this->gain_; }

protected:
  /** Gain setting (string). */
  //std::string gain_;
  ADS131M04Gain gain_;
};

}  // namespace ads131m04
}  // namespace esphome
