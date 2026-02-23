#define F_CPU 16000000UL //definisce la velocità del processore (16MHz)
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "sensori.h"


//INIZIALIZZAZIONE DEI SENSORI
//configuro i pin passati come argomento affinchè funzionino come input
void sensore_init(uint8_t s_ingresso , uint8_t s_uscita){
    DDRD &= ~(s_ingresso | s_uscita);  //DDRD (Data Direction Register): Imposto a 0 i bit dei sensori. &= ~ forza a 0 soltato i bit indicati non modificandone aktri
    PORTD |=(s_ingresso | s_uscita);  //PORTD: scrive 1 su pin già configurati come input. Questo mantiene il segnale stabile a alto(1) quando non ci sono ostacoli
}


//funzione che restituisce true se l'ostacolo è rilevato , false altrimenti
bool rilevato_ostacolo(uint8_t m){ // m è il pin da controllare
    //PIND legge lo stato fisico della porta D
    //& m isola il pin m 
    //i sensori a infrarossi inviano 0 (considerato false) quando vedono un ostacolo , if con il not(!) inverte la lettura: se il sensore legge 0 l'if valuta 1(true)
    if(!(PIND & m)){  
        return true; //ostacolo rilevato
    }
    else{
        return false; //ostacolo non rilevato
    }
}
