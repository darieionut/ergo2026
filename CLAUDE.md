# CLAUDE.md - Ghid Complet pentru Proiectul ERGO26

---

## 1. Introducere și Scop

### 1.1 Scopul Documentului CLAUDE.md

Acest document servește ca ghid principal pentru asistentul AI (Claude) în înțelegerea, navigarea și contribuția la proiectul ERGO26. Conține informații tehnice detaliate, convenții de cod, arhitectură și instrucțiuni specifice pentru dezvoltare.

**Utilizare principală:**
- Context rapid pentru AI Assistant
- Referință tehnică pentru dezvoltatori
- Documentație de arhitectură
- Ghid pentru mentenanță și extindere

### 1.2 Rolul AI-ului în Proiectul ERGO26

AI Assistant (Claude) poate fi utilizat pentru:
- **Analiză cod:** Înțelegerea fluxurilor și identificarea problemelor
- **Dezvoltare:** Implementarea de noi funcționalități respectând convențiile
- **Debugging:** Identificarea și rezolvarea bug-urilor
- **Refactorizare:** Îmbunătățirea structurii codului
- **Documentare:** Generarea de comentarii și documentație tehnică
- **Code Review:** Verificarea conformității cu standardele proiectului

---

## 2. Prezentare Generală a Proiectului

### 2.1 Ce este ERGO26

ERGO26 este un **sistem industrial de detectare a gazelor și alarmare**, compus din firmware Arduino pentru două tipuri de dispozitive hardware care comunică wireless. Proiectul implementează o soluție completă de monitorizare și alertare pentru medii industriale.

### 2.2 Domeniu de Utilizare

- **Industrie:** Monitorizarea scurgerilor de gaze în spații industriale
- **Clădiri comerciale:** Sisteme de siguranță pentru etaje multiple
- **Infrastructură:** Protecția spațiilor cu risc de acumulare gaze

### 2.3 Obiective Principale ale Sistemului

| Obiectiv | Descriere |
|----------|-----------|
| **Detectare** | Identificarea rapidă a concentrațiilor periculoase de gaze |
| **Alertare** | Notificarea imediată prin alarme vizuale, sonore și relee |
| **Centralizare** | Agregarea datelor de la multiple detectoare într-un singur punct |
| **Fiabilitate** | Funcționare continuă cu mecanisme de watchdog și retry |
| **Scalabilitate** | Suport pentru rețele mari de detectoare prin forwarding |

---

## 3. Informații Cheie ale Proiectului

| Aspect | Detaliu |
|--------|---------|
| **Limbaj de programare** | C++ pentru Arduino (AVR) |
| **Platformă hardware** | Microcontrolere AVR (Arduino compatible) |
| **Tip comunicație** | Wireless RF 868 MHz (modul CC1101) |
| **Stil arhitectural** | Controller-Service Pattern |
| **Limba documentației** | Română |
| **Versiune Detector** | 2.5 |
| **Versiune Floor Module** | 4.0 |

---

## 4. Structura Repository-ului

### 4.1 Structura Directoarelor

```
ergo2026/
├── common/                              # Cod partajat între dispozitive
│   ├── Config.h                         # CC1101, Types, CCPACKET, LED, Output configs
│   ├── Controllers.h/.cpp               # ButtonController, CC1101Controller, EEPROMController,
│   │                                    # LEDController, OutputController
│   └── Services.h/.cpp                  # SerialDebug, CommunicationService
│
├── detector/                            # Firmware Detector Gaze (DEVICE_TYPE=1)
│   ├── Config.h                         # Main, Hardware, EEPROM, Alert, Buzzer, TGS configs
│   ├── Models.h                         # CompleteData struct
│   ├── Controllers.h/.cpp               # BuzzerController, TGSController, SemaphoreLEDController
│   ├── Services.h/.cpp                  # AlarmService, ServiceMode, ProgrammingService, Startup
│   └── fw-ergo-detector.ino             # Entry point Arduino
│
├── floor-module/                        # Firmware Modul Etaj (DEVICE_TYPE=2)
│   ├── Config.h                         # Main, Hardware, EEPROM, SRAM, Menu configs
│   ├── Models.h                         # AlarmEntry, DetectorEntry, OfflineDetectorEntry, etc.
│   ├── Controllers.h/.cpp               # MicrochipSRAM, SRAMController
│   ├── Services.h/.cpp                  # MenuService, CoreService, ServiceMode, ProgrammingService
│   └── ergo-floor-module.ino            # Entry point Arduino
│
├── ergo-floor-module-v4.0.cpp           # Versiune monolitică (legacy)
├── fw-ergo-detector-source-v2.5.cpp     # Versiune monolitică (legacy)
├── refactorizare.md                     # Documentație refactorizare
├── CLAUDE.md                            # Acest document
└── README.md                            # Documentație generală
```

### 4.2 Fișiere Principale

| Fișier | Tip | Descriere |
|--------|-----|-----------|
| `fw-ergo-detector.ino` | Entry Point | Punct de intrare pentru firmware detector |
| `ergo-floor-module.ino` | Entry Point | Punct de intrare pentru firmware modul etaj |
| `common/Config.h` | Configurare | Definiții CC1101, tipuri, constante partajate |
| `common/Controllers.cpp` | Implementare | Controlere hardware comune |
| `common/Services.cpp` | Implementare | Servicii de comunicare și debug |

### 4.3 Fișiere Legacy (Versiuni Monolitice)

| Fișier | Versiune | Linii Cod | Status |
|--------|----------|-----------|--------|
| `ergo-floor-module-v4.0.cpp` | 4.0 | 3869 | Legacy - doar pentru referință |
| `fw-ergo-detector-source-v2.5.cpp` | 2.5 | 2725 | Legacy - doar pentru referință |

### 4.4 Documentație Existentă

| Document | Scop |
|----------|------|
| `CLAUDE.md` | Context pentru AI Assistant și documentație tehnică |
| `README.md` | Prezentare generală și instrucțiuni de utilizare |
| `refactorizare.md` | Plan detaliat de refactorizare a codului |

---

## 5. Arhitectura Sistemului

### 5.1 Viziune de Ansamblu

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         SISTEM ERGO26                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│   ┌─────────────┐        868 MHz         ┌─────────────────────┐        │
│   │  Detector   │ ──────────────────────▶│   Floor Module      │        │
│   │  (Type 1)   │   CC1101 Wireless      │      (Type 2)       │        │
│   │             │                         │                     │        │
│   │  ┌───────┐  │                         │  ┌───────────────┐  │        │
│   │  │ TGS   │  │                         │  │   LCD 16x2    │  │        │
│   │  │Sensor │  │                         │  │   Display     │  │        │
│   │  └───────┘  │                         │  └───────────────┘  │        │
│   │  ┌───────┐  │                         │  ┌───────────────┐  │        │
│   │  │Buzzer │  │                         │  │   4 Buttons   │  │        │
│   │  └───────┘  │                         │  │ Menu Control  │  │        │
│   │  ┌───────┐  │                         │  └───────────────┘  │        │
│   │  │LED RGB│  │                         │  ┌───────────────┐  │        │
│   │  │Semafor│  │                         │  │  External     │  │        │
│   │  └───────┘  │                         │  │  SRAM 64KB    │  │        │
│   │  ┌───────┐  │                         │  └───────────────┘  │        │
│   │  │Relay  │  │                         │  ┌───────────────┐  │        │
│   │  │  AC   │  │                         │  │  2x Relays    │  │        │
│   │  └───────┘  │                         │  │   AC / DC     │  │        │
│   │             │                         │  └───────────────┘  │        │
│   └─────────────┘                         └──────────┬──────────┘        │
│                                                       │                  │
│                                                       │ Forwarding       │
│                                                       │ (Slave Mode)     │
│                                                       ▼                  │
│                                           ┌─────────────────────┐        │
│                                           │  Alt Floor Module   │        │
│                                           │   (Master/Slave)    │        │
│                                           └─────────────────────┘        │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### 5.2 Fluxul de Comunicare

