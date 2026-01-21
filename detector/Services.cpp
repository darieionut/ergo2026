/**
 * @file Services.cpp
 * @brief Implementari pentru serviciile Detector
 *
 * Implementari pentru: AlarmService, ServiceMode, ProgrammingService, Startup
 */

#include "Services.h"

// ============================================================================
// Detector Communication Service Implementation
// ============================================================================

DetectorCommunicationService::DetectorCommunicationService(CC1101Controller *radio, SerialDebug *debug,
    EEPROMController *eepromController, LEDController *ledController)
    : CommunicationService(radio, debug, eepromController, ledController) {
    this->radioCommunicationStatus = 0;
}

void DetectorCommunicationService::setup() {
    reloadEepromValues();

    delay(500);

    testConnection();
}

void DetectorCommunicationService::reloadEepromValues() {
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    receiverAddress = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
    deviceId = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);
}

void DetectorCommunicationService::send(const uint8_t *buff, uint8_t *len) {
    testConnection();

    packet.length = *len;
    for(i=0; i<packet.length; i++) {
        packet.data[i] = buff[i];
    }
    bool resp = radio->sendData(packet);
    if(debug->getDebugEnabled()) {
        debug->print(F("# Radio send ["));
        debug->print(resp);
        debug->print(F("]: "));
        for(i=0; i<packet.length; i++) {
            debug->print(packet.data[i]);
            debug->print(F(" "));
        }
        debug->println();
    }
    *len = 0;
}

void DetectorCommunicationService::receive(uint8_t *buff, uint8_t *len) {
    if (radio->receiveData(&packet) > 0) {
        debug->print(F("# Radio recv "));

        lastLqi[indexRssiLqi] = radio->computeLqi(packet.lqi);
        lastRssi[indexRssiLqi] = radio->computeRssi(packet.rssi);
        if(debug->getDebugEnabled()) {
            debug->print(F("[LQI: "));
            debug->print(lastLqi[indexRssiLqi]);
            debug->print(F(" | RSSI: "));
            debug->print(lastRssi[indexRssiLqi]);
            debug->print(F("dBm]: "));
        }
        previousIndexRssiLqi = indexRssiLqi;
        if(indexRssiLqi < RSSI_LQI_SAMPLES-1) indexRssiLqi++;
        else indexRssiLqi = 0;

        if (!packet.crc_ok) {
            debug->println(F("CRC FAIL"));
        }

        if (packet.crc_ok && packet.length > 0) {
            if(debug->getDebugEnabled()) {
                debug->print(F("("));
                debug->print(packet.length);
                debug->print(F("): "));
            }

            for(i=0; i<packet.length; i++) {
                if(debug->getDebugEnabled()) {
                    debug->print(packet.data[i]);
                    debug->print(" ");
                }
                buff[*len+i] = packet.data[i];
            }
            *len += packet.length;

            debug->println();
        }
    }
}

void DetectorCommunicationService::testConnection() {
    if(radioCommunicationStatus == 0) {
        radio->init();
        radio->setSyncWord(syncWord);
        radio->disableAddressCheck();

        if (radio->readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER) == 0 &&
            radio->readReg(CC1101_VERSION, CC1101_STATUS_REGISTER) > 0) {
            debug->println(F("# Radio communication initialized!"));
            radioCommunicationStatus = 1;
        } else {
            debug->println(F("# Could not initilize radio communication!"));
            ((DetectorLEDController*)ledController)->setAnimation(LED_STATUS_COMM_ERROR);
            radioCommunicationStatus = 0;
        }
    }
}

int16_t DetectorCommunicationService::getComputedLinkQuality(int16_t lqi, int16_t rssi) {
    if(lqi <= 0) {
        lqiPercent = 0;
    } else {
        lqiPercent = constrain(map(lqi, 120, 20, 0, 100), 0, 100);
    }
    rssiPercent = constrain(map(rssi, -110, -20, 0, 100), 0, 100);
    return ((lqiPercent * 4) + (rssiPercent * 6)) / 10;
}

int16_t DetectorCommunicationService::getLastMessageComputedLinkQuality() {
    return getComputedLinkQuality(lastLqi[previousIndexRssiLqi], lastRssi[previousIndexRssiLqi]);
}

int16_t DetectorCommunicationService::getAvarageLinkQuality() {
    avarageLqi = 0;
    avarageRssi = 0;
    for(i=0; i<RSSI_LQI_SAMPLES; i++) {
        avarageLqi += lastLqi[i];
        avarageRssi += lastRssi[i];
    }
    avarageLqi /= RSSI_LQI_SAMPLES;
    avarageRssi /= RSSI_LQI_SAMPLES;
    return getComputedLinkQuality(avarageLqi, avarageRssi);
}

