# Plan de Refactorizare - Proiect ERGO2026

## Rezumat

Acest document conține logica de împărțire a codului sursă din cele două fișiere monolitice:
- `ergo-floor-module-v4.0.cpp` (3869 linii)
- `fw-ergo-detector-source-v2.5.cpp` (2725 linii)

---

## Structura Propusă pentru Directoare

```
ergo2026/
├── common/                          # Cod partajat între Module și Detector
│   ├── config/
│   │   ├── CC1101Config.h           # Configurări RF CC1101
│   │   └── TypesConfig.h            # Tipuri de date comune (unions)
│   ├── controllers/
│   │   ├── ButtonController.h
│   │   ├── ButtonController.cpp
│   │   ├── CC1101Controller.h
│   │   ├── CC1101Controller.cpp
│   │   ├── EEPROMController.h
│   │   ├── EEPROMController.cpp
│   │   ├── LEDController.h
│   │   ├── LEDController.cpp
│   │   ├── OutputController.h
│   │   └── OutputController.cpp
│   ├── services/
│   │   ├── CommunicationService.h
│   │   ├── CommunicationService.cpp
│   │   ├── SerialDebug.h
│   │   └── SerialDebug.cpp
│   └── utils/
│       └── CCPACKET.h
│
├── floor-module/                    # Cod specific pentru Floor Module
│   ├── config/
│   │   ├── MainConfig.h             # APP_VERSION, DEVICE_TYPE, etc.
│   │   ├── HardwareConfig.h         # Pinii hardware specifici
│   │   ├── EEPROMConfig.h           # Adrese EEPROM specifice
│   │   ├── SRAMConfig.h             # Configurări memorie SRAM
│   │   └── MenuConfig.h             # Texte și configurări meniu LCD
│   ├── controllers/
│   │   ├── MicrochipSRAM.h
│   │   ├── MicrochipSRAM.cpp
│   │   ├── SRAMController.h
│   │   └── SRAMController.cpp
│   ├── services/
│   │   ├── MenuService.h
│   │   ├── MenuService.cpp
│   │   ├── CoreService.h
│   │   ├── CoreService.cpp
│   │   ├── ServiceMode.h
│   │   ├── ServiceMode.cpp
│   │   ├── ProgrammingService.h
│   │   ├── ProgrammingService.cpp
│   │   ├── Startup.h
│   │   └── Startup.cpp
│   ├── models/
│   │   ├── AlarmEntry.h
│   │   ├── DetectorEntry.h
│   │   ├── OfflineDetectorEntry.h
│   │   ├── AddressVerificationInterval.h
│   │   └── CompleteData.h
│   └── ergo-floor-module.ino        # Fișier principal Arduino
│
├── detector/                        # Cod specific pentru Detector
│   ├── config/
│   │   ├── MainConfig.h             # APP_VERSION, DEVICE_TYPE, etc.
│   │   ├── HardwareConfig.h         # Pinii hardware specifici
│   │   ├── EEPROMConfig.h           # Adrese EEPROM specifice
│   │   ├── AlertConfig.h            # Configurări alarme
│   │   ├── BuzzerConfig.h           # Configurări buzzer
│   │   └── TGSConfig.h              # Configurări senzor TGS
│   ├── controllers/
│   │   ├── BuzzerController.h
│   │   ├── BuzzerController.cpp
│   │   ├── TGSController.h
│   │   └── TGSController.cpp
│   ├── services/
│   │   ├── AlarmService.h
│   │   ├── AlarmService.cpp
│   │   ├── ServiceMode.h
│   │   ├── ServiceMode.cpp
│   │   ├── ProgrammingService.h
│   │   ├── ProgrammingService.cpp
│   │   ├── Startup.h
│   │   └── Startup.cpp
│   ├── models/
│   │   └── CompleteData.h
│   └── fw-ergo-detector.ino         # Fișier principal Arduino
│
└── README.md
```

---

## PARTEA 1: Cod Comun (common/)

### 1.1 common/config/CC1101Config.h
**Linii sursă:** ergo-floor-module-v4.0.cpp: 19-169, fw-ergo-detector-source-v2.5.cpp: 44-199