```
┌──────────────────────────────────────────────────────────────────────────┐
│                         FLUX COMUNICARE                                   │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│   1. DETECTARE GAZ                                                        │
│      Detector: TGS Sensor → citire analogică → comparare prag             │
│                                                                           │
│   2. CONFIRMARE ALARMĂ                                                    │
│      Detector: 6 măsurători consecutive > prag = ALARMĂ CONFIRMATĂ        │
│                                                                           │
│   3. TRANSMITERE RF                                                       │
│      Detector → CC1101 → pachet RF → CC1101 → Floor Module                │
│                                                                           │
│   4. PROCESARE                                                            │
│      Floor Module: recepție → validare CRC → stocare SRAM → afișare LCD   │
│                                                                           │
│   5. FORWARDING (dacă Slave Mode)                                         │
│      Floor Module Slave → retransmitere → Floor Module Master             │
│                                                                           │
│   6. ACȚIUNI LOCALE                                                       │
│      - Activare relee (AC/DC)                                             │
│      - Actualizare stare detectoare                                       │
│      - Marcare detectoare offline (timeout 7 min)                         │
│                                                                           │
└──────────────────────────────────────────────────────────────────────────┘
```

---

## 6. Tipuri de Dispozitive

### 6.1 Detector (DEVICE_TYPE = 1)

#### Rol și Responsabilități

- **Monitorizare continuă** a nivelului de gaz prin senzorul TGS
- **Detectare automată** a concentrațiilor periculoase
- **Alertare locală** prin buzzer și LED-uri semafor
- **Transmitere RF** a stării către Floor Module
- **Activare releu** pentru sisteme externe de ventilație/siguranță

#### Funcționare Autonomă

```
┌────────────────────────────────────────────────────────────────┐
│                    CICLU DE FUNCȚIONARE DETECTOR                │
├────────────────────────────────────────────────────────────────┤
│                                                                 │
│   PORNIRE                                                       │
│      │                                                          │
│      ▼                                                          │
│   ┌──────────────────┐                                          │
│   │ Încălzire senzor │  (60 secunde - TGS_STARTUP_DELAY_MS)     │
│   │   LED GALBEN     │                                          │
│   └────────┬─────────┘                                          │
│            │                                                    │
│            ▼                                                    │
│   ┌──────────────────┐                                          │
│   │ Mod Normal       │                                          │
│   │   LED VERDE      │◄─────────────────────────┐               │
│   └────────┬─────────┘                          │               │
│            │                                    │               │
│            ▼                                    │               │
│   ┌──────────────────┐                          │               │
│   │ Citire TGS       │                          │               │
│   │ (continuu)       │                          │               │
│   └────────┬─────────┘                          │               │
│            │                                    │               │
│            ▼                                    │               │
│      ┌───────────┐                              │               │
│      │ Valoare > │───NO────────────────────────►│               │
│      │   prag?   │                              │               │
│      └─────┬─────┘                              │               │
│            │YES                                 │               │
│            ▼                                    │               │
│   ┌──────────────────┐                          │               │
│   │ Incrementare     │                          │               │
│   │ contor alarme    │                          │               │
│   └────────┬─────────┘                          │               │
│            │                                    │               │
│            ▼                                    │               │
│      ┌───────────┐                              │               │
│      │ Contor >= │───NO────────────────────────►│               │
│      │    6 ?    │                              │               │
│      └─────┬─────┘                              │               │
│            │YES                                 │               │
│            ▼                                    │               │
│   ┌──────────────────┐                          │               │
│   │ STARE ALARMĂ     │                          │               │
│   │ - LED ROȘU       │                          │               │
│   │ - Buzzer ON      │                          │               │
│   │ - Releu AC ON    │                          │               │
│   │ - Transmit RF    │                          │               │
│   └────────┬─────────┘                          │               │
│            │                                    │               │
│            ▼                                    │               │
│      ┌───────────┐                              │               │
│      │ Valoare < │───NO───┐                     │               │
│      │   prag?   │        │                     │               │
│      └─────┬─────┘        │                     │               │
│            │YES           │                     │               │
│            ▼              │                     │               │
│      ┌───────────┐        │                     │               │
│      │ Timeout   │◄───────┘                     │               │
│      │ 3 min ?   │                              │               │
│      └─────┬─────┘                              │               │
│            │YES                                 │               │
│            ▼                                    │               │
│   ┌──────────────────┐                          │               │
│   │ Oprire alarmă    │                          │               │
│   │ - Transmit stop  │──────────────────────────┘               │
│   └──────────────────┘                                          │
│                                                                 │
└────────────────────────────────────────────────────────────────┘
```

#### Interacțiuni RF

| Eveniment | Acțiune RF | Retrimiteri |
|-----------|------------|-------------|
| Pornire alarmă | `sendAlarmStartPackage()` | Max 5, interval 30s-2min |
| Oprire alarmă | `sendAlarmStopPackage()` | Max 5, interval 30s-2min |
| Heartbeat | Status periodic | N/A |

### 6.2 Floor Module / Modul Etaj (DEVICE_TYPE = 2)

#### Rol de Hub Central

- **Recepție RF** de la toate detectoarele din rază
- **Agregare date** - menține lista detectoarelor și alarmelor
- **Afișare status** pe LCD 16x2
- **Interfață utilizator** prin meniu și 4 butoane
- **Stocare persistentă** în EEPROM și SRAM extern
- **Forwarding** (în mod Slave) către alt Floor Module

#### Moduri de Operare

| Mod | Valoare | Descriere | Utilizare |
|-----|---------|-----------|-----------|
| `OPERATION_MODE_MASTER` | 1 | Unitate principală | Primește direct de la detectoare, procesează și afișează |
| `OPERATION_MODE_SLAVE` | 2 | Repetor/Extender | Primește de la detectoare ȘI retransmite către Master |

#### Interfață Utilizator

```
┌────────────────────────────────────────┐
│           LCD DISPLAY 16x2             │
│  ┌──────────────────────────────────┐  │
│  │ ERGO        OK     │ Linia 1     │  │
│  │ 0 alarms    3 det  │ Linia 2     │  │
│  └──────────────────────────────────┘  │
│                                        │
│     [BACK]  [LEFT]  [RIGHT]  [OK]      │
│                                        │
│  BACK: Înapoi în meniu                 │
│  LEFT: Navigare stânga / decrementare  │
│  RIGHT: Navigare dreapta / incrementare│
│  OK: Selectare / confirmare            │
│                                        │
└────────────────────────────────────────┘
```

---

## 7. Specificații Tehnice

### 7.1 Microcontroler