void DetectorCommunicationService::sendAlarmStartPackage(uint8_t *buff, uint8_t *len) {
    convertBytesToUInt.intVal = receiverAddress;
    buff[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceAddress;
    buff[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceId;
    buff[BUFF_INDEX_ID_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ID_HI] = convertBytesToUInt.byte[1];
    buff[BUFF_INDEX_COMMAND] = COMMAND_ID_ALARM_START;
    buff[BUFF_INDEX_IS_FORWARD] = 0;
    *len = 8;

    send(buff, len);
}

void DetectorCommunicationService::sendAlarmStopPackage(uint8_t *buff, uint8_t *len) {
    convertBytesToUInt.intVal = receiverAddress;
    buff[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceAddress;
    buff[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceId;
    buff[BUFF_INDEX_ID_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ID_HI] = convertBytesToUInt.byte[1];
    buff[BUFF_INDEX_COMMAND] = COMMAND_ID_ALARM_STOP;
    buff[BUFF_INDEX_IS_FORWARD] = 0;
    *len = 8;

    send(buff, len);
}

// ============================================================================
// Alarm Service Implementation
// ============================================================================

AlarmService::AlarmService(SerialDebug *debug, ButtonController *buttonController, BuzzerController *buzzerController,
    EEPROMController *eepromController, LEDController *ledController, TGSController *tgsController,
    DetectorCommunicationService *communicationService, OutputController *outputController) {
    this->debug = debug;
    this->buttonController = buttonController;
    this->buzzerController = buzzerController;
    this->eepromController = eepromController;
    this->ledController = ledController;
    this->tgsController = tgsController;
    this->communicationService = communicationService;
    this->outputController = outputController;
}

void AlarmService::setup() {
    alarmTriggerValue = eepromController->getUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT);

    testModeActive = false;
    currentAlarmValue = 0;
    lastLedBlinkMode = 0;
    alarmStarted = false;
    isOtherAlert = false;
    skipStartup = false;

    activatedAlarmSendCount = 0;
    deactivatedAlarmSendCount = 0;
    lastSendStatusMillis = 0;
    alarmRunningSinceMillis = 0;

    generateNextSendStatusMillis();
}

void AlarmService::tick(uint32_t currentMillis) {
    if(buttonController->hasStateChanged(BTN_TEST)) {
        if(buttonController->getClickedState(BTN_TEST)) {

            testModeActive = !testModeActive;
            if(testModeActive) {
                startAlarm();
            } else {
                stopAlarm();
            }
        }
    }
    if(!testModeActive) {

        if(!skipStartup && currentMillis <= TGS_STARTUP_DELAY_MS) {

            if(lastLedBlinkMode == LED_SEMAPHORE_OFF) {
                lastLedBlinkMode = LED_SEMAPHORE_YELLOW;
            } else {
                lastLedBlinkMode = LED_SEMAPHORE_OFF;
            }
            ((DetectorLEDController*)ledController)->setSemaphore(lastLedBlinkMode);
        } else {

            currentAlarmValue = tgsController->getValue();

            if(currentAlarmValue > 30) {
                if(currentAlarmValue >= alarmTriggerValue) {
                    alarmCount++;
                    if(alarmCount >= TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM) {

                        if(alarmRunningSinceMillis == 0) {
                            alarmRunningSinceMillis = currentMillis;
                        }
                        startAlarm();
                    }
                } else if(alarmRunningSinceMillis != 0 && (currentMillis - alarmRunningSinceMillis >= ALARM_MIN_ACTIVE_PERIOD_MS)) {
                    alarmRunningSinceMillis = 0;
                    alarmCount = 0;

                    stopAlarm();
                } else {

                    checkOtherAlert();
                }
            } else {

                setIsOtherAlert(true);
            }
        }
    }
}

void AlarmService::setSendBuffer(uint8_t*sendBufferPtr, uint8_t *sendBufferLenPtr) {
    this->sendBuffer = sendBufferPtr;
    this->sendBufferLen = sendBufferLenPtr;
}

void AlarmService::sendPeriodicStatus(uint32_t currentMillis) {
    if(alarmStarted && activatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        activatedAlarmSendCount++;
    } else if(!alarmStarted && deactivatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        deactivatedAlarmSendCount++;
    }
    if(currentMillis - lastSendStatusMillis > nextSendStatusMillis) {
        if(alarmStarted) {
            communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        } else {
            communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        }
        generateNextSendStatusMillis();
        lastSendStatusMillis = currentMillis;
    }
}

void AlarmService::generateNextSendStatusMillis() {
    nextSendStatusMillis = random(ALARM_STATUS_MESSAGE_RESEND_MS_MIN, ALARM_STATUS_MESSAGE_RESEND_MS_MAX);
}

void AlarmService::startAlarm() {
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_RED);
    buzzerController->start();
    if(!alarmStarted) {

        outputController->setPulsed(RELAY_AC, true, OUTPUTS_PULSE_COUNT);
        communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        activatedAlarmSendCount = 1;
        alarmStarted = true;
    }
}

void AlarmService::stopAlarm() {
    checkOtherAlert();

    buzzerController->stop();

    if(alarmStarted) {

        communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        deactivatedAlarmSendCount = 1;
        alarmStarted = false;
    }
}

void AlarmService::checkOtherAlert() {
    if(isOtherAlert) {

        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_YELLOW);
    } else {

        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
    }
}

void AlarmService::setAlarmTriggerValue(uint16_t alarmTrigger) {
    alarmTriggerValue = alarmTrigger;
    eepromController->setUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT, alarmTrigger);
}

