#include "ads131m04_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04.sensor";

void ADS131M04Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS131M04 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Channel: %u", this->channel_);
  ESP_LOGCONFIG(TAG, "    Gain: %u", this->gain_);
}

float ADS131M04Sensor::sample() {
  return this->parent_->request_measurement(this->channel_, this->gain_);
}

void ADS131M04Sensor::update() {
  float v = this->sample();
  if (!std::isnan(v)) {
    ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    this->publish_state(v);
  }
}

}  // namespace ads131m04
}  // namespace esphome
