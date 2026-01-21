/**
 * @file Controllers.cpp
 * @brief Implementări pentru controllerele Floor Module
 *
 * Implementări pentru: MicrochipSRAM și SRAMController
 */

#include "Controllers.h"

// ============================================================================
// MicrochipSRAM Implementation
// ============================================================================

MicrochipSRAM::MicrochipSRAM(const uint8_t SSPin, const uint32_t sramType) {
    _SSPin = SSPin;
    SRAMBytes = sramType;
    spiSettings = new SPISettings(26000000, MSBFIRST, SPI_MODE0);

    pinMode(_SSPin, OUTPUT);
    digitalWrite(_SSPin, HIGH);

    SPI.beginTransaction(*spiSettings);
    digitalWrite(_SSPin, LOW);
    SPI.transfer(SRAM_WRITE_MODE_REG);
    SPI.transfer(SRAM_SEQ_MODE);
    digitalWrite(_SSPin, HIGH);
    SPI.endTransaction();
}

MicrochipSRAM::~MicrochipSRAM() {
    delete spiSettings;
}

void MicrochipSRAM::clearMemory(const uint8_t clearValue) {
    SPI.beginTransaction(*spiSettings);
    digitalWrite(_SSPin, LOW);
    SPI.transfer(SRAM_WRITE_CODE);
    if (SRAMBytes == SRAM_1024) {
        SPI.transfer(0);
    }
    SPI.transfer(0);
    SPI.transfer(0);
    for (uint32_t i = 0; i < SRAMBytes; i++) {
        SPI.transfer(clearValue);
    }
    digitalWrite(_SSPin, HIGH);
    SPI.endTransaction();
}

// ============================================================================
// SRAMController Implementation
// ============================================================================

SRAMController::SRAMController(EEPROMController *eepr) {
    this->eepr = eepr;
    numberOfAlarms = 0;
    numberOfDetectors = 0;
    numberOfOfflineDetectors = 0;
}

void SRAMController::setup() {
    sram = new MicrochipSRAM(HW_SPI_CS_SRAM_PIN, SRAM_64);
}

uint32_t SRAMController::getSRAMType() {
    return sram->SRAMBytes;
}

uint16_t SRAMController::getNumberOfAlarms() {
    return numberOfAlarms;
}

uint16_t SRAMController::getNumberOfDetectors() {
    return numberOfDetectors;
}

uint16_t SRAMController::getNumberOfOfflineDetectors() {
    return numberOfOfflineDetectors;
}

// ============================================================================
// Alarm Management
// ============================================================================

int16_t SRAMController::addAlarm(AlarmEntry *alarm, boolean isUpdateOnly) {
    return addAlarm(alarm, isUpdateOnly, false);
}

