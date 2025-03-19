import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_GAIN,
    CONF_MULTIPLEXER,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_TYPE,
)
from .. import ads131m04_ns, ADS131M04, CONF_ADS131M04_ID

ADS131M04Multiplexer = ads131m04_ns.enum("ADS131M04Multiplexer")
MUX = {
  ADS131M04_MULTIPLEXER_AIN0P_AIN0N,
  ADS131M04_MULTIPLEXER_AIN1P_AIN1N,
  ADS131M04_MULTIPLEXER_AIN2P_AIN2N,
  ADS131M04_MULTIPLEXER_AIN3P_AIN3N,
  ADS131M04_MULTIPLEXER_AIN0P_AIN1N,
  ADS131M04_MULTIPLEXER_AIN1P_AIN2N,
  ADS131M04_MULTIPLEXER_AIN2P_AIN3N,
  ADS131M04_MULTIPLEXER_AIN3P_AIN0N,
  ADS131M04_MULTIPLEXER_AIN0,
  ADS131M04_MULTIPLEXER_AIN1,
  ADS131M04_MULTIPLEXER_AIN2,
  ADS131M04_MULTIPLEXER_AIN3,
  ADS131M04_MULTIPLEXER_SHORTED,
  ADS131M04_MULTIPLEXER_DC_TEST_POS,
  ADS131M04_MULTIPLEXER_DC_TEST_NEG,
}

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain")
GAIN = {
  ADS131M04_GAIN_1,
  ADS131M04_GAIN_2,
  ADS131M04_GAIN_4,
  ADS131M04_GAIN_8,
  ADS131M04_GAIN_16,
  ADS131M04_GAIN_32,
  ADS131M04_GAIN_64,
  ADS131M04_GAIN_128,
}

DEPENDENCIES = ["ads131m04"]
ADS131M04Sensor = ads131m04_ns.class_(
    "ADS131M04Sensor",
    cg.PollingComponent,
    sensor.Sensor,
    cg.Parented.template(ADS131M04),
)

TYPE_ADC = "adc"

CONFIG_SCHEMA = cv.typed_schema(
    {
        TYPE_ADC: sensor.sensor_schema(
            ADS131M04Sensor,
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        )
        .extend(
            {
                cv.GenerateID(CONF_ADS131M04_ID): cv.use_id(ADS131M04),
                cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                cv.Required(CONF_GAIN): cv.enum(GAIN, string=True),
            }
        )
        .extend(cv.polling_component_schema("60s")),
    },
    default_type=TYPE_ADC,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS131M04_ID])
