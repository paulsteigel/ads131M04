import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi, pins, sensor, gpio # Add gpio here
from esphome.const import CONF_ID

CONF_RESET_PIN = "reset_pin"
CONF_DRDY_PIN = "drdy_pin"
CONF_SAMPLING_RATE = "sampling_rate"

DEPENDENCIES = ["spi", "sensor", "gpio"] # Add gpio here
AUTO_LOAD = ["sensor"]

ads131m04_ns = cg.esphome_ns.namespace("ads131m04")
ADS131M04Component = ads131m04_ns.class_("ADS131M04Component", cg.Component, spi.SPIDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ADS131M04Component),
        cv.Required(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_DRDY_PIN): pins.gpio_input_pin_schema,
        cv.Optional(CONF_SAMPLING_RATE, default=16000): cv.positive_int,
    }
).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema(cs_pin_required=True))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    reset_pin = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset_pin))

    drdy_pin = await cg.gpio_pin_expression(config[CONF_DRDY_PIN])
    cg.add(var.set_drdy_pin(drdy_pin))

    cg.add(var.set_sampling_rate(config[CONF_SAMPLING_RATE]))