| Specificație | Detaliu |
|--------------|---------|
| **Arhitectură** | AVR 8-bit |
| **Compatibilitate** | Arduino |
| **Watchdog** | Activat (timeout configurabil) |
| **Framework** | Arduino.h |

### 7.2 Comunicație RF (CC1101)

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| **Frecvență** | 868 MHz | Bandă ISM Europa |
| **Modul** | CC1101 | Texas Instruments |
| **Interfață** | SPI | Pin SS standard |
| **Interrupt** | Pin 3 (GDO2) | Notificare pachet primit |
| **Sync Words** | 0xB5, 0x47 | Identificare protocol ERGO |
| **Buffer TX** | 15 bytes max | Dimensiune maximă pachet transmisie |
| **Buffer RX** | 100 bytes | Buffer recepție |
| **PA Level** | 0xC0 | Long Distance mode |
| **Modulație** | 2-FSK | Default CC1101 |

### 7.3 Sisteme de Memorie

#### EEPROM Intern/Extern (I2C)

| Caracteristică | Valoare |
|----------------|---------|
| **Page Size** | 16 bytes |
| **Adresă I2C** | Standard EEPROM |
| **Utilizare** | Configurări persistente, alarme backup |

#### SRAM Extern (doar Floor Module)

| Zonă | Start | End | Segment Size | Capacitate |
|------|-------|-----|--------------|------------|
| Alarme | 0 | 6050 | 5 bytes | ~1210 alarme |
| Detectoare | 6050 | 54050 | 10 bytes | ~4800 detectoare |
| Offline | 54050 | 63650 | 2 bytes | ~4800 indecși |

### 7.4 Afișaj și Interfață (Floor Module)

| Parametru | Valoare |
|-----------|---------|
| **Tip** | LCD Character |
| **Dimensiune** | 16 caractere x 2 rânduri |
| **Interfață** | I2C |
| **Adresă I2C** | 0x27 |
| **Backlight** | Auto-off după 60s inactivitate |
| **Timeout meniu** | 120s (revenire la HOME) |

### 7.5 Senzori și Ieșiri

#### Detector

| Componentă | Pin | Funcție |
|------------|-----|---------|
| **Senzor TGS** | A17 (analog) | Detectare gaz |
| **LED Roșu** | D5 | Alarmă activă |
| **LED Galben** | D4 | Încălzire/Warning |
| **LED Verde** | D9 | Status OK |
| **Buzzer** | D7 | Alarmă sonoră |
| **Releu AC** | D2 | Ieșire externă |
| **Buton TEST** | D16 | Testare manuală |
| **LED Status** | D6 | Comunicare/Status |

#### Floor Module

| Componentă | Pin | Funcție |
|------------|-----|---------|
| **Releu AC** | D9 | Ieșire alarmă AC |
| **Releu DC** | D7 | Ieșire alarmă DC |
| **Buton BACK** | D5 | Navigare meniu |
| **Buton LEFT** | D4 | Navigare meniu |
| **Buton RIGHT** | D17 | Navigare meniu |
| **Buton OK** | D16 | Confirmare meniu |
| **SRAM CS** | D15 | Chip Select SRAM |
| **LED Status** | D6 | Comunicare/Status |

---

## 8. Parametri de Rețea

### 8.1 Adresare Dispozitive

| Parametru | Tip | Range | Descriere |
|-----------|-----|-------|-----------|
| **Device Address** | uint16_t | 1-65535 | Adresa unică CC1101 |
| **Device Channel** | uint8_t | 0-255 | Canal RF |
| **Device ID** | uint16_t | 1-65534 | ID logic detector |
| **Receiver Address** | uint16_t | 1-65535 | Adresa Floor Module țintă |

### 8.2 Canale RF

- **Canal default:** 0
- **Range:** 0-255
- **Recomandare:** Toate dispozitivele din aceeași rețea pe același canal

### 8.3 Timeout Detectori

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `DETECTOR_OFFLINE_MS` | 420000 (7 min) | Timp până detector e marcat offline |
| `WATCHDOG_RECEIVE_RESET_MS` | 300000 (5 min) | Timeout watchdog recepție |

### 8.4 Evaluarea Calității Legăturii

```cpp
// LQI (Link Quality Indicator)
// Valoare 0-100% calculată din registrul CC1101
int computeLqi(uint8_t raw) {
    // raw & 0x7F normalizat la procent
    return ((raw & 0x7F) * 100) / 127;
}

// RSSI (Received Signal Strength Indicator)
// Convertit din valoare brută CC1101 în dBm
int computeRssi(uint8_t rssi_dec) {
    // Formulă specifică CC1101
}
```

---

## 9. Protocol de Comunicație

### 9.1 Formatul Mesajelor

```cpp
struct CCPACKET {
    uint8_t length;                    // Lungime date
    uint8_t data[CC1101_MAX_BUFFER_SIZE]; // Date utile (max 30 bytes)
    boolean crc_ok;                    // Validare CRC
    int8_t rssi;                       // Putere semnal
    uint8_t lqi;                       // Calitate legătură
};
```

#### Structura Pachetului de Date

| Offset | Dimensiune | Conținut |
|--------|------------|----------|
| 0 | 2 bytes | Adresa sursă (uint16_t) |
| 2 | 2 bytes | ID dispozitiv (uint16_t) |
| 4 | 1 byte | Tip mesaj |
| 5 | 1 byte | Status alarmă (0=stop, 1=start) |
| 6+ | variabil | Date adiționale |

### 9.2 Tipuri de Mesaje

| Tip | Cod | Direcție | Descriere |
|-----|-----|----------|-----------|
| Alarm Start | 1 | Detector → Module | Notificare început alarmă |
| Alarm Stop | 0 | Detector → Module | Notificare sfârșit alarmă |
| Heartbeat | 2 | Detector → Module | Status periodic |
| Forward | 100 | Module → Module | Mesaj retransmis (Slave mode) |

### 9.3 Mecanisme de Validare

```
┌─────────────────────────────────────────────────────────────┐
│                    VALIDARE PACHET                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   1. CRC Check                                               │
│      └─► packet.crc_ok == true                               │
│                                                              │
│   2. Address Verification                                    │
│      └─► Verifică dacă adresa sursă e în intervalele         │
│          configurate (AddressVerificationInterval)           │
│                                                              │
│   3. Length Validation                                       │
│      └─► packet.length >= RECV_MSG_MIN_LEN                   │
│      └─► packet.length <= RECV_MSG_MAX_LEN                   │
│                                                              │
│   4. Duplicate Check                                         │
│      └─► Verifică dacă alarma există deja în SRAM            │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 9.4 Forwarding și Rutare

```
┌─────────────────────────────────────────────────────────────────┐
│                      FLUX FORWARDING                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   SLAVE MODE (OPERATION_MODE_SLAVE = 2)                          │
│                                                                  │
│   1. Primire pachet de la detector                               │
│      │                                                           │
│      ▼                                                           │
│   2. Procesare locală (afișare, stocare)                         │
│      │                                                           │
│      ▼                                                           │
│   3. Modificare pachet:                                          │
│      - Adaugă flag MESSAGE_IS_FORWARDED (100)                    │
│      - Păstrează datele originale                                │
│      │                                                           │
│      ▼                                                           │
│   4. Schimbare canal RF (EEPROM_CC1101_FORWARD_CHANNEL_BYTE)     │
│      │                                                           │
│      ▼                                                           │
│   5. Transmitere către EEPROM_CC1101_FORWARD_ADDRESS_INT         │
│      │                                                           │
│      ▼                                                           │
│   6. Revenire la canal original pentru recepție                  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 10. Arhitectura Software

