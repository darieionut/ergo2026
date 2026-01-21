/**
 * @file Config.h
 * @brief Configurații specifice pentru Floor Module
 *
 * Conține: Main, Hardware, EEPROM, SRAM și Menu configs
 */

#ifndef FLOOR_MODULE_CONFIG_H
#define FLOOR_MODULE_CONFIG_H

#include "../common/Config.h"

// ============================================================================
// Main Configuration
// ============================================================================

#define APP_VERSION 40
#define DEVICE_TYPE 2

#define WATCHDOG_ENABLED true
#define WATCHDOG_RECEIVE_RESET_MS 300000

#define SERIAL_BAUDRATE 19200

#define DETECTOR_OFFLINE_MS 420000

#define DEBUG_ENABLED false

// ============================================================================
// Hardware Configuration - Pin Definitions
// ============================================================================

#define HW_STATUS_LED_PIN 6

#define HW_OUTPUT_RELAY_AC_PIN 9
#define HW_OUTPUT_RELAY_DC_PIN 7

#define HW_INPUT_BTN_BACK_PIN 5
#define HW_INPUT_BTN_LEFT_PIN 4
#define HW_INPUT_BTN_RIGHT_PIN 17
#define HW_INPUT_BTN_OK_PIN 16

#define HW_SPI_CS_SRAM_PIN 15

#define HW_IO_NC_PIN 14

#define HW_RXD0_PIN 0

// ============================================================================
// Button Configuration
// ============================================================================

#define BTN_COUNT 5
#define BTN_DEBOUNCE_PERIOD 40

#define BTN_BACK 0
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_OK 3

// ============================================================================
// Output Configuration
// ============================================================================

#define OUTPUTS_COUNT 2

#define RELAY_AC 0
#define RELAY_DC 1

// ============================================================================
// LED Configuration
// ============================================================================

#define LED_COLOR_NONE 0

#define LED_STATUS_COUNT 5

#define LED_STATUS_NONE 0
#define LED_STATUS_OK 1
#define LED_STATUS_WARNING 2
#define LED_STATUS_ERROR 3
#define LED_STATUS_COMM_ERROR 4

// ============================================================================
// EEPROM Configuration
// ============================================================================

#define EEPROM_PAGE_SIZE 16

#define EEPROM_WIPE 0
#define EEPROM_WIPE_START_INDEX 0
#define EEPROM_WIPE_END_INDEX 210

#define EEPROM_SERIAL_NUMBER_ADDRESS_START 0
#define EEPROM_SERIAL_NUMBER_ADDRESS_END 3

#define EEPROM_STARTUP_FLAG_ADDRESS_INT 10

#define EEPROM_CONFIG_OPERATION_MODE_BYTE 50

#define EEPROM_CC1101_DEVICE_ADDRESS_INT 100
#define EEPROM_CC1101_DEVICE_CHANNEL_BYTE 102
#define EEPROM_CC1101_FORWARD_ADDRESS_INT 103
#define EEPROM_CC1101_FORWARD_CHANNEL_BYTE 105

#define EEPROM_ALARMS_MEMORY_INDEX_BYTE 109
#define EEPROM_ALARMS_MEMORY_COUNT_BYTE 110
#define EEPROM_ALARMS_MEMORY_START 111
#define EEPROM_ALARMS_MEMORY_END 210
#define EEPROM_ALARMS_MEMORY_SEGMENT_SIZE 5

#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE 250
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START 251
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_END 291
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE 4

// ============================================================================
// SRAM Configuration
// ============================================================================

#define SRAM_ALARMS_MEMORY_START 0
#define SRAM_ALARMS_MEMORY_END 6050
#define SRAM_ALARMS_MEMORY_SEGMENT_SIZE 5

#define SRAM_DETECTORS_MEMORY_START 6050
#define SRAM_DETECTORS_MEMORY_END 54050
#define SRAM_DETECTORS_MEMORY_SEGMENT_SIZE 10

#define SRAM_OFFLINE_DETECTORS_MEMORY_START 54050
#define SRAM_OFFLINE_DETECTORS_MEMORY_END 63650
#define SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE 2

// ============================================================================
// Menu Configuration
// ============================================================================

#define LCD_ADDRESS 0x27
#define LCD_NUMBER_OF_CHARS 16
#define LCD_NUMBER_OF_ROWS 2

#define MENU_NUMBER_OF_PAGES 10
#define MENU_TIMEOUT_MS 120000
#define MENU_BACKLIGHT_OFF_MS 60000
#define MENU_VALUE_REFRESH_MS 3000

