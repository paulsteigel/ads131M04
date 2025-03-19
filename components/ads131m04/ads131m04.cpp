#include "ads131m04.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";

void IRAM_ATTR HOT ADS131M04::gpio_intr(ADS131M04 *arg) {
  arg->data_ready_ = true;
}

void ADS131M04::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04...");

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);  // Active low reset
  }

  if (this->drdy_pin_ != nullptr) {
    this->drdy_pin_->setup();
    this->isr_pin_ = new ISRInternalGPIOPin(this->drdy_pin_);
    this->isr_pin_->attach_interrupt(ADS131M04::gpio_intr, this, FALLING);
  }

  // Perform reset
  if (!this->reset_()) {
    ESP_LOGE(TAG, "Reset failed!");
    this->mark_failed();
    return;
  }

  delay(100);  // Wait for device to stabilize after reset

  // Initialize
  if (!this->init_()) {
    ESP_LOGE(TAG, "Initialization failed!");
    this->mark_failed();
    return;
  }

  // Read and verify device ID
  uint16_t id = this->read_register_(ADS131M04_REG_ID);
  ESP_LOGD(TAG, "Device ID: 0x%04X", id);

  if ((id & 0xFF00) != 0x4000) {
    ESP_LOGE(TAG, "Invalid device ID!");
    this->mark_failed();
    return;
  }
}

void ADS131M04::dump_config() {
  ESP_LOGCONFIG(TAG, "ADS131M04:");
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  DRDY Pin: ", this->drdy_pin_);
  ESP_LOGCONFIG(TAG, "  Sampling Rate: %u Hz", this->sampling_rate_);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication failed!");
    return;
  }

  for (int i = 0; i < 4; i++) {
    if (this->voltage_sensors_[i] != nullptr || this->current_sensors_[i] != nullptr) {
      ESP_LOGCONFIG(TAG, "  Channel %d:", i + 1);
      if (this->voltage_sensors_[i] != nullptr)
        ESP_LOGCONFIG(TAG, "    Voltage Sensor: YES");
      if (this->current_sensors_[i] != nullptr)
        ESP_LOGCONFIG(TAG, "    Current Sensor: YES");
    }
  }
}

bool ADS131M04::write_register_(uint8_t reg, uint16_t value) {
  ESP_LOGVV(TAG, "Writing register 0x%02X = 0x%04X", reg, value);

  this->enable();
  this->write_byte(0x40 | reg);  // Write command
  this->write_byte(0x00);        // Reserved
  this->write_byte((value >> 8) & 0xFF);
  this->write_byte(value & 0xFF);
  this->disable();

  return true;
}

uint16_t ADS131M04::read_register_(uint8_t reg) {
  this->enable();
  this->write_byte(0x20 | reg);  // Read command
  this->write_byte(0x00);        // Reserved
  uint8_t msb = this->read_byte();
  uint8_t lsb = this->read_byte();
  this->disable();

  uint16_t value = (msb << 8) | lsb;
  ESP_LOGVV(TAG, "Read register 0x%02X = 0x%04X", reg, value);

  return value;
}

void ADS131M04::read_data_() {
  uint8_t data[16];  // 4 bytes per channel

  this->enable();
  this->write_byte(ADS131M04_NULL_CMD >> 8);
  this->write_byte(ADS131M04_NULL_CMD & 0xFF);
  this->read_array(data, 16);
  this->disable();

  // Process channel data
  for (int i = 0; i < 4; i++) {
    int32_t raw_value = (data[i*4] << 24) | (data[i*4+1] << 16) | (data[i*4+2] << 8) | data[i*4+3];

    if (this->voltage_sensors_[i] != nullptr) {
      float voltage = raw_value * VOLTAGE_SCALE;
      this->voltage_sensors_[i]->publish_state(voltage);
      ESP_LOGV(TAG, "CH%d Voltage: %.6f V", i + 1, voltage);
    }

    if (this->current_sensors_[i] != nullptr) {
      float current = raw_value * CURRENT_SCALE;
      this->current_sensors_[i]->publish_state(current);
      ESP_LOGV(TAG, "CH%d Current: %.6f A", i + 1, current);
    }
  }
}

bool ADS131M04::reset_() {
  ESP_LOGD(TAG, "Resetting ADS131M04");

  if (this->reset_pin_ != nullptr) {
    // Hardware reset
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(10);
  } else {
    // Software reset
    this->enable();
    this->write_byte(ADS131M04_RESET >> 8);
    this->write_byte(ADS131M04_RESET & 0xFF);
    this->disable();
  }

  delay(10);
  return true;
}

bool ADS131M04::init_() {
  ESP_LOGD(TAG, "Initializing ADS131M04");

  // Unlock registers
  if (!this->write_register_(ADS131M04_UNLOCK, 0x0000)) {
    ESP_LOGE(TAG, "Failed to unlock registers");
    return false;
  }

  // Configure MODE register for continuous conversion
  if (!this->write_register_(ADS131M04_REG_MODE, ADS131M04_MODE_CONTINUOUS)) {
    ESP_LOGE(TAG, "Failed to configure MODE register");
    return false;
  }

  // Lock registers
  if (!this->write_register_(ADS131M04_LOCK, 0x0000)) {
    ESP_LOGE(TAG, "Failed to lock registers");
    return false;
  }

  return true;
}

void ADS131M04::loop() {
  if (this->is_failed())
    return;

  if (this->drdy_pin_ != nullptr) {
    // Interrupt-driven mode
    if (this->data_ready_) {
      this->data_ready_ = false;
      this->read_data_();
    }
  } else {
    // Polling mode - read at regular intervals
    this->read_data_();
  }
}

}  // namespace ads131m04
}  // namespace esphome