### 10.1 Controlere de Bază

#### CC1101Controller

**Locație:** `common/Controllers.cpp`

**Responsabilități:**
- Inițializare modul CC1101
- Configurare registre RF
- Transmisie/recepție pachete
- Gestionare stări (IDLE, RX, TX)

```cpp
class CC1101Controller {
public:
    void init();                          // Inițializare completă
    void reset();                         // Reset hardware
    void setChannel(uint8_t chnl);        // Setare canal RF
    void setDevAddress(uint8_t addr);     // Setare adresă dispozitiv
    bool sendData(CCPACKET packet);       // Transmitere pachet
    uint8_t receiveData(CCPACKET *packet);// Recepție pachet
    void setRxState();                    // Mod recepție
    void setTxState();                    // Mod transmisie
    int computeRssi(uint8_t rssi_dec);    // Calcul RSSI
    int computeLqi(uint8_t raw);          // Calcul LQI
};
```

#### EEPROMController

**Locație:** `common/Controllers.cpp`

**Responsabilități:**
- Citire/scriere valori în EEPROM extern (I2C)
- Gestionare numere seriale
- Stocare configurări persistente
- Backup alarme (Floor Module)

```cpp
class EEPROMController {
public:
    void begin();
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
    uint16_t readInt(uint16_t address);
    void writeInt(uint16_t address, uint16_t value);
    uint32_t readLong(uint16_t address);
    void writeLong(uint16_t address, uint32_t value);
    // Floor Module specific:
    void saveAlarmToMemory(AlarmEntry alarm);
    AlarmEntry readAlarmFromMemory(uint8_t index);
};
```

#### ButtonController

**Locație:** `common/Controllers.cpp`

**Responsabilități:**
- Citire stare butoane cu debounce (40ms)
- Detectare click-uri
- Notificare schimbare stare

```cpp
class ButtonController {
public:
    void setup();                         // Configurare pini
    void tick(uint32_t currentMillis);    // Update în loop
    boolean hasAnyStateChanged();         // Verificare schimbare
    boolean getClickedState(uint8_t btn); // Stare buton specific
};
```

#### LEDController

**Locație:** `common/Controllers.cpp`

**Responsabilități:**
- Control LED status cu animații
- Moduri: ON, SLOW_FADE, NORMAL_FADE, FAST_FADE, PULSE

```cpp
class LEDController {
public:
    void setup(uint8_t pin);
    void tick(uint32_t currentMillis);
    void setAnimation(uint8_t anim);
    void setColor(uint8_t color);
};
```

#### OutputController

**Locație:** `common/Controllers.cpp`

**Responsabilități:**
- Control ieșiri releu
- Moduri: ON, OFF, PULSE, ONESHOT

```cpp
class OutputController {
public:
    void setup();
    void tick(uint32_t currentMillis);
    void setOutput(uint8_t output, boolean state);
    void pulse(uint8_t output);           // 3 pulsuri
    void oneshot(uint8_t output);         // Activare temporară (3s default)
};
```

#### BuzzerController (doar Detector)

**Locație:** `detector/Controllers.cpp`

**Responsabilități:**
- Control buzzer alarmă
- Beeping intermitent (400ms interval)

```cpp
class BuzzerController {
public:
    void setup(uint8_t pin);
    void tick(uint32_t currentMillis);
    void start();
    void stop();
    boolean isBeeping();
};
```

### 10.2 Servicii de Nivel Înalt

#### CommunicationService

**Locație:** `common/Services.cpp`

**Responsabilități:**
- Abstractizare comunicare RF
- Construire și parsare pachete
- Trimitere mesaje alarmă (Detector)

```cpp
class CommunicationService {
public:
    CommunicationService(CC1101Controller *cc1101);
    void sendPacket(uint8_t *data, uint8_t len);
    // Detector specific:
    void sendAlarmStartPackage(uint16_t deviceId, uint16_t receiverAddr);
    void sendAlarmStopPackage(uint16_t deviceId, uint16_t receiverAddr);
};
```

#### AlarmService (doar Detector)

**Locație:** `detector/Services.cpp`

**Responsabilități:**
- Gestionare stare alarmă
- Logic retrimiteri mesaje
- Coordonare buzzer, LED, releu

```cpp
class AlarmService {
public:
    void tick(uint32_t currentMillis);
    void startAlarm();
    void stopAlarm();
    boolean isAlarmActive();
    boolean shouldResendMessage();
};
```

#### CoreService (doar Floor Module)

**Locație:** `floor-module/Services.cpp`

**Responsabilități:**
- Procesare pachete primite
- Gestionare liste detectoare/alarme
- Marcare detectoare offline
- Coordonare forwarding

```cpp
class CoreService {
public:
    void tick(uint32_t currentMillis);
    void processPacket(CCPACKET *packet);
    void checkOfflineDetectors();
    void forwardPacket(CCPACKET *packet);
    uint16_t getAlarmCount();
    uint16_t getDetectorCount();
    uint16_t getOfflineCount();
};
```

#### MenuService (doar Floor Module)

**Locație:** `floor-module/Services.cpp`

**Responsabilități:**
- Gestionare navigare meniu LCD
- Afișare pagini
- Procesare input butoane

```cpp
class MenuService {
public:
    void setup();
    void tick(uint32_t currentMillis);
    void handleButtonPress(uint8_t button);
    void goToPage(uint8_t pageIndex);
    void refreshDisplay();
};
```

#### ProgrammingService

**Locație:** `detector/Services.cpp` și `floor-module/Services.cpp`

**Responsabilități:**
- Procesare comenzi seriale
- Configurare dispozitiv prin serial
- Citire/scriere parametri EEPROM

```cpp
class ProgrammingService {
public:
    void tick();
    void processCommand(uint8_t cmdId, uint8_t *data, uint8_t len);
};
```

#### SerialDebug

**Locație:** `common/Services.cpp`

**Responsabilități:**
- Output debug condiționat
- Activare/dezactivare la runtime

```cpp
class SerialDebug {
public:
    SerialDebug(boolean enabled);
    void begin(uint32_t baud);
    void print(const char* msg);
    void println(const char* msg);
    void print(int val);
    void println(int val);
};
```

---

## 11. Structuri de Date

### 11.1 Alarme

```cpp
// Intrare alarmă (5 bytes) - stocat în SRAM
struct AlarmEntry {
    uint16_t address;     // Adresa detector sursă
    uint16_t id;          // ID detector
    boolean state;        // true=activ, false=inactiv
};

// Capacitate SRAM: (6050 - 0) / 5 = ~1210 alarme
```

### 11.2 Detectoare

```cpp
// Intrare detector (10 bytes) - stocat în SRAM
struct DetectorEntry {
    uint16_t address;        // Adresa CC1101
    uint16_t id;             // ID logic
    uint8_t linkQuality;     // LQI ultima recepție (0-100%)
    uint8_t status;          // Status curent
    uint32_t lastUpdateMs;   // Timestamp ultima comunicare
};

// Capacitate SRAM: (54050 - 6050) / 10 = ~4800 detectoare
```