```cpp
// Conținut:
#ifndef CC1101CONFIG_H
#define CC1101CONFIG_H

#define CC1101_Interrupt 1
#define CC1101_GDO2 3

#define CC1101_MAX_BUFFER_SIZE 30
#define CC1101_MAX_TX_BUFFER_SIZE 15
#define CC1101_MAX_RX_BUFFER_SIZE 100

// Toate definițiile WRITE_BURST, READ_SINGLE, READ_BURST
// Toate definițiile CC1101_* (registre, comenzi strobe)
// Toate definițiile CC1101_DEFVAL_* (valori default)
// Definițiile CCDEF_*

#endif
```

### 1.2 common/config/TypesConfig.h
**Linii sursă:** ergo-floor-module-v4.0.cpp: 224-242, fw-ergo-detector-source-v2.5.cpp: 290-313

```cpp
// Conținut:
#ifndef TYPESCONFIG_H
#define TYPESCONFIG_H

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

#endif
```

### 1.3 common/utils/CCPACKET.h
**Linii sursă:** ergo-floor-module-v4.0.cpp: 211-219, fw-ergo-detector-source-v2.5.cpp: 269-282

```cpp
// Conținut:
#ifndef CCPACKET_H
#define CCPACKET_H

#include "CC1101Config.h"

struct CCPACKET {
  public:
    uint8_t length;
    uint8_t data[CC1101_MAX_BUFFER_SIZE];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

#endif
```

### 1.4 common/controllers/ButtonController.h & .cpp

**Header (.h):**
```cpp
#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H

#include <Arduino.h>

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

#endif
```

**Implementare (.cpp):**
- **Floor Module linii:** 1172-1241
- **Detector linii:** 1026-1088

**NOTĂ:** Implementarea diferă între module datorită numărului diferit de butoane:
- Floor Module: 4 butoane (BACK, LEFT, RIGHT, OK)
- Detector: 1 buton (TEST)

**Soluție:** Păstrăm o singură clasă dar parametrizăm prin config.

### 1.5 common/controllers/CC1101Controller.h & .cpp

**Header (.h):**
```cpp
#ifndef CC1101CONTROLLER_H
#define CC1101CONTROLLER_H

#include <SPI.h>
#include "CC1101Config.h"
#include "CCPACKET.h"
#include "EEPROMController.h"

enum RFSTATE {
  RFSTATE_IDLE = 0,
  RFSTATE_RX,
  RFSTATE_TX
};

// Macro-uri helper
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

#endif
```

**Implementare (.cpp):**
- **Floor Module linii:** 1245-1591
- **Detector linii:** 1127-1474

**NOTĂ:** Implementările sunt identice. Diferența este doar în liniile citite din EEPROM (adrese diferite).

### 1.6 common/controllers/EEPROMController.h & .cpp

**Header (.h):**
- Clasa de bază cu funcționalități comune
- **Floor Module linii:** 267-314
- **Detector linii:** 315-344

**Implementare (.cpp):**
- **Floor Module linii:** 1594-1803 (include și funcții pentru alarme)
- **Detector linii:** 1478-1552

**NOTĂ:** Floor Module are funcționalități extinse pentru alarme și intervale de verificare adrese. Detector are versiune simplificată.

**Soluție:** Creăm o clasă de bază și extindem pentru Floor Module.

### 1.7 common/controllers/LEDController.h & .cpp

**Header și Implementare:**
- **Floor Module linii:** 540-571 (header), 1804-1896 (impl)
- **Detector linii:** 445-490 (header), 1556-1735 (impl)

**NOTĂ:** Detector are funcția `setSemaphore()` pentru LED-uri colorate (roșu, galben, verde). Floor Module are doar LED status.

### 1.8 common/controllers/OutputController.h & .cpp

**Header și Implementare:**
- **Floor Module linii:** 575-603 (header), 1898-1971 (impl)
- **Detector linii:** 512-551 (header), 1739-1816 (impl)

**NOTĂ:** Floor Module are 2 ieșiri (RELAY_AC, RELAY_DC), Detector are 1 ieșire (RELAY_AC).

### 1.9 common/services/SerialDebug.h & .cpp

**Header și Implementare:**
- **Floor Module linii:** 609-647
- **Detector linii:** 558-601

