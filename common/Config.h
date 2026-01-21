/**
 * @file Config.h
 * @brief Configurări comune pentru CC1101, Types și CCPACKET
 *
 * Acest fișier conține toate configurările partajate între
 * Floor Module și Detector.
 */

#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <Arduino.h>

// ============================================================================
// CC1101 Configuration
// ============================================================================

#define CC1101_Interrupt 1
#define CC1101_GDO2 3

#define CC1101_MAX_BUFFER_SIZE 30
#define CC1101_MAX_TX_BUFFER_SIZE 15
#define CC1101_MAX_RX_BUFFER_SIZE 100

// SPI Commands
#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0

#define CC1101_CONFIG_REGISTER   READ_SINGLE
#define CC1101_STATUS_REGISTER   READ_BURST

// CC1101 FIFO & PATABLE
#define CC1101_PATABLE           0x3E
#define CC1101_TXFIFO            0x3F
#define CC1101_RXFIFO            0x3F

// Strobe commands
#define CC1101_SRES              0x30
#define CC1101_SFSTXON           0x31
#define CC1101_SXOFF             0x32
#define CC1101_SCAL              0x33
#define CC1101_SRX               0x34
#define CC1101_STX               0x35
#define CC1101_SIDLE             0x36
#define CC1101_SWOR              0x38
#define CC1101_SPWD              0x39
#define CC1101_SFRX              0x3A
#define CC1101_SFTX              0x3B
#define CC1101_SWORRST           0x3C
#define CC1101_SNOP              0x3D

// Configuration registers
#define CC1101_IOCFG2            0x00
#define CC1101_IOCFG1            0x01
#define CC1101_IOCFG0            0x02
#define CC1101_FIFOTHR           0x03
#define CC1101_SYNC1             0x04
#define CC1101_SYNC0             0x05
#define CC1101_PKTLEN            0x06
#define CC1101_PKTCTRL1          0x07
#define CC1101_PKTCTRL0          0x08
#define CC1101_ADDR              0x09
#define CC1101_CHANNR            0x0A
#define CC1101_FSCTRL1           0x0B
#define CC1101_FSCTRL0           0x0C
#define CC1101_FREQ2             0x0D
#define CC1101_FREQ1             0x0E
#define CC1101_FREQ0             0x0F
#define CC1101_MDMCFG4           0x10
#define CC1101_MDMCFG3           0x11
#define CC1101_MDMCFG2           0x12
#define CC1101_MDMCFG1           0x13
#define CC1101_MDMCFG0           0x14
#define CC1101_DEVIATN           0x15
#define CC1101_MCSM2             0x16
#define CC1101_MCSM1             0x17
#define CC1101_MCSM0             0x18
#define CC1101_FOCCFG            0x19
#define CC1101_BSCFG             0x1A
#define CC1101_AGCCTRL2          0x1B
#define CC1101_AGCCTRL1          0x1C
#define CC1101_AGCCTRL0          0x1D
#define CC1101_WOREVT1           0x1E
#define CC1101_WOREVT0           0x1F
#define CC1101_WORCTRL           0x20
#define CC1101_FREND1            0x21
#define CC1101_FREND0            0x22
#define CC1101_FSCAL3            0x23
#define CC1101_FSCAL2            0x24
#define CC1101_FSCAL1            0x25
#define CC1101_FSCAL0            0x26
#define CC1101_RCCTRL1           0x27
#define CC1101_RCCTRL0           0x28
#define CC1101_FSTEST            0x29
#define CC1101_PTEST             0x2A
#define CC1101_AGCTEST           0x2B
#define CC1101_TEST2             0x2C
#define CC1101_TEST1             0x2D
#define CC1101_TEST0             0x2E

// Status registers
#define CC1101_PARTNUM           0x30
#define CC1101_VERSION           0x31
#define CC1101_FREQEST           0x32
#define CC1101_LQI               0x33
#define CC1101_RSSI              0x34
#define CC1101_MARCSTATE         0x35
#define CC1101_WORTIME1          0x36
#define CC1101_WORTIME0          0x37
#define CC1101_PKTSTATUS         0x38
#define CC1101_VCO_VC_DAC        0x39
#define CC1101_TXBYTES           0x3A
#define CC1101_RXBYTES           0x3B
#define CC1101_RCCTRL1_STATUS    0x3C
#define CC1101_RCCTRL0_STATUS    0x3D

