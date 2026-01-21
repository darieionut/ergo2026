/**
 * @file Controllers.cpp
 * @brief Implementări pentru controllerele Detector
 *
 * Implementări pentru: BuzzerController și TGSController
 */

#include "Controllers.h"

// ============================================================================
// Buzzer Controller Implementation
// ============================================================================

BuzzerController::BuzzerController() {
    lastChangeMillis = 0;
    running = false;
    state = false;
}

void BuzzerController::setup() {
    pinMode(HW_OUTPUT_BUZZER_PIN, OUTPUT);
    digitalWrite(HW_OUTPUT_BUZZER_PIN, LOW);
}

void BuzzerController::tick(uint32_t currentMillis) {
    if (running && currentMillis - lastChangeMillis >= BEEPING_INTERVAL_MS) {
        state = !state;
        digitalWrite(HW_OUTPUT_BUZZER_PIN, state);
        lastChangeMillis = currentMillis;
    }
}

void BuzzerController::start() {
    if (!running) {
        lastChangeMillis = 0;
        running = true;
    }
}

void BuzzerController::stop() {
    if (running) {
        running = false;
        digitalWrite(HW_OUTPUT_BUZZER_PIN, LOW);
    }
}

// ============================================================================
// TGS Controller Implementation
// ============================================================================

TGSController::TGSController() {
    currentValue = 0;
    lastValue = 0;
    valueChanged = false;
}

void TGSController::setup() {
    pinMode(HW_TGS_SENSOR_PIN, INPUT);
}

void TGSController::tick() {
    lastValue = currentValue;
    currentValue = analogRead(HW_TGS_SENSOR_PIN);
    valueChanged = (currentValue != lastValue);
}

boolean TGSController::hasValueChanged() {
    return valueChanged;
}

uint16_t TGSController::getValue() {
    return currentValue;
}

// ============================================================================
// Detector LED Controller Implementation
// ============================================================================

DetectorLEDController::DetectorLEDController() : LEDController() {
    lastSemaphoreColor = 0;
    lastSemaphoreState = 0;
    lastSemaphoreChange = 0;
}

void DetectorLEDController::setup() {
    LEDController::setup();

    pinMode(HW_OUTPUT_LED_RED_PIN, OUTPUT);
    pinMode(HW_OUTPUT_LED_YELLOW_PIN, OUTPUT);
    pinMode(HW_OUTPUT_LED_GREEN_PIN, OUTPUT);

    setSemaphore(LED_SEMAPHORE_OFF);
}

void DetectorLEDController::tick(uint32_t currentMillis) {
    // Handle semaphore blinking for red
    if (lastSemaphoreColor == LED_SEMAPHORE_RED && (lastSemaphoreChange + BLINK_INTERVAL_MS) < currentMillis) {
        lastSemaphoreState = !lastSemaphoreState;
        digitalWrite(HW_OUTPUT_LED_RED_PIN, lastSemaphoreState);
        lastSemaphoreChange = currentMillis;
    }

    // Call base class tick for status LED
    LEDController::tick(currentMillis);
}

void DetectorLEDController::setSemaphore(uint8_t color) {
    if (lastSemaphoreColor != color) {
        lastSemaphoreColor = color;
        lastSemaphoreState = 0;
        lastSemaphoreChange = millis();

        // Turn off all semaphore LEDs first
        digitalWrite(HW_OUTPUT_LED_RED_PIN, LOW);
        digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, LOW);
        digitalWrite(HW_OUTPUT_LED_GREEN_PIN, LOW);

        // Turn on the selected color
        switch (color) {
            case LED_SEMAPHORE_RED:
                // Will blink in tick()
                digitalWrite(HW_OUTPUT_LED_RED_PIN, HIGH);
                lastSemaphoreState = 1;
                break;
            case LED_SEMAPHORE_YELLOW:
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, HIGH);
                break;
            case LED_SEMAPHORE_GREEN:
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, HIGH);
                break;
            case LED_SEMAPHORE_OFF:
            default:
                // All LEDs already off
                break;
        }
    }
}

// ============================================================================
// Detector Button Controller Implementation
// ============================================================================

DetectorButtonController::DetectorButtonController() : ButtonController() {
    btnStates[0].pin = HW_INPUT_BTN_TEST_PIN;
    btnStates[0].lastValue = false;
    btnStates[0].timestamp = 0;
    btnStates[0].clicked = false;
    btnStates[0].stateChanged = false;
}

// ============================================================================
// Detector Output Controller Implementation
// ============================================================================

DetectorOutputController::DetectorOutputController() : OutputController() {
    outputPins[RELAY_AC] = HW_OUTPUT_RELAY_PIN;
}

void DetectorOutputController::setup() {
    OutputController::setup();
}

// ============================================================================
// Detector EEPROM Controller Implementation
// ============================================================================

DetectorEEPROMController::DetectorEEPROMController() : EEPROMController() {
}

void DetectorEEPROMController::setStartupFlag(boolean state) {
    // Detector uses different magic numbers (3, 8) vs Floor Module (4, 2)
    if (state) {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 3);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1, 8);
    } else {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 0);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1, 0);
    }
}

boolean DetectorEEPROMController::getStartupFlag() {
    if (eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT) == 3 && eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1) == 8) {
        return true;
    } else {
        return false;
    }
}
