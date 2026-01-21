# ERGO 2026 - Sistem Industrial de Detectie Gaze si Alarma

Firmware Arduino pentru un sistem industrial de detectie a gazelor si managementul alarmelor. Sistemul consta din doua componente principale: **Detectorul de Gaze** si **Modulul de Etaj (Floor Module)**.

## Cuprins

- [Descriere Generala](#descriere-generala)
- [Arhitectura Sistemului](#arhitectura-sistemului)
- [Structura Proiectului](#structura-proiectului)
- [Componente Hardware](#componente-hardware)
- [Comunicatie Wireless](#comunicatie-wireless)
- [Specificatii Tehnice](#specificatii-tehnice)
- [Clase si Servicii](#clase-si-servicii)
- [Structuri de Date](#structuri-de-date)
- [Configurare](#configurare)
- [Instalare si Utilizare](#instalare-si-utilizare)
- [Testare si Debug](#testare-si-debug)

---

## Descriere Generala

Acest proiect contine codul sursa pentru un sistem wireless de monitorizare si alarmare pentru detectia gazelor, destinat utilizarii in medii industriale sau comerciale.

Caracteristici principale:
- Detectie gaze cu senzori TGS
- Comunicatie wireless pe 868 MHz (CC1101)
- Management centralizat al alarmelor
- Interfata LCD pentru monitorizare
- Stocare persistenta in EEPROM/SRAM
- Watchdog pentru siguranta functionarii

---

## Arhitectura Sistemului

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         SISTEM ERGO 2026                                 │
└─────────────────────────────────────────────────────────────────────────┘

     ┌──────────────────────┐              ┌──────────────────────┐
     │   DETECTOR DE GAZE   │              │   MODUL DE ETAJ      │
     │   (Device Type: 1)   │              │   (Device Type: 2)   │
     │   Versiune: 2.5      │              │   Versiune: 4.0      │
     ├──────────────────────┤              ├──────────────────────┤
     │                      │              │                      │
     │  ┌────────────────┐  │   868 MHz    │  ┌────────────────┐  │
     │  │  Senzor TGS    │  │ ◄──────────► │  │  LCD Display   │  │
     │  └────────────────┘  │   CC1101     │  └────────────────┘  │
     │                      │   Wireless   │                      │
     │  ┌────────────────┐  │              │  ┌────────────────┐  │
     │  │  LED-uri RGB   │  │              │  │  Sistem Meniu  │  │
     │  │  (Semafor)     │  │              │  │  (4 butoane)   │  │
     │  └────────────────┘  │              │  └────────────────┘  │
     │                      │              │                      │
     │  ┌────────────────┐  │              │  ┌────────────────┐  │
     │  │    Buzzer      │  │              │  │  SRAM Externa  │  │
     │  └────────────────┘  │              │  │  (64KB)        │  │
     │                      │              │  └────────────────┘  │
     │  ┌────────────────┐  │              │                      │
     │  │  Releu AC      │  │              │  ┌────────────────┐  │
     │  └────────────────┘  │              │  │  Releu AC/DC   │  │
     │                      │              │  └────────────────┘  │
     │  ┌────────────────┐  │              │                      │
     │  │  Buton TEST    │  │              │  ┌────────────────┐  │
     │  └────────────────┘  │              │  │  EEPROM I2C    │  │
     │                      │              │  └────────────────┘  │
     └──────────────────────┘              └──────────────────────┘
```

### Flux de Date

```
Detector Gaze                                    Modul Etaj
     │                                                │
     │ 1. Citire senzor TGS                           │
     │ 2. Comparare cu prag alarma                    │
     │ 3. Activare alarma locala (LED, Buzzer)        │
     │ 4. Trimitere pachet radio ──────────────────►  │
     │                                                │ 5. Receptie si validare
     │                                                │ 6. Stocare in SRAM
     │                                                │ 7. Actualizare LCD
     │                                                │ 8. Activare releu (daca e cazul)
     │                                                │ 9. Forward la alt modul (slave)
```

---

## Structura Proiectului

Proiectul foloseste o arhitectura modulara cu cod partajat intre componente:

```
ergo2026/
│
├── README.md                           # Acest fisier
├── CLAUDE.md                           # Context pentru Claude Code
├── refactorizare.md                    # Documentatie refactorizare
│
├── ergo-floor-module-v4.0.cpp          # Cod sursa original Floor Module
├── fw-ergo-detector-source-v2.5.cpp    # Cod sursa original Detector
│
├── common/                             # Cod partajat
│   ├── Config.h                        # Configurari CC1101, tipuri, CCPACKET
│   ├── Controllers.h                   # Declaratii controllere comune
│   ├── Controllers.cpp                 # Implementari CC1101, Button, LED, etc.
│   ├── Services.h                      # Declaratii servicii comune
│   └── Services.cpp                    # SerialDebug, CommunicationService
│
├── floor-module/                       # Cod specific Floor Module
│   ├── Config.h                        # Configurari: Main, HW, EEPROM, SRAM, Menu
│   ├── Models.h                        # Structuri de date
│   ├── Controllers.h                   # MicrochipSRAM, SRAMController
│   ├── Controllers.cpp                 # Implementari controllere
│   ├── Services.h                      # Menu, Core, Programming services
│   ├── Services.cpp                    # Implementari servicii
│   └── ergo-floor-module.ino           # Fisier principal Arduino
│
└── detector/                           # Cod specific Detector
    ├── Config.h                        # Configurari: Main, HW, TGS, Alert, Buzzer
    ├── Models.h                        # Structuri de date
    ├── Controllers.h                   # Buzzer, TGS controllers
    ├── Controllers.cpp                 # Implementari controllere
    ├── Services.h                      # Alarm, Programming services
    ├── Services.cpp                    # Implementari servicii
    └── fw-ergo-detector.ino            # Fisier principal Arduino
```

---

## Componente Hardware

### 1. Detector de Gaze (`fw-ergo-detector-source-v2.5.cpp`)

| Parametru | Valoare |
|-----------|---------|
| **Versiune** | 2.5 |
| **Tip Dispozitiv** | 1 |
| **Microcontroler** | AVR (Arduino compatibil) |

**Pinout Hardware:**

| Pin | Functie | Descriere |
|-----|---------|-----------|
| 2 | HW_OUTPUT_RELAY_PIN | Iesire releu AC |
| 4 | HW_OUTPUT_LED_YELLOW_PIN | LED galben (avertizare) |
| 5 | HW_OUTPUT_LED_RED_PIN | LED rosu (alarma) |
| 6 | HW_STATUS_LED_PIN | LED status sistem |
| 7 | HW_OUTPUT_BUZZER_PIN | Buzzer alarma |
| 9 | HW_OUTPUT_LED_GREEN_PIN | LED verde (OK) |
| 16 | HW_INPUT_BTN_TEST_PIN | Buton TEST |
| 17 | HW_TGS_SENSOR_PIN | Senzor gaz TGS |

**Functionalitati:**
- Senzor TGS pentru detectia gazelor
- Sistem de alarma cu valori de declansare configurabile
- Indicatori LED de stare (semafor RGB):
  - `OK` - Verde - Functionare normala
  - `WARNING` - Galben - Avertizare
  - `ERROR` - Rosu - Eroare/Alarma
  - `COMM_ERROR` - Rosu intermitent - Eroare comunicatie
- Buzzer pentru alarme sonore (interval 400ms)
- Iesire releu (RELAY_AC) pentru echipamente externe
- Buton de test manual
- Mod de service/programare prin serial

### 2. Modul de Etaj (`ergo-floor-module-v4.0.cpp`)

| Parametru | Valoare |
|-----------|---------|
| **Versiune** | 4.0 |
| **Tip Dispozitiv** | 2 |
| **Microcontroler** | AVR (Arduino compatibil) |

**Pinout Hardware:**

| Pin | Functie | Descriere |
|-----|---------|-----------|
| 4 | HW_INPUT_BTN_LEFT_PIN | Buton LEFT (navigare) |
| 5 | HW_INPUT_BTN_BACK_PIN | Buton BACK (inapoi) |
| 6 | HW_STATUS_LED_PIN | LED status sistem |
| 7 | HW_OUTPUT_RELAY_DC_PIN | Iesire releu DC |
| 9 | HW_OUTPUT_RELAY_AC_PIN | Iesire releu AC |
| 14 | HW_IO_NC_PIN | Pin neconectat |
| 15 | HW_SPI_CS_SRAM_PIN | Chip Select SRAM |
| 16 | HW_INPUT_BTN_OK_PIN | Buton OK (confirmare) |
| 17 | HW_INPUT_BTN_RIGHT_PIN | Buton RIGHT (navigare) |

**Functionalitati:**
- Unitate centrala de receptie/control
- Afisaj LCD I2C (16x2 caractere, adresa 0x27)
- 4 butoane de navigare (BACK, LEFT, RIGHT, OK)
- Management al pana la 4800 detectoare simultan
- Monitorizare detectoare offline (timeout 7 minute)
- Afisare si gestionare alarme (pana la 1210 alarme in SRAM)
- Stocare persistenta in EEPROM extern I2C
- SRAM externa 64KB (Microchip) pentru date runtime
- Moduri de operare: MASTER, SLAVE
- Forwarding mesaje intre module

---

## Comunicatie Wireless

Ambele dispozitive utilizeaza modulul radio **CC1101** pentru comunicatie wireless.

### Parametri Radio

| Parametru | Valoare |
|-----------|---------|
| Frecventa | 868 MHz |
| Modulatie | 2-FSK |
| Rata date | ~38.4 kbps |
| Interfata | SPI |
| Interrupt | Pin 1 (GDO0) |
| GDO2 | Pin 3 |

### Sync Words

| Registru | Valoare | Hex |
|----------|---------|-----|
| SYNC1 | 181 | 0xB5 |
| SYNC0 | 71 | 0x47 |

### Format Pachet (CCPACKET)

```cpp
struct CCPACKET {
    uint8_t length;                          // Lungime date
    uint8_t data[CC1101_MAX_BUFFER_SIZE];    // Date (max 30 bytes)
    boolean crc_ok;                          // Validare CRC
    int8_t rssi;                             // Putere semnal
    uint8_t lqi;                             // Calitate link
};
```

### Niveluri Putere Transmisie

| Nivel | Valoare | Utilizare |
|-------|---------|-----------|
| PA_LowPower | 0x60 | Distante scurte |
| PA_LongDistance | 0xC0 | Distante mari |

---

## Specificatii Tehnice

### Timere si Intervale

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `WATCHDOG_RECEIVE_RESET_MS` | 300000 ms (5 min) | Reset watchdog la receptie |
| `DETECTOR_OFFLINE_MS` | 420000 ms (7 min) | Timp pana detector e marcat offline |
| `BTN_DEBOUNCE_PERIOD` | 40 ms | Perioada debounce butoane |
| `ALARM_STATUS_MESSAGE_RESEND_MS_MIN` | 30000 ms (30 sec) | Interval minim retrimitere status |
| `ALARM_STATUS_MESSAGE_RESEND_MS_MAX` | 120000 ms (2 min) | Interval maxim retrimitere status |
| `ALARM_MIN_ACTIVE_PERIOD_MS` | 180000 ms (3 min) | Perioada minima alarma activa |
| `TGS_STARTUP_DELAY_MS` | 60000 ms (1 min) | Delay pornire senzor TGS |
| `MENU_TIMEOUT_MS` | 120000 ms (2 min) | Timeout revenire meniu principal |
| `MENU_BACKLIGHT_OFF_MS` | 60000 ms (1 min) | Timeout oprire backlight LCD |
| `MENU_VALUE_REFRESH_MS` | 3000 ms (3 sec) | Interval refresh valori meniu |
| `BEEPING_INTERVAL_MS` | 400 ms | Interval bip buzzer |

### Configurare Senzor TGS

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `TGS_DEFAULT_ALERT_VALUE` | 406 | Prag alarma default |
| `TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM` | 6 | Citiri consecutive pentru alarma |

### Configurare Memorie SRAM (Floor Module)

| Zona | Start | End | Segment | Capacitate |
|------|-------|-----|---------|------------|
| Alarme | 0 | 6050 | 5 bytes | 1210 intrari |
| Detectoare | 6050 | 54050 | 10 bytes | 4800 intrari |
| Detectoare Offline | 54050 | 63650 | 2 bytes | 4800 intrari |

### Configurare Memorie EEPROM

**Detector:**
| Adresa | Continut |
|--------|----------|
| 0-3 | Numar serial |
| 10 | Flag pornire |
| 50-51 | Valoare declansare alarma |
| 52-53 | ID dispozitiv |
| 100-101 | Adresa CC1101 |
| 102 | Canal CC1101 |
| 103-104 | Adresa receptor |

**Floor Module:**
| Adresa | Continut |
|--------|----------|
| 0-3 | Numar serial |
| 10 | Flag pornire |
| 50 | Mod operare |
| 100-101 | Adresa dispozitiv |
| 102 | Canal dispozitiv |
| 103-104 | Adresa forward |
| 105 | Canal forward |
| 109-110 | Index/Count alarme |
| 111-210 | Memorie alarme |
| 250-291 | Intervale verificare adrese |

---

## Clase si Servicii

### Clase Comune (common/)

| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `ButtonController` | Controllers.cpp | Gestionare butoane cu debounce |
| `CC1101Controller` | Controllers.cpp | Control modul radio CC1101 |
| `EEPROMController` | Controllers.cpp | Gestionare memorie EEPROM I2C |
| `LEDController` | Controllers.cpp | Control LED-uri de stare |
| `OutputController` | Controllers.cpp | Control iesiri releu |
| `SerialDebug` | Services.cpp | Debug conditionat pe serial |
| `CommunicationService` | Services.cpp | Serviciu comunicatie radio |

### Clase Floor Module (floor-module/)

| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `MicrochipSRAM` | Controllers.cpp | Driver SRAM Microchip |
| `SRAMController` | Controllers.cpp | Gestionare date in SRAM |
| `MenuService` | Services.cpp | Sistem meniu LCD navigabil |
| `CoreService` | Services.cpp | Logica principala procesare |
| `ServiceMode` | Services.cpp | Mod testare/service |
| `ProgrammingService` | Services.cpp | Programare prin serial |
| `Startup` | Services.cpp | Initializare la pornire |

### Clase Detector (detector/)

| Clasa | Fisier | Descriere |
|-------|--------|-----------|
| `BuzzerController` | Controllers.cpp | Control buzzer alarma |
| `TGSController` | Controllers.cpp | Interfata senzor gaz TGS |
| `AlarmService` | Services.cpp | Gestionare stari alarma |
| `ServiceMode` | Services.cpp | Mod testare cu semafor |
| `ProgrammingService` | Services.cpp | Programare prin serial |
| `Startup` | Services.cpp | Initializare la pornire |

---

## Structuri de Date

### AlarmEntry (Floor Module)
```cpp
struct AlarmEntry {
    uint16_t address;      // Adresa detector sursa
    uint16_t id;           // ID alarma
    boolean state;         // Stare activa/inactiva
};
```

### DetectorEntry (Floor Module)
```cpp
struct DetectorEntry {
    uint16_t address;      // Adresa detector
    uint16_t id;           // ID detector
    uint8_t linkQuality;   // Calitate semnal (%)
    uint8_t status;        // Status curent
    uint32_t lastUpdateMs; // Timestamp ultima actualizare
};
```

### OfflineDetectorEntry (Floor Module)
```cpp
struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;  // Index in lista detectoare
};
```

### AddressVerificationInterval (Floor Module)
```cpp
struct AddressVerificationInterval {
    uint16_t min;          // Adresa minima
    uint16_t max;          // Adresa maxima
};
```

### CompleteData - Detector
```cpp
struct CompleteData {
    uint16_t address;           // Adresa dispozitiv
    uint8_t channel;            // Canal radio
    uint16_t receiverAddress;   // Adresa receptor
    uint16_t deviceId;          // ID dispozitiv
    int16_t alarmTrigger;       // Prag alarma
    boolean debugEnabled;       // Mod debug activ
};
```

### CompleteData - Floor Module
```cpp
struct CompleteData {
    uint16_t address;           // Adresa dispozitiv
    uint8_t channel;            // Canal radio
    uint16_t forwardAddress;    // Adresa forwarding
    uint8_t forwardChannel;     // Canal forwarding
    uint8_t operationMode;      // Mod operare (MASTER/SLAVE)
    boolean debugEnabled;       // Mod debug activ
};
```

### Union-uri Conversie Bytes
```cpp
union ConvertBytesToLong  { uint8_t byte[4]; int32_t longVal; };
union ConvertBytesToULong { uint8_t byte[4]; uint32_t longVal; };
union ConvertBytesToInt   { uint8_t byte[2]; int16_t intVal; };
union ConvertBytesToUInt  { uint8_t byte[2]; uint16_t intVal; };
```

---

## Configurare

### Constante Principale

```cpp
// Floor Module
#define APP_VERSION 40              // Versiune firmware
#define DEVICE_TYPE 2               // Tip dispozitiv
#define SERIAL_BAUDRATE 19200       // Viteza serial
#define WATCHDOG_ENABLED true       // Watchdog activ
#define DEBUG_ENABLED false         // Debug dezactivat in productie

// Detector
#define APP_VERSION 25              // Versiune firmware
#define DEVICE_TYPE 1               // Tip dispozitiv
#define DEFAULT_DEVICE_ID 65000     // ID default
#define DEFAULT_RECEIVER_ADDRESS 1  // Adresa receptor default
```

### Moduri Operare Floor Module

| Mod | Valoare | Descriere |
|-----|---------|-----------|
| MASTER | 1 | Receptor principal, afiseaza alarme |
| SLAVE | 2 | Receptor secundar, primeste forward |

### Comenzi Programare Serial

**Detector:**
| ID | Comanda | Descriere |
|----|---------|-----------|
| 1 | SET_DEVICE_ADDRESS | Seteaza adresa dispozitiv |
| 2 | SET_DEVICE_CHANNEL | Seteaza canal radio |
| 3 | SET_RECEIVER_ADDRESS | Seteaza adresa receptor |
| 4 | SET_DEVICE_ID | Seteaza ID dispozitiv |
| 5 | SET_ALARM_TRIGGER | Seteaza prag alarma |
| 6 | SET_DEBUG_MODE | Activeaza/dezactiveaza debug |
| 10 | GET_DEVICE_TYPE | Returneaza tipul dispozitivului |
| 20 | GET_ALL_VALUES | Returneaza toate valorile |
| 21 | SET_ALL_VALUES | Seteaza toate valorile |
| 101 | TEST_MODE | Intra in mod test |

**Floor Module:**
| ID | Comanda | Descriere |
|----|---------|-----------|
| 1 | SET_DEVICE_ADDRESS | Seteaza adresa dispozitiv |
| 2 | SET_DEVICE_CHANNEL | Seteaza canal radio |
| 3 | SET_FORWARD_ADDRESS | Seteaza adresa forwarding |
| 4 | SET_FORWARD_CHANNEL | Seteaza canal forwarding |
| 5 | SET_OPERATION_MODE | Seteaza mod operare |
| 6 | SET_DEBUG_MODE | Activeaza/dezactiveaza debug |
| 10 | GET_DEVICE_TYPE | Returneaza tipul dispozitivului |
| 20 | GET_ALL_VALUES | Returneaza toate valorile |
| 21 | SET_ALL_VALUES | Seteaza toate valorile |
| 30 | STREAM_ALL_MEMORY | Stream continut memorie |
| 31 | CLEAR_ALARMS | Sterge toate alarmele |
| 32 | CLEAR_DETECTORS | Sterge toate detectoarele |
| 40 | ADD_ADDRESS_VERIFICATION_INTERVAL | Adauga interval verificare |
| 41 | CLEAR_ADDRESS_VERIFICATION_INTERVALS | Sterge intervalele |
| 101 | TEST_MODE | Intra in mod test |

---

## Instalare si Utilizare

### Cerinte

- Arduino IDE 1.8+ sau PlatformIO
- Placa AVR compatibila Arduino
- Biblioteci necesare:
  - `Arduino.h` - Core Arduino
  - `SPI.h` - Comunicatie SPI
  - `avr/wdt.h` - Watchdog Timer
  - `extEEPROM.h` - EEPROM extern I2C
  - `LiquidCrystal_I2C.h` - LCD I2C (doar Floor Module)

### Pasi Instalare

1. **Clonare repository:**
   ```bash
   git clone https://github.com/darieionut/ergo2026.git
   cd ergo2026
   ```

2. **Instalare biblioteci (Arduino IDE):**
   - Sketch -> Include Library -> Manage Libraries
   - Cauta si instaleaza: `extEEPROM`, `LiquidCrystal_I2C`

3. **Selectare placa:**
   - Tools -> Board -> Arduino AVR Boards -> (selecteaza placa ta)

4. **Compilare si Upload:**
   - Pentru Floor Module: deschide `floor-module/ergo-floor-module.ino`
   - Pentru Detector: deschide `detector/fw-ergo-detector.ino`
   - Click Upload

### Utilizare PlatformIO

```bash
# Floor Module
cd floor-module
pio run -t upload

# Detector
cd detector
pio run -t upload
```

---

## Testare si Debug

### Mod Test Detector

1. Apasa butonul **TEST** pe detector
2. Detectorul va simula o alarma
3. LED-urile vor indica starea de alarma
4. Buzzerul va suna
5. Mesajul va fi transmis catre Floor Module

### Mod Test Floor Module

1. Trimite comanda serial `101` pentru a intra in mod test
2. Verifica afisajul LCD
3. Testeaza navigarea cu butoanele
4. Verifica comunicatia cu detectoarele

### Monitorizare Serial

```bash
# Linux/Mac
screen /dev/ttyUSB0 19200

# Windows (folosind PuTTY sau Arduino Serial Monitor)
# Baud rate: 19200
```

### Activare Debug

Modifica `DEBUG_ENABLED` in fisierul Config.h corespunzator:
```cpp
#define DEBUG_ENABLED true  // Activeaza mesaje debug
```

**Atentie:** Dezactiveaza debug-ul in productie pentru performanta optima.

### Note Importante pentru Dezvoltare

1. **Watchdog:** Este activ - codul trebuie sa execute `wdt_reset()` periodic
2. **EEPROM:** Folosit pentru stocare persistenta - nu suprascrie datele fara backup
3. **Interrupt-uri:** Pachetele radio sunt semnalate prin interrupt pe pin 1
4. **Variabile volatile:** `packetWaiting` trebuie sa ramana `volatile`

---

## Autori

**Energo Instal**

---

## Licenta

Proprietar - Toate drepturile rezervate.

---

## Versiuni

| Componenta | Versiune | Data |
|------------|----------|------|
| Floor Module | 4.0 | 2024 |
| Detector | 2.5 | 2024 |
| Documentatie | 1.0 | 2026 |
