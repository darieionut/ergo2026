# CLAUDE.md - Context pentru Claude Code

## Descriere Proiect

Acesta este un proiect de firmware Arduino pentru un sistem industrial de detectie a gazelor si alarmare. Proiectul contine doua componente hardware separate care comunica wireless.

## Structura Proiect

```
ergo2026/
├── common/                              # Cod partajat
│   ├── Config.h                         # CC1101, Types, CCPACKET, LED, Output configs
│   ├── Controllers.h/.cpp               # ButtonController, CC1101Controller, EEPROMController, LEDController, OutputController
│   └── Services.h/.cpp                  # SerialDebug, CommunicationService
│
├── detector/                            # Firmware Detector Gaze
│   ├── Config.h                         # Main, Hardware, EEPROM, Alert, Buzzer, TGS configs
│   ├── Models.h                         # CompleteData struct
│   ├── Controllers.h/.cpp               # BuzzerController, TGSController, SemaphoreLEDController
│   ├── Services.h/.cpp                  # AlarmService, ServiceMode, ProgrammingService, Startup
│   └── fw-ergo-detector.ino             # Entry point Arduino
│
├── floor-module/                        # Firmware Modul Etaj
│   ├── Config.h                         # Main, Hardware, EEPROM, SRAM, Menu configs
│   ├── Models.h                         # AlarmEntry, DetectorEntry, OfflineDetectorEntry, etc.
│   ├── Controllers.h/.cpp               # MicrochipSRAM, SRAMController
│   ├── Services.h/.cpp                  # MenuService, CoreService, ServiceMode, ProgrammingService
│   └── ergo-floor-module.ino            # Entry point Arduino
│
├── ergo-floor-module-v4.0.cpp           # Versiune monolitica (legacy)
├── fw-ergo-detector-source-v2.5.cpp     # Versiune monolitica (legacy)
└── refactorizare.md                     # Documentatie refactorizare
```

## Fisiere Principale (Legacy)

| Fisier | Descriere | Versiune |
|--------|-----------|----------|
| `ergo-floor-module-v4.0.cpp` | Firmware Modul Etaj (monolitic) | 4.0 |
| `fw-ergo-detector-source-v2.5.cpp` | Firmware Detector Gaze (monolitic) | 2.5 |

## Arhitectura Sistemului

```
[Detector Gaze] ---(CC1101 868MHz)---> [Modul Etaj]
     |                                      |
  DEVICE_TYPE=1                        DEVICE_TYPE=2
  Senzor TGS                           LCD 16x2 I2C + Meniu
  LED Semafor (R/Y/G)                  EEPROM I2C extern
  Buzzer                               SRAM SPI extern
  Releu AC                             2x Relee (AC/DC)
  1 Buton (TEST)                       4 Butoane (BACK/LEFT/RIGHT/OK)
```

## Conventii de Cod

- **Limbaj:** C++ pentru Arduino (AVR)
- **Prefixe clase:** Controller (hardware), Service (logica business)
- **Constante:** UPPERCASE cu underscore (ex: `APP_VERSION`, `BTN_COUNT`)
- **Variabile:** camelCase (ex: `currentValue`, `lastUpdateMs`)
- **Tipuri:** Foloseste `uint8_t`, `uint16_t`, `int32_t` etc.

## Clase Principale

### Comune (`common/`)
| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `ButtonController` | Controllers.cpp | Gestionare butoane cu debounce |
| `CC1101Controller` | Controllers.cpp | Control modul radio CC1101 |
| `EEPROMController` | Controllers.cpp | Operatii EEPROM I2C extern |
| `LEDController` | Controllers.cpp | Control LED status cu animatii |
| `OutputController` | Controllers.cpp | Control iesiri releu cu pulse/oneshot |
| `SerialDebug` | Services.cpp | Debug conditional prin serial |
| `CommunicationService` | Services.cpp | Trimitere/receptie pachete radio |

