# Timeline Buffer Mesaje - Sistem ERGO26 Floor Module

**Bazat pe:** `ergo-floor-module-v4.0.cpp` (cod în producție)

---

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
│  │  │  • Se golește cu flushRxFifo() după citire                      │    │    │
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
│  │                                                                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  CCPACKET packet - Variabilă membru CommunicationService        │    │    │
│  │  │  (ergo-floor-module-v4.0.cpp:654)                               │    │    │
│  │  │  ═══════════════════════════════════════════════════            │    │    │
│  │  │  • Dimensiune: 30 bytes data + 1 length + 1 crc_ok + 1 rssi     │    │    │
│  │  │              + 1 lqi = ~34 bytes                                │    │    │
│  │  │  • Locație: RAM (membru al clasei, nu stack)                    │    │    │
│  │  │  • Persistență: Permanent (instanță globală)                    │    │    │
│  │  │                                                                 │    │    │
│  │  │  struct CCPACKET {                                              │    │    │
│  │  │      uint8_t length;                    // Lungime date         │    │    │
│  │  │      uint8_t data[30];                  // Date utile           │    │    │
│  │  │      boolean crc_ok;                    // Validare CRC         │    │    │
│  │  │      int8_t rssi;                       // Putere semnal        │    │    │
│  │  │      uint8_t lqi;                       // Calitate legătură    │    │    │
│  │  │  };                                                             │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │                                                                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  recvBuffer[100] - Variabilă globală                            │    │    │
│  │  │  (ergo-floor-module-v4.0.cpp:962)                               │    │    │
│  │  │  ══════════════════════════════════════                         │    │    │
│  │  │  • 100 bytes rezervați pentru recepție                          │    │    │
│  │  │  • Tipul: uint8_t[]                                             │    │    │
│  │  │  • Locație: RAM global (secțiunea .bss)                         │    │    │
│  │  │  • Persistență: Permanent între cicluri loop()                  │    │    │
│  │  │  • Inițializat cu 0 la pornire (C++ standard)                   │    │    │
│  │  │                                                                 │    │    │
│  │  │  ⚠️  BUFFER ACUMULATIV - poate conține MULTIPLE pachete!        │    │    │
│  │  │                                                                 │    │    │
│  │  │  Structura unui pachet în buffer (8 bytes):                     │    │    │
│  │  │  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐              │    │    │
│  │  │  │  0  │  1  │  2  │  3  │  4  │  5  │  6  │  7  │              │    │    │
│  │  │  ├─────┴─────┼─────┴─────┼─────┴─────┼─────┼─────┤              │    │    │
│  │  │  │ RECEIVER  │  ADDRESS  │    ID     │ CMD │ FWD │              │    │    │
│  │  │  │  uint16   │  uint16   │  uint16   │ u8  │ u8  │              │    │    │
│  │  │  └───────────┴───────────┴───────────┴─────┴─────┘              │    │    │
│  │  │                                                                 │    │    │
│  │  │  Când buffer conține 2 pachete (recvBufferLen = 16):            │    │    │
│  │  │  ┌─────────────────────────┬─────────────────────────┐          │    │    │
│  │  │  │ Pachet 1 (bytes 0-7)   │ Pachet 2 (bytes 8-15)   │          │    │    │
│  │  │  └─────────────────────────┴─────────────────────────┘          │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │                                                                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  sendBuffer[15] - Variabilă globală                             │    │    │
│  │  │  (ergo-floor-module-v4.0.cpp:962)                               │    │    │
│  │  │  ═══════════════════════════════════                            │    │    │
│  │  │  • 15 bytes pentru transmisie                                   │    │    │
│  │  │  • Tipul: uint8_t[]                                             │    │    │
│  │  │  • Locație: RAM global                                          │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │                                                                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  recvBufferLen, sendBufferLen - Variabile globale               │    │    │
│  │  │  (ergo-floor-module-v4.0.cpp:963)                               │    │    │
│  │  │  ══════════════════════════════════════════════                 │    │    │
│  │  │  • uint8_t - contoare lungime                                   │    │    │
│  │  │  • recvBufferLen = 0 la setup() (linia 1028)                    │    │    │
│  │  │  • Se incrementează cu fiecare pachet primit                    │    │    │
│  │  │  • Se resetează la 0 după procesare completă                    │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │                                                                          │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  packetWaiting - volatile boolean                               │    │    │
│  │  │  (ergo-floor-module-v4.0.cpp:961)                               │    │    │
│  │  │  ════════════════════════════════                               │    │    │
│  │  │  • Flag setat de ISR (interrupt handler)                        │    │    │
│  │  │  • volatile pentru acces din ISR și main loop                   │    │    │
│  │  │  • Citit în cc1101Receive() la 200Hz                            │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────────────────────────┘    │
│                                      │                                           │
│                                      │ Pointeri (setReceiveBuffer)               │
│                                      ▼                                           │
│  ┌─────────────────────────────────────────────────────────────────────────┐    │
│  │                  SRAM Extern 23LC512 (64 KB) - SPI                       │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  Zona Alarme: 0x0000 - 0x17A2 (6050 bytes)                      │    │    │
│  │  │  ════════════════════════════════════════                       │    │    │
│  │  │  • Capacitate: ~1210 alarme (SRAM_ALARMS_MEMORY_SEGMENT_COUNT)  │    │    │
│  │  │  • Segment size: 5 bytes/alarmă                                 │    │    │
│  │  │  │  address (2B) + id (2B) + state (1B)                         │    │    │
│  │  │  • Persistență: DOAR în timpul funcționării (SRAM = volatil)    │    │    │
│  │  │  • Se pierde complet la power-off/reset                         │    │    │
│  │  └─────────────────────────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────────────────────────┐    │    │
│  │  │  Zona Detectoare: 0x17A2 - 0xD332 (48000 bytes)                 │    │    │
│  │  │  ═══════════════════════════════════════════                    │    │    │
│  │  │  • Capacitate: ~4800 detectoare                                 │    │    │
│  │  │  • Segment size: 10 bytes/detector                              │    │    │
│  │  │  │  address(2) + id(2) + linkQuality(1) + status(1)             │    │    │
│  │  │  │  + lastUpdateMs(4)                                           │    │    │
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
│  │                   EEPROM Extern 24LC04B (512 bytes) - I2C                │    │
│  │  ══════════════════════════════════════════════════                     │    │
│  │  • Stocare configurări permanente (adresă, canal, etc.)                 │    │
│  │  • Backup alarme: adrese 111-210 (max ~20 alarme)                       │    │
│  │  • Address Verification Intervals: adrese 251-291 (max 10 intervale)    │    │
│  │  • Persistență: Permanentă (non-volatil, ~1M cicluri scriere)           │    │
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
    │   DETECTOR: Gaz detectat → Alarmă confirmată (6 citiri consecutive)
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ Construcție pachet în detector (fw-ergo-detector-source-v2.5.cpp)       │
    │   │                                                                          │
    │   │ Structura pachetului transmis (8 bytes):                                │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ [0-1] receiverAddress = 0x01 0x00 (Floor Module addr=1)        │     │
    │   │ │ [2-3] deviceAddress   = 0x0F 0x00 (Detector addr=15)           │     │
    │   │ │ [4-5] deviceId        = 0x03 0x00 (ID=3)                       │     │
    │   │ │ [6]   command         = 0x01 (COMMAND_ID_ALARM_START)          │     │
    │   │ │ [7]   isForward       = 0x00                                   │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   │                                                                          │
    │   │ CC1101Controller::sendData(packet)                                      │
    │   │   └─► Scrie în TX FIFO CC1101 prin SPI                                  │
    │   │   └─► Comutare TX state → transmisie RF 868 MHz                         │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+5 │   ══════════════════════════════════════════════════════════════════════════
    │   FLOOR MODULE: Semnal RF recepționat
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CC1101 RX FIFO (Hardware - pe modulul RF)                               │
    │   │                                                                          │
    │   │ Pachet stocat în FIFO intern:                                           │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ LEN │ DATA (8 bytes)                       │ RSSI │ LQI+CRC │   │     │
    │   │ │ 08  │ 01 00 0F 00 03 00 01 00              │ raw  │ raw     │   │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   │                                                                          │
    │   │ GDO2 pin → FALLING edge → Trigger interrupt pe MCU (pin 3)              │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+5 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ ISR: ihrMessageReceived() [ergo-floor-module-v4.0.cpp:986-988]          │
    │   │                                                                          │
    │   │ void ihrMessageReceived() {                                             │
    │   │     packetWaiting = true;  ◄── Flag setat în RAM MCU (volatile)         │
    │   │ }                                                                        │
    │   │                                                                          │
    │   │ Timp execuție: ~1-2 µs (doar setare flag)                               │
    │   │ NU citește datele - doar semnalizează                                   │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │                    ⏳ AȘTEPTARE LOOP 200Hz                               │
    │   │                    (max 5ms între iterații)                              │
    │   │                    loop() → lastMillis200Hz >= 5                         │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+10│   ══════════════════════════════════════════════════════════════════════════
    │   LOOP 200Hz: cc1101Receive() [ergo-floor-module-v4.0.cpp:990-997]
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ void cc1101Receive() {                                                  │
    │   │     if (packetWaiting) {                          ◄── Verifică flag     │
    │   │         detachInterrupt(CC1101_Interrupt);        ◄── Dezactivează ISR  │
    │   │                                                       (previne race)    │
    │   │         communicationService.receive(recvBuffer, &recvBufferLen);       │
    │   │         lastCommunicationRX = millis();           ◄── Timestamp pt WDT  │
    │   │         packetWaiting = false;                    ◄── Reset flag        │
    │   │         attachInterrupt(..., ihrMessageReceived, FALLING);              │
    │   │     }                                                ◄── Reactivează    │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+10│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CC1101Controller::receiveData() [ergo-floor-module-v4.0.cpp:1521-1553]  │
    │   │                                                                          │
    │   │ uint8_t CC1101Controller::receiveData(CCPACKET *packet) {               │
    │   │                                                                          │
    │   │     rxBytes = readStatusReg(CC1101_RXBYTES);    ◄── SPI: câți bytes?    │
    │   │                                                                          │
    │   │     if (rxBytes & 0x7F && !(rxBytes & 0x80)) {  ◄── Bytes disponibili   │
    │   │                                                     && fără overflow    │
    │   │                                                                          │
    │   │         packet->length = readConfigReg(CC1101_RXFIFO);                  │
    │   │                          ▲                                              │
    │   │                          └── SPI: citește primul byte = lungime (8)     │
    │   │                                                                          │
    │   │         if (packet->length > CC1101_MAX_BUFFER_SIZE) // > 30?           │
    │   │             packet->length = 0;                 ◄── Pachet prea mare    │
    │   │         else {                                                          │
    │   │             readBurstReg(packet->data, CC1101_RXFIFO, packet->length);  │
    │   │             │                                                           │
    │   │             │  ┌─────────────────────────────────────────────────────┐  │
    │   │             └─►│ SPI BURST READ: CC1101 FIFO → packet.data (RAM)    │  │
    │   │                │                                                     │  │
    │   │                │ CC1101 RX FIFO         CCPACKET.data[]              │  │
    │   │                │ ┌───────────────┐      ┌───────────────┐            │  │
    │   │                │ │01 00 0F 00    │─────►│01 00 0F 00    │ [0-3]     │  │
    │   │                │ │03 00 01 00    │─────►│03 00 01 00    │ [4-7]     │  │
    │   │                │ └───────────────┘      └───────────────┘            │  │
    │   │                │                                                     │  │
    │   │                │ Transfer: 8 bytes via SPI                           │  │
    │   │                └─────────────────────────────────────────────────────┘  │
    │   │                                                                          │
    │   │             packet->rssi = readConfigReg(CC1101_RXFIFO); ◄── RSSI raw   │
    │   │             val = readConfigReg(CC1101_RXFIFO);                         │
    │   │             packet->lqi = val & 0x7F;                   ◄── LQI (0-127) │
    │   │             packet->crc_ok = bitRead(val, 7);           ◄── CRC bit     │
    │   │         }                                                               │
    │   │     } else                                                              │
    │   │         packet->length = 0;                     ◄── Nimic în FIFO      │
    │   │                                                                          │
    │   │     setIdleState();                             ◄── CC1101 → IDLE       │
    │   │     flushRxFifo();                              ◄── Golește FIFO CC1101 │
    │   │     setRxState();                               ◄── CC1101 → RX mode    │
    │   │                                                                          │
    │   │     return packet->length;                      ◄── Returnează 8        │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+11│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CommunicationService::receive() [ergo-floor-module-v4.0.cpp:2406-2443]  │
    │   │                                                                          │
    │   │ void CommunicationService::receive(uint8_t *buff, uint8_t *len) {       │
    │   │                                                                          │
    │   │     if (radio->receiveData(&packet) > 0) {      ◄── packet.length > 0?  │
    │   │                                                                          │
    │   │         // Salvează RSSI și LQI pentru statistici                       │
    │   │         lastLqi[indexRssiLqi] = radio->computeLqi(packet.lqi);          │
    │   │         lastRssi[indexRssiLqi] = radio->computeRssi(packet.rssi);       │
    │   │                                                                          │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │         // ✓ VERIFICARE CRC - PACHETELE CU CRC INVALID SUNT IGNORATE    │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │                                                                          │
    │   │         if (!packet.crc_ok && debug->getDebugEnabled()) {               │
    │   │             debug->print(F("CRC FAIL: "));      ◄── Loghează eroarea    │
    │   │             // ... print packet data ...                                │
    │   │         }                                                               │
    │   │                                                                          │
    │   │         if (packet.crc_ok && packet.length > 0) {  ◄── ✓ DOAR CRC OK    │
    │   │                                                                          │
    │   │             // ═══════════════════════════════════════════════════════  │
    │   │             // ⚠️  BUFFER ACUMULATIV - ADAUGĂ LA SFÂRȘITUL BUFFER-ULUI  │
    │   │             // ═══════════════════════════════════════════════════════  │
    │   │                                                                          │
    │   │             for(i=0; i<packet.length; i++) {                            │
    │   │                 buff[*len + i] = packet.data[i];   ◄── Adaugă la offset │
    │   │             }                                       │                   │
    │   │             *len += packet.length;                  ◄── len = 0 + 8 = 8 │
    │   │         }                                                               │
    │   │     }                                                                   │
    │   │ }                                                                        │
    │   │                                                                          │
    │   │ DUPĂ EXECUȚIE:                                                          │
    │   │ ┌─────────────────────────────────────────────────────────────────┐     │
    │   │ │ recvBuffer[] (RAM global):                                      │     │
    │   │ │ ┌────┬────┬────┬────┬────┬────┬────┬────┬────────────────────┐  │     │
    │   │ │ │ 01 │ 00 │ 0F │ 00 │ 03 │ 00 │ 01 │ 00 │ ... (rest = 0)    │  │     │
    │   │ │ │RECV│    │ADDR│    │ ID │    │CMD │FWD │                    │  │     │
    │   │ │ └────┴────┴────┴────┴────┴────┴────┴────┴────────────────────┘  │     │
    │   │ │  [0]  [1]  [2]  [3]  [4]  [5]  [6]  [7]   [8-99]                │     │
    │   │ │                                                                 │     │
    │   │ │ recvBufferLen = 8                                               │     │
    │   │ └─────────────────────────────────────────────────────────────────┘     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │                    ⏳ AȘTEPTARE LOOP 100Hz                               │
    │   │                    (max 10ms între iterații)                             │
    │   │                    loop() → lastMillis100Hz >= 10                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+20│   ══════════════════════════════════════════════════════════════════════════
    │   LOOP 100Hz: coreService.tick() [ergo-floor-module-v4.0.cpp:2508-2517]
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ void CoreService::tick(uint32_t currentMillis) {                        │
    │   │     switch(operationMode) {                                             │
    │   │         case OPERATION_MODE_MASTER:            // = 1                   │
    │   │             operationModeMaster(currentMillis);                         │
    │   │         break;                                                          │
    │   │         case OPERATION_MODE_SLAVE:             // = 2                   │
    │   │             operationModeSlave(currentMillis);                          │
    │   │         break;                                                          │
    │   │     }                                                                   │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+20│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CoreService::operationModeMaster()                                      │
    │   │ [ergo-floor-module-v4.0.cpp:2532-2586]                                  │
    │   │                                                                          │
    │   │ void CoreService::operationModeMaster(uint32_t currentMillis) {         │
    │   │                                                                          │
    │   │     if(*receiveBufferLen > 0) {                 ◄── len=8, DA           │
    │   │         index = 0;                                                      │
    │   │                                                                          │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │         // WHILE LOOP - PROCESEAZĂ TOATE PACHETELE DIN BUFFER           │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │                                                                          │
    │   │         while(index < *receiveBufferLen && *receiveBufferLen > 7) {     │
    │   │               │                                    │                    │
    │   │               │ index=0 < 8 ✓                      │ 8 > 7 ✓            │
    │   │               │                                                         │
    │   │               │  ┌─────────────────────────────────────────────────┐    │
    │   │               └─►│ VERIFICARE 1: Este pachetul pentru mine?        │    │
    │   │                  │                                                 │    │
    │   │                  │ convertBytesToUInt.intVal = deviceAddress;      │    │
    │   │                  │ // deviceAddress = 1 (din EEPROM)               │    │
    │   │                  │                                                 │    │
    │   │                  │ if(recvBuffer[0] == 0x01 &&                     │    │
    │   │                  │    recvBuffer[1] == 0x00) {   ◄── DA, pentru FM │    │
    │   │                  └─────────────────────────────────────────────────┘    │
    │   │                                                                          │
    │   │                  ┌─────────────────────────────────────────────────┐    │
    │   │                  │ VERIFICARE 2: Address Verification Intervals    │    │
    │   │                  │ [ergo-floor-module-v4.0.cpp:2541-2554]          │    │
    │   │                  │                                                 │    │
    │   │                  │ if(numberOfAddressVerificationIntervals > 0) {  │    │
    │   │                  │     // Citește address din buffer               │    │
    │   │                  │     convertBytesToUInt.byte[0] = recvBuffer[2]; │    │
    │   │                  │     convertBytesToUInt.byte[1] = recvBuffer[3]; │    │
    │   │                  │     // address = 0x000F = 15                    │    │
    │   │                  │                                                 │    │
    │   │                  │     for(j=0; j<numberOfAddressVerificationIntervals; j++) { │
    │   │                  │         if(min <= address && address <= max) {  │    │
    │   │                  │             aux = 1; // VALID                   │    │
    │   │                  │             break;                              │    │
    │   │                  │         }                                       │    │
    │   │                  │     }                                           │    │
    │   │                  │     if(aux == 0) {                              │    │
    │   │                  │         goto SKIP_PROCESSING;  ◄── SKIP dacă   │    │
    │   │                  │     }                               invalid     │    │
    │   │                  │ }                                               │    │
    │   │                  │                                                 │    │
    │   │                  │ ⚠️ DACĂ numberOfAddressVerificationIntervals=0  │    │
    │   │                  │    VERIFICAREA NU SE FACE!                      │    │
    │   │                  │    → Orice adresă (inclusiv 0) TRECE            │    │
    │   │                  └─────────────────────────────────────────────────┘    │
    │   │                                                                          │
    │   │                  ┌─────────────────────────────────────────────────┐    │
    │   │                  │ PROCESARE COMANDĂ                               │    │
    │   │                  │ [ergo-floor-module-v4.0.cpp:2556-2578]          │    │
    │   │                  │                                                 │    │
    │   │                  │ switch(recvBuffer[6]) {  // COMMAND = 0x01     │    │
    │   │                  │     case COMMAND_ID_ALARM_START:  // = 1        │    │
    │   │                  │         addReceiveToDetectorsList(currentMillis, 0); │
    │   │                  │         aux = addReceiveToAlarmsList(0, true);  │    │
    │   │                  │         if(aux == -1 || aux == -3) {            │    │
    │   │                  │             // Alarmă nouă sau reactivată       │    │
    │   │                  │             outputController->setPulsed(RELAY_AC, ...); │
    │   │                  │             outputController->setPulsed(RELAY_DC, ...); │
    │   │                  │         }                                       │    │
    │   │                  │     break;                                      │    │
    │   │                  │ }                                               │    │
    │   │                  └─────────────────────────────────────────────────┘    │
    │   │                                                                          │
    │   │             SKIP_PROCESSING:                                            │
    │   │             index += 8;                          ◄── Avansează la       │
    │   │         }                                            următorul pachet   │
    │   │                                                                          │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │         // RESETARE BUFFER DUPĂ PROCESARE COMPLETĂ                      │
    │   │         // ═══════════════════════════════════════════════════════      │
    │   │         *receiveBufferLen = 0;                   ◄── DOAR len=0         │
    │   │                                                      datele rămân!      │
    │   │     }                                                                   │
    │   │ }                                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+21│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ CoreService::addReceiveToAlarmsList()                                   │
    │   │ [ergo-floor-module-v4.0.cpp:2671-2688]                                  │
    │   │                                                                          │
    │   │ EXTRAGERE DATE DIN recvBuffer:                                          │
    │   │                                                                          │
    │   │ convertBytesToUInt.byte[0] = recvBuffer[index + 2];  // 0x0F            │
    │   │ convertBytesToUInt.byte[1] = recvBuffer[index + 3];  // 0x00            │
    │   │ alarmEntry.address = convertBytesToUInt.intVal;      // = 15            │
    │   │                                                                          │
    │   │ convertBytesToUInt.byte[0] = recvBuffer[index + 4];  // 0x03            │
    │   │ convertBytesToUInt.byte[1] = recvBuffer[index + 5];  // 0x00            │
    │   │ alarmEntry.id = convertBytesToUInt.intVal;           // = 3             │
    │   │                                                                          │
    │   │ alarmEntry.state = state;                            // = true          │
    │   │                                                                          │
    │   │ TRANSFER CĂTRE SRAM EXTERN:                                             │
    │   │                                                                          │
    │   │ recvBuffer[]                 AlarmEntry              SRAM 23LC512       │
    │   │ (RAM MCU)                    (RAM stack)             (SPI extern)       │
    │   │ ┌─────────────┐              ┌─────────────┐         ┌─────────────┐    │
    │   │ │[2] 0F       │─►address ───►│address: 15  │────────►│ Addr 0x0000 │    │
    │   │ │[3] 00       │              │id: 3        │         │ 0F 00 03 00 │    │
    │   │ │[4] 03       │─►id ────────►│state: true  │         │ 01          │    │
    │   │ │[5] 00       │              └─────────────┘         └─────────────┘    │
    │   │ └─────────────┘                                      5 bytes/alarmă     │
    │   │                                                                          │
    │   │ SRAMController::addAlarm() execută:                                     │
    │   │   1. Caută dacă alarma există deja (findAlarm)                          │
    │   │   2. Dacă nu, scrie în SRAM la următoarea poziție liberă                │
    │   │   3. Incrementează numberOfAlarms                                       │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+22│   ══════════════════════════════════════════════════════════════════════════
    │   MESAJUL PROCESAT - BUFFER RESETAT
    │
    │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ STARE DUPĂ PROCESARE:                                                   │
    │   │                                                                          │
    │   │ recvBuffer[100] (RAM MCU):                                              │
    │   │ ┌────┬────┬────┬────┬────┬────┬────┬────┬────────────────────────────┐  │
    │   │ │ 01 │ 00 │ 0F │ 00 │ 03 │ 00 │ 01 │ 00 │ 00 00 00 00 ... (0-uri)  │  │
    │   │ └────┴────┴────┴────┴────┴────┴────┴────┴────────────────────────────┘  │
    │   │  ▲                                                                       │
    │   │  └── DATELE VECHI RĂMÂN! (dar nu contează, len=0)                       │
    │   │                                                                          │
    │   │ recvBufferLen = 0  ◄── Resetat, buffer considerat gol                   │
    │   │                                                                          │
    │   │ ✓ La următorul pachet, datele noi vor suprascrie (începând de la [0])   │
    │   │ ✓ CRC verificat - pachete corupte nu ajung în buffer                    │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    ▼
```

---

## Scenariul cu Multiple Pachete în Buffer

```
TIMP (ms)
    │
T+0 │   Detector A transmite alarmă
T+3 │   Detector B transmite alarmă (înainte ca FM să proceseze pe A)
    │
T+5 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ ISR: packetWaiting = true (pentru pachetul A)                           │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+8 │   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ ISR: packetWaiting = true (pentru pachetul B, deja era true)            │
    │   │ ⚠️  Pachetul B poate fi în FIFO CC1101 sau pierdut dacă FIFO e plin     │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+10│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ cc1101Receive() procesează pachetul A                                   │
    │   │                                                                          │
    │   │ CommunicationService::receive():                                        │
    │   │   - Citește pachetul A din FIFO CC1101                                  │
    │   │   - buff[0..7] = pachet A                                               │
    │   │   - *len = 8                                                            │
    │   │                                                                          │
    │   │ recvBuffer: [Pachet A (8 bytes)] [gol...]                               │
    │   │ recvBufferLen: 8                                                        │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    │   Presupunem că CC1101 a primit și pachetul B în FIFO înainte de flush
    │   (sau un nou interrupt vine rapid)
    │
T+15│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ cc1101Receive() procesează pachetul B                                   │
    │   │                                                                          │
    │   │ CommunicationService::receive():                                        │
    │   │   - Citește pachetul B din FIFO CC1101                                  │
    │   │   - buff[*len + 0..7] = buff[8..15] = pachet B                          │
    │   │   - *len += 8 → *len = 16                                               │
    │   │                                                                          │
    │   │ recvBuffer: [Pachet A (8 bytes)][Pachet B (8 bytes)] [gol...]           │
    │   │ recvBufferLen: 16                                                       │
    │   │                                                                          │
    │   │ ⚠️  BUFFER ACUMULATIV permite acest comportament!                       │
    │   │     buff[*len + i] = packet.data[i];                                    │
    │   │     *len += packet.length;                                              │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
T+20│   ┌─────────────────────────────────────────────────────────────────────────┐
    │   │ coreService.tick() procesează AMBELE pachete                            │
    │   │                                                                          │
    │   │ while(index < 16 && 16 > 7) {                                           │
    │   │                                                                          │
    │   │   // Prima iterație: index=0                                            │
    │   │   Procesează Pachet A (bytes 0-7)                                       │
    │   │   index += 8; // index = 8                                              │
    │   │                                                                          │
    │   │   // A doua iterație: index=8                                           │
    │   │   Procesează Pachet B (bytes 8-15)                                      │
    │   │   index += 8; // index = 16                                             │
    │   │                                                                          │
    │   │   // index=16 < 16 = false, ieșire din while                            │
    │   │ }                                                                       │
    │   │                                                                          │
    │   │ *receiveBufferLen = 0;  // Reset după procesare completă                │
    │   └─────────────────────────────────────────────────────────────────────────┘
    │
    ▼
```

---

## Scenariul Problematic: Cum Apare Address = 0

### Cauza 1: numberOfAddressVerificationIntervals = 0

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│ SITUAȚIE: Floor Module fără intervale de verificare configurate                 │
│                                                                                  │
│ La setup() [ergo-floor-module-v4.0.cpp:2500]:                                   │
│     numberOfAddressVerificationIntervals = 0;  ◄── Inițializat cu 0             │
│                                                                                  │
│ La loadAddressVerificationIntervals() [ergo-floor-module-v4.0.cpp:2735-2739]:   │
│     numberOfAddressVerificationIntervals = eepromController->                   │
│         getNumberOfAddressVerificationIntervals();                              │
│     // Dacă EEPROM-ul nu a fost configurat, returnează 0                        │
│                                                                                  │
│ REZULTAT:                                                                        │
│ În operationModeMaster() [linia 2541]:                                          │
│     if(numberOfAddressVerificationIntervals > 0) {  ◄── 0 > 0 = FALSE           │
│         // Verificarea NU se execută!                                           │
│     }                                                                            │
│                                                                                  │
│ → Orice adresă (inclusiv 0) TRECE fără verificare                               │
│ → Dacă un detector trimite address=0 (EEPROM corupt), va fi acceptat            │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### Cauza 2: Detector cu EEPROM Neprogramat/Corupt

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│ SITUAȚIE: Detector cu valori 0 în EEPROM                                        │
│                                                                                  │
│ În detector (fw-ergo-detector-source-v2.5.cpp):                                 │
│                                                                                  │
│ La citirea din EEPROM:                                                          │
│     deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT); │
│     deviceId = eepromController->getUIntValue(EEPROM_DEVICE_ID_INT);            │
│                                                                                  │
│ Dacă EEPROM nu a fost programat sau e corupt:                                   │
│     deviceAddress = 0x0000 sau 0xFFFF (65535)                                   │
│     deviceId = 0x0000 sau 0xFFFF (65535)                                        │
│                                                                                  │
│ La trimitere alarmă:                                                            │
│     buff[BUFF_INDEX_ADDRESS_LO] = 0x00;                                         │
│     buff[BUFF_INDEX_ADDRESS_HI] = 0x00;                                         │
│     buff[BUFF_INDEX_ID_LO] = 0x00;                                              │
│     buff[BUFF_INDEX_ID_HI] = 0x00;                                              │
│                                                                                  │
│ Floor Module primește pachet VALID (CRC OK) cu:                                 │
│     address = 0                                                                 │
│     id = 0                                                                      │
│                                                                                  │
│ → Pachetul trece verificarea CRC (date valide, doar valori greșite)             │
│ → Dacă numberOfAddressVerificationIntervals=0, nu se filtrează                  │
│ → Alarmă stocată cu address=0, id=0                                             │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## Rezumat Locații Buffer și Persistență

| Buffer | Memorie | Dimensiune | Locație în Cod | Persistență |
|--------|---------|------------|----------------|-------------|
| CC1101 RX FIFO | Hardware CC1101 | 64 bytes | (modul extern) | Până la citire+flush |
| CC1101 TX FIFO | Hardware CC1101 | 64 bytes | (modul extern) | Până la transmisie |
| CCPACKET packet | RAM MCU | ~34 bytes | :654 (membru clasă) | Permanent |
| `recvBuffer` | RAM MCU global | 100 bytes | :962 | Permanent |
| `sendBuffer` | RAM MCU global | 15 bytes | :962 | Permanent |
| `recvBufferLen` | RAM MCU global | 1 byte | :963 | Permanent |
| Alarme | SRAM 23LC512 | 6050 bytes | SPI CS=pin 15 | Runtime (volatil) |
| Detectoare | SRAM 23LC512 | 48000 bytes | SPI CS=pin 15 | Runtime (volatil) |
| Config | EEPROM 24LC04B | 512 bytes | I2C | Permanent |

---

## Puncte Importante din Codul Legacy v4.0

### ✓ CE FUNCȚIONEAZĂ CORECT

1. **CRC este verificat** (linia 2425):
   ```cpp
   if (packet.crc_ok && packet.length > 0) {
       // Doar pachetele cu CRC valid sunt copiate în buffer
   }
   ```

2. **Lungimea minimă este verificată** (linia 2536):
   ```cpp
   while(index < *receiveBufferLen && *receiveBufferLen > 7) {
       // Procesează doar dacă avem minim 8 bytes
   }
   ```

3. **Buffer-ul este resetat după procesare** (linia 2585):
   ```cpp
   *receiveBufferLen = 0;
   ```

### ⚠️ CE POATE CAUZA PROBLEMA CU ADDRESS=0

1. **Verificarea intervalelor este opțională** (linia 2541):
   ```cpp
   if(numberOfAddressVerificationIntervals > 0) {
       // Dacă e 0, verificarea NU se face deloc!
   }
   ```

2. **Nu există validare că address != 0**:
   ```cpp
   // Lipsește:
   // if(alarmEntry.address == 0) { skip; }
   ```

3. **Detector cu EEPROM neprogramat/corupt** poate trimite valori valide dar greșite (address=0, id=0).

---

*Document generat din analiza fișierului ergo-floor-module-v4.0.cpp (cod în producție).*
