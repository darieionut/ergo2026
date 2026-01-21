# CLAUDE.md - Context pentru Claude Code

## Descriere Proiect

Acesta este un proiect de firmware Arduino pentru un sistem industrial de detectie a gazelor si alarmare. Proiectul contine doua componente hardware separate care comunica wireless.

## Fisiere Principale

| Fisier | Descriere | Versiune |
|--------|-----------|----------|
| `ergo-floor-module-v4.0.cpp` | Firmware pentru Modulul de Etaj (receptor central) | 4.0 |
| `fw-ergo-detector-source-v2.5.cpp` | Firmware pentru Detectorul de Gaze | 2.5 |

## Arhitectura Sistemului

```
[Detector Gaze] ---(CC1101 868MHz)---> [Modul Etaj]
     |                                      |
  Senzor TGS                            LCD + Meniu
  LED Status                            EEPROM + SRAM
  Buzzer                                Management Alarme
  Releu AC                              Releu AC + DC
                                        Lista Detectoare
```

## Conventii de Cod

- **Limbaj:** C++ pentru Arduino (AVR)
- **Prefixe clase:** Controller (hardware), Service (logica business)
- **Constante:** UPPERCASE cu underscore (ex: `APP_VERSION`, `BTN_COUNT`)
- **Variabile:** camelCase (ex: `currentValue`, `lastUpdateMs`)
- **Tipuri:** Foloseste `uint8_t`, `uint16_t`, `int32_t` etc.

## Clase Importante

### Detector (`fw-ergo-detector-source-v2.5.cpp`)

| Clasa | Descriere |
|-------|-----------|
| `TGSController` | Citire senzor gaz TGS |
| `AlarmService` | Gestionare stari alarma |
| `BuzzerController` | Control buzzer |
| `LEDController` | Indicatori LED cu animatii |
| `ButtonController` | Gestionare buton TEST |
| `OutputController` | Control iesire releu AC |
| `EEPROMController` | Stocare persistenta |
| `CC1101Controller` | Comunicatie radio |
| `CommunicationService` | Protocol comunicatie |
| `ProgrammingService` | Configurare prin serial |
| `ServiceMode` | Mod service/testare |
| `SerialDebug` | Mesaje debug conditionale |

### Floor Module (`ergo-floor-module-v4.0.cpp`)

| Clasa | Descriere |
|-------|-----------|
| `MenuService` | Sistem meniu LCD navigabil |
| `SRAMController` | Stocare detectoare/alarme in SRAM |
| `CC1101Controller` | Comunicatie radio |
| `EEPROMController` | Stocare persistenta + intervale verificare |
| `ButtonController` | Gestionare 4 butoane navigare |
| `LEDController` | Indicator LED status |
| `OutputController` | Control 2 relee (AC + DC) |
| `CommunicationService` | Protocol comunicatie |
| `CoreService` | Logica principala aplicatie |
| `ServiceMode` | Mod service |
| `ProgrammingService` | Configurare prin serial |

## Configuratie Hardware (Pini)

### Detector de Gaze

| Pin | Functie | Constanta |
|-----|---------|-----------|
| 6 | LED Status | `HW_STATUS_LED_PIN` |
| 5 | LED Rosu | `HW_OUTPUT_LED_RED_PIN` |
| 4 | LED Galben | `HW_OUTPUT_LED_YELLOW_PIN` |
| 9 | LED Verde | `HW_OUTPUT_LED_GREEN_PIN` |
| 7 | Buzzer | `HW_OUTPUT_BUZZER_PIN` |
| 2 | Releu | `HW_OUTPUT_RELAY_PIN` |
| 16 | Buton TEST | `HW_INPUT_BTN_TEST_PIN` |
| 17 | Senzor TGS | `HW_TGS_SENSOR_PIN` |
| 0 | RXD0 | `HW_RXD0_PIN` |

### Modul de Etaj

| Pin | Functie | Constanta |
|-----|---------|-----------|
| 6 | LED Status | `HW_STATUS_LED_PIN` |
| 9 | Releu AC | `HW_OUTPUT_RELAY_AC_PIN` |
| 7 | Releu DC | `HW_OUTPUT_RELAY_DC_PIN` |
| 5 | Buton BACK | `HW_INPUT_BTN_BACK_PIN` |
| 4 | Buton LEFT | `HW_INPUT_BTN_LEFT_PIN` |
| 17 | Buton RIGHT | `HW_INPUT_BTN_RIGHT_PIN` |
| 16 | Buton OK | `HW_INPUT_BTN_OK_PIN` |
| 15 | SPI CS SRAM | `HW_SPI_CS_SRAM_PIN` |
| 14 | NC | `HW_IO_NC_PIN` |