### 11.3 Detectoare Offline

```cpp
// Intrare detector offline (2 bytes)
struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;  // Index în lista detectoare
};

// Capacitate SRAM: (63650 - 54050) / 2 = ~4800 indecși
```

### 11.4 Intervale de Verificare Adresă

```cpp
// Interval verificare adrese (4 bytes) - stocat în EEPROM
struct AddressVerificationInterval {
    uint16_t min;    // Adresă minimă acceptată
    uint16_t max;    // Adresă maximă acceptată
};

// Utilizare: Filtrare pachete de la adrese neautorizate
// Capacitate EEPROM: (291 - 251) / 4 = 10 intervale
```

### 11.5 Date Complete Configurare

```cpp
// Detector - pentru comenzi GET_ALL_VALUES / SET_ALL_VALUES
struct CompleteData {
    uint16_t address;          // Adresă CC1101
    uint8_t channel;           // Canal RF
    uint16_t receiverAddress;  // Adresă modul etaj
    uint16_t deviceId;         // ID dispozitiv
    int16_t alarmTrigger;      // Prag alarmă TGS
    boolean debugEnabled;      // Debug activ
};

// Floor Module - pentru comenzi GET_ALL_VALUES / SET_ALL_VALUES
struct CompleteData {
    uint16_t address;          // Adresă CC1101
    uint8_t channel;           // Canal RF
    uint16_t forwardAddress;   // Adresă forward (Slave mode)
    uint8_t forwardChannel;    // Canal forward
    uint8_t operationMode;     // Master/Slave
    boolean debugEnabled;      // Debug activ
};
```

---

## 12. Managementul Configurației

### 12.1 Chei EEPROM

#### Detector

| Adresă | Tip | Descriere | Valoare Default |
|--------|-----|-----------|-----------------|
| 0-3 | uint32_t | Număr serial | - |
| 10 | uint16_t | Startup flag | - |
| 50 | uint16_t | Valoare declanșare alarmă | 406 |
| 52 | uint16_t | Device ID | 65000 |
| 100 | uint16_t | Adresă CC1101 | - |
| 102 | uint8_t | Canal CC1101 | 0 |
| 103 | uint16_t | Adresă receiver | 1 |

#### Floor Module

| Adresă | Tip | Descriere | Valoare Default |
|--------|-----|-----------|-----------------|
| 0-3 | uint32_t | Număr serial | - |
| 10 | uint16_t | Startup flag | - |
| 50 | uint8_t | Mod operare | 1 (Master) |
| 100 | uint16_t | Adresă CC1101 | - |
| 102 | uint8_t | Canal CC1101 | 0 |
| 103 | uint16_t | Adresă forward | - |
| 105 | uint8_t | Canal forward | - |
| 109 | uint8_t | Index alarme memorate | 0 |
| 110 | uint8_t | Număr alarme memorate | 0 |
| 111-210 | - | Zona stocare alarme backup | - |
| 250 | uint8_t | Număr intervale verificare | 0 |
| 251-291 | - | Intervale verificare adrese | - |

### 12.2 Moduri de Operare

| Constantă | Valoare | Descriere |
|-----------|---------|-----------|
| `OPERATION_MODE_MASTER` | 1 | Procesare locală, fără forwarding |
| `OPERATION_MODE_SLAVE` | 2 | Procesare locală + forwarding |

### 12.3 Comenzi de Programare Serială

#### Detector (Baud: 19200)

| ID | Comandă | Parametri | Descriere |
|----|---------|-----------|-----------|
| 1 | `SET_DEVICE_ADDRESS` | uint16_t | Setează adresa CC1101 |
| 2 | `SET_DEVICE_CHANNEL` | uint8_t | Setează canalul CC1101 |
| 3 | `SET_RECEIVER_ADDRESS` | uint16_t | Setează adresa modulului etaj |
| 4 | `SET_DEVICE_ID` | uint16_t | Setează ID-ul dispozitivului |
| 5 | `SET_ALARM_TRIGGER` | uint16_t | Setează pragul declanșare alarmă |
| 6 | `SET_DEBUG_MODE` | boolean | Activează/dezactivează debug |
| 10 | `GET_DEVICE_TYPE` | - | Returnează tipul (1=Detector) |
| 20 | `GET_ALL_VALUES` | - | Citește toate valorile |
| 21 | `SET_ALL_VALUES` | CompleteData | Setează toate valorile |
| 101 | `TEST_MODE` | - | Intră în mod testare |

#### Floor Module (Baud: 19200)

| ID | Comandă | Parametri | Descriere |
|----|---------|-----------|-----------|
| 1 | `SET_DEVICE_ADDRESS` | uint16_t | Setează adresa CC1101 |
| 2 | `SET_DEVICE_CHANNEL` | uint8_t | Setează canalul CC1101 |
| 3 | `SET_FORWARD_ADDRESS` | uint16_t | Setează adresa forward |
| 4 | `SET_FORWARD_CHANNEL` | uint8_t | Setează canal forward |
| 5 | `SET_OPERATION_MODE` | uint8_t | Setează mod (Master/Slave) |
| 6 | `SET_DEBUG_MODE` | boolean | Activează/dezactivează debug |
| 10 | `GET_DEVICE_TYPE` | - | Returnează tipul (2=Module) |
| 20 | `GET_ALL_VALUES` | - | Citește toate valorile |
| 21 | `SET_ALL_VALUES` | CompleteData | Setează toate valorile |
| 30 | `STREAM_ALL_MEMORY` | - | Transmite conținutul memoriei |
| 31 | `CLEAR_ALARMS` | - | Șterge toate alarmele |
| 32 | `CLEAR_DETECTORS` | - | Șterge lista detectoare |
| 40 | `ADD_ADDRESS_VERIFICATION_INTERVAL` | min, max | Adaugă interval verificare |
| 41 | `CLEAR_ADDRESS_VERIFICATION_INTERVALS` | - | Șterge intervalele |
| 101 | `TEST_MODE` | - | Intră în mod testare |

---

## 13. Interfața Utilizator (Floor Module)

### 13.1 Structura Meniului

