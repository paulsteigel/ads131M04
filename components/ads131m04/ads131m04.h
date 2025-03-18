#pragma once

#include "esphome/components/spi/spi.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ads131m04 {

enum ADS131M04Multiplexer {
  ADS131M04_MULTIPLEXER_P0_N1 = 0b000,
  ADS131M04_MULTIPLEXER_P0_N3 = 0b001,
  ADS131M04_MULTIPLEXER_P1_N3 = 0b010,
  ADS131M04_MULTIPLEXER_P2_N3 = 0b011,
  ADS131M04_MULTIPLEXER_P0_NG = 0b100,
  ADS131M04_MULTIPLEXER_P1_NG = 0b101,
  ADS131M04_MULTIPLEXER_P2_NG = 0b110,
  ADS131M04_MULTIPLEXER_P3_NG = 0b111,
};

enum ADS131M04Gain {
  ADS131M04_GAIN_6P144 = 0b000,
  ADS131M04_GAIN_4P096 = 0b001,
  ADS131M04_GAIN_2P048 = 0b010,
  ADS131M04_GAIN_1P024 = 0b011,
  ADS131M04_GAIN_0P512 = 0b100,
  ADS131M04_GAIN_0P256 = 0b101,
};

class ADS131M04 : public Component,
                public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_TRAILING,
                                      spi::DATA_RATE_1MHZ> {
 public:
  ADS131M04() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  /// Helper method to request a measurement from a sensor.
  float request_measurement(ADS131M04Multiplexer multiplexer, ADS131M04Gain gain, bool temperature_mode);

 protected:
  uint16_t config_{0};
};

}  // namespace ads131m04
}  // namespace esphome