### Detector (`detector/`)
| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `TGSController` | Controllers.cpp | Citire senzor gaz TGS |
| `BuzzerController` | Controllers.cpp | Control buzzer alarma |
| `SemaphoreLEDController` | Controllers.cpp | LED-uri semafor (R/Y/G) |
| `AlarmService` | Services.cpp | Gestionare stari alarma |
| `ProgrammingService` | Services.cpp | Programare prin serial |

### Floor Module (`floor-module/`)
| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `MicrochipSRAM` | Controllers.cpp | Driver SRAM SPI Microchip |
| `SRAMController` | Controllers.cpp | Stocare detectoare/alarme in SRAM |
| `MenuService` | Services.cpp | Sistem meniu LCD navigabil |
| `CoreService` | Services.cpp | Logica principala receptie/forwarding |

## Configurare Hardware - Pini

### Detector
```cpp
#define HW_STATUS_LED_PIN        6    // LED status
#define HW_OUTPUT_LED_RED_PIN    5    // LED rosu semafor
#define HW_OUTPUT_LED_YELLOW_PIN 4    // LED galben semafor
#define HW_OUTPUT_LED_GREEN_PIN  9    // LED verde semafor
#define HW_OUTPUT_BUZZER_PIN     7    // Buzzer
#define HW_OUTPUT_RELAY_PIN      2    // Releu AC
#define HW_INPUT_BTN_TEST_PIN    16   // Buton TEST
#define HW_TGS_SENSOR_PIN        17   // Senzor TGS (analog)
```

### Floor Module
```cpp
#define HW_STATUS_LED_PIN        6    // LED status
#define HW_OUTPUT_RELAY_AC_PIN   9    // Releu AC
#define HW_OUTPUT_RELAY_DC_PIN   7    // Releu DC
#define HW_INPUT_BTN_BACK_PIN    5    // Buton BACK
#define HW_INPUT_BTN_LEFT_PIN    4    // Buton LEFT
#define HW_INPUT_BTN_RIGHT_PIN   17   // Buton RIGHT
#define HW_INPUT_BTN_OK_PIN      16   // Buton OK
#define HW_SPI_CS_SRAM_PIN       15   // Chip Select SRAM
```

## Comunicatie Radio CC1101

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| Frecventa | 868 MHz | Banda ISM Europa |
| Interfata | SPI | Pin SS standard |
| Interrupt | Pin 3 (GDO2) | Notificare pachet primit |
| Sync Words | 0xB5, 0x47 | Identificare protocol |
| Buffer TX | 15 bytes max | Dimensiune pachet transmisie |
| Buffer RX | 100 bytes | Buffer receptie |
| PA Level | 0xC0 (Long Distance) | Putere transmisie |

## Moduri Operare Floor Module

| Mod | Valoare | Descriere |
|-----|---------|-----------|
| `OPERATION_MODE_MASTER` | 1 | Unitate principala - primeste de la detectoare |
| `OPERATION_MODE_SLAVE` | 2 | Retransmite catre alt modul (forwarding) |

## Adrese EEPROM

### Detector
| Adresa | Tip | Descriere |
|--------|-----|-----------|
| 0-3 | uint32_t | Numar serial |
| 10 | uint16_t | Startup flag |
| 50 | uint16_t | Valoare declansare alarma |
| 52 | uint16_t | Device ID |
| 100 | uint16_t | Adresa CC1101 dispozitiv |
| 102 | uint8_t | Canal CC1101 |
| 103 | uint16_t | Adresa receiver (modul etaj) |

### Floor Module
| Adresa | Tip | Descriere |
|--------|-----|-----------|
| 0-3 | uint32_t | Numar serial |
| 10 | uint16_t | Startup flag |
| 50 | uint8_t | Mod operare (Master/Slave) |
| 100 | uint16_t | Adresa CC1101 dispozitiv |
| 102 | uint8_t | Canal CC1101 |
| 103 | uint16_t | Adresa forward (pentru Slave mode) |
| 105 | uint8_t | Canal forward |
| 109 | uint8_t | Index alarme memorate |
| 110 | uint8_t | Numar alarme memorate |
| 111-210 | - | Zona stocare alarme (5 bytes/alarma) |
| 250 | uint8_t | Numar intervale verificare adrese |
| 251-291 | - | Intervale verificare adrese (4 bytes/interval) |

