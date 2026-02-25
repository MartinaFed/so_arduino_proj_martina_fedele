#define F_CPU 16000000UL //definisco la frequenza di clock a 16 MHz
#include "uart.h"  
#include <avr/io.h>
#include <avr/interrupt.h>


#define RX_BUFFER_SIZE 64 //definisco dimensione massima del buffer (64 byte)
volatile unsigned char buffer_salvataggio[RX_BUFFER_SIZE]; //array di dim RX_BUFFER_SIZE per salvare i caratteri ricevuti.
volatile unsigned int head = 0; // l'indice che indica in quale "casella" l'ISR scriverà il carattere in arrivo
volatile unsigned int r = 0; // indice dove il main legge(il più vecchio carattere non letto)



//INIZIALIZZAZIONE
void uart_init(unsigned int baud) {
    unsigned int ubrr = F_CPU / 16 / baud - 1;  // calcolo il valore da inserire nel registro per ottenere il Baud Rate desiderato  
    // Il registro UBRR è a 16 bit ma ci si possono scrivere solo 8 bit  alla volta perciò lo divido in due parti alta e bassa (H e L)
    UBRR0H= (unsigned char)(ubrr >> 8); // sposta i bit alti a destra per salvarli
    UBRR0L= (unsigned char)ubrr;        // Salva i bit bassi 
    

    // Accendo contemporaneamente tre moduli hardware usando l'operatore OR (|):
    // TXEN0  (Transmit Enable): attiva il trasmettitore, permettendo al pin TX di inviare dati
    // RXEN0  (Receive Enable): attiva il ricevitore, permettendo al pin RX di ascoltare i dati in arrivo
    // RXCIE0 (RX Complete Interrupt Enable): dice al microcontrollore di ricevere appena finisce e lanciare ISR(USART_RX_vect)
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); 

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // UCSZ01 e UCSZ00 sono i due bit che controllano la dimensione del carattere: se li accendo entrambi dico al chip di usare il formato standard a 8 bit.

    // abilita gli interrupt globali
    sei(); 
}




//funzione che scatta in automatico appena arriva un singolo carattere sulla UART (ISR=interrupt service routine , USART_RX_vec=nome dell'interrupt di ricezione uart)
ISR(USART_RX_vect) {

    // leggo il registro dati UDR0 (leggendolo si svuota e comunica al chip di poter ricevere un altro byte)
    unsigned char data = UDR0;
    
    //Calcolo quale sarà il prossimo posto in cui scrivere.
    unsigned int next_head = (head + 1) % RX_BUFFER_SIZE;  // il modulo (%): se head arriva a 63, (63 + 1) % 64 fa 0. Così l'indice ricomincia dall'inizio dell'array senza andare fuori dalla memoria.
    
    // se il buffer non è pieno, salva il carattere
    if (next_head != r) {
        buffer_salvataggio[head] = data; //salvo il carattere appena letto nella casella attuale
        head = next_head; //aggiorno indice della testa
    }
}





// funzione che restituisce il numero esatto di byte in attesa, oppure 0 se il buffer è vuoto
unsigned int uart_available(void) {
    
    //Calcola la distanza tra la testa (dove scrive l'ISR) e la coda (dove leggiamo noi). Va sommato RX_BUFFER_SIZE prima di sottrarre 'r' per evitare risultati negativi in caso la testa sia ripartita da 0 e quindi numericamente minore della coda 
    // l'operatore modulo (%) toglie l'eccesso mantenendo il risultato nel range corretto.
    return (RX_BUFFER_SIZE + head - r) % RX_BUFFER_SIZE;
}



// funzione che estrae il carattere più vecchio dal buffer
unsigned char uart_read(void) {
    //se testa e coda sono nella stessa posizione
    if (head == r) {
        return 0; //non c'è nulla da leggere quindi il buffer è vuoto
    }
    //Copio il carattere dalla casella attualmente indicata dall'indice 'r'.
    unsigned char data = buffer_salvataggio[r]; // Legge il dato
    
    // sposto la "coda" in avanti di un passo (il modulo (%) fa sì che l'indice non superi mai la fine dell'array).
    r = (r + 1) % RX_BUFFER_SIZE;
    
    //restituisco il carattere
    return data;
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