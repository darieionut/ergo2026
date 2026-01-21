/**
 * @file Services.cpp
 * @brief Implementări pentru serviciile Floor Module
 *
 * Implementări pentru: MenuService, CoreService, ServiceMode, ProgrammingService, Startup
 */

#include "Services.h"

// ============================================================================
// Floor Module Communication Service Implementation
// ============================================================================

FloorModuleCommunicationService::FloorModuleCommunicationService(CC1101Controller *radio, SerialDebug *debug,
    EEPROMController *eepromController, LEDController *ledController)
    : CommunicationService(radio, debug, eepromController, ledController) {
}

void FloorModuleCommunicationService::setup() {
    CommunicationService::setup();
}

void FloorModuleCommunicationService::reloadEepromValues() {
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    deviceChannel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
    forwardAddress = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
    forwardChannel = eepromController->getByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE);

    radio->setChannel(deviceChannel);
    radio->setSyncWord(syncWord);
}

// ============================================================================
// Menu Service Implementation
// ============================================================================

MenuService::MenuService(ButtonController *buttonController, EEPROMController *eepromController, SRAMController *sramController) {
    this->buttonController = buttonController;
    this->eepromController = eepromController;
    this->sramController = sramController;
}

void MenuService::setup() {
    lcd = new LiquidCrystal_I2C(LCD_ADDRESS, LCD_NUMBER_OF_CHARS, LCD_NUMBER_OF_ROWS);
    lcd->init();
    lcd->backlight();

    setupMenuSystem();
    loadPage(PAGE_INDEX_HOME);

    homeAlarmIndex = 0;
    alarmIndex = 0;
    detectorIndex = 0;
    offlineDetectorIndex = 0;
    lastButtonClick = 0;
    lastValueRefresh = 0;
}

void MenuService::setupMenuSystem() {
    // Home page
    menu[PAGE_INDEX_HOME].textLine1 = MENU_TEXT_ENERGO_INSTAL;
    menu[PAGE_INDEX_HOME].textLine2 = MENU_TEXT_EVERYTHING_OK;
    menu[PAGE_INDEX_HOME].isLine1Dynamic = true;
    menu[PAGE_INDEX_HOME].isLine2Dynamic = true;
    menu[PAGE_INDEX_HOME].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].left = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_HOME].right = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_HOME].ok = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].valueRefreshEnabled = true;

    // Alarms page
    menu[PAGE_INDEX_ALARMS].textLine1 = MENU_TEXT_DETECTOR_ALARMS;
    menu[PAGE_INDEX_ALARMS].textLine2 = MENU_TEXT_NO_ALARMS;
    menu[PAGE_INDEX_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_ALARMS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].left = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].right = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_ALARMS].ok = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_ALARMS].valueRefreshEnabled = true;

    // Detectors page
    menu[PAGE_INDEX_DETECTORS].textLine1 = MENU_TEXT_ONLINE;
    menu[PAGE_INDEX_DETECTORS].textLine2 = MENU_TEXT_DETECTORS;
    menu[PAGE_INDEX_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS].left = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_DETECTORS].right = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS].ok = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_DETECTORS].valueRefreshEnabled = true;

    // Offline detectors page
    menu[PAGE_INDEX_DETECTORS_OFFLINE].textLine1 = MENU_TEXT_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].textLine2 = MENU_TEXT_DETECTORS;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].left = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].right = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    // List alarms page
    menu[PAGE_INDEX_LIST_ALARMS].textLine1 = MENU_TEXT_ALARM;
    menu[PAGE_INDEX_LIST_ALARMS].textLine2 = NULL;
    menu[PAGE_INDEX_LIST_ALARMS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].back = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].left = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].right = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].ok = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].valueRefreshEnabled = false;

    // List detectors page
    menu[PAGE_INDEX_LIST_DETECTORS].textLine1 = MENU_TEXT_DETECTOR;
    menu[PAGE_INDEX_LIST_DETECTORS].textLine2 = NULL;
    menu[PAGE_INDEX_LIST_DETECTORS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].back = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].left = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].right = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].ok = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].valueRefreshEnabled = true;

    // List offline detectors page
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].textLine1 = MENU_TEXT_DETECTOR;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].textLine2 = NULL;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].back = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].left = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].right = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    // Clear alarms page
    menu[PAGE_INDEX_CLEAR_ALARMS].textLine1 = MENU_TEXT_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_CLEAR_ALARMS].left = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_CLEAR_ALARMS].right = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_ALARMS].ok = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].valueRefreshEnabled = false;

    // Clear detectors page
    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine1 = MENU_TEXT_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_CLEAR_DETECTORS].left = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].right = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_CLEAR_DETECTORS].ok = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].valueRefreshEnabled = false;

    // Firmware page
    menu[PAGE_INDEX_FIRMWARE].textLine1 = MENU_TEXT_FIRMWARE;
    menu[PAGE_INDEX_FIRMWARE].textLine2 = MENU_TEXT_VERSION;
    menu[PAGE_INDEX_FIRMWARE].isLine1Dynamic = false;
    menu[PAGE_INDEX_FIRMWARE].isLine2Dynamic = true;
    menu[PAGE_INDEX_FIRMWARE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].left = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_FIRMWARE].right = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].ok = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_FIRMWARE].valueRefreshEnabled = false;
}

