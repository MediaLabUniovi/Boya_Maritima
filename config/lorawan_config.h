/**
 * @file      lorawan_config_template.h
 * @brief     Plantilla de configuración de claves LoRaWAN
 *
 * Este archivo es una PLANTILLA para configurar las claves LoRaWAN.
 * Para usar:
 * 1. Copia este archivo como lorawan_config.h
 * 2. Actualiza las claves con tus valores reales de TTN
 * 3. El archivo lorawan_config.h será ignorado por git (está en .gitignore)
 *
 * @author    Proyecto IoT de Bajo Consumo
 * @version   1.0
 * @date      2025
 */

#ifndef LORAWAN_CONFIG_H
#define LORAWAN_CONFIG_H

#include <lmic.h>
#include <Arduino.h> // For PROGMEM and uint8_t/u1_t

// ==================== CLAVES LoRaWAN OTAA ====================
// ¡ACTUALIZAR CON TUS VALORES DE TTN!
// Formato: LSB para APPEUI/DEVEUI, MSB para APPKEY
// Obtén estos valores de la consola de TTN en tu aplicación

// Application EUI (AppEUI) - LSB format
// Ejemplo: {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}
static const u1_t PROGMEM APPEUI[8]  = {
  0x1E, 0x65, 0xB0, 0xC5,
  0xA4, 0x29, 0xB1, 0x71
};  // LSB

static const u1_t PROGMEM DEVEUI[8]  = {
  0xD8, 0x20, 0x6E, 0x76,
  0x9B, 0x66, 0x84, 0x63
};  // LSB

static const u1_t PROGMEM APPKEY[16] = {
  0x83, 0x5E, 0x2D, 0x60, 0x6B, 0xEC, 0x13, 0x77,
  0xD6, 0xC1, 0xCF, 0x35, 0xA8, 0xB6, 0xEB, 0x22
};  // MSB

#endif // LORAWAN_CONFIG_H