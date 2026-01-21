/**
 * @file Services.cpp
 * @brief Implementări pentru serviciile Detector
 *
 * Implementări pentru: AlarmService, ServiceMode, ProgrammingService, Startup
 */

#include "Services.h"

// ============================================================================
// Detector Communication Service Implementation
// ============================================================================

DetectorCommunicationService::DetectorCommunicationService(CC1101Controller *radio, SerialDebug *debug,
    EEPROMController *eepromController, LEDController *ledController)
    : CommunicationService(radio, debug, eepromController, ledController) {
}

void DetectorCommunicationService::setup() {
    CommunicationService::setup();
}

void DetectorCommunicationService::reloadEepromValues() {
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    receiverAddress = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
    deviceId = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);

    radio->setChannel(eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
    radio->setSyncWord(syncWord);
}

void DetectorCommunicationService::sendAlarmStartPackage(uint8_t *buff, uint8_t *len) {
    // Build alarm start package
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
    // Build alarm stop package
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

    alarmStarted = false;
    isOtherAlert = false;
    testModeActive = false;
    skipStartup = false;
    alarmCount = 0;
    activatedAlarmSendCount = 0;
    deactivatedAlarmSendCount = 0;
    lastSendStatusMillis = 0;
    nextSendStatusMillis = 0;
    alarmRunningSinceMillis = 0;
}

void AlarmService::setup() {
    // Load alarm trigger value from EEPROM
    alarmTriggerValue = eepromController->getUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT);
    if (alarmTriggerValue == 0 || alarmTriggerValue == 65535) {
        alarmTriggerValue = TGS_DEFAULT_ALERT_VALUE;
    }

    // Set initial LED state
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
}

void AlarmService::tick(uint32_t currentMillis) {
    // Skip during startup delay
    if (!skipStartup && currentMillis < TGS_STARTUP_DELAY_MS) {
        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_YELLOW);
        return;
    }

    // Check TGS sensor value
    currentAlarmValue = tgsController->getValue();

    // Check for alarm condition
    if (currentAlarmValue >= alarmTriggerValue) {
        alarmCount++;
        if (alarmCount >= TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM && !alarmStarted) {
            startAlarm();
        }
    } else {
        if (alarmCount > 0) {
            alarmCount--;
        }
        if (alarmStarted && alarmCount == 0) {
            // Check minimum active period
            if (currentMillis - alarmRunningSinceMillis > ALARM_MIN_ACTIVE_PERIOD_MS) {
                stopAlarm();
            }
        }
    }

    // Check test button
    if (buttonController->getClickedState(BTN_TEST)) {
        if (!alarmStarted) {
            startAlarm();
        } else {
            stopAlarm();
        }
    }

    // Update LED based on alarm state
    if (alarmStarted) {
        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_RED);
    } else if (isOtherAlert) {
        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_YELLOW);
    } else {
        ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
    }
}

void AlarmService::startAlarm() {
    alarmStarted = true;
    alarmRunningSinceMillis = millis();
    alarmCount = TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM;
    activatedAlarmSendCount = 0;

    // Activate outputs
    buzzerController->start();
    outputController->set(RELAY_AC, true);

    // Send alarm start message
    communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);

    debug->println(F("[ALARM] Started"));
}

void AlarmService::stopAlarm() {
    alarmStarted = false;
    alarmCount = 0;
    deactivatedAlarmSendCount = 0;

    // Deactivate outputs
    buzzerController->stop();
    outputController->set(RELAY_AC, false);

    // Send alarm stop message
    communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);

    debug->println(F("[ALARM] Stopped"));
}

void AlarmService::sendPeriodicStatus(uint32_t currentMillis) {
    if (alarmStarted && activatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        if (currentMillis >= nextSendStatusMillis) {
            communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
            activatedAlarmSendCount++;
            generateNextSendStatusMillis();
        }
    } else if (!alarmStarted && deactivatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        if (currentMillis >= nextSendStatusMillis) {
            communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
            deactivatedAlarmSendCount++;
            generateNextSendStatusMillis();
        }
    }
}

void AlarmService::generateNextSendStatusMillis() {
    nextSendStatusMillis = millis() + random(ALARM_STATUS_MESSAGE_RESEND_MS_MIN, ALARM_STATUS_MESSAGE_RESEND_MS_MAX);
}

void AlarmService::setSendBuffer(uint8_t *sendBufferPtr, uint8_t *sendBufferLenPtr) {
    this->sendBuffer = sendBufferPtr;
    this->sendBufferLen = sendBufferLenPtr;
}

void AlarmService::setAlarmTriggerValue(uint16_t alarmTrigger) {
    alarmTriggerValue = alarmTrigger;
    eepromController->setUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT, alarmTriggerValue);
}

uint16_t AlarmService::getAlarmTriggerValue() {
    return alarmTriggerValue;
}

void AlarmService::setIsOtherAlert(bool isOtherAlertParam) {
    isOtherAlert = isOtherAlertParam;
}

void AlarmService::setSkipStartup(bool state) {
    skipStartup = state;
}

void AlarmService::checkOtherAlert() {
    // Check for other alert conditions
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
    buzzerController->start();
    outputController->set(RELAY_AC, true);
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_RED);
}

void ServiceMode::test1Stop() {
    testRunning = false;
    buzzerController->stop();
    outputController->set(RELAY_AC, false);
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
}

void ServiceMode::test2Start() {
    // Test 2 - Yellow LED
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_YELLOW);
}

void ServiceMode::test3Start() {
    // Test 3 - Green LED
    ((DetectorLEDController*)ledController)->setSemaphore(LED_SEMAPHORE_GREEN);
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
            switch (commandId) {
                case COMMAND_ID_GET_DEVICE_TYPE:
                    responseBuffer[0] = DEVICE_TYPE;
                    responseBuffer[1] = APP_VERSION;
                    sendResponseOK(commandId);
                    break;
                case COMMAND_ID_GET_ALL_VALUES:
                    dataTemp.address = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
                    dataTemp.channel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
                    dataTemp.receiverAddress = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
                    dataTemp.deviceId = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);
                    dataTemp.alarmTrigger = alarmService->getAlarmTriggerValue();
                    dataTemp.debugEnabled = serialDebug->getDebugEnabled();
                    sendResponseOK(commandId);
                    break;
                case COMMAND_ID_SET_ALARM_TRIGGER:
                    if (msgLen >= 4) {
                        convertBytesToInt.byte[0] = receiveBuffer[2];
                        convertBytesToInt.byte[1] = receiveBuffer[3];
                        alarmService->setAlarmTriggerValue(convertBytesToInt.intVal);
                        sendResponseOK(commandId);
                    } else {
                        sendResponseFail(commandId);
                    }
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

uint8_t ProgrammingService::calculateCRC(const uint8_t *buff, uint8_t len) {
    crc = 0;
    for (i = 0; i < len; i++) {
        crc ^= buff[i];
    }
    return crc;
}

void ProgrammingService::sendResponse() {
    Serial.write(responseBuffer, responseBuffer[1]);
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

Startup::Startup(EEPROMController *eepr) {
    this->eepr = eepr;
}

void Startup::doStartup() {
    // Perform startup procedures
}
