#include "../../config/config.h"

#ifdef ENABLE_SENSOR_PH
// Implementacion sensor de pH DFRobot
#include <DFRobot_PH.h>
#include <Preferences.h>
#include "sensor_interface.h"
#include "LoRaBoards.h"

// Objeto global del sensor DFRobot_PH
static DFRobot_PH ph_sensor;

// Estado del sensor
static bool sensor_available = false;
static bool sensor_powered = false;

// Variables para lecturas
static float temperature = PH_DEFAULT_TEMPERATURE;  // Temperatura para compensacion

// Simulación avanzada para testing: rampa suave, ruido y persistencia
#ifndef PH_SIMULATE_DEFAULT
static bool ph_simulate = false;
#else
static bool ph_simulate = PH_SIMULATE_DEFAULT;
#endif

static float ph_sim_base = PH_SIM_BASE;
static float ph_sim_value = PH_SIM_BASE;
static float ph_sim_delta = PH_SIM_DELTA;
static unsigned long ph_sim_interval_ms = (unsigned long)PH_SIM_INTERVAL_MINUTES * 60UL * 1000UL;
static float ph_sim_noise_max = PH_SIM_NOISE_MAX;
static bool ph_sim_persist_enabled = PH_SIM_PERSISTENCE;

// Forward declaration for persistence save function (required before first use)
static void ph_sim_save_state(void);

// Rampa state
static float ph_sim_start_value = PH_SIM_BASE;
static float ph_sim_target_value = PH_SIM_BASE + PH_SIM_DELTA;
static unsigned long ph_sim_start_time = 0;
static bool ph_sim_direction_up = true;

#if PH_SIM_PERSISTENCE
static Preferences ph_prefs;
static bool ph_prefs_initialized = false;
#endif

/**
 * @brief Enciende alimentacion de sensores
 */
static void sensor_ph_power_on(void) {
#if PH_USE_POWER_CONTROL
    if (sensor_powered) return;
    
    pinMode(PH_POWER_PIN, OUTPUT);
    digitalWrite(PH_POWER_PIN, HIGH);
    sensor_powered = true;
    
    Serial.println("pH: Alimentacion de sensores activada");
    Serial.printf("pH: Esperando %d ms para estabilizacion...\n", PH_POWER_ON_DELAY_MS);
    delay(PH_POWER_ON_DELAY_MS);
#else
    Serial.println("pH: Alimentacion permanente (sin control por MOSFET)");
    sensor_powered = true;
    delay(100);  // Pequeño delay para estabilización
#endif
}

/**
 * @brief Apaga alimentacion de sensores
 */
static void sensor_ph_power_off(void) {
#if PH_USE_POWER_CONTROL
    if (!sensor_powered) return;
    
    digitalWrite(PH_POWER_PIN, LOW);
    sensor_powered = false;
    
    Serial.println("pH: Alimentacion de sensores desactivada");
    delay(1000);  // Delay para garantizar desconexion completa
#else
    // No hacer nada si la alimentación es permanente
    Serial.println("pH: Alimentacion permanente (no se apaga)");
#endif
}

/**
 * @brief Inicializa el sensor de pH DFRobot
 */
bool sensor_ph_init(void) {
    Serial.println("pH: Iniciando sensor de pH DFRobot...");
    
    // Configurar pin ADC
    pinMode(PH_ANALOG_PIN, INPUT);
    
    // Configurar pin de alimentacion
    pinMode(PH_POWER_PIN, OUTPUT);
    
    // Inicializar EEPROM para cargar datos de calibración
    EEPROM.begin(32);
    Serial.println("pH: EEPROM inicializada (32 bytes)");
    
    // Inicializar la libreria DFRobot_PH
    ph_sensor.begin();
    
    Serial.printf("pH: Pin ADC configurado en GPIO%d\n", PH_ANALOG_PIN);
    Serial.printf("pH: Pin de alimentacion configurado en GPIO%d\n", PH_POWER_PIN);
    Serial.println("pH: Libreria DFRobot_PH inicializada");
    
    sensor_available = true;
    
    return true;
}

/**
 * @brief Verifica si el sensor esta disponible
 */
bool sensor_ph_is_available(void) {
    return sensor_available;
}

/**
 * @brief Intenta reinicializar el sensor
 */
bool sensor_ph_retry_init(void) {
    if (sensor_available) return true;
    Serial.println("Reintentando inicializacion del sensor de pH...");
    return sensor_ph_init();
}

/**
 * @brief Lee el valor de pH usando la libreria DFRobot
 */
