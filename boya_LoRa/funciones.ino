// Librerías:
#pragma once
#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <math.h>

// Variables globales:
extern const int pinSensor_Temp_1m;
//extern const int pinSensor_pH;
//extern const int pinSensor_T;
//extern const int pin_mos;
extern const int pin_bat;

OneWire oneWireTemp(pinSensor_Temp_1m); // Objeto del sensor de temperatura en pin del sensor
DallasTemperature Temp1m(&oneWireTemp); // Inicialización del objeto
Adafruit_BMP280 bmp;

/* Función de dormir sensores: Desconecta los sensores de la alimentación */
/*
void duerme_sensores(){
  Serial.println("Durmiendo sensores...");
  digitalWrite(pin_mos, LOW);               // Pin del mosfet a LOW
}
*/
/* Función de despertar sensores: Conecta los sensores a alimentación */
/*
void despierta_sensores(){
  Serial.println("Derpertando sensores...");
  digitalWrite(pin_mos, HIGH);            // Pin del mosfet a HIGH
}*/
// Bandera para estado del BME280
bool status_bmp = false;

// Inicialización de sensores (debes llamarla en setup)
void iniciaSensores() {
  Serial.begin(115200);
  delay(1000);

  // Inicializa el BMP280 (cambia 0x76 por 0x77 si es necesario)
  status_bmp = bmp.begin(0x76);
  if (!status_bmp) {
    Serial.println("ERROR: No se encontró el sensor BMP280. Verifica el cableado o dirección I2C.");
    while (1);  // Detiene ejecución
  } else {
    Serial.println("BMP280 detectado e inicializado.");
  }

  Temp1m.begin();
}
uint16_t obten_temp1m(){
  //Temp1m.begin();
  Temp1m.requestTemperatures();
  delay(750);  // Tiempo de conversión típico para DS18B20

  float temperatura1m = Temp1m.getTempCByIndex(0);
  Serial.print("Sensores encontrados: ");
  Serial.println(Temp1m.getDeviceCount());
  Serial.print("Temperatura a 1m: ");
  Serial.println(temperatura1m);

  if (temperatura1m == -127.0) {
    Serial.println("ERROR: No se pudo leer el sensor DS18B20 (1m). Verifica conexión.");
    return 0xFFFF;  // valor especial para indicar error
  }

  return temperatura1m * 100;
}

/* Función de medida de la temperatura de la electrónica: *//*
uint16_t obten_temp_elec(){
  float Temp_analog=analogRead(pinSensor_T);        // Leo el pin del sensor de temperatura
  float temp_V=Temp_analog*3.3/4095;                // Transformo los bits a voltios
  Serial.print("El voltaje del sensor de T es: ");
  Serial.println(temp_V);
  float temperatura=temp_V*(30/3.3);                // Recta de ajuste de la temperatura         
  Serial.print("La temperatura del circuito es: ");
  Serial.println(temperatura);
  return temperatura*100;                           // Multiplico por 100 para trabajar con valores enteros
}*/

/* Función de medida del pH: */
/*
uint16_t obten_pH(){
  float medida_pH= analogRead(pinSensor_pH);  // Leo el pin del sensor de pH
  float pH_a_V=medida_pH*3.3/4095;            // Transformo los bits a voltios
  Serial.print("El voltaje de pH: ");
  Serial.println(pH_a_V);
  float valor_pH=(7/1.65)*pH_a_V;              // Recta de ajuste del pH
  Serial.print("El valor de pH es: ");
  Serial.println(valor_pH);
  return valor_pH*100;                        // Multiplico por 100 para manejar valores enteros
}*/
// Lectura de temperatura desde BME280
uint16_t obten_temp_BMP280() {
  float temperatura = bmp.readTemperature(); // °C
  Serial.print("Temperatura ambiente (BMP280): ");
  Serial.println(temperatura);
  return temperatura * 100;
}
uint16_t obten_presion_BMP280() {
  float presion = bmp.readPressure(); // en Pa
  Serial.print("Presión atmosférica: ");
  Serial.println(presion);
  return presion / 10;  // Convertimos a deci-hPa (hPa * 10)
}

uint16_t obten_bat() {
  uint16_t analogValue = analogRead(pin_bat);
  Serial.print(F("Valor analógico de la batería: ")); Serial.print(analogValue); Serial.println(F(" bits"));
  Serial.println(F("===================")); 
  float VBAT = (float)(analogValue) / 4095 * 1.4545 * 3.3; // Conversión del ADC a voltaje
  float sin_offset = VBAT + 0.3; //Ajuste por el offset de medida
  uint8_t nivel_bateria = constrain((sin_offset - 3.1) * (100.0 / (4.2 - 3.1)), 0, 100); 
  Serial.print("Lectura ADC: ");
  Serial.print(analogValue);  
  Serial.print(" - Voltaje: ");
  Serial.print(sin_offset, 2);
  Serial.print("V - Porcentaje: ");
  Serial.print(nivel_bateria);
  Serial.println("%");

  return nivel_bateria; // Multiplicamos por 100 para usar enteros (ej. 95.34% -> 9534)
}



// Especificación del duty
int duty(){
  return SEND_INTERVAL;
}

void BuildPacket(uint8_t txBuffer[8]) {
  Serial.println("Iniciando medida...");

  uint16_t temp_bmp = obten_temp_BMP280();   // Byte 0-1
  uint16_t temp_1m  = obten_temp1m();        // Byte 2-3
  uint16_t bat      = obten_bat();           // Byte 4-5
  uint16_t presion  = obten_presion_BMP280(); // Byte 6-7

  txBuffer[0] = lowByte(temp_bmp);
  txBuffer[1] = highByte(temp_bmp);

  txBuffer[2] = lowByte(temp_1m);
  txBuffer[3] = highByte(temp_1m);

  txBuffer[4] = lowByte(bat);
  txBuffer[5] = highByte(bat);

  txBuffer[6] = lowByte(presion);
  txBuffer[7] = highByte(presion);

  Serial.print("Buffer enviado: [");
  for (int i = 0; i < 8; i++) {
    Serial.print(txBuffer[i]);
    Serial.print(" ");
  }
  Serial.println("]");
  Serial.println("Fin de la medida");
}
