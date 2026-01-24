# Analiză Bug: Adrese Aleatoare 5 Cifre pe Display

## Problema Raportată

Pe display-ul MASTER apar alarme cu adrese de 5 cifre aleatorii în loc de 3 cifre așteptate.

**Exemplu:**
- Așteptat: `123`, `456`, `789`
- Afișat: `54321`, `12847`, `65535`

---

## Cauza Identificată: RACE CONDITION în SRAMController

### Descriere Tehnică

Bug-ul este cauzat de **variabile partajate nesincronizate** în clasa `SRAMController` care sunt accesate simultan din mai multe locuri în main loop.

### Localizare Cod

```
floor-module/Controllers.h (liniile 110-119):
─────────────────────────────────────────────────────────────────────────────
class SRAMController {
private:
    ...
    ConvertBytesToUInt convertBytesToUInt;  // ← PROBLEMA #1: Variabilă partajată

    boolean anyErrorsExist, foundElement, isStateChange, lastState;
    uint16_t i, j, index;                    // ← PROBLEMA #2: Contori partajați

    uint8_t alarmAuxBuffer[5];               // ← PROBLEMA #3: Buffer partajat
    uint8_t detectorAuxBuffer[10];
    uint8_t offlineDetectorAuxBuffer[2];
    uint8_t comparatorAuxBuffer[2];
    ...
};
```

---

## Mecanismul Bug-ului

### Secvența de Evenimente

```
┌────────────────────────────────────────────────────────────────────────────┐
│                    RACE CONDITION - TIMELINE                                │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Main Loop timing:                                                         │
│   - menuService.tick() rulează la 10Hz (o dată la 100ms)                    │
│   - coreService.tick() rulează la 100Hz (o dată la 10ms)                    │
│                                                                             │
│   Timeline (milisecunde):                                                   │
│   ─────────────────────────────────────────────────────────────────────── │
│                                                                             │
│   T=0ms    ┌─────────────────────────────────────────────────────────────┐ │
│            │ menuService.tick() începe                                   │ │
│            │ ├── Apelează sramController->getAlarm(&alarmEntry, idx)     │ │
│            │ │   ├── Citește 5 bytes din SRAM în alarmAuxBuffer          │ │
│            │ │   │   for(i=0; i<5; i++)                                  │ │
│            │ │   │       sram->get(..., alarmAuxBuffer[i]);   ← CITIRE   │ │
│            └─┴───┴───────────────────────────────────────────────────────┘ │
│                                                                             │
│   T=10ms   ┌─────────────────────────────────────────────────────────────┐ │
│            │ coreService.tick() ÎNTRERUPE (pachet RF primit)              │ │
│            │ ├── Apelează sramController->findAlarm(address)             │ │
│            │ │   ├── Scrie în convertBytesToUInt                ← SCRIERE│ │
│            │ │   ├── Scrie în alarmAuxBuffer[0], [1]            ← SCRIERE│ │
│            │ │   │                                                       │ │
│            │ │   │   *** alarmAuxBuffer CORUPT! ***                      │ │
│            │ │   │                                                       │ │
│            └─┴───┴───────────────────────────────────────────────────────┘ │
│                                                                             │
│   T=11ms   ┌─────────────────────────────────────────────────────────────┐ │
│            │ menuService.tick() CONTINUĂ (nu știe de corupție)           │ │
│            │ │   ├── Convertește bytes în adresă:                        │ │
│            │ │   │   convertBytesToUInt.byte[0] = alarmAuxBuffer[0];     │ │
│            │ │   │   convertBytesToUInt.byte[1] = alarmAuxBuffer[1];     │ │
│            │ │   │   alarm->address = convertBytesToUInt.intVal;         │ │
│            │ │   │                                                       │ │
│            │ │   │   *** ADRESA = VALOARE ALEATOARE (bytes corupți) ***  │ │
│            │ │   │                                                       │ │
│            │ ├── lcd->print(alarmEntry.address);  → Afișează "54321"    │ │
│            └─┴───────────────────────────────────────────────────────────┘ │
│                                                                             │
└────────────────────────────────────────────────────────────────────────────┘
```

### Funcțiile Implicate

**1. getAlarm() - CITITOR (victimă)**
```cpp
// floor-module/Controllers.cpp:139-156
boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    if (elemIndex < numberOfAlarms) {
        for (i = 0; i < 5; i++) {                                    // ← i = partajat
            sram->get(SRAM_ALARMS_MEMORY_START +
                (elemIndex * SRAM_ALARMS_MEMORY_SEGMENT_SIZE) + i,
                alarmAuxBuffer[i]);                                  // ← buffer partajat
        }

        // ÎN ACEST PUNCT alarmAuxBuffer POATE FI CORUPT!

        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];              // ← union partajat
        convertBytesToUInt.byte[1] = alarmAuxBuffer[1];
        alarm->address = convertBytesToUInt.intVal;                  // ← VALOARE CORUPTĂ
        ...
    }
}
```