**Codul este identic în ambele fișiere.**

### 1.10 common/services/CommunicationService.h & .cpp

**Header:**
- **Floor Module linii:** 649-678
- **Detector linii:** 605-657

**NOTĂ:** Detector are funcții suplimentare: `sendAlarmStartPackage()`, `sendAlarmStopPackage()`.

---

## PARTEA 2: Floor Module (floor-module/)

### 2.1 floor-module/config/MainConfig.h

```cpp
#ifndef MAINCONFIG_H
#define MAINCONFIG_H

#define APP_VERSION 40
#define DEVICE_TYPE 2
#define WATCHDOG_ENABLED true
#define WATCHDOG_RECEIVE_RESET_MS 300000
#define SERIAL_BAUDRATE 19200
#define DETECTOR_OFFLINE_MS 420000
#define DEBUG_ENABLED false

#endif
```

### 2.2 floor-module/config/HardwareConfig.h
**Linii:** 1131-1146

```cpp
#ifndef HARDWARECONFIG_H
#define HARDWARECONFIG_H

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

#define BTN_COUNT 5
#define BTN_DEBOUNCE_PERIOD 40
#define BTN_BACK 0
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_OK 3

#endif
```

### 2.3 floor-module/config/EEPROMConfig.h
**Linii:** 1102-1129

```cpp
#ifndef EEPROMCONFIG_H
#define EEPROMCONFIG_H

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

#endif
```

### 2.4 floor-module/config/SRAMConfig.h
**Linii:** 1158-1168

```cpp
#ifndef SRAMCONFIG_H
#define SRAMCONFIG_H

#define SRAM_ALARMS_MEMORY_START 0
#define SRAM_ALARMS_MEMORY_END 6050
#define SRAM_ALARMS_MEMORY_SEGMENT_SIZE 5
#define SRAM_DETECTORS_MEMORY_START 6050
#define SRAM_DETECTORS_MEMORY_END 54050
#define SRAM_DETECTORS_MEMORY_SEGMENT_SIZE 10
#define SRAM_OFFLINE_DETECTORS_MEMORY_START 54050
#define SRAM_OFFLINE_DETECTORS_MEMORY_END 63650
#define SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE 2

#endif
```

### 2.5 floor-module/config/MenuConfig.h
**Linii:** 681-750

Conține toate textele pentru LCD și configurările meniului.

### 2.6 floor-module/controllers/MicrochipSRAM.h & .cpp
**Linii:** 416-489

Controller pentru memoria SRAM externă Microchip.

### 2.7 floor-module/controllers/SRAMController.h & .cpp
**Linii:** 491-536

Gestionează alarme, detectoare și detectoare offline în SRAM.

### 2.8 floor-module/services/MenuService.h & .cpp
**Linii:** 752-784

Serviciu pentru interfața LCD și navigarea în meniu.

### 2.9 floor-module/services/CoreService.h & .cpp
**Linii:** 808-856

Logica principală: procesare mesaje, gestionare detectoare, forwarding.

### 2.10 floor-module/services/ServiceMode.h & .cpp
**Linii:** 860-878

Mod de testare/service.

### 2.11 floor-module/services/ProgrammingService.h & .cpp
**Linii:** 880-946

Programare prin serial.

### 2.12 floor-module/services/Startup.h & .cpp
**Linii:** 950-958

Inițializare la pornire.

### 2.13 floor-module/models/

**AlarmEntry.h:**
```cpp
struct AlarmEntry {
    uint16_t address;
    uint16_t id;
    boolean state;
};
```

**DetectorEntry.h:**
```cpp
struct DetectorEntry {
    uint16_t address;
    uint16_t id;
    uint8_t linkQuality;
    uint8_t status;
    uint32_t lastUpdateMs;
};
```

**OfflineDetectorEntry.h:**
```cpp
struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;
};
```

**AddressVerificationInterval.h:**
```cpp
struct AddressVerificationInterval {
    uint16_t min;
    uint16_t max;
};
```

**CompleteData.h:**
```cpp
struct CompleteData {
    uint16_t address;
    uint8_t channel;
    uint16_t forwardAddress;
    uint8_t forwardChannel;
    uint8_t operationMode;
    boolean debugEnabled;
};
```

