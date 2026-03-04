#define F_CPU 16000000UL // Definisce la frequenza della Maker Uno (16 MHz)

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "tastiera.h"


uint16_t key_status = 0; // stato della keyboard, 1 bit per tasto

void tastiera_init(void){
    DDRC = 0x38;  // PC3,PC4,PC5 come Output (righe)
    PORTC = 0x07; // PC0,PC1,PC2 come Input (colonne)
}


//funzione che restituisce quanti eventi sono accaduti
uint8_t keyScan(KeyEvent* events){
  uint16_t new_status = 0; // nuovo key status
  uint8_t num_events = 0;      // numero degli eventi
  uint8_t key_num = 0;         // contatore che va da 0 a 8

  //controllo le tre righe una alla volta
  for (int r=0; r<3; ++r){
    // mask per l'output: (1 << (r+3)) prende un 1 e lo sposta a sinistra, se r=0 (prima riga), sposta di 3 posti: diventa 0000 1000 (pin A3), '~' inverte tutto: diventa 1111 0111.
    uint8_t row_mask= ~(1<<(r+3));
    
    // metto 0 sul pin della riga
    PORTC =row_mask;

    _delay_us(100); //stabilizzo il segnale

    //leggo i 3 bit negandoli
    uint8_t row = ~(0x07 & PINC);
    
    //leggo le colonne
    for (int c=0;c<3;++c){

        uint16_t key_mask = 1<<key_num;  //Crea un 1 nella posizione corrispondente al tasto attuale
        uint8_t col_mask = 1<<c;   // Crea un 1 nella posizione della colonna attuale
        
        // 1 se key è premuto
        uint8_t cs = (row & col_mask) != 0; 
        
        // se il tasto è premuto accendo il suo bit
        if (cs){
            new_status |= key_mask;
        }
        
        //controllo lo stato precedente: guardo se al giro precedente il tasto era già premuto
        uint8_t ps=(key_mask & key_status) != 0;

        // Se lo stato attuale è diverso da quello di prima
        if (cs != ps){
            KeyEvent e; //pacchetto vuoto
            e.key = key_num; // scrivo il numero del tasto
            e.status = cs; //scrivo se è stato premuto 1 o rilasciato 0
            events[num_events]=e; //metto il pacchetto nella lista degli eventi da stampare
            ++num_events;
        }
        ++key_num; //tasto successivo
        }
    }
    key_status=new_status;
    return num_events;
}
