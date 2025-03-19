import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_GAIN,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_TYPE,
)
from .. import ads131m04_ns, ADS131M04, CONF_ADS131M04_ID

DEPENDENCIES = ["ads131m04"]

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain")
GAIN = {
    "1": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_1,
    "2": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_2,
    "4": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_4,
    "8": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_8,
    "16": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_16,
    "32": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_32,
    "64": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_64,
    "128": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_128,
}

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
                cv.Optional(CONF_GAIN): cv.enum(GAIN, string=True),
            }
        )
        .extend(cv.polling_component_schema("60s")),
    },
    default_type=TYPE_ADC,
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ADS131M04_ID])
    if CONF_GAIN in config:
        cg.add(parent.set_gain(config[CONF_GAIN]))
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS131M04_ID])
