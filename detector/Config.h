/**
 * @file Config.h
 * @brief Configurații specifice pentru Detector
 *
 * Conține: Main, Hardware, EEPROM, Alert, Buzzer și TGS configs
 */

#ifndef DETECTOR_CONFIG_H
#define DETECTOR_CONFIG_H

#include "../common/Config.h"

// ============================================================================
// Main Configuration
// ============================================================================

#define APP_VERSION 25
#define DEVICE_TYPE 1

#define WATCHDOG_ENABLED true

#define SERIAL_BAUDRATE 19200

#define DEFAULT_DEVICE_ID 65000
#define DEFAULT_RECEIVER_ADDRESS 1

#define DISABLE_CC1101_RECEIVE

#define DEBUG_ENABLED false

// ============================================================================
// Hardware Configuration - Pin Definitions
// ============================================================================

#define HW_STATUS_LED_PIN 6

#define HW_OUTPUT_LED_RED_PIN 5
#define HW_OUTPUT_LED_YELLOW_PIN 4
#define HW_OUTPUT_LED_GREEN_PIN 9
#define HW_OUTPUT_BUZZER_PIN 7
#define HW_OUTPUT_RELAY_PIN 2

#define HW_INPUT_BTN_TEST_PIN 16

#define HW_TGS_SENSOR_PIN 17

#define HW_RXD0_PIN 0

// ============================================================================
// Button Configuration
// ============================================================================

#define BTN_COUNT 1
#define BTN_DEBOUNCE_PERIOD 40

#define BTN_TEST 0

// ============================================================================
// Output Configuration
// ============================================================================

#define OUTPUTS_COUNT 1

#define RELAY_AC 0

// ============================================================================
// LED Configuration
// ============================================================================

#define BLINK_INTERVAL_MS 300

#define LED_COLOR_NONE 0

#define LED_STATUS_COUNT 5

#define LED_STATUS_NONE 0
#define LED_STATUS_OK 1
#define LED_STATUS_WARNING 2
#define LED_STATUS_ERROR 3
#define LED_STATUS_COMM_ERROR 4

#define LED_SEMAPHORE_OFF 0
#define LED_SEMAPHORE_RED 1
#define LED_SEMAPHORE_YELLOW 2
#define LED_SEMAPHORE_GREEN 3

// ============================================================================
// EEPROM Configuration
// ============================================================================

#define EEPROM_PAGE_SIZE 16

#define EEPROM_WIPE 0
#define EEPROM_WIPE_START_INDEX 0
#define EEPROM_WIPE_END_INDEX 150

#define EEPROM_SERIAL_NUMBER_ADDRESS_START 0
#define EEPROM_SERIAL_NUMBER_ADDRESS_END 3

#define EEPROM_STARTUP_FLAG_ADDRESS_INT 10

#define EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT 50

#define EEPROM_CONFIG_DEVICE_ID_INT 52

#define EEPROM_CC1101_DEVICE_ADDRESS_INT 100
#define EEPROM_CC1101_DEVICE_CHANNEL_BYTE 102
#define EEPROM_CC1101_RECEIVER_ADDRESS_INT 103

// ============================================================================
// Alert Configuration
// ============================================================================

#define ALARM_STATUS_MESSAGE_RESEND_MS_MIN 30000
#define ALARM_STATUS_MESSAGE_RESEND_MS_MAX 120000

#define ALARM_MESSAGE_MAX_RESEND_COUNT 5

#define ALARM_MIN_ACTIVE_PERIOD_MS 180000

// ============================================================================
// Buzzer Configuration
// ============================================================================

#define BEEPING_INTERVAL_MS 400

// ============================================================================
// TGS Sensor Configuration
// ============================================================================

#define TGS_STARTUP_DELAY_MS 60000

#define TGS_DEFAULT_ALERT_VALUE 406

#define TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM 6

// ============================================================================
// Serial Programming Configuration
// ============================================================================

#define SERIAL_RECEIVE_BUFF_SIZE 50
#define SERIAL_WORKING_BUFF_SIZE 25
#define SERIAL_SEND_BUFF_SIZE 25
#define RECV_MSG_MIN_LEN 2
#define RECV_MSG_MAX_LEN 21

// Programming command IDs
#define COMMAND_ID_SET_DEVICE_ADDRESS 1
#define COMMAND_ID_SET_DEVICE_CHANNEL 2
#define COMMAND_ID_SET_RECEIVER_ADDRESS 3
#define COMMAND_ID_SET_DEVICE_ID 4
#define COMMAND_ID_SET_ALARM_TRIGGER 5
#define COMMAND_ID_SET_DEBUG_MODE 6
#define COMMAND_ID_GET_DEVICE_TYPE 10
#define COMMAND_ID_GET_ALL_VALUES 20
#define COMMAND_ID_SET_ALL_VALUES 21
#define COMMAND_ID_TEST_MODE 101

#endif // DETECTOR_CONFIG_H
