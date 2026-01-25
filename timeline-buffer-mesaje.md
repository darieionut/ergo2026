# Timeline Buffer Mesaje - Sistem ERGO26 Floor Module

## Tipuri de Memorie Implicate

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        HARTA MEMORIEI SISTEM                                     │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                     MODUL CC1101 (RF Transceiver)                        │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  RX FIFO - 64 bytes                                             │    │    │
│  │  │  ══════════════════                                             │    │    │
│  │  │  • Buffer hardware intern CC1101                                │    │    │
│  │  │  • Accesibil prin SPI (readBurstReg)                            │    │    │
│  │  │  • Se golește automat după citire                               │    │    │
│  │  │  • Conține: length + data + RSSI + LQI/CRC                      │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  TX FIFO - 64 bytes                                             │    │    │
│  │  │  ══════════════════                                             │    │    │
│  │  │  • Buffer pentru transmisie                                     │    │    │
│  │  │  • Scris prin SPI (writeBurstReg)                               │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────────────────┘    │
│                                      │                                           │
│                                      │ SPI (10 MHz)                              │
│                                      ▼                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                    MCU ATmega328PB - RAM (2 KB)                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  recvBuffer[100] - Adresă: RAM global                           │    │    │
│  │  │  ══════════════════════════════════════                         │    │    │
│  │  │  • 100 bytes rezervați pentru recepție                          │    │    │
│  │  │  • Tipul: uint8_t[]                                             │    │    │
│  │  │  • Persistent între cicluri loop()                              │    │    │
│  │  │  • ⚠️  NU ESTE INIȚIALIZAT EXPLICIT LA PORNIRE                  │    │    │
│  │  │                                                                 │    │    │
│  │  │  Structura pachet în buffer (8 bytes pentru alarmă):            │    │    │
│  │  │  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐              │    │    │
│  │  │  │  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │              │    │    │
│  │  │  ├─────┴─────┼─────┴─────┼─────┴─────┼─────┼─────┤              │    │    │
│  │  │  │ RECEIVER  │  ADDRESS  │    ID     │ CMD │ FWD │              │    │    │
│  │  │  │  uint16   │  uint16   │  uint16   │ u8  │ u8  │              │    │    │
│  │  │  └───────────┴───────────┴───────────┴─────┴─────┘              │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  sendBuffer[15] - Adresă: RAM global                            │    │    │
│  │  │  ═══════════════════════════════════                            │    │    │
│  │  │  • 15 bytes pentru transmisie                                   │    │    │
│  │  │  • Tipul: uint8_t[]                                             │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  recvBufferLen, sendBufferLen - Adresă: RAM global              │    │    │
│  │  │  ══════════════════════════════════════════════                 │    │    │
│  │  │  • uint8_t - contoare lungime                                   │    │    │
│  │  │  • recvBufferLen = 0 la startup (setup(), linia 125)            │    │    │
│  │  │  • ⚠️  recvBuffer[] NU E RESETAT când len devine 0              │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  packetWaiting - volatile boolean                               │    │    │
│  │  │  ════════════════════════════════                               │    │    │
│  │  │  • Flag setat de ISR (interrupt)                                │    │    │
│  │  │  • Citit în loop principal                                      │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────────────────┘    │
│                                      │                                           │
│                                      │ Pointeri                                  │
│                                      ▼                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                  SRAM Extern 23LC512 (64 KB) - SPI                       │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  Zona Alarme: 0x0000 - 0x17A2 (6050 bytes)                      │    │    │
│  │  │  ════════════════════════════════════════                       │    │    │
│  │  │  • Capacitate: ~1210 alarme                                     │    │    │
│  │  │  • Segment size: 5 bytes/alarmă                                 │    │    │
│  │  │  │  address (2B) + id (2B) + state (1B)                         │    │    │
│  │  │  • Persistență: DOAR în timpul funcționării                     │    │    │
│  │  │  • Se pierde la power-off (SRAM volatil)                        │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  Zona Detectoare: 0x17A2 - 0xD332 (48000 bytes)                 │    │    │
│  │  │  ═══════════════════════════════════════════                    │    │    │
│  │  │  • Capacitate: ~4800 detectoare                                 │    │    │
│  │  │  • Segment size: 10 bytes/detector                              │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  Zona Offline: 0xD332 - 0xF8A2 (9600 bytes)                     │    │    │
│  │  │  ═════════════════════════════════════════                      │    │    │
│  │  │  • Capacitate: ~4800 indecși                                    │    │    │
│  │  │  • Segment size: 2 bytes/index                                  │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────────────────┘    │
│                                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                   EEPROM 24LC04B (512 bytes) - I2C                       │    │
│  │  ══════════════════════════════════════════════════                     │    │
│  │  • Stocare configurări permanente                                       │    │
│  │  • Backup alarme (max 10)                                               │    │
│  │  • Address Verification Intervals                                       │    │
│  │  • Persistență: Permanentă (non-volatil)                                │    │
│  └─────────────────────────────────────────────────────────────────────────┘    │
│                                                                                  │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## Timeline: Ciclul de Viață al unui Mesaj RF

