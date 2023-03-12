import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_HEIGHT, CONF_TIMEOUT, ICON_GAUGE, STATE_CLASS_MEASUREMENT

DEPENDENCIES = ['uart']
AUTO_LOAD = ['sensor']

jarvis_ns = cg.esphome_ns.namespace('jarvis_jcb36n2')

Desky = jarvis_ns.class_('JarvisJcb36n2', cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(Desky),
    cv.Optional(CONF_HEIGHT): sensor.sensor_schema(
        icon=ICON_GAUGE,
        accuracy_decimals=1,
        unit_of_measurement="cm",
        state_class=STATE_CLASS_MEASUREMENT,
    ),
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    if CONF_HEIGHT in config:
        sens = await sensor.new_sensor(config[CONF_HEIGHT])
        cg.add(var.set_height_sensor(sens))