// Default values for CC1101 registers
#define CC1101_DEFVAL_IOCFG2     0x06
#define CC1101_DEFVAL_IOCFG1     0x2E
#define CC1101_DEFVAL_IOCFG0     0x2E
#define CC1101_DEFVAL_FIFOTHR    0x07
#define CC1101_DEFVAL_SYNC1      0xB5
#define CC1101_DEFVAL_SYNC0      0x47
#define CC1101_DEFVAL_PKTLEN     0x3D
#define CC1101_DEFVAL_PKTCTRL1   0x06
#define CC1101_DEFVAL_PKTCTRL0   0x05
#define CC1101_DEFVAL_ADDR       0x00
#define CC1101_DEFVAL_CHANNR     0x00
#define CC1101_DEFVAL_FSCTRL1    0x08
#define CC1101_DEFVAL_FSCTRL0    0x00
#define CC1101_DEFVAL_FREQ2_868  0x21
#define CC1101_DEFVAL_FREQ1_868  0x62
#define CC1101_DEFVAL_FREQ0_868  0x76
#define CC1101_DEFVAL_MDMCFG4    0xCA
#define CC1101_DEFVAL_MDMCFG3    0x83
#define CC1101_DEFVAL_MDMCFG2    0x93
#define CC1101_DEFVAL_MDMCFG1    0x22
#define CC1101_DEFVAL_MDMCFG0    0xF8
#define CC1101_DEFVAL_DEVIATN    0x35
#define CC1101_DEFVAL_MCSM2      0x07
#define CC1101_DEFVAL_MCSM1      0x20
#define CC1101_DEFVAL_MCSM0      0x18
#define CC1101_DEFVAL_FOCCFG     0x16
#define CC1101_DEFVAL_BSCFG      0x6C
#define CC1101_DEFVAL_AGCCTRL2   0x43
#define CC1101_DEFVAL_AGCCTRL1   0x40
#define CC1101_DEFVAL_AGCCTRL0   0x91
#define CC1101_DEFVAL_WOREVT1    0x87
#define CC1101_DEFVAL_WOREVT0    0x6B
#define CC1101_DEFVAL_WORCTRL    0xFB
#define CC1101_DEFVAL_FREND1     0x56
#define CC1101_DEFVAL_FREND0     0x10
#define CC1101_DEFVAL_FSCAL3     0xE9
#define CC1101_DEFVAL_FSCAL2     0x2A
#define CC1101_DEFVAL_FSCAL1     0x00
#define CC1101_DEFVAL_FSCAL0     0x1F
#define CC1101_DEFVAL_RCCTRL1    0x41
#define CC1101_DEFVAL_RCCTRL0    0x00
#define CC1101_DEFVAL_FSTEST     0x59
#define CC1101_DEFVAL_PTEST      0x7F
#define CC1101_DEFVAL_AGCTEST    0x3F
#define CC1101_DEFVAL_TEST2      0x81
#define CC1101_DEFVAL_TEST1      0x35
#define CC1101_DEFVAL_TEST0      0x09

// Aliases for default values
#define CCDEF_CHANNR  CC1101_DEFVAL_CHANNR
#define CCDEF_SYNC0   CC1101_DEFVAL_SYNC0
#define CCDEF_SYNC1   CC1101_DEFVAL_SYNC1
#define CCDEF_ADDR    CC1101_DEFVAL_ADDR

// ============================================================================
// Types Configuration - Union types for byte conversion
// ============================================================================

union ConvertBytesToLong {
    uint8_t byte[4];
    int32_t longVal;
};

union ConvertBytesToULong {
    uint8_t byte[4];
    uint32_t longVal;
};

union ConvertBytesToInt {
    uint8_t byte[2];
    int16_t intVal;
};

union ConvertBytesToUInt {
    uint8_t byte[2];
    uint16_t intVal;
};

// ============================================================================
// CCPACKET Structure
// ============================================================================

struct CCPACKET {
    uint8_t length;
    uint8_t data[CC1101_MAX_BUFFER_SIZE];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

// ============================================================================
// Common LED Configuration
// ============================================================================

#define LED_ANIM_ON 0
#define LED_ANIM_SLOW_FADE 1
#define LED_ANIM_NORMAL_FADE 2
#define LED_ANIM_FAST_FADE 3
#define LED_ANIM_PULSE 4

#define LED_COLOR_OFF 0
#define LED_COLOR_RED 1

// ============================================================================
// Common Output Configuration
// ============================================================================

#define OUTPUTS_DEFAULT_ONESHOT_DELAY 3000
#define OUTPUTS_DEFAULT_PULSE_DELAY 1000
#define OUTPUTS_PULSE_COUNT 3

// ============================================================================
// Serial Configuration
// ============================================================================

#define SERIAL_TX_BUFFER_SIZE 80
#define SERIAL_RX_BUFFER_SIZE 120

#ifndef DEC
#define DEC 10
#endif

#endif // COMMON_CONFIG_H