int16_t SRAMController::addAlarm(AlarmEntry *alarm, boolean isUpdateOnly, boolean isStartup) {
    // Check if alarm already exists
    index = findAlarm(alarm->address);

    if (index >= 0) {
        // Update existing alarm
        sram->get(SRAM_ALARMS_MEMORY_START + (index * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + 4, lastState);
        isStateChange = (lastState != alarm->state);

        if (isStateChange || isStartup) {
            convertBytesToUInt.intVal = alarm->address;
            alarmAuxBuffer[0] = convertBytesToUInt.byte[0];
            alarmAuxBuffer[1] = convertBytesToUInt.byte[1];
            convertBytesToUInt.intVal = alarm->id;
            alarmAuxBuffer[2] = convertBytesToUInt.byte[0];
            alarmAuxBuffer[3] = convertBytesToUInt.byte[1];
            alarmAuxBuffer[4] = alarm->state;

            for (i = 0; i < 5; i++) {
                sram->put(SRAM_ALARMS_MEMORY_START + (index * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i, alarmAuxBuffer[i]);
            }
        }
        return index;
    }

    if (isUpdateOnly) {
        return -1;
    }

    // Add new alarm
    if (numberOfAlarms < (SRAM_ALARMS_MEMORY_END - SRAM_ALARMS_MEMORY_START) / SRAM_ALARMS_MEMORY_SEGMENT_SIZE) {
        convertBytesToUInt.intVal = alarm->address;
        alarmAuxBuffer[0] = convertBytesToUInt.byte[0];
        alarmAuxBuffer[1] = convertBytesToUInt.byte[1];
        convertBytesToUInt.intVal = alarm->id;
        alarmAuxBuffer[2] = convertBytesToUInt.byte[0];
        alarmAuxBuffer[3] = convertBytesToUInt.byte[1];
        alarmAuxBuffer[4] = alarm->state;

        for (i = 0; i < 5; i++) {
            sram->put(SRAM_ALARMS_MEMORY_START + (numberOfAlarms * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i, alarmAuxBuffer[i]);
        }

        numberOfAlarms++;
        return numberOfAlarms - 1;
    }

    return -1;
}

boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    if (elemIndex < numberOfAlarms) {
        for (i = 0; i < 5; i++) {
            sram->get(SRAM_ALARMS_MEMORY_START + (elemIndex * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i, alarmAuxBuffer[i]);
        }

        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[1];
        alarm->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = alarmAuxBuffer[2];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[3];
        alarm->id = convertBytesToUInt.intVal;
        alarm->state = alarmAuxBuffer[4];

        return true;
    }
    return false;
}

int16_t SRAMController::findAlarm(uint16_t address) {
    convertBytesToUInt.intVal = address;
    comparatorAuxBuffer[0] = convertBytesToUInt.byte[0];
    comparatorAuxBuffer[1] = convertBytesToUInt.byte[1];

    for (j = 0; j < numberOfAlarms; j++) {
        sram->get(SRAM_ALARMS_MEMORY_START + (j * SRAM_ALARMS_MEMORY_SEGMENT_SIZE), alarmAuxBuffer[0]);
        sram->get(SRAM_ALARMS_MEMORY_START + (j * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + 1, alarmAuxBuffer[1]);

        if (alarmAuxBuffer[0] == comparatorAuxBuffer[0] && alarmAuxBuffer[1] == comparatorAuxBuffer[1]) {
            return j;
        }
    }

    return -1;
}

boolean SRAMController::deleteAlarm(uint16_t elemIndex) {
    if (elemIndex < numberOfAlarms) {
        // Move last element to deleted position
        if (elemIndex + 1 != numberOfAlarms) {
            for (i = 0; i < 5; i++) {
                sram->get(SRAM_ALARMS_MEMORY_START + ((numberOfAlarms - 1) * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i, alarmAuxBuffer[i]);
                sram->put(SRAM_ALARMS_MEMORY_START + (elemIndex * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i, alarmAuxBuffer[i]);
            }
        }
        numberOfAlarms--;
        return true;
    }
    return false;
}

boolean SRAMController::clearAlarms() {
    numberOfAlarms = 0;
    return true;
}

// ============================================================================
// Detector Management
// ============================================================================

int16_t SRAMController::addDetector(DetectorEntry *detector) {
    // Check if detector already exists
    index = findDetector(detector->address);

    if (index >= 0) {
        // Update existing detector
        convertBytesToUInt.intVal = detector->address;
        detectorAuxBuffer[0] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[1] = convertBytesToUInt.byte[1];
        convertBytesToUInt.intVal = detector->id;
        detectorAuxBuffer[2] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[3] = convertBytesToUInt.byte[1];
        detectorAuxBuffer[4] = detector->linkQuality;
        detectorAuxBuffer[5] = detector->status;
        convertBytesToULong.longVal = detector->lastUpdateMs;
        detectorAuxBuffer[6] = convertBytesToULong.byte[0];
        detectorAuxBuffer[7] = convertBytesToULong.byte[1];
        detectorAuxBuffer[8] = convertBytesToULong.byte[2];
        detectorAuxBuffer[9] = convertBytesToULong.byte[3];

        for (i = 0; i < 10; i++) {
            sram->put(SRAM_DETECTORS_MEMORY_START + (index * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + i, detectorAuxBuffer[i]);
        }

        return index;
    }

    // Add new detector
    if (numberOfDetectors < (SRAM_DETECTORS_MEMORY_END - SRAM_DETECTORS_MEMORY_START) / SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) {
        convertBytesToUInt.intVal = detector->address;
        detectorAuxBuffer[0] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[1] = convertBytesToUInt.byte[1];
        convertBytesToUInt.intVal = detector->id;
        detectorAuxBuffer[2] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[3] = convertBytesToUInt.byte[1];
        detectorAuxBuffer[4] = detector->linkQuality;
        detectorAuxBuffer[5] = detector->status;
        convertBytesToULong.longVal = detector->lastUpdateMs;
        detectorAuxBuffer[6] = convertBytesToULong.byte[0];
        detectorAuxBuffer[7] = convertBytesToULong.byte[1];
        detectorAuxBuffer[8] = convertBytesToULong.byte[2];
        detectorAuxBuffer[9] = convertBytesToULong.byte[3];

        for (i = 0; i < 10; i++) {
            sram->put(SRAM_DETECTORS_MEMORY_START + (numberOfDetectors * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + i, detectorAuxBuffer[i]);
        }

        numberOfDetectors++;
        return numberOfDetectors - 1;
    }

    return -1;
}

boolean SRAMController::getDetector(DetectorEntry *detector, uint16_t elemIndex) {
    if (elemIndex < numberOfDetectors) {
        for (i = 0; i < 10; i++) {
            sram->get(SRAM_DETECTORS_MEMORY_START + (elemIndex * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + i, detectorAuxBuffer[i]);
        }

        convertBytesToUInt.byte[0] = detectorAuxBuffer[0];
        convertBytesToUInt.byte[1] = detectorAuxBuffer[1];
        detector->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = detectorAuxBuffer[2];
        convertBytesToUInt.byte[1] = detectorAuxBuffer[3];
        detector->id = convertBytesToUInt.intVal;
        detector->linkQuality = detectorAuxBuffer[4];
        detector->status = detectorAuxBuffer[5];
        convertBytesToULong.byte[0] = detectorAuxBuffer[6];
        convertBytesToULong.byte[1] = detectorAuxBuffer[7];
        convertBytesToULong.byte[2] = detectorAuxBuffer[8];
        convertBytesToULong.byte[3] = detectorAuxBuffer[9];
        detector->lastUpdateMs = convertBytesToULong.longVal;

        return true;
    }
    return false;
}

int16_t SRAMController::findDetector(uint16_t address) {
    convertBytesToUInt.intVal = address;
    comparatorAuxBuffer[0] = convertBytesToUInt.byte[0];
    comparatorAuxBuffer[1] = convertBytesToUInt.byte[1];

    for (j = 0; j < numberOfDetectors; j++) {
        sram->get(SRAM_DETECTORS_MEMORY_START + (j * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE), detectorAuxBuffer[0]);
        sram->get(SRAM_DETECTORS_MEMORY_START + (j * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + 1, detectorAuxBuffer[1]);

        if (detectorAuxBuffer[0] == comparatorAuxBuffer[0] && detectorAuxBuffer[1] == comparatorAuxBuffer[1]) {
            return j;
        }
    }

    return -1;
}

boolean SRAMController::deleteDetector(uint16_t elemIndex) {
    if (elemIndex < numberOfDetectors) {
        if (elemIndex + 1 != numberOfDetectors) {
            for (i = 0; i < 10; i++) {
                sram->get(SRAM_DETECTORS_MEMORY_START + ((numberOfDetectors - 1) * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + i, detectorAuxBuffer[i]);
                sram->put(SRAM_DETECTORS_MEMORY_START + (elemIndex * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) + i, detectorAuxBuffer[i]);
            }
        }
        numberOfDetectors--;
        return true;
    }
    return false;
}

boolean SRAMController::clearDetectors() {
    numberOfDetectors = 0;
    return true;
}

// ============================================================================
// Offline Detector Management
// ============================================================================

int16_t SRAMController::addOfflineDetector(OfflineDetectorEntry *offlineDetector) {
    // Check if already exists
    if (findOfflineDetector(offlineDetector->detectorEntryIndex) >= 0) {
        return -1;
    }

    if (numberOfOfflineDetectors < (SRAM_OFFLINE_DETECTORS_MEMORY_END - SRAM_OFFLINE_DETECTORS_MEMORY_START) / SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) {
        convertBytesToUInt.intVal = offlineDetector->detectorEntryIndex;
        offlineDetectorAuxBuffer[0] = convertBytesToUInt.byte[0];
        offlineDetectorAuxBuffer[1] = convertBytesToUInt.byte[1];

        for (i = 0; i < 2; i++) {
            sram->put(SRAM_OFFLINE_DETECTORS_MEMORY_START + (numberOfOfflineDetectors * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) + i, offlineDetectorAuxBuffer[i]);
        }

        numberOfOfflineDetectors++;
        return numberOfOfflineDetectors - 1;
    }

    return -1;
}

boolean SRAMController::getOfflineDetector(OfflineDetectorEntry *offlineDetector, uint16_t elemIndex) {
    if (elemIndex < numberOfOfflineDetectors) {
        for (i = 0; i < 2; i++) {
            sram->get(SRAM_OFFLINE_DETECTORS_MEMORY_START + (elemIndex * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) + i, offlineDetectorAuxBuffer[i]);
        }

        convertBytesToUInt.byte[0] = offlineDetectorAuxBuffer[0];
        convertBytesToUInt.byte[1] = offlineDetectorAuxBuffer[1];
        offlineDetector->detectorEntryIndex = convertBytesToUInt.intVal;

        return true;
    }
    return false;
}

int16_t SRAMController::findOfflineDetector(uint16_t detectorEntryIndex) {
    convertBytesToUInt.intVal = detectorEntryIndex;
    comparatorAuxBuffer[0] = convertBytesToUInt.byte[0];
    comparatorAuxBuffer[1] = convertBytesToUInt.byte[1];

    for (j = 0; j < numberOfOfflineDetectors; j++) {
        sram->get(SRAM_OFFLINE_DETECTORS_MEMORY_START + (j * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE), offlineDetectorAuxBuffer[0]);
        sram->get(SRAM_OFFLINE_DETECTORS_MEMORY_START + (j * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) + 1, offlineDetectorAuxBuffer[1]);

        if (offlineDetectorAuxBuffer[0] == comparatorAuxBuffer[0] && offlineDetectorAuxBuffer[1] == comparatorAuxBuffer[1]) {
            return j;
        }
    }

    return -1;
}

boolean SRAMController::deleteOfflineDetector(uint16_t detectorEntryIndex) {
    index = findOfflineDetector(detectorEntryIndex);
    if (index >= 0) {
        if ((uint16_t)(index + 1) != numberOfOfflineDetectors) {
            for (i = 0; i < 2; i++) {
                sram->get(SRAM_OFFLINE_DETECTORS_MEMORY_START + ((numberOfOfflineDetectors - 1) * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) + i, offlineDetectorAuxBuffer[i]);
                sram->put(SRAM_OFFLINE_DETECTORS_MEMORY_START + (index * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) + i, offlineDetectorAuxBuffer[i]);
            }
        }
        numberOfOfflineDetectors--;
        return true;
    }
    return false;
}

boolean SRAMController::clearOfflineDetectors() {
    numberOfOfflineDetectors = 0;
    return true;
}

// ============================================================================
// FloorModuleEEPROMController Implementation
// ============================================================================

FloorModuleEEPROMController::FloorModuleEEPROMController() : EEPROMController() {
    numberOfAddressVerificationIntervals = 254;
    numberOfAlarms = 0;
    index = EEPROM_ALARMS_MEMORY_START;
}

void FloorModuleEEPROMController::get(uint32_t addr, uint8_t *val, uint8_t len) {
    for (aux = 0; aux < len; aux++) {
        val[aux] = eepr.read(addr + aux);
    }
}

void FloorModuleEEPROMController::put(uint32_t addr, uint8_t *val, uint8_t len) {
    for (aux = 0; aux < len; aux++) {
        eepr.write(addr + aux, val[aux]);
    }
}

int16_t FloorModuleEEPROMController::calculateIndex(uint8_t elemIndex) {
    aux = index + elemIndex * EEPROM_ALARMS_MEMORY_SEGMENT_SIZE;
    if (aux > EEPROM_ALARMS_MEMORY_END - EEPROM_ALARMS_MEMORY_SEGMENT_SIZE + 1) {
        aux = EEPROM_ALARMS_MEMORY_START + (aux - EEPROM_ALARMS_MEMORY_END) - 1;
    }
    return aux;
}

void FloorModuleEEPROMController::saveAlarmDetails() {
    eepr.write(EEPROM_ALARMS_MEMORY_INDEX_BYTE, index);
    eepr.write(EEPROM_ALARMS_MEMORY_COUNT_BYTE, numberOfAlarms);
}

void FloorModuleEEPROMController::loadAlarmDetails() {
    index = eepr.read(EEPROM_ALARMS_MEMORY_INDEX_BYTE);
    numberOfAlarms = eepr.read(EEPROM_ALARMS_MEMORY_COUNT_BYTE);
}

uint8_t FloorModuleEEPROMController::getNumberOfAlarms() {
    return numberOfAlarms;
}

void FloorModuleEEPROMController::addAlarm(AlarmEntry* alarm) {
    convertBytesToUInt.intVal = alarm->address;
    alarmAuxBuffer[0] = convertBytesToUInt.byte[0];
    alarmAuxBuffer[1] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = alarm->id;
    alarmAuxBuffer[2] = convertBytesToUInt.byte[0];
    alarmAuxBuffer[3] = convertBytesToUInt.byte[1];
    alarmAuxBuffer[4] = alarm->state;

    put(calculateIndex(numberOfAlarms), alarmAuxBuffer, 5);

    if (numberOfAlarms < 20) numberOfAlarms++;
    else {
        index += EEPROM_ALARMS_MEMORY_SEGMENT_SIZE;
        if (index >= EEPROM_ALARMS_MEMORY_END) {
            index = EEPROM_ALARMS_MEMORY_START;
        }
    }
    saveAlarmDetails();
}

boolean FloorModuleEEPROMController::getAlarm(AlarmEntry* alarm, uint8_t elemIndex) {
    if (elemIndex >= 0 && elemIndex < numberOfAlarms) {
        get(calculateIndex(elemIndex), alarmAuxBuffer, 5);
        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[1];
        alarm->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = alarmAuxBuffer[2];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[3];
        alarm->id = convertBytesToUInt.intVal;
        alarm->state = alarmAuxBuffer[4];
        return true;
    }
    return false;
}

boolean FloorModuleEEPROMController::deleteAlarm(uint16_t address) {
    for (j = 0; j < numberOfAlarms; j++) {
        if (getUIntValue(calculateIndex(j)) == address) {
            if (j + 1 != numberOfAlarms) {
                get(calculateIndex(numberOfAlarms - 1), alarmAuxBuffer, 5);
                put(calculateIndex(j), alarmAuxBuffer, 5);
            }
            numberOfAlarms--;
            saveAlarmDetails();
            return true;
        }
    }
    return false;
}

boolean FloorModuleEEPROMController::clearAlarms() {
    numberOfAlarms = 0;
    index = EEPROM_ALARMS_MEMORY_START;
    saveAlarmDetails();
    return true;
}

uint8_t FloorModuleEEPROMController::getNumberOfAddressVerificationIntervals() {
    if (numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    return numberOfAddressVerificationIntervals;
}

boolean FloorModuleEEPROMController::addAddressVerificationInterval(AddressVerificationInterval *interval) {
    if (numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    aux = (numberOfAddressVerificationIntervals * EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE) + EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START;
    if (aux < EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_END) {
        convertBytesToUInt.intVal = interval->min;
        eepr.write(aux, convertBytesToUInt.byte[0]);
        eepr.write(aux + 1, convertBytesToUInt.byte[1]);
        convertBytesToUInt.intVal = interval->max;
        eepr.write(aux + 2, convertBytesToUInt.byte[0]);
        eepr.write(aux + 3, convertBytesToUInt.byte[1]);

        numberOfAddressVerificationIntervals++;
        eepr.write(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, numberOfAddressVerificationIntervals);

        return true;
    }
    return false;
}

boolean FloorModuleEEPROMController::getAddressVerificationInterval(uint8_t idx, AddressVerificationInterval *interval) {
    if (numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    if (idx < numberOfAddressVerificationIntervals) {
        aux = (idx * EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE) + EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START;
        convertBytesToUInt.byte[0] = eepr.read(aux);
        convertBytesToUInt.byte[1] = eepr.read(aux + 1);
        interval->min = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = eepr.read(aux + 2);
        convertBytesToUInt.byte[1] = eepr.read(aux + 3);
        interval->max = convertBytesToUInt.intVal;
        return true;
    }
    return false;
}

boolean FloorModuleEEPROMController::clearAddressVerificationIntervals() {
    if (eepr.write(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, 0) == 0) {
        numberOfAddressVerificationIntervals = 254;
        return true;
    }
    return false;
}

// ============================================================================
// FloorModuleButtonController Implementation
// ============================================================================

FloorModuleButtonController::FloorModuleButtonController() : ButtonController() {
    btnStates[0].pin = HW_INPUT_BTN_BACK_PIN;
    btnStates[0].lastValue = false;
    btnStates[0].timestamp = 0;
    btnStates[0].clicked = false;
    btnStates[0].stateChanged = false;

    btnStates[1].pin = HW_INPUT_BTN_LEFT_PIN;
    btnStates[1].lastValue = false;
    btnStates[1].timestamp = 0;
    btnStates[1].clicked = false;
    btnStates[1].stateChanged = false;

    btnStates[2].pin = HW_INPUT_BTN_RIGHT_PIN;
    btnStates[2].lastValue = false;
    btnStates[2].timestamp = 0;
    btnStates[2].clicked = false;
    btnStates[2].stateChanged = false;

    btnStates[3].pin = HW_INPUT_BTN_OK_PIN;
    btnStates[3].lastValue = false;
    btnStates[3].timestamp = 0;
    btnStates[3].clicked = false;
    btnStates[3].stateChanged = false;
}

// ============================================================================
// FloorModuleOutputController Implementation
// ============================================================================

FloorModuleOutputController::FloorModuleOutputController() : OutputController() {
    outputPins[RELAY_AC] = HW_OUTPUT_RELAY_AC_PIN;
    outputPins[RELAY_DC] = HW_OUTPUT_RELAY_DC_PIN;
}

void FloorModuleOutputController::setup() {
    OutputController::setup();
}
