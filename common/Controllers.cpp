/**
 * @file Controllers.cpp
 * @brief Implementări pentru controllerele comune
 *
 * Implementări pentru: CC1101Controller, ButtonController,
 * LEDController, OutputController, EEPROMController
 */

#include "Controllers.h"

// ============================================================================
// CC1101 Helper Macros
// ============================================================================

#define cc1101_Select() digitalWrite(SS, LOW)
#define cc1101_Deselect() digitalWrite(SS, HIGH)
#define wait_Miso() while (digitalRead(MISO) > 0)
#define getGDO2state() digitalRead(CC1101_GDO2)
#define wait_GDO2_high() while (!getGDO2state())
#define wait_GDO2_low() while (getGDO2state())

// ============================================================================
// Button Controller Implementation
// ============================================================================

ButtonController::ButtonController() {
    // Initializarea se face în subclase specifice fiecărui modul
}

void ButtonController::configureButton(uint8_t pin, uint8_t id) {
    btnStates[id].pin = pin;
    btnStates[id].lastValue = false;
    btnStates[id].timestamp = 0;
    btnStates[id].clicked = false;
    btnStates[id].stateChanged = false;
}

void ButtonController::setup() {
    for (i = 0; i < BTN_COUNT; i++) {
        pinMode(btnStates[i].pin, INPUT_PULLUP);
    }
}

void ButtonController::tick(uint32_t currentMillis) {
    for (i = 0; i < BTN_COUNT; i++) {
        currentState = !digitalRead(btnStates[i].pin);
        if (btnStates[i].lastValue != currentState) {
            if (currentState) {
                btnStates[i].lastValue = currentState;
                btnStates[i].timestamp = currentMillis;
            } else {
                if (currentMillis - btnStates[i].timestamp >= BTN_DEBOUNCE_PERIOD) {
                    btnStates[i].clicked = true;
                    btnStates[i].stateChanged = true;
                }
                btnStates[i].lastValue = currentState;
                btnStates[i].timestamp = 0;
            }
        }
    }
}

bool ButtonController::hasAnyStateChanged() {
    for (i = 0; i < BTN_COUNT; i++) {
        if (btnStates[i].stateChanged) return true;
    }
    return false;
}

bool ButtonController::hasStateChanged(uint8_t button) {
    return btnStates[button].stateChanged;
}

bool* ButtonController::getAllClickedStates() {
    for (i = 0; i < BTN_COUNT; i++) {
        clickedStates[i] = btnStates[i].clicked;
        btnStates[i].clicked = false;
        btnStates[i].stateChanged = false;
    }
    return clickedStates;
}

bool ButtonController::getClickedState(uint8_t button) {
    currentState = btnStates[button].clicked;
    btnStates[button].clicked = false;
    btnStates[button].stateChanged = false;
    return currentState;
}

// ============================================================================
// CC1101 Controller Implementation
// ============================================================================

CC1101Controller::CC1101Controller(EEPROMController *eepromController) {
    this->eepromController = eepromController;
    this->spiSettings = new SPISettings(26000000, MSBFIRST, SPI_MODE0);

    channel = CC1101_DEFVAL_CHANNR;
    syncWord[0] = CC1101_DEFVAL_SYNC1;
    syncWord[1] = CC1101_DEFVAL_SYNC0;
    devAddress = CC1101_DEFVAL_ADDR;
}

void CC1101Controller::wakeUp(void) {
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    cc1101_Deselect();
    SPI.endTransaction();
}

void CC1101Controller::writeReg(uint8_t regAddr, uint8_t value) {
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    SPI.transfer(regAddr);
    SPI.transfer(value);
    cc1101_Deselect();
    SPI.endTransaction();
}

void CC1101Controller::writeBurstReg(uint8_t regAddr, uint8_t *buffer, uint8_t len) {
    addr = regAddr | WRITE_BURST;
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    SPI.transfer(addr);
    for (i = 0; i < len; i++) {
        SPI.transfer(buffer[i]);
    }
    cc1101_Deselect();
    SPI.endTransaction();
}

void CC1101Controller::cmdStrobe(uint8_t cmd) {
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    SPI.transfer(cmd);
    cc1101_Deselect();
    SPI.endTransaction();
}

