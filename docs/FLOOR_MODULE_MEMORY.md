# Sistemul de Memorie al Modulului de Etaj (Floor Module)

## Cuprins

1. [Arhitectura Generală](#1-arhitectura-generală)
2. [SRAM Extern (64KB)](#2-sram-extern-64kb)
3. [Zone de Memorie SRAM](#3-zone-de-memorie-sram)
4. [EEPROM Intern](#4-eeprom-intern)
5. [Structuri de Date](#5-structuri-de-date)
6. [Controllere de Memorie](#6-controllere-de-memorie)
7. [Utilitare de Conversie](#7-utilitare-de-conversie)
8. [Operații de Citire/Scriere](#8-operații-de-citire-scriere)
9. [Scenarii de Utilizare](#9-scenarii-de-utilizare)
10. [Referințe Cod Sursă](#10-referințe-cod-sursă)

---

## 1. Arhitectura Generală

Floor Module implementează un **sistem dual de memorie**:
- **SRAM extern 64KB** - pentru date volatile de runtime (alarme, detectoare)
- **EEPROM intern** - pentru configurări persistente și backup alarme

```
┌─────────────────────────────────────────────────────────────────────┐
│                    FLOOR MODULE - ARHITECTURA MEMORIEI               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌────────────────────────────────────────────────────────────────┐ │
│  │                    SRAM EXTERN (64KB via SPI)                   │ │
│  ├────────────────────────────────────────────────────────────────┤ │
│  │                                                                  │ │
│  │  ┌──────────────────┐  ┌──────────────────┐  ┌───────────────┐  │ │
│  │  │  ZONA ALARME     │  │  ZONA DETECTOARE │  │ ZONA OFFLINE  │  │ │
│  │  │  0 - 6,050       │  │  6,050 - 54,050  │  │ 54,050-63,650 │  │ │
│  │  │  1,210 alarme    │  │  4,800 detectoare│  │ 4,800 indecși │  │ │
│  │  │  5 bytes/entry   │  │  10 bytes/entry  │  │ 2 bytes/entry │  │ │
│  │  └──────────────────┘  └──────────────────┘  └───────────────┘  │ │
│  │                                                                  │ │
│  │  ┌──────────────────────────────────────────────────────────┐   │ │
│  │  │              SPAȚIU NEUTILIZAT: 63,650 - 65,536          │   │ │
│  │  │                        (1,886 bytes)                      │   │ │
│  │  └──────────────────────────────────────────────────────────┘   │ │
│  └────────────────────────────────────────────────────────────────┘ │
│                                                                      │
│  ┌────────────────────────────────────────────────────────────────┐ │
│  │                    EEPROM INTERN (~1KB)                         │ │
│  ├────────────────────────────────────────────────────────────────┤ │
│  │                                                                  │ │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │ │
│  │  │ SERIAL NO.  │  │ CONFIGURARE │  │     BACKUP ALARME       │  │ │
│  │  │ 0-3 (4B)    │  │ 50-105      │  │     111-210 (20 alarme) │  │ │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │ │
│  │                                                                  │ │
│  │  ┌──────────────────────────────────────────────────────────┐   │ │
│  │  │          INTERVALE VERIFICARE ADRESE: 251-291            │   │ │
│  │  │                    (10 intervale)                         │   │ │
│  │  └──────────────────────────────────────────────────────────┘   │ │
│  └────────────────────────────────────────────────────────────────┘ │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### Rezumat Capacități

| Memorie | Capacitate Totală | Utilizare |
|---------|-------------------|-----------|
| SRAM | 65,536 bytes | Date runtime volatile |
| EEPROM | ~1,024 bytes | Configurare + backup persistent |

---

## 2. SRAM Extern (64KB)

### 2.1 Specificații Hardware

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| **Chip** | Microchip 23/25 series | SRAM SPI compatibil |
| **Capacitate** | 64KB (65,536 bytes) | Constantă `SRAM_64` |
| **Interfață** | SPI | Serial Peripheral Interface |
| **Pin CS** | D15 | `HW_SPI_CS_SRAM_PIN` |
| **Viteză SPI** | 26MHz | Clock SPI maxim |
| **Mod Operare** | Sequential (0x40) | `SRAM_SEQ_MODE` |

### 2.2 Registre și Coduri SPI

```cpp
// Coduri de operare SRAM
const uint8_t SRAM_WRITE_MODE_REG = 0x01;   // Scriere registru mod
const uint8_t SRAM_READ_MODE_REG = 0x05;    // Citire registru mod
const uint8_t SRAM_WRITE_CODE = 0x02;       // Instrucțiune scriere date
const uint8_t SRAM_READ_CODE = 0x03;        // Instrucțiune citire date

// Moduri de operare
const uint8_t SRAM_BYTE_MODE = 0b00000000;  // Operații singulare
const uint8_t SRAM_PAGE_MODE = 0b10000000;  // Scriere pagină
const uint8_t SRAM_SEQ_MODE  = 0b01000000;  // Mod secvențial (utilizat)
```

### 2.3 Protocol SPI de Comunicare

```
┌─────────────────────────────────────────────────────────────────────┐
│                    SECVENȚA CITIRE SRAM                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   MCU                                              SRAM              │
│    │                                                 │               │
│    │──── beginTransaction() ────────────────────────▶│               │
│    │                                                 │               │
│    │──── CS = LOW ──────────────────────────────────▶│               │
│    │                                                 │               │
│    │──── transfer(0x03) [READ_CODE] ────────────────▶│               │
│    │                                                 │               │
│    │──── transfer(addr_high) ───────────────────────▶│               │
│    │                                                 │               │
│    │──── transfer(addr_low) ────────────────────────▶│               │
│    │                                                 │               │
│    │──── transfer(0x00) ───────────────────────────▶│               │
│    │◀─── data_byte ─────────────────────────────────│               │
│    │                                                 │               │
│    │──── CS = HIGH ─────────────────────────────────▶│               │
│    │                                                 │               │
│    │──── endTransaction() ──────────────────────────▶│               │
│    │                                                 │               │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. Zone de Memorie SRAM

### 3.1 Harta Completă SRAM

```
Adresă      │ Dimensiune │ Zonă                    │ Capacitate
────────────┼────────────┼─────────────────────────┼─────────────
0           │            │                         │
            │  6,050 B   │ ALARME                  │ 1,210 alarme
6,050       │            │                         │
────────────┼────────────┼─────────────────────────┼─────────────
6,050       │            │                         │
            │ 48,000 B   │ DETECTOARE              │ 4,800 detectoare
54,050      │            │                         │
────────────┼────────────┼─────────────────────────┼─────────────
54,050      │            │                         │
            │  9,600 B   │ DETECTOARE OFFLINE      │ 4,800 indecși
63,650      │            │                         │
────────────┼────────────┼─────────────────────────┼─────────────
63,650      │            │                         │
            │  1,886 B   │ NEUTILIZAT              │ -
65,536      │            │                         │
────────────┴────────────┴─────────────────────────┴─────────────
```

### 3.2 Zona Alarme (0 - 6,050)

**Configurare:**
- **Start:** `SRAM_ALARMS_MEMORY_START` = 0
- **End:** `SRAM_ALARMS_MEMORY_END` = 6,050
- **Segment:** `SRAM_ALARMS_MEMORY_SEGMENT_SIZE` = 5 bytes
- **Capacitate:** 6,050 / 5 = **1,210 alarme**

**Layout Segment (5 bytes):**
```
Offset │ Dim │ Câmp    │ Tip      │ Descriere
───────┼─────┼─────────┼──────────┼────────────────────────
0      │ 2   │ address │ uint16_t │ Adresa detector sursă
2      │ 2   │ id      │ uint16_t │ ID alarmă
4      │ 1   │ state   │ boolean  │ true=activ, false=inactiv
```

**Exemplu Layout în Memorie:**
```
Index │ Adresă Start │ Adresă End │ Conținut
──────┼──────────────┼────────────┼──────────────────────────────
0     │ 0            │ 5          │ [ADDR_LO][ADDR_HI][ID_LO][ID_HI][STATE]
1     │ 5            │ 10         │ [ADDR_LO][ADDR_HI][ID_LO][ID_HI][STATE]
2     │ 10           │ 15         │ [ADDR_LO][ADDR_HI][ID_LO][ID_HI][STATE]
...   │ ...          │ ...        │ ...
1209  │ 6,045        │ 6,050      │ [ADDR_LO][ADDR_HI][ID_LO][ID_HI][STATE]
```

### 3.3 Zona Detectoare (6,050 - 54,050)

**Configurare:**
- **Start:** `SRAM_DETECTORS_MEMORY_START` = 6,050
- **End:** `SRAM_DETECTORS_MEMORY_END` = 54,050
- **Segment:** `SRAM_DETECTORS_MEMORY_SEGMENT_SIZE` = 10 bytes
- **Capacitate:** 48,000 / 10 = **4,800 detectoare**

**Layout Segment (10 bytes):**
```
Offset │ Dim │ Câmp          │ Tip      │ Descriere
───────┼─────┼───────────────┼──────────┼──────────────────────────────
0      │ 2   │ address       │ uint16_t │ Adresa CC1101 a detectorului
2      │ 2   │ id            │ uint16_t │ ID-ul detectorului
4      │ 1   │ linkQuality   │ uint8_t  │ Calitate link 0-100%
5      │ 1   │ status        │ uint8_t  │ Status curent
6      │ 4   │ lastUpdateMs  │ uint32_t │ Timestamp ultima comunicare
```

**Exemplu Layout în Memorie:**
```
Index │ Adresă Start │ Adresă End │ Conținut (10 bytes)
──────┼──────────────┼────────────┼─────────────────────────────────────────
0     │ 6,050        │ 6,060      │ [ADDR][ID][LQI][STS][TIMESTAMP_4B]
1     │ 6,060        │ 6,070      │ [ADDR][ID][LQI][STS][TIMESTAMP_4B]
...   │ ...          │ ...        │ ...
4799  │ 54,040       │ 54,050     │ [ADDR][ID][LQI][STS][TIMESTAMP_4B]
```

### 3.4 Zona Detectoare Offline (54,050 - 63,650)

**Configurare:**
- **Start:** `SRAM_OFFLINE_DETECTORS_MEMORY_START` = 54,050
- **End:** `SRAM_OFFLINE_DETECTORS_MEMORY_END` = 63,650
- **Segment:** `SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE` = 2 bytes
- **Capacitate:** 9,600 / 2 = **4,800 indecși**

**Layout Segment (2 bytes):**
```
Offset │ Dim │ Câmp               │ Tip      │ Descriere
───────┼─────┼────────────────────┼──────────┼──────────────────────────────
0      │ 2   │ detectorEntryIndex │ uint16_t │ Index în lista de detectoare
```

**Scop:** Această zonă stochează doar **indecșii** detectoarelor marcate offline, nu datele complete. Acest design:
- Economisește memorie (2 bytes vs 10 bytes)
- Permite iterare rapidă a detectoarelor offline
- Evită duplicarea datelor

---

## 4. EEPROM Intern

### 4.1 Harta Adreselor EEPROM

| Adresă | Dimensiune | Nume | Tip | Descriere |
|--------|------------|------|-----|-----------|
| 0-3 | 4 bytes | Serial Number | uint32_t | Numărul serial al dispozitivului |
| 10 | 1 byte | Startup Flag | uint8_t | Flag pentru prima pornire |
| 50 | 1 byte | Operation Mode | uint8_t | 1=Master, 2=Slave |
| 100 | 2 bytes | CC1101 Address | uint16_t | Adresa RF a dispozitivului |
| 102 | 1 byte | CC1101 Channel | uint8_t | Canalul RF (0-255) |
| 103 | 2 bytes | Forward Address | uint16_t | Adresa pentru forwarding (mod Slave) |
| 105 | 1 byte | Forward Channel | uint8_t | Canalul pentru forwarding |
| 109 | 1 byte | Alarms Index | uint8_t | Index curent în buffer circular |
| 110 | 1 byte | Alarms Count | uint8_t | Numărul de alarme în backup |
| 111-210 | 100 bytes | Alarms Backup | AlarmEntry[] | Buffer circular 20 alarme |
| 250 | 1 byte | Verification Count | uint8_t | Număr intervale verificare |
| 251-291 | 40 bytes | Verification Intervals | Interval[] | 10 intervale adrese |

### 4.2 Zona Backup Alarme (111-210)

**Configurare:**
- **Start:** `EEPROM_ALARMS_MEMORY_START` = 111
- **End:** `EEPROM_ALARMS_MEMORY_END` = 210
- **Segment:** 5 bytes (identic cu SRAM)
- **Capacitate:** 100 / 5 = **20 alarme**

**Funcționare Buffer Circular:**
```
┌─────────────────────────────────────────────────────────────────────┐
│                    BUFFER CIRCULAR EEPROM                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   index = 109 (EEPROM)  │  numberOfAlarms = 110 (EEPROM)             │
│                         │                                            │
│   ┌─────────────────────────────────────────────────────────────┐   │
│   │ 111 │ 116 │ 121 │ 126 │ ... │ 196 │ 201 │ 206 │             │   │
│   │  0  │  1  │  2  │  3  │ ... │ 17  │ 18  │ 19  │             │   │
│   └──▲──┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘             │   │
│      │                                                           │   │
│      └── index pointează aici                                    │   │
│                                                                      │
│   La adăugare:                                                       │
│   1. Scrie alarma la poziția curentă                                 │
│   2. Incrementează index                                             │
│   3. Dacă index >= 20, resetează la 0 (wrap around)                 │
│   4. Incrementează count până la max 20                              │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 4.3 Intervale Verificare Adrese (251-291)

**Configurare:**
- **Start:** `EEPROM_ADDRESS_VERIFICATION_INTERVALS_START` = 251
- **End:** `EEPROM_ADDRESS_VERIFICATION_INTERVALS_END` = 291
- **Segment:** 4 bytes per interval
- **Capacitate:** 40 / 4 = **10 intervale**

**Layout Interval (4 bytes):**
```
Offset │ Dim │ Câmp │ Tip      │ Descriere
───────┼─────┼──────┼──────────┼─────────────────────────
0      │ 2   │ min  │ uint16_t │ Adresa minimă acceptată
2      │ 2   │ max  │ uint16_t │ Adresa maximă acceptată
```

**Scop:** Filtrarea mesajelor RF - doar adresele în intervalele definite sunt acceptate.

---

## 5. Structuri de Date

### 5.1 AlarmEntry

**Locație:** `floor-module/Models.h`
**Dimensiune:** 5 bytes

```cpp
struct AlarmEntry {
    uint16_t address;    // Adresa detectorului sursă (2 bytes)
    uint16_t id;         // ID-ul alarmei (2 bytes)
    boolean state;       // Stare: true=activă, false=inactivă (1 byte)
};
```

### 5.2 DetectorEntry

**Locație:** `floor-module/Models.h`
**Dimensiune:** 10 bytes

```cpp
struct DetectorEntry {
    uint16_t address;        // Adresa CC1101 (2 bytes)
    uint16_t id;             // ID detector (2 bytes)
    uint8_t linkQuality;     // LQI 0-100% (1 byte)
    uint8_t status;          // Status byte (1 byte)
    uint32_t lastUpdateMs;   // Timestamp ultima comunicare (4 bytes)
};
```

### 5.3 OfflineDetectorEntry

**Locație:** `floor-module/Models.h`
**Dimensiune:** 2 bytes

```cpp
struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;  // Index în array-ul DetectorEntry
};
```

### 5.4 AddressVerificationInterval

**Locație:** `floor-module/Models.h`
**Dimensiune:** 4 bytes

```cpp
struct AddressVerificationInterval {
    uint16_t min;   // Adresa minimă
    uint16_t max;   // Adresa maximă
};
```

---

## 6. Controllere de Memorie

### 6.1 MicrochipSRAM (Driver Hardware)

**Locație:** `floor-module/Controllers.h:19-97`

**Clasa de bază pentru acces SRAM via SPI:**

```cpp
class MicrochipSRAM {
private:
    uint32_t capacity;
    SPISettings spiSettings;
    uint8_t cs_pin;

public:
    MicrochipSRAM(uint32_t size, uint8_t ss_pin, uint32_t clock);

    // Metode template pentru acces generic
    template <typename T> uint32_t& get(uint32_t addr, T& value);
    template <typename T> uint32_t& put(uint32_t addr, const T& value);

    // Operații pe memorie
    void clearMemory(uint8_t clearValue = 0);
    void fillMemory(uint32_t startAddr, uint8_t fillValue);
};
```

### 6.2 SRAMController (Logică Aplicație)

**Locație:** `floor-module/Controllers.h:100-152`
**Implementare:** `floor-module/Controllers.cpp:50-390`

**Gestionează toate cele trei zone de memorie:**

```cpp
class SRAMController {
private:
    MicrochipSRAM *sram;

    // Contoare
    uint16_t numberOfAlarms;
    uint16_t numberOfDetectors;
    uint16_t numberOfOfflineDetectors;

    // Buffere auxiliare
    uint8_t alarmAuxBuffer[5];
    uint8_t detectorAuxBuffer[10];
    uint8_t offlineDetectorAuxBuffer[2];

public:
    // Inițializare
    void setup();

    // Operații Alarme
    int16_t addAlarm(AlarmEntry *alarm, boolean isUpdateOnly);
    boolean getAlarm(AlarmEntry *alarm, uint16_t elemIndex);
    int16_t findAlarm(uint16_t address);
    boolean deleteAlarm(uint16_t elemIndex);
    boolean clearAlarms();
    uint16_t getNumberOfAlarms();

    // Operații Detectoare
    int16_t addDetector(DetectorEntry *detector);
    boolean getDetector(DetectorEntry *detector, uint16_t elemIndex);
    int16_t findDetector(uint16_t address);
    boolean deleteDetector(uint16_t elemIndex);
    boolean clearDetectors();
    uint16_t getNumberOfDetectors();

    // Operații Detectoare Offline
    int16_t addOfflineDetector(OfflineDetectorEntry *offlineDetector);
    boolean getOfflineDetector(OfflineDetectorEntry *offlineDetector, uint16_t elemIndex);
    int16_t findOfflineDetector(uint16_t detectorEntryIndex);
    boolean deleteOfflineDetector(uint16_t detectorEntryIndex);
    boolean clearOfflineDetectors();
    uint16_t getNumberOfOfflineDetectors();
};
```

### 6.3 FloorModuleEEPROMController

**Locație:** `floor-module/Controllers.h:157-184`
**Implementare:** `floor-module/Controllers.cpp:392-545`

**Extinde EEPROMController pentru funcționalități specifice:**

```cpp
class FloorModuleEEPROMController : public EEPROMController {
private:
    uint8_t alarmAuxBuffer[5];
    uint8_t index;
    uint8_t numberOfAlarms;

public:
    // Backup Alarme
    void saveAlarmDetails();
    void loadAlarmDetails();
    uint8_t getNumberOfAlarms();
    void addAlarm(AlarmEntry* alarm);
    boolean getAlarm(AlarmEntry* alarm, uint8_t elemIndex);
    boolean deleteAlarm(uint16_t address);
    boolean clearAlarms();

    // Intervale Verificare Adrese
    uint8_t getNumberOfAddressVerificationIntervals();
    boolean addAddressVerificationInterval(AddressVerificationInterval *interval);
    boolean getAddressVerificationInterval(uint8_t index, AddressVerificationInterval *interval);
    boolean clearAddressVerificationIntervals();
};
```

---

## 7. Utilitare de Conversie

**Locație:** `common/Config.h:174-195`

Pentru a converti între valori multi-byte și bytes individuali (necesar pentru SRAM/EEPROM):

```cpp
// Conversie 32-bit signed
union ConvertBytesToLong {
    uint8_t byte[4];      // [0]=LSB, [3]=MSB
    int32_t longVal;
};

// Conversie 32-bit unsigned
union ConvertBytesToULong {
    uint8_t byte[4];
    uint32_t longVal;
};

// Conversie 16-bit signed
union ConvertBytesToInt {
    uint8_t byte[2];      // [0]=LSB, [1]=MSB
    int16_t intVal;
};

// Conversie 16-bit unsigned
union ConvertBytesToUInt {
    uint8_t byte[2];
    uint16_t intVal;
};
```

**Exemplu Utilizare:**
```cpp
// Scrierea unei adrese uint16_t în buffer
ConvertBytesToUInt conv;
conv.intVal = detector->address;
buffer[0] = conv.byte[0];  // Low byte
buffer[1] = conv.byte[1];  // High byte

// Citirea înapoi
conv.byte[0] = buffer[0];
conv.byte[1] = buffer[1];
uint16_t address = conv.intVal;
```

---

## 8. Operații de Citire/Scriere

### 8.1 Scrierea unui Detector în SRAM

**Funcție:** `SRAMController::addDetector()`
**Locație:** `floor-module/Controllers.cpp:199-251`

```
┌─────────────────────────────────────────────────────────────────────┐
│                    FLUX SCRIERE DETECTOR                             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   1. PREGĂTIRE BUFFER (10 bytes)                                     │
│      ┌────────────────────────────────────────────────────────────┐ │
│      │ detectorAuxBuffer[0-1] = address (little-endian)           │ │
│      │ detectorAuxBuffer[2-3] = id (little-endian)                │ │
│      │ detectorAuxBuffer[4]   = linkQuality                       │ │
│      │ detectorAuxBuffer[5]   = status                            │ │
│      │ detectorAuxBuffer[6-9] = lastUpdateMs (little-endian)      │ │
│      └────────────────────────────────────────────────────────────┘ │
│                                                                      │
│   2. CALCUL ADRESĂ                                                   │
│      baseAddr = SRAM_DETECTORS_MEMORY_START +                        │
│                 (numberOfDetectors * SEGMENT_SIZE)                   │
│      baseAddr = 6050 + (index * 10)                                  │
│                                                                      │
│   3. SCRIERE ÎN SRAM                                                 │
│      for (i = 0; i < 10; i++) {                                      │
│          sram->put(baseAddr + i, detectorAuxBuffer[i]);             │
│      }                                                               │
│                                                                      │
│   4. INCREMENTARE CONTOR                                             │
│      numberOfDetectors++;                                            │
│                                                                      │
│   5. RETURNARE INDEX                                                 │
│      return numberOfDetectors - 1;                                   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 8.2 Citirea unui Detector din SRAM

**Funcție:** `SRAMController::getDetector()`
**Locație:** `floor-module/Controllers.cpp:253-276`

```
┌─────────────────────────────────────────────────────────────────────┐
│                    FLUX CITIRE DETECTOR                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   1. VALIDARE INDEX                                                  │
│      if (elemIndex >= numberOfDetectors) return false;               │
│                                                                      │
│   2. CALCUL ADRESĂ                                                   │
│      baseAddr = SRAM_DETECTORS_MEMORY_START +                        │
│                 (elemIndex * SEGMENT_SIZE)                           │
│                                                                      │
│   3. CITIRE DIN SRAM                                                 │
│      for (i = 0; i < 10; i++) {                                      │
│          sram->get(baseAddr + i, detectorAuxBuffer[i]);             │
│      }                                                               │
│                                                                      │
│   4. RECONSTRUIRE STRUCTURĂ                                          │
│      ┌────────────────────────────────────────────────────────────┐ │
│      │ detector->address      = bytes[0-1] → uint16_t             │ │
│      │ detector->id           = bytes[2-3] → uint16_t             │ │
│      │ detector->linkQuality  = bytes[4]   → uint8_t              │ │
│      │ detector->status       = bytes[5]   → uint8_t              │ │
│      │ detector->lastUpdateMs = bytes[6-9] → uint32_t             │ │
│      └────────────────────────────────────────────────────────────┘ │
│                                                                      │
│   5. RETURNARE SUCCES                                                │
│      return true;                                                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 8.3 Adăugare Alarmă în Buffer Circular EEPROM

**Funcție:** `FloorModuleEEPROMController::addAlarm()`
**Locație:** `floor-module/Controllers.cpp:422-445`

```
┌─────────────────────────────────────────────────────────────────────┐
│                    FLUX BUFFER CIRCULAR                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   1. CONVERSIE ALARMĂ → BYTES                                        │
│      alarmAuxBuffer[0-4] = alarm data (5 bytes)                      │
│                                                                      │
│   2. CALCUL POZIȚIE ÎN BUFFER                                        │
│      if (numberOfAlarms < 20) {                                      │
│          writePos = EEPROM_ALARMS_MEMORY_START +                     │
│                     (numberOfAlarms * 5)                             │
│          numberOfAlarms++;                                           │
│      } else {                                                        │
│          writePos = EEPROM_ALARMS_MEMORY_START + (index * 5)         │
│          index = (index + 1) % 20;  // Wrap around                   │
│      }                                                               │
│                                                                      │
│   3. SCRIERE ÎN EEPROM                                               │
│      put(writePos, alarmAuxBuffer, 5);                               │
│                                                                      │
│   4. SALVARE METADATA                                                │
│      saveAlarmDetails();  // Scrie index și count în EEPROM          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 9. Scenarii de Utilizare

### 9.1 Instalație Mică (Clădire de Birouri)

| Parametru | Valoare | Memorie Utilizată |
|-----------|---------|-------------------|
| Detectoare active | 50 | 500 bytes |
| Alarme active | 10 | 50 bytes |
| Detectoare offline | 5 | 10 bytes |
| **Total SRAM** | - | **560 bytes (0.9%)** |

### 9.2 Instalație Medie (Fabrică)

| Parametru | Valoare | Memorie Utilizată |
|-----------|---------|-------------------|
| Detectoare active | 500 | 5,000 bytes |
| Alarme active | 100 | 500 bytes |
| Detectoare offline | 50 | 100 bytes |
| **Total SRAM** | - | **5,600 bytes (8.5%)** |

### 9.3 Instalație Mare (Complex Industrial)

| Parametru | Valoare | Memorie Utilizată |
|-----------|---------|-------------------|
| Detectoare active | 2,000 | 20,000 bytes |
| Alarme active | 500 | 2,500 bytes |
| Detectoare offline | 200 | 400 bytes |
| **Total SRAM** | - | **22,900 bytes (35%)** |

### 9.4 Capacitate Maximă

| Parametru | Valoare | Memorie Utilizată |
|-----------|---------|-------------------|
| Detectoare | 4,800 | 48,000 bytes |
| Alarme | 1,210 | 6,050 bytes |
| Offline | 4,800 | 9,600 bytes |
| **Total SRAM** | - | **63,650 bytes (97%)** |

---

## 10. Referințe Cod Sursă

### 10.1 Fișiere Principale

| Fișier | Conținut |
|--------|----------|
| `floor-module/Config.h` | Constante memorie, adrese SRAM/EEPROM |
| `floor-module/Models.h` | Structuri de date (AlarmEntry, DetectorEntry, etc.) |
| `floor-module/Controllers.h` | Declarații clase MicrochipSRAM, SRAMController |
| `floor-module/Controllers.cpp` | Implementări (liniile 50-545) |
| `common/Config.h` | Utilitare conversie bytes (liniile 174-195) |

### 10.2 Constante Cheie

| Constantă | Valoare | Fișier | Linie |
|-----------|---------|--------|-------|
| `HW_SPI_CS_SRAM_PIN` | 15 | floor-module/Config.h | 43 |
| `SRAM_64` | 65,536 | floor-module/Controllers.h | 31 |
| `SRAM_ALARMS_MEMORY_START` | 0 | floor-module/Config.h | 121 |
| `SRAM_ALARMS_MEMORY_END` | 6,050 | floor-module/Config.h | 122 |
| `SRAM_ALARMS_MEMORY_SEGMENT_SIZE` | 5 | floor-module/Config.h | 123 |
| `SRAM_DETECTORS_MEMORY_START` | 6,050 | floor-module/Config.h | 125 |
| `SRAM_DETECTORS_MEMORY_END` | 54,050 | floor-module/Config.h | 126 |
| `SRAM_DETECTORS_MEMORY_SEGMENT_SIZE` | 10 | floor-module/Config.h | 127 |
| `SRAM_OFFLINE_DETECTORS_MEMORY_START` | 54,050 | floor-module/Config.h | 129 |
| `SRAM_OFFLINE_DETECTORS_MEMORY_END` | 63,650 | floor-module/Config.h | 130 |
| `SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE` | 2 | floor-module/Config.h | 131 |

### 10.3 Funcții Principale

| Funcție | Locație | Descriere |
|---------|---------|-----------|
| `SRAMController::setup()` | Controllers.cpp:55 | Inițializare SRAM |
| `SRAMController::addAlarm()` | Controllers.cpp:89 | Adaugă/actualizează alarmă |
| `SRAMController::addDetector()` | Controllers.cpp:199 | Adaugă/actualizează detector |
| `SRAMController::addOfflineDetector()` | Controllers.cpp:303 | Adaugă detector offline |
| `FloorModuleEEPROMController::addAlarm()` | Controllers.cpp:422 | Backup alarmă în EEPROM |

---

## Rezumat

Modulul de Etaj implementează un sistem de memorie robust și eficient:

1. **SRAM Extern 64KB** - stocare rapidă pentru date runtime
   - 1,210 alarme simultane
   - 4,800 detectoare
   - 4,800 indecși offline

2. **EEPROM Intern** - persistență pentru:
   - Configurare (adrese, canale, mod operare)
   - 20 alarme backup (buffer circular)
   - 10 intervale verificare adrese

3. **Design Eficient**:
   - Segmente de dimensiune fixă (fără fragmentare)
   - Acces O(1) prin index
   - Conversie byte-level pentru compatibilitate AVR
   - Buffer circular pentru backup persistent

---

*Document generat automat pe baza analizei codului sursă ERGO26.*