```
TIMP (ms)
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │                    DETECTORUL TRANSMITE                                  │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+0 │   ══════════════════════════════════════════════════════════════════════════
    │   DETECTOR: Gaz detectat → Alarm confirmată (6 citiri consecutive)
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ AlarmService::sendAlarmStartPackage()                                   │
    │   │                                                                          │
    │   │ 1. Construiește pachet în sendBuffer[15] (RAM Detector):                │
    │   │    [0-1] receiverAddress = 0x0001 (Floor Module)                        │
    │   │    [2-3] deviceAddress   = 0x000F (Detector #15)                        │
    │   │    [4-5] deviceId        = 0x0003 (ID #3)                               │
    │   │    [6]   command         = 0x01 (ALARM_START)                           │
    │   │    [7]   isForward       = 0x00                                         │
    │   │                                                                          │
    │   │ 2. CC1101Controller::sendData(packet)                                   │
    │   │    └─► Scrie în TX FIFO CC1101 prin SPI                                 │
    │   │    └─► Comutare TX state → transmisie RF 868 MHz                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+5 │   ══════════════════════════════════════════════════════════════════════════
    │   FLOOR MODULE: Semnal RF recepționat
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CC1101 RX FIFO (Hardware - pe modulul RF)                               │
    │   │                                                                          │
    │   │ Pachet stocat în FIFO intern (64 bytes max):                            │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ LEN │ DATA (8 bytes)                      │ RSSI │ LQI+CRC │    │     │
    │   │ │ 08  │ 01 00 │ 0F 00 │ 03 00 │ 01 │ 00    │ -70  │ 80+OK   │    │     │
    │   │ │     │RECV   │ADDR   │ ID    │CMD │FWD    │ dBm  │         │    │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   │                                                                          │
    │   │ GDO2 pin → LOW → Trigger interrupt pe MCU                               │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+5 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ ISR: ihrMessageReceived() [ergo-floor-module.ino:69-71]                 │
    │   │                                                                          │
    │   │ void ihrMessageReceived() {                                             │
    │   │     packetWaiting = true;  ◄── Flag setat în RAM MCU (volatile)         │
    │   │ }                                                                        │
    │   │                                                                          │
    │   │ Timp execuție: ~1-2 µs                                                  │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │                    ⏳ AȘTEPTARE LOOP 200Hz                               │
    │   │                    (max 5ms între iterații)                              │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+10│   ══════════════════════════════════════════════════════════════════════════
    │   LOOP 200Hz: cc1101Receive() [ergo-floor-module.ino:77-85]
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ void cc1101Receive() {                                                  │
    │   │     if (packetWaiting) {                          ◄── Verifică flag     │
    │   │         detachInterrupt(CC1101_Interrupt);        ◄── Dezactivează ISR  │
    │   │         communicationService.receive(recvBuffer, &recvBufferLen);       │
    │   │         lastCommunicationRX = millis();                                 │
    │   │         packetWaiting = false;                    ◄── Reset flag        │
    │   │         attachInterrupt(...);                     ◄── Reactivează ISR   │
    │   │     }                                                                   │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+10│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CommunicationService::receive() → CC1101Controller::receiveData()       │
    │   │ [common/Controllers.cpp:328-352]                                        │
    │   │                                                                          │
    │   │ uint8_t CC1101Controller::receiveData(CCPACKET *packet) {               │
    │   │     rxBytes = readStatusReg(CC1101_RXBYTES);    ◄── SPI read            │
    │   │                                                                          │
    │   │     if (rxBytes & 0x7F && !(rxBytes & 0x80)) {  ◄── Verifică bytes      │
    │   │         packet->length = readConfigReg(CC1101_RXFIFO); ◄── Citește LEN  │
    │   │                                                                          │
    │   │         if (packet->length > 30)                                        │
    │   │             packet->length = 0;                 ◄── ⚠️ Pachet INVALID   │
    │   │         else {                                                          │
    │   │             readBurstReg(packet->data, ..., packet->length);            │
    │   │             │                                                           │
    │   │             │  ┌─────────────────────────────────────────────────────┐  │
    │   │             └─►│ TRANSFER SPI: CC1101 FIFO → packet.data (RAM MCU)  │  │
    │   │                │                                                     │  │
    │   │                │ CC1101 FIFO          packet.data (CCPACKET)         │  │
    │   │                │ ┌───────────┐        ┌───────────┐                  │  │
    │   │                │ │01 00 0F 00│ ────►  │01 00 0F 00│  bytes 0-3      │  │
    │   │                │ │03 00 01 00│ ────►  │03 00 01 00│  bytes 4-7      │  │
    │   │                │ └───────────┘        └───────────┘                  │  │
    │   │                └─────────────────────────────────────────────────────┘  │
    │   │                                                                          │
    │   │             packet->rssi = readConfigReg(CC1101_RXFIFO);                │
    │   │             packet->lqi = val & 0x7F;                                   │
    │   │             packet->crc_ok = bitRead(val, 7);                           │
    │   │         }                                                               │
    │   │     } else                                                              │
    │   │         packet->length = 0;                     ◄── ⚠️ Nimic în FIFO   │
    │   │                                                                          │
    │   │     flushRxFifo();                              ◄── Golește FIFO CC1101 │
    │   │     return packet->length;                                              │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+11│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CommunicationService::receive() continuare                              │
    │   │                                                                          │
    │   │ COPIERE: CCPACKET.data → recvBuffer (RAM global)                        │
    │   │                                                                          │
    │   │ CCPACKET packet (stack)    recvBuffer[100] (RAM global)                 │
    │   │ ┌───────────────────┐      ┌───────────────────────────────────────┐    │
    │   │ │ length: 8         │      │ [0] 01  ◄── RECEIVER_LO               │    │
    │   │ │ data[0-7]:        │ ───► │ [1] 00  ◄── RECEIVER_HI               │    │
    │   │ │   01 00 0F 00     │      │ [2] 0F  ◄── ADDRESS_LO  ⚠️ POATE FI 0 │    │
    │   │ │   03 00 01 00     │      │ [3] 00  ◄── ADDRESS_HI                │    │
    │   │ │ rssi: -70         │      │ [4] 03  ◄── ID_LO       ⚠️ POATE FI 0 │    │
    │   │ │ lqi: 85           │      │ [5] 00  ◄── ID_HI                     │    │
    │   │ │ crc_ok: true      │      │ [6] 01  ◄── COMMAND                   │    │
    │   │ └───────────────────┘      │ [7] 00  ◄── IS_FORWARD                │    │
    │   │                             │ [8-99] ?? ◄── NEDEFINIT/VECHI         │    │
    │   │                             └───────────────────────────────────────┘    │
    │   │                                                                          │
    │   │ recvBufferLen = 8;  ◄── Setează lungimea validă                         │
    │   │                                                                          │
    │   │ ⚠️ NOTĂ: Dacă packet.length era 0, recvBuffer NU E MODIFICAT            │
    │   │          dar recvBufferLen POATE fi setat greșit (depinde de impl.)     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │                    ⏳ AȘTEPTARE LOOP 100Hz                               │
    │   │                    (max 10ms între iterații)                             │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+20│   ══════════════════════════════════════════════════════════════════════════
    │   LOOP 100Hz: coreService.tick() [floor-module/Services.cpp:445-456]
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ void CoreService::tick(uint32_t currentMillis) {                        │
    │   │     if (*receiveBufferLen > 0) {                ◄── Verifică len=8      │
    │   │                                                                          │
    │   │         ⚠️ PROBLEMĂ: NU verifică dacă len >= 8 (min pentru alarmă)      │
    │   │         ⚠️ PROBLEMĂ: NU verifică crc_ok (e pierdut la copiere)          │
    │   │                                                                          │
    │   │         if (operationMode == OPERATION_MODE_MASTER) {                   │
    │   │             operationModeMaster(currentMillis);                         │
    │   │         }                                                               │
    │   │         *receiveBufferLen = 0;                  ◄── ⚠️ RESETEAZĂ DOAR   │
    │   │     }                                                LUNGIMEA, NU DATELE│
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+20│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CoreService::operationModeMaster() [floor-module/Services.cpp:458-483]  │
    │   │                                                                          │
    │   │ 1. EXTRAGERE RECEIVER ADDRESS din recvBuffer:                           │
    │   │    convertBytesToUInt.byte[0] = recvBuffer[0];  // 0x01                 │
    │   │    convertBytesToUInt.byte[1] = recvBuffer[1];  // 0x00                 │
    │   │    → receiverAddress = 0x0001                                           │
    │   │                                                                          │
    │   │ 2. VERIFICARE: receiverAddress == deviceAddress (0x0001)?               │
    │   │    → DA, pachetul e pentru mine                                         │
    │   │                                                                          │
    │   │ 3. EXTRAGERE DETECTOR ADDRESS din recvBuffer:                           │
    │   │    convertBytesToUInt.byte[0] = recvBuffer[2];  // 0x0F                 │
    │   │    convertBytesToUInt.byte[1] = recvBuffer[3];  // 0x00                 │
    │   │    → detectorAddress = 0x000F (15)                                      │
    │   │                                                                          │
    │   │    ⚠️ DACĂ recvBuffer[2-3] = 0x00 0x00 → address = 0                    │
    │   │                                                                          │
    │   │ 4. VERIFICARE COMMAND:                                                  │
    │   │    if (recvBuffer[6] == COMMAND_ID_ALARM_START) {                       │
    │   │        addReceiveToDetectorsList(...);                                  │
    │   │        addReceiveToAlarmsList(...);             ◄── SCRIE ÎN SRAM       │
    │   │        outputController->set(RELAY_AC, true);   ◄── ACTIVEAZĂ RELEU     │
    │   │    }                                                                    │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+21│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CoreService::addReceiveToAlarmsList() → SRAMController::addAlarm()      │
    │   │                                                                          │
    │   │ TRANSFER: recvBuffer (RAM MCU) → SRAM Extern 23LC512                    │
    │   │                                                                          │
    │   │ recvBuffer[100]              AlarmEntry           SRAM 23LC512          │
    │   │ (RAM MCU)                    (stack temp)         (SPI extern)          │
    │   │ ┌─────────────┐              ┌─────────────┐      ┌─────────────┐       │
    │   │ │[2] 0F       │─►address─────│address:0x0F │─────►│ Addr 0x0000 │       │
    │   │ │[3] 00       │              │id: 0x03     │      │ 0F 00 03 00 │       │
    │   │ │[4] 03       │─►id──────────│state: true  │      │ 01          │       │
    │   │ │[5] 00       │              └─────────────┘      └─────────────┘       │
    │   │ └─────────────┘                                   5 bytes/alarmă        │
    │   │                                                                          │
    │   │ SPI Write:                                                              │
    │   │   1. CS pin LOW                                                         │
    │   │   2. Comandă WRITE (0x02)                                               │
    │   │   3. Adresă 16-bit                                                      │
    │   │   4. 5 bytes date                                                       │
    │   │   5. CS pin HIGH                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+22│   ══════════════════════════════════════════════════════════════════════════
    │   MESAJUL PROCESAT - BUFFER RESETAT
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ După procesare:                                                         │
    │   │                                                                          │
    │   │ recvBuffer[100] (RAM MCU):                                              │
    │   │ ┌───────────────────────────────────────────────────────────────────┐   │
    │   │ │ [0] 01  │ ◄── DATELE VECHI RĂMÂN!                                 │   │
    │   │ │ [1] 00  │                                                         │   │
    │   │ │ [2] 0F  │ ◄── Address veche rămâne în memorie                     │   │
    │   │ │ [3] 00  │                                                         │   │
    │   │ │ [4] 03  │ ◄── ID vechi rămâne în memorie                          │   │
    │   │ │ [5] 00  │                                                         │   │
    │   │ │ [6] 01  │                                                         │   │
    │   │ │ [7] 00  │                                                         │   │
    │   │ └───────────────────────────────────────────────────────────────────┘   │
    │   │                                                                          │
    │   │ recvBufferLen = 0;  ◄── DOAR LUNGIMEA E RESETATĂ                        │
    │   │                                                                          │
    │   │ ⚠️ RISC: Dacă următorul pachet are length < 8, datele vechi             │
    │   │          se vor combina cu cele noi → valori corupte                     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    ▼
```