void MenuService::tick(uint32_t currentTimeMs) {
    if (buttonController->hasAnyStateChanged()) {
        lastButtonClick = currentTimeMs;
        lcd->backlight();

        if (buttonController->hasStateChanged(BTN_BACK)) {
            onButtonAction(BTN_BACK);
        } else if (buttonController->hasStateChanged(BTN_LEFT)) {
            onButtonAction(BTN_LEFT);
        } else if (buttonController->hasStateChanged(BTN_RIGHT)) {
            onButtonAction(BTN_RIGHT);
        } else if (buttonController->hasStateChanged(BTN_OK)) {
            onButtonAction(BTN_OK);
        }

        buttonController->getAllClickedStates();
    }

    // Auto refresh values
    if (menu[currentPageId].valueRefreshEnabled && currentTimeMs - lastValueRefresh >= MENU_VALUE_REFRESH_MS) {
        refreshDisplayedValues();
        lastValueRefresh = currentTimeMs;
    }

    // Backlight timeout
    if (currentTimeMs - lastButtonClick >= MENU_BACKLIGHT_OFF_MS) {
        lcd->noBacklight();
    }

    // Menu timeout - return to home
    if (currentTimeMs - lastButtonClick >= MENU_TIMEOUT_MS && currentPageId != PAGE_INDEX_HOME) {
        loadPage(PAGE_INDEX_HOME);
    }
}

void MenuService::onButtonAction(uint8_t button) {
    switch (button) {
        case BTN_BACK:
            loadPage(menu[currentPageId].back);
            break;
        case BTN_LEFT:
            if (currentPageId == PAGE_INDEX_LIST_ALARMS && alarmIndex > 0) {
                alarmIndex--;
                refreshDisplayedValues();
            } else if (currentPageId == PAGE_INDEX_LIST_DETECTORS && detectorIndex > 0) {
                detectorIndex--;
                refreshDisplayedValues();
            } else if (currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE && offlineDetectorIndex > 0) {
                offlineDetectorIndex--;
                refreshDisplayedValues();
            } else {
                loadPage(menu[currentPageId].left);
            }
            break;
        case BTN_RIGHT:
            if (currentPageId == PAGE_INDEX_LIST_ALARMS && alarmIndex < sramController->getNumberOfAlarms() - 1) {
                alarmIndex++;
                refreshDisplayedValues();
            } else if (currentPageId == PAGE_INDEX_LIST_DETECTORS && detectorIndex < sramController->getNumberOfDetectors() - 1) {
                detectorIndex++;
                refreshDisplayedValues();
            } else if (currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE && offlineDetectorIndex < sramController->getNumberOfOfflineDetectors() - 1) {
                offlineDetectorIndex++;
                refreshDisplayedValues();
            } else {
                loadPage(menu[currentPageId].right);
            }
            break;
        case BTN_OK:
            if (currentPageId == PAGE_INDEX_CLEAR_ALARMS) {
                sramController->clearAlarms();
                loadPage(PAGE_INDEX_HOME);
            } else if (currentPageId == PAGE_INDEX_CLEAR_DETECTORS) {
                sramController->clearDetectors();
                sramController->clearOfflineDetectors();
                loadPage(PAGE_INDEX_HOME);
            } else {
                loadPage(menu[currentPageId].ok);
            }
            break;
    }
}

