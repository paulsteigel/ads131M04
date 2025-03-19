#include "ads131m04.h"
#include "esphome/core/log.h"
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
  // Setup multiplexer
  //        0bx000xxxxxxxxxxxx
  this->config_ |= ADS131M04_MULTIPLEXER_P0_NG << 12;

  // Setup Gain
  //        0bxxxx000xxxxxxxxx
  this->config_ |= ADS131M04_GAIN_6P144 << 9;

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

float ADS131M04::request_measurement(ADS131M04Multiplexer multiplexer, ADS131M04Gain gain, bool temperature_mode) {
  uint16_t temp_config = this->config_;
  // Multiplexer
  //        0bxBBBxxxxxxxxxxxx
  temp_config &= 0b1000111111111111;
  temp_config |= (multiplexer & 0b111) << 12;

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

  digitalWrite(this->cs_, LOW);
  delayMicroseconds(1);

  cmd = (CMD_WRITE_REG) | (address << 7) | 0;

  uint8_t tx_buffer[13];
  uint8_t rx_buffer[13];

  tx_buffer[0] = (cmd >> 8) & 0xFF;
  tx_buffer[1] = cmd & 0xFF;
  tx_buffer[2] = 0x00;
  tx_buffer[3] = (value >> 8) & 0xFF;
  tx_buffer[4] = value & 0xFF;

  for(int i = 5; i < 13; i++){
    tx_buffer[i] = 0x00;
  }
  this->transfer(tx_buffer, rx_buffer, 13);
  res = ((uint16_t)rx_buffer[7] << 8) | rx_buffer[8];

  delayMicroseconds(1);
  digitalWrite(this->cs_, HIGH);

  addressRcv = (res & REGMASK_CMD_READ_REG_ADDRESS) >> 7;
  bytesRcv = (res & REGMASK_CMD_READ_REG_BYTES);

  if (addressRcv == address)
  {
    return bytesRcv + 1;
  }
  return 0;
}
// ... (setup(), dump_config(), readRegister(), writeRegister()) ...

void ADS131M04::writeRegisterMasked(uint8_t address, uint16_t value, uint16_t mask) {
  uint16_t register_contents = readRegister(address);
  register_contents = register_contents & ~mask;
  register_contents = register_contents | value;
  writeRegister(address, register_contents);
}

bool ADS131M04::command(uint16_t cmd) {
  digitalWrite(this->cs_, LOW);
  delayMicroseconds(1);

  uint8_t tx_buffer[13];
  for (int i=0; i<13; i++){
    tx_buffer[i] = 0x00;
  }
  tx_buffer[0] = (cmd >> 8) & 0xFF;
  tx_buffer[1] = cmd & 0xFF;

  this->transfer(tx_buffer, nullptr, 13);

  delayMicroseconds(1);
  digitalWrite(this->cs_, HIGH);
  return true;
}

bool ADS131M04::isDataReady() {
  if (this->data_ready_pin_ == 0) {
    ESP_LOGW(TAG, "Data ready pin not configured.");
    return false;
  }
  auto drdy_pin = gpio_pin_expression(this->data_ready_pin_);
  if (drdy_pin == nullptr){
    ESP_LOGW(TAG, "Data ready pin is nullptr.");
    return false;
  }
  return drdy_pin->digital_read() == false; // DRDY is low when data is ready
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
  if (channel > 3) return false;
  uint8_t msb_reg, lsb_reg;
  switch (channel) {
    case 0: msb_reg = REG_CH0_OCAL_MSB; lsb_reg = REG_CH0_OCAL_LSB; break;
    case 1: msb_reg = REG_CH1_OCAL_MSB; lsb_reg = REG_CH1_OCAL_LSB; break;
    case 2: msb_reg = REG_CH2_OCAL_MSB; lsb_reg = REG_CH2_OCAL_LSB; break;
    case 3: msb_reg = REG_CH3_OCAL_MSB; lsb_reg = REG_CH3_OCAL_LSB; break;
    default: return false;
  }
  writeRegister(msb_reg, (offset >> 8) & 0xFFFF);
  writeRegisterMasked(lsb_reg, (offset &
}  // namespace ads131m04
}  // namespace esphome
