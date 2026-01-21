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
// Core Service Implementation
// ============================================================================

CoreService::CoreService(OutputController *outputController, EEPROMController *eepromController, SRAMController *sramController,
            SerialDebug *debug, CommunicationService *communicationService) {
    this->outputController = outputController;
    this->eepromController = eepromController;
    this->sramController = sramController;
    this->debug = debug;
    this->communicationService = communicationService;

    numberOfAddressVerificationIntervals = 0;
}

void CoreService::setup() {
    reloadEepromValues();
    loadAddressVerificationIntervals();
}

void CoreService::tick(uint32_t currentMillis) {
    switch(operationMode) {
        case OPERATION_MODE_MASTER:
            operationModeMaster(currentMillis);
        break;
        case OPERATION_MODE_SLAVE:
            operationModeSlave(currentMillis);
        break;
    }
}

void CoreService::checkOfflineDetectors(uint32_t currentMillis) {
    for(i=0; i<sramController->getNumberOfDetectors(); i++) {
        if(sramController->getDetector(&detectorEntry, i)) {
            if(currentMillis - detectorEntry.lastUpdateMs >= DETECTOR_OFFLINE_MS) {
                detectorEntry.status = 0;
                sramController->addDetector(&detectorEntry);
                offlineDetectorEntry.detectorEntryIndex = i;
                sramController->addOfflineDetector(&offlineDetectorEntry);
            }
        }
    }
}

void CoreService::operationModeMaster(uint32_t currentMillis) {
    if(*receiveBufferLen > 0) {
        index = 0;

        while(index < *receiveBufferLen && *receiveBufferLen > 7) {

            convertBytesToUInt.intVal = deviceAddress;
            if(receiveBuffer[index + BUFF_INDEX_RECEIVER_LO] == convertBytesToUInt.byte[0] && receiveBuffer[index + BUFF_INDEX_RECEIVER_HI] == convertBytesToUInt.byte[1]) {

                if(numberOfAddressVerificationIntervals > 0) {
                    aux = 0;
                    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
                    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
                    for(j=0;j<numberOfAddressVerificationIntervals;j++) {
                        if(addressVerificationIntervals[j].min <= convertBytesToUInt.intVal && convertBytesToUInt.intVal <= addressVerificationIntervals[j].max) {
                            aux = 1;
                            break;
                        }
                    }
                    if(aux == 0) {
                        goto SKIP_PROCESSING;
                    }
                }

                switch(receiveBuffer[index + BUFF_INDEX_COMMAND]) {
                    case(COMMAND_ID_TEST_RSSI):
                        sendRssiMessageFromReceive(index);
                    break;
                    case(COMMAND_ID_ALARM_START):
                        addReceiveToDetectorsList(currentMillis, index);
                        aux = addReceiveToAlarmsList(index, true);
                        if(aux == -1 || aux == -3) {

                            outputController->setPulsed(RELAY_AC, true, OUTPUTS_PULSE_COUNT);
                            outputController->setPulsed(RELAY_DC, true, OUTPUTS_PULSE_COUNT);
                        }
                    break;
                    case(COMMAND_ID_ALARM_STOP):
                        addReceiveToDetectorsList(currentMillis, index);
                        addReceiveToAlarmsList(index, false);

                    break;
                    default:
                        debug->print(F("# UCI: "));
                        debug->println(receiveBuffer[index + BUFF_INDEX_COMMAND]);
                    break;
                }

                doSend();
            }
            SKIP_PROCESSING:
            index += 8;
        }
        *receiveBufferLen = 0;
    }
}

void CoreService::operationModeSlave(uint32_t currentMillis) {
    if(*receiveBufferLen > 0) {
        index = 0;

        while(index < *receiveBufferLen && *receiveBufferLen > 7) {

            convertBytesToUInt.intVal = deviceAddress;
            if(receiveBuffer[index + BUFF_INDEX_RECEIVER_LO] == convertBytesToUInt.byte[0] && receiveBuffer[index + BUFF_INDEX_RECEIVER_HI] == convertBytesToUInt.byte[1]) {

                switch(receiveBuffer[index + BUFF_INDEX_COMMAND]) {
                    case(COMMAND_ID_TEST_RSSI):
                        sendRssiMessageFromReceive(index);
                    break;
                    case(COMMAND_ID_ALARM_START):
                        addReceiveToDetectorsList(currentMillis, index);
                        forwardReceiveMessage(index);
                    break;
                    case(COMMAND_ID_ALARM_STOP):
                        addReceiveToDetectorsList(currentMillis, index);
                        forwardReceiveMessage(index);
                    break;
                    default:
                        debug->print(F("# UCI: "));
                        debug->println(receiveBuffer[index + BUFF_INDEX_COMMAND]);
                    break;
                }

                doSend();
            }
            index += 8;
        }
        *receiveBufferLen = 0;
    }
}

