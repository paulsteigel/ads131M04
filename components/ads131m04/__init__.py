import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome.const import (
    CONF_ID,
    CONF_VOLTAGE,
    CONF_CURRENT,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    UNIT_AMPERE,
)
from esphome import pins


CODEOWNERS = ["@paulsteigel"]
DEPENDENCIES = ["spi"]
MULTI_CONF = True

CONF_ADS131M04_ID = "ads131m04_id"
CONF_DRDY_PIN = "data_ready_pin"
CONF_RESET_PIN = "reset_pin"
CONF_SAMPLING_RATE = "sampling_rate"
DEFAULT_SAMPLING_RATE = 16000

ads131m04_ns = cg.esphome_ns.namespace("ads131m04")
ADS131M04 = ads131m04_ns.class_("ADS131M04", cg.Component, spi.SPIDevice)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ADS131M04),
		cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
		cv.Optional(CONF_DRDY_PIN): pins.gpio_input_pin_schema,
    }
)
.extend(cv.COMPONENT_SCHEMA)
.extend(spi.spi_device_schema(cs_pin_required=True))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)
	
	if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

	if CONF_DRDY_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_DRDY_PIN])
        cg.add(var.set_data_ready_pin(reset))
	
