#include "ads131m04.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";

void ADS131M04::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04...");
  this->spi_setup();

  // Read and verify device ID
  uint16_t id = this->read_register_(ADS131M04_REG_ID);
  ESP_LOGD(TAG, "Device ID: 0x%04X", id);

  // Unlock registers
  this->write_register_(ADS131M04_UNLOCK, 0x0000);

  // Configure device for continuous conversion
  this->write_register_(ADS131M04_REG_MODE, 0x0000);  // Continuous mode

  // Set gain and other default settings
  this->write_register_(ADS131M04_REG_GAIN, 0x0000);  // Unity gain for all channels

  // Lock registers
  this->write_register_(ADS131M04_LOCK, 0x0000);
}

void ADS131M04::dump_config() {
  ESP_LOGCONFIG(TAG, "ADS131M04:");
  LOG_PIN("  CS Pin: ", this->cs_);
}

bool ADS131M04::write_register_(uint8_t reg, uint16_t value) {
  this->enable();
  delayMicroseconds(1);

  this->write_byte(0x40 | reg);  // Write command
  this->write_byte(0x00);        // Reserved
  this->write_byte((value >> 8) & 0xFF);
  this->write_byte(value & 0xFF);

  delayMicroseconds(1);
  this->disable();
  return true;
}

uint16_t ADS131M04::read_register_(uint8_t reg) {
  this->enable();
  delayMicroseconds(1);

  this->write_byte(0x20 | reg);  // Read command
  this->write_byte(0x00);        // Reserved
  uint8_t msb = this->read_byte();
  uint8_t lsb = this->read_byte();

  delayMicroseconds(1);
  this->disable();
  return (msb << 8) | lsb;
}

float ADS131M04::request_measurement(uint8_t channel, ADS131M04Gain gain) {
  if (channel > 3)
    return NAN;

  // Set gain for the specified channel
  uint16_t gain_reg = this->read_register_(ADS131M04_REG_GAIN);
  gain_reg &= ~(0x7 << (channel * 4));  // Clear current gain
  gain_reg |= (gain << (channel * 4));   // Set new gain
  this->write_register_(ADS131M04_REG_GAIN, gain_reg);

  // Read the data
  this->read_data_();

  // Convert based on gain and 1.2V reference
  float scale = 1.2f / (8388608.0f * (1 << gain));
  return scale;  // Return the scaled value
}

void ADS131M04::read_data_() {
  this->enable();
  delayMicroseconds(1);

  // Send NULL command to read data
  this->write_byte(ADS131M04_NULL_CMD >> 8);
  this->write_byte(ADS131M04_NULL_CMD & 0xFF);

  // Status word (2 bytes)
  uint8_t status_msb = this->read_byte();
  uint8_t status_lsb = this->read_byte();
  uint16_t status = (status_msb << 8) | status_lsb;

  // Read all channels (24 bits each)
  for (int i = 0; i < 4; i++) {
    uint32_t raw_value = 0;
    for (int j = 0; j < 3; j++) {
      raw_value = (raw_value << 8) | this->read_byte();
    }

    // Convert to signed 24-bit
    int32_t value = raw_value & 0x00FFFFFF;
    if (value & 0x800000)
      value |= 0xFF000000;  // Sign extend

    ESP_LOGVV(TAG, "Channel %d raw value: %d", i, value);
  }

  delayMicroseconds(1);
  this->disable();
}

void ADS131M04::loop() {
  this->read_data_();
  delay(10);  // Don't read too fast
}

}  // namespace ads131m04
}  // namespace esphome