uint8_t CC1101Controller::readReg(uint8_t regAddr, uint8_t regType) {
    addr = regAddr | regType;
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    SPI.transfer(addr);
    val = SPI.transfer(0x00);
    cc1101_Deselect();
    SPI.endTransaction();

    return val;
}

void CC1101Controller::readBurstReg(uint8_t *buffer, uint8_t regAddr, uint8_t len) {
    addr = regAddr | READ_BURST;
    SPI.beginTransaction(*spiSettings);
    cc1101_Select();
    wait_Miso();
    SPI.transfer(addr);
    for (i = 0; i < len; i++) {
        buffer[i] = SPI.transfer(0x00);
    }
    cc1101_Deselect();
    SPI.endTransaction();
}

void CC1101Controller::reset(void) {
    SPI.beginTransaction(*spiSettings);
    cc1101_Deselect();
    delayMicroseconds(5);
    cc1101_Select();
    delayMicroseconds(10);
    cc1101_Deselect();
    delayMicroseconds(41);
    cc1101_Select();

    wait_Miso();
    SPI.transfer(CC1101_SRES);
    wait_Miso();

    cc1101_Deselect();
    SPI.endTransaction();

    setCCregs();
}

void CC1101Controller::setCCregs(void) {
    writeReg(CC1101_IOCFG2, CC1101_DEFVAL_IOCFG2);
    writeReg(CC1101_IOCFG1, CC1101_DEFVAL_IOCFG1);
    writeReg(CC1101_IOCFG0, CC1101_DEFVAL_IOCFG0);
    writeReg(CC1101_FIFOTHR, CC1101_DEFVAL_FIFOTHR);
    writeReg(CC1101_PKTLEN, CC1101_DEFVAL_PKTLEN);
    writeReg(CC1101_PKTCTRL1, CC1101_DEFVAL_PKTCTRL1);
    writeReg(CC1101_PKTCTRL0, CC1101_DEFVAL_PKTCTRL0);

    setSyncWord(syncWord);
    setDevAddress(devAddress);
    setChannel(channel);

    writeReg(CC1101_FSCTRL1, CC1101_DEFVAL_FSCTRL1);
    writeReg(CC1101_FSCTRL0, CC1101_DEFVAL_FSCTRL0);
    writeReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2_868);
    writeReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1_868);
    writeReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0_868);

    writeReg(CC1101_MDMCFG4, CC1101_DEFVAL_MDMCFG4);
    writeReg(CC1101_MDMCFG3, CC1101_DEFVAL_MDMCFG3);
    writeReg(CC1101_MDMCFG2, CC1101_DEFVAL_MDMCFG2);
    writeReg(CC1101_MDMCFG1, CC1101_DEFVAL_MDMCFG1);
    writeReg(CC1101_MDMCFG0, CC1101_DEFVAL_MDMCFG0);
    writeReg(CC1101_DEVIATN, CC1101_DEFVAL_DEVIATN);
    writeReg(CC1101_MCSM2, CC1101_DEFVAL_MCSM2);
    writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1);
    writeReg(CC1101_MCSM0, CC1101_DEFVAL_MCSM0);
    writeReg(CC1101_FOCCFG, CC1101_DEFVAL_FOCCFG);
    writeReg(CC1101_BSCFG, CC1101_DEFVAL_BSCFG);
    writeReg(CC1101_AGCCTRL2, CC1101_DEFVAL_AGCCTRL2);
    writeReg(CC1101_AGCCTRL1, CC1101_DEFVAL_AGCCTRL1);
    writeReg(CC1101_AGCCTRL0, CC1101_DEFVAL_AGCCTRL0);
    writeReg(CC1101_WOREVT1, CC1101_DEFVAL_WOREVT1);
    writeReg(CC1101_WOREVT0, CC1101_DEFVAL_WOREVT0);
    writeReg(CC1101_WORCTRL, CC1101_DEFVAL_WORCTRL);
    writeReg(CC1101_FREND1, CC1101_DEFVAL_FREND1);
    writeReg(CC1101_FREND0, CC1101_DEFVAL_FREND0);
    writeReg(CC1101_FSCAL3, CC1101_DEFVAL_FSCAL3);
    writeReg(CC1101_FSCAL2, CC1101_DEFVAL_FSCAL2);
    writeReg(CC1101_FSCAL1, CC1101_DEFVAL_FSCAL1);
    writeReg(CC1101_FSCAL0, CC1101_DEFVAL_FSCAL0);
    writeReg(CC1101_RCCTRL1, CC1101_DEFVAL_RCCTRL1);
    writeReg(CC1101_RCCTRL0, CC1101_DEFVAL_RCCTRL0);
    writeReg(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
    writeReg(CC1101_PTEST, CC1101_DEFVAL_PTEST);
    writeReg(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
    writeReg(CC1101_TEST2, CC1101_DEFVAL_TEST2);
    writeReg(CC1101_TEST1, CC1101_DEFVAL_TEST1);
    writeReg(CC1101_TEST0, CC1101_DEFVAL_TEST0);

    CCPACKET packet;
    packet.length = 0;
    sendData(packet);
}

void CC1101Controller::init() {
    SPI.begin();
    pinMode(CC1101_GDO2, INPUT);

    setValuesFromEeprom();

    reset();

    setTxPowerAmp(PA_LongDistance);

    setRxState();
}

void CC1101Controller::setSyncWord(uint8_t syncH, uint8_t syncL) {
    writeReg(CC1101_SYNC1, syncH);
    writeReg(CC1101_SYNC0, syncL);
    syncWord[0] = syncH;
    syncWord[1] = syncL;
}

void CC1101Controller::setSyncWord(uint8_t *sync) {
    CC1101Controller::setSyncWord(sync[0], sync[1]);
}

void CC1101Controller::setDevAddress(uint8_t addr) {
    writeReg(CC1101_ADDR, addr);
    devAddress = addr;
}

void CC1101Controller::setChannel(uint8_t chnl) {
    writeReg(CC1101_CHANNR, chnl);
    channel = chnl;
}

void CC1101Controller::setPowerDownState() {
    cmdStrobe(CC1101_SIDLE);
    cmdStrobe(CC1101_SPWD);
}

bool CC1101Controller::sendData(CCPACKET packet) {
    res = false;

    tries = 0;

    while (tries++ < 1000 && ((marcState = readStatusReg(CC1101_MARCSTATE)) & 0x1F) != 0x0D) {
        if (marcState == 0x11)
            flushRxFifo();
    }
    if (tries >= 1000) {
        return false;
    }

    delayMicroseconds(500);

    if (packet.length > 0) {
        writeReg(CC1101_TXFIFO, packet.length);
        writeBurstReg(CC1101_TXFIFO, packet.data, packet.length);
        setTxState();
    }

    marcState = readStatusReg(CC1101_MARCSTATE) & 0x1F;
    if ((marcState != 0x13) && (marcState != 0x14) && (marcState != 0x15)) {
        setIdleState();
        flushTxFifo();
        setRxState();
        return false;
    }

    wait_GDO2_high();
    wait_GDO2_low();

    if ((readStatusReg(CC1101_TXBYTES) & 0x7F) == 0)
        res = true;

    setIdleState();
    flushTxFifo();

    setRxState();

    return res;
}

uint8_t CC1101Controller::receiveData(CCPACKET *packet) {
    rxBytes = readStatusReg(CC1101_RXBYTES);

    if (rxBytes & 0x7F && !(rxBytes & 0x80)) {
        packet->length = readConfigReg(CC1101_RXFIFO);

        if (packet->length > CC1101_MAX_BUFFER_SIZE)
            packet->length = 0;
        else {
            readBurstReg(packet->data, CC1101_RXFIFO, packet->length);
            packet->rssi = readConfigReg(CC1101_RXFIFO);
            val = readConfigReg(CC1101_RXFIFO);
            packet->lqi = val & 0x7F;
            packet->crc_ok = bitRead(val, 7);
        }
    } else
        packet->length = 0;

    setIdleState();
    flushRxFifo();

    setRxState();

    return packet->length;
}

void CC1101Controller::setRxState(void) {
    cmdStrobe(CC1101_SRX);
    rfState = RFSTATE_RX;
}

void CC1101Controller::setTxState(void) {
    cmdStrobe(CC1101_STX);
    rfState = RFSTATE_TX;
}

void CC1101Controller::setValuesFromEeprom(void) {
    setChannel(eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
}

void CC1101Controller::setTxPowerAmp(uint8_t paLevel) {
    writeReg(CC1101_PATABLE, paLevel);
}

int CC1101Controller::computeRssi(uint8_t rssi_dec) {
    rssi_offset = 74;
    if (rssi_dec >= 128) {
        return ((int)(rssi_dec - 256) / 2) - rssi_offset;
    } else {
        return (rssi_dec / 2) - rssi_offset;
    }
}

int CC1101Controller::computeLqi(uint8_t raw) {
    return 0x3F - raw;
}

void CC1101Controller::gotoIdleState() {
    setIdleState();
}

// ============================================================================
// EEPROM Controller Implementation
// ============================================================================

EEPROMController::EEPROMController() {
    eepr.setup(kbits_4, 1, EEPROM_PAGE_SIZE, 80);
}

void EEPROMController::setup() {
    i2cState = eepr.begin();
}

boolean EEPROMController::getI2CStatus() {
    return i2cState == 0;
}

void EEPROMController::wipeEEPROM() {
    for (i = EEPROM_WIPE_START_INDEX; i < EEPROM_WIPE_END_INDEX; i++) {
        eepr.write(i, 0);
    }
}

void EEPROMController::setSerialNumber(uint32_t serialNumber) {
    convertBytesToULong.longVal = serialNumber;

    j = 0;
    for (i = EEPROM_SERIAL_NUMBER_ADDRESS_START; i <= EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        eepr.write(i, convertBytesToULong.byte[j]);
        j++;
    }
}

uint32_t EEPROMController::getSerialNumber() {
    j = 0;
    for (i = EEPROM_SERIAL_NUMBER_ADDRESS_START; i <= EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        convertBytesToULong.byte[j] = eepr.read(i);
        j++;
    }

    return convertBytesToULong.longVal;
}

void EEPROMController::setStartupFlag(boolean state) {
    if (state) {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 4);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1, 2);
    } else {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 0);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1, 0);
    }
}