void CoreService::sendRssiMessageFromReceive(uint16_t index) {

    sendBuffer[BUFF_INDEX_RECEIVER_LO] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    sendBuffer[BUFF_INDEX_RECEIVER_HI] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    convertBytesToUInt.intVal = deviceAddress;
    sendBuffer[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    sendBuffer[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    sendBuffer[BUFF_INDEX_ID_LO] = 0;
    sendBuffer[BUFF_INDEX_ID_HI] = 0;
    sendBuffer[BUFF_INDEX_COMMAND] = 1;
    *sendBufferLen = 7;
}

void CoreService::addReceiveToDetectorsList(uint32_t currentMillis, uint16_t index) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    detectorEntry.address = convertBytesToUInt.intVal;
    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    detectorEntry.id = convertBytesToUInt.intVal;
    detectorEntry.linkQuality = (uint8_t)communicationService->getLastMessageComputedLinkQuality();
    detectorEntry.status = 1;
    detectorEntry.lastUpdateMs = currentMillis;

    if(operationMode == OPERATION_MODE_MASTER) {
        i = sramController->addDetector(&detectorEntry);
        if(i >= 0) {

            j = sramController->findOfflineDetector(i);
            if(j >= 0) {
                sramController->deleteOfflineDetector(j);
            }
        }
    } else {
        if(receiveBuffer[index + BUFF_INDEX_IS_FORWARD] != MESSAGE_IS_FORWARDED) {
            i = sramController->addDetector(&detectorEntry);
            if(i >= 0) {

                j = sramController->findOfflineDetector(i);
                if(j >= 0) {
                    sramController->deleteOfflineDetector(j);
                }
            }
        }
    }

}

int16_t CoreService::addReceiveToAlarmsList(uint16_t index, boolean state) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    alarmEntry.address = convertBytesToUInt.intVal;
    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    alarmEntry.id = convertBytesToUInt.intVal;
    alarmEntry.state = state;
    if(state) {
        return sramController->addAlarm(&alarmEntry, false);
    } else {
        return sramController->addAlarm(&alarmEntry, true);
    }
}

void CoreService::removeReceiveFromAlarmsList(uint16_t index) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    eepromController->deleteAlarm(convertBytesToUInt.intVal);
    sramController->deleteAlarm(sramController->findAlarm(convertBytesToUInt.intVal));
}

