#ifndef LED_H    // #indef, #define, #endif prevengono doppie inclusioni, se l'header viede chiamato da più file nello stesso progetto, il compilatore lo legge una sola volta evitando errori
#define LED_H

#include <stdint.h>


// FUNZIONI PUBBLICHE
void led_init(uint8_t led_1, uint8_t led_2); //funzione per inizializzare i led
void led_on(uint8_t m);   // funzione per accendere il led 
void led_off(uint8_t m);  // funzione per spegnere il led

#endif