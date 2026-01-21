/**
 * @file Controllers.h
 * @brief Controllere specifice pentru Detector
 *
 * Conține: BuzzerController și TGSController
 */

#ifndef DETECTOR_CONTROLLERS_H
#define DETECTOR_CONTROLLERS_H

#include <Arduino.h>
#include "../common/Config.h"
#include "../common/Controllers.h"
#include "Config.h"
#include "Models.h"

// ============================================================================
// Buzzer Controller - Control buzzer/alarmă sonoră
// ============================================================================

class BuzzerController {
private:
    boolean state, running;
    uint32_t lastChangeMillis;

public:
    BuzzerController();
    void setup();
    void tick(uint32_t currentMillis);
    void start();
    void stop();
};

// ============================================================================
// TGS Controller - Control senzor gaz TGS
// ============================================================================

class TGSController {
private:
    uint16_t currentValue, lastValue;
    boolean valueChanged;

public:
    TGSController();
    void setup();
    void tick();
    boolean hasValueChanged();
    uint16_t getValue();
};

// ============================================================================
// Detector LED Controller - Extended cu semaphore
// ============================================================================

class DetectorLEDController : public LEDController {
private:
    uint8_t lastSemaphoreColor, lastSemaphoreState;
    uint32_t lastSemaphoreChange;

public:
    DetectorLEDController();
    void setup() override;
    void tick(uint32_t currentMillis) override;
    void setSemaphore(uint8_t color);
};

// ============================================================================
// Detector Button Controller
// ============================================================================

class DetectorButtonController : public ButtonController {
public:
    DetectorButtonController();
};

// ============================================================================
// Detector Output Controller
// ============================================================================

class DetectorOutputController : public OutputController {
public:
    DetectorOutputController();
    void setup() override;
};

// ============================================================================
// Detector EEPROM Controller
// ============================================================================

class DetectorEEPROMController : public EEPROMController {
public:
    DetectorEEPROMController();

    // Override setStartupFlag with detector-specific values
    void setStartupFlag(boolean state);
    boolean getStartupFlag();
};

#endif // DETECTOR_CONTROLLERS_H
