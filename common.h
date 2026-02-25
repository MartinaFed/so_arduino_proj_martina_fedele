#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>

//DEFINIZIONE PIN SENSORI
#define sensore_ingresso (1 << PD6)  //pin 6 della porta D per il sensore di ingresso
#define sensore_uscita (1 << PD7)  //pin 7 della porta D per il sensore di uscita

//DEFINIZIONE PIN LED
#define led_rosso (1 << PB4) //pin 12 della porta B
#define led_verde (1 << PB5) //pin 13 della porta B

//DEFINIZIONE PIN SERVO
#define servo (1 << PB1) //pin 9 della porta B

//DEFINIZIONE MASSIMO POSTI DISPONIBILI
#define MAX_POSTI 5

#endif