#include "ads131m04.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";

void ADS131M04Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04...");
  if (!init_()) {
    ESP_LOGE(TAG, "ADS131M04 initialization failed.");
  }

  if (drdy_pin_ != nullptr) {
    isr_pin_ = make_unique<ISRInternalGPIOPin>(drdy_pin_);
    isr_pin_->setup();
    isr_pin_->attach_interrupt(gpio_intr, this, FALLING_EDGE);
  }
}

void ADS131M04Component::loop() {
  if (drdy_pin_ == nullptr) {
    read_data_();
  }
}

void ADS131M04Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ADS131M04:");
  ESP_LOGCONFIG(TAG, "  Reset Pin: %u", reset_pin_->get_pin());
  ESP_LOGCONFIG(TAG, "  DRDY Pin: %u", drdy_pin_->get_pin());
  ESP_LOGCONFIG(TAG, "  Sampling Rate: %u Hz", sampling_rate_);
}

bool ADS131M04Component::write_register_(uint8_t reg, uint16_t value) {
  uint8_t tx_buffer[3] = {reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
  return this->transfer(tx_buffer, nullptr, 3, 0);
}

uint16_t ADS131M04Component::read_register_(uint8_t reg) {
  uint8_t tx_buffer[3] = {reg | 0x80, 0x00, 0x00};
  uint8_t rx_buffer[3];
  if (!this->transfer(tx_buffer, rx_buffer, 3, 3)) {
    ESP_LOGE(TAG, "Failed to read register 0x%02X", reg);
    return 0;
  }
  return ((uint16_t)rx_buffer[1] << 8) | rx_buffer[2];
}

bool ADS131M04Component::reset_() {
  if (reset_pin_ == nullptr) {
    ESP_LOGE(TAG, "Reset pin is not configured.");
    return false;
  }
  reset_pin_->digital_write(false);
  delay(10);
  reset_pin_->digital_write(true);
  delay(100);
  return true;
}

bool ADS131M04Component::init_() {
  if (!reset_()) {
    return false;
  }
  if (read_register_(ADS131M04_REG_ID) != 0x4310) {
    ESP_LOGE(TAG, "ADS131M04 ID mismatch.");
    return false;
  }
  if (!write_register_(ADS131M04_REG_MODE, ADS131M04_MODE_CONTINUOUS)) {
    return false;
  }
  if (!write_register_(ADS131M04_REG_CLOCK, 0x0000)) { // Example clock config
    return false;
  }
  return true;
}

void ADS131M04Component::read_data_() {
  uint8_t tx_buffer[1] = {0x12}; // RDATA command
  uint8_t rx_buffer[12];
  if (!this->transfer(tx_buffer, rx_buffer, 1, 12)) {
    ESP_LOGE(TAG, "Failed to read data.");
    return;
  }
  for (int i = 0; i < 4; ++i) {
    int32_t raw_value = ((uint32_t)rx_buffer[i * 3] << 16) | ((uint32_t)rx_buffer[i * 3 + 1] << 8) | rx_buffer[i * 3 + 2];
    voltages_[i] = (float)raw_value * VOLTAGE_SCALE;
    currents_[i] = voltages_[i] / 100.0f; // Assuming 0.01Ω shunt
  }
  data_ready_ = true;
}

void IRAM_ATTR ADS131M04Component::gpio_intr(ADS131M04Component *arg) {
  arg->read_data_();
}

} // namespace ads131m04
} // namespace esphome
