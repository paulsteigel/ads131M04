#include "ads131m04_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {
static const char *const TAG = "ads131m04.sensor";

void ADS131M04Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS131M04 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %u", this->gain_);
}

float ADS131M04Sensor::sample() {
  adcOutput res = this->parent_->readADC();
  float voltage = 0.0f;
  switch (this->channel_) { // Corrected line
    case 0:
      voltage = this->parent_->convert(res.ch0, this->gain_);
      break;
    case 1:
      voltage = this->parent_->convert(res.ch1, this->gain_);
      break;
    case 2:
      voltage = this->parent_->convert(res.ch2, this->gain_);
      break;
    case 3:
      voltage = this->parent_->convert(res.ch3, this->gain_);
      break;
    default:
      ESP_LOGE(TAG, "Invalid sensor number: %u", this->channel_);
      return NAN;
  }
  return voltage;
}

void ADS131M04Sensor::update() {
  float v = this->sample();
  if (!std::isnan(v)) {
    ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    this->publish_state(v);
  }
}

void ADS131M04Sensor::set_channel(uint8_t channel) {
  this->channel_ = channel;
}

}  // namespace ads131m04
}  // namespace esphome
