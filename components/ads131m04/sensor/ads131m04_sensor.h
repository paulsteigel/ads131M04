#pragma once

#include "esphome/components/sensor/sensor.h"
#include "../ads131m04.h"

namespace esphome {
namespace ads131m04_sensor {

class ADS131M04Sensor : public sensor::Sensor {
public:
  void set_parent(ads131m04::ADS131M04Component *parent) { parent_ = parent; }
  void set_channel(uint8_t channel) { channel_ = channel; }
  void set_is_voltage(bool is_voltage) { is_voltage_ = is_voltage; }
  void update() override;

protected:
  ads131m04::ADS131M04Component *parent_;
  uint8_t channel_;
  bool is_voltage_;
};

} // namespace ads131m04_sensor
} // namespace esphome
