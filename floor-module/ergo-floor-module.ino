/**
 * @file ergo-floor-module.ino
 * @brief Fișier principal pentru Floor Module
 *
 * ERGO Floor Module - Receptor central pentru sistem de detectare gaze
 * Versiune: 4.0
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

boolean startupFlag;
volatile boolean packetWaiting;
uint8_t sendBuffer[CC1101_MAX_TX_BUFFER_SIZE], recvBuffer[CC1101_MAX_RX_BUFFER_SIZE];
uint8_t sendBufferLen, recvBufferLen;
int32_t communicationStartupDelayMs;

// ============================================================================
// Controllers
// ============================================================================

SerialDebug debug;

FloorModuleButtonController buttonController;
FloorModuleEEPROMController eepromController;
SRAMController sramController(&eepromController);
LEDController ledController;
FloorModuleOutputController outputController;
CC1101Controller cc1101Controller(&eepromController);

// ============================================================================
// Services
// ============================================================================

Startup startup(&eepromController, &sramController);

FloorModuleCommunicationService communicationService(&cc1101Controller, &debug, &eepromController, &ledController);
MenuService menuService(&buttonController, &eepromController, &sramController);
CoreService coreService(&outputController, &eepromController, &sramController, &debug, &communicationService);
ServiceMode serviceMode(&outputController, &communicationService);
ProgrammingService programmingService(&debug, &cc1101Controller, &eepromController, &communicationService, &coreService, &serviceMode, &sramController);

// ============================================================================
// Timing Variables
// ============================================================================

uint32_t currentMillis = 0;
uint32_t lastMillis1Min = 0;
uint32_t lastMillis20Hz = 0;
uint32_t lastMillis10Hz = 0;
uint32_t lastMillis100Hz = 0;
uint32_t lastMillis200Hz = 0;
uint32_t lastMillis2Hz = 0;
uint32_t lastCommunicationRX = 0;

// ============================================================================
// Interrupt Handler
// ============================================================================

void ihrMessageReceived() {
    packetWaiting = true;
}

// ============================================================================
// CC1101 Receive Handler
// ============================================================================

void cc1101Receive() {
    if (packetWaiting) {
        detachInterrupt(CC1101_Interrupt);
        communicationService.receive(recvBuffer, &recvBufferLen);
        lastCommunicationRX = millis();
        packetWaiting = false;
        attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);
    }
}

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
    eepromController.setup();
    ledController.setup();
    outputController.setup();
    sramController.setup();

    // Run startup procedure
    startup.doStartup();

    // Check startup flag and enable watchdog
    startupFlag = eepromController.getStartupFlag();
    if (startupFlag && WATCHDOG_ENABLED) {
        wdt_reset();
        wdt_enable(WDTO_8S);
    }

    // Initialize services
    menuService.setup();
    coreService.setup();
    programmingService.setup();

    // Attach interrupt for CC1101
    attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);

    // Initialize communication
    communicationStartupDelayMs = random(0, 3000);
    sendBufferLen = 0;
    recvBufferLen = 0;
    packetWaiting = false;

    // Set buffers for services
    coreService.setReceiveBuffer(recvBuffer, &recvBufferLen);
    coreService.setSendBuffer(sendBuffer, &sendBufferLen);
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
            if (startupFlag && WATCHDOG_ENABLED) {
                wdt_reset();
                wdt_enable(WDTO_8S);
            }
        }

        // Check for offline detectors
        coreService.checkOfflineDetectors(currentMillis);

        lastMillis1Min = currentMillis;
    }

    // 2Hz tasks (500ms)
    if (currentMillis - lastMillis2Hz >= 500) {
        // Watchdog reset with communication check
        if (startupFlag && WATCHDOG_ENABLED) {
            if (currentMillis - lastCommunicationRX < WATCHDOG_RECEIVE_RESET_MS) {
                wdt_reset();
            }
        }

        lastMillis2Hz = currentMillis;
    }

    // 10Hz tasks (100ms)
    if (currentMillis - lastMillis10Hz >= 100) {
        if (!serviceMode.isTestRunning()) {
            outputController.tick(currentMillis);
        }
        menuService.tick(currentMillis);

        lastMillis10Hz = currentMillis;
    }

    // 20Hz tasks (50ms)
    if (currentMillis - lastMillis20Hz >= 50) {
        programmingService.tick();

        lastMillis20Hz = currentMillis;
    }

    // 100Hz tasks (10ms)
    if (currentMillis - lastMillis100Hz >= 10) {
        // Initialize communication after startup delay
        if (communicationStartupDelayMs != -1 && currentMillis > (unsigned long)communicationStartupDelayMs) {
            communicationService.setup();
            communicationStartupDelayMs = -1;
        }

        coreService.tick(currentMillis);
        buttonController.tick(currentMillis);

        lastMillis100Hz = currentMillis;
    }

    // 200Hz tasks (5ms)
    if (currentMillis - lastMillis200Hz >= 5) {
        cc1101Receive();
        ledController.tick(currentMillis);

        lastMillis200Hz = currentMillis;
    }
}
