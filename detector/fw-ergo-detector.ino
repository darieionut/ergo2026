/**
 * @file fw-ergo-detector.ino
 * @brief Fișier principal pentru Detector
 *
 * ERGO Gas Detector - Detector de gaze cu comunicare wireless
 * Versiune: 2.5
 */

#include <avr/wdt.h>
#include <Arduino.h>

#include "Config.h"
#include "Models.h"
#include "Controllers.h"
#include "Services.h"

// ============================================================================
// Global Variables
// ============================================================================

boolean packetWaiting, startupFlag;
uint8_t sendBuffer[CC1101_MAX_TX_BUFFER_SIZE];
uint8_t sendBufferLen;

#ifndef DISABLE_CC1101_RECEIVE
    uint8_t recvBuffer[CC1101_MAX_RX_BUFFER_SIZE];
    uint8_t recvBufferLen;
#endif

// ============================================================================
// Controllers
// ============================================================================

SerialDebug debug;

DetectorButtonController buttonController;
BuzzerController buzzerController;
DetectorEEPROMController eepromController;
DetectorLEDController ledController;
DetectorOutputController outputController;
TGSController tgsController;
CC1101Controller cc1101Controller(&eepromController);

// ============================================================================
// Services
// ============================================================================

Startup startup(&eepromController);

DetectorCommunicationService communicationService(&cc1101Controller, &debug, &eepromController, &ledController);
AlarmService alarmService(&debug, &buttonController, &buzzerController, &eepromController, &ledController,
    &tgsController, &communicationService, &outputController);
ServiceMode serviceMode(&buzzerController, &ledController, &outputController, &alarmService, &communicationService);
ProgrammingService programmingService(&alarmService, &debug, &cc1101Controller, &eepromController, &communicationService, &serviceMode);

// ============================================================================
// Timing Variables
// ============================================================================

uint32_t currentMillis;
uint32_t lastMillis1Hz = 0;
uint32_t lastMillis2Hz = 0;
uint32_t lastMillis10Hz = 0;
uint32_t lastMillis100Hz = 0;
uint32_t lastMillis200Hz = 0;
uint32_t lastMillis1Min = 0;

// ============================================================================
// Interrupt Handler
// ============================================================================

void ihrMessageReceived() {
    packetWaiting = true;
}

// ============================================================================
// CC1101 Receive Handler (optional)
// ============================================================================

#ifndef DISABLE_CC1101_RECEIVE
void cc1101Receive(uint32_t currentMillis) {
    if (packetWaiting) {
        detachInterrupt(CC1101_Interrupt);
        packetWaiting = false;
        communicationService.receive(recvBuffer, &recvBufferLen);
        attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);
    }
}
#endif

// ============================================================================
// Setup Function
// ============================================================================

void setup() {
    // Disable watchdog during setup
    MCUSR = 0;
    wdt_disable();

    // Initialize controllers
    debug.setup();
    buttonController.setup();
    buzzerController.setup();
    eepromController.setup();
    ledController.setup();
    outputController.setup();
    tgsController.setup();

    // Run startup procedure
    startup.doStartup();

    // Check startup flag and enable watchdog
    startupFlag = eepromController.getStartupFlag();
    if (startupFlag && WATCHDOG_ENABLED) {
        wdt_reset();
        wdt_enable(WDTO_8S);
    }

    // Initialize services
    communicationService.setup();
    alarmService.setup();
    programmingService.setup();

    // Initialize send buffer
    sendBufferLen = 0;

    #ifndef DISABLE_CC1101_RECEIVE
        // Attach interrupt for CC1101 receive
        attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);

        recvBufferLen = 0;
        packetWaiting = false;
    #endif

    // Set buffers for services
    alarmService.setSendBuffer(sendBuffer, &sendBufferLen);
    serviceMode.setSendBuffer(sendBuffer, &sendBufferLen);
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    currentMillis = millis();

    // 1 minute tasks
    if (currentMillis - lastMillis1Min >= 60000) {
        // Check startup flag periodically
        if (!startupFlag) {
            startupFlag = eepromController.getStartupFlag();
        }

        lastMillis1Min = currentMillis;
    }

    // 1Hz tasks (1000ms)
    if (currentMillis - lastMillis1Hz >= 1000) {
        if (!serviceMode.isTestRunning()) {
            alarmService.sendPeriodicStatus(currentMillis);
        }

        lastMillis1Hz = currentMillis;
    }

    // 2Hz tasks (500ms)
    if (currentMillis - lastMillis2Hz >= 500) {
        // Watchdog reset
        if (startupFlag && WATCHDOG_ENABLED) {
            wdt_reset();
        }

        // Read TGS sensor
        tgsController.tick();

        // Process alarm logic
        if (!serviceMode.isTestRunning()) {
            alarmService.tick(currentMillis);
        }

        lastMillis2Hz = currentMillis;
    }

    // 10Hz tasks (100ms)
    if (currentMillis - lastMillis10Hz >= 100) {
        buzzerController.tick(currentMillis);
        outputController.tick(currentMillis);
        programmingService.tick();

        lastMillis10Hz = currentMillis;
    }

    // 100Hz tasks (10ms)
    if (currentMillis - lastMillis100Hz >= 10) {
        buttonController.tick(currentMillis);

        lastMillis100Hz = currentMillis;
    }

    // 200Hz tasks (5ms)
    if (currentMillis - lastMillis200Hz >= 5) {
        #ifndef DISABLE_CC1101_RECEIVE
            cc1101Receive(currentMillis);
        #endif
        ledController.tick(currentMillis);

        lastMillis200Hz = currentMillis;
    }
}
