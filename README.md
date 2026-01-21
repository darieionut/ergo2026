# ERGO 2026 - Sistem de Detectie Gaze si Alarma

Firmware Arduino pentru un sistem industrial de detectie a gazelor si managementul alarmelor. Sistemul consta din doua componente principale: **Detectorul de Gaze** si **Modulul de Etaj (Floor Module)**.

## Descriere Generala

Acest proiect contine codul sursa pentru un sistem wireless de monitorizare si alarmare pentru detectia gazelor, destinat utilizarii in medii industriale sau comerciale.

## Componente

### 1. Detector de Gaze (`fw-ergo-detector-source-v2.5.cpp`)
- **Versiune:** 2.5
- **Tip Dispozitiv:** 1

**Functionalitati:**
- Senzor TGS pentru detectia gazelor
- Sistem de alarma cu valori de declansare configurabile
- Indicatori LED de stare:
  - `OK` - Functionare normala
  - `WARNING` - Avertizare
  - `ERROR` - Eroare
  - `COMM_ERROR` - Eroare comunicatie
- Buzzer pentru alarme sonore
- Iesire releu (RELAY_AC) pentru echipamente externe
- Buton de test
- Mod de service/programare

### 2. Modul de Etaj (`ergo-floor-module-v4.0.cpp`)
- **Versiune:** 4.0
- **Tip Dispozitiv:** 2

**Functionalitati:**
- Unitate centrala de receptie/control
- Afisaj LCD cu sistem de meniu navigabil
- 4 butoane de navigare (BACK, LEFT, RIGHT, OK)
- Management al mai multor detectoare simultan
- Monitorizare detectoare offline
- Afisare si gestionare alarme
- Stocare persistenta in EEPROM extern

## Comunicatie Wireless

Ambele dispozitive utilizeaza modulul radio **CC1101** pentru comunicatie wireless:

| Parametru | Valoare |
|-----------|---------|
| Frecventa | 868 MHz |
| Baud Rate Serial | 19200 |
| Protocol | Sincron (SYNC1: 0xB5, SYNC0: 0x47) |

## Specificatii Tehnice

### Hardware

- **Microcontroler:** AVR (compatibil Arduino)
- **Comunicatie Radio:** CC1101 (868 MHz)
- **Memorie Externa:** EEPROM I2C (extEEPROM)
- **SRAM Externa:** (doar Floor Module)
- **Interfata SPI:** Pentru modulul CC1101

### Timere si Intervale

| Parametru | Valoare | Descriere |
|-----------|---------|-----------|
| WATCHDOG_RECEIVE_RESET_MS | 300000 ms (5 min) | Reset watchdog la receptie |
| DETECTOR_OFFLINE_MS | 420000 ms (7 min) | Timp pana detector offline |
| BTN_DEBOUNCE_PERIOD | 40 ms | Perioada debounce butoane |
| ALARM_STATUS_MESSAGE_RESEND_MIN | 30000 ms | Interval minim retrimitere status |
| ALARM_STATUS_MESSAGE_RESEND_MAX | 120000 ms | Interval maxim retrimitere status |

## Dependente (Biblioteci Arduino)

- `Arduino.h` - Core Arduino
- `SPI.h` - Comunicatie SPI
- `avr/wdt.h` - Watchdog Timer
- `extEEPROM.h` - EEPROM extern I2C

## Structura Codului

### Clase Principale

| Clasa | Descriere |
|-------|-----------|
| `CC1101Controller` | Control modul radio CC1101 |
| `EEPROMController` | Gestionare memorie EEPROM |
| `ButtonController` | Gestionare butoane cu debounce |
| `LEDController` | Control LED-uri de stare |
| `OutputController` | Control iesiri releu |
| `BuzzerController` | Control buzzer (doar detector) |
| `TGSController` | Interfata senzor gaz TGS (doar detector) |
| `SRAMController` | Gestionare SRAM (doar floor module) |
| `MenuService` | Sistem meniu LCD (doar floor module) |
| `AlarmService` | Gestionare alarme |
| `CommunicationService` | Serviciu comunicatie radio |
| `ProgrammingService` | Mod programare/configurare |

## Configurare

### Constante Principale

```cpp
#define APP_VERSION 40           // Versiune aplicatie (Floor Module)
#define APP_VERSION 25           // Versiune aplicatie (Detector)
#define SERIAL_BAUDRATE 19200    // Viteza comunicatie seriala
#define WATCHDOG_ENABLED true    // Watchdog activ
#define DEBUG_ENABLED false      // Debug dezactivat in productie
```

## Instalare si Utilizare

1. **Instalati Arduino IDE** sau PlatformIO
2. **Instalati bibliotecile necesare:**
   - extEEPROM
3. **Selectati placa AVR corespunzatoare**
4. **Compilati si incarcati firmware-ul:**
   - `ergo-floor-module-v4.0.cpp` pentru Modulul de Etaj
   - `fw-ergo-detector-source-v2.5.cpp` pentru Detector

## Autori

Energo Instal

## Licenta

Proprietar - Toate drepturile rezervate.
