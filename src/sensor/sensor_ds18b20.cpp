#include "../../config/config.h"

#ifdef ENABLE_SENSOR_DS18B20

#include <OneWire.h>
#include <DallasTemperature.h>
#include "sensor_interface.h"

static OneWire oneWire(DS18B20_DATA_PIN);
static DallasTemperature sensors(&oneWire);

static DeviceAddress sensor_3m = DS18B20_ADDR_3M;
static DeviceAddress sensor_2m = DS18B20_ADDR_2M;
static DeviceAddress sensor_1m = DS18B20_ADDR_1M;

static bool sensor_available = false;
static bool sensor_powered = false;

static bool has_power_control(void) {
    return DS18B20_POWER_PIN >= 0;
}

static void sensor_ds18b20_power_on(void) {
    if (!has_power_control() || sensor_powered) return;

    pinMode(DS18B20_POWER_PIN, OUTPUT);
    digitalWrite(DS18B20_POWER_PIN, HIGH);
    sensor_powered = true;

    Serial.println("DS18B20: Alimentacion de sensores activada");
    delay(DS18B20_POWER_ON_DELAY_MS);
#else
    Serial.println("DS18B20: Alimentación permanente (sin control por MOSFET)");
    sensor_powered = true;
    delay(100);  // Pequeño delay para estabilización
#endif
}

static void sensor_ds18b20_power_off(void) {
    if (!has_power_control() || !sensor_powered) return;

    digitalWrite(DS18B20_POWER_PIN, LOW);
    sensor_powered = false;

    Serial.println("DS18B20: Alimentacion de sensores desactivada");
    delay(1000);
}

static bool is_valid_temperature(float temp) {
    return temp != DEVICE_DISCONNECTED_C &&
        temp >= DS18B20_TEMPERATURE_MIN &&
        temp <= DS18B20_TEMPERATURE_MAX;
}

static bool check_sensor_address(const DeviceAddress address, const char* label) {
    if (!sensors.isConnected(address)) {
        Serial.printf("DS18B20: ERROR - No se encontro la sonda de %s\n", label);
        return false;
    }

    sensors.setResolution(address, DS18B20_RESOLUTION);
    Serial.printf("DS18B20: Sonda de %s encontrada\n", label);
    return true;
}

bool sensor_ds18b20_init(void) {
    Serial.printf("DS18B20: Iniciando bus OneWire en GPIO%d...\n", DS18B20_DATA_PIN);

    sensor_ds18b20_power_on();
    sensors.begin();

    int deviceCount = sensors.getDeviceCount();
    Serial.printf("DS18B20: %d dispositivo(s) encontrado(s) en el bus OneWire\n", deviceCount);

    bool found_1m = check_sensor_address(sensor_1m, "1m");
    bool found_2m = check_sensor_address(sensor_2m, "2m");
    bool found_3m = check_sensor_address(sensor_3m, "3m");

    sensor_available = found_1m || found_2m || found_3m;

    if (!sensor_available) {
        Serial.println("DS18B20: ERROR - No se encontro ninguna de las sondas configuradas");
        Serial.printf("Verifica conexiones: VCC->3.3V, GND->GND, DATA->GPIO%d con resistencia pull-up\n", DS18B20_DATA_PIN);
    } else {
        Serial.println("DS18B20: Bus de 3 sondas inicializado");
    }

    sensor_ds18b20_power_off();
    return sensor_available;
}

bool sensor_ds18b20_is_available(void) {
    return sensor_available;
}

bool sensor_ds18b20_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicializacion del bus DS18B20...");
    return sensor_ds18b20_init();
}

bool sensor_ds18b20_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    data->temperature_1m = SENSOR_ERROR_TEMPERATURE;
    data->temperature_2m = SENSOR_ERROR_TEMPERATURE;
    data->temperature_3m = SENSOR_ERROR_TEMPERATURE;

    sensor_ds18b20_power_on();

    sensors.requestTemperatures();
    delay(DS18B20_CONVERSION_DELAY_MS);

    bool any_valid = false;

    float temp_1m = sensors.getTempC(sensor_1m);
    if (is_valid_temperature(temp_1m)) {
        data->temperature_1m = temp_1m;
        any_valid = true;
        Serial.printf("DS18B20: Temperatura a 1m = %.2f C\n", temp_1m);
    } else {
        Serial.println("DS18B20: ERROR - Lectura invalida en sonda 1m");
    }

    float temp_2m = sensors.getTempC(sensor_2m);
    if (is_valid_temperature(temp_2m)) {
        data->temperature_2m = temp_2m;
        any_valid = true;
        Serial.printf("DS18B20: Temperatura a 2m = %.2f C\n", temp_2m);
    } else {
        Serial.println("DS18B20: ERROR - Lectura invalida en sonda 2m");
    }

    float temp_3m = sensors.getTempC(sensor_3m);
    if (is_valid_temperature(temp_3m)) {
        data->temperature_3m = temp_3m;
        any_valid = true;
        Serial.printf("DS18B20: Temperatura a 3m = %.2f C\n", temp_3m);
    } else {
        Serial.println("DS18B20: ERROR - Lectura invalida en sonda 3m");
    }

    sensor_ds18b20_power_off();
    return any_valid;
}

uint8_t sensor_ds18b20_get_payload(payload_config_t* config) {
    if (!config || !sensor_available || config->max_size < 6) return 0;

    sensor_data_t data;
    if (!sensor_ds18b20_read_all(&data)) {
        data.temperature_1m = SENSOR_ERROR_TEMPERATURE;
        data.temperature_2m = SENSOR_ERROR_TEMPERATURE;
        data.temperature_3m = SENSOR_ERROR_TEMPERATURE;
    }

    uint8_t offset = 0;
    int16_t temp_1m = (int16_t)(data.temperature_1m * 100);
    int16_t temp_2m = (int16_t)(data.temperature_2m * 100);
    int16_t temp_3m = (int16_t)(data.temperature_3m * 100);

    config->buffer[offset++] = temp_1m & 0xFF;
    config->buffer[offset++] = temp_1m >> 8;
    config->buffer[offset++] = temp_2m & 0xFF;
    config->buffer[offset++] = temp_2m >> 8;
    config->buffer[offset++] = temp_3m & 0xFF;
    config->buffer[offset++] = temp_3m >> 8;

    return offset;
}

const char* sensor_ds18b20_get_name(void) {
    return SENSOR_DS18B20_NAME;
}

void sensor_ds18b20_set_available_for_testing(bool available) {
    sensor_available = available;
}

#endif // ENABLE_SENSOR_DS18B20
