#include "ads131m04_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h" // For delayMicroseconds

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04.sensor";

void ADS131M04Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS131M04 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %u", static_cast<uint8_t>(this->gain_));
  ESP_LOGCONFIG(TAG, "    Voltage Calibration Offset: %f", this->voltage_offset_);
  ESP_LOGCONFIG(TAG, "    Voltage Calibration Scale: %f", this->voltage_scale_);
  ESP_LOGCONFIG(TAG, "    Current Calibration Offset: %f", this->current_offset_);
  ESP_LOGCONFIG(TAG, "    Current Calibration Scale: %f", this->current_scale_);
}

ACValues ADS131M04Sensor::sample() {
  const int WINDOW_SIZE = 1000; // Adjust window size as needed
  int32_t current_samples[WINDOW_SIZE];
  int32_t voltage_samples[WINDOW_SIZE];
  ACValues ac_values;
  adcOutput res;

  // Collect samples
  for (int i = 0; i < WINDOW_SIZE; ++i) {
    res = this->parent_->readADC();
    current_samples[i] = res.ch0; // Assuming CH0 is current
    voltage_samples[i] = res.ch2; // Assuming CH2 is voltage
    delayMicroseconds(100); // adjust as needed to reach correct sampling rate
  }

  // Calculate RMS
  ac_values.current = calculateRMS(current_samples, WINDOW_SIZE, this->current_offset_, this->current_scale_);
  ac_values.voltage = calculateRMS(voltage_samples, WINDOW_SIZE, this->voltage_offset_, this->voltage_scale_);

  return ac_values;
}

float ADS131M04Sensor::calculateRMS(const int32_t* samples, int numSamples, float offset, float scale) {
  double sum = 0.0;
  for (int i = 0; i < numSamples; ++i) {
    double sample = this->parent_->convert(samples[i], this->gain_);
    sample = (sample + offset) * scale; // Apply calibration
    sum += sample * sample;
  }
  return sqrt(sum / numSamples);
}

void ADS131M04Sensor::update() {
  ACValues ac_values = this->sample();
  ESP_LOGD(TAG, "'%s': Got Voltage=%fV, Current=%fA", this->get_name().c_str(), ac_values.voltage, ac_values.current);
  this->publish_state(ac_values.voltage);

  // Example: Adaptive update interval
  static float previous_voltage = 0.0f;
  float voltage_change = abs(ac_values.voltage - previous_voltage);
  previous_voltage = ac_values.voltage;

  if (voltage_change > 0.1f) { // Adjust threshold as needed
    this->set_update_interval(100); // Shorter interval if voltage is changing
  } else {
    this->set_update_interval(1000); // Longer interval if voltage is stable
  }
}

void ADS131M04Sensor::set_channel(uint8_t channel) {
  this->channel_ = channel;
}

void ADS131M04Sensor::set_voltage_offset(float offset) {
  this->voltage_offset_ = offset;
}

void ADS131M04Sensor::set_voltage_scale(float scale) {
  this->voltage_scale_ = scale;
}

void ADS131M04Sensor::set_current_offset(float offset) {
  this->current_offset_ = offset;
}

void ADS131M04Sensor::set_current_scale(float scale) {
  this->current_scale_ = scale;
}

}  // namespace ads131m04
}  // namespace esphome