void CoreService::forwardReceiveMessage(uint16_t index) {
    convertBytesToUInt.intVal = forwardAddress;
    sendBuffer[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    sendBuffer[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    sendBuffer[BUFF_INDEX_ADDRESS_LO] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    sendBuffer[BUFF_INDEX_ADDRESS_HI] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    sendBuffer[BUFF_INDEX_ID_LO] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    sendBuffer[BUFF_INDEX_ID_HI] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    sendBuffer[BUFF_INDEX_COMMAND] = receiveBuffer[index + BUFF_INDEX_COMMAND];
    sendBuffer[BUFF_INDEX_IS_FORWARD] = MESSAGE_IS_FORWARDED;

    *sendBufferLen = 8;
}

void CoreService::setReceiveBuffer(uint8_t *receiveBuffer, uint8_t *receiveBufferLen) {
    this->receiveBuffer = receiveBuffer;
    this->receiveBufferLen = receiveBufferLen;
}

void CoreService::setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

void CoreService::reloadEepromValues() {
    operationMode = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    deviceChannel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
    forwardAddress = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
}

void CoreService::doSend() {
    if(*sendBufferLen > 0) {
        communicationService->send(sendBuffer, sendBufferLen);
        sendBufferLen = 0;
    }
}

void CoreService::loadAddressVerificationIntervals() {
    numberOfAddressVerificationIntervals = eepromController->getNumberOfAddressVerificationIntervals();
    for(i=0;i<numberOfAddressVerificationIntervals;i++) {
        eepromController->getAddressVerificationInterval(i, &addressVerificationIntervals[i]);
    }
}

boolean CoreService::addAddressVerificationInterval(uint16_t min, uint16_t max) {
    if(numberOfAddressVerificationIntervals < 10) {
        addressVerificationIntervals[numberOfAddressVerificationIntervals].min = min;
        addressVerificationIntervals[numberOfAddressVerificationIntervals].max = max;
        if(eepromController->addAddressVerificationInterval(&addressVerificationIntervals[numberOfAddressVerificationIntervals])) {
            numberOfAddressVerificationIntervals++;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

boolean CoreService::clearAddressVerificationIntervals() {
    if(eepromController->clearAddressVerificationIntervals()) {
        numberOfAddressVerificationIntervals = 0;
        return true;
    }
    return false;
}

// ============================================================================
// Menu Service Implementation
// ============================================================================

#include <Configs/MainConfig.h>

MenuService::MenuService(ButtonController *buttonController, EEPROMController *eepromController, SRAMController *sramController) {
    this->buttonController = buttonController;
    this->eepromController = eepromController;
    this->sramController = sramController;

    this->lcd = new LiquidCrystal_I2C(LCD_ADDRESS, LCD_NUMBER_OF_CHARS, LCD_NUMBER_OF_ROWS);
}

void MenuService::setup() {
    lcd->begin();
    lcd->backlight();
    lcd->noBlink();

    setupMenuSystem();

    lastPageId = PAGE_INDEX_HOME;
    currentPageId = PAGE_INDEX_HOME;
    lastButtonClick = 0;
    lastValueRefresh = 0;

    homeAlarmIndex = 0;
    alarmIndex = 0;
    detectorIndex = 0;
    offlineDetectorIndex = 0;

    loadPage(PAGE_INDEX_HOME);
}

void MenuService::tick(uint32_t currentTimeMs) {

    for(i=BTN_BACK; i<=BTN_OK; i++) {
        if(buttonController->hasStateChanged(i)) {
            if(buttonController->getClickedState(i)) {
                onButtonAction(i);
                lastButtonClick = currentTimeMs;
                lastValueRefresh = currentTimeMs;
            }
        }
    }

    if(sramController->getNumberOfAlarms() > 0 && !lcd->getBacklight()) {
        lcd->backlight();
    }

    if(sramController->getNumberOfAlarms() == 0 && currentTimeMs - lastButtonClick >= MENU_BACKLIGHT_OFF_MS) {
        lcd->noBacklight();
    }

    if(currentPageId != PAGE_INDEX_HOME && currentTimeMs - lastButtonClick >= MENU_TIMEOUT_MS) {
        loadPage(PAGE_INDEX_HOME);
        lastValueRefresh = currentTimeMs;
    }

    if(menu[currentPageId].valueRefreshEnabled && currentTimeMs - lastValueRefresh >= MENU_VALUE_REFRESH_MS) {
        refreshDisplayedValues();
        lastValueRefresh = currentTimeMs;
    }
}

void MenuService::onButtonAction(uint8_t button) {
    if(!lcd->getBacklight()) {

        lcd->backlight();
    } else {
        switch(button) {
            case BTN_BACK:
                if(menu[currentPageId].back != currentPageId) {
                    loadPage(menu[currentPageId].back);
                }
            break;
            case BTN_LEFT:
                if(menu[currentPageId].left == menu[currentPageId].right) {

                    if(currentPageId == PAGE_INDEX_LIST_ALARMS) {
                        if(alarmIndex > 0) {
                            alarmIndex--;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS) {
                        if(detectorIndex > 0) {
                            detectorIndex--;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE) {
                        if(offlineDetectorIndex > 0) {
                            offlineDetectorIndex--;
                            loadPage(currentPageId);
                        }
                    }
                } else if(menu[currentPageId].left != currentPageId) {
                    loadPage(menu[currentPageId].left);
                }
            break;
            case BTN_RIGHT:
                if(menu[currentPageId].left == menu[currentPageId].right) {

                    if(currentPageId == PAGE_INDEX_LIST_ALARMS) {
                        if(sramController->getNumberOfAlarms() > 0 && alarmIndex < sramController->getNumberOfAlarms()-1) {
                            alarmIndex++;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS) {
                        if(sramController->getNumberOfDetectors() > 0 && detectorIndex < sramController->getNumberOfDetectors()-1) {
                            detectorIndex++;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE) {
                        if(sramController->getNumberOfOfflineDetectors() > 0 && offlineDetectorIndex < sramController->getNumberOfOfflineDetectors()-1) {
                            offlineDetectorIndex++;
                            loadPage(currentPageId);
                        }
                    }
                } else if(menu[currentPageId].right != currentPageId) {
                    loadPage(menu[currentPageId].right);
                }
            break;
            case BTN_OK:
                if(currentPageId == PAGE_INDEX_CLEAR_ALARMS) {

                    sramController->clearAlarms();
                    eepromController->clearAlarms();
                    homeAlarmIndex = 0;
                }
                if(currentPageId == PAGE_INDEX_CLEAR_DETECTORS) {

                    sramController->clearDetectors();
                    sramController->clearOfflineDetectors();
                }
                if((sramController->getNumberOfAlarms() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_ALARMS) ||
                    (sramController->getNumberOfDetectors() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_DETECTORS) ||
                    (sramController->getNumberOfOfflineDetectors() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_DETECTORS_OFFLINE)) {

                } else if(currentPageId == PAGE_INDEX_HOME) {

                    i = sramController->getNumberOfAlarms();
                    if(i > 0) {
                        alarmIndex = 0;
                        loadPage(PAGE_INDEX_LIST_ALARMS);
                    }
                } else if(menu[currentPageId].ok != currentPageId) {
                    switch(menu[currentPageId].ok) {
                        case PAGE_INDEX_LIST_ALARMS:
                            alarmIndex = 0;
                        break;
                        case PAGE_INDEX_LIST_DETECTORS:
                            detectorIndex = 0;
                        break;
                        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:
                            offlineDetectorIndex = 0;
                        break;
                    }
                    loadPage(menu[currentPageId].ok);
                }
            break;
        }
    }
}

void MenuService::loadPage(uint8_t id) {
    lcd->clear();
    if(menu[id].isLine1Dynamic) {
        setLine1DynamicText(id);
    } else {
        lcd->print(menu[id].textLine1);
    }
    lcd->setCursor(0,1);
    if(menu[id].isLine2Dynamic) {
        setLine2DynamicText(id);
    } else {
        lcd->print(menu[id].textLine2);
    }
    lastPageId = currentPageId;
    currentPageId = id;
}

void MenuService::refreshDisplayedValues() {
    if(menu[currentPageId].isLine1Dynamic) {
        lcd->setCursor(0,0);
        charsPrinted = 0;
        setLine1DynamicText(currentPageId);
        printClearPadding();
    }
    if(menu[currentPageId].isLine2Dynamic) {
        lcd->setCursor(0,1);
        charsPrinted = 0;
        setLine2DynamicText(currentPageId);
        printClearPadding();
    }
}

void MenuService::setLine1DynamicText(uint8_t id) {
    switch(id) {
        case PAGE_INDEX_HOME:
            charsPrinted += lcd->print(MENU_TEXT_ENERGO_INSTAL);
            charsPrinted += lcd->print(MENU_TEXT_VERSION);
            charsPrinted += lcd->print(APP_VERSION/10);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DOT);
            charsPrinted += lcd->print(APP_VERSION%10);
        break;
        case PAGE_INDEX_LIST_ALARMS:

            charsPrinted += lcd->print(MENU_TEXT_ALARM);
            charsPrinted += lcd->print(alarmIndex+1);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
            if(sramController->getAlarm(&alarmEntry, alarmIndex)) {
                charsPrinted += lcd->print(alarmEntry.id);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS:

            if(sramController->getDetector(&detectorEntry, detectorIndex)) {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(detectorEntry.id);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:

            if(sramController->getOfflineDetector(&offlineDetectorEntry, offlineDetectorIndex) &&
                sramController->getDetector(&detectorEntry, offlineDetectorEntry.detectorEntryIndex)) {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(detectorEntry.id);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_FIRMWARE:

            charsPrinted += lcd->print(MENU_TEXT_FIRMWARE);
            charsPrinted += lcd->print(MENU_TEXT_VERSION);
            charsPrinted += lcd->print(APP_VERSION/10);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DOT);
            charsPrinted += lcd->print(APP_VERSION%10);
        break;
        default:
            charsPrinted += lcd->print(MENU_TEXT_NOT_IMPLEMENTED);
        break;
    }
}

void MenuService::setLine2DynamicText(uint8_t id) {
    switch(id) {
        case PAGE_INDEX_HOME:

            i = sramController->getNumberOfAlarms();
            if(i == 0) {
                charsPrinted += lcd->print(MENU_TEXT_EVERYTHING_OK);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_ALARM);
                charsPrinted += lcd->print(homeAlarmIndex+1);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                if(sramController->getAlarm(&alarmEntry, homeAlarmIndex)) {
                    charsPrinted += lcd->print(alarmEntry.id);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_NA);
                }
                homeAlarmIndex++;
                if(homeAlarmIndex >= i) homeAlarmIndex = 0;
            }
        break;
        case PAGE_INDEX_ALARMS:

            if(sramController->getNumberOfAlarms() == 0) {
                charsPrinted += lcd->print(MENU_TEXT_NO_ALARMS);
            } else {
                charsPrinted += lcd->print(sramController->getNumberOfAlarms());
                charsPrinted += lcd->print(MENU_TEXT_ALARM_OR_ALARMS);
            }
        break;
        case PAGE_INDEX_DETECTORS:

            charsPrinted += lcd->print(MENU_TEXT_DETECTORS);
            charsPrinted += lcd->print(sramController->getNumberOfDetectors());
        break;
        case PAGE_INDEX_DETECTORS_OFFLINE:

            charsPrinted += lcd->print(MENU_TEXT_DETECTORS);
            charsPrinted += lcd->print(sramController->getNumberOfOfflineDetectors());
        break;
        case PAGE_INDEX_LIST_ALARMS:

            if(sramController->getAlarm(&alarmEntry, alarmIndex)) {
                charsPrinted += lcd->print(alarmEntry.address);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DASH_SPACED);
                if(alarmEntry.state) {
                    charsPrinted += lcd->print(MENU_TEXT_ON);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_OFF);
                }
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS:

            if(sramController->getDetector(&detectorEntry, detectorIndex)) {
                if(detectorEntry.status == 1) {
                charsPrinted += lcd->print(MENU_TEXT_ONLINE);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_OFFLINE);
                }
                charsPrinted += lcd->print(MENU_TEXT_LQI_OPEN);
                charsPrinted += lcd->print(detectorEntry.linkQuality);
                charsPrinted += lcd->print(MENU_TEXT_LQI_CLOSED);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:

            if(sramController->getOfflineDetector(&offlineDetectorEntry, offlineDetectorIndex) &&
                sramController->getDetector(&detectorEntry, offlineDetectorEntry.detectorEntryIndex)) {
                printTimeFromMs(millis()-detectorEntry.lastUpdateMs);
            }
        break;
        default:
            charsPrinted += lcd->print(MENU_TEXT_NOT_IMPLEMENTED);
        break;
    }
}

void MenuService::printClearPadding() {
    for(i=charsPrinted; i<LCD_NUMBER_OF_CHARS; i++) {
        lcd->print(MENU_TEXT_SEPARATOR_SPACE);
    }
}

void MenuService::printTimeFromMs(uint32_t timeSinceLastMessageMs) {
    if(timeSinceLastMessageMs > 3600000) {

        charsPrinted += lcd->print(timeSinceLastMessageMs / 3600000);
        charsPrinted += lcd->print(MENU_TEXT_HRS);
        charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
        charsPrinted += lcd->print((timeSinceLastMessageMs % 3600000) / 60000);
        charsPrinted += lcd->print(MENU_TEXT_MIN);
    } else if(timeSinceLastMessageMs > 60000) {

        charsPrinted += lcd->print(timeSinceLastMessageMs / 60000);
        charsPrinted += lcd->print(MENU_TEXT_MIN);
        charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
        charsPrinted += lcd->print((timeSinceLastMessageMs % 60000) / 1000);
        charsPrinted += lcd->print(MENU_TEXT_SEC);
    } else if(timeSinceLastMessageMs > 1000) {
        charsPrinted += lcd->print(timeSinceLastMessageMs / 1000);
        charsPrinted += lcd->print(MENU_TEXT_SEC);
    } else {
        charsPrinted += lcd->print(MENU_TEXT_NA);
    }
}

void MenuService::setupMenuSystem() {
    menu[PAGE_INDEX_HOME].isLine1Dynamic = true;
    menu[PAGE_INDEX_HOME].isLine2Dynamic = true;
    menu[PAGE_INDEX_HOME].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].left = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].right = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_HOME].ok = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].valueRefreshEnabled = true;

    menu[PAGE_INDEX_ALARMS].textLine1 = MENU_TEXT_DETECTOR_ALARMS;
    menu[PAGE_INDEX_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_ALARMS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].left = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].right = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_ALARMS].ok = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_ALARMS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_DETECTORS].textLine1 = MENU_TEXT_CONNECTED;
    menu[PAGE_INDEX_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS].left = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_DETECTORS].right = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS].ok = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_DETECTORS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_DETECTORS_OFFLINE].textLine1 = MENU_TEXT_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].left = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].right = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    menu[PAGE_INDEX_FIRMWARE].textLine2 = MENU_TEXT_SEPARATOR_SPACE;
    menu[PAGE_INDEX_FIRMWARE].isLine1Dynamic = true;
    menu[PAGE_INDEX_FIRMWARE].isLine2Dynamic = false;
    menu[PAGE_INDEX_FIRMWARE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].left = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_FIRMWARE].right = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_FIRMWARE].ok = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].valueRefreshEnabled = false;

    menu[PAGE_INDEX_LIST_ALARMS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].back = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].left = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].right = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].ok = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_LIST_DETECTORS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].back = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].left = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].right = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].ok = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].back = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].left = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].right = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    menu[PAGE_INDEX_CLEAR_ALARMS].textLine1 = MENU_TEXT_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].back = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].left = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].right = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].ok = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].valueRefreshEnabled = false;

    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine1 = MENU_TEXT_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].back = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].left = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].right = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].ok = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].valueRefreshEnabled = false;
}

