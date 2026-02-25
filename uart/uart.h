#ifndef UART_H
#define UART_H

// funzioni pubbliche
void uart_init(unsigned int baud);
void uart_transmit(unsigned char data);
void uart_print(const char* str);

#endif