void MenuService::loadPage(uint8_t id) {
    lastPageId = currentPageId;
    currentPageId = id;

    lcd->clear();
    lcd->setCursor(0, 0);

    if (!menu[id].isLine1Dynamic && menu[id].textLine1 != NULL) {
        lcd->print(menu[id].textLine1);
    }

    lcd->setCursor(0, 1);
    if (!menu[id].isLine2Dynamic && menu[id].textLine2 != NULL) {
        lcd->print(menu[id].textLine2);
    }

    if (menu[id].isLine1Dynamic || menu[id].isLine2Dynamic) {
        refreshDisplayedValues();
    }
}

void MenuService::refreshDisplayedValues() {
    if (menu[currentPageId].isLine1Dynamic) {
        lcd->setCursor(0, 0);
        setLine1DynamicText(currentPageId);
    }
    if (menu[currentPageId].isLine2Dynamic) {
        lcd->setCursor(0, 1);
        setLine2DynamicText(currentPageId);
    }
}

void MenuService::setLine1DynamicText(uint8_t id) {
    // Implementation depends on specific page
    switch (id) {
        case PAGE_INDEX_HOME:
            lcd->print(MENU_TEXT_ENERGO_INSTAL);
            if (sramController->getNumberOfAlarms() > 0) {
                lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                lcd->print(sramController->getNumberOfAlarms());
                lcd->print(MENU_TEXT_ALERT_OR_ALERTS);
            }
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_ALARMS:
            lcd->print(MENU_TEXT_ALARM);
            lcd->print(alarmIndex + 1);
            lcd->print(MENU_TEXT_SEPARATOR_DASH);
            lcd->print(sramController->getNumberOfAlarms());
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_DETECTORS:
            lcd->print(MENU_TEXT_DETECTOR);
            lcd->print(detectorIndex + 1);
            lcd->print(MENU_TEXT_SEPARATOR_DASH);
            lcd->print(sramController->getNumberOfDetectors());
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:
            lcd->print(MENU_TEXT_DETECTOR);
            lcd->print(offlineDetectorIndex + 1);
            lcd->print(MENU_TEXT_SEPARATOR_DASH);
            lcd->print(sramController->getNumberOfOfflineDetectors());
            printClearPadding();
            break;
    }
}

void MenuService::setLine2DynamicText(uint8_t id) {
    // Implementation depends on specific page
    switch (id) {
        case PAGE_INDEX_HOME:
            if (sramController->getNumberOfAlarms() == 0) {
                lcd->print(MENU_TEXT_EVERYTHING_OK);
            } else {
                if (sramController->getAlarm(&alarmEntry, homeAlarmIndex)) {
                    lcd->print(alarmEntry.address);
                    lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                    lcd->print(alarmEntry.state ? MENU_TEXT_ON : MENU_TEXT_OFF);
                }
                homeAlarmIndex++;
                if (homeAlarmIndex >= sramController->getNumberOfAlarms()) {
                    homeAlarmIndex = 0;
                }
            }
            printClearPadding();
            break;
        case PAGE_INDEX_ALARMS:
            lcd->print(sramController->getNumberOfAlarms());
            lcd->print(MENU_TEXT_ALARM_OR_ALARMS);
            printClearPadding();
            break;
        case PAGE_INDEX_DETECTORS:
            lcd->print(sramController->getNumberOfDetectors());
            lcd->print(MENU_TEXT_SEPARATOR_SPACE);
            lcd->print(MENU_TEXT_DETECTORS);
            printClearPadding();
            break;
        case PAGE_INDEX_DETECTORS_OFFLINE:
            lcd->print(sramController->getNumberOfOfflineDetectors());
            lcd->print(MENU_TEXT_SEPARATOR_SPACE);
            lcd->print(MENU_TEXT_DETECTORS);
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_ALARMS:
            if (sramController->getAlarm(&alarmEntry, alarmIndex)) {
                lcd->print(alarmEntry.address);
                lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                lcd->print(alarmEntry.state ? MENU_TEXT_ON : MENU_TEXT_OFF);
            }
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_DETECTORS:
            if (sramController->getDetector(&detectorEntry, detectorIndex)) {
                lcd->print(detectorEntry.address);
                lcd->print(MENU_TEXT_LQI_OPEN);
                lcd->print(detectorEntry.linkQuality);
                lcd->print(MENU_TEXT_LQI_CLOSED);
            }
            printClearPadding();
            break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:
            if (sramController->getOfflineDetector(&offlineDetectorEntry, offlineDetectorIndex)) {
                if (sramController->getDetector(&detectorEntry, offlineDetectorEntry.detectorEntryIndex)) {
                    lcd->print(detectorEntry.address);
                    printTimeFromMs(millis() - detectorEntry.lastUpdateMs);
                }
            }
            printClearPadding();
            break;
        case PAGE_INDEX_FIRMWARE:
            lcd->print(MENU_TEXT_VERSION);
            lcd->print(APP_VERSION / 10);
            lcd->print(MENU_TEXT_SEPARATOR_DOT);
            lcd->print(APP_VERSION % 10);
            printClearPadding();
            break;
    }
}

void MenuService::printTimeFromMs(uint32_t timeSinceLastMessageMs) {
    uint32_t seconds = timeSinceLastMessageMs / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;

    lcd->print(MENU_TEXT_SEPARATOR_SPACE);
    if (hours > 0) {
        lcd->print(hours);
        lcd->print(MENU_TEXT_HRS);
    } else if (minutes > 0) {
        lcd->print(minutes);
        lcd->print(MENU_TEXT_MIN);
    } else {
        lcd->print(seconds);
        lcd->print(MENU_TEXT_SEC);
    }
}

void MenuService::printClearPadding() {
    // Print spaces to clear remaining characters
    for (charsPrinted = lcd->getCursorX(); charsPrinted < LCD_NUMBER_OF_CHARS; charsPrinted++) {
        lcd->print(' ');
    }
}

// ============================================================================
// Core Service Implementation
// ============================================================================

CoreService::CoreService(OutputController *outputController, EEPROMController *eepromController, SRAMController *sramController,
    SerialDebug *debug, CommunicationService *communicationService) {
    this->outputController = outputController;
    this->eepromController = eepromController;
    this->sramController = sramController;
    this->debug = debug;
    this->communicationService = communicationService;
}

void CoreService::setup() {
    reloadEepromValues();
    loadAddressVerificationIntervals();
}

void CoreService::reloadEepromValues() {
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    deviceChannel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
    forwardAddress = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
    forwardChannel = eepromController->getByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE);
    operationMode = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
}

void CoreService::tick(uint32_t currentMillis) {
    if (*receiveBufferLen > 0) {
        if (operationMode == OPERATION_MODE_MASTER) {
            operationModeMaster(currentMillis);
        } else if (operationMode == OPERATION_MODE_SLAVE) {
            operationModeSlave(currentMillis);
        }
        *receiveBufferLen = 0;
    }

    doSend();
}

void CoreService::operationModeMaster(uint32_t currentMillis) {
    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_RECEIVER_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_RECEIVER_HI];

    if (convertBytesToUInt.intVal == deviceAddress) {
        convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ADDRESS_LO];
        convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ADDRESS_HI];
        index = sramController->findDetector(convertBytesToUInt.intVal);

        if (receiveBuffer[BUFF_INDEX_COMMAND] == COMMAND_ID_ALARM_START) {
            addReceiveToDetectorsList(currentMillis, index);
            aux = addReceiveToAlarmsList(index, true);
            if (aux >= 0) {
                outputController->set(RELAY_AC, true);
            }
        } else if (receiveBuffer[BUFF_INDEX_COMMAND] == COMMAND_ID_ALARM_STOP) {
            addReceiveToDetectorsList(currentMillis, index);
            removeReceiveFromAlarmsList(index);
            if (sramController->getNumberOfAlarms() == 0) {
                outputController->set(RELAY_AC, false);
            }
        } else if (receiveBuffer[BUFF_INDEX_COMMAND] == COMMAND_ID_TEST_RSSI) {
            addReceiveToDetectorsList(currentMillis, index);
            sendRssiMessageFromReceive(index);
        }
    }
}

void CoreService::operationModeSlave(uint32_t currentMillis) {
    // Similar to master but with forwarding
    operationModeMaster(currentMillis);
}

void CoreService::addReceiveToDetectorsList(uint32_t currentMillis, uint16_t idx) {
    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ADDRESS_HI];
    detectorEntry.address = convertBytesToUInt.intVal;

    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ID_HI];
    detectorEntry.id = convertBytesToUInt.intVal;

    detectorEntry.linkQuality = communicationService->getLastMessageComputedLinkQuality();
    detectorEntry.status = 0;
    detectorEntry.lastUpdateMs = currentMillis;

    sramController->addDetector(&detectorEntry);

    // Remove from offline list if present
    if (idx >= 0) {
        sramController->deleteOfflineDetector(idx);
    }
}

int16_t CoreService::addReceiveToAlarmsList(uint16_t idx, boolean state) {
    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ADDRESS_HI];
    alarmEntry.address = convertBytesToUInt.intVal;

    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ID_HI];
    alarmEntry.id = convertBytesToUInt.intVal;

    alarmEntry.state = state;

    return sramController->addAlarm(&alarmEntry, false);
}

void CoreService::removeReceiveFromAlarmsList(uint16_t idx) {
    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ADDRESS_HI];

    int16_t alarmIdx = sramController->findAlarm(convertBytesToUInt.intVal);
    if (alarmIdx >= 0) {
        sramController->deleteAlarm(alarmIdx);
    }
}

void CoreService::sendRssiMessageFromReceive(uint16_t idx) {
    // Send RSSI response
}

void CoreService::forwardReceiveMessage(uint16_t idx) {
    // Forward message to another module
}

void CoreService::doSend() {
    if (*sendBufferLen > 0) {
        communicationService->send(sendBuffer, sendBufferLen);
    }
}

void CoreService::checkOfflineDetectors(uint32_t currentMillis) {
    for (i = 0; i < sramController->getNumberOfDetectors(); i++) {
        if (sramController->getDetector(&detectorEntry, i)) {
            if (currentMillis - detectorEntry.lastUpdateMs > DETECTOR_OFFLINE_MS) {
                offlineDetectorEntry.detectorEntryIndex = i;
                sramController->addOfflineDetector(&offlineDetectorEntry);
            }
        }
    }
}

void CoreService::setReceiveBuffer(uint8_t *receiveBuffer, uint8_t *receiveBufferLen) {
    this->receiveBuffer = receiveBuffer;
    this->receiveBufferLen = receiveBufferLen;
}

void CoreService::setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

void CoreService::loadAddressVerificationIntervals() {
    // Load from EEPROM
    numberOfAddressVerificationIntervals = 0;
}

boolean CoreService::addAddressVerificationInterval(uint16_t min, uint16_t max) {
    if (numberOfAddressVerificationIntervals < 2) {
        addressVerificationIntervals[numberOfAddressVerificationIntervals].min = min;
        addressVerificationIntervals[numberOfAddressVerificationIntervals].max = max;
        numberOfAddressVerificationIntervals++;
        return true;
    }
    return false;
}

boolean CoreService::clearAddressVerificationIntervals() {
    numberOfAddressVerificationIntervals = 0;
    return true;
}

// ============================================================================
// Service Mode Implementation
// ============================================================================

ServiceMode::ServiceMode(OutputController *outputController, CommunicationService *communicationService) {
    this->outputController = outputController;
    this->communicationService = communicationService;
    testRunning = false;
}

void ServiceMode::test1Start() {
    testRunning = true;
    outputController->set(RELAY_AC, true);
    outputController->set(RELAY_DC, true);
}

