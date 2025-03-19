#pragma once

#include "esphome/components/spi/spi.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ads131m04 {

// Commands (from Pico implementation)
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
static const uint8_t ADS131M04_REG_GAIN = 0x04;
static const uint8_t ADS131M04_REG_CFG = 0x06;

enum ADS131M04Gain {
  ADS131M04_GAIN_1 = 0b000,
  ADS131M04_GAIN_2 = 0b001,
  ADS131M04_GAIN_4 = 0b010,
  ADS131M04_GAIN_8 = 0b011,
  ADS131M04_GAIN_16 = 0b100,
  ADS131M04_GAIN_32 = 0b101,
  ADS131M04_GAIN_64 = 0b110,
  ADS131M04_GAIN_128 = 0b111,
};

class ADS131M04 : public Component,
                public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_TRAILING,
                                      spi::DATA_RATE_8MHZ> {
 public:
  ADS131M04() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Helper method to read measurements
  float request_measurement(uint8_t channel, ADS131M04Gain gain);

 protected:
  bool write_register_(uint8_t reg, uint16_t value);
  uint16_t read_register_(uint8_t reg);
  void read_data_();
  uint16_t config_{0};
};

}  // namespace ads131m04
}  // namespace esphome