**2. findAlarm() - SCRIITOR (agresor)**
```cpp
// floor-module/Controllers.cpp:158-173
int16_t SRAMController::findAlarm(uint16_t address) {
    convertBytesToUInt.intVal = address;                             // ← SUPRASCRIE union
    comparatorAuxBuffer[0] = convertBytesToUInt.byte[0];
    comparatorAuxBuffer[1] = convertBytesToUInt.byte[1];

    for (j = 0; j < numberOfAlarms; j++) {                           // ← j = partajat
        sram->get(..., alarmAuxBuffer[0]);                           // ← SUPRASCRIE buffer!
        sram->get(..., alarmAuxBuffer[1]);                           // ← SUPRASCRIE buffer!
        ...
    }
}
```

---

## De Ce Apar 5 Cifre

### Matematica Bug-ului

```
Adresă normală:    100-999 (3 cifre)
Reprezentare:      uint16_t = 2 bytes

Când bytes sunt corupți:
┌─────────────────────────────────────────────────────────────────────────────┐
│   Byte[0] (Low)     Byte[1] (High)      Valoare uint16_t                    │
│   ─────────────     ────────────────    ────────────────                    │
│   0x64 (100)        0x00                100          ← CORECT (3 cifre)     │
│   0xE8 (232)        0x03                1000         ← CORECT (4 cifre)     │
│                                                                              │
│   CORUPT:                                                                    │
│   0xFF (255)        0xFF                65535        ← CORUPT (5 cifre)     │
│   0x21 (33)         0xD4 (212)          54305        ← CORUPT (5 cifre)     │
│   0x0F (15)         0x32 (50)           12815        ← CORUPT (5 cifre)     │
└─────────────────────────────────────────────────────────────────────────────┘

Probabilitate 5 cifre = valori > 9999 = (65535-9999)/65535 = 85%
```

---

## Condiții de Reproducere

Bug-ul apare când:

1. **Trafic RF activ** - pachete care declanșează `coreService.tick()` să proceseze
2. **Meniu pe pagina LIST_ALARMS** - care apelează `getAlarm()` frecvent
3. **Timing "nefericit"** - citirea SRAM întreruptă de scriere

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    PROBABILITATE REPRODUCERE                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│   Factori:                                                                   │
│   ─────────────────────────────────────────────────────────────────────────  │
│   • Pachete RF primite: 18-70 pkt/s (depinde de numărul detectoare)          │
│   • Menu refresh: 10 Hz (constant)                                           │
│   • Timp critic per refresh: ~5ms (citire SRAM + conversie)                  │
│                                                                              │
│   Calculul probabilității:                                                   │
│   ─────────────────────────────────────────────────────────────────────────  │
│   P(coliziune) = (pachete/s) × (timp_critic) / 1000                          │
│                                                                              │
│   200 detectoare: P = 18.3 × 5 / 1000 = 0.09 = 9% per refresh                │
│   400 detectoare: P = 70 × 5 / 1000 = 0.35 = 35% per refresh                 │
│                                                                              │
│   OBSERVAȚIE: Bug-ul apare MAI DES cu trafic mai mare!                       │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Locații Toate Variabilele Partajate

### În SRAMController

| Variabilă | Tip | Folosită în | Risc |
|-----------|-----|-------------|------|
| `convertBytesToUInt` | union | getAlarm, addAlarm, findAlarm, getDetector, addDetector, findDetector | 🔴 CRITIC |
| `alarmAuxBuffer[5]` | uint8_t[] | getAlarm, addAlarm, findAlarm, deleteAlarm | 🔴 CRITIC |
| `detectorAuxBuffer[10]` | uint8_t[] | getDetector, addDetector, deleteDetector | 🔴 CRITIC |
| `comparatorAuxBuffer[2]` | uint8_t[] | findAlarm, findDetector, findOfflineDetector | 🟡 MEDIU |
| `i`, `j`, `index` | uint16_t | Toate metodele cu loop | 🔴 CRITIC |

### În CoreService (Services.h:100-102)

| Variabilă | Tip | Folosită în | Risc |
|-----------|-----|-------------|------|
| `convertBytesToUInt` | union | operationModeMaster, addReceiveToDetectorsList, addReceiveToAlarmsList | 🟡 MEDIU |
| `i`, `j`, `index` | uint16_t | checkOfflineDetectors | 🟡 MEDIU |

