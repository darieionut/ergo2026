/**
 * @file Services.h
 * @brief Servicii specifice pentru Detector
 *
 * Conține: AlarmService, ServiceMode, ProgrammingService, Startup
 */

#ifndef DETECTOR_SERVICES_H
#define DETECTOR_SERVICES_H

#include <Arduino.h>
#include "../common/Config.h"
#include "../common/Controllers.h"
#include "../common/Services.h"
#include "Config.h"
#include "Models.h"
#include "Controllers.h"

// ============================================================================
// Detector Communication Service - Extended cu funcții de alarmă
// ============================================================================

class DetectorCommunicationService : public CommunicationService {
private:
    uint16_t deviceAddress, receiverAddress, deviceId;
    ConvertBytesToUInt convertBytesToUInt;

public:
    DetectorCommunicationService(CC1101Controller *radio, SerialDebug *debug,
        EEPROMController *eepromController, LEDController *ledController);
    void setup() override;
    void reloadEepromValues() override;

    void sendAlarmStartPackage(uint8_t *buff, uint8_t *len);
    void sendAlarmStopPackage(uint8_t *buff, uint8_t *len);
};

// ============================================================================
// Alarm Service - Gestionarea alarmelor
// ============================================================================

class AlarmService {
private:
    SerialDebug *debug;
    ButtonController *buttonController;
    BuzzerController *buzzerController;
    EEPROMController *eepromController;
    LEDController *ledController;
    TGSController *tgsController;
    DetectorCommunicationService *communicationService;
    OutputController *outputController;

    uint16_t currentAlarmValue, alarmTriggerValue, alarmCount;
    uint8_t lastLedBlinkMode, activatedAlarmSendCount, deactivatedAlarmSendCount;
    bool isOtherAlert, testModeActive, alarmStarted, skipStartup;

    uint32_t lastSendStatusMillis, nextSendStatusMillis, alarmRunningSinceMillis;

    uint8_t *sendBuffer;
    uint8_t *sendBufferLen;

    void startAlarm();
    void stopAlarm();
    void checkOtherAlert();
    void generateNextSendStatusMillis();

public:
    AlarmService(SerialDebug *debug, ButtonController *buttonController, BuzzerController *buzzerController,
        EEPROMController *eepromController, LEDController *ledController, TGSController *tgsController,
        DetectorCommunicationService *communicationService, OutputController *outputController);
    void setup();
    void tick(uint32_t currentMillis);

    void setSendBuffer(uint8_t *sendBufferPtr, uint8_t *sendBufferLenPtr);
    void sendPeriodicStatus(uint32_t currentMillis);

    void setAlarmTriggerValue(uint16_t alarmTrigger);
    uint16_t getAlarmTriggerValue();

    void setIsOtherAlert(bool isOtherAlertParam);

    void setSkipStartup(bool state);
};

// ============================================================================
// Service Mode - Mod de testare
// ============================================================================

class ServiceMode {
private:
    BuzzerController *buzzerController;
    LEDController *ledController;
    OutputController *outputController;
    AlarmService *alarmService;
    DetectorCommunicationService *communicationService;

    bool testRunning;

    uint8_t *sendBuffer;
    uint8_t *sendBufferLen;

public:
    ServiceMode(BuzzerController *buzzerController, LEDController *ledController, OutputController *outputController,
        AlarmService *alarmService, DetectorCommunicationService *communicationService);

    void test1Start();
    void test1Stop();
    void test2Start();
    void test3Start();

    bool isTestRunning();
    void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);
};

// ============================================================================
// Programming Service - Programare prin serial
// ============================================================================

class ProgrammingService {
private:
    AlarmService *alarmService;
    SerialDebug *serialDebug;
    CC1101Controller *cc1101Controller;
    EEPROMController *eepromController;
    DetectorCommunicationService *communicationService;
    ServiceMode *serviceMode;

    uint8_t receiveBuffer[SERIAL_RECEIVE_BUFF_SIZE], workingBuffer[SERIAL_WORKING_BUFF_SIZE], responseBuffer[SERIAL_SEND_BUFF_SIZE];
    uint8_t availableBytesToRead;
    int16_t i, j, k, crc;
    CompleteData dataTemp;

    ConvertBytesToInt convertBytesToInt;
    ConvertBytesToLong convertBytesToLong;

    void serialProgramming();
    uint8_t calculateCRC(const uint8_t *buff, uint8_t len);
    void sendResponse();
    void sendResponseOK(uint8_t commandId);
    void sendResponseFail(uint8_t commandId);

public:
    ProgrammingService(AlarmService *alarmService, SerialDebug *serialDebug, CC1101Controller *cc1101Controller,
        EEPROMController *eepromController, DetectorCommunicationService *communicationService, ServiceMode *serviceMode);
    void setup();
    void tick();
};

// ============================================================================
// Startup Service - Inițializare la pornire
// ============================================================================

class Startup {
private:
    EEPROMController *eepr;

public:
    Startup(EEPROMController *eepr);
    void doStartup();
};

#endif // DETECTOR_SERVICES_H