```
┌───────────────────────────────────────────────────────────────┐
│                    STRUCTURA MENIU LCD                         │
├───────────────────────────────────────────────────────────────┤
│                                                                │
│   PAGE_INDEX_HOME (0)                                          │
│   ┌──────────────────────────────────────────────────────┐     │
│   │  "ERGO"           "OK" sau "ALERT"                   │     │
│   │  "X alarms    Y det"                                 │     │
│   └──────────────────────────────────────────────────────┘     │
│        │                                                       │
│        ├──► LEFT: PAGE_INDEX_ALARMS (1)                        │
│        │    ┌─────────────────────────────────────────┐        │
│        │    │  "Detector alarms:"                     │        │
│        │    │  "X alarm(s)" sau "No alarms"           │        │
│        │    └─────────────────────────────────────────┘        │
│        │         │                                             │
│        │         └──► OK: PAGE_INDEX_LIST_ALARMS (4)           │
│        │              ┌─────────────────────────────────┐      │
│        │              │  "Alarm X"                      │      │
│        │              │  "ADDR/ID (LQI%)"               │      │
│        │              └─────────────────────────────────┘      │
│        │                                                       │
│        ├──► RIGHT: PAGE_INDEX_DETECTORS (2)                    │
│        │    ┌─────────────────────────────────────────┐        │
│        │    │  "Online"                               │        │
│        │    │  "detectors: X"                         │        │
│        │    └─────────────────────────────────────────┘        │
│        │         │                                             │
│        │         └──► OK: PAGE_INDEX_LIST_DETECTORS (5)        │
│        │                                                       │
│        └──► RIGHT x2: PAGE_INDEX_DETECTORS_OFFLINE (3)         │
│             ┌─────────────────────────────────────────┐        │
│             │  "Offline"                              │        │
│             │  "detectors: X"                         │        │
│             └─────────────────────────────────────────┘        │
│                  │                                             │
│                  └──► OK: PAGE_INDEX_LIST_DETECTORS_OFFLINE(6) │
│                                                                │
│   PAGE_INDEX_CLEAR_ALARMS (7)                                  │
│   ┌──────────────────────────────────────────────────────┐     │
│   │  "Clear alarms?"                                     │     │
│   │  "No           Yes"                                  │     │
│   └──────────────────────────────────────────────────────┘     │
│                                                                │
│   PAGE_INDEX_CLEAR_DETECTORS (8)                               │
│   ┌──────────────────────────────────────────────────────┐     │
│   │  "Clear detectors?"                                  │     │
│   │  "No           Yes"                                  │     │
│   └──────────────────────────────────────────────────────┘     │
│                                                                │
│   PAGE_INDEX_FIRMWARE (9)                                      │
│   ┌──────────────────────────────────────────────────────┐     │
│   │  "Firmware"                                          │     │
│   │  "v4.0"                                              │     │
│   └──────────────────────────────────────────────────────┘     │
│                                                                │
└───────────────────────────────────────────────────────────────┘
```

### 13.2 Funcțiile Butoanelor

| Buton | Funcție Generală | În Liste | În Confirmare |
|-------|------------------|----------|---------------|
| **BACK** | Înapoi la pagina anterioară | Înapoi la summary | Anulare (No) |
| **LEFT** | Pagina anterioară în nivel | Element anterior | - |
| **RIGHT** | Pagina următoare în nivel | Element următor | - |
| **OK** | Intră în sub-pagină / Confirmă | - | Confirmare (Yes) |

### 13.3 Timeouts Interfață

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| `MENU_TIMEOUT_MS` | 120000 (2 min) | Revenire automată la HOME |
| `MENU_BACKLIGHT_OFF_MS` | 60000 (1 min) | Stingere backlight LCD |
| `MENU_VALUE_REFRESH_MS` | 3000 (3 sec) | Refresh valori afișate |

---

## 14. Fluxuri de Dezvoltare

### 14.1 Principii de Organizare a Codului

```
┌─────────────────────────────────────────────────────────────────┐
│                 ARHITECTURA CONTROLLER-SERVICE                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   CONTROLLER (Hardware Layer)                                    │
│   ├── Interacțiune directă cu hardware                           │
│   ├── Abstractizare pini și registre                             │
│   ├── Nu conține logică de business                              │
│   └── Exemple: CC1101Controller, ButtonController, LEDController │
│                                                                  │
│   SERVICE (Business Logic Layer)                                 │
│   ├── Orchestrare controllere                                    │
│   ├── Implementare logică aplicație                              │
│   ├── Gestionare stări și tranziții                              │
│   └── Exemple: AlarmService, MenuService, CoreService            │
│                                                                  │
│   MODEL (Data Layer)                                             │
│   ├── Structuri de date                                          │
│   ├── Fără comportament                                          │
│   └── Exemple: AlarmEntry, DetectorEntry, CCPACKET               │
│                                                                  │
│   CONFIG (Configuration Layer)                                   │
│   ├── Constante și define-uri                                    │
│   ├── Configurări hardware (pini)                                │
│   └── Parametri sistem                                           │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 14.2 Managementul Memoriei

**Constrângeri AVR:**
- RAM limitat (~2KB tipic)
- PROGMEM pentru stringuri constante
- Evitare alocare dinamică (heap fragmentation)

**Strategii utilizate:**
```cpp
// Stringuri în PROGMEM (Flash) - nu ocupă RAM
#define MENU_TEXT_ERGO F("ERGO")
#define MENU_TEXT_OK F("OK")

// Buffere statice, dimensiuni fixe
static uint8_t txBuffer[CC1101_MAX_TX_BUFFER_SIZE];
static uint8_t rxBuffer[CC1101_MAX_RX_BUFFER_SIZE];

// SRAM extern pentru date mari (Floor Module)
// - 64KB adresabil prin SPI
// - Împărțit în zone: alarme, detectoare, offline
```

### 14.3 Mecanisme de Fiabilitate

| Mecanism | Descriere | Configurare |
|----------|-----------|-------------|
| **Watchdog** | Reset automat la blocare | `WATCHDOG_ENABLED = true` |
| **CRC** | Validare pachete RF | Automat CC1101 |
| **Retry** | Retrimitere mesaje alarmă | Max 5, interval progresiv |
| **Timeout** | Detectare offline | 7 minute fără comunicare |
| **Debounce** | Filtrare zgomot butoane | 40ms |

---

## 15. Testare și Depanare

### 15.1 Moduri de Test

#### Test Manual (Buton TEST - Detector)

```
1. Apasă butonul TEST pe detector
2. Detectorul intră în stare de alarmă simulată
3. LED roșu se aprinde, buzzer pornește
4. Mesaj RF transmis către Floor Module
5. Floor Module afișează alarma
6. După 3 minute, alarma se oprește automat
```

#### Test Mode prin Serial (Comanda 101)

```
1. Conectează prin serial (19200 baud)
2. Trimite comanda 101
3. Dispozitivul intră în mod test
4. Toate funcțiile pot fi testate individual
5. Ieșire prin reset sau comandă specifică
```

### 15.2 Interfață de Debug Serial

**Activare debug:**
```cpp
// În Config.h al dispozitivului
#define DEBUG_ENABLED true
```

**Output debug tipic:**
```
[CC1101] Init complete
[CC1101] Channel: 0, Address: 100
[ALARM] Start detected, value: 450
[RF] Sending alarm start packet
[RF] Packet sent, retries: 0
```

### 15.3 Monitorizarea Calității Legăturii

```cpp
// După recepție pachet
CCPACKET packet;
if (cc1101.receiveData(&packet)) {
    int rssi = cc1101.computeRssi(packet.rssi);  // dBm
    int lqi = cc1101.computeLqi(packet.lqi);     // 0-100%

    // RSSI bun: > -70 dBm
    // RSSI acceptabil: -70 to -85 dBm
    // RSSI slab: < -85 dBm

    // LQI bun: > 70%
    // LQI acceptabil: 40-70%
    // LQI slab: < 40%
}
```

---

## 16. Sarcini Comune de Dezvoltare

### 16.1 Adăugare Pagină Meniu Nouă

```cpp
// 1. În floor-module/Config.h - adaugă index pagină
#define PAGE_INDEX_NEW_PAGE 10
#define MENU_NUMBER_OF_PAGES 11  // incrementează

// 2. În floor-module/Config.h - adaugă texte
#define MENU_TEXT_NEW_PAGE F("New Page Title")

