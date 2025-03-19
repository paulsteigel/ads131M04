#include "ads131m04.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";

void ADS131M04::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04...");

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
  return (msb << 8) | lsb;
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

void ADS131M04::loop() {
  if (this->is_failed())
    return;

  this->read_data_();
}

}  // namespace ads131m04
}  // namespace esphome
