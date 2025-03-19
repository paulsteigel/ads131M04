#pragma once

#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace ads131m04 {

// Commands (16-bit)
static const uint16_t ADS131M04_NULL_CMD = 0x0000;
static const uint16_t ADS131M04_RESET = 0x0011;
static const uint16_t ADS131M04_STANDBY = 0x0022;
static const uint16_t ADS131M04_WAKEUP = 0x0033;
static const uint16_t ADS131M04_LOCK = 0x0555;
static const uint16_t ADS131M04_UNLOCK = 0x0655;

// Registers
static const uint8_t ADS131M04_REG_ID = 0x00;
static const uint8_t ADS131M04_REG_STATUS = 0x01;
static const uint8_t ADS131M04_REG_MODE = 0x02;
static const uint8_t ADS131M04_REG_CLOCK = 0x03;

// Register values
static const uint16_t ADS131M04_MODE_CONTINUOUS = 0x0100;

class ADS131M04Component : public Component, public spi::SPIDevice {
 public:
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_reset_pin(GPIOPin *reset_pin) { reset_pin_ = reset_pin; }
  void set_drdy_pin(GPIOPin *drdy_pin) { drdy_pin_ = drdy_pin; }
  void set_sampling_rate(uint32_t rate) { sampling_rate_ = rate; }

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
  static void gpio_intr(ADS131M04Component *arg);

  bool write_register_(uint8_t reg, uint16_t value);
  uint16_t read_register_(uint8_t reg);
  bool reset_();
  bool init_();
  void read_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *drdy_pin_{nullptr};
  ISRInternalGPIOPin *isr_pin_{nullptr};

  sensor::Sensor *voltage_sensors_[4] = {nullptr, nullptr, nullptr, nullptr};
  sensor::Sensor *current_sensors_[4] = {nullptr, nullptr, nullptr, nullptr};
  uint32_t sampling_rate_{16000};  // Default sampling rate in Hz

  volatile bool data_ready_{false};

  // Calibration factors
  static constexpr float VOLTAGE_SCALE = 2.048f / 8388608.0f;  // 2.048V reference / 2^23
  static constexpr float CURRENT_SCALE = VOLTAGE_SCALE / 100.0f;  // Assuming 0.01Ω shunt
};

}  // namespace ads131m04
}  // namespace esphome