## Configurare SRAM (Floor Module)

| Zona | Start | End | Segment | Descriere |
|------|-------|-----|---------|-----------|
| Alarme | 0 | 6050 | 5 bytes | Stocarea alarmelor active |
| Detectoare | 6050 | 54050 | 10 bytes | Lista detectoare cunoscute |
| Offline | 54050 | 63650 | 2 bytes | Index detectoare offline |

## Comenzi Programare Serial

### Detector (Baud: 19200)
| ID | Comanda | Descriere |
|----|---------|-----------|
| 1 | SET_DEVICE_ADDRESS | Seteaza adresa CC1101 |
| 2 | SET_DEVICE_CHANNEL | Seteaza canalul CC1101 |
| 3 | SET_RECEIVER_ADDRESS | Seteaza adresa modulului etaj |
| 4 | SET_DEVICE_ID | Seteaza ID-ul dispozitivului |
| 5 | SET_ALARM_TRIGGER | Seteaza valoarea declansare alarma |
| 6 | SET_DEBUG_MODE | Activeaza/dezactiveaza debug |
| 10 | GET_DEVICE_TYPE | Returneaza tipul dispozitivului |
| 20 | GET_ALL_VALUES | Citeste toate valorile configurate |
| 21 | SET_ALL_VALUES | Seteaza toate valorile |
| 101 | TEST_MODE | Intra in mod testare |

### Floor Module (Baud: 19200)
| ID | Comanda | Descriere |
|----|---------|-----------|
| 1 | SET_DEVICE_ADDRESS | Seteaza adresa CC1101 |
| 2 | SET_DEVICE_CHANNEL | Seteaza canalul CC1101 |
| 3 | SET_FORWARD_ADDRESS | Seteaza adresa forward (Slave mode) |
| 4 | SET_FORWARD_CHANNEL | Seteaza canal forward |
| 5 | SET_OPERATION_MODE | Seteaza mod operare (Master/Slave) |
| 6 | SET_DEBUG_MODE | Activeaza/dezactiveaza debug |
| 10 | GET_DEVICE_TYPE | Returneaza tipul dispozitivului |
| 20 | GET_ALL_VALUES | Citeste toate valorile |
| 21 | SET_ALL_VALUES | Seteaza toate valorile |
| 30 | STREAM_ALL_MEMORY | Transmite continutul memoriei |
| 31 | CLEAR_ALARMS | Sterge toate alarmele |
| 32 | CLEAR_DETECTORS | Sterge lista detectoare |
| 40 | ADD_ADDRESS_VERIFICATION_INTERVAL | Adauga interval verificare |
| 41 | CLEAR_ADDRESS_VERIFICATION_INTERVALS | Sterge intervalele |
| 101 | TEST_MODE | Intra in mod testare |

## Configurare Senzor TGS (Detector)

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `TGS_STARTUP_DELAY_MS` | 60000 (1 min) | Timp incalzire senzor |
| `TGS_DEFAULT_ALERT_VALUE` | 406 | Prag default alarma (ADC) |
| `TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM` | 6 | Masuratori consecutive pentru confirmare |

## Configurare Alarme (Detector)

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `ALARM_STATUS_MESSAGE_RESEND_MS_MIN` | 30000 (30s) | Interval minim retrimitere |
| `ALARM_STATUS_MESSAGE_RESEND_MS_MAX` | 120000 (2 min) | Interval maxim retrimitere |
| `ALARM_MESSAGE_MAX_RESEND_COUNT` | 5 | Numar maxim retrimiteri |
| `ALARM_MIN_ACTIVE_PERIOD_MS` | 180000 (3 min) | Durata minima alarma activa |

## Constante Critice

