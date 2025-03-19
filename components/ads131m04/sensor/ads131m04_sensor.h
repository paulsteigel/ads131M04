#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

#include "esphome/components/sensor/sensor.h"

#include "../ads131m04.h"

namespace esphome {
namespace ads131m04 {

class ADS131M04Sensor : public PollingComponent,
                         public sensor::Sensor,
                         public Parented<ADS131M04> {
public:
  /**
   * @brief Constructor for ADS131M04 sensor.
   *
   * @param parent Pointer to the ADS131M04 parent component.
   * @param sensor_num Sensor number (1-4).
   * @param multiplexer Multiplexer setting (string).
   * @param gain Gain setting (string).
   */
/*  
ADS131M04Sensor(ADS131M04 *parent, uint8_t sensor_num, std::string multiplexer, std::string gain)
      : PollingComponent(1000), // Default polling interval (1 second)
        sensor::Sensor(),
        Parented(parent),
        sensor_num_(sensor_num),
        multiplexer_(multiplexer),
        gain_(gain) {}
*/
  //void set_multiplexer(ADS1118Multiplexer multiplexer) { this->multiplexer_ = multiplexer; }  
  void update() override;
  float sample(); // Removed 'override'
  void dump_config() override;

protected:
  /** Sensor number (1-4). */
  uint8_t sensor_num_;
  /** Multiplexer setting (string). */
  std::string multiplexer_;
  /** Gain setting (string). */
  std::string gain_;
};

}  // namespace ads131m04
}  // namespace esphome
