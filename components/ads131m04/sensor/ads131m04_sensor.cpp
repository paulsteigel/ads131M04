#include "ads131m04_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04.sensor";

void ADS131M04Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS131M04 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %u", this->gain_);
  //ESP_LOGCONFIG(TAG, "    Gain: %u", static_cast<uint8_t>(this->gain_));
}

float ADS131M04Sensor::sample() {
  float voltage = 0.0f;
  /*
  adcOutput res = this->parent_->readADC();
  float voltage = 0.0f;
  switch (sensor_num_) {
    case 1:
      voltage = this->parent_->convert(res.ch0, gain_);
      break;
    case 2:
      voltage = this->parent_->convert(res.ch1, gain_);
      break;
    case 3:
      voltage = this->parent_->convert(res.ch2, gain_);
      break;
    case 4:
      voltage = this->parent_->convert(res.ch3, gain_);
      break;
    default:
      ESP_LOGE(TAG, "Invalid sensor number: %u", sensor_num_);      
      return NAN;
  }
  */
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
