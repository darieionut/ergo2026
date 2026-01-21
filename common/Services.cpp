/**
 * @file Services.cpp
 * @brief Implementări pentru serviciile comune
 *
 * Implementări pentru: SerialDebug și CommunicationService de bază
 */

#include "Services.h"

// ============================================================================
// Serial Debug Implementation
// ============================================================================

SerialDebug::SerialDebug() {
    debugEnabled = DEBUG_ENABLED;
}

SerialDebug::SerialDebug(boolean enableDebug) {
    debugEnabled = enableDebug;
}

void SerialDebug::setup() {
    Serial.begin(SERIAL_BAUDRATE);
}

void SerialDebug::setDebugEnabled(boolean state) {
    debugEnabled = state;
}

boolean SerialDebug::getDebugEnabled() {
    return debugEnabled;
}

void SerialDebug::printDebugStart() {
    if (debugEnabled) {
        Serial.println(F("[DEBUG] Debug mode enabled"));
    }
}

size_t SerialDebug::print(const __FlashStringHelper *str) {
    if (debugEnabled) return Serial.print(str);
    return 0;
}

size_t SerialDebug::print(const String &s) {
    if (debugEnabled) return Serial.print(s);
    return 0;
}

size_t SerialDebug::print(const char c[]) {
    if (debugEnabled) return Serial.print(c);
    return 0;
}

size_t SerialDebug::print(int8_t n) {
    if (debugEnabled) return Serial.print(n);
    return 0;
}

size_t SerialDebug::print(uint8_t n, int16_t base) {
    if (debugEnabled) return Serial.print(n, base);
    return 0;
}

size_t SerialDebug::print(int16_t n, int16_t base) {
    if (debugEnabled) return Serial.print(n, base);
    return 0;
}

size_t SerialDebug::print(uint16_t n, int16_t base) {
    if (debugEnabled) return Serial.print(n, base);
    return 0;
}

size_t SerialDebug::print(int32_t n, int16_t base) {
    if (debugEnabled) return Serial.print(n, base);
    return 0;
}

size_t SerialDebug::print(uint32_t n, int16_t base) {
    if (debugEnabled) return Serial.print(n, base);
    return 0;
}

size_t SerialDebug::print(float n, int16_t digits) {
    if (debugEnabled) return Serial.print(n, digits);
    return 0;
}

size_t SerialDebug::print(const Printable &x) {
    if (debugEnabled) return Serial.print(x);
    return 0;
}

size_t SerialDebug::println(const __FlashStringHelper *str) {
    if (debugEnabled) return Serial.println(str);
    return 0;
}

size_t SerialDebug::println(const String &s) {
    if (debugEnabled) return Serial.println(s);
    return 0;
}

size_t SerialDebug::println(const char c[]) {
    if (debugEnabled) return Serial.println(c);
    return 0;
}

size_t SerialDebug::println(int8_t n) {
    if (debugEnabled) return Serial.println(n);
    return 0;
}

size_t SerialDebug::println(uint8_t n, int16_t base) {
    if (debugEnabled) return Serial.println(n, base);
    return 0;
}

size_t SerialDebug::println(int16_t n, int16_t base) {
    if (debugEnabled) return Serial.println(n, base);
    return 0;
}

size_t SerialDebug::println(uint16_t n, int16_t base) {
    if (debugEnabled) return Serial.println(n, base);
    return 0;
}

size_t SerialDebug::println(int32_t n, int16_t base) {
    if (debugEnabled) return Serial.println(n, base);
    return 0;
}

size_t SerialDebug::println(uint32_t n, int16_t base) {
    if (debugEnabled) return Serial.println(n, base);
    return 0;
}

size_t SerialDebug::println(float n, int16_t digits) {
    if (debugEnabled) return Serial.println(n, digits);
    return 0;
}

size_t SerialDebug::println(const Printable &x) {
    if (debugEnabled) return Serial.println(x);
    return 0;
}

size_t SerialDebug::println(void) {
    if (debugEnabled) return Serial.println();
    return 0;
}

// ============================================================================
// Communication Service Implementation
// ============================================================================

CommunicationService::CommunicationService(CC1101Controller *radio, SerialDebug *debug,
    EEPROMController *eepromController, LEDController *ledController) {
    this->radio = radio;
    this->debug = debug;
    this->eepromController = eepromController;
    this->ledController = ledController;

    radioCommunicationStatus = 0;
    indexRssiLqi = 0;
    previousIndexRssiLqi = 0;

    for (i = 0; i < RSSI_LQI_SAMPLES; i++) {
        lastRssi[i] = 0;
        lastLqi[i] = 0;
    }
}

void CommunicationService::setup() {
    radio->init();
    reloadEepromValues();
}

void CommunicationService::reloadEepromValues() {
    // Override in subclasses for specific EEPROM values
}

void CommunicationService::send(const uint8_t *buff, uint8_t *len) {
    if (*len > 0) {
        packet.length = *len;
        for (i = 0; i < *len; i++) {
            packet.data[i] = buff[i];
        }
        radio->sendData(packet);
        *len = 0;
    }
}

void CommunicationService::receive(uint8_t *buff, uint8_t *len) {
    radio->receiveData(&packet);
    if (packet.length > 0 && packet.crc_ok) {
        *len = packet.length;
        for (i = 0; i < packet.length; i++) {
            buff[i] = packet.data[i];
        }

        // Update RSSI/LQI tracking
        lastRssi[indexRssiLqi] = radio->computeRssi(packet.rssi);
        lastLqi[indexRssiLqi] = radio->computeLqi(packet.lqi);
        previousIndexRssiLqi = indexRssiLqi;
        indexRssiLqi++;
        if (indexRssiLqi >= RSSI_LQI_SAMPLES) {
            indexRssiLqi = 0;
        }
    } else {
        *len = 0;
    }
}

void CommunicationService::testConnection() {
    // Implement connection test
}

int16_t CommunicationService::getComputedLinkQuality(int16_t lqi, int16_t rssi) {
    // Compute link quality based on LQI and RSSI
    int16_t quality = 0;

    // RSSI contribution (typically -100 to 0 dBm, map to 0-50)
    int16_t rssiQuality = (rssi + 100) / 2;
    if (rssiQuality < 0) rssiQuality = 0;
    if (rssiQuality > 50) rssiQuality = 50;

    // LQI contribution (0-63, map to 0-50)
    int16_t lqiQuality = (lqi * 50) / 63;
    if (lqiQuality < 0) lqiQuality = 0;
    if (lqiQuality > 50) lqiQuality = 50;

    quality = rssiQuality + lqiQuality;
    if (quality > 100) quality = 100;

    return quality;
}

int16_t CommunicationService::getAvarageLinkQuality() {
    avarageRssi = 0;
    avarageLqi = 0;

    for (i = 0; i < RSSI_LQI_SAMPLES; i++) {
        avarageRssi += lastRssi[i];
        avarageLqi += lastLqi[i];
    }

    avarageRssi /= RSSI_LQI_SAMPLES;
    avarageLqi /= RSSI_LQI_SAMPLES;

    return getComputedLinkQuality(avarageLqi, avarageRssi);
}

int16_t CommunicationService::getLastMessageComputedLinkQuality() {
    return getComputedLinkQuality(lastLqi[previousIndexRssiLqi], lastRssi[previousIndexRssiLqi]);
}