### 2.14 floor-module/ergo-floor-module.ino
**Linii:** 960-1097

Fișierul principal cu `setup()` și `loop()`.

---

## PARTEA 3: Detector (detector/)

### 3.1 detector/config/MainConfig.h

```cpp
#ifndef MAINCONFIG_H
#define MAINCONFIG_H

#define APP_VERSION 25
#define DEVICE_TYPE 1
#define WATCHDOG_ENABLED true
#define SERIAL_BAUDRATE 19200
#define DEFAULT_DEVICE_ID 65000
#define DEFAULT_RECEIVER_ADDRESS 1
#define DISABLE_CC1101_RECEIVE
#define DEBUG_ENABLED false

#endif
```

### 3.2 detector/config/HardwareConfig.h
**Linii:** 994-1011

```cpp
#ifndef HARDWARECONFIG_H
#define HARDWARECONFIG_H

#define HW_STATUS_LED_PIN 6
#define HW_OUTPUT_LED_RED_PIN 5
#define HW_OUTPUT_LED_YELLOW_PIN 4
#define HW_OUTPUT_LED_GREEN_PIN 9
#define HW_OUTPUT_BUZZER_PIN 7
#define HW_OUTPUT_RELAY_PIN 2
#define HW_INPUT_BTN_TEST_PIN 16
#define HW_TGS_SENSOR_PIN 17
#define HW_RXD0_PIN 0

#define BTN_COUNT 1
#define BTN_DEBOUNCE_PERIOD 40
#define BTN_TEST 0

#endif
```

### 3.3 detector/config/EEPROMConfig.h
**Linii:** 970-992

```cpp
#ifndef EEPROMCONFIG_H
#define EEPROMCONFIG_H

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

#endif
```

### 3.4 detector/config/AlertConfig.h
**Linii:** 943-953

```cpp
#ifndef ALERTCONFIG_H
#define ALERTCONFIG_H

#define ALARM_STATUS_MESSAGE_RESEND_MS_MIN 30000
#define ALARM_STATUS_MESSAGE_RESEND_MS_MAX 120000
#define ALARM_MESSAGE_MAX_RESEND_COUNT 5
#define ALARM_MIN_ACTIVE_PERIOD_MS 180000

#endif
```

### 3.5 detector/config/BuzzerConfig.h
**Linii:** 963-968

```cpp
#ifndef BUZZERCONFIG_H
#define BUZZERCONFIG_H

#define BEEPING_INTERVAL_MS 400

#endif
```

### 3.6 detector/config/TGSConfig.h
**Linii:** 1013-1022

```cpp
#ifndef TGSCONFIG_H
#define TGSCONFIG_H

#define TGS_STARTUP_DELAY_MS 60000
#define TGS_DEFAULT_ALERT_VALUE 406
#define TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM 6

#endif
```

### 3.7 detector/controllers/BuzzerController.h & .cpp
**Linii:** 243-261 (header), 1092-1123 (impl)

Controller pentru buzzer/alarma sonoră.

### 3.8 detector/controllers/TGSController.h & .cpp
**Linii:** 492-510 (header)

Controller pentru senzori TGS de gaz.

### 3.9 detector/services/AlarmService.h & .cpp
**Linii:** 660-701

Serviciu pentru gestionarea alarmelor.

### 3.10 detector/services/ServiceMode.h & .cpp
**Linii:** 709-740

Mod de testare cu funcții specifice detector.

### 3.11 detector/services/ProgrammingService.h & .cpp
**Linii:** 742-786

Programare prin serial pentru detector.

### 3.12 detector/services/Startup.h & .cpp
**Linii:** 788-801

Inițializare la pornire pentru detector.

### 3.13 detector/models/CompleteData.h

```cpp
struct CompleteData {
    uint16_t address;
    uint8_t channel;
    uint16_t receiverAddress;
    uint16_t deviceId;
    int16_t alarmTrigger;
    boolean debugEnabled;
};
```

### 3.14 detector/fw-ergo-detector.ino
**Linii:** 804-941

Fișierul principal cu `setup()` și `loop()`.

---

## Pași de Implementare

