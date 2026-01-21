/**
 * @file Services.h
 * @brief Declarații pentru serviciile comune
 *
 * Conține: SerialDebug și CommunicationService de bază
 */

#ifndef COMMON_SERVICES_H
#define COMMON_SERVICES_H

#include <Arduino.h>
#include "Config.h"
#include "Controllers.h"

// ============================================================================
// Serial Debug Service
// ============================================================================

class SerialDebug {
private:
    boolean debugEnabled;

public:
    SerialDebug();
    SerialDebug(boolean enableDebug);
    void setup();
    void setDebugEnabled(boolean state);
    boolean getDebugEnabled();
    void printDebugStart();

    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(int8_t);
    size_t print(uint8_t, int16_t = DEC);
    size_t print(int16_t, int16_t = DEC);
    size_t print(uint16_t, int16_t = DEC);
    size_t print(int32_t, int16_t = DEC);
    size_t print(uint32_t, int16_t = DEC);
    size_t print(float, int16_t = 2);
    size_t print(const Printable&);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(int8_t);
    size_t println(uint8_t, int16_t = DEC);
    size_t println(int16_t, int16_t = DEC);
    size_t println(uint16_t, int16_t = DEC);
    size_t println(int32_t, int16_t = DEC);
    size_t println(uint32_t, int16_t = DEC);
    size_t println(float, int16_t = 2);
    size_t println(const Printable&);
    size_t println(void);
};

// ============================================================================
// Communication Service Base
// ============================================================================

#define RSSI_LQI_SAMPLES 20

// Buffer indices for communication protocol
#define BUFF_INDEX_RECEIVER_LO 0
#define BUFF_INDEX_RECEIVER_HI 1
#define BUFF_INDEX_ADDRESS_LO 2
#define BUFF_INDEX_ADDRESS_HI 3
#define BUFF_INDEX_ID_LO 4
#define BUFF_INDEX_ID_HI 5
#define BUFF_INDEX_COMMAND 6
#define BUFF_INDEX_IS_FORWARD 7

// Command IDs
#define COMMAND_ID_TEST_RSSI 1
#define COMMAND_ID_ALARM_START 10
#define COMMAND_ID_ALARM_STOP 11

class CommunicationService {
protected:
    CC1101Controller *radio;
    CCPACKET packet;
    SerialDebug *debug;
    EEPROMController *eepromController;
    LEDController *ledController;

    uint8_t radioCommunicationStatus;

    uint8_t syncWord[2] = {199, 10};
    int16_t lastRssi[RSSI_LQI_SAMPLES], lastLqi[RSSI_LQI_SAMPLES], avarageRssi, avarageLqi;
    uint8_t indexRssiLqi, previousIndexRssiLqi, lqiPercent, rssiPercent;

    int16_t i;

public:
    CommunicationService(CC1101Controller *radio, SerialDebug *debug, EEPROMController *eepromController, LEDController *ledController);
    virtual void setup();
    virtual void reloadEepromValues();

    void send(const uint8_t *buff, uint8_t *len);
    void receive(uint8_t *buff, uint8_t *len);

    void testConnection();
    int16_t getComputedLinkQuality(int16_t lqi, int16_t rssi);
    int16_t getAvarageLinkQuality();
    int16_t getLastMessageComputedLinkQuality();
};

#endif // COMMON_SERVICES_H