// 3. În MenuService::displayPage() - adaugă case
case PAGE_INDEX_NEW_PAGE:
    lcd.print(MENU_TEXT_NEW_PAGE);
    lcd.setCursor(0, 1);
    lcd.print(someValue);
    break;

// 4. În MenuService::handleButton() - adaugă navigare
// Definește ce butoane duc la/de la noua pagină
```

### 16.2 Adăugare Parametru EEPROM

```cpp
// 1. În Config.h - definește adresa
#define EEPROM_NEW_PARAM_INT 120  // adresă liberă

// 2. În ProgrammingService - adaugă comandă
#define COMMAND_ID_SET_NEW_PARAM 7

// 3. În processCommand() - implementează
case COMMAND_ID_SET_NEW_PARAM:
    uint16_t value = (data[1] << 8) | data[0];
    eepromController->writeInt(EEPROM_NEW_PARAM_INT, value);
    break;

// 4. Citire la startup
uint16_t newParam = eepromController->readInt(EEPROM_NEW_PARAM_INT);
```

### 16.3 Definire Comandă RF Nouă

```cpp
// 1. Definește structura mesajului
// Offset 0-1: adresa sursă
// Offset 2-3: device ID
// Offset 4: tip mesaj (nou)
// Offset 5+: date specifice

// 2. În CommunicationService - adaugă funcție transmitere
void sendNewMessageType(uint16_t param1, uint8_t param2) {
    uint8_t buffer[7];
    buffer[0] = lowByte(deviceAddress);
    buffer[1] = highByte(deviceAddress);
    buffer[2] = lowByte(deviceId);
    buffer[3] = highByte(deviceId);
    buffer[4] = NEW_MESSAGE_TYPE;  // tip nou
    buffer[5] = lowByte(param1);
    buffer[6] = param2;

    CCPACKET packet;
    packet.length = 7;
    memcpy(packet.data, buffer, 7);
    cc1101->sendData(packet);
}

// 3. În CoreService::processPacket() - adaugă handling
if (msgType == NEW_MESSAGE_TYPE) {
    // procesează mesajul nou
}
```

### 16.4 Modificare Timeout Detector

```cpp
// În floor-module/Config.h
// Schimbă valoarea (în milisecunde)
#define DETECTOR_OFFLINE_MS 600000  // 10 minute în loc de 7

// Efecte:
// - Detectoarele vor fi marcate offline mai târziu
// - Poate crește timpul de reacție la probleme reale
// - Recomandat: păstrează proporția cu intervalul de heartbeat
```

### 16.5 Optimizare Memorie

```cpp
// 1. Mută stringuri în PROGMEM
// Înainte:
const char* msg = "Error message";
// După:
const char msg[] PROGMEM = "Error message";
// sau
#define MSG F("Error message")

// 2. Reduce dimensiuni buffere dacă posibil
// Verifică utilizarea reală vs. dimensiune alocată

// 3. Folosește tipuri de date mai mici
// Înainte:
int counter = 0;  // 2 bytes
// După (dacă range permite):
uint8_t counter = 0;  // 1 byte

// 4. Evită variabile globale
// Preferă variabile locale sau membri de clasă
```

### 16.6 Integrare Senzor Nou

```cpp
// 1. Creează controller nou în detector/Controllers.h
class NewSensorController {
private:
    uint8_t pin;
    uint16_t lastValue;
public:
    void setup(uint8_t sensorPin);
    void tick(uint32_t currentMillis);
    uint16_t getValue();
    boolean isAboveThreshold(uint16_t threshold);
};

// 2. Implementează în detector/Controllers.cpp
void NewSensorController::setup(uint8_t sensorPin) {
    pin = sensorPin;
    pinMode(pin, INPUT);
}

uint16_t NewSensorController::getValue() {
    return analogRead(pin);
}

// 3. Adaugă în Config.h
#define HW_NEW_SENSOR_PIN 18
#define NEW_SENSOR_THRESHOLD 500

// 4. Instanțiază în .ino și integrează în loop
NewSensorController newSensor;
newSensor.setup(HW_NEW_SENSOR_PIN);

void loop() {
    newSensor.tick(millis());
    if (newSensor.isAboveThreshold(NEW_SENSOR_THRESHOLD)) {
        // acțiune
    }
}
```

---

## 17. Convenții de Cod

### 17.1 Convenții de Denumire

| Tip | Convenție | Exemple |
|-----|-----------|---------|
| **Constante** | UPPERCASE_SNAKE | `APP_VERSION`, `BTN_COUNT`, `EEPROM_PAGE_SIZE` |
| **Variabile** | camelCase | `currentValue`, `lastUpdateMs`, `isAlarmActive` |
| **Funcții** | camelCase | `getValue()`, `setChannel()`, `processPacket()` |
| **Clase** | PascalCase | `AlarmService`, `CC1101Controller`, `MenuService` |
| **Structuri** | PascalCase | `AlarmEntry`, `CCPACKET`, `DetectorEntry` |
| **Prefixe clase** | Controller/Service | `ButtonController` (hw), `AlarmService` (logic) |
| **Pini hardware** | HW_ prefix | `HW_STATUS_LED_PIN`, `HW_OUTPUT_RELAY_PIN` |
| **Adrese EEPROM** | EEPROM_ prefix | `EEPROM_SERIAL_NUMBER_ADDRESS_START` |

### 17.2 Structura Fișierelor

```cpp
/**
 * @file FileName.h
 * @brief Descriere scurtă
 *
 * Descriere detaliată dacă e necesar.
 */

#ifndef FILENAME_H
#define FILENAME_H

// ============================================================================
// Includes
// ============================================================================
#include <Arduino.h>
#include "../common/Config.h"

// ============================================================================
// Constants
// ============================================================================
#define CONSTANT_NAME value

// ============================================================================
// Types / Structures
// ============================================================================
struct StructureName {
    type member;
};

// ============================================================================
// Class Declaration
// ============================================================================
class ClassName {
private:
    type privateMember;

public:
    ClassName();
    void publicMethod();
};

#endif // FILENAME_H
```

### 17.3 Directive de Preprocesor

```cpp
// Include guards - obligatorii
#ifndef CONFIG_H
#define CONFIG_H
// ... conținut
#endif

// Compilare condiționată pentru debugging
#if DEBUG_ENABLED
    Serial.println("Debug message");
#endif

// Dezactivare funcționalități
#define DISABLE_CC1101_RECEIVE  // definit în detector - nu primește

// Valori default cu override posibil
#ifndef CUSTOM_VALUE
#define CUSTOM_VALUE 100
#endif
```

### 17.4 Conversii de Tipuri

```cpp
// Folosește union-uri pentru conversii byte
union ConvertBytesToInt {
    uint8_t byte[2];
    int16_t intVal;
};

// Utilizare
ConvertBytesToInt conv;
conv.byte[0] = lowByte;
conv.byte[1] = highByte;
int16_t result = conv.intVal;

// SAU folosește cast explicit
uint16_t value = (highByte << 8) | lowByte;

