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
CONF_CHANNEL = "reading_channel"
CONF_VOLTAGE_OFFSET = "voltage_offset"
CONF_VOLTAGE_SCALE = "voltage_scale"
CONF_CURRENT_OFFSET = "current_offset"
CONF_CURRENT_SCALE = "current_scale"
CONF_WINDOW_SIZE = "window_size"

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain")
GAIN = {
    "1": ADS131M04Gain.ADS131M04_GAIN_1,
    "2": ADS131M04Gain.ADS131M04_GAIN_2,
    "4": ADS131M04Gain.ADS131M04_GAIN_4,
    "8": ADS131M04Gain.ADS131M04_GAIN_8,
    "16": ADS131M04Gain.ADS131M04_GAIN_16,
    "32": ADS131M04Gain.ADS131M04_GAIN_32,
    "64": ADS131M04Gain.ADS131M04_GAIN_64,
    "128": ADS131M04Gain.ADS131M04_GAIN_128,
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
                cv.Optional(CONF_CHANNEL, default=1): cv.int_,
                cv.Optional(CONF_VOLTAGE_OFFSET, default=0.0): cv.float_,
                cv.Optional(CONF_VOLTAGE_SCALE, default=1.0): cv.float_,
                cv.Optional(CONF_CURRENT_OFFSET, default=0.0): cv.float_,
                cv.Optional(CONF_CURRENT_SCALE, default=1.0): cv.float_,
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
    if CONF_VOLTAGE_OFFSET in config:
        cg.add(var.set_voltage_offset(config[CONF_VOLTAGE_OFFSET]))
    if CONF_VOLTAGE_SCALE in config:
        cg.add(var.set_voltage_scale(config[CONF_VOLTAGE_SCALE]))
    if CONF_CURRENT_OFFSET in config:
        cg.add(var.set_current_offset(config[CONF_CURRENT_OFFSET]))
    if CONF_CURRENT_SCALE in config:
        cg.add(var.set_current_scale(config[CONF_CURRENT_SCALE]))

    cg.add_library("math.h", "")

    if CONF_GAIN in config:
        cg.add(var.set_gain(config[CONF_GAIN]))
