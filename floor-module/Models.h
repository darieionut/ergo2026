/**
 * @file Models.h
 * @brief Structurile de date pentru Floor Module
 *
 * Conține toate modelele de date specifice Floor Module
 */

#ifndef FLOOR_MODULE_MODELS_H
#define FLOOR_MODULE_MODELS_H

#include <Arduino.h>

// ============================================================================
// Alarm Entry - Intrare în lista de alarme
// ============================================================================

struct AlarmEntry {
    uint16_t address;    // Adresa detectorului
    uint16_t id;         // ID-ul alarmei
    boolean state;       // Starea alarmei (activ/inactiv)
};

// ============================================================================
// Detector Entry - Intrare în lista de detectoare
// ============================================================================

struct DetectorEntry {
    uint16_t address;        // Adresa detectorului
    uint16_t id;             // ID-ul detectorului
    uint8_t linkQuality;     // Calitatea linkului RF
    uint8_t status;          // Status detector
    uint32_t lastUpdateMs;   // Ultimul update (milisecunde)
};

// ============================================================================
// Offline Detector Entry - Detectoare offline
// ============================================================================

struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;  // Index în lista de detectoare
};

// ============================================================================
// Address Verification Interval - Interval pentru verificare adrese
// ============================================================================

struct AddressVerificationInterval {
    uint16_t min;   // Adresă minimă
    uint16_t max;   // Adresă maximă
};

// ============================================================================
// Complete Data - Date complete pentru programare
// ============================================================================

struct CompleteData {
    uint16_t address;          // Adresa dispozitivului
    uint8_t channel;           // Canal RF
    uint16_t forwardAddress;   // Adresa pentru forwarding
    uint8_t forwardChannel;    // Canal pentru forwarding
    uint8_t operationMode;     // Mod operare (master/slave)
    boolean debugEnabled;      // Debug activat
};

// ============================================================================
// Menu Element - Element de meniu pentru LCD
// ============================================================================

struct MenuElement {
    const __FlashStringHelper *textLine1;  // Text linia 1
    const __FlashStringHelper *textLine2;  // Text linia 2
    boolean isLine1Dynamic;                 // Linia 1 este dinamică
    boolean isLine2Dynamic;                 // Linia 2 este dinamică
    uint8_t back, left, right, ok;          // Navigare meniu
    boolean valueRefreshEnabled;            // Refresh automat valori
};

#endif // FLOOR_MODULE_MODELS_H
