#define F_CPU 16000000UL //definisco la frequenza di clock a 16 MHz
#include "uart.h"  
#include <avr/io.h>


//INIZIALIZZAZIONE
void uart_init(unsigned int baud) {
    unsigned int ubrr = F_CPU / 16 / baud - 1;  // Calcolo il valore da inserire nel registro per ottenere il Baud Rate desiderato
   // Il registro UBRR è a 16 bit ma ci si possono scrivere solo 8 bit  alla volta perciò lo divido in due parti alta e bassa (H e L)
    UBRR0H = (unsigned char)(ubrr >> 8); // Sposta i bit alti a destra per salvarli
    UBRR0L = (unsigned char)ubrr;        // Salva i bit bassi

    UCSR0B = (1 << TXEN0); //accendo l'hardware di trasmissione tramite TXEN0 (Transmit Enable) che accende il pin di uscita dei dati
  
    
    // UCSZ01 e UCSZ00 sono i due interruttori che, accesi insieme, impostano la 'taglia' a 8 bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // accendo questi due bit per dire al chip di usare pacchetti da 8 bit
}



// Invio di un solo carattere
void uart_transmit(unsigned char data) {
    // UCSR0A: registro di stato
    // UDRE0: (UART Data Register Empty) è un bit che diventa 1 solo quando la il buffer è vuoto
    // Aspetto il buffer vuoto è pronto a ricevere)
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Mette il nostro carattere nel registro dati UDR0
    UDR0 = data;
}




//  Invio di una frase
void uart_print(const char* str) {
    // Finchè non punto il carattere nullo (\0)
    while (*str) {
        // Prendi il carattere attuale, passalo alla funzione transmit, sposta il puntatore alla lettera successiva
        uart_transmit(*str++);
    }
}