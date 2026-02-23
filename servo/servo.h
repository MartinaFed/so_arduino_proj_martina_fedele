#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>


//FUNZIONI PUBBLICHE PER IL SERVOMOTORE

void servo_init(void);  //inizializza il Timer1 e manda il servo alla posizione 0

void open_barrier(void);  //alza la sbarra di 90°

void close_barrier(void);  //abbassa la sbarra riportandola a 0°

#endif
