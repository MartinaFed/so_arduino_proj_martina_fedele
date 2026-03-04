#ifndef TASTIERA_H
#define TASTIERA_H

#include <stdint.h>

#define MAX_EVENTS 9 // Numero massimo di eventi simultanei (3x3)

// La tua struct originale!
typedef struct {
  uint8_t status: 1; // 1 = premuto, 0 = rilasciato
  uint8_t key:    7; // Numero del tasto (0-8)
} KeyEvent;

// Funzioni pubbliche
void tastiera_init(void);
uint8_t keyScan(KeyEvent* events); // Restituisce il numero di eventi

#endif