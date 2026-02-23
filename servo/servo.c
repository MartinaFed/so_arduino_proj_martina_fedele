#define F_CPU 16000000UL  //definisco la frequenza di clock a 16 MHz
#include <util/delay.h>   //contiene la funzione per le pause
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>   //contiene nomi i registri hardware (PORTB,DDRB...)


// INIZIALIZZAZIONE DEL SERVO
// al servo serve un impulso preciso ogni 20 ms, noi cerchiamo di dirgli di mandare un impulso di 1,5 ms sul pin 9 (Timer 1 conta piu di Timer 0 o 2)
// imposta il Timer 1 (a 16 bit) in modalità 'Fast PWM' e genera un segnale a 50Hz richiesto ai servomotori
void servo_init(void) {
    DDRB |= (1 << PB1);  // configuro il pin 9 come uscita , su Maker uno il pin 9 corrisponde a PB1 ed è collegato al canale A del timer 1

    //configurazione del Registro TCCR1A (Timer/Counter Control Register A)
    //COM1A1 imposta il comportamento del pin: in Fast PWM mette il pin a 1 all'inizio del ciclo e lo mette a 0 quando il contatore raggiunge la soglia OCR1A
    //WGM11: è uno dei bit per scegliere la modalità di funzionamento (Mode 14)
    TCCR1A = (1 << COM1A1) | (1 << WGM11);   
    
    //configurazione del registro TCCR1B
    //WGM12,WGM13 : assieme a WGM11 completano la scelta della 'Mode 14'
    //CS11 (Clock select) imposta il Prescaler a 8: rallenta l'orologio di 8 volte (da 16MHz/8) altrimenti sarebbe troppo veloce per il servo
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    ICR1 = 39999;   //dico al timer di contare fino a 39999 prima di tornare a 0 (garantisce che ogni ciclo duri 20 ms) ( la formula è ICR1= (fcpu/(N*fPWM)-1))
    OCR1A = 1550;   //indica la posizione iniziale del servo
}


// Funzione per alzare il servo di circa 90° (passo da 1550 a 3600 con passi di 20), utilizzo uint16_t poichè OCR1A è un registro a 16 bit
void open_barrier(void){
    for(uint16_t i=1550; i<=3600; i+=20){
        OCR1A = i;
        _delay_ms(10); // pausa utile al servomotore per muoversi più 'delicatamente'
    }
}


//Funzione per abbassare la sbarra da 90° a 0° 
void close_barrier(void){
    for (uint16_t i = 3600; i >= 1550; i -= 20) {
            OCR1A = i;
            _delay_ms(10); // pausa utile al servomotore per muoversi più 'delicatamente'
        }
}