static float read_ph_value(void) {
    uint32_t sum = 0;
    
    // Tomar multiples muestras y promediar
    for (int i = 0; i < PH_READ_SAMPLES; i++) {
        sum += analogRead(PH_ANALOG_PIN);
        delay(PH_READ_DELAY_MS);
    }
    
    float avg_reading = sum / (float)PH_READ_SAMPLES;
    
    // Convertir a voltaje
    float voltage = (avg_reading / PH_ADC_RESOLUTION) * PH_REFERENCE_VOLTAGE;
    
    Serial.printf("pH: Lectura ADC promedio = %.1f, Voltaje = %.3f V\n", avg_reading, voltage);
    
    // Usar la libreria DFRobot_PH para calcular el pH con compensacion de temperatura
    float ph_value = ph_sensor.readPH(voltage, temperature);
    
    return ph_value;
}

/**
 * @brief Actualiza la temperatura para compensacion de pH
 * @param temp Temperatura en grados C
 */
void sensor_ph_set_temperature(float temp) {
    if (temp >= -50.0f && temp <= 100.0f) {
        temperature = temp;
        Serial.printf("pH: Temperatura actualizada a %.2f grados C\n", temp);
    }
}

/**
 * @brief Lee todos los datos del sensor de pH
 */
bool sensor_ph_read_all(sensor_data_t* data) {
    if (!sensor_available || !data) return false;

    // Si está en modo simulación, generar valor simulado en lugar de leer hardware
    if (ph_simulate) {
        unsigned long now = millis();
        if (ph_sim_start_time == 0) {
            ph_sim_start_time = now;
            ph_sim_start_value = ph_sim_value;
            ph_sim_target_value = ph_sim_base + (ph_sim_direction_up ? ph_sim_delta : -ph_sim_delta);
        }

        float progress = (float)(now - ph_sim_start_time) / (float)ph_sim_interval_ms;
        if (progress >= 1.0f) {
            // alcanzó el objetivo: fijar, preparar siguiente segmento
            ph_sim_value = ph_sim_target_value;
            ph_sim_start_value = ph_sim_value;
            ph_sim_direction_up = !ph_sim_direction_up;
            ph_sim_target_value = ph_sim_base + (ph_sim_direction_up ? ph_sim_delta : -ph_sim_delta);
            ph_sim_start_time = now;
#if PH_SIM_PERSISTENCE
            if (ph_sim_persist_enabled && PH_SIM_SAVE_ON_TOGGLE) ph_sim_save_state();
#endif
            Serial.printf("pH(SIM): Reached target -> %.2f (next %s)\n", ph_sim_value, ph_sim_direction_up ? "up" : "down");
            progress = 0.0f;
        }

        // interpolar
        float interp = ph_sim_start_value + (ph_sim_target_value - ph_sim_start_value) * progress;
        // añadir ruido pequeño
        float noise = ((float)random(-1000, 1000) / 1000.0f) * ph_sim_noise_max;
        ph_sim_value = interp + noise;
        if (ph_sim_value < PH_MIN) ph_sim_value = PH_MIN;
        if (ph_sim_value > PH_MAX) ph_sim_value = PH_MAX;

        data->ph = ph_sim_value;
        Serial.printf("pH(SIM): Valor de pH = %.2f (noise=%.3f)\n", data->ph, noise);
        return true;
    }

    // Encender alimentacion de sensores antes de leer
    sensor_ph_power_on();
    
    // Leer valor de pH
    float ph = read_ph_value();
    
    // Verificar si la lectura es valida
    if (isnan(ph)) {
        Serial.println("pH: ERROR - Sensor no calibrado, devuelve NaN");
        Serial.println("pH: Usa comandos ENTERPH, CALPH (pH 7.0), CALPH (pH 4.0), EXITPH para calibrar");
        data->ph = SENSOR_ERROR_PH;  // Enviar valor de error
    } else if (ph < PH_MIN || ph > PH_MAX) {
        Serial.printf("pH: ADVERTENCIA - Lectura fuera de rango: %.2f\n", ph);
        data->ph = ph;  // Enviar el valor aunque esté fuera de rango
    } else {
        data->ph = ph;
        Serial.printf("pH: Valor de pH = %.2f\n", ph);
    }
    
    // Apagar alimentacion despues de leer
    sensor_ph_power_off();
    
    return true;
}

/**
 * @brief Obtiene el payload del sensor de pH
 */
