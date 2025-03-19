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

# --- Multiplexer Configuration Removed ---
# Hardware configuration has fixed differential input pairs (AIN0P/N, AIN1P/N, etc.).
# Therefore, multiplexer switching is not required, and related code has been removed.
# ADS131M04Multiplexer and MUX definitions are no longer needed.
# --- End Multiplexer Removal ---

# ADS131M04Multiplexer = ads131m04_ns.enum("ADS131M04Multiplexer")
# MUX = {
#     "AIN0P_AIN0N": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_AIN0P_AIN0N,
#     "AIN1P_AIN1N": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_AIN1P_AIN1N,
#     "AIN2P_AIN2N": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_AIN2P_AIN2N,
#     "AIN3P_AIN3N": ADS131M04Multiplexer.ADS131M04_MULTIPLEXER_AIN3P_AIN3N,
#     # ... (other multiplexer options) ...
# }

ADS131M04Gain = ads131m04_ns.enum("ADS131M04Gain") # Corrected namespace
GAIN = {
    "1": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_1, # Corrected namespace
    "2": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_2, # Corrected namespace
    "4": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_4, # Corrected namespace
    "8": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_8, # Corrected namespace
    "16": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_16, # Corrected namespace
    "32": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_32, # Corrected namespace
    "64": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_64, # Corrected namespace
    "128": ads131m04_ns.ADS131M04Gain.ADS131M04_GAIN_128, # Corrected namespace
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
            }
        )
        .extend(cv.polling_component_schema("60s")),
    },
    default_type=TYPE_ADC,
)

ADS131M04_SCHEMA = cv.Schema({
    # --- Multiplexer Configuration Removed ---
    # Removed CONF_MULTIPLEXER from the parent schema as multiplexer is fixed in hardware.
    # cv.Optional(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
    # --- End Multiplexer Removal ---
    cv.Optional(CONF_GAIN): cv.enum(GAIN, string=True),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ADS131M04_ID])

    # --- Multiplexer Configuration Removed ---
    # Removed multiplexer setting logic from to_code as multiplexer is fixed in hardware.
    # if CONF_MULTIPLEXER in config:
    #     cg.add(parent.set_multiplexer(config[CONF_MULTIPLEXER]))
    # --- End Multiplexer Removal ---

    if CONF_GAIN in config:
        cg.add(parent.set_gain(config[CONF_GAIN]))
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS131M04_ID])
