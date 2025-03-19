#include "ads131m04.h"
#include "esphome/core/log.h"
#include "sensor/ads131m04_sensor.h"
//#include "sensor/ads131m04_sensor.h" // Include the sensor header file
//#include "esphome/core/helpers.h" // For encoding and decoding functions
//#include "esphome/components/gpio/gpio.h" // For GPIO pin handling
//#include "esphome/core/application.h" // For delay functions

namespace esphome {
namespace ads131m04 {

static const char *const TAG = "ads131m04";
static const uint8_t ADS131M04_DATA_RATE_860_SPS = 0b111;

void ADS131M04::setup() {

/////////////////////
  ESP_LOGCONFIG(TAG, "Setting up ADS131M04");
  /*
  this->spi_setup();
  this->spi_bus_ = new spi::SPIBus(this->clk_pin_, this->sdi_pin_, this->sdo_pin_);
  this->clk_pin_->setup();
  this->clk_pin_->digital_write(true);
  this->sdo_pin_->setup();
  this->sdi_pin_->setup();
  this->cs_->setup();
  this->cs_->digital_write(true);
  */
  this->spi_setup();
  
  // Reset Pin Setup   
  if (this->reset_pin_ != nullptr) {
      this->reset_pin_->setup();
      this->reset_pin_->digital_write(true); // Initial high state
      delay(10);
      this->reset_pin_->digital_write(false);
      delay(10);
      this->reset_pin_->digital_write(true);
      delay(10);
  }
  // Set channel for sensor
  for (uint8_t i = 0; i < this->sensors_.size(); i++) {
    this->sensors_[i]->set_channel(i);
  }
  
  // DRDY Pin Setup
  if (this->data_ready_pin_ != nullptr) {
      this->data_ready_pin_->setup();
  }


  // Example: Set OSR
  setOsr(OSR_1024);

  // Example: Enable Channel 0
  setChannelEnable(0,1);

  // Example: Set channel 0 PGA
  setChannelPGA(0, CHANNEL_PGA_1);

  // Example: Set channel 0 input mux
  setInputChannelSelection(0, INPUT_CHANNEL_MUX_AIN0P_AIN0N);

  // Add other initial configuration steps here
  this->config_ = 0;
  
  // Set singleshot mode
  //        0bxxxxxxx1xxxxxxxx
  this->config_ |= 0b0000000100000000;

  // Set data rate - 860 samples per second (we're in singleshot mode)
  //        0bxxxxxxxx100xxxxx
  this->config_ |= ADS131M04_DATA_RATE_860_SPS << 5;

  // Set temperature sensor mode - ADC
  //        0bxxxxxxxxxxx0xxxx
  this->config_ |= 0b0000000000000000;

  // Set DOUT pull up - enable
  //        0bxxxxxxxxxxxx0xxx
  this->config_ |= 0b0000000000001000;

  // NOP - must be 01
  //        0bxxxxxxxxxxxxx01x
  this->config_ |= 0b0000000000000010;

  // Not used - can be 0 or 1, lets be positive
  //        0bxxxxxxxxxxxxxxx1
  this->config_ |= 0b0000000000000001;
}

void ADS131M04::dump_config() {
  ESP_LOGCONFIG(TAG, "ADS131M04:");
  LOG_PIN("  CS Pin:", this->cs_)  
  LOG_PIN("  DRDY Pin:", this->data_ready_pin_);
  LOG_PIN("  RESET Pin:", this->reset_pin_);
}

/*
float ADS131M04::request_measurement(ADS131M04Gain gain, bool temperature_mode) {
  uint16_t temp_config = this->config_;
  
  // Gain
  temp_config &= 0b1111000111111111;
  temp_config |= (gain & 0b111) << 9;

  // ADC Mode Only
  temp_config &= 0b1111111111101111;

  // Start Conversion
  temp_config |= 0b1000000000000000;

  // Send Configuration (using readADC() style)
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  this->write_byte((temp_config >> 8) & 0xFF);
  this->write_byte(temp_config & 0xFF);
  this->write_byte(0x00); // Extra byte as in readADC()
  delayMicroseconds(1);
  this->cs_->digital_write(true);

  // Delay for Conversion
  delay(2);

  // Read Result (using readADC() style)
  this->cs_->digital_write(false);
  delayMicroseconds(1);
  uint8_t adc_first_byte = this->read_byte();
  uint8_t adc_second_byte = this->read_byte();
  this->read_byte(); // Discard extra byte
  delayMicroseconds(1);
  this->cs_->digital_write(true);

  uint16_t raw_conversion = encode_uint16(adc_first_byte, adc_second_byte);
  auto signed_conversion = static_cast<int16_t>(raw_conversion);

  // Convert to millivolts
  float millivolts;
  float divider = 32768.0f;
  switch (gain) {
    case ADS131M04_GAIN_6P144:
      millivolts = (signed_conversion * 6144) / divider;
      break;
    case ADS131M04_GAIN_4P096:
      millivolts = (signed_conversion * 4096) / divider;
      break;
    case ADS131M04_GAIN_2P048:
      millivolts = (signed_conversion * 2048) / divider;
      break;
    case ADS131M04_GAIN_1P024:
      millivolts = (signed_conversion * 1024) / divider;
      break;
    case ADS131M04_GAIN_0P512:
      millivolts = (signed_conversion * 512) / divider;
      break;
    case ADS131M04_GAIN_0P256:
      millivolts = (signed_conversion * 256) / divider;
      break;
    default:
      millivolts = NAN;
  }

  return millivolts / 1e3f;
}

float ADS131M04::request_measurementX(ADS131M04Gain gain, bool temperature_mode) {
  uint16_t temp_config = this->config_;
  
  // Gain
  //        0bxxxxBBBxxxxxxxxx
  temp_config &= 0b1111000111111111;
  temp_config |= (gain & 0b111) << 9;

  if (temperature_mode) {
    // Set temperature sensor mode
    //        0bxxxxxxxxxxx1xxxx
    temp_config |= 0b0000000000010000;
  } else {
    // Set ADC mode
    //        0bxxxxxxxxxxx0xxxx
    temp_config &= 0b1111111111101111;
  }

  // Start conversion
  temp_config |= 0b1000000000000000;

  this->enable();
  this->write_byte16(temp_config);
  this->disable();

  // about 1.2 ms with 860 samples per second
  delay(2);

  this->enable();
  uint8_t adc_first_byte = this->read_byte();
  uint8_t adc_second_byte = this->read_byte();
  this->disable();
  uint16_t raw_conversion = encode_uint16(adc_first_byte, adc_second_byte);

  auto signed_conversion = static_cast<int16_t>(raw_conversion);

  if (temperature_mode) {
    return (signed_conversion >> 2) * 0.03125f;
  } else {
    float millivolts;
    float divider = 32768.0f;
    switch (gain) {
      case ADS131M04_GAIN_6P144:
        millivolts = (signed_conversion * 6144) / divider;
        break;
      case ADS131M04_GAIN_4P096:
        millivolts = (signed_conversion * 4096) / divider;
        break;
      case ADS131M04_GAIN_2P048:
        millivolts = (signed_conversion * 2048) / divider;
        break;
      case ADS131M04_GAIN_1P024:
        millivolts = (signed_conversion * 1024) / divider;
        break;
      case ADS131M04_GAIN_0P512:
        millivolts = (signed_conversion * 512) / divider;
        break;
      case ADS131M04_GAIN_0P256:
        millivolts = (signed_conversion * 256) / divider;
        break;
      default:
        millivolts = NAN;
    }

    return millivolts / 1e3f;
  }
}
*/

uint16_t ADS131M04::readRegister(uint8_t address) {
  uint16_t cmd;
  uint16_t data;

  cmd = CMD_READ_REG | (address << 7 | 0);

  this->cs_->digital_write(false); // Replace digitalWrite(ADS131M04_CS_PIN, LOW)
  delayMicroseconds(1);

  // Send the command (16 bits)
  uint8_t cmd_high = (cmd >> 8) & 0xFF;
  uint8_t cmd_low = cmd & 0xFF;
  this->write_byte(cmd_high);
  this->write_byte(cmd_low);
  this->write_byte(0x00); // Send the extra byte

  // Send 16 bits of 0x0000 followed by 1 byte of 0x00, 6 times
  for (int i = 0; i < 6; i++) {
    this->write_byte(0x00);
    this->write_byte(0x00);
    this->write_byte(0x00);
  }

  // Read the 16 bits of data
  uint8_t data_high = this->read_byte();
  uint8_t data_low = this->read_byte();
  data = (data_high << 8) | data_low;
  this->read_byte(); // Read the extra byte

  // Send 16 bits of 0x0000 followed by 1 byte of 0x00, 6 times
  for (int i = 0; i < 6; i++) {
    this->write_byte(0x00);
    this->write_byte(0x00);
    this->write_byte(0x00);
  }

  this->cs_->digital_write(true); // Replace digitalWrite(ADS131M04_CS_PIN, HIGH)
  return data;
}

uint8_t ADS131M04::writeRegister(uint8_t address, uint16_t value) {
  uint16_t res;
  uint8_t addressRcv;
  uint8_t bytesRcv;
  uint16_t cmd = 0;

  this->cs_->digital_write(false); // Replace digitalWrite(ADS131M04_CS_PIN, LOW)
  delayMicroseconds(1);

  cmd = (CMD_WRITE_REG) | (address << 7) | 0;

  // Send the command (16 bits)
  uint8_t cmd_high = (cmd >> 8) & 0xFF;
  uint8_t cmd_low = cmd & 0xFF;
  this->write_byte(cmd_high);
  this->write_byte(cmd_low);
  this->write_byte(0x00); // Send the extra byte

  // Send the value (16 bits)
  uint8_t value_high = (value >> 8) & 0xFF;
  uint8_t value_low = value & 0xFF;
  this->write_byte(value_high);
  this->write_byte(value_low);
  this->write_byte(0x00); // Send the extra byte

  // Send 16 bits of 0x0000 followed by 1 byte of 0x00, 6 times
  for (int i = 0; i < 6; i++) {
    this->write_byte(0x00);
    this->write_byte(0x00);
    this->write_byte(0x00);
  }

  // Read the response (16 bits)
  uint8_t res_high = this->read_byte();
  uint8_t res_low = this->read_byte();
  res = (res_high << 8) | res_low;
  this->read_byte(); // Read the extra byte

  // Send 16 bits of 0x0000 followed by 1 byte of 0x00, 6 times
  for (int i = 0; i < 6; i++) {
    this->write_byte(0x00);
    this->write_byte(0x00);
    this->write_byte(0x00);
  }

  this->cs_->digital_write(true); // Replace digitalWrite(ADS131M04_CS_PIN, HIGH)

  addressRcv = (res & REGMASK_CMD_READ_REG_ADDRESS) >> 7;
  bytesRcv = (res & REGMASK_CMD_READ_REG_BYTES);

  if (addressRcv == address) {
    return bytesRcv + 1;
  }
  return 0;
}

void ADS131M04::writeRegisterMasked(uint8_t address, uint16_t value, uint16_t mask) {
  uint16_t register_contents = readRegister(address);
  register_contents = register_contents & ~mask;
  register_contents = register_contents | value;
  writeRegister(address, register_contents);
}

bool ADS131M04::command(uint16_t cmd) {
  // Issue command (6 word frame)
  this->cs_->digital_write(false); // Replace digitalWrite(ADS131M04_CS_PIN, LOW)
  delayMicroseconds(1);

  // Send the command (16 bits)
  uint8_t cmd_high = (cmd >> 8) & 0xFF;
  uint8_t cmd_low = cmd & 0xFF;
  this->write_byte(cmd_high);
  this->write_byte(cmd_low);
  this->write_byte(0x00); // Send the extra byte

  // Send 16 bits of 0x0000 followed by 1 byte of 0x00, 5 times
  for (int i = 0; i < 5; i++) {
    this->write_byte(0x00);
    this->write_byte(0x00);
    this->write_byte(0x00);
  }

  this->cs_->digital_write(true); // Replace digitalWrite(ADS131M04_CS_PIN, HIGH)
  return true;
}

bool ADS131M04::isDataReady() {
  if (this->data_ready_pin_->digital_read()) { // Replace digitalRead(ADS131M04_DRDY_PIN) == HIGH
    return false;
  }
  return true;
}

int8_t ADS131M04::isDataReadySoft(uint8_t channel) {
  uint16_t status = readRegister(REG_STATUS);
  switch (channel) {
    case 0: return (status & REGMASK_STATUS_DRDY0) != 0;
    case 1: return (status & REGMASK_STATUS_DRDY1) != 0;
    case 2: return (status & REGMASK_STATUS_DRDY2) != 0;
    case 3: return (status & REGMASK_STATUS_DRDY3) != 0;
    default: return -1;
  }
}

bool ADS131M04::isResetStatus() {
  return (readRegister(REG_STATUS) & REGMASK_STATUS_RESET) != 0;
}

bool ADS131M04::isLockSPI() {
  return (readRegister(REG_STATUS) & REGMASK_STATUS_LOCK) != 0;
}

bool ADS131M04::setDrdyFormat(uint8_t drdyFormat) {
  if (drdyFormat > 1) return false;
  writeRegisterMasked(REG_MODE, drdyFormat, REGMASK_MODE_DRDY_FMT);
  return true;
}

bool ADS131M04::setDrdyStateWhenUnavailable(uint8_t drdyState) {
  if (drdyState > 1) return false;
  writeRegisterMasked(REG_MODE, drdyState < 1, REGMASK_MODE_DRDY_HiZ);
  return true;
}

bool ADS131M04::setPowerMode(uint8_t powerMode) {
  if (powerMode > 3) return false;
  writeRegisterMasked(REG_CLOCK, powerMode, REGMASK_CLOCK_PWR);
  return true;
}

bool ADS131M04::setOsr(uint16_t osr) {
  if (osr > 7) return false;
  writeRegisterMasked(REG_CLOCK, osr << 2, REGMASK_CLOCK_OSR);
  return true;
}

bool ADS131M04::setChannelEnable(uint8_t channel, uint16_t enable) {
  if (channel > 3) return false;
  uint16_t mask;
  uint16_t shift;
  switch (channel) {
    case 0: mask = REGMASK_CLOCK_CH0_EN; shift = 8; break;
    case 1: mask = REGMASK_CLOCK_CH1_EN; shift = 9; break;
    case 2: mask = REGMASK_CLOCK_CH2_EN; shift = 10; break;
    case 3: mask = REGMASK_CLOCK_CH3_EN; shift = 11; break;
    default: return false;
  }
  writeRegisterMasked(REG_CLOCK, enable << shift, mask);
  return true;
}

bool ADS131M04::setChannelPGA(uint8_t channel, uint16_t pga) {
  if (channel > 3) return false;
  uint16_t mask;
  uint16_t shift;
  switch (channel) {
    case 0: mask = REGMASK_GAIN_PGAGAIN0; shift = 0; break;
    case 1: mask = REGMASK_GAIN_PGAGAIN1; shift = 4; break;
    case 2: mask = REGMASK_GAIN_PGAGAIN2; shift = 8; break;
    case 3: mask = REGMASK_GAIN_PGAGAIN3; shift = 12; break;
    default: return false;
  }
  writeRegisterMasked(REG_GAIN, pga << shift, mask);
  return true;
}

void ADS131M04::setGlobalChop(uint16_t global_chop) {
  writeRegisterMasked(REG_CFG, global_chop << 8, REGMASK_CFG_GC_EN);
}

void ADS131M04::setGlobalChopDelay(uint16_t delay) {
  writeRegisterMasked(REG_CFG, delay << 9, REGMASK_CFG_GC_DLY);
}

bool ADS131M04::setInputChannelSelection(uint8_t channel, uint8_t input) {
  if (channel > 3) return false;
  uint8_t reg_addr;
  switch (channel) {
    case 0: reg_addr = REG_CH0_CFG; break;
    case 1: reg_addr = REG_CH1_CFG; break;
    case 2: reg_addr = REG_CH2_CFG; break;
    case 3: reg_addr = REG_CH3_CFG; break;
    default: return false;
  }
  writeRegisterMasked(reg_addr, input, REGMASK_CHX_CFG_MUX);
  return true;
}

bool ADS131M04::setChannelOffsetCalibration(uint8_t channel, int32_t offset) {
  uint16_t MSB = offset >> 8;
  uint8_t LSB = offset;
  bool returnval = false;

  if (channel > 3) {
    returnval = false;
  } else if (channel == 0) {
    writeRegisterMasked(REG_CH0_OCAL_MSB, MSB, 0xFFFF);
    writeRegisterMasked(REG_CH0_OCAL_LSB, LSB << 8, REGMASK_CHX_OCAL0_LSB);
    returnval = true;
  } else if (channel == 1) {
    writeRegisterMasked(REG_CH1_OCAL_MSB, MSB, 0xFFFF);
    writeRegisterMasked(REG_CH1_OCAL_LSB, LSB << 8, REGMASK_CHX_OCAL0_LSB);
    returnval = true;
  } else if (channel == 2) {
    writeRegisterMasked(REG_CH2_OCAL_MSB, MSB, 0xFFFF);
    writeRegisterMasked(REG_CH2_OCAL_LSB, LSB << 8, REGMASK_CHX_OCAL0_LSB);
    returnval = true;
  } else if (channel == 3) {
    writeRegisterMasked(REG_CH3_OCAL_MSB, MSB, 0xFFFF);
    writeRegisterMasked(REG_CH3_OCAL_LSB, LSB << 8, REGMASK_CHX_OCAL0_LSB);
    returnval = true;
  }
  return returnval;
}
adcOutput ADS131M04::readADC(void) {
  uint8_t x = 0;
  uint8_t x2 = 0;
  uint8_t x3 = 0;
  int32_t aux;
  adcOutput res;

  this->cs_->digital_write(false); // Replace digitalWrite(ADS131M04_CS_PIN, LOW)
  delayMicroseconds(1);

  x = this->read_byte();
  x2 = this->read_byte();
  this->read_byte(); // Discard extra byte

  res.status = ((x << 8) | x2);

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  aux = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);
  aux = twoscom(aux);
  res.ch0 = aux;

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  aux = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);
  aux = twoscom(aux);
  res.ch1 = aux;

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  aux = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);
  aux = twoscom(aux);
  res.ch2 = aux;

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  aux = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);
  aux = twoscom(aux);
  res.ch3 = aux;

  // CRC
  this->read_byte();
  this->read_byte();
  this->read_byte();

  delayMicroseconds(1);
  this->cs_->digital_write(true); // Replace digitalWrite(ADS131M04_CS_PIN, HIGH)

  return res;
}

adcOutputraw ADS131M04::readADCraw(void) {
  uint8_t x = 0;
  uint8_t x2 = 0;
  uint8_t x3 = 0;
  adcOutputraw res;

  this->cs_->digital_write(false); // Replace digitalWrite(ADS131M04_CS_PIN, LOW)
  delayMicroseconds(1);

  x = this->read_byte();
  x2 = this->read_byte();
  this->read_byte(); // Discard extra byte

  res.status = ((x << 8) | x2);

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  res.ch0 = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  res.ch1 = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  res.ch2 = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);

  x = this->read_byte();
  x2 = this->read_byte();
  x3 = this->read_byte();

  res.ch3 = (((x << 16) | (x2 << 8) | x3) & 0x00FFFFFF);

  // CRC
  this->read_byte();
  this->read_byte();
  this->read_byte();

  delayMicroseconds(1);
  this->cs_->digital_write(true); // Replace digitalWrite(ADS131M04_CS_PIN, HIGH)

  return res;
}

}  // namespace ads131m04
}  // namespace esphome