// ============================================================================
// Programming Service Implementation
// ============================================================================

ProgrammingService::ProgrammingService(SerialDebug *serialDebug, CC1101Controller *cc1101Controller, EEPROMController *eepromController,
    CommunicationService *communicationService, CoreService *coreService, ServiceMode *serviceMode, SRAMController *sramController) {
    this->serialDebug = serialDebug;
    this->cc1101Controller = cc1101Controller;
    this->eepromController = eepromController;
    this->communicationService = communicationService;
    this->coreService = coreService;
    this->serviceMode = serviceMode;
    this->sramController = sramController;
}

void ProgrammingService::setup() {
    if(!Serial) {
        Serial.begin(SERIAL_BAUDRATE);
        pinMode(HW_RXD0_PIN, INPUT_PULLUP);
    }
}

void ProgrammingService::tick() {
    serialProgramming();
}

void ProgrammingService::serialProgramming() {
    availableBytesToRead = Serial.available();
    if(availableBytesToRead > 0) {

        if(availableBytesToRead > SERIAL_RECEIVE_BUFF_SIZE) availableBytesToRead = SERIAL_RECEIVE_BUFF_SIZE;
        Serial.readBytes(receiveBuffer, availableBytesToRead);
        i=0;
        while(i < availableBytesToRead) {

            if(receiveBuffer[i] >= RECV_MSG_MIN_LEN && receiveBuffer[i] <= RECV_MSG_MAX_LEN) {

                for(j=0; j<receiveBuffer[i]; j++) {
                    workingBuffer[j] = receiveBuffer[i+j];
                }

                if(receiveBuffer[i+j] == calculateCRC(workingBuffer, j)) {

                    switch(workingBuffer[1]) {
                        case COMMAND_ID_SET_DEVICE_ADDRESS:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_DEVICE_ADDRESS);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEVICE_ADDRESS);
                            }
                        break;
                        case COMMAND_ID_SET_DEVICE_CHANNEL:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                cc1101Controller->setChannel((uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_DEVICE_CHANNEL);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEVICE_CHANNEL);
                            }
                        break;
                        case COMMAND_ID_SET_FORWARD_ADDRESS:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_FORWARD_ADDRESS);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_FORWARD_ADDRESS);
                            }
                        break;
                        case COMMAND_ID_SET_FORWARD_CHANNEL:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_FORWARD_CHANNEL);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_FORWARD_CHANNEL);
                            }
                        break;
                        case COMMAND_ID_SET_OPERATION_MODE:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 1 && convertBytesToInt.intVal <= 2) {
                                eepromController->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, (uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_OPERATION_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_OPERATION_MODE);
                            }
                        break;
                        case COMMAND_ID_SET_DEBUG_MODE:

                            if(workingBuffer[2] == 0 || workingBuffer[2] == 1) {
                                serialDebug->setDebugEnabled((bool) workingBuffer[2]);
                                sendResponseOK(COMMAND_ID_SET_DEBUG_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEBUG_MODE);
                            }
                        break;
                        case COMMAND_ID_GET_DEVICE_TYPE:

                            responseBuffer[0] = 4;
                            responseBuffer[1] = COMMAND_ID_GET_DEVICE_TYPE;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case COMMAND_ID_GET_ALL_VALUES:

                            responseBuffer[0] = 21;
                            responseBuffer[1] = COMMAND_ID_GET_ALL_VALUES;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
                            responseBuffer[4] = convertBytesToLong.byte[0];
                            responseBuffer[5] = convertBytesToLong.byte[1];
                            responseBuffer[6] = convertBytesToLong.byte[2];
                            responseBuffer[7] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
                            responseBuffer[8] = convertBytesToInt.byte[0];
                            responseBuffer[9] = convertBytesToInt.byte[1];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
                            responseBuffer[10] = convertBytesToLong.byte[0];
                            responseBuffer[11] = convertBytesToLong.byte[1];
                            responseBuffer[12] = convertBytesToLong.byte[2];
                            responseBuffer[13] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE);
                            responseBuffer[14] = convertBytesToInt.byte[0];
                            responseBuffer[15] = convertBytesToInt.byte[1];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
                            responseBuffer[16] = convertBytesToInt.byte[0];
                            responseBuffer[17] = convertBytesToInt.byte[1];

                            responseBuffer[18] = serialDebug->getDebugEnabled();

                            convertBytesToInt.intVal = communicationService->getAvarageLinkQuality();
                            responseBuffer[19] = convertBytesToInt.byte[0];
                            responseBuffer[20] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case COMMAND_ID_SET_ALL_VALUES:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal == DEVICE_TYPE) {
                                l=1;

                                convertBytesToLong.byte[0] = workingBuffer[4];
                                convertBytesToLong.byte[1] = workingBuffer[5];
                                convertBytesToLong.byte[2] = workingBuffer[6];
                                convertBytesToLong.byte[3] = workingBuffer[7];
                                if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.address = convertBytesToLong.longVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[8];
                                convertBytesToInt.byte[1] = workingBuffer[9];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.channel = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToLong.byte[0] = workingBuffer[10];
                                convertBytesToLong.byte[1] = workingBuffer[11];
                                convertBytesToLong.byte[2] = workingBuffer[12];
                                convertBytesToLong.byte[3] = workingBuffer[13];
                                if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.forwardAddress = convertBytesToLong.longVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[14];
                                convertBytesToInt.byte[1] = workingBuffer[15];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.forwardChannel = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[16];
                                convertBytesToInt.byte[1] = workingBuffer[17];
                                if(convertBytesToInt.intVal >= 1 && convertBytesToInt.intVal <= 3) {
                                    dataTemp.operationMode = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                if(workingBuffer[18] == 0 || workingBuffer[18] == 1) {
                                    dataTemp.debugEnabled = (bool) workingBuffer[18];
                                } else {
                                    l=0;
                                }

                                if(l == 1) {
                                    eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, dataTemp.address);
                                    eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, dataTemp.channel);
                                    cc1101Controller->setChannel(dataTemp.channel);
                                    eepromController->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, dataTemp.forwardAddress);
                                    eepromController->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, dataTemp.forwardChannel);
                                    eepromController->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, dataTemp.operationMode);
                                    serialDebug->setDebugEnabled(dataTemp.debugEnabled);

                                    communicationService->reloadEepromValues();
                                    coreService->reloadEepromValues();

                                    sendResponseOK(COMMAND_ID_SET_ALL_VALUES);
                                } else {
                                    sendResponseFail(COMMAND_ID_SET_ALL_VALUES);
                                }
                            } else {
                                sendResponseFail(COMMAND_ID_SET_ALL_VALUES);
                            }
                        break;
                        case COMMAND_ID_STREAM_ALL_MEMORY:

                            sendResponseStartStream(COMMAND_ID_STREAM_ALL_MEMORY);
                            delay(100);

                            for(l=0;l<sramController->getNumberOfAlarms();l++) {
                                sramController->getAlarm(&alarmEntry, l);

                                responseBuffer[0] = 11;
                                responseBuffer[1] = COMMAND_ID_STREAM_ALL_MEMORY;

                                convertBytesToLong.longVal = alarmEntry.address;
                                responseBuffer[2] = convertBytesToLong.byte[0];
                                responseBuffer[3] = convertBytesToLong.byte[1];
                                responseBuffer[4] = convertBytesToLong.byte[2];
                                responseBuffer[5] = convertBytesToLong.byte[3];

                                convertBytesToLong.longVal = alarmEntry.id;
                                responseBuffer[6] = convertBytesToLong.byte[0];
                                responseBuffer[7] = convertBytesToLong.byte[1];
                                responseBuffer[8] = convertBytesToLong.byte[2];
                                responseBuffer[9] = convertBytesToLong.byte[3];

                                responseBuffer[10] = alarmEntry.state;

                                sendResponse();
                                delay(100);
                            }

                            for(l=0;l<sramController->getNumberOfDetectors();l++) {
                                sramController->getDetector(&detectorEntry, l);

                                responseBuffer[0] = 17;
                                responseBuffer[1] = COMMAND_ID_STREAM_ALL_MEMORY;

                                convertBytesToLong.longVal = detectorEntry.address;
                                responseBuffer[2] = convertBytesToLong.byte[0];
                                responseBuffer[3] = convertBytesToLong.byte[1];
                                responseBuffer[4] = convertBytesToLong.byte[2];
                                responseBuffer[5] = convertBytesToLong.byte[3];

                                convertBytesToLong.longVal = detectorEntry.id;
                                responseBuffer[6] = convertBytesToLong.byte[0];
                                responseBuffer[7] = convertBytesToLong.byte[1];
                                responseBuffer[8] = convertBytesToLong.byte[2];
                                responseBuffer[9] = convertBytesToLong.byte[3];

                                convertBytesToInt.intVal = detectorEntry.linkQuality;
                                responseBuffer[10] = convertBytesToInt.byte[0];
                                responseBuffer[11] = convertBytesToInt.byte[1];

                                responseBuffer[12] = detectorEntry.status;

                                convertBytesToULong.longVal = detectorEntry.lastUpdateMs;
                                responseBuffer[13] = convertBytesToULong.byte[0];
                                responseBuffer[14] = convertBytesToULong.byte[1];
                                responseBuffer[15] = convertBytesToULong.byte[2];
                                responseBuffer[16] = convertBytesToULong.byte[3];

                                sendResponse();
                                delay(100);
                            }

                            sendResponseOK(COMMAND_ID_STREAM_ALL_MEMORY);
                        break;
                        case COMMAND_ID_CLEAR_ALARMS:

                            sramController->clearAlarms();
                            eepromController->clearAlarms();
                            sendResponseOK(COMMAND_ID_CLEAR_ALARMS);
                        break;
                        case COMMAND_ID_CLEAR_DETECTORS:

                            sramController->clearDetectors();
                            sramController->clearOfflineDetectors();
                            sendResponseOK(COMMAND_ID_CLEAR_DETECTORS);
                        break;
                        case COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];
                            l = convertBytesToLong.longVal;

                            convertBytesToLong.byte[0] = workingBuffer[6];
                            convertBytesToLong.byte[1] = workingBuffer[7];
                            convertBytesToLong.byte[2] = workingBuffer[8];
                            convertBytesToLong.byte[3] = workingBuffer[9];

                            if(coreService->addAddressVerificationInterval(l, convertBytesToLong.longVal)) {
                                sendResponseOK(COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL);
                            } else {
                                sendResponseFail(COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL);
                            }
                        break;
                        case COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS:

                            if(coreService->clearAddressVerificationIntervals()) {
                                sendResponseOK(COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS);
                            } else {
                                sendResponseFail(COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS);
                            }
                        break;
                        case COMMAND_ID_TEST_MODE:

                            if(workingBuffer[2] == 1) {
                                serviceMode->test1Start();
                                sendResponseOK(COMMAND_ID_TEST_MODE);
                            } else if(workingBuffer[2] == 0) {
                                serviceMode->test1Stop();
                                sendResponseOK(COMMAND_ID_TEST_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_TEST_MODE);
                            }
                        break;
                        default:

                            responseBuffer[0] = 3;
                            responseBuffer[1] = workingBuffer[1];
                            responseBuffer[2] = -1;

                            sendResponse();
                        break;
                    }

                    i+=j;
                } else {

                    i++;
                }
            } else {

                i++;
            }
        }
    }
}