### Pasul 1: Crearea structurii de directoare
```bash
mkdir -p common/{config,controllers,services,utils}
mkdir -p floor-module/{config,controllers,services,models}
mkdir -p detector/{config,controllers,services,models}
```

### Pasul 2: Extragerea codului comun
1. Creați fișierele de configurare comune (CC1101Config.h, TypesConfig.h)
2. Extrageti structurile de date comune
3. Creați clasele controller comune cu parametrizare

### Pasul 3: Extragerea codului specific Floor Module
1. Copiați configurările specifice
2. Adaptați controllere specifice (MicrochipSRAM, SRAMController)
3. Extrageți serviciile (MenuService, CoreService, etc.)
4. Creați fișierul principal .ino

### Pasul 4: Extragerea codului specific Detector
1. Copiați configurările specifice
2. Adaptați controllerele specifice (BuzzerController, TGSController)
3. Extrageți serviciile (AlarmService, etc.)
4. Creați fișierul principal .ino

### Pasul 5: Actualizarea include-urilor
Fiecare fișier .cpp trebuie să includă header-urile corecte:
```cpp
#include "../common/config/CC1101Config.h"
#include "../common/controllers/CC1101Controller.h"
// etc.
```

### Pasul 6: Testare
1. Compilați fiecare proiect separat
2. Verificați că funcționalitatea rămâne identică
3. Testați pe hardware real

---

## Beneficiile Refactorizării

1. **Mentenanță îmbunătățită**: Modificările în codul comun se reflectă în ambele proiecte
2. **Reducerea duplicării**: ~60% din cod este partajat
3. **Claritate**: Structura clară separă responsabilitățile
4. **Testabilitate**: Clasele pot fi testate independent
5. **Extensibilitate**: Adăugarea de noi funcționalități devine mai ușoară

---

## Note Importante

1. **Dependințe externe necesare:**
   - `<avr/wdt.h>` - Watchdog timer
   - `<Arduino.h>` - Framework Arduino
   - `<SPI.h>` - Comunicație SPI
   - `<extEEPROM.h>` - EEPROM extern I2C
   - `<LiquidCrystal_I2C.h>` - LCD (doar Floor Module)

2. **Variabile volatile:**
   - `packetWaiting` - trebuie să rămână volatile pentru interrupt

3. **Macro-uri specifice hardware:**
   - `cc1101_Select()`, `cc1101_Deselect()`, `wait_Miso()`, etc.
   - Trebuie definite în CC1101Controller sau într-un fișier hardware specific

---

## VARIANTA 2: Refactorizare Simplificată (Medium)

Această variantă reduce numărul de fișiere prin gruparea componentelor similare în fișiere mai mari.

### Structura Propusă (~20 fișiere vs ~50+ în varianta completă)

```
ergo2026/
├── common/
│   ├── Config.h                    # Toate configurările comune (CC1101, Types, CCPACKET)
│   ├── Controllers.h               # Declarații pentru toate controllerele comune
│   ├── Controllers.cpp             # Implementări CC1101, Button, LED, Output, EEPROM
│   └── Services.h/.cpp             # SerialDebug + CommunicationService
│
├── floor-module/
│   ├── Config.h                    # Main + Hardware + EEPROM + SRAM + Menu configs
│   ├── Models.h                    # Toate structurile de date
│   ├── Controllers.h/.cpp          # MicrochipSRAM + SRAMController
│   ├── Services.h/.cpp             # Menu + Core + ServiceMode + Programming + Startup
│   └── ergo-floor-module.ino       # Fișier principal
│
└── detector/
    ├── Config.h                    # Main + Hardware + EEPROM + Alert + Buzzer + TGS
    ├── Models.h                    # CompleteData
    ├── Controllers.h/.cpp          # Buzzer + TGS controllers
    ├── Services.h/.cpp             # Alarm + ServiceMode + Programming + Startup
    └── fw-ergo-detector.ino        # Fișier principal
```

### Detalii Fișiere

