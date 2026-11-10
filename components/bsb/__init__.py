import functools
import re
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, web_server_base
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID
)
from esphome import automation

from .bsb_lan_defs import CustomDefinitions, DefinitionsError
from .dump_table import build_dump_rows, render_dump_table

CODEOWNERS = ["@eringerli"]
MULTI_CONF = True

DEPENDENCIES = ["uart"]
CONF_BSB_ID = "bsb_id"
CONF_PARAMETER_NUMBER = "parameter_number"
CONF_SOURCE_ADDRESS = "source_address"
CONF_DESTINATION_ADDRESS = "destination_address"
CONF_QUERY_INTERVAL = "query_interval"
CONF_RETRY_INTERVAL = "retry_interval"
CONF_RETRY_COUNT = "retry_count"
CONF_BSB_TYPE= "type"
CONF_PARAMETER_DUMP = "parameter_dump"
CONF_DEFINITIONS = "definitions"
CONF_DEVICE_FAMILY = "device_family"
CONF_DEVICE_VARIANT = "device_variant"
CONF_REQUEST_HANDLER_ID = "request_handler_id"


def AUTO_LOAD(config):
    loads = ["sensor", "text_sensor", "select", "button"] #, "switch", "binary_sensor"
    # A falsy config is ESPHome probing for the maximal set.
    if not config or any(CONF_PARAMETER_DUMP in bus for bus in config):
        loads.append("web_server_base")
    return loads

CONF_BSB_TYPE_ENUM = {
    "UINT8":0,
    "INT8":1,
    "INT16":2,
    "INT32":3,
    "TEMPERATURE":4,
    "ROOMTEMPERATURE":5,
    "DATETIME":6
}

bsb_ns = cg.esphome_ns.namespace("bsb")
BsbComponent = bsb_ns.class_(
    "BsbComponent", cg.Component, uart.UARTDevice
)

BsbTimeoutTrigger = bsb_ns.class_(
    "BsbTimeoutTrigger", automation.Trigger
)
BsbWaitNextReadoutTrigger = bsb_ns.class_(
    "BsbWaitNextReadoutTrigger", automation.Trigger
)
BsbParameterDump = bsb_ns.class_("BsbParameterDump")
BsbDumpRequestHandler = bsb_ns.class_("BsbDumpRequestHandler", cg.Component)

def validate_baud_rate(value):
    if value > 0:
        baud_rates = [ 4800 ]
        if value not in baud_rates:
            raise cv.Invalid(f"Non standard baud rate {value}. Use one of {baud_rates}")

    return value


@functools.cache
def load_dump_rows(definitions_path, device_family, device_variant):
    with open(definitions_path, encoding="utf-8") as file:
        definitions = CustomDefinitions(file.read())
    return build_dump_rows(definitions, device_family, device_variant)


def validate_parameter_dump(config):
    try:
        load_dump_rows(config[CONF_DEFINITIONS], config[CONF_DEVICE_FAMILY], config[CONF_DEVICE_VARIANT])
    except DefinitionsError as error:
        raise cv.Invalid(f"{config[CONF_DEFINITIONS]}: {error}", path=[CONF_DEFINITIONS]) from error
    return config


PARAMETER_DUMP_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BsbParameterDump),
            cv.GenerateID(CONF_REQUEST_HANDLER_ID): cv.declare_id(BsbDumpRequestHandler),
            cv.GenerateID(web_server_base.CONF_WEB_SERVER_BASE_ID): cv.use_id(web_server_base.WebServerBase),
            cv.Required(CONF_DEFINITIONS): cv.file_,
            cv.Required(CONF_DEVICE_FAMILY): cv.uint8_t,
            cv.Required(CONF_DEVICE_VARIANT): cv.uint8_t,
        }
    ),
    cv.only_on_esp32,
    validate_parameter_dump,
)


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BsbComponent),
            cv.Optional(CONF_RETRY_COUNT, default="3"): cv.hex_int_range(0x00,0xff),
            cv.Optional(CONF_RETRY_INTERVAL, default="15s"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_QUERY_INTERVAL, default="0.25s"): cv.positive_time_period_milliseconds,
            cv.Optional(
                CONF_SOURCE_ADDRESS, default="66"
            ): cv.positive_int,
            cv.Optional(
                CONF_DESTINATION_ADDRESS, default="0"
            ): cv.positive_int,
            cv.Optional(CONF_PARAMETER_DUMP): PARAMETER_DUMP_SCHEMA,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_QUERY_INTERVAL in config:
        cg.add(var.set_query_interval(config[CONF_QUERY_INTERVAL]))

    if CONF_RETRY_INTERVAL in config:
        cg.add(var.set_retry_interval(config[CONF_RETRY_INTERVAL]))

    if CONF_RETRY_COUNT in config:
        cg.add(var.set_retry_count(config[CONF_RETRY_COUNT]))

    if CONF_SOURCE_ADDRESS in config:
        cg.add(var.set_source_address(config[CONF_SOURCE_ADDRESS]))

    if CONF_DESTINATION_ADDRESS in config:
        cg.add(var.set_destination_address(config[CONF_DESTINATION_ADDRESS]))

    if CONF_PARAMETER_DUMP in config:
        await parameter_dump_to_code(var, config)


async def parameter_dump_to_code(component, config):
    dump_config = config[CONF_PARAMETER_DUMP]
    dump_id = dump_config[CONF_ID]
    rows = load_dump_rows(dump_config[CONF_DEFINITIONS], dump_config[CONF_DEVICE_FAMILY], dump_config[CONF_DEVICE_VARIANT])

    cg.add_define("USE_BSB_PARAMETER_DUMP")
    cg.add_global(cg.RawStatement(render_dump_table(dump_id.id, rows)))
    dump = cg.new_Pvariable(
        dump_id,
        cg.RawExpression(f"{dump_id.id}_parameters"),
        len(rows),
        dump_config[CONF_DEVICE_FAMILY],
        config[CONF_SOURCE_ADDRESS],
        config[CONF_DESTINATION_ADDRESS],
    )
    cg.add(component.set_parameter_dump(dump))

    base = await cg.get_variable(dump_config[web_server_base.CONF_WEB_SERVER_BASE_ID])
    handler = cg.new_Pvariable(dump_config[CONF_REQUEST_HANDLER_ID], base, cg.RawExpression(f"&{dump}->channel()"))
    await cg.register_component(handler, {})