---

## Scenariul Problematic: Cum Apare Address = 0

```
TIMP (ms)
    │
T+0 │   ══════════════════════════════════════════════════════════════════════════
    │   SITUAȚIE: Pachet RF trunchiat sau interferență
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CC1101 primește semnal RF corupt                                        │
    │   │                                                                          │
    │   │ RX FIFO CC1101 (parțial populat):                                       │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ LEN │ DATA                               │ RSSI │ LQI    │      │     │
    │   │ │ 02  │ 01 00 │ ?? ?? │ ?? ?? │ ?? │ ??   │ -90  │ 20+BAD │      │     │
    │   │ │     │ RECV  │ LIPSĂ │ LIPSĂ │LIPSĂ│LIPSĂ│ slab │ CRC NG │      │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   │                                                                          │
    │   │ CRC FAIL dar GDO2 tot generează interrupt!                              │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+5 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CC1101Controller::receiveData()                                         │
    │   │                                                                          │
    │   │ packet->length = 2;  (doar 2 bytes primiți)                             │
    │   │ readBurstReg(packet->data, ..., 2);  ◄── Citește doar bytes 0-1         │
    │   │                                                                          │
    │   │ packet->crc_ok = false;  ◄── CRC invalid, DAR NU E VERIFICAT NICĂIERI!  │
    │   │                                                                          │
    │   │ CCPACKET.data[] după citire:                                            │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ [0] 01  │ ◄── Receiver LO (valid)                               │     │
    │   │ │ [1] 00  │ ◄── Receiver HI (valid)                               │     │
    │   │ │ [2] ??  │ ◄── Address LO - NEDEFINIT (valoare veche sau 0)      │     │
    │   │ │ [3] ??  │ ◄── Address HI - NEDEFINIT                            │     │
    │   │ │ [4] ??  │ ◄── ID LO - NEDEFINIT                                 │     │
    │   │ │ [5] ??  │ ◄── ID HI - NEDEFINIT                                 │     │
    │   │ │ [6] ??  │ ◄── Command - NEDEFINIT                               │     │
    │   │ │ [7] ??  │ ◄── IsForward - NEDEFINIT                             │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+6 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CommunicationService::receive() copiază în recvBuffer                   │
    │   │                                                                          │
    │   │ VARIANTA A: Copiază doar length bytes                                   │
    │   │ memcpy(recvBuffer, packet.data, packet.length);  // doar 2 bytes        │
    │   │ recvBufferLen = 2;                                                      │
    │   │                                                                          │
    │   │ recvBuffer[] după copiere:                                              │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ [0] 01  │ ◄── NOU: Receiver LO                                  │     │
    │   │ │ [1] 00  │ ◄── NOU: Receiver HI                                  │     │
    │   │ │ [2] 0F  │ ◄── VECHI: Address LO de la pachetul anterior        │     │
    │   │ │ [3] 00  │ ◄── VECHI                                            │     │
    │   │ │ [4] 03  │ ◄── VECHI: ID de la pachetul anterior                │     │
    │   │ │ [5] 00  │ ◄── VECHI                                            │     │
    │   │ │ [6] 01  │ ◄── VECHI: Command de la pachetul anterior           │     │
    │   │ │ [7] 00  │ ◄── VECHI                                            │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   │                                                                          │
    │   │ VARIANTA B: Copiază tot (dacă implementare diferită)                    │
    │   │ Dacă CCPACKET.data[] era inițializat cu 0:                              │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ [0] 01  │ ◄── Receiver LO                                       │     │
    │   │ │ [1] 00  │ ◄── Receiver HI                                       │     │
    │   │ │ [2] 00  │ ◄── Address LO = 0 (neinițializat)                    │     │
    │   │ │ [3] 00  │ ◄── Address HI = 0                                    │     │
    │   │ │ [4] 00  │ ◄── ID LO = 0                                         │     │
    │   │ │ [5] 00  │ ◄── ID HI = 0                                         │     │
    │   │ │ [6] 00  │ ◄── Command = 0                                       │     │
    │   │ │ [7] 00  │ ◄── IsForward = 0                                     │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+15│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CoreService::tick() - PROCESARE PACHET INVALID                          │
    │   │                                                                          │
    │   │ if (*receiveBufferLen > 0) {  ◄── len=2, trece verificarea!             │
    │   │                                                                          │
    │   │ ⚠️ BUG: Nu verifică len >= 8                                            │
    │   │ ⚠️ BUG: Nu verifică CRC (info pierdută la copiere)                      │
    │   │                                                                          │
    │   │ operationModeMaster():                                                  │
    │   │ - Citește recvBuffer[2-3] = 0x00 0x00 → address = 0                     │
    │   │ - Citește recvBuffer[4-5] = 0x00 0x00 → id = 0                          │
    │   │ - Citește recvBuffer[6] = 0x00 sau 0x01 → command                       │
    │   │                                                                          │
    │   │ REZULTAT: Alarmă stocată cu address=0, id=0                             │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    ▼
```

