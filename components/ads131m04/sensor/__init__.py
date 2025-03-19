import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    CONF_MULTIPLEXER,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_VOLT,
    CONF_TYPE,
)
from .. import ads131m04_ns, ADS131M04, CONF_ADS131M04_ID

AUTO_LOAD = ["voltage_sampler"]
DEPENDENCIES = ["ads131m04"]

ADS131M04Multiplexer = ads131m04_ns.enum("ADS131M04Multiplexer")
MUX = {
    "A0_A1": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P0_N1,
    "A0_A3": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P0_N3,
    "A1_A3": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P1_N3,
    "A2_A3": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P2_N3,
    "A0_GND": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P0_NG,
    "A1_GND": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P1_NG,
    "A2_GND": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P2_NG,
    "A3_GND": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_P3_NG,
}

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain")
GAIN = {
    "6.144": ADS131M04Gain.ADS131M04_GAIN_6P144,
    "4.096": ADS131M04Gain.ADS131M04_GAIN_4P096,
    "2.048": ADS131M04Gain.ADS131M04_GAIN_2P048,
    "1.024": ADS131M04Gain.ADS131M04_GAIN_1P024,
    "0.512": ADS131M04Gain.ADS131M04_GAIN_0P512,
    "0.256": ADS131M04Gain.ADS131M04_GAIN_0P256,
}


ADS131M04Sensor = ads131m04_ns.class_(
    "ADS131M04Sensor",
    cg.PollingComponent,
    sensor.Sensor,
    voltage_sampler.VoltageSampler,
    cg.Parented.template(ADS131M04),
)

TYPE_ADC = "adc"
TYPE_TEMPERATURE = "temperature"

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
        TYPE_TEMPERATURE: sensor.sensor_schema(
            ADS131M04Sensor,
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        )
        .extend(
            {
                cv.GenerateID(CONF_ADS131M04_ID): cv.use_id(ADS131M04),
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

    if config[CONF_TYPE] == TYPE_ADC:
        cg.add(var.set_multiplexer(config[CONF_MULTIPLEXER]))
        cg.add(var.set_gain(config[CONF_GAIN]))
    if config[CONF_TYPE] == TYPE_TEMPERATURE:
        cg.add(var.set_temperature_mode(True))
