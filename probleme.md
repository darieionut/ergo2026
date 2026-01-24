# Probleme Cunoscute - Sistem ERGO26

## Istoric Probleme cu Alarme

### Versiunea 3.0 - Alarme cu Numere Mari Eronate

**Descriere problemă:**
- Sistemul afișa alarme cu numere mari eronate
- ID-urile și adresele detectoarelor aveau valori neașteptat de mari
- Datele primite prin RF erau corupte sau interpretate incorect

**Impact:**
- Imposibilitatea identificării corecte a detectoarelor în alarmă
- Confuzie în monitorizarea sistemului

---

### Versiunea 4.0 - Introducerea Verificării Intervalului de Adrese

**Soluție implementată de producător:**
- A fost adăugat codul pentru verificarea intervalului de adrese (`AddressVerificationInterval`)
- Scopul era filtrarea pachetelor cu adrese invalide

**Rezultat:**
- Soluția **nu a rezolvat problema**
- Problemele cu alarmele au persistat

---

### Versiunea 4.0 - Probleme Curente

#### Problema 1: Alarme cu ID și Adresă din 5 Cifre Aleatorii

**Descriere:**
- Se primesc alarme cu ID și adresă formate din 5 cifre aleatorii
- Valorile așteptate ar trebui să fie de **2 cifre** (interval 1-99)
- Exemple de valori eronate: 12345, 54321, 99999, etc.

**Cauze posibile:**
- Corupere date în transmisia RF (interferențe, zgomot)
- Buffer overflow la citirea pachetelor
- Interpretare greșită a octeților din pachet
- Probleme de sincronizare la recepție
- CRC validat incorect sau dezactivat

#### Problema 2: Alarme cu ID și Adresă 0

**Descriere:**
- Se primesc alarme unde atât ID-ul cât și adresa sunt **0**
- Aceste valori sunt invalide (adresele valide încep de la 1)

**Cauze posibile:**
- Pachet RF incomplet sau trunchiat
- Buffer-ul nu este populat corect înainte de procesare
- Detector neprogramat corect (valori default 0)
- Eroare la citirea din SRAM/EEPROM

---

## Rezumat Probleme

| Versiune | Problemă | Status |
|----------|----------|--------|
| 3.0 | Alarme cu numere mari eronate | Nerezolvată (migrare la 4.0) |
| 4.0 | Verificare interval adrese ineficientă | Nerezolvată |
| 4.0 | ID și adresă cu 5 cifre aleatorii | **Activă** |
| 4.0 | ID și adresă cu valoarea 0 | **Activă** |

---

## Zone de Cod Relevante pentru Investigare

1. **Recepție RF** - `common/Controllers.cpp` - `CC1101Controller::receiveData()`
2. **Procesare pachete** - `floor-module/Services.cpp` - `CoreService::processPacket()`
3. **Validare CRC** - verificare dacă `packet.crc_ok` este testat corect
4. **Verificare intervale** - logica `AddressVerificationInterval` în procesarea pachetelor
5. **Parsare date** - extragerea adresei și ID-ului din `packet.data[]`

---

## Recomandări pentru Debugging

1. Activarea debug-ului serial (`DEBUG_ENABLED = true`)
2. Logarea valorilor brute ale pachetelor primite
3. Verificarea valorilor RSSI și LQI pentru calitatea semnalului
4. Testarea cu un singur detector pentru a izola problema
5. Verificarea programării corecte a detectoarelor (adresă, ID în intervalul așteptat)

---

*Document creat pentru urmărirea și rezolvarea problemelor de fiabilitate ale sistemului ERGO26.*