| Constanta | Valoare | Scop |
|-----------|---------|------|
| `DETECTOR_OFFLINE_MS` | 420000 (7 min) | Timp pana detector e marcat offline |
| `WATCHDOG_RECEIVE_RESET_MS` | 300000 (5 min) | Timeout watchdog receptie |
| `SERIAL_BAUDRATE` | 19200 | Viteza seriala |
| `BTN_DEBOUNCE_PERIOD` | 40 ms | Debounce butoane |
| `BEEPING_INTERVAL_MS` | 400 ms | Interval buzzer |
| `MENU_TIMEOUT_MS` | 120000 (2 min) | Timeout meniu LCD |
| `MENU_BACKLIGHT_OFF_MS` | 60000 (1 min) | Timeout backlight LCD |

## Structuri de Date Importante

```cpp
// Pachet radio
struct CCPACKET {
    uint8_t length;
    uint8_t data[30];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

// Intrare alarma (5 bytes)
struct AlarmEntry {
    uint16_t address;
    uint16_t id;
    boolean state;
};

// Intrare detector (10 bytes)
struct DetectorEntry {
    uint16_t address;
    uint16_t id;
    uint8_t linkQuality;
    uint8_t status;
    uint32_t lastUpdateMs;
};

// Interval verificare adrese (4 bytes)
struct AddressVerificationInterval {
    uint16_t min;
    uint16_t max;
};
```

## Configurare Meniu LCD (Floor Module)

| Parametru | Valoare |
|-----------|---------|
| Adresa I2C | 0x27 |
| Caractere | 16 |
| Randuri | 2 |
| Numar pagini | 10 |

### Pagini Meniu
| Index | Pagina | Descriere |
|-------|--------|-----------|
| 0 | HOME | Ecran principal - status general |
| 1 | ALARMS | Numar alarme active |
| 2 | DETECTORS | Numar detectoare online |
| 3 | DETECTORS_OFFLINE | Numar detectoare offline |
| 4 | LIST_ALARMS | Lista alarme |
| 5 | LIST_DETECTORS | Lista detectoare |
| 6 | LIST_DETECTORS_OFFLINE | Lista offline |
| 7 | CLEAR_ALARMS | Confirmare stergere alarme |
| 8 | CLEAR_DETECTORS | Confirmare stergere detectoare |
| 9 | FIRMWARE | Versiune firmware |

## Dependinte Externe

| Biblioteca | Utilizare |
|------------|-----------|
| `<avr/wdt.h>` | Watchdog timer |
| `<Arduino.h>` | Framework Arduino |
| `<SPI.h>` | Comunicatie SPI (CC1101, SRAM) |
| `<extEEPROM.h>` | EEPROM extern I2C |
| `<LiquidCrystal_I2C.h>` | LCD I2C (doar Floor Module) |

## Comenzi de Build

```bash
# Arduino IDE
# 1. Deschide fisierul .ino din detector/ sau floor-module/
# 2. Selecteaza placa AVR corespunzatoare
# 3. Compile & Upload

# PlatformIO
pio run -t upload

# Pentru versiunile legacy monolitice:
# Deschide direct .cpp si compileaza
```

## Note pentru Dezvoltare

1. **Watchdog:** Este activ - codul trebuie sa ruleze `wdt_reset()` periodic
2. **EEPROM:** Folosit pentru stocare persistenta (numar serial, setari, alarme)
3. **SRAM:** Floor Module foloseste SRAM extern pentru liste mari de detectoare
4. **Debug:** Setat `DEBUG_ENABLED false` in productie
5. **Butoane:** Au debounce de 40ms implementat
6. **Interrupt:** `packetWaiting` este volatile - folosit in ISR pentru CC1101
7. **Forwarding:** In mod Slave, Floor Module retransmite mesajele catre alt modul

## Testare

- Foloseste butonul TEST pe detector pentru a simula alarma
- Modulul de etaj afiseaza statusul pe LCD
- Comunicatia seriala la 19200 baud pentru debug si programare
- Comanda 101 (TEST_MODE) pentru mod testare prin serial
