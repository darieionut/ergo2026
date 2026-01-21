/**
 * @file Controllers.h
 * @brief Declarații pentru toate controllerele comune
 *
 * Conține declarațiile pentru: CC1101Controller, ButtonController,
 * LEDController, OutputController, EEPROMController
 */

#ifndef COMMON_CONTROLLERS_H
#define COMMON_CONTROLLERS_H

#include <Arduino.h>
#include <SPI.h>
#include <extEEPROM.h>
#include "Config.h"

// ============================================================================
// Button Controller
// ============================================================================

struct btnState {
    uint8_t pin;
    boolean lastValue;
    uint32_t timestamp;
    boolean clicked;
    boolean stateChanged;
};

class ButtonController {
private:
    boolean clickedStates[BTN_COUNT];
    boolean currentState;
    btnState btnStates[BTN_COUNT];
    int16_t i;

    void configureButton(uint8_t pin, uint8_t id);

public:
    ButtonController();
    void setup();
    void tick(uint32_t currentMillis);
    boolean hasAnyStateChanged();
    boolean hasStateChanged(uint8_t button);
    boolean* getAllClickedStates();
    boolean getClickedState(uint8_t button);
};

// ============================================================================
// EEPROM Controller Base
// ============================================================================

class EEPROMController {
protected:
    extEEPROM eepr;
    uint8_t i2cState;
    ConvertBytesToLong convertBytesToLong;
    ConvertBytesToULong convertBytesToULong;
    ConvertBytesToInt convertBytesToInt;
    ConvertBytesToUInt convertBytesToUInt;

    int16_t i, j, aux;

public:
    EEPROMController();
    void setup();
    boolean getI2CStatus();

    void wipeEEPROM();

    void setSerialNumber(uint32_t serialNumber);
    uint32_t getSerialNumber();

    void setStartupFlag(boolean state);
    boolean getStartupFlag();

    void setByteValue(uint32_t addr, uint8_t val);
    uint8_t getByteValue(uint32_t addr);
    void setUIntValue(uint32_t addr, uint16_t val);
    uint16_t getUIntValue(uint32_t addr);
};

// ============================================================================
// CC1101 Controller
// ============================================================================

enum RFSTATE {
    RFSTATE_IDLE = 0,
    RFSTATE_RX,
    RFSTATE_TX
};

// Helper macros for CC1101
#define readConfigReg(regAddr)    readReg(regAddr, CC1101_CONFIG_REGISTER)
#define readStatusReg(regAddr)    readReg(regAddr, CC1101_STATUS_REGISTER)
#define setIdleState()            cmdStrobe(CC1101_SIDLE)
#define flushRxFifo()             cmdStrobe(CC1101_SFRX)
#define flushTxFifo()             cmdStrobe(CC1101_SFTX)
#define disableAddressCheck()     writeReg(CC1101_PKTCTRL1, 0x04)
#define enableAddressCheck()      writeReg(CC1101_PKTCTRL1, 0x06)
#define disableCCA()              writeReg(CC1101_MCSM1, 0)
#define enableCCA()               writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1)

#define PA_LowPower               0x60
#define PA_LongDistance           0xC0

class CC1101Controller {
private:
    EEPROMController *eepromController;
    SPISettings *spiSettings;

    uint8_t addr, i, val, marcState, rxBytes, rssi_dec, rssi_offset;
    bool res;
    int tries;

    void writeBurstReg(uint8_t regAddr, uint8_t* buffer, uint8_t len);
    void readBurstReg(uint8_t* buffer, uint8_t regAddr, uint8_t len);
    void setValuesFromEeprom(void);

public:
    uint8_t rfState;
    uint8_t channel;
    uint8_t syncWord[2];
    uint8_t devAddress;

    CC1101Controller(EEPROMController *eepromController);

    void cmdStrobe(uint8_t cmd);
    void wakeUp(void);
    uint8_t readReg(uint8_t regAddr, uint8_t regType);
    void writeReg(uint8_t regAddr, uint8_t value);
    void setCCregs(void);
    void reset(void);
    void init(void);
    void setSyncWord(uint8_t syncH, uint8_t syncL);
    void setSyncWord(uint8_t *sync);
    void setDevAddress(uint8_t addr);
    void setChannel(uint8_t chnl);
    void setPowerDownState();
    bool sendData(CCPACKET packet);
    uint8_t receiveData(CCPACKET *packet);
    void setRxState(void);
    void setTxState(void);
    void setTxPowerAmp(uint8_t paLevel);
    int computeRssi(uint8_t rssi_dec);
    int computeLqi(uint8_t raw);
    void gotoIdleState(void);
};

// ============================================================================
// LED Controller
// ============================================================================

struct ledColor {
    uint8_t valueRed;
    uint8_t animateRed;
};

class LEDController {
protected:
    uint8_t currentAnimation;
    int16_t incrementor, pulseTimer;
    void assignColor(ledColor *copyTo, ledColor *copyFrom);
    boolean isDifferentColor(ledColor cmp1, ledColor cmp2);
    boolean _canWriteRed;
    ledColor currentColor, currentSetColor, predefinedColors[2];

public:
    LEDController();
    virtual void setup();
    virtual void tick(uint32_t currentMillis);
    void setColor(uint8_t predefinedColor);
    void setAnimation(uint8_t animation);
    void set(uint8_t predefinedColor, uint8_t animation);
    void resetCurrentColor();
};

// ============================================================================
// Output Controller
// ============================================================================

class OutputController {
protected:
    boolean outputState[OUTPUTS_COUNT];
    uint32_t outputTimer[OUTPUTS_COUNT];
    uint8_t outputPulseCount[OUTPUTS_COUNT];
    uint8_t outputPins[OUTPUTS_COUNT];

    int16_t i;

public:
    OutputController();
    virtual void setup();
    void tick(uint32_t currentMillis);

    boolean get(int16_t outputId);
    void set(int16_t outputId, boolean state);
    void set(int16_t outputId, boolean state, uint32_t timeout);
    void setPulsed(int16_t outputId, boolean state, uint8_t pulses);
};

#endif // COMMON_CONTROLLERS_H
