# Boya_Maritima
Este es el repositorio del proyecto de la Boya Marítima con sensor de PH y Temperatura. Surgido de un TFG de la EPI Gijón y convertido en un proyecto MediaLab para permitir su réplica a toda persona interesada.

## Features

- **Diseño impreso en 3D**: La estructura de la boya emplea elementos impresos en 3D.
- **ESP32 Lilygo T7 V1.3**: El proyecto emplea una Esp32 Lilygo para el procesamiento de los datos de los sensores y su envío por tecnología LoRaWAN.
- **Sensor de pH PH-405C**: Se utiliza este sensor con adaptador de señales incorporado para medir el PH del agua.
- **Sensor de temperatura**: Se utiliza un sensor de temperatura del agua.
- **Baterías 18650 y gestor de baterías**: Incorporar baterías de larga duración y gestor de baterías con regulador de salida y adaptador para la carga.
- **Placa solar de alimentación**: Se emplea una placa solar para alimentar la batería.
  
## PH4502C pH Sensor Module 

### Introduction
El sensor pH PH405C es un dispositivo electrónico diseñado para medir el nivel del pH de un líquido.

### Technical Specifications
- **Rango de medidas:** 0-14 pH
- **Resolución:** 0.01 pH
- **Precisión:** ±0.1 pH (a 25°C)
- **Tiempo de respuesta:** ≤ 1 minut0
- **Temperatura de trabajo:** 0-60°C
- **Voltaje de entrada (Vcc):** 5V DC
- **Voltaje de salida (pH):** 0-5V (proporcional al valor de pH)
- **Tamaño:** 42mm x 32mm x 20mm

### Configuración de los pines y descripción

| Número      | Nombre   | Descripción                           |
|------------|----------|---------------------------------------|
| 1          | V+       | Alimentación (5V DC)           |
| 2          | GND      | Tierra                               |
| 3          | Po       | Valor analógico de salida              |
| 4          | Do       | Valor digital de salida (No usado) |
| 5          | To       | Valor de temperatura (No usado) |

