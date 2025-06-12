# Boya_Maritima
Este es el repositorio del proyecto de la Boya Marítima con sensor de temperatura y presión. Surgido de un TFG de la EPI Gijón y convertido en un proyecto MediaLab para permitir su réplica a toda persona interesada.

## Características

- **Diseño impreso en 3D**: La estructura de la boya emplea elementos impresos en 3D.
- **ESP32 Lilygo T7 V1.3**: El proyecto emplea una Esp32 Lilygo para el procesamiento de los datos de los sensores y su envío por tecnología LoRaWAN.
- **Sensor de temperatura y presión BMP280**: Se utiliza este sensor I2C para medir temperatura ambiental y presión atmosférica.
- **Baterías 18650 y gestor de baterías**: Incorpora baterías de larga duración y gestor de baterías con regulador de salida y adaptador para la carga.
- **Placa solar de alimentación**: Se emplea una placa solar para alimentar la batería.
  
## Módulo BMP280 (temperatura y presión)
El sensor BMP280 es un dispositivo electrónico diseñado para medir temperatura y presión ambiental con alta precisión.

### Especificaciones técnicas
- **Rango de presión:** 300 - 1100 hPa
- **Rango de temperatura:** -40 a +85 °C
- **Precisión temperatura:** ±1 °C
- **Precisión presión:** ±1 hPa
- **Voltaje de entrada (Vcc):** 3.3V DC
- **Interfaz:** I2C (usada en este proyecto)
- **Tamaño:** 15mm x 12mm (aproximado)

### Configuración de los pines y descripción

| Número      | Nombre   | Descripción                           |
|------------|----------|---------------------------------------|
| 1          | 3V3      | Alimentación 3.3V                     |
| 2          | GND      | Tierra                                 |
| 3          | SDA      | Comunicación I2C                      |
| 4          | SCL      | Comunicación I2C                      |
| 5          | CSB      | No usado (SPI)                        |

## ESP32 Lilygo
El ESP32 Lilygo es un microcontrolador preparado para aplicaciones LoRaWAN con el módulo de antena ya integrado. Se alimenta por puerto USB a 5V.

### Configuración de los pines y descripción
| Número      |  Descripción                           |
|------------|----------------------------------------|
| 15          |  Pin de lectura de temperatura (BMP280)|
| 21          |  SDA I2C (BMP280)                      |
| 22          |  SCL I2C (BMP280)                      |
| 34          |  Medidor de nivel de batería           |

## Bajo consumo de energía
En proyectos de este tipo es fundamental el ahorro de energía por eso para este proyecto se emplean baterías de 18650 con una capacidad de alrededor de 3000mAh conectada a un módulo gestor de baterías que permita su carga con placa solar.


## Funcionamiento
1. **Copia el repositorio**:

   Copia este repositorio a tu máquina local usando:

   ```bash
   git clone https://github.com/MedialabUniovi/Boya_Maritima.git
   ```

2. **Configura el entorno**:

   Abre el proyecto con **Arduino IDE** y comprueba que se encuentren instaladas todas la librerias.

3. **Monta el dispositivo**:

   Conecta la Lilygo a los sensores y demás elementos según el esquemático.

5. **Carga el código**:

   Compila y carga el código al microcontrolador.

6. **Añade la alimentación**:

   Conecta la placa solar de alimentación y orientala correctamente.

## Imágenes relacionadas

![Schematic_Boya](https://github.com/user-attachments/assets/53854baf-ec5f-4ede-a19f-cbfd5a31cf83)
_<p align="center">Figura 1: Esquemático del proyecto </p>_
![BoyaMontada](https://github.com/user-attachments/assets/afe61540-370c-45ae-a216-8b882d690783)
_<p align="center">Figura 2: Boya armada </p>_

## Información adicional

Se recomienda, como se puede observar en la foto, aislar el circuito electrónico tanto con sprays de laca para protejer contra la corrosión como en cajas aislantes de humedad.

