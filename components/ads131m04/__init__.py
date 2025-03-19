import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi, sensor
from esphome.const import (
    CONF_ID,
    CONF_CHANNEL,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    UNIT_AMPERE,
)
from esphome import pins

DEPENDENCIES = ['spi']
AUTO_LOAD = ['sensor']

# Configuration constants
CONF_RESET_PIN = "reset_pin"
CONF_DRDY_PIN = "drdy_pin"
CONF_SAMPLING_RATE = "sampling_rate"

DEFAULT_SAMPLING_RATE = 16000  # Hz

ads131m04_ns = cg.esphome_ns.namespace('ads131m04')
ADS131M04Component = ads131m04_ns.class_('ADS131M04Component', cg.Component, spi.SPIDevice)

CHANNEL_CONFIG = cv.Schema({
    cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=4),
})

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ADS131M04Component),
    cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_DRDY_PIN): pins.gpio_input_pin_schema,
    cv.Optional(CONF_SAMPLING_RATE, default=DEFAULT_SAMPLING_RATE): cv.int_range(min=1000, max=32000),
    cv.Optional("voltage"): cv.ensure_list(sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(CHANNEL_CONFIG)),
    cv.Optional("current"): cv.ensure_list(sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(CHANNEL_CONFIG)),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    if CONF_DRDY_PIN in config:
        drdy = await cg.gpio_pin_expression(config[CONF_DRDY_PIN])
        cg.add(var.set_drdy_pin(drdy))

    if CONF_SAMPLING_RATE in config:
        cg.add(var.set_sampling_rate(config[CONF_SAMPLING_RATE]))

    for conf in config.get("voltage", []):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_channel_voltage_sensor(conf[CONF_CHANNEL], sens))

    for conf in config.get("current", []):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_channel_current_sensor(conf[CONF_CHANNEL], sens))