boolean EEPROMController::getStartupFlag() {
    if (eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT) == 4 && eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT + 1) == 2) {
        return true;
    } else {
        return false;
    }
}

void EEPROMController::setByteValue(uint32_t addr, uint8_t val) {
    eepr.write(addr, val);
}

uint8_t EEPROMController::getByteValue(uint32_t addr) {
    return eepr.read(addr);
}

void EEPROMController::setUIntValue(uint32_t addr, uint16_t val) {
    convertBytesToUInt.intVal = val;
    eepr.write(addr, convertBytesToUInt.byte[0]);
    eepr.write(addr + 1, convertBytesToUInt.byte[1]);
}

uint16_t EEPROMController::getUIntValue(uint32_t addr) {
    convertBytesToUInt.byte[0] = eepr.read(addr);
    convertBytesToUInt.byte[1] = eepr.read(addr + 1);
    return convertBytesToUInt.intVal;
}

// ============================================================================
// LED Controller Implementation
// ============================================================================

LEDController::LEDController() {
    currentColor.valueRed = 0;
    currentColor.animateRed = 0;

    predefinedColors[LED_COLOR_OFF].valueRed = 0;
    predefinedColors[LED_COLOR_OFF].animateRed = 0;

    predefinedColors[LED_COLOR_RED].valueRed = 255;
    predefinedColors[LED_COLOR_RED].animateRed = 1;

    pulseTimer = 0;
    _canWriteRed = false;
}

