/**
 * @file Controllers.h
 * @brief Controllere specifice pentru Floor Module
 *
 * Conține: MicrochipSRAM și SRAMController
 */

#ifndef FLOOR_MODULE_CONTROLLERS_H
#define FLOOR_MODULE_CONTROLLERS_H

#include <Arduino.h>
#include <SPI.h>
#include "../common/Config.h"
#include "../common/Controllers.h"
#include "Config.h"
#include "Models.h"

// ============================================================================
// Microchip SRAM Constants
// ============================================================================

const uint8_t SRAM_WRITE_MODE_REG = 0x01;
const uint8_t SRAM_READ_MODE_REG = 0x05;
const uint8_t SRAM_BYTE_MODE = B00000000;
const uint8_t SRAM_PAGE_MODE = B10000000;
const uint8_t SRAM_SEQ_MODE = B01000000;
const uint32_t SRAM_1024 = 131072;
const uint32_t SRAM_512 = 65536;
const uint32_t SRAM_256 = 32768;
const uint32_t SRAM_128 = 16384;
const uint32_t SRAM_64 = 8192;
const uint8_t SRAM_WRITE_CODE = 0x02;
const uint8_t SRAM_READ_CODE = 0x03;

// ============================================================================
// MicrochipSRAM Class - Driver pentru SRAM extern
// ============================================================================

class MicrochipSRAM {
private:
    uint8_t _SSPin = 0;
    SPISettings *spiSettings;

public:
    MicrochipSRAM(const uint8_t SSPin, const uint32_t sramType);
    ~MicrochipSRAM();

    void clearMemory(const uint8_t clearValue = 0);

    template <typename T> uint32_t &get(const uint32_t addr, T &value) {
        static uint32_t returnAddress;
        uint8_t *bytePtr = (uint8_t *)&value;
        returnAddress = (addr + sizeof(T)) % SRAMBytes;
        SPI.beginTransaction(*spiSettings);
        digitalWrite(_SSPin, LOW);
        SPI.transfer(SRAM_READ_CODE);
        if (SRAMBytes == SRAM_1024) {
            SPI.transfer((uint8_t)(addr >> 16) & 0xFF);
        }
        SPI.transfer((uint8_t)(addr >> 8) & 0xFF);
        SPI.transfer((uint8_t)addr);
        for (uint32_t i = 0; i < sizeof(T); i++) {
            *bytePtr++ = SPI.transfer(0x00);
        }
        digitalWrite(_SSPin, HIGH);
        SPI.endTransaction();
        return (returnAddress);
    }

    template <typename T> uint32_t &put(const uint32_t addr, const T &value) {
        static uint32_t returnAddress;
        const uint8_t *bytePtr = (const uint8_t *)&value;
        returnAddress = (addr + sizeof(T)) % SRAMBytes;
        SPI.beginTransaction(*spiSettings);
        digitalWrite(_SSPin, LOW);
        SPI.transfer(SRAM_WRITE_CODE);
        if (SRAMBytes == SRAM_1024) {
            SPI.transfer((uint8_t)(addr >> 16) & 0xFF);
        }
        SPI.transfer((uint8_t)(addr >> 8) & 0xFF);
        SPI.transfer((uint8_t)addr);
        for (uint32_t i = 0; i < sizeof(T); i++) {
            SPI.transfer(*bytePtr++);
        }
        digitalWrite(_SSPin, HIGH);
        SPI.endTransaction();
        return (returnAddress);
    }

    template <typename T> uint32_t &fillMemory(uint32_t addr, T &value) {
        while (addr < (SRAMBytes - sizeof(T)))
            addr = put(addr, value);
        return (addr);
    }

    uint32_t SRAMBytes = 0;
};

// ============================================================================
// SRAMController Class - Gestionare alarme și detectoare în SRAM
// ============================================================================

class SRAMController {
private:
    MicrochipSRAM *sram;
    EEPROMController *eepr;

    ConvertBytesToULong convertBytesToULong;
    ConvertBytesToInt convertBytesToInt;
    ConvertBytesToUInt convertBytesToUInt;

    boolean anyErrorsExist, foundElement, isStateChange, lastState;
    uint16_t i, j, index;
    uint16_t numberOfAlarms, numberOfDetectors, numberOfOfflineDetectors;

    uint8_t alarmAuxBuffer[5];
    uint8_t detectorAuxBuffer[10];
    uint8_t offlineDetectorAuxBuffer[2];
    uint8_t comparatorAuxBuffer[2];

public:
    SRAMController(EEPROMController *eepr);

    void setup();
    uint32_t getSRAMType();

    uint16_t getNumberOfAlarms();
    uint16_t getNumberOfDetectors();
    uint16_t getNumberOfOfflineDetectors();

    // Alarm management
    int16_t addAlarm(AlarmEntry *alarm, boolean isUpdateOnly);
    int16_t addAlarm(AlarmEntry *alarm, boolean isUpdateOnly, boolean isStartup);
    boolean getAlarm(AlarmEntry *alarm, uint16_t elemIndex);
    int16_t findAlarm(uint16_t address);
    boolean deleteAlarm(uint16_t elemIndex);
    boolean clearAlarms();

    // Detector management
    int16_t addDetector(DetectorEntry *detector);
    boolean getDetector(DetectorEntry *detector, uint16_t elemIndex);
    int16_t findDetector(uint16_t address);
    boolean deleteDetector(uint16_t elemIndex);
    boolean clearDetectors();

    // Offline detector management
    int16_t addOfflineDetector(OfflineDetectorEntry *offlineDetector);
    boolean getOfflineDetector(OfflineDetectorEntry *offlineDetector, uint16_t elemIndex);
    int16_t findOfflineDetector(uint16_t detectorEntryIndex);
    boolean deleteOfflineDetector(uint16_t detectorEntryIndex);
    boolean clearOfflineDetectors();
};

// ============================================================================
// Floor Module EEPROM Controller - Extended pentru alarme
// ============================================================================

class FloorModuleEEPROMController : public EEPROMController {
private:
    int16_t index;
    uint8_t numberOfAlarms, numberOfAddressVerificationIntervals;
    uint8_t alarmAuxBuffer[5];
    uint8_t comparatorAuxBuffer[2];

    void get(uint32_t addr, uint8_t *val, uint8_t len);
    void put(uint32_t addr, uint8_t *val, uint8_t len);
    int16_t calculateIndex(uint8_t elemIndex);

public:
    FloorModuleEEPROMController();

    void saveAlarmDetails();
    void loadAlarmDetails();
    uint8_t getNumberOfAlarms();
    void addAlarm(AlarmEntry* alarm);
    boolean getAlarm(AlarmEntry* alarm, uint8_t elemIndex);
    boolean deleteAlarm(uint16_t address);
    boolean clearAlarms();

    uint8_t getNumberOfAddressVerificationIntervals();
    boolean addAddressVerificationInterval(AddressVerificationInterval *interval);
    boolean getAddressVerificationInterval(uint8_t index, AddressVerificationInterval *interval);
    boolean clearAddressVerificationIntervals();
};

// ============================================================================
// Floor Module Button Controller
// ============================================================================

class FloorModuleButtonController : public ButtonController {
public:
    FloorModuleButtonController();
};

// ============================================================================
// Floor Module Output Controller
// ============================================================================

class FloorModuleOutputController : public OutputController {
public:
    FloorModuleOutputController();
    void setup() override;
};

#endif // FLOOR_MODULE_CONTROLLERS_H