---

## Soluții Propuse

### Soluția 1: Variabile Locale (RECOMANDATĂ)

**Complexitate:** Medie
**Impact:** Minim pe memorie

```cpp
// ÎNAINTE (floor-module/Controllers.cpp):
boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    if (elemIndex < numberOfAlarms) {
        for (i = 0; i < 5; i++) {  // ← i este membru al clasei
            sram->get(..., alarmAuxBuffer[i]);  // ← buffer membru
        }
        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];  // ← union membru
        ...
    }
}

// DUPĂ:
boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    if (elemIndex < numberOfAlarms) {
        uint8_t localBuffer[5];  // ← LOCAL
        ConvertBytesToUInt localConvert;  // ← LOCAL

        for (uint8_t idx = 0; idx < 5; idx++) {  // ← idx LOCAL
            sram->get(..., localBuffer[idx]);
        }
        localConvert.byte[0] = localBuffer[0];
        localConvert.byte[1] = localBuffer[1];
        alarm->address = localConvert.intVal;
        ...
    }
}
```

**Fișiere de modificat:**
- `floor-module/Controllers.cpp` - toate metodele SRAMController
- Estimare: ~2 ore de lucru

### Soluția 2: Mutex/Flag de Blocare

**Complexitate:** Mică
**Impact:** Adaugă mică latență

```cpp
// În Controllers.h:
class SRAMController {
private:
    volatile boolean operationInProgress;  // ← NOU
    ...
};

// În Controllers.cpp:
boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    while (operationInProgress) { /* așteaptă */ }  // ← BLOCARE
    operationInProgress = true;

    // ... operații SRAM ...

    operationInProgress = false;  // ← DEBLOCARE
    return true;
}
```

**Dezavantaj:** Poate cauza blocări dacă nu e implementat corect.

### Soluția 3: Copie Defensivă în MenuService

**Complexitate:** Mică
**Impact:** Cel mai rapid de implementat

```cpp
// În Services.cpp - MenuService::setLine2DynamicText():
case PAGE_INDEX_LIST_ALARMS:
    if (sramController->getAlarm(&alarmEntry, alarmIndex)) {
        // Copie defensivă - citește de 2 ori și compară
        AlarmEntry verify;
        if (sramController->getAlarm(&verify, alarmIndex)) {
            if (verify.address == alarmEntry.address) {
                lcd->print(alarmEntry.address);  // ← Afișează doar dacă consistent
            } else {
                lcd->print(F("---"));  // ← Indică eroare
            }
        }
    }
    break;
```

**Dezavantaj:** Nu rezolvă cauza root, doar maschează simptomul.

### Soluția 4: Sincronizare cu noInterrupts()

**Complexitate:** Mică
**Impact:** Poate afecta recepția RF

```cpp
boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    noInterrupts();  // ← Dezactivează întreruperi

    // ... operații SRAM ...

    interrupts();  // ← Reactivează
    return true;
}
```

**Dezavantaj:** Poate cauza pierderi de pachete RF dacă operația durează prea mult.

---

## Recomandare Finală

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│   RECOMANDARE: Implementați SOLUȚIA 1 (Variabile Locale)                     │
│   ════════════════════════════════════════════════════════════════════════  │
│                                                                              │
│   Motivație:                                                                 │
│   ✅ Rezolvă cauza root (nu doar simptomul)                                  │
│   ✅ Nu introduce overhead de sincronizare                                   │
│   ✅ Nu afectează recepția RF                                                │
│   ✅ Impact RAM: +15 bytes per apel de funcție (acceptabil)                  │
│   ✅ Compatibil cu codul existent                                            │
│                                                                              │
│   Funcții de modificat (prioritate):                                         │
│   1. getAlarm()      - cea mai afectată                                      │
│   2. findAlarm()     - cauzează corupția                                     │
│   3. addAlarm()      - poate cauza corupție                                  │
│   4. getDetector()   - simptome similare posibile                            │
│   5. findDetector()  - cauzează corupție                                     │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Verificare Rapidă

Pentru a confirma că acesta este bug-ul, poți face un test simplu:

```cpp
// Adaugă în loop() temporar pentru test:
void loop() {
    // ... cod existent ...

    // TEST: Dezactivează procesarea RF
    // Comentează linia: coreService.tick(currentMillis);

    // Dacă bug-ul DISPARE = confirmat race condition
}
```

---

*Analiză generată: 24 Ianuarie 2026*
