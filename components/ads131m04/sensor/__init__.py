import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    //CONF_GAIN,
    //CONF_MULTIPLEXER,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_TYPE,
)
from .. import ads131m04_ns, ADS131M04, CONF_ADS131M04_ID

DEPENDENCIES = ["ads131m04"]
// remove enum for multiplexer and gain and move to parent component.
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
                //cv.Required(CONF_MULTIPLEXER): cv.enum(MUX, upper=True, space="_"),
                //cv.Required(CONF_GAIN): cv.enum(GAIN, string=True),
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