## Comunicatie Radio CC1101

| Parametru | Valoare |
|-----------|---------|
| Frecventa | 868 MHz |
| Interfata | SPI |
| Interrupt Pin | 1 (`CC1101_Interrupt`) |
| GDO2 Pin | 3 (`CC1101_GDO2`) |
| Sync Words | 0xB5, 0x47 |
| Max Buffer | 30 bytes |
| Max TX Buffer | 15 bytes |
| Max RX Buffer | 100 bytes |

### Niveluri Putere Transmisie

| Constanta | Valoare | Descriere |
|-----------|---------|-----------|
| `PA_LowPower` | 0x60 | Putere redusa |
| `PA_LongDistance` | 0xC0 | Distanta mare |

### Stari RF

```cpp
enum RFSTATE {
  RFSTATE_IDLE = 0,
  RFSTATE_RX,
  RFSTATE_TX
};
```

## Protocol Comunicatie

### Format Buffer Transmisie

| Index | Constanta | Descriere |
|-------|-----------|-----------|
| 0 | `BUFF_INDEX_RECEIVER_LO` | Adresa receiver (low byte) |
| 1 | `BUFF_INDEX_RECEIVER_HI` | Adresa receiver (high byte) |
| 2 | `BUFF_INDEX_ADDRESS_LO` | Adresa dispozitiv (low byte) |
| 3 | `BUFF_INDEX_ADDRESS_HI` | Adresa dispozitiv (high byte) |
| 4 | `BUFF_INDEX_ID_LO` | ID dispozitiv (low byte) |
| 5 | `BUFF_INDEX_ID_HI` | ID dispozitiv (high byte) |
| 6 | `BUFF_INDEX_COMMAND` | Comanda |
| 7 | `BUFF_INDEX_IS_FORWARD` | Flag forward |

### Comenzi Protocol (Detector)

| Constanta | Valoare | Descriere |
|-----------|---------|-----------|
| `COMMAND_ID_TEST_RSSI` | 1 | Test calitate semnal |
| `COMMAND_ID_ALARM_START` | 10 | Alarma pornita |
| `COMMAND_ID_ALARM_STOP` | 11 | Alarma oprita |

### Comenzi Protocol (Floor Module - Programare)

| Constanta | Valoare | Descriere |
|-----------|---------|-----------|
| `COMMAND_ID_SET_DEVICE_ADDRESS` | 1 | Seteaza adresa dispozitiv |
| `COMMAND_ID_SET_DEVICE_CHANNEL` | 2 | Seteaza canal |
| `COMMAND_ID_SET_FORWARD_ADDRESS` | 3 | Seteaza adresa forward |
| `COMMAND_ID_SET_FORWARD_CHANNEL` | 4 | Seteaza canal forward |
| `COMMAND_ID_SET_OPERATION_MODE` | 5 | Seteaza mod operare |
| `COMMAND_ID_SET_DEBUG_MODE` | 6 | Seteaza mod debug |
| `COMMAND_ID_GET_DEVICE_TYPE` | 10 | Citeste tip dispozitiv |
| `COMMAND_ID_GET_ALL_VALUES` | 20 | Citeste toate valorile |
| `COMMAND_ID_SET_ALL_VALUES` | 21 | Seteaza toate valorile |
| `COMMAND_ID_STREAM_ALL_MEMORY` | 30 | Stream memorie |
| `COMMAND_ID_CLEAR_ALARMS` | 31 | Sterge alarme |
| `COMMAND_ID_CLEAR_DETECTORS` | 32 | Sterge detectoare |

## Harta Memorie EEPROM (Detector)

| Adresa | Constanta | Tip | Descriere |
|--------|-----------|-----|-----------|
| 50 | `EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT` | uint16_t | Valoare declansare alarma |
| 52 | `EEPROM_CONFIG_DEVICE_ID_INT` | uint16_t | ID dispozitiv |
| 100 | `EEPROM_CC1101_DEVICE_ADDRESS_INT` | uint16_t | Adresa radio |
| 102 | `EEPROM_CC1101_DEVICE_CHANNEL_BYTE` | uint8_t | Canal radio |
| 103 | `EEPROM_CC1101_RECEIVER_ADDRESS_INT` | uint16_t | Adresa receiver |

## Harta Memorie EEPROM (Floor Module)

| Adresa | Constanta | Descriere |
|--------|-----------|-----------|
| 250 | `EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE` | Numar intervale verificare |
| 251-291 | `EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_*` | Intervale verificare (segment 4 bytes) |

## Structuri de Date

