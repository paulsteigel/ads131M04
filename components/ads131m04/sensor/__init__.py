import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi, sensor, binary_sensor, gpio
from esphome.const import (
    CONF_ID,
    CONF_SPI_ID,
    CONF_CS_PIN,
    CONF_DATA_READY_PIN,
    CONF_RESET_PIN,
    CONF_GAIN,
    CONF_UPDATE_INTERVAL,
    UNIT_VOLT,
    UNIT_AMPERE,
    ICON_FLASH,
    STATE_CLASS_MEASUREMENT,
    CONF_VOLTAGE_OFFSET,
    CONF_VOLTAGE_SCALE,
    CONF_CURRENT_OFFSET,
    CONF_CURRENT_SCALE,
)

DEPENDENCIES = ["spi"]
AUTO_LOAD = ["binary_sensor"]

CONF_ADS131M04_ID = "ads131m04_id"

ads131m04_ns = cg.esphome_ns.namespace("ads131m04")
ADS131M04 = ads131m04_ns.class_("ADS131M04", cg.PollingComponent, spi.SPIDevice)
ADS131M04Gain = ads131m04_ns.enum_("ADS131M04Gain")

GAINS = {
    "1": ADS131M04Gain.ADS131M04_GAIN_1,
    "2": ADS131M04Gain.ADS131M04_GAIN_2,
    "4": ADS131M04Gain.ADS131M04_GAIN_4,
    "8": ADS131M04Gain.ADS131M04_GAIN_8,
    "16": ADS131M04Gain.ADS131M04_GAIN_16,
    "32": ADS131M04Gain.ADS131M04_GAIN_32,
    "64": ADS131M04Gain.ADS131M04_GAIN_64,
    "128": ADS131M04Gain.ADS131M04_GAIN_128,
}

ADS131M04Sensor = ads131m04_ns.class_("ADS131M04Sensor", sensor.Sensor, cg.Parented.template(ADS131M04))

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ADS131M04),
            cv.GenerateID(CONF_SPI_ID): cv.use_id(spi.SPIDevice),
            cv.Required(CONF_CS_PIN): gpio.output_pin_schema,
            cv.Optional(CONF_DATA_READY_PIN): gpio.input_pin_schema,
            cv.Optional(CONF_RESET_PIN): gpio.output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(spi.spi_device_schema())
)

ADS131M04_SENSOR_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_VOLT,
    icon=ICON_FLASH,
    accuracy_decimals=3,
    state_class=STATE_CLASS_MEASUREMENT,
).extend(
    {
        cv.GenerateID(CONF_ADS131M04_ID): cv.use_id(ADS131M04),
        cv.Optional(CONF_GAIN, default="1"): cv.enum(GAINS, upper=True),
        cv.Optional(CONF_VOLTAGE_OFFSET, default=0.0): cv.float_,
        cv.Optional(CONF_VOLTAGE_SCALE, default=1.0): cv.float_,
        cv.Optional(CONF_CURRENT_OFFSET, default=0.0): cv.float_,
        cv.Optional(CONF_CURRENT_SCALE, default=1.0): cv.float_,
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    if CONF_CS_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_CS_PIN])
        cg.add(var.set_cs_pin(pin))
    if CONF_DATA_READY_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_DATA_READY_PIN])
        cg.add(var.set_data_ready_pin(pin))
    if CONF_RESET_PIN in config:
        pin = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(pin))

async def to_code_sensor(config):
    var = cg.new_Pvariable(config[CONF_ID], template_args=(cg.RawExpression("float"),))
    await cg.register_component(var, config)
    parent = await cg.get_variable(config[CONF_ADS131M04_ID])
    cg.add(var.set_parent(parent))

    if CONF_GAIN in config:
        cg.add(var.set_gain(config[CONF_GAIN]))
    if CONF_VOLTAGE_OFFSET in config:
        cg.add(var.set_voltage_offset(config[CONF_VOLTAGE_OFFSET]))
    if CONF_VOLTAGE_SCALE in config:
        cg.add(var.set_voltage_scale(config[CONF_VOLTAGE_SCALE]))
    if CONF_CURRENT_OFFSET in config:
        cg.add(var.set_current_offset(config[CONF_CURRENT_OFFSET]))
    if CONF_CURRENT_SCALE in config:
        cg.add(var.set_current_scale(config[CONF_CURRENT_SCALE]))

    cg.add_library("math.h", "")
