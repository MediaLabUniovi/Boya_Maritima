/**
 * @file      ttn_decoder_generator.cpp
 * @brief     Generador dinamico de decoders TTN segun configuracion de sensores
 */

#include "../config/config.h"
#include <Arduino.h>

#ifndef SHOW_TTN_DECODER
#define SHOW_TTN_DECODER 0
#endif

static void print_decoder_header() {
    Serial.println(F(""));
    Serial.println(F("==================== DECODIFICADOR PAYLOAD TTN ===================="));
    Serial.println(F(""));
    Serial.println(F("// COPIA Y PEGA ESTE CODIGO EN TTN CONSOLE -> APPLICATIONS -> PAYLOAD FORMATTERS"));
    Serial.println(F("function decodeUplink(input) {"));
    Serial.println(F("  var data = {};"));
    Serial.println(F("  var bytes = input.bytes;"));
    Serial.println(F("  var offset = 0;"));
    Serial.println(F(""));
}

static void print_payload_validation() {
    Serial.printf("  // Validar tamano del payload (%u bytes esperados)\n", PAYLOAD_SIZE_BYTES);
    Serial.printf("  if (bytes.length !== %u) {\n", PAYLOAD_SIZE_BYTES);
    Serial.println(F("    return {"));
    Serial.println(F("      data: data,"));
    Serial.printf("      warnings: ['Payload size should be %u bytes, got ' + bytes.length],\n", PAYLOAD_SIZE_BYTES);
    Serial.println(F("      errors: []"));
    Serial.println(F("    };"));
    Serial.println(F("  }"));
    Serial.println(F(""));
}

static void print_battery_percent_decoder() {
    Serial.println(F("  var battery_raw = bytes[offset++] | (bytes[offset++] << 8);"));
    Serial.println(F("  data.battery_percent = battery_raw;"));
}

static void print_temperature_ext_decoder() {
    Serial.println(F("  var temp_ext_raw = bytes[offset++] | (bytes[offset++] << 8);"));
    Serial.println(F("  data.temperature_ext = temp_ext_raw / 100.0;"));
}

static void print_temperature_water_decoder(const char* field) {
    Serial.printf("  var %s_raw = bytes[offset++] | (bytes[offset++] << 8);\n", field);
    Serial.printf("  data.%s = %s_raw / 100.0;\n", field, field);
}

static void print_humidity_decoder() {
    Serial.println(F("  var humidity_raw = bytes[offset++] | (bytes[offset++] << 8);"));
    Serial.println(F("  data.humidity = humidity_raw / 100.0;"));
}

static void print_pressure_decoder() {
    Serial.println(F("  var pressure_raw = bytes[offset++] | (bytes[offset++] << 8);"));
    Serial.println(F("  data.pressure = pressure_raw / 10.0;"));
}

static void print_turbidez_decoder() {
    Serial.println(F("  var turbidez_raw = bytes[offset++] | (bytes[offset++] << 8);"));
    Serial.println(F("  data.turbidez = turbidez_raw / 100.0;"));
}

static void print_decoder_footer() {
    Serial.println(F(""));
    Serial.println(F("  return { data: data };"));
    Serial.println(F("}"));
    Serial.println(F(""));
    Serial.println(F("==================== FIN DEL DECODIFICADOR ===================="));
    Serial.println(F(""));
}