uint16_t AlarmService::getAlarmTriggerValue() {
    return alarmTriggerValue;
}

void AlarmService::setIsOtherAlert(bool isOtherAlertParam) {
    this->isOtherAlert = isOtherAlertParam;
    if(!alarmStarted) {
        if(isOtherAlert) {
            ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_YELLOW);
        } else {
            ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
        }
    }
}

void AlarmService::setSkipStartup(bool state) {
    this->skipStartup = state;
}

// ============================================================================
// Service Mode Implementation
// ============================================================================

ServiceMode::ServiceMode(BuzzerController *buzzerController, LEDController *ledController, OutputController *outputController,
    AlarmService *alarmService, DetectorCommunicationService *communicationService) {
    this->buzzerController = buzzerController;
    this->ledController = ledController;
    this->outputController = outputController;
    this->alarmService = alarmService;
    this->communicationService = communicationService;

    testRunning = false;
}

void ServiceMode::test1Start() {
    testRunning = true;

    ((DetectorLEDController*)ledController)->setSemaphore(10);
    buzzerController->start();
    outputController->set(RELAY_AC, true);
}

void ServiceMode::test1Stop() {
    testRunning = false;

    ((DetectorLEDController*)ledController)->setSemaphore(3);
    buzzerController->stop();
    outputController->set(RELAY_AC, false);
}

void ServiceMode::test2Start() {
    communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
}

void ServiceMode::test3Start() {
    alarmService->setSkipStartup(true);
}

bool ServiceMode::isTestRunning() {
    return testRunning;
}