void ServiceMode::test1Stop() {
    testRunning = false;
    outputController->set(RELAY_AC, false);
    outputController->set(RELAY_DC, false);
}

bool ServiceMode::isTestRunning() {
    return testRunning;
}

void ServiceMode::setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

// ============================================================================
// Programming Service Implementation
// ============================================================================

ProgrammingService::ProgrammingService(SerialDebug *serialDebug, CC1101Controller *cc1101Controller,
    EEPROMController *eepromController, CommunicationService *communicationService,
    CoreService *coreService, ServiceMode *serviceMode, SRAMController *sramController) {
    this->serialDebug = serialDebug;
    this->cc1101Controller = cc1101Controller;
    this->eepromController = eepromController;
    this->communicationService = communicationService;
    this->coreService = coreService;
    this->serviceMode = serviceMode;
    this->sramController = sramController;
}

void ProgrammingService::setup() {
    // Initialize serial programming
}

void ProgrammingService::tick() {
    if (Serial.available() > 0) {
        serialProgramming();
    }
}

void ProgrammingService::serialProgramming() {
    // Read available bytes
    availableBytesToRead = Serial.available();
    if (availableBytesToRead > SERIAL_RECEIVE_BUFF_SIZE) {
        availableBytesToRead = SERIAL_RECEIVE_BUFF_SIZE;
    }

    for (i = 0; i < availableBytesToRead; i++) {
        receiveBuffer[i] = Serial.read();
    }

    // Process command
    if (availableBytesToRead >= RECV_MSG_MIN_LEN) {
        uint8_t commandId = receiveBuffer[0];
        uint8_t msgLen = receiveBuffer[1];

        if (msgLen >= RECV_MSG_MIN_LEN && msgLen <= RECV_MSG_MAX_LEN) {
            // Process based on command ID
            switch (commandId) {
                case COMMAND_ID_GET_DEVICE_TYPE:
                    responseBuffer[0] = DEVICE_TYPE;
                    responseBuffer[1] = APP_VERSION;
                    sendResponseOK(commandId);
                    break;
                case COMMAND_ID_GET_ALL_VALUES:
                    dataTemp.address = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
                    dataTemp.channel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
                    dataTemp.forwardAddress = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
                    dataTemp.forwardChannel = eepromController->getByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE);
                    dataTemp.operationMode = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
                    dataTemp.debugEnabled = serialDebug->getDebugEnabled();
                    sendResponseOK(commandId);
                    break;
                case COMMAND_ID_TEST_MODE:
                    if (receiveBuffer[2] == 1) {
                        serviceMode->test1Start();
                    } else {
                        serviceMode->test1Stop();
                    }
                    sendResponseOK(commandId);
                    break;
                default:
                    sendResponseFail(commandId);
                    break;
            }
        }
    }
}

uint8_t ProgrammingService::calculateCRC(uint8_t *buff, uint8_t len) {
    crc = 0;
    for (i = 0; i < len; i++) {
        crc ^= buff[i];
    }
    return crc;
}

void ProgrammingService::sendResponse() {
    Serial.write(responseBuffer, responseBuffer[1]);
}

void ProgrammingService::sendResponseStartStream(uint8_t commandId) {
    responseBuffer[0] = commandId;
    responseBuffer[1] = 3;
    responseBuffer[2] = 0xAA;
    sendResponse();
}

void ProgrammingService::sendResponseOK(uint8_t commandId) {
    responseBuffer[0] = commandId;
    responseBuffer[1] = 3;
    responseBuffer[2] = 0x01;
    sendResponse();
}

void ProgrammingService::sendResponseFail(uint8_t commandId) {
    responseBuffer[0] = commandId;
    responseBuffer[1] = 3;
    responseBuffer[2] = 0x00;
    sendResponse();
}

// ============================================================================
// Startup Implementation
// ============================================================================

Startup::Startup(EEPROMController *eepr, SRAMController *sram) {
    this->eepr = eepr;
    this->sram = sram;
}

void Startup::doStartup() {
    // Perform startup procedures
    // Load saved alarms to SRAM if any
}