static void print_configuration_info() {
    Serial.println(F(""));
    Serial.println(F("=== CONFIGURACION BOYA MARITIMA V2 ==="));
    Serial.println(F("Sensores activos:"));

#ifdef ENABLE_SENSOR_BME280
    Serial.println(F("  - BME280 (Temperatura exterior, Humedad, Presion)"));
#endif
#ifdef ENABLE_SENSOR_DS18B20
    Serial.println(F("  - DS18B20 x3 (Temperatura agua 1m, 2m, 3m)"));
#endif
#ifdef ENABLE_SENSOR_TURBIDEZ
    Serial.println(F("  - Sensor de turbidez (TSS/turbidez)"));
#endif

    Serial.println(F(""));
    Serial.printf("Tamano del payload: %u bytes\r\n", PAYLOAD_SIZE_BYTES);
    Serial.printf("Estructura del payload (%u bytes):\r\n", PAYLOAD_SIZE_BYTES);
    Serial.println(F("  2 bytes: Bateria (%) - Little-endian"));
#ifdef ENABLE_SENSOR_BME280
    Serial.println(F("  2 bytes: Temperatura exterior (x100) - Little-endian"));
#endif
#ifdef ENABLE_SENSOR_DS18B20
    Serial.println(F("  2 bytes: Temperatura agua 1m (x100) - Little-endian"));
    Serial.println(F("  2 bytes: Temperatura agua 2m (x100) - Little-endian"));
    Serial.println(F("  2 bytes: Temperatura agua 3m (x100) - Little-endian"));
#endif
#ifdef ENABLE_SENSOR_BME280
    Serial.println(F("  2 bytes: Humedad (x100) - Little-endian"));
    Serial.println(F("  2 bytes: Presion (x10) - Little-endian"));
#endif
#ifdef ENABLE_SENSOR_TURBIDEZ
    Serial.println(F("  2 bytes: Turbidez/TSS (x100) - Little-endian"));
#endif
    Serial.println(F(""));
}

void generate_and_print_ttn_decoder() {
    if (!SHOW_TTN_DECODER) {
        return;
    }

    print_configuration_info();
    print_decoder_header();
    print_payload_validation();
    print_battery_percent_decoder();

#ifdef ENABLE_SENSOR_BME280
    print_temperature_ext_decoder();
#endif
#ifdef ENABLE_SENSOR_DS18B20
    print_temperature_water_decoder("temperature_water_1m");
    print_temperature_water_decoder("temperature_water_2m");
    print_temperature_water_decoder("temperature_water_3m");
#endif
#ifdef ENABLE_SENSOR_BME280
    print_humidity_decoder();
    print_pressure_decoder();
#endif
#ifdef ENABLE_SENSOR_TURBIDEZ
    print_turbidez_decoder();
#endif

    print_decoder_footer();
}

uint16_t generate_ttn_decoder_string(char* buffer, uint16_t max_size) {
    if (!buffer || max_size < 100) return 0;

    uint16_t offset = 0;

    offset += snprintf(buffer + offset, max_size - offset,
        "function decodeUplink(input) {\n"
        "  var data = {};\n"
        "  var bytes = input.bytes;\n"
        "  var offset = 0;\n\n"
        "  if (bytes.length !== %u) {\n"
        "    return { data: data, warnings: ['Payload size should be %u bytes'], errors: [] };\n"
        "  }\n\n"
        "  var battery_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.battery_percent = battery_raw;\n",
        PAYLOAD_SIZE_BYTES,
        PAYLOAD_SIZE_BYTES);

#ifdef ENABLE_SENSOR_BME280
    offset += snprintf(buffer + offset, max_size - offset,
        "  var temp_ext_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.temp_ambiente = temp_ext_raw / 100.0;\n");
#endif

#ifdef ENABLE_SENSOR_DS18B20
    offset += snprintf(buffer + offset, max_size - offset,
        "  var temperature_water_1m_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.temperature_water_1m = temperature_water_1m_raw / 100.0;\n"
        "  var temperature_water_2m_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.temperature_water_2m = temperature_water_2m_raw / 100.0;\n"
        "  var temperature_water_3m_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.temperature_water_3m = temperature_water_3m_raw / 100.0;\n");
#endif

#ifdef ENABLE_SENSOR_BME280
    offset += snprintf(buffer + offset, max_size - offset,
        "  var humidity_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.humidity = humidity_raw / 100.0;\n"
        "  var pressure_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.presion_hPa = pressure_raw / 10.0;\n");
#endif

#ifdef ENABLE_SENSOR_TURBIDEZ
    offset += snprintf(buffer + offset, max_size - offset,
        "  var turbidez_raw = bytes[offset++] | (bytes[offset++] << 8);\n"
        "  data.turbidez = turbidez_raw / 100.0;\n");
#endif

    offset += snprintf(buffer + offset, max_size - offset,
        "\n  return { data: data, warnings: [], errors: [] };\n"
        "}\n");

    return offset;
}
