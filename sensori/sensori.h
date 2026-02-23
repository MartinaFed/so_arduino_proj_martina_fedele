#ifndef SENSORI_H
#define SENSORI_H

#include <avr/io.h>
#include <stdbool.h>

//FUNZIONI PUBBLICHE
//Inizializza i pin dei sensori come input e attiva le resistenze di pull-up
void sensore_init(uint8_t s_ingresso , uint8_t s_uscita);

//Legge lo stato del sensore e restituisce true se rileva un ostacolo, false altrimenti
bool rilevato_ostacolo(uint8_t m);

#endif