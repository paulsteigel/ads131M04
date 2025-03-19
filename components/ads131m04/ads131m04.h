#include "ads131m04.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";

void ADS131M04::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04...");

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    this->reset_();
  }

  this->spi_setup();

  // Read and verify device ID
  uint16_t id = this->read_register_(ADS131M04_REG_ID);
  ESP_LOGD(TAG, "Device ID: 0x%04X", id);

  if ((id & 0xFF00) != 0x4000) {
    ESP_LOGE(TAG, "Invalid device ID!");
    this->mark_failed();
    return;
  }

  // Configure device for continuous conversion
  if (!this->write_register_(ADS131M04_REG_MODE, ADS131M04_MODE_CONTINUOUS)) {
    ESP_LOGE(TAG, "Failed to configure MODE register");
    this->mark_failed();
    return;
  }
}

void ADS131M04::dump_config() {
  ESP_LOGCONFIG(TAG, "ADS131M04:");
  LOG_PIN("  CS Pin:", this->cs_);
  LOG_PIN("  Reset Pin:", this->reset_pin_);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication failed!");
    return;
  }
}

void ADS131M04::reset_() {
  if (this->reset_pin_ == nullptr)
    return;

  ESP_LOGD(TAG, "Performing hardware reset");
  this->reset_pin_->digital_write(false);
  delay(10);  // Hold reset low for 10ms
  this->reset_pin_->digital_write(true);
  delay(10);  // Wait for 10ms after reset
}

bool ADS131M04::write_register_(uint8_t reg, uint16_t value) {
  this->enable();
  delayMicroseconds(1);

  // SPI frame structure from Pico implementation
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

void ADS131M04::read_data_() {
  uint8_t data[16];  // 4 bytes per channel

  this->enable();
  delayMicroseconds(1);

  // Send NULL command to read data
  this->write_byte(ADS131M04_NULL_CMD >> 8);
  this->write_byte(ADS131M04_NULL_CMD & 0xFF);

  // Status word - 2 bytes
  uint8_t status_msb = this->read_byte();
  uint8_t status_lsb = this->read_byte();
  uint16_t status = (status_msb << 8) | status_lsb;

  // Read channel data (4 bytes per channel)
  for (int i = 0; i < 4; i++) {
    data[i*4] = this->read_byte();
    data[i*4+1] = this->read_byte();
    data[i*4+2] = this->read_byte();
    data[i*4+3] = this->read_byte();
  }

  delayMicroseconds(1);
  this->disable();

  // Process channel data
  for (int i = 0; i < 4; i++) {
    int32_t raw_value = (data[i*4] << 24) | (data[i*4+1] << 16) | (data[i*4+2] << 8) | data[i*4+3];

    // Two's complement conversion (if bit 23 is set)
    if (raw_value & 0x800000) {
      raw_value -= 0x1000000;
    }

    if (this->voltage_sensors_[i] != nullptr) {
      float voltage = raw_value * VOLTAGE_SCALE;
      this->voltage_sensors_[i]->publish_state(voltage);
    }

    if (this->current_sensors_[i] != nullptr) {
      float current = raw_value * CURRENT_SCALE;
      this->current_sensors_[i]->publish_state(current);
    }
  }
}

void ADS131M04::loop() {
  if (this->is_failed())
    return;

  this->read_data_();
}

}  // namespace ads131m04
}  // namespace esphome
