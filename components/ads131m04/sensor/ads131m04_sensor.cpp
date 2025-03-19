#include "ads131m04_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04_sensor {

static const char *const TAG = "ads131m04_sensor";

void ADS131M04Sensor::update() {
  if (this->parent_->data_ready_) {
    if (this->channel_ >= 1 && this->channel_ <= 4) {
      if (this->is_voltage_) {
        this->publish_state(this->parent_->voltages_[this->channel_ - 1]);
      } else {
        this->publish_state(this->parent_->currents_[this->channel_ - 1]);
      }
    }
    this->parent_->data_ready_ = false;
  }
}

} // namespace ads131m04_sensor
} // namespace esphome