uint8_t sensor_ph_get_payload(payload_config_t* config) {
    if (!config || !sensor_available) return 0;

    sensor_data_t data;
    if (!sensor_ph_read_all(&data)) {
        data.ph = SENSOR_ERROR_PH;
    }

    // Codificar pH como uint16 (multiplicar por 100)
    uint16_t ph_int = (uint16_t)(data.ph * 100);
    
    config->buffer[0] = ph_int >> 8;
    config->buffer[1] = ph_int & 0xFF;
    
    return 2;  // 2 bytes enviados
}

/**
 * @brief Obtiene el nombre del sensor
 */
const char* sensor_ph_get_name(void) {
    return SENSOR_PH_NAME;
}

/**
 * @brief Fuerza el estado del sensor para testing
 */
void sensor_ph_set_available_for_testing(bool available) {
    sensor_available = available;
    Serial.printf("TESTING: Sensor pH forzado a %s\n", available ? "disponible" : "no disponible");
}

/**
 * @brief Guarda el estado de la simulación en NVS (Preferences)
 */
static void ph_sim_save_state(void) {
#if PH_SIM_PERSISTENCE
    if (!ph_prefs_initialized) {
        ph_prefs.begin("pHSim", false);
        ph_prefs_initialized = true;
    }
    if (ph_sim_persist_enabled) {
        ph_prefs.putBool("init", true);
        ph_prefs.putFloat("val", ph_sim_value);
        ph_prefs.putUChar("dir", ph_sim_direction_up ? 1 : 0);
    }
#endif
}

/**
 * @brief Habilita o deshabilita la simulación de pH
 */
void sensor_ph_enable_simulation(bool enabled) {
    ph_simulate = enabled;
    if (ph_simulate) {
        // seed PRNG razonable
        randomSeed((unsigned long)micros());
        // restaurar estado persistido si está disponible
#if PH_SIM_PERSISTENCE
        if (!ph_prefs_initialized) {
            ph_prefs.begin("pHSim", false);
            ph_prefs_initialized = true;
        }
        if (ph_sim_persist_enabled && ph_prefs.getBool("init", false)) {
            ph_sim_value = ph_prefs.getFloat("val", PH_SIM_BASE);
            ph_sim_direction_up = ph_prefs.getUChar("dir", 1) ? true : false;
        } else {
            ph_sim_value = ph_sim_base;
            ph_sim_direction_up = true;
        }
#endif
        ph_sim_start_value = ph_sim_value;
        ph_sim_target_value = ph_sim_base + (ph_sim_direction_up ? ph_sim_delta : -ph_sim_delta);
        ph_sim_start_time = millis();
        Serial.printf("pH: Simulación habilitada. Base=%.2f, Delta=%.2f, Intervalo=%lu min, Noise=%.3f\n",
            ph_sim_value, ph_sim_delta, (unsigned long)(ph_sim_interval_ms/60000UL), ph_sim_noise_max);
    } else {
        Serial.println("pH: Simulación deshabilitada.");
    }
}

/**
 * @brief Configura parámetros de la simulacion
 */
void sensor_ph_set_simulation_params(float base, float delta, uint32_t interval_minutes) {
    ph_sim_base = base;
    ph_sim_delta = delta;
    ph_sim_interval_ms = (unsigned long)interval_minutes * 60UL * 1000UL;
    ph_sim_start_value = ph_sim_value;
    ph_sim_target_value = ph_sim_base + (ph_sim_direction_up ? ph_sim_delta : -ph_sim_delta);
    ph_sim_start_time = millis();
    Serial.printf("pH: Parámetros de simulación actualizados. Base=%.2f, Delta=%.2f, Intervalo=%u min\n", base, delta, interval_minutes);
    ph_sim_save_state();
}

/**
 * @brief Ajusta la amplitud máxima del ruido en la simulación
 */
void sensor_ph_set_simulation_noise(float max_noise) {
    ph_sim_noise_max = max_noise;
    Serial.printf("pH: Ruido simulado ajustado a %.3f\n", ph_sim_noise_max);
}

/**
 * @brief Habilita/deshabilita persistencia de simulación
 */
void sensor_ph_set_simulation_persistence(bool enabled) {
    ph_sim_persist_enabled = enabled;
#if PH_SIM_PERSISTENCE
    if (!ph_sim_persist_enabled && ph_prefs_initialized) {
        ph_prefs.end();
        ph_prefs_initialized = false;
    }
#endif
    Serial.printf("pH: Persistencia de simulación %s\n", ph_sim_persist_enabled ? "habilitada" : "deshabilitada");
}

#endif // ENABLE_SENSOR_PH
