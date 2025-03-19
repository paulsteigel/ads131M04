#pragma once

#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/spi/spi.h"
//#include "esphome/components/gpio/gpio.h"

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
