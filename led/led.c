//Funzioni per il controllo hardware dei led tramite i registri AVR

#define F_CPU 16000000UL  //definisco la frequenza di clock della CPU (16MHz nel caso di Maker uno)
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>   //contiene le funzioni per i ritardi
#include <avr/io.h>   //contiene la definizione dei registri hardware (PORTB,DDRB...)
#include <avr/interrupt.h>  //contiene le librerie per gestire gli interrupt 8le ISR e la funzione sei())
#include "led.h"



//funzione che accende un led 
//accende un pin senza modificare lo stato degli altri  utilizzando l'operatore logico bit a bit OR che confronta i bit attuali di PORTB con la maschera m
void led_on(uint8_t m){
    PORTB |= m; // accende il led senza toccare gli altri (se un bit nella maschera vale 1 il pin corrispondente in PORTB viene forzato a 1 e si accende, se vale 0 invece viene tenuto inalterato)
}


//funione che spegne un led
//spegne un pin senza modificare lo stato degli altri
void led_off(uint8_t m){
    PORTB &= ~m;  // ~ inverte tutti i bit , uccido il pin dove vedo 0, &= prende lo stato attuale della porta e lo confronta con quello invertito , il risultato è 1 se entrambi sono 1 altrimenti è 0. Lascia tutto come è ma forzo solo il bit indicato da m
}
