#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ads131m04 {

// ADS131M04 Commands (16-bit)
static const uint16_t ADS131M04_NULL_CMD = 0x0000;
static const uint16_t ADS131M04_RESET = 0x0011;
static const uint16_t ADS131M04_LOCK = 0x0555;
static const uint16_t ADS131M04_UNLOCK = 0x0655;

// ADS131M04 Registers
static const uint8_t ADS131M04_REG_ID = 0x00;
static const uint8_t ADS131M04_REG_STATUS = 0x01;
static const uint8_t ADS131M04_REG_MODE = 0x02;

class ADS131M04 : public Component, public spi::SPIDevice {
 public:
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_channel_voltage_sensor(uint8_t channel, sensor::Sensor *voltage_sensor) {
    if (channel >= 1 && channel <= 4) {
      voltage_sensors_[channel - 1] = voltage_sensor;
    }
  }

  void set_channel_current_sensor(uint8_t channel, sensor::Sensor *current_sensor) {
    if (channel >= 1 && channel <= 4) {
      current_sensors_[channel - 1] = current_sensor;
    }
  }

 protected:
  bool write_register_(uint8_t reg, uint16_t value);
  uint16_t read_register_(uint8_t reg);
  void read_data_();

  sensor::Sensor *voltage_sensors_[4] = {nullptr, nullptr, nullptr, nullptr};
  sensor::Sensor *current_sensors_[4] = {nullptr, nullptr, nullptr, nullptr};

  // Calibration factors
  static constexpr float VOLTAGE_SCALE = 2.048f / 8388608.0f;  // 2.048V reference / 2^23
  static constexpr float CURRENT_SCALE = VOLTAGE_SCALE / 100.0f;  // Assuming 0.01Ω shunt
};

}  // namespace ads131m04
}  // namespace esphome
