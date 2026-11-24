#include "../../config/config.h"

#ifdef ENABLE_SENSOR_TURBIDEZ

#include "sensor_interface.h"

static bool sensor_available = false;
static bool sensor_powered = false;
static float calibration_temperature = TURBIDEZ_DEFAULT_TEMPERATURE;

static bool has_power_control(void) {
    return TURBIDEZ_POWER_PIN >= 0;
}

static void sensor_turbidez_power_on(void) {
    if (!has_power_control() || sensor_powered) return;

    pinMode(TURBIDEZ_POWER_PIN, OUTPUT);
    digitalWrite(TURBIDEZ_POWER_PIN, HIGH);
    sensor_powered = true;

    Serial.println("Turbidez: Alimentacion del sensor activada");
    delay(TURBIDEZ_POWER_ON_DELAY_MS);
}

static void sensor_turbidez_power_off(void) {
    if (!has_power_control() || !sensor_powered) return;

    digitalWrite(TURBIDEZ_POWER_PIN, LOW);
    sensor_powered = false;

    Serial.println("Turbidez: Alimentacion del sensor desactivada");
}

static float read_turbidez_voltage(void) {
    uint32_t sum = 0;

    for (int i = 0; i < TURBIDEZ_READ_SAMPLES; i++) {
        sum += analogRead(TURBIDEZ_ANALOG_PIN);
        delay(TURBIDEZ_READ_DELAY_MS);
    }

    float avg_reading = sum / (float)TURBIDEZ_READ_SAMPLES;
    float voltage_pin = avg_reading * (TURBIDEZ_REFERENCE_VOLTAGE / TURBIDEZ_ADC_RESOLUTION);
    float voltage_sensor = (voltage_pin * TURBIDEZ_VOLTAGE_CORRECTION) *
        ((TURBIDEZ_R2 + TURBIDEZ_R1) / TURBIDEZ_R2);

    Serial.printf("Turbidez: ADC promedio = %.1f, Voltaje = %.3f V\n", avg_reading, voltage_sensor);
    return voltage_sensor;
}

static float calculate_tss(float voltage, float temp_c) {
    (void)temp_c;

    if (voltage >= 2.5f) {
        return 0.0f;
    }

    float tss = 1000.0f * (2.5f - voltage);
    if (tss < TURBIDEZ_TSS_MIN) return TURBIDEZ_TSS_MIN;
    if (tss > TURBIDEZ_TSS_MAX) return TURBIDEZ_TSS_MAX;
    return tss;
}

bool sensor_turbidez_init(void) {
    Serial.println("Turbidez: Iniciando sensor de turbidez...");
    pinMode(TURBIDEZ_ANALOG_PIN, INPUT);
    Serial.printf("Turbidez: Pin ADC configurado en GPIO%d\n", TURBIDEZ_ANALOG_PIN);
    if (has_power_control()) {
        pinMode(TURBIDEZ_POWER_PIN, OUTPUT);
        digitalWrite(TURBIDEZ_POWER_PIN, LOW);
        sensor_powered = false;
        Serial.printf("Turbidez: Pin de alimentacion configurado en GPIO%d\n", TURBIDEZ_POWER_PIN);
    }

    sensor_available = true;
    return true;
}

bool sensor_turbidez_is_available(void) {
    return sensor_available;
}

bool sensor_turbidez_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicializacion del sensor de turbidez...");
    return sensor_turbidez_init();
}

bool sensor_turbidez_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    sensor_turbidez_power_on();

    float voltage = read_turbidez_voltage();
    float tss = calculate_tss(voltage, calibration_temperature);

    data->turbidez = tss;
    Serial.printf("Turbidez: TSS estimado = %.2f\n", tss);

    sensor_turbidez_power_off();
    return true;
}

uint8_t sensor_turbidez_get_payload(payload_config_t* config) {
    if (!config || !sensor_available || config->max_size < 2) return 0;

    sensor_data_t data;
    if (!sensor_turbidez_read_all(&data)) {
        data.turbidez = SENSOR_ERROR_TURBIDEZ;
    }

    uint16_t turbidez_int = (uint16_t)(data.turbidez * 100);
    config->buffer[0] = turbidez_int & 0xFF;
    config->buffer[1] = turbidez_int >> 8;

    return 2;
}

const char* sensor_turbidez_get_name(void) {
    return SENSOR_TURBIDEZ_NAME;
}

void sensor_turbidez_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor turbidez forzado a %s\n", available ? "disponible" : "no disponible");
}

void sensor_turbidez_set_temperature(float temp) {
    if (temp >= -50.0f && temp <= 100.0f) {
        calibration_temperature = temp;
        Serial.printf("Turbidez: Temperatura de calibracion actualizada a %.2f grados C\n", temp);
    }
}

#endif // ENABLE_SENSOR_TURBIDEZ
