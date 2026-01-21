# CLAUDE.md - Context pentru Claude Code

## Descriere Proiect

Acesta este un proiect de firmware Arduino pentru un sistem industrial de detectie a gazelor si alarmare. Proiectul contine doua componente hardware separate care comunica wireless.

## Fisiere Principale

| Fisier | Descriere |
|--------|-----------|
| `ergo-floor-module-v4.0.cpp` | Firmware pentru Modulul de Etaj (receptor central) |
| `fw-ergo-detector-source-v2.5.cpp` | Firmware pentru Detectorul de Gaze |

## Arhitectura Sistemului

```
[Detector Gaze] ---(CC1101 868MHz)---> [Modul Etaj]
     |                                      |
  Senzor TGS                            LCD + Meniu
  LED Status                            EEPROM + SRAM
  Buzzer                                Management Alarme
  Releu AC                              Lista Detectoare
```

## Conventii de Cod

- **Limbaj:** C++ pentru Arduino (AVR)
- **Prefixe clase:** Controller (hardware), Service (logica business)
- **Constante:** UPPERCASE cu underscore (ex: `APP_VERSION`, `BTN_COUNT`)
- **Variabile:** camelCase (ex: `currentValue`, `lastUpdateMs`)
- **Tipuri:** Foloseste `uint8_t`, `uint16_t`, `int32_t` etc.

## Clase Importante

### Detector (`fw-ergo-detector-source-v2.5.cpp`)
- `TGSController` - Citire senzor gaz
- `AlarmService` - Gestionare stari alarma
- `BuzzerController` - Control buzzer
- `LEDController` - Indicatori LED

### Floor Module (`ergo-floor-module-v4.0.cpp`)
- `MenuService` - Sistem meniu LCD
- `SRAMController` - Stocare detectoare/alarme in RAM
- `CC1101Controller` - Comunicatie radio

## Comunicatie Radio

- **Modul:** CC1101
- **Frecventa:** 868 MHz
- **Interfata:** SPI
- **Sync Words:** 0xB5, 0x47

## Comenzi de Build

```bash
# Arduino IDE
# 1. Deschide fisierul .cpp
# 2. Selecteaza placa AVR
# 3. Compile & Upload

# PlatformIO
pio run -t upload
```

## Structuri de Date Importante

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
```

## Constante Critice

| Constanta | Valoare | Scop |
|-----------|---------|------|
| `DETECTOR_OFFLINE_MS` | 420000 (7 min) | Timp pana detector e marcat offline |
| `WATCHDOG_RECEIVE_RESET_MS` | 300000 (5 min) | Timeout watchdog |
| `SERIAL_BAUDRATE` | 19200 | Viteza seriala |

## Note pentru Dezvoltare

1. **Watchdog:** Este activ - codul trebuie sa ruleze `wdt_reset()` periodic
2. **EEPROM:** Folosit pentru stocare persistenta (numar serial, setari)
3. **Debug:** Setat `DEBUG_ENABLED false` in productie
4. **Butoane:** Au debounce de 40ms implementat

## Testare

- Foloseste butonul TEST pe detector pentru a simula alarma
- Modulul de etaj afiseaza statusul pe LCD
- Comunicatia seriala la 19200 baud pentru debug