void LEDController::setup() {
    pinMode(HW_STATUS_LED_PIN, OUTPUT);
    set(LED_COLOR_RED, LED_ANIM_SLOW_FADE);
}

void LEDController::assignColor(ledColor *copyTo, ledColor *copyFrom) {
    copyTo->valueRed = copyFrom->valueRed;
    copyTo->animateRed = copyFrom->animateRed;
}

boolean LEDController::isDifferentColor(ledColor cmp1, ledColor cmp2) {
    if (cmp1.valueRed != cmp2.valueRed) return true;
    return false;
}

void LEDController::tick(uint32_t currentMillis) {
    if (currentAnimation == 0) {
        if (isDifferentColor(currentColor, currentSetColor)) {
            assignColor(&currentColor, &currentSetColor);
            _canWriteRed = true;
        } else {
            _canWriteRed = false;
        }
    } else {
        if (currentAnimation == 1) {
            incrementor = 1;
        } else if (currentAnimation == 2) {
            incrementor = 2;
        } else if (currentAnimation == 3) {
            incrementor = 4;
        } else if (currentAnimation == 4) {
            if (pulseTimer == 235) {
                incrementor = 255;
            }
            if (pulseTimer == 250) {
                incrementor = 0;
                pulseTimer = 0;
            }
            pulseTimer++;
        }

        if (incrementor >= 1 && incrementor <= 4) {
            if (currentSetColor.animateRed == 1) {
                if (currentColor.animateRed == 0) {
                    currentColor.animateRed = incrementor;
                    currentColor.valueRed = 0;
                }
                currentColor.valueRed += currentColor.animateRed;
                if (currentColor.valueRed >= currentSetColor.valueRed || currentColor.valueRed <= 0) {
                    currentColor.animateRed *= (-1);
                    currentColor.valueRed += currentColor.animateRed;
                }
                _canWriteRed = true;
            } else {
                if (currentColor.valueRed != 0) {
                    currentColor.valueRed = 0;
                    currentColor.animateRed = 0;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            }
        } else {
            if (currentSetColor.animateRed == 1) {
                if (currentColor.valueRed != incrementor) {
                    currentColor.valueRed = incrementor;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            } else {
                if (currentColor.valueRed != 0) {
                    currentColor.valueRed = 0;
                    currentColor.animateRed = 0;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            }
        }
    }

    if (_canWriteRed) analogWrite(HW_STATUS_LED_PIN, currentColor.valueRed);
}

void LEDController::setColor(uint8_t predefinedColor) {
    if (isDifferentColor(currentSetColor, predefinedColors[predefinedColor])) {
        resetCurrentColor();
        assignColor(&currentSetColor, &predefinedColors[predefinedColor]);
        pulseTimer = 0;
    }
}

void LEDController::setAnimation(uint8_t animation) {
    if (currentAnimation != animation) {
        resetCurrentColor();
        currentAnimation = animation;
        pulseTimer = 0;
    }
}

void LEDController::set(uint8_t predefinedColor, uint8_t animation) {
    if (isDifferentColor(currentSetColor, predefinedColors[predefinedColor]) || currentAnimation != animation) {
        resetCurrentColor();
        assignColor(&currentSetColor, &predefinedColors[predefinedColor]);
        currentAnimation = animation;
        pulseTimer = 0;
    }
}

void LEDController::resetCurrentColor() {
    currentColor.animateRed = 0;
}

// ============================================================================
// Output Controller Implementation
// ============================================================================

OutputController::OutputController() {
    for (i = 0; i < OUTPUTS_COUNT; i++) {
        outputState[i] = false;
        outputTimer[i] = 0;
        outputPulseCount[i] = 0;
    }
}

void OutputController::setup() {
    for (i = 0; i < OUTPUTS_COUNT; i++) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], LOW);
    }
}

void OutputController::tick(uint32_t currentMillis) {
    for (i = 0; i < OUTPUTS_COUNT; i++) {
        if (outputTimer[i] > 0 && currentMillis >= outputTimer[i]) {
            if (outputPulseCount[i] > 0) {
                outputPulseCount[i]--;
                outputState[i] = !outputState[i];
                digitalWrite(outputPins[i], outputState[i]);
                outputTimer[i] = currentMillis + OUTPUTS_DEFAULT_PULSE_DELAY;
            } else {
                outputState[i] = false;
                digitalWrite(outputPins[i], LOW);
                outputTimer[i] = 0;
            }
        }
    }
}

boolean OutputController::get(int16_t outputId) {
    return outputState[outputId];
}

void OutputController::set(int16_t outputId, boolean state) {
    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    outputTimer[outputId] = 0;
    outputPulseCount[outputId] = 0;
}

void OutputController::set(int16_t outputId, boolean state, uint32_t timeout) {
    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    outputTimer[outputId] = millis() + timeout;
    outputPulseCount[outputId] = 0;
}

void OutputController::setPulsed(int16_t outputId, boolean state, uint8_t pulses) {
    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    outputTimer[outputId] = millis() + OUTPUTS_DEFAULT_PULSE_DELAY;
    outputPulseCount[outputId] = pulses * 2;
}