```cpp
// Intrare alarma
struct AlarmEntry {
    uint16_t address;
    uint16_t id;
    boolean state;
};

// Intrare detector
struct DetectorEntry {
    uint16_t address;
    uint16_t id;
    uint8_t linkQuality;
    uint8_t status;
    uint32_t lastUpdateMs;
};

// Detector offline
struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;
};

// Interval verificare adrese
struct AddressVerificationInterval {
    uint16_t min;
    uint16_t max;
};

// Pachet radio
struct CCPACKET {
    uint8_t length;
    uint8_t data[CC1101_MAX_BUFFER_SIZE];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

// Conversii bytes
union ConvertBytesToLong { uint8_t byte[4]; int32_t longVal; };
union ConvertBytesToULong { uint8_t byte[4]; uint32_t longVal; };
union ConvertBytesToInt { uint8_t byte[2]; int16_t intVal; };
union ConvertBytesToUInt { uint8_t byte[2]; uint16_t intVal; };
```

## Constante Critice

| Constanta | Valoare | Descriere |
|-----------|---------|-----------|
| `DETECTOR_OFFLINE_MS` | 420000 (7 min) | Timp pana detector offline |
| `WATCHDOG_RECEIVE_RESET_MS` | 300000 (5 min) | Timeout watchdog |
| `SERIAL_BAUDRATE` | 19200 | Viteza seriala |
| `BTN_DEBOUNCE_PERIOD` | 40 ms | Debounce butoane |
| `ALARM_STATUS_MESSAGE_RESEND_MS_MIN` | 30000 (30s) | Interval minim retrimitere |
| `ALARM_STATUS_MESSAGE_RESEND_MS_MAX` | 120000 (2 min) | Interval maxim retrimitere |
| `ALARM_MESSAGE_MAX_RESEND_COUNT` | 5 | Numar maxim retrimiteri alarma |
| `TGS_DEFAULT_ALERT_VALUE` | 406 | Prag implicit alarma gaz |
| `DEFAULT_DEVICE_ID` | 65000 | ID implicit dispozitiv |
| `DEFAULT_RECEIVER_ADDRESS` | 1 | Adresa implicita receiver |

## Animatii LED

| Constanta | Valoare | Descriere |
|-----------|---------|-----------|
| `LED_ANIM_ON` | 0 | LED aprins constant |
| `LED_ANIM_SLOW_FADE` | 1 | Fade lent |
| `LED_ANIM_NORMAL_FADE` | 2 | Fade normal |
| `LED_ANIM_FAST_FADE` | 3 | Fade rapid |
| `LED_ANIM_PULSE` | 4 | Pulsatie |

## Tipuri SRAM Suportate (Floor Module)

| Constanta | Capacitate |
|-----------|------------|
| `SRAM_64` | 8 KB |
| `SRAM_128` | 16 KB |
| `SRAM_256` | 32 KB |
| `SRAM_512` | 64 KB |
| `SRAM_1024` | 128 KB |

## Comenzi de Build

```bash
# Arduino IDE
# 1. Deschide fisierul .cpp
# 2. Selecteaza placa AVR (Arduino Pro Mini / Nano)
# 3. Compile & Upload

# PlatformIO
pio run -t upload
```

## Note pentru Dezvoltare

1. **Watchdog:** Este activ (`WATCHDOG_ENABLED true`) - codul trebuie sa ruleze `wdt_reset()` periodic
2. **EEPROM:** Folosit pentru stocare persistenta (numar serial, setari, adrese)
3. **Debug:** Setat `DEBUG_ENABLED false` in productie pentru a economisi memorie
4. **Butoane:** Au debounce de 40ms implementat hardware
5. **Alarma:** Se trimite de maxim 5 ori (`ALARM_MESSAGE_MAX_RESEND_COUNT`)
6. **SRAM:** Doar pe Floor Module - stocheaza liste detectoare si alarme
7. **Relee:** Detector are 1 releu (AC), Floor Module are 2 (AC + DC)

## Testare

- **Detector:** Foloseste butonul TEST pentru a simula alarma
- **Floor Module:** Navigheaza cu butoanele BACK/LEFT/RIGHT/OK
- **Debug Serial:** 19200 baud, activat cu `DEBUG_ENABLED true`
- **Test RSSI:** Comanda `COMMAND_ID_TEST_RSSI` pentru calitate semnal

## Fluxul Alarmei

1. Senzorul TGS detecteaza gaz peste `TGS_DEFAULT_ALERT_VALUE` (406)
2. `AlarmService` porneste alarma (`startAlarm()`)
3. Se activeaza buzzer, LED rosu, releu AC
4. Se trimite pachet `COMMAND_ID_ALARM_START` la Floor Module
5. Se retrimite periodic (30s-2min) pana la confirmare sau maxim 5 ori
6. Cand valoarea scade, se trimite `COMMAND_ID_ALARM_STOP`