uint8_t ProgrammingService::calculateCRC(uint8_t *buff, uint8_t len) {
    crc = 0;
    for(k=0; k<len; k++) {
        crc += buff[k];
    }
    return (char)(crc % 256);
}

void ProgrammingService::sendResponseStartStream(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 2;

    sendResponse();
}

void ProgrammingService::sendResponseOK(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 1;

    sendResponse();
}

void ProgrammingService::sendResponseFail(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 0;

    sendResponse();
}

void ProgrammingService::sendResponse() {
    responseBuffer[(int)responseBuffer[0]] = calculateCRC(responseBuffer, responseBuffer[0]);

    Serial.write(responseBuffer, responseBuffer[0]+1);
    Serial.flush();
}

// ============================================================================
// Serial Debug Implementation
// ============================================================================

SerialDebug::SerialDebug() {
    debugEnabled = DEBUG_ENABLED;
}

SerialDebug::SerialDebug(boolean enableDebug) {
    this->debugEnabled = enableDebug;
}

void SerialDebug::setup() {
    Serial.begin(SERIAL_BAUDRATE);
    pinMode(HW_RXD0_PIN, INPUT_PULLUP);
    if(debugEnabled) Serial.println(F("# DE"));
}

void SerialDebug::setDebugEnabled(boolean state) {
    debugEnabled = state;
}

