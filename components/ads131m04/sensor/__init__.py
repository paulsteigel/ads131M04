import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
)
from .. import ads131m04_ns, ADS131M04, CONF_ADS131M04_ID

DEPENDENCIES = ['ads131m04']
AUTO_LOAD = ['voltage_sampler']

CONF_CHANNEL = "channel"

ADS131M04Sensor = ads131m04_ns.class_(
    "ADS131M04Sensor",
    sensor.Sensor,
    cg.PollingComponent,
    voltage_sampler.VoltageSampler,
    cg.Parented.template(ADS131M04),
)

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain")
GAIN_OPTIONS = {
    "1": ADS131M04Gain.ADS131M04_GAIN_1,
    "2": ADS131M04Gain.ADS131M04_GAIN_2,
    "4": ADS131M04Gain.ADS131M04_GAIN_4,
    "8": ADS131M04Gain.ADS131M04_GAIN_8,
    "16": ADS131M04Gain.ADS131M04_GAIN_16,
    "32": ADS131M04Gain.ADS131M04_GAIN_32,
    "64": ADS131M04Gain.ADS131M04_GAIN_64,
    "128": ADS131M04Gain.ADS131M04_GAIN_128,
}

CONFIG_SCHEMA = sensor.sensor_schema(
    ADS131M04Sensor,
    unit_of_measurement=UNIT_VOLT,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_VOLTAGE,
    state_class=STATE_CLASS_MEASUREMENT,
).extend({
    cv.GenerateID(CONF_ADS131M04_ID): cv.use_id(ADS131M04),
    cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=3),
    cv.Optional(CONF_GAIN, default="1"): cv.enum(GAIN_OPTIONS, upper=True),
})

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS131M04_ID])

    cg.add(var.set_channel(config[CONF_CHANNEL]))
    cg.add(var.set_gain(config[CONF_GAIN]))