// Menu text definitions
#define MENU_TEXT_ENERGO_INSTAL F("ERGO")
#define MENU_TEXT_EVERYTHING_OK F("Everything OK")
#define MENU_TEXT_DETECTOR_ALARMS F("Detector alarms:")
#define MENU_TEXT_NO_ALARMS F("No alarms")
#define MENU_TEXT_CONNECTED F("Connected")
#define MENU_TEXT_ALERT_OR_ALERTS F(" alert(s)")
#define MENU_TEXT_ALARM_OR_ALARMS F(" alarm(s)")
#define MENU_TEXT_ALARM F("Alarm ")
#define MENU_TEXT_DETECTOR F("Detector ")
#define MENU_TEXT_DETECTORS F("detectors: ")
#define MENU_TEXT_ONLINE F("Online")
#define MENU_TEXT_OFFLINE F("Offline")
#define MENU_TEXT_CLEAR_ALARMS F("Clear alarms?")
#define MENU_TEXT_NO_YES F("No           Yes")
#define MENU_TEXT_CLEAR_DETECTORS F("Clear detectors?")
#define MENU_TEXT_OPERATION_MODE F("Operation mode")
#define MENU_TEXT_MASTER F("Master")
#define MENU_TEXT_SLAVE F("Slave")
#define MENU_TEXT_SHIFTER F("Shifter")
#define MENU_TEXT_DEVICE_ADDRESS F("Device address:")
#define MENU_TEXT_DEVICE_CHANNEL F("Device channel:")
#define MENU_TEXT_FORWARD_ADDRESS F("Forward address:")
#define MENU_TEXT_FORWARD_CHANNEL F("Forward channel:")
#define MENU_TEXT_SEPARATOR_DASH F("/")
#define MENU_TEXT_SEPARATOR_TWO_POINTS F(":")
#define MENU_TEXT_SEPARATOR_SPACE F(" ")
#define MENU_TEXT_LQI_OPEN F(" (")
#define MENU_TEXT_LQI_CLOSED F("%)")
#define MENU_TEXT_HRS F(" hrs")
#define MENU_TEXT_MIN F(" min")
#define MENU_TEXT_SEC F(" sec")
#define MENU_TEXT_NA F("N/A")
#define MENU_TEXT_SEPARATOR_DASH_SPACED F(" | ")
#define MENU_TEXT_ON F("ON")
#define MENU_TEXT_OFF F("OFF")
#define MENU_TEXT_NOT_IMPLEMENTED F("Not implemented!")
#define MENU_TEXT_FIRMWARE F("Firmware")
#define MENU_TEXT_VERSION F(" v")
#define MENU_TEXT_SEPARATOR_DOT F(".")

// Menu page indices
#define PAGE_INDEX_HOME 0
#define PAGE_INDEX_ALARMS 1
#define PAGE_INDEX_DETECTORS 2
#define PAGE_INDEX_DETECTORS_OFFLINE 3
#define PAGE_INDEX_LIST_ALARMS 4
#define PAGE_INDEX_LIST_DETECTORS 5
#define PAGE_INDEX_LIST_DETECTORS_OFFLINE 6
#define PAGE_INDEX_CLEAR_ALARMS 7
#define PAGE_INDEX_CLEAR_DETECTORS 8
#define PAGE_INDEX_FIRMWARE 9

// ============================================================================
// Operation Mode Configuration
// ============================================================================

#define OPERATION_MODE_MASTER 1
#define OPERATION_MODE_SLAVE 2

#define MESSAGE_IS_FORWARDED 100

// ============================================================================
// Serial Programming Configuration
// ============================================================================

#define SERIAL_RECEIVE_BUFF_SIZE 50
#define SERIAL_WORKING_BUFF_SIZE 25
#define SERIAL_SEND_BUFF_SIZE 25
#define RECV_MSG_MIN_LEN 2
#define RECV_MSG_MAX_LEN 19

// Programming command IDs
#define COMMAND_ID_SET_DEVICE_ADDRESS 1
#define COMMAND_ID_SET_DEVICE_CHANNEL 2
#define COMMAND_ID_SET_FORWARD_ADDRESS 3
#define COMMAND_ID_SET_FORWARD_CHANNEL 4
#define COMMAND_ID_SET_OPERATION_MODE 5
#define COMMAND_ID_SET_DEBUG_MODE 6
#define COMMAND_ID_GET_DEVICE_TYPE 10
#define COMMAND_ID_GET_ALL_VALUES 20
#define COMMAND_ID_SET_ALL_VALUES 21
#define COMMAND_ID_STREAM_ALL_MEMORY 30
#define COMMAND_ID_CLEAR_ALARMS 31
#define COMMAND_ID_CLEAR_DETECTORS 32
#define COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL 40
#define COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS 41
#define COMMAND_ID_TEST_MODE 101

#endif // FLOOR_MODULE_CONFIG_H
