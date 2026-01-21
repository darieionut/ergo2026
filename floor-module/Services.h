/**
 * @file Services.h
 * @brief Servicii specifice pentru Floor Module
 *
 * Conține: MenuService, CoreService, ServiceMode, ProgrammingService, Startup
 */

#ifndef FLOOR_MODULE_SERVICES_H
#define FLOOR_MODULE_SERVICES_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "../common/Config.h"
#include "../common/Controllers.h"
#include "../common/Services.h"
#include "Config.h"
#include "Models.h"
#include "Controllers.h"

// ============================================================================
// Floor Module Communication Service
// ============================================================================

class FloorModuleCommunicationService : public CommunicationService {
private:
    uint16_t deviceAddress, forwardAddress;
    uint8_t deviceChannel, forwardChannel;

public:
    FloorModuleCommunicationService(CC1101Controller *radio, SerialDebug *debug,
        EEPROMController *eepromController, LEDController *ledController);
    void setup() override;
    void reloadEepromValues() override;
};

// ============================================================================
// Menu Service - Interfața LCD și navigare meniu
// ============================================================================

class MenuService {
private:
    LiquidCrystal_I2C *lcd;
    ButtonController *buttonController;
    EEPROMController *eepromController;
    SRAMController *sramController;

    MenuElement menu[MENU_NUMBER_OF_PAGES];
    uint8_t currentPageId, lastPageId, charsPrinted;
    uint32_t lastButtonClick, lastValueRefresh;

    AlarmEntry alarmEntry;
    DetectorEntry detectorEntry;
    OfflineDetectorEntry offlineDetectorEntry;

    int8_t line1Text[LCD_NUMBER_OF_CHARS], line2Text[LCD_NUMBER_OF_CHARS];

    uint16_t i, homeAlarmIndex, alarmIndex, detectorIndex, offlineDetectorIndex;

    void onButtonAction(uint8_t button);
    void loadPage(uint8_t id);
    void refreshDisplayedValues();
    void setLine1DynamicText(uint8_t id);
    void setLine2DynamicText(uint8_t id);
    void setupMenuSystem();
    void printTimeFromMs(uint32_t timeSinceLastMessageMs);
    void printClearPadding();

public:
    MenuService(ButtonController *buttonController, EEPROMController *eepromController, SRAMController *sramController);
    void setup();
    void tick(uint32_t currentTimeMs);
};

// ============================================================================
// Core Service - Logica principală
// ============================================================================

class CoreService {
private:
    OutputController *outputController;
    EEPROMController *eepromController;
    SRAMController *sramController;
    SerialDebug *debug;
    CommunicationService *communicationService;

    uint8_t *receiveBuffer, *sendBuffer;
    uint8_t *receiveBufferLen, *sendBufferLen;

    uint16_t deviceAddress, forwardAddress;
    uint8_t operationMode, deviceChannel, forwardChannel;
    int16_t aux;

    AlarmEntry alarmEntry;
    DetectorEntry detectorEntry;
    OfflineDetectorEntry offlineDetectorEntry;

    uint8_t numberOfAddressVerificationIntervals;
    AddressVerificationInterval addressVerificationIntervals[2];

    ConvertBytesToUInt convertBytesToUInt;

    uint16_t i, j, index;

    void addReceiveToDetectorsList(uint32_t currentMillis, uint16_t index);
    int16_t addReceiveToAlarmsList(uint16_t index, boolean state);
    void removeReceiveFromAlarmsList(uint16_t index);
    void sendRssiMessageFromReceive(uint16_t index);
    void forwardReceiveMessage(uint16_t index);
    void operationModeMaster(uint32_t currentMillis);
    void operationModeSlave(uint32_t currentMillis);
    void doSend();

public:
    CoreService(OutputController *outputController, EEPROMController *eepromController, SRAMController *sramController,
        SerialDebug *debug, CommunicationService *communicationService);
    void setup();
    void tick(uint32_t currentMillis);
    void checkOfflineDetectors(uint32_t currentMillis);

    void setReceiveBuffer(uint8_t *receiveBuffer, uint8_t *receiveBufferLen);
    void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);
    void reloadEepromValues();

    void loadAddressVerificationIntervals();
    boolean addAddressVerificationInterval(uint16_t min, uint16_t max);
    boolean clearAddressVerificationIntervals();
};

// ============================================================================
// Service Mode - Mod de testare
// ============================================================================

class ServiceMode {
private:
    OutputController *outputController;
    CommunicationService *communicationService;

    bool testRunning;

    uint8_t *sendBuffer;
    uint8_t *sendBufferLen;

public:
    ServiceMode(OutputController *outputController, CommunicationService *communicationService);

    void test1Start();
    void test1Stop();

    bool isTestRunning();
    void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);
};

// ============================================================================
// Programming Service - Programare prin serial
// ============================================================================

class ProgrammingService {
private:
    SerialDebug *serialDebug;
    CC1101Controller *cc1101Controller;
    EEPROMController *eepromController;
    CommunicationService *communicationService;
    CoreService *coreService;
    ServiceMode *serviceMode;
    SRAMController *sramController;

    uint8_t receiveBuffer[SERIAL_RECEIVE_BUFF_SIZE], workingBuffer[SERIAL_WORKING_BUFF_SIZE], responseBuffer[SERIAL_SEND_BUFF_SIZE];
    uint8_t availableBytesToRead;
    int16_t i, j, k, crc;
    uint16_t l;
    CompleteData dataTemp;

    AlarmEntry alarmEntry;
    DetectorEntry detectorEntry;
    OfflineDetectorEntry offlineDetectorEntry;

    ConvertBytesToInt convertBytesToInt;
    ConvertBytesToLong convertBytesToLong;
    ConvertBytesToULong convertBytesToULong;

    void serialProgramming();
    uint8_t calculateCRC(uint8_t *buff, uint8_t len);
    void sendResponse();
    void sendResponseStartStream(uint8_t commandId);
    void sendResponseOK(uint8_t commandId);
    void sendResponseFail(uint8_t commandId);

public:
    ProgrammingService(SerialDebug *serialDebug, CC1101Controller *cc1101Controller, EEPROMController *eepromController,
        CommunicationService *communicationService, CoreService *coreService, ServiceMode *serviceMode, SRAMController *sramController);
    void setup();
    void tick();
};

// ============================================================================
// Startup Service - Inițializare la pornire
// ============================================================================

class Startup {
private:
    EEPROMController *eepr;
    SRAMController *sram;

public:
    Startup(EEPROMController *eepr, SRAMController *sram);
    void doStartup();
};

#endif // FLOOR_MODULE_SERVICES_H