boolean SerialDebug::getDebugEnabled() {
    return debugEnabled;
}

void SerialDebug::printDebugStart() {
    if(debugEnabled) Serial.print(F("# "));
}

size_t SerialDebug::print(const __FlashStringHelper *m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(const String &m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(const char m[]) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(int8_t m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(uint8_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(int16_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(uint16_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(int32_t  m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(uint32_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(float m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(const Printable &m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::println(const __FlashStringHelper *m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(const String &m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(const char m[]) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(int8_t m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(uint8_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(int16_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(uint16_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(int32_t  m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(uint32_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(float m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(const Printable &m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(void) {
    if(debugEnabled) return Serial.println();
    else return 0;
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

void ServiceMode::setSendBuffer(uint8_t*sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

// ============================================================================
// Startup Implementation
// ============================================================================

Startup::Startup(EEPROMController *eepr, SRAMController *sram) {
    this->eepr = eepr;
    this->sram = sram;
}

void Startup::doStartup() {

    pinMode(HW_IO_NC_PIN, INPUT);
    if(!eepr->getStartupFlag()) {
        randomSeed(analogRead(HW_IO_NC_PIN));
    } else {
        randomSeed(analogRead(HW_IO_NC_PIN) + eepr->getSerialNumber() +
            eepr->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT) + eepr->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
    }

    if(!eepr->getStartupFlag() || EEPROM_WIPE) {

        eepr->wipeEEPROM();

        eepr->setSerialNumber(random(100000,2147483640));

        eepr->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, 1);

        eepr->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, 1);
        eepr->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, CC1101_DEFVAL_CHANNR);
        eepr->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, 0);
        eepr->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, 0);

        eepr->setByteValue(EEPROM_ALARMS_MEMORY_INDEX_BYTE, EEPROM_ALARMS_MEMORY_START);
        eepr->setByteValue(EEPROM_ALARMS_MEMORY_COUNT_BYTE, 0);
        eepr->setByteValue(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, 0);

        eepr->setStartupFlag(true);
    }

    eepr->loadAlarmDetails();
    AlarmEntry alarmEntry;
    for(uint8_t i=0;i<eepr->getNumberOfAlarms();i++) {
        eepr->getAlarm(&alarmEntry, i);
        sram->addAlarm(&alarmEntry, false, true);
    }
}
