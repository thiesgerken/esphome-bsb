import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from . import BsbComponent, bsb_ns, CONF_BSB_ID, CONF_PARAMETER_NUMBER

from esphome.const import (
    CONF_ID,
    CONF_OPTIONS,
    CONF_UPDATE_INTERVAL,
)

CONF_FIELD_ID = "field_id"
CONF_ENABLE_BYTE = "enable_byte"

BsbSelect = bsb_ns.class_("BsbSelect", select.Select)

CONFIG_SCHEMA = cv.All(
    select.select_schema(
        BsbSelect,
    ).extend(
        {
            cv.GenerateID(CONF_BSB_ID): cv.use_id(BsbComponent),
            cv.Required(CONF_FIELD_ID): cv.positive_int,
            cv.Optional(CONF_ENABLE_BYTE, default="1"): cv.hex_int_range(0x00, 0xFF),
            cv.Optional(CONF_PARAMETER_NUMBER, default="0"): cv.positive_int,
            cv.Optional(CONF_UPDATE_INTERVAL, default="15min"): cv.update_interval,
            cv.Required(CONF_OPTIONS): cv.Schema({cv.int_: cv.string}),
        }
    ),
    cv.has_exactly_one_key(CONF_FIELD_ID),
)


async def to_code(config):
    component = await cg.get_variable(config[CONF_BSB_ID])

    options_map = config[CONF_OPTIONS]
    options_list = [options_map[k] for k in sorted(options_map.keys())]

    var = await select.new_select(config, options=options_list)

    if CONF_FIELD_ID in config:
        cg.add(var.set_field_id(config[CONF_FIELD_ID]))

    if CONF_ENABLE_BYTE in config:
        cg.add(var.set_enable_byte(config[CONF_ENABLE_BYTE]))

    if CONF_UPDATE_INTERVAL in config:
        cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))

    for value, option in options_map.items():
        cg.add(var.add_option_mapping(value, option))

    cg.add(component.register_select(var))
    cg.add(var.set_retry_interval(component.get_retry_interval()))
    cg.add(var.set_retry_count(component.get_retry_count()))