#### common/Config.h
```cpp
#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

// ========== CC1101 Config ==========
#define CC1101_Interrupt 1
#define CC1101_GDO2 3
#define CC1101_MAX_BUFFER_SIZE 30
// ... toate CC1101_* și CCDEF_*

// ========== Types ==========
union ConvertBytesToLong { uint8_t byte[4]; int32_t longVal; };
union ConvertBytesToULong { uint8_t byte[4]; uint32_t longVal; };
union ConvertBytesToInt { uint8_t byte[2]; int16_t intVal; };
union ConvertBytesToUInt { uint8_t byte[2]; uint16_t intVal; };

// ========== CCPACKET ==========
struct CCPACKET {
    uint8_t length;
    uint8_t data[CC1101_MAX_BUFFER_SIZE];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

#endif
```

#### common/Controllers.h
```cpp
#ifndef COMMON_CONTROLLERS_H
#define COMMON_CONTROLLERS_H

#include "Config.h"
#include <Arduino.h>
#include <SPI.h>

// ========== Button Controller ==========
struct btnState { uint8_t pin; boolean lastValue; uint32_t timestamp; boolean clicked; boolean stateChanged; };
class ButtonController {
    // ... declarații
};

// ========== LED Controller ==========
class LEDController {
    // ... declarații
};

// ========== Output Controller ==========
class OutputController {
    // ... declarații
};

// ========== EEPROM Controller ==========
class EEPROMController {
    // ... declarații
};

// ========== CC1101 Controller ==========
class CC1101Controller {
    // ... declarații
};

#endif
```

### Avantaje Varianta Medium
- **~20 fișiere** în loc de ~50+
- Mai ușor de navigat pentru proiecte mici
- Toate configurările într-un singur loc per modul
- Compilare mai rapidă (mai puține fișiere de procesat)

### Dezavantaje
- Fișiere mai mari, mai greu de citit
- Modificări într-o componentă afectează întregul fișier
- Mai puțină granularitate pentru control versiune

---

## VARIANTA 3: Refactorizare Minimală (Simple)

Această variantă păstrează codul aproape monolitic, dar cu o separare clară în doar 3-4 fișiere per proiect.

### Structura Propusă (~10 fișiere total)

```
ergo2026/
├── common/
│   └── Common.h                    # TOT codul comun într-un singur header
│
├── floor-module/
│   ├── FloorModuleConfig.h         # Toate configurările + modele
│   ├── FloorModuleLib.h            # Toate declarațiile claselor
│   ├── FloorModuleLib.cpp          # Toate implementările
│   └── ergo-floor-module.ino       # setup() + loop()
│
└── detector/
    ├── DetectorConfig.h            # Toate configurările + modele
    ├── DetectorLib.h               # Toate declarațiile claselor
    ├── DetectorLib.cpp             # Toate implementările
    └── fw-ergo-detector.ino        # setup() + loop()
```

### Detalii Fișiere