// Tipuri fixe - preferă
uint8_t   // în loc de byte
uint16_t  // în loc de unsigned int
int32_t   // în loc de long
uint32_t  // în loc de unsigned long
```

---

## 18. Ghid pentru AI Assistant

### 18.1 Reguli la Modificarea Codului

1. **CITEȘTE ÎNTÂI** - Înțelege contextul complet înainte de modificări
2. **RESPECTĂ CONVENȚIILE** - Folosește stilul existent în proiect
3. **MODIFICĂRI MINIME** - Schimbă doar ce e necesar
4. **PĂSTREAZĂ COMPATIBILITATEA** - Nu strica funcționalități existente
5. **DOCUMENTEAZĂ** - Adaugă comentarii pentru cod complex
6. **TESTEAZĂ MENTAL** - Verifică fluxul logic înainte de propunere

### 18.2 Reguli la Adăugarea de Funcționalități

```
✓ DO:
  - Urmează pattern-ul Controller/Service existent
  - Folosește constantele definite în Config.h
  - Adaugă în fișierele corespunzătoare (nu creezi fișiere noi fără motiv)
  - Verifică constrângerile de memorie AVR
  - Folosește PROGMEM pentru stringuri
  - Implementează error handling

✗ DON'T:
  - Nu adăuga dependințe externe fără aprobare
  - Nu schimba interfețele publice fără motiv
  - Nu folosi alocare dinamică (new/malloc)
  - Nu ignora watchdog (apelează wdt_reset() în bucle lungi)
  - Nu hardcoda valori - folosește constante
```

### 18.3 Reguli de Depanare

1. **Verifică configurația** - Config.h, adrese EEPROM, pini
2. **Verifică comunicația** - CC1101 init, canal, adresă
3. **Verifică timing** - Intervale, timeout-uri, debounce
4. **Verifică memoria** - Stack overflow, buffer overflow
5. **Folosește SerialDebug** - Activează `DEBUG_ENABLED` temporar

### 18.4 Reguli de Code Review

La review, verifică:

| Aspect | Întrebare |
|--------|-----------|
| **Funcționalitate** | Codul face ce trebuie? |
| **Convenții** | Respectă stilul proiectului? |
| **Memorie** | Nu depășește limitele AVR? |
| **Erori** | Gestionează cazurile de eroare? |
| **Watchdog** | Nu blochează sistemul? |
| **Comentarii** | Codul complex e documentat? |
| **Teste** | Poate fi testat? |

---

## 19. Considerații Importante

### 19.1 Limbaj și Localizare

- **Cod sursă:** Engleză (variabile, funcții, comentarii tehnice)
- **Texte UI (LCD):** Engleză
- **Documentație:** Română (CLAUDE.md, README.md)
- **Mesaje debug:** Engleză

### 19.2 Limitări Hardware

| Limitare | Valoare Tipică | Impact |
|----------|----------------|--------|
| **RAM** | ~2KB | Buffere mici, evită recursie adâncă |
| **Flash** | ~32KB | Cod optimizat, PROGMEM pentru stringuri |
| **EEPROM** | ~1KB | Configurări esențiale doar |
| **Stack** | ~256B | Funcții mici, puține variabile locale |
| **Clock** | 16MHz | Timing precis pentru comunicații |

### 19.3 Siguranță și Fiabilitate

**Mecanisme implementate:**

| Mecanism | Scop | Configurare |
|----------|------|-------------|
| **Watchdog** | Recuperare din blocări | 8s timeout |
| **CRC RF** | Integritate date | Hardware CC1101 |
| **Retry alarmă** | Livrare garantată | 5 încercări, backoff |
| **Confirmare măsurători** | Evitare false pozitive | 6 citiri consecutive |
| **Timeout offline** | Detectare pierdere contact | 7 minute |
| **Backup EEPROM** | Persistență alarme | La fiecare alarmă |

**Recomandări pentru cod nou:**
- Validează TOATE inputurile externe
- Nu presupune că comunicația RF reușește
- Implementează timeout-uri pentru orice așteptare
- Loghează erorile pentru debugging

---

## 20. Starea Actuală a Proiectului

### 20.1 Versiuni Curente

| Componentă | Versiune | Status |
|------------|----------|--------|
| Firmware Detector | 2.5 | Stabil, în producție |
| Firmware Floor Module | 4.0 | Stabil, în producție |
| Cod refactorizat | N/A | În dezvoltare |

### 20.2 Nivel de Implementare

| Funcționalitate | Detector | Floor Module |
|-----------------|----------|--------------|
| Detectare gaz | ✅ Complet | N/A |
| Alarmă locală | ✅ Complet | N/A |
| Comunicare RF | ✅ Complet | ✅ Complet |
| Afișare LCD | N/A | ✅ Complet |
| Meniu navigare | N/A | ✅ Complet |
| Stocare SRAM | N/A | ✅ Complet |
| Forwarding | N/A | ✅ Complet |
| Programare serial | ✅ Complet | ✅ Complet |
| Mod test | ✅ Complet | ✅ Complet |

### 20.3 Următorii Pași (Recomandări)

1. **Finalizare refactorizare** - Migrare completă de la fișiere monolitice
2. **Teste unitare** - Framework de teste pentru controllere
3. **Documentare API** - Doxygen pentru funcții publice
4. **Optimizare consum** - Sleep modes pentru baterie
5. **Logging persistent** - Istoric alarme în EEPROM/SD

---

## 21. Resurse

### 21.1 Documente Cheie în Repository

| Document | Cale | Descriere |
|----------|------|-----------|
| Acest ghid | `CLAUDE.md` | Context complet pentru AI și dezvoltatori |
| README | `README.md` | Prezentare generală proiect |
| Plan refactorizare | `refactorizare.md` | Strategia de modularizare |
| Config comun | `common/Config.h` | Toate constantele partajate |
| Config detector | `detector/Config.h` | Constante specifice detector |
| Config modul | `floor-module/Config.h` | Constante specifice modul |

### 21.2 Biblioteci Externe

| Bibliotecă | Utilizare | Link |
|------------|-----------|------|
| `<avr/wdt.h>` | Watchdog timer | AVR Libc standard |
| `<Arduino.h>` | Framework Arduino | arduino.cc |
| `<SPI.h>` | Comunicație SPI | Arduino standard |
| `<extEEPROM.h>` | EEPROM extern I2C | GitHub |
| `<LiquidCrystal_I2C.h>` | LCD I2C | Arduino Library Manager |

### 21.3 Referințe Tehnice

| Resursă | Descriere |
|---------|-----------|
| CC1101 Datasheet | Specificații complete modul RF |
| AVR Libc Reference | Funcții standard microcontroler |
| Arduino Reference | API Arduino |

### 21.4 Comenzi de Build

```bash
# Arduino IDE
# 1. Deschide fișierul .ino (detector/ sau floor-module/)
# 2. Selectează placa: Tools > Board > Arduino AVR Boards
# 3. Selectează port: Tools > Port
# 4. Compile: Sketch > Verify/Compile (Ctrl+R)
# 5. Upload: Sketch > Upload (Ctrl+U)

# PlatformIO CLI
cd detector/        # sau floor-module/
pio run             # compilare
pio run -t upload   # upload

# PlatformIO cu environment specific
pio run -e detector -t upload
```

---

## Changelog Document

| Versiune | Data | Modificări |
|----------|------|------------|
| 2.0 | 2026-01 | Restructurare completă cu 21 secțiuni |
| 1.0 | 2025 | Versiune inițială |

---

*Acest document este întreținut ca parte a proiectului ERGO26 și servește ca referință principală pentru dezvoltare și mentenanță.*
