# Scenarii Posibile: Alarme cu ID și Adresă 0

**Problemă analizată:** În versiunea 4.0 apar alarme cu ID și adresă **0**, pe lângă problema existentă din v3.0 cu numere mari aleatorii (5 cifre).

---

## Scenariul 1: Buffer RF Neinițializat sau Citit Prematur

### Descriere

Bufferul `receiveBuffer` conține valori 0 când datele sunt citite **înainte** ca modulul CC1101 să finalizeze recepția pachetului, sau când bufferul nu este inițializat corect la pornire.

### Analiza Codului

**În `CC1101Controller::receiveData()` (common/Controllers.cpp:328-344):**
```cpp
rxBytes = readStatusReg(CC1101_RXBYTES);

if (rxBytes & 0x7F && !(rxBytes & 0x80)) {
    packet->length = readConfigReg(CC1101_RXFIFO);

    if (packet->length > CC1101_MAX_BUFFER_SIZE)
        packet->length = 0;  // Pachet invalid → lungime 0, dar structura nu e resetată
    else {
        readBurstReg(packet->data, CC1101_RXFIFO, packet->length);
        // ... citire RSSI, LQI, CRC
    }
} else
    packet->length = 0;  // Nimic de citit → lungime 0
```

**Problema:** Când `packet->length = 0`, array-ul `packet->data[]` **nu este modificat**. Dacă bufferul conținea 0-uri de la inițializare sau de la un pachet anterior invalid, acestea persistă.

**În `CoreService::operationModeMaster()` (floor-module/Services.cpp:458-483):**
```cpp
void CoreService::operationModeMaster(uint32_t currentMillis) {
    convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_RECEIVER_LO];  // index 0
    convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_RECEIVER_HI];  // index 1

    if (convertBytesToUInt.intVal == deviceAddress) {
        // Procesare pachet fără verificare dacă length > 0
        convertBytesToUInt.byte[0] = receiveBuffer[BUFF_INDEX_ADDRESS_LO];  // index 2
        convertBytesToUInt.byte[1] = receiveBuffer[BUFF_INDEX_ADDRESS_HI];  // index 3
```

**Problema:** Nu există verificare `if (*receiveBufferLen >= 8)` înainte de procesare. Dacă `receiveBufferLen` e setat greșit, se citesc valori vechi/0.

