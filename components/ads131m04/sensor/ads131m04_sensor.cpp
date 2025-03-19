#include "ads131m04_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {
static const char *const TAG = "ads131m04.sensor";

void ADS131M04Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS131M04 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %u", this->gain_);
}

ACValues ADS131M04Sensor::sample() {
  adcOutput res = this->parent_->readADC();
  ACValues ac_values;

  // Assign channels to AC Line 1 and AC Line 2
  int32_t current_line1_adc = res.ch0;
  int32_t current_line2_adc = res.ch1;
  int32_t voltage_line1_adc = res.ch2;
  int32_t voltage_line2_adc = res.ch3;

  // Convert to actual voltage and current values
  ac_values.current = this->parent_->convert(current_line1_adc, this->gain_); //current line 1
  ac_values.voltage = this->parent_->convert(voltage_line1_adc, this->gain_); //voltage line 1

  return ac_values;
}

void ADS131M04Sensor::update() {
  ACValues ac_values = this->sample();
  ESP_LOGD(TAG, "'%s': Got Voltage=%fV, Current=%fA", this->get_name().c_str(), ac_values.voltage, ac_values.current);
  // Publish voltage and current states separately
  this->publish_state(ac_values.voltage); // Publish voltage
  // Publish current as a separate sensor if needed.
}

/*
void ADS131M04Sensor::update() {
  float v = this->sample();
  if (!std::isnan(v)) {
    ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    this->publish_state(v);
  }
}
*/
void ADS131M04Sensor::set_channel(uint8_t channel) {
  this->channel_ = channel;
}

}  // namespace ads131m04
}  // namespace esphome
