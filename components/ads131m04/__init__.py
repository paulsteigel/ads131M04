import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi, sensor
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

CODEOWNERS = ["@solomondg1"]
DEPENDENCIES = ["spi"]
MULTI_CONF = True

CONF_ADS131M04_ID = "ads131m04_id"
CONF_RESET_PIN = "reset_pin"
CONF_CHANNEL = "channel"

ads131m04_ns = cg.esphome_ns.namespace("ads131m04")
ADS131M04 = ads131m04_ns.class_("ADS131M04", cg.Component, spi.SPIDevice)

CHANNEL_CONFIG = cv.Schema({
    cv.Required(CONF_CHANNEL): cv.int_range(min=1, max=4),
})

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ADS131M04),
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_VOLTAGE): cv.ensure_list(sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ).extend(CHANNEL_CONFIG)),
            cv.Optional(CONF_CURRENT): cv.ensure_list(sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ).extend(CHANNEL_CONFIG)),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema(cs_pin_required=True))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    for conf in config.get(CONF_VOLTAGE, []):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_channel_voltage_sensor(conf[CONF_CHANNEL], sens))

    for conf in config.get(CONF_CURRENT, []):
        sens = await sensor.new_sensor(conf)
        cg.add(var.set_channel_current_sensor(conf[CONF_CHANNEL], sens))