#### common/Common.h (~500 linii)
```cpp
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <SPI.h>

// ==================== CC1101 DEFINITIONS ====================
#define CC1101_Interrupt 1
#define CC1101_GDO2 3
#define CC1101_MAX_BUFFER_SIZE 30
#define CC1101_MAX_TX_BUFFER_SIZE 15
#define CC1101_MAX_RX_BUFFER_SIZE 100
// ... (toate definițiile CC1101_*, CCDEF_*, WRITE_BURST, etc.)

// ==================== TYPE UNIONS ====================
union ConvertBytesToLong { uint8_t byte[4]; int32_t longVal; };
union ConvertBytesToULong { uint8_t byte[4]; uint32_t longVal; };
union ConvertBytesToInt { uint8_t byte[2]; int16_t intVal; };
union ConvertBytesToUInt { uint8_t byte[2]; uint16_t intVal; };

// ==================== CCPACKET ====================
struct CCPACKET {
    uint8_t length;
    uint8_t data[CC1101_MAX_BUFFER_SIZE];
    boolean crc_ok;
    int8_t rssi;
    uint8_t lqi;
};

// ==================== SERIAL DEBUG ====================
class SerialDebug {
private:
    boolean debugEnabled;
public:
    SerialDebug(boolean enabled) : debugEnabled(enabled) {}
    void begin(uint32_t baud) { if(debugEnabled) Serial.begin(baud); }
    void print(const char* msg) { if(debugEnabled) Serial.print(msg); }
    void println(const char* msg) { if(debugEnabled) Serial.println(msg); }
    void print(int val) { if(debugEnabled) Serial.print(val); }
    void println(int val) { if(debugEnabled) Serial.println(val); }
};

// ==================== CC1101 CONTROLLER (INLINE) ====================
// Macros și funcții helper inline
#define cc1101_Select()   digitalWrite(SS, LOW)
#define cc1101_Deselect() digitalWrite(SS, HIGH)
#define wait_Miso()       while(digitalRead(MISO))

class CC1101Controller {
private:
    SPISettings spiSettings;
    uint8_t rfState;

    void writeBurstReg(uint8_t regAddr, uint8_t* buffer, uint8_t len);
    void readBurstReg(uint8_t* buffer, uint8_t regAddr, uint8_t len);

public:
    uint8_t channel;
    uint8_t syncWord[2];
    uint8_t devAddress;

    CC1101Controller();
    void init();
    void reset();
    void cmdStrobe(uint8_t cmd);
    uint8_t readReg(uint8_t regAddr, uint8_t regType);
    void writeReg(uint8_t regAddr, uint8_t value);
    void setCCregs();
    void setSyncWord(uint8_t syncH, uint8_t syncL);
    void setDevAddress(uint8_t addr);
    void setChannel(uint8_t chnl);
    bool sendData(CCPACKET packet);
    uint8_t receiveData(CCPACKET *packet);
    void setRxState();
    void setTxState();
    int computeRssi(uint8_t rssi_dec);
    int computeLqi(uint8_t raw);
};

#endif
```

#### floor-module/FloorModuleConfig.h
```cpp
#ifndef FLOOR_MODULE_CONFIG_H
#define FLOOR_MODULE_CONFIG_H

// ==================== MAIN CONFIG ====================
#define APP_VERSION 40
#define DEVICE_TYPE 2
#define WATCHDOG_ENABLED true
#define WATCHDOG_RECEIVE_RESET_MS 300000
#define SERIAL_BAUDRATE 19200
#define DETECTOR_OFFLINE_MS 420000
#define DEBUG_ENABLED false

// ==================== HARDWARE CONFIG ====================
#define HW_STATUS_LED_PIN 6
#define HW_OUTPUT_RELAY_AC_PIN 9
#define HW_OUTPUT_RELAY_DC_PIN 7
#define HW_INPUT_BTN_BACK_PIN 5
#define HW_INPUT_BTN_LEFT_PIN 4
#define HW_INPUT_BTN_RIGHT_PIN 17
#define HW_INPUT_BTN_OK_PIN 16
#define HW_SPI_CS_SRAM_PIN 15

#define BTN_COUNT 5
#define BTN_DEBOUNCE_PERIOD 40
#define BTN_BACK 0
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_OK 3

// ==================== EEPROM CONFIG ====================
#define EEPROM_SERIAL_NUMBER_ADDRESS_START 0
#define EEPROM_CC1101_DEVICE_ADDRESS_INT 100
#define EEPROM_CC1101_DEVICE_CHANNEL_BYTE 102
// ... restul definițiilor EEPROM

// ==================== SRAM CONFIG ====================
#define SRAM_ALARMS_MEMORY_START 0
#define SRAM_ALARMS_MEMORY_END 6050
#define SRAM_DETECTORS_MEMORY_START 6050
#define SRAM_DETECTORS_MEMORY_END 54050
// ... restul definițiilor SRAM

// ==================== MODELS ====================
struct AlarmEntry { uint16_t address; uint16_t id; boolean state; };
struct DetectorEntry { uint16_t address; uint16_t id; uint8_t linkQuality; uint8_t status; uint32_t lastUpdateMs; };
struct OfflineDetectorEntry { uint16_t detectorEntryIndex; };
struct AddressVerificationInterval { uint16_t min; uint16_t max; };
struct CompleteData { uint16_t address; uint8_t channel; uint16_t forwardAddress; uint8_t forwardChannel; uint8_t operationMode; boolean debugEnabled; };

#endif
```

