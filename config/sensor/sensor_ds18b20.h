#ifndef SENSOR_CONFIG_DS18B20_H
#define SENSOR_CONFIG_DS18B20_H

// Nombre del sensor
#define SENSOR_DS18B20_NAME "DS18B20 x3"

// Que mide
#define SENSOR_DS18B20_HAS_TEMPERATURE true

// Tres sondas DS18B20 en el mismo bus OneWire de la Lily (GPIO13).
#undef DS18B20_DATA_PIN
#define DS18B20_DATA_PIN 13

// Transistor/MOSFET para alimentar las sondas DS18B20.
#undef DS18B20_POWER_PIN
#define DS18B20_POWER_PIN 14
#define DS18B20_POWER_ON_DELAY_MS 1000

// IDs de las sondas por profundidad.
#define DS18B20_ADDR_3M {0x28, 0x74, 0x5E, 0x73, 0x81, 0x22, 0x08, 0x16}
#define DS18B20_ADDR_2M {0x28, 0xB9, 0xCF, 0x51, 0x83, 0x22, 0x07, 0xC0}
#define DS18B20_ADDR_1M {0x28, 0x88, 0xFF, 0x5F, 0x5F, 0x20, 0x01, 0x65}

// Configuracion del sensor
#define DS18B20_RESOLUTION 12
#define DS18B20_CONVERSION_DELAY_MS 750

// Rangos validos
#define DS18B20_TEMPERATURE_MIN -55.0f
#define DS18B20_TEMPERATURE_MAX 125.0f

// Configuracion de lecturas
#define DS18B20_READ_ATTEMPTS 3
#define DS18B20_READ_DELAY_MS 100

#endif // SENSOR_CONFIG_DS18B20_H
