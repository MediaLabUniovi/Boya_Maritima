#ifndef SENSOR_CONFIG_TURBIDEZ_H
#define SENSOR_CONFIG_TURBIDEZ_H

// Nombre del sensor
#define SENSOR_TURBIDEZ_NAME "Sensor de Turbidez"

// Que mide
#define SENSOR_TURBIDEZ_HAS_TSS true

// Configuracion hardware
#define TURBIDEZ_ANALOG_PIN 12
#ifndef TURBIDEZ_POWER_PIN
#define TURBIDEZ_POWER_PIN SENSOR_POWER_PIN
#endif
#define TURBIDEZ_POWER_ON_DELAY_MS 1000

// Divisor resistivo usado en el montaje de turbidez original
#define TURBIDEZ_R1 1000.0f
#define TURBIDEZ_R2 2200.0f
#define TURBIDEZ_VOLTAGE_CORRECTION 1.07f

// Configuracion ADC ESP32
#define TURBIDEZ_REFERENCE_VOLTAGE 3.3f
#define TURBIDEZ_ADC_RESOLUTION 4095.0f

// Temperatura por defecto para compensacion/calibracion
#define TURBIDEZ_DEFAULT_TEMPERATURE 25.0f

// Rango valido de TSS estimado
#define TURBIDEZ_TSS_MIN 0.0f
#define TURBIDEZ_TSS_MAX 2500.0f

// Configuracion de lecturas
#define TURBIDEZ_READ_SAMPLES 10
#define TURBIDEZ_READ_DELAY_MS 20

#endif // SENSOR_CONFIG_TURBIDEZ_H