#### floor-module/FloorModuleLib.h
```cpp
#ifndef FLOOR_MODULE_LIB_H
#define FLOOR_MODULE_LIB_H

#include "../common/Common.h"
#include "FloorModuleConfig.h"
#include <extEEPROM.h>
#include <LiquidCrystal_I2C.h>

// ==================== BUTTON CONTROLLER ====================
class ButtonController { /* ... */ };

// ==================== LED CONTROLLER ====================
class LEDController { /* ... */ };

// ==================== OUTPUT CONTROLLER ====================
class OutputController { /* ... */ };

// ==================== EEPROM CONTROLLER ====================
class EEPROMController { /* ... */ };

// ==================== SRAM CONTROLLER ====================
class MicrochipSRAM { /* ... */ };
class SRAMController { /* ... */ };

// ==================== SERVICES ====================
class MenuService { /* ... */ };
class CoreService { /* ... */ };
class ServiceMode { /* ... */ };
class ProgrammingService { /* ... */ };
class Startup { /* ... */ };

#endif
```

#### detector/DetectorConfig.h
```cpp
#ifndef DETECTOR_CONFIG_H
#define DETECTOR_CONFIG_H

// ==================== MAIN CONFIG ====================
#define APP_VERSION 25
#define DEVICE_TYPE 1
#define WATCHDOG_ENABLED true
#define SERIAL_BAUDRATE 19200
#define DEBUG_ENABLED false

// ==================== HARDWARE CONFIG ====================
#define HW_STATUS_LED_PIN 6
#define HW_OUTPUT_LED_RED_PIN 5
#define HW_OUTPUT_LED_YELLOW_PIN 4
#define HW_OUTPUT_LED_GREEN_PIN 9
#define HW_OUTPUT_BUZZER_PIN 7
#define HW_OUTPUT_RELAY_PIN 2
#define HW_INPUT_BTN_TEST_PIN 16
#define HW_TGS_SENSOR_PIN 17

#define BTN_COUNT 1
#define BTN_TEST 0

// ==================== TGS CONFIG ====================
#define TGS_STARTUP_DELAY_MS 60000
#define TGS_DEFAULT_ALERT_VALUE 406
#define TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM 6

// ==================== ALERT CONFIG ====================
#define ALARM_STATUS_MESSAGE_RESEND_MS_MIN 30000
#define ALARM_STATUS_MESSAGE_RESEND_MS_MAX 120000
#define ALARM_MESSAGE_MAX_RESEND_COUNT 5
#define ALARM_MIN_ACTIVE_PERIOD_MS 180000

// ==================== MODELS ====================
struct CompleteData { uint16_t address; uint8_t channel; uint16_t receiverAddress; uint16_t deviceId; int16_t alarmTrigger; boolean debugEnabled; };

#endif
```

### Comparație Variante

| Aspect | Varianta 1 (Completă) | Varianta 2 (Medium) | Varianta 3 (Minimală) |
|--------|----------------------|---------------------|----------------------|
| **Nr. fișiere** | ~50+ | ~20 | ~10 |
| **Complexitate** | Ridicată | Medie | Scăzută |
| **Granularitate** | Foarte bună | Bună | Limitată |
| **Timp implementare** | Lung | Mediu | Scurt |
| **Mentenanță** | Excelentă | Bună | Acceptabilă |
| **Curba învățare** | Abruptă | Moderată | Ușoară |
| **Refolosire cod** | Maximă | Bună | Limitată |
| **Compilare** | Mai lentă | Medie | Rapidă |

### Recomandare

- **Varianta 1 (Completă)**: Pentru echipe mari sau proiecte pe termen lung
- **Varianta 2 (Medium)**: Cel mai bun compromis între organizare și simplitate
- **Varianta 3 (Minimală)**: Pentru prototipare rapidă sau dezvoltatori singuri

### Pași Implementare Varianta 3

```bash
# 1. Creare structură
mkdir -p common floor-module detector

# 2. Creare Common.h
# - Copiază CC1101 defines din ambele fișiere
# - Copiază type unions
# - Copiază CCPACKET struct
# - Copiază SerialDebug inline

# 3. Pentru fiecare modul:
# - Extrage configurările în *Config.h
# - Extrage declarațiile în *Lib.h
# - Extrage implementările în *Lib.cpp
# - Păstrează setup()/loop() în *.ino
```