### Flux Eroare

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 1. Pornire Floor Module                                                 │
│    └── receiveBuffer[] inițializat cu 0-uri (sau valori nedefinite)     │
│                                                                         │
│ 2. Interferență RF sau pachet trunchiat                                 │
│    └── CC1101 primește date parțiale (ex: doar bytes 0-1)               │
│                                                                         │
│ 3. receiveData() citește pachetul                                       │
│    └── length = 2 (doar receiver address)                               │
│    └── bytes 2-7 rămân 0 (address, id, command = 0)                     │
│                                                                         │
│ 4. tick() verifică receiveBufferLen > 0 ✓                               │
│    └── Dar nu verifică dacă len >= 8 (dimensiune minimă validă)         │
│                                                                         │
│ 5. operationModeMaster() procesează                                     │
│    └── Citește address = 0 (bytes 2-3 = 0)                              │
│    └── Citește id = 0 (bytes 4-5 = 0)                                   │
│    └── Creează AlarmEntry cu address=0, id=0                            │
│                                                                         │
│ 6. Alarmă cu 0 înregistrată în SRAM                                     │
└─────────────────────────────────────────────────────────────────────────┘
```

### Dovezi din Cod

1. **Verificarea lungimii lipsește** - În versiunea legacy (`ergo-floor-module-v4.0.cpp:2536`):
   ```cpp
   while(index < *receiveBufferLen && *receiveBufferLen > 7)
   ```
   Dar în versiunea refactorizată (`floor-module/Services.cpp:446`):
   ```cpp
   if (*receiveBufferLen > 0)  // ← lipsește verificarea >= 8
   ```

2. **Nu există validare că address != 0** înainte de stocare.

### Impact

- Alarme false cu address=0, id=0
- Confuzie în monitorizare
- Imposibilitatea identificării sursei reale

### Probabilitate: **RIDICATĂ** (70%)

---

## Scenariul 2: Pachet RF cu CRC Valid dar Date Corupte (Coliziune)

### Descriere

Două sau mai multe detectoare transmit simultan, rezultând în **coliziune RF**. Modulul CC1101 poate valida CRC-ul pe o combinație aleatorie de biți care, prin coincidență, generează un CRC valid, dar cu date corupte (0-uri sau valori aleatorii).

### Analiza Codului

**Validare CRC în `receiveData()` (common/Controllers.cpp:341):**
```cpp
packet->crc_ok = bitRead(val, 7);
```

**Dar CRC nu este verificat la procesare!** În `CoreService::tick()` și `operationModeMaster()` nu există:
```cpp
if (!packet.crc_ok) return;  // ← LIPSEȘTE
```

### Flux Eroare

```
┌─────────────────────────────────────────────────────────────────────────┐
│ 1. Detector A transmite alarmă (address=15, id=3)                       │
│ 2. Detector B transmite simultan (address=27, id=8)                     │
│                                                                         │
│ 3. Coliziune RF în aer                                                  │
│    └── Semnale se suprapun                                              │
│    └── Floor Module primește mix de biți                                │
│                                                                         │
│ 4. CC1101 procesează semnalul corupt                                    │
│    └── Demodulare rezultă în pattern aleator                            │
│    └── CRC calculat pe date corupte                                     │
│                                                                         │
│ 5. CAZUL SPECIAL: Pattern-ul 0x00 0x00 ... 0x00                         │
│    └── CRC pentru secvență de 0-uri poate fi valid                      │
│    └── packet.crc_ok = true (fals pozitiv)                              │
│                                                                         │
│ 6. Pachetul trece validarea (care oricum lipsește)                      │
│    └── address = 0, id = 0 stocat ca alarmă                             │
│                                                                         │
│ 7. SAU: Pachet cu doar câțiva biți corupți                              │
│    └── Receiver address valid (pachet acceptat)                         │
│    └── Dar address/id corupte spre 0 sau valori mari                    │
└─────────────────────────────────────────────────────────────────────────┘
```

### Factori Agravanți

1. **Toate detectoarele pe același canal RF** - Risc mare de coliziune
2. **Fără mecanism CSMA/CA** - Detectoarele nu ascultă înainte de transmisie
3. **Retry-uri multiple** - 5 retrimiteri la intervale scurte cresc șansa coliziunii

### Diferența față de v3.0

În v3.0 sistemul avea aceleași probleme, dar:
- Coruperea producea valori **aleatorii** (numere mari)
- În v4.0, cu adăugarea `AddressVerificationInterval`, pachetele cu numere mari sunt **filtrate**
- Dar pachetele cu 0 **trec** filtrarea (dacă intervalul include 0 sau dacă nu sunt intervale configurate)

### Cod relevant - Verificarea intervalelor NU FILTREAZĂ 0:

```cpp
// ergo-floor-module-v4.0.cpp:2541-2553
if(numberOfAddressVerificationIntervals > 0) {
    aux = 0;
    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    for(j=0;j<numberOfAddressVerificationIntervals;j++) {
        if(addressVerificationIntervals[j].min <= convertBytesToUInt.intVal
           && convertBytesToUInt.intVal <= addressVerificationIntervals[j].max) {
            aux = 1;  // Valid
            break;
        }
    }
    if(aux == 0) {
        goto SKIP_PROCESSING;  // Filtrat
    }
}
// Dacă min=1, pachetele cu address=0 AR TREBUI filtrate
// DAR: numberOfAddressVerificationIntervals poate fi 0!
```

**Problema:** Dacă `numberOfAddressVerificationIntervals == 0`, verificarea este **complet ignorată** și orice adresă (inclusiv 0) trece.

### Probabilitate: **MEDIE** (50%)

---

## Scenariul 3: Detector Neprogramat sau cu EEPROM Corupt

### Descriere

Un detector din rețea are **EEPROM corupt** sau nu a fost programat corect, rezultând în valori default 0 pentru address și ID. Când acest detector transmite, Floor Module primește și stochează valori 0 valide.

### Analiza Codului

**Citire valori din EEPROM la pornire detector (detector/Services.cpp:335):**
```cpp
dataTemp.address = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
```

**Dacă EEPROM-ul conține 0xFFFF (neprogrmat) sau 0x0000 (corupt):**
- Address = 0 sau 65535
- ID = 0 sau 65535

**Transmisia pachetului (detector/Services.cpp:35-47):**
```cpp
void AlarmService::sendAlarmStartPackage() {
    convertBytesToUInt.intVal = receiverAddress;
    buff[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];

    convertBytesToUInt.intVal = deviceAddress;  // ← Poate fi 0
    buff[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];

    convertBytesToUInt.intVal = deviceId;  // ← Poate fi 0
    buff[BUFF_INDEX_ID_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ID_HI] = convertBytesToUInt.byte[1];
```

### Flux Eroare

```
┌─────────────────────────────────────────────────────────────────────────┐
│ CAUZA A: EEPROM nescris/defect                                          │
│ ─────────────────────────────────────────                               │
│ 1. Detector nou montat fără programare                                  │
│    └── EEPROM conține 0xFF (valoare default chip)                       │
│    └── getUIntValue() returnează 0xFFFF sau 0x0000                      │
│                                                                         │
│ 2. Detector pornește și intră în funcționare normală                    │
│    └── Senzor TGS detectează gaz real                                   │
│    └── Alarmă locală activată (buzzer, LED roșu)                        │
│                                                                         │
│ 3. sendAlarmStartPackage() trimite pachet RF                            │
│    └── address = 0, id = 0 (sau 65535)                                  │
│    └── receiver_address = 0 → pachet ignorat de FM                      │
│    └── SAU receiver_address valid → FM procesează                       │
│                                                                         │
│ 4. Floor Module primește pachet valid                                   │
│    └── Stochează alarmă cu address=0, id=0                              │
├─────────────────────────────────────────────────────────────────────────┤
│ CAUZA B: EEPROM corupt în timp                                          │
│ ─────────────────────────────────────────                               │
│ 1. Detector funcțional cu address=15, id=3                              │
│                                                                         │
│ 2. Eveniment de corupere EEPROM:                                        │
│    └── Fluctuație tensiune la scriere                                   │
│    └── Interferență electromagnetică                                    │
│    └── Uzură EEPROM (>100.000 cicluri scriere)                          │
│    └── Resetare în timpul operației de scriere                          │
│                                                                         │
│ 3. Celule EEPROM corupte                                                │
│    └── Adresa EEPROM 100-101 (CC1101_ADDRESS) = 0x0000                  │
│    └── Adresa EEPROM 52-53 (DEVICE_ID) = 0x0000                         │
│                                                                         │
│ 4. La următoarea repornire/transmisie                                   │
│    └── Detectorul trimite address=0, id=0                               │
└─────────────────────────────────────────────────────────────────────────┘
```

### De ce apare în v4.0 și nu în v3.0?

Această problemă **exista și în v3.0**, dar era mascată:
- În v3.0, alarmele cu numere mari (corupție RF) erau **mai frecvente**
- Alarmele cu 0 erau **rare** și treceau neobservate printre cele cu numere mari
- În v4.0, cu filtrarea numerelor mari prin `AddressVerificationInterval`, cele cu 0 au devenit **vizibile**

### Verificare

Pentru a confirma acest scenariu:
1. Verifică cu aplicația "Ergo Installer" configurația fiecărui detector
2. Citește valorile EEPROM prin comanda serială `GET_ALL_VALUES` (ID=20)
3. Caută detectoare cu address=0 sau id=0

### Probabilitate: **MEDIE** (40%)

---

## Rezumat Comparativ

| Aspect | Scenariul 1 | Scenariul 2 | Scenariul 3 |
|--------|-------------|-------------|-------------|
| **Cauza principală** | Buffer neinițializat | Coliziune RF | EEPROM corupt/neprogramat |
| **Nivel** | Software (FM) | Hardware/RF | Hardware (Detector) |
| **Frecvență** | Sporadică | Crește cu nr. detectoare | Constantă per detector |
| **Reproductibilitate** | Dificilă | Aleatorie | Ușor de verificat |
| **Fix rapid** | Da (validare length) | Nu (hardware) | Da (reprogramare) |
| **Probabilitate** | 70% | 50% | 40% |

---

## Recomandări de Investigare

### Pas 1: Activare Debug Serial
```cpp
#define DEBUG_ENABLED true
```
Logare automată a tuturor pachetelor primite (brut).

### Pas 2: Adăugare Validări în Cod

**Validare lungime minimă:**
```cpp
if (*receiveBufferLen >= 8) {
    // procesare
}
```

**Validare adresă != 0:**
```cpp
if (alarmEntry.address == 0 || alarmEntry.id == 0) {
    debug->println(F("WARN: Alarm with address/id 0 ignored"));
    return;
}
```

### Pas 3: Audit Detectoare
- Verifică programarea tuturor detectoarelor din rețea
- Identifică detectoare cu valori default/invalide

### Pas 4: Test Izolat
- Rulează sistemul cu un singur detector
- Dacă problema persistă → Scenariul 1 (bug software)
- Dacă dispare → Scenariul 2 sau 3 (coliziune/detector defect)

---

*Document generat pentru analiza problemei "Alarme cu ID și Adresă 0" în versiunea 4.0 a sistemului ERGO26.*
