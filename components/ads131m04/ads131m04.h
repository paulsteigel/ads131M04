#pragma once

#include "esphome/components/spi/spi.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
//#include "esphome/components/gpio/gpio.h"
//#include "esphome/core/log.h"

namespace esphome {
namespace ads131m04 {

#define SPIfreq 8000000 // 8 Mhz SPI
#define DRDY_STATE_LOGIC_HIGH 0 // DEFAULS
#define DRDY_STATE_HI_Z 1

// ... (All the #define lines from the Arduino header) ...
struct adcOutput{ // ADC output converted from two's complement
  uint16_t status;
  int32_t ch0;
  int32_t ch1;
  int32_t ch2;
  int32_t ch3;
};

struct adcOutputraw{ // ADC output in two's complement
  uint16_t status;
  uint32_t ch0;
  uint32_t ch1;
  uint32_t ch2;
  uint32_t ch3;
};

enum ADS131M04Multiplexer {
  ADS131M04_MULTIPLEXER_P0_N1 = 0b000,
  ADS131M04_MULTIPLEXER_P0_N3 = 0b001,
  ADS131M04_MULTIPLEXER_P1_N3 = 0b010,
  ADS131M04_MULTIPLEXER_P2_N3 = 0b011,
  ADS131M04_MULTIPLEXER_P0_NG = 0b100,
  ADS131M04_MULTIPLEXER_P1_NG = 0b101,
  ADS131M04_MULTIPLEXER_P2_NG = 0b110,
  ADS131M04_MULTIPLEXER_P3_NG = 0b111,
};

enum ADS131M04Gain {
  ADS131M04_GAIN_6P144 = 0b000,
  ADS131M04_GAIN_4P096 = 0b001,
  ADS131M04_GAIN_2P048 = 0b010,
  ADS131M04_GAIN_1P024 = 0b011,
  ADS131M04_GAIN_0P512 = 0b100,
  ADS131M04_GAIN_0P256 = 0b101,
};

#define SPIfreq 8000000 // 8 Mhz SPI 

#define DRDY_STATE_LOGIC_HIGH 0 // DEFAULS
#define DRDY_STATE_HI_Z 1

#define POWER_MODE_VERY_LOW_POWER 0
#define POWER_MODE_LOW_POWER 1
#define POWER_MODE_HIGH_RESOLUTION 2 // DEFAULT

#define CHANNEL_PGA_1 0
#define CHANNEL_PGA_2 1
#define CHANNEL_PGA_4 2
#define CHANNEL_PGA_8 3
#define CHANNEL_PGA_16 4
#define CHANNEL_PGA_32 5
#define CHANNEL_PGA_64 6
#define CHANNEL_PGA_128 7

#define INPUT_CHANNEL_MUX_AIN0P_AIN0N 0 // Default
#define INPUT_CHANNEL_MUX_INPUT_SHORTED 1
#define INPUT_CHANNEL_MUX_POSITIVE_DC_TEST_SIGNAL 2
#define INPUT_CHANNEL_MUX_NEGATIVE_DC_TEST_SIGNAL 3

#define OSR_128 0    // 32 kSPS
#define OSR_256 1    // 16 kSPS
#define OSR_512 2    // 8 kSPS
#define OSR_1024 3   // 4 kSPS defaulT
#define OSR_2048 4   // 2 kSPS
#define OSR_4096 5   // 1k kSPS
#define OSR_8192 6   // 500 SPS
#define OSR_16384 7  // 250 SPS


// Commands
#define CMD_NULL 0x0000 // This command gives the STATUS REGISTER
#define CMD_RESET 0x0011
#define CMD_STANDBY 0x0022
#define CMD_WAKEUP 0x0033
#define CMD_LOCK 0x0555
#define CMD_UNLOCK 0x0655
#define CMD_READ_REG 0xA000 // 101a aaaa annn nnnn   a=adress  n=numero de registros-1
#define CMD_WRITE_REG 0x6000

// Responses
#define RSP_RESET_OK 0xFF24
#define RSP_RESET_NOK 0x0011

// Registers Read Only
#define REG_ID 0x00
#define REG_STATUS 0x01

// Registers Global Settings across channels
#define REG_MODE 0x02
#define REG_CLOCK 0x03
#define REG_GAIN 0x04
#define REG_CFG 0x06
#define THRSHLD_MSB 0x07
#define THRSHLD_LSB 0x08

// Registers Channel 0 Specific
#define REG_CH0_CFG 0x09
#define REG_CH0_OCAL_MSB 0x0A
#define REG_CH0_OCAL_LSB 0x0B
#define REG_CH0_GCAL_MSB 0x0C
#define REG_CH0_GCAL_LSB 0x0D

// Registers Channel 1 Specific
#define REG_CH1_CFG 0x0E
#define REG_CH1_OCAL_MSB 0x0F
#define REG_CH1_OCAL_LSB 0x10
#define REG_CH1_GCAL_MSB 0x11
#define REG_CH1_GCAL_LSB 0x12

// Registers Channel 2 Specific
#define REG_CH2_CFG 0x13
#define REG_CH2_OCAL_MSB 0x14
#define REG_CH2_OCAL_LSB 0x15
#define REG_CH2_GCAL_MSB 0x16
#define REG_CH2_GCAL_LSB 0x17

// Registers Channel 3 Specific
#define REG_CH3_CFG 0x18
#define REG_CH3_OCAL_MSB 0x19
#define REG_CH3_OCAL_LSB 0x1A
#define REG_CH3_GCAL_MSB 0x1B
#define REG_CH3_GCAL_LSB 0x1C

// Registers MAP CRC
#define REG_MAP_CRC 0x3E

// ------------------------------------------------------------------------------------

// Mask READ_REG
#define REGMASK_CMD_READ_REG_ADDRESS 0x1F80
#define REGMASK_CMD_READ_REG_BYTES 0x007F

// Mask Register STATUS
#define REGMASK_STATUS_LOCK 0x8000
#define REGMASK_STATUS_RESYNC 0x4000
#define REGMASK_STATUS_REGMAP 0x2000
#define REGMASK_STATUS_CRC_ERR 0x1000
#define REGMASK_STATUS_CRC_TYPE 0x0800
#define REGMASK_STATUS_RESET 0x0400
#define REGMASK_STATUS_WLENGTH 0x0300
#define REGMASK_STATUS_DRDY3 0x0008
#define REGMASK_STATUS_DRDY2 0x0004
#define REGMASK_STATUS_DRDY1 0x0002
#define REGMASK_STATUS_DRDY0 0x0001

// Mask Register MODE
#define REGMASK_MODE_REG_CRC_EN 0x2000
#define REGMASK_MODE_RX_CRC_EN 0x1000
#define REGMASK_MODE_CRC_TYPE 0x0800
#define REGMASK_MODE_RESET 0x0400
#define REGMASK_MODE_WLENGTH 0x0300
#define REGMASK_MODE_TIMEOUT 0x0010
#define REGMASK_MODE_DRDY_SEL 0x000C
#define REGMASK_MODE_DRDY_HiZ 0x0002
#define REGMASK_MODE_DRDY_FMT 0x0001

// Mask Register CLOCK
#define REGMASK_CLOCK_CH3_EN 0x0800
#define REGMASK_CLOCK_CH2_EN 0x0400
#define REGMASK_CLOCK_CH1_EN 0x0200
#define REGMASK_CLOCK_CH0_EN 0x0100
#define REGMASK_CLOCK_OSR 0x001C
#define REGMASK_CLOCK_PWR 0x0003

// Mask Register GAIN
#define REGMASK_GAIN_PGAGAIN3 0x7000
#define REGMASK_GAIN_PGAGAIN2 0x0700
#define REGMASK_GAIN_PGAGAIN1 0x0070
#define REGMASK_GAIN_PGAGAIN0 0x0007

// Mask Register CFG
#define REGMASK_CFG_GC_DLY 0x1E00
#define REGMASK_CFG_GC_EN 0x0100
#define REGMASK_CFG_CD_ALLCH 0x0080
#define REGMASK_CFG_CD_NUM 0x0070
#define REGMASK_CFG_CD_LEN 0x000E
#define REGMASK_CFG_CD_EN 0x0001

// Mask Register THRSHLD_LSB
#define REGMASK_THRSHLD_LSB_CD_TH_LSB 0xFF00
#define REGMASK_THRSHLD_LSB_DCBLOCK 0x000F

// Mask Register CHX_CFG
#define REGMASK_CHX_CFG_PHASE 0xFFC0
#define REGMASK_CHX_CFG_DCBLKX_DIS0 0x0004
#define REGMASK_CHX_CFG_MUX 0x0003

// Mask Register CHX_OCAL_LSB
#define REGMASK_CHX_OCAL0_LSB 0xFF00

// Mask Register CHX_GCAL_LSB
#define REGMASK_CHX_GCAL0_LSB 0xFF00

//   --------------------------------------------------------------------

// Conversion modes
#define CONVERSION_MODE_CONT 0
#define CONVERSION_MODE_SINGLE 1

// Data Format
#define DATA_FORMAT_TWO_COMPLEMENT 0
#define DATA_FORMAT_BINARY 1

// Measure Mode
#define MEASURE_UNIPOLAR 1
#define MEASURE_BIPOLAR 0

// Clock Type
#define CLOCK_EXTERNAL 1
#define CLOCK_INTERNAL 0

// PGA Gain
#define PGA_GAIN_1 0
#define PGA_GAIN_2 1
#define PGA_GAIN_4 2
#define PGA_GAIN_8 3
#define PGA_GAIN_16 4
#define PGA_GAIN_32 5
#define PGA_GAIN_64 6
#define PGA_GAIN_128 7

// Input Filter
#define FILTER_SINC 0
#define FILTER_FIR 2
#define FILTER_FIR_IIR 3

// Data Mode
#define DATA_MODE_24BITS 0
#define DATA_MODE_32BITS 1

// Data Rate
#define DATA_RATE_0 0
#define DATA_RATE_1 1
#define DATA_RATE_2 2
#define DATA_RATE_3 3
#define DATA_RATE_4 4
#define DATA_RATE_5 5
#define DATA_RATE_6 6
#define DATA_RATE_7 7
#define DATA_RATE_8 8
#define DATA_RATE_9 9
#define DATA_RATE_10 10
#define DATA_RATE_11 11
#define DATA_RATE_12 12
#define DATA_RATE_13 13
#define DATA_RATE_14 14
#define DATA_RATE_15 15

// Sync Mpdes
#define SYNC_CONTINUOUS 1
#define SYNC_PULSE 0

// DIO Config Mode
#define DIO_OUTPUT 1
#define DIO_INPUT 0

#define SPI_MASTER_DUMMY 0xFF
#define SPI_MASTER_DUMMY16 0xFFFF
#define SPI_MASTER_DUMMY32 0xFFFFFFFF
// ... (End of the #define lines from the Arduino header) ...


class ADS131M04 : public Component,
                 public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                      spi::CLOCK_PHASE_TRAILING, spi::DATA_RATE_1MHZ> {
public:
  ADS131M04() = default;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  
  /// Helper method to request a measurement from a sensor.
  float request_measurement(ADS131M04Multiplexer multiplexer, ADS131M04Gain gain, bool temperature_mode);
  
  //void set_num_ac_channels(int num_ac_channels) { num_ac_channels_ = num_ac_channels; }
  //void set_data_ready_pin(uint8_t data_ready_pin) { data_ready_pin_ = data_ready_pin; }
  //void set_reset_pin(uint8_t reset_pin) { reset_pin_ = reset_pin; }      
  void set_data_ready_pin(GPIOPin *data_ready_pin) { this->data_ready_pin_ = data_ready_pin; }  
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }  

protected:
  uint16_t config_{0};
  GPIOPin *data_ready_pin_{nullptr};  
  GPIOPin *reset_pin_{nullptr};  
  //uint8_t data_ready_pin_ = 0;
  //uint8_t reset_pin_ = 0;

private:
    uint16_t readRegister(uint8_t address);
    uint8_t writeRegister(uint8_t address, uint16_t value);
    void writeRegisterMasked(uint8_t address, uint16_t value, uint16_t mask);
};
}  // namespace ads131m04
}  // namespace esphome