---

## Rezumat Locații Buffer

| Buffer | Memorie | Dimensiune | Persistență | Risc |
|--------|---------|------------|-------------|------|
| CC1101 RX FIFO | Hardware CC1101 | 64 bytes | Până la citire | Corupție RF |
| CC1101 TX FIFO | Hardware CC1101 | 64 bytes | Până la transmisie | - |
| CCPACKET.data | RAM MCU (stack) | 30 bytes | Doar în funcție | Neinițializat |
| `recvBuffer` | RAM MCU (global) | 100 bytes | Permanent | **Date vechi rămân** |
| `sendBuffer` | RAM MCU (global) | 15 bytes | Permanent | - |
| Alarme | SRAM 23LC512 | 6050 bytes | Runtime | Se pierde la reset |
| Detectoare | SRAM 23LC512 | 48000 bytes | Runtime | Se pierde la reset |
| Config | EEPROM I2C | 512 bytes | Permanent | - |

---

## Puncte Critice Identificate

1. **`recvBuffer` nu e curățat** - Doar `recvBufferLen` e resetat, datele vechi persistă
2. **CRC nu e verificat** - `packet.crc_ok` e ignorat la procesare
3. **Lungime minimă nu e validată** - Se procesează pachete cu len < 8
4. **`CCPACKET.data[]` nu e inițializat** - Poate conține 0-uri sau garbage

---

*Document generat pentru analiza fluxului de date în sistemul ERGO26 Floor Module.*