void ServiceMode::setSendBuffer(uint8_t*sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

// ============================================================================
// Programming Service Implementation
// ============================================================================

ProgrammingService::ProgrammingService(AlarmService *alarmService, SerialDebug *serialDebug,
    CC1101Controller *cc1101Controller, EEPROMController *eepromController,
    DetectorCommunicationService *communicationService, ServiceMode *serviceMode) {
    this->alarmService = alarmService;
    this->serialDebug = serialDebug;
    this->cc1101Controller = cc1101Controller;
    this->eepromController = eepromController;
    this->communicationService = communicationService;
    this->serviceMode = serviceMode;
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
                        case 1:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 10 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(1);
                            } else {
                                sendResponseFail(1);
                            }
                        break;
                        case 2:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                cc1101Controller->setChannel((uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(2);
                            } else {
                                sendResponseFail(2);
                            }
                        break;
                        case 3:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(3);
                            } else {
                                sendResponseFail(3);
                            }
                        break;
                        case 4:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(4);
                            } else {
                                sendResponseFail(4);
                            }
                        break;
                        case 5:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 500) {
                                alarmService->setAlarmTriggerValue(convertBytesToInt.intVal);
                                sendResponseOK(5);
                            } else {
                                sendResponseFail(5);
                            }
                        break;
                        case 6:

                            if(workingBuffer[2] == 0 || workingBuffer[2] == 1) {

                                serialDebug->setDebugEnabled((bool) workingBuffer[2]);
                                sendResponseOK(6);
                            } else {
                                sendResponseFail(6);
                            }
                        break;
                        case 10:

                            responseBuffer[0] = 4;
                            responseBuffer[1] = 10;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case 20:

                            responseBuffer[0] = 23;
                            responseBuffer[1] = 20;

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

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
                            responseBuffer[10] = convertBytesToLong.byte[0];
                            responseBuffer[11] = convertBytesToLong.byte[1];
                            responseBuffer[12] = convertBytesToLong.byte[2];
                            responseBuffer[13] = convertBytesToLong.byte[3];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);
                            responseBuffer[14] = convertBytesToLong.byte[0];
                            responseBuffer[15] = convertBytesToLong.byte[1];
                            responseBuffer[16] = convertBytesToLong.byte[2];
                            responseBuffer[17] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = alarmService->getAlarmTriggerValue();
                            responseBuffer[18] = convertBytesToInt.byte[0];
                            responseBuffer[19] = convertBytesToInt.byte[1];

                            responseBuffer[20] = serialDebug->getDebugEnabled();

                            convertBytesToInt.intVal = communicationService->getAvarageLinkQuality();
                            responseBuffer[21] = convertBytesToInt.byte[0];
                            responseBuffer[22] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case 21:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal == DEVICE_TYPE) {

                                convertBytesToLong.byte[0] = workingBuffer[4];
                                convertBytesToLong.byte[1] = workingBuffer[5];
                                convertBytesToLong.byte[2] = workingBuffer[6];
                                convertBytesToLong.byte[3] = workingBuffer[7];
                                if(convertBytesToLong.longVal >= 10 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.address = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToInt.byte[0] = workingBuffer[8];
                                convertBytesToInt.byte[1] = workingBuffer[9];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.channel = convertBytesToInt.intVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToLong.byte[0] = workingBuffer[10];
                                convertBytesToLong.byte[1] = workingBuffer[11];
                                convertBytesToLong.byte[2] = workingBuffer[12];
                                convertBytesToLong.byte[3] = workingBuffer[13];
                                if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.receiverAddress = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToLong.byte[0] = workingBuffer[14];
                                convertBytesToLong.byte[1] = workingBuffer[15];
                                convertBytesToLong.byte[2] = workingBuffer[16];
                                convertBytesToLong.byte[3] = workingBuffer[17];
                                if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.deviceId = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToInt.byte[0] = workingBuffer[18];
                                convertBytesToInt.byte[1] = workingBuffer[19];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 500) {
                                    dataTemp.alarmTrigger = convertBytesToInt.intVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                if(workingBuffer[20] == 0 || workingBuffer[20] == 1) {
                                    dataTemp.debugEnabled = (bool) workingBuffer[20];
                                } else {
                                    sendResponseFail(21);
                                }

                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, dataTemp.address);
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, dataTemp.channel);
                                cc1101Controller->setChannel(dataTemp.channel);
                                eepromController->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, dataTemp.receiverAddress);
                                eepromController->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, dataTemp.deviceId);
                                alarmService->setAlarmTriggerValue(dataTemp.alarmTrigger);
                                serialDebug->setDebugEnabled(dataTemp.debugEnabled);
                                communicationService->reloadEepromValues();

                                sendResponseOK(21);
                            } else {
                                sendResponseFail(21);
                            }
                        break;
                        case 101:

                            if(workingBuffer[2] == 1) {
                                serviceMode->test1Start();
                                sendResponseOK(101);
                            } else if(workingBuffer[2] == 0) {
                                serviceMode->test1Stop();
                                sendResponseOK(101);
                            } else {
                                sendResponseFail(101);
                            }
                        break;
                        case 102:

                            serviceMode->test2Start();
                            sendResponseOK(102);
                        break;
                        case 103:

                            serviceMode->test3Start();
                            sendResponseOK(103);
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

uint8_t ProgrammingService::calculateCRC(const uint8_t *buff, uint8_t len) {
    crc = 0;
    for(k=0; k<len; k++) {
        crc += buff[k];
    }
    return (uint8_t)(crc % 256);
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
    if(debugEnabled) Serial.println(F("# Debug enabled"));
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
// Startup Implementation
// ============================================================================

Startup::Startup(EEPROMController *eepr) {
    this->eepr = eepr;
}

void Startup::doStartup() {

    pinMode(HW_TGS_SENSOR_PIN, INPUT);
    if(!eepr->getStartupFlag()) {
        randomSeed(analogRead(HW_TGS_SENSOR_PIN));
    } else {
        randomSeed(analogRead(HW_TGS_SENSOR_PIN) + eepr->getSerialNumber() + eepr->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT) +
            eepr->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT) + eepr->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
    }

    if(!eepr->getStartupFlag() || EEPROM_WIPE) {

        eepr->wipeEEPROM();

        eepr->setSerialNumber(random(100000,2147483640));

        eepr->setUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT, TGS_DEFAULT_ALERT_VALUE);

        eepr->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, DEFAULT_DEVICE_ID);

        eepr->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, DEFAULT_DEVICE_ID);
        eepr->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, CC1101_DEFVAL_CHANNR);
        eepr->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, DEFAULT_RECEIVER_ADDRESS);

        eepr->setStartupFlag(true);
    }
}
