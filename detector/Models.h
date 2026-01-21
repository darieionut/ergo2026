/**
 * @file Models.h
 * @brief Structurile de date pentru Detector
 *
 * Conține modelele de date specifice Detector
 */

#ifndef DETECTOR_MODELS_H
#define DETECTOR_MODELS_H

#include <Arduino.h>

// ============================================================================
// Complete Data - Date complete pentru programare
// ============================================================================

struct CompleteData {
    uint16_t address;          // Adresa dispozitivului
    uint8_t channel;           // Canal RF
    uint16_t receiverAddress;  // Adresa receptorului (Floor Module)
    uint16_t deviceId;         // ID-ul dispozitivului
    int16_t alarmTrigger;      // Valoare prag alarmă
    boolean debugEnabled;      // Debug activat
};

#endif // DETECTOR_MODELS_H
