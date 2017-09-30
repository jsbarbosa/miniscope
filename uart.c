// This code waits for a character and transmits the character back (No interrupts)

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>                 // needed for uint8_t
#include <util/delay.h>
#include <string.h>

#define FOSC 1000000                       // Clock Speed

uint8_t DELAYX, DELAYY, NMESUREMENTS;

uint8_t HALFPOINTS;

int main(void)
{
    /*Set baud rate */
    // UBRR0H = (MYUBRR >> 8);
    // UBRR0L = MYUBRR;
    //
    // UCSR0B = (1 << TXEN0)| (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); ;      // Enable receiver and transmitter
    // UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // Set frame: 8data, 1 stp
    //
    // DDRD = 0b01000000;   //set all of port D as inputs except for TX
    DDRB = 0xFF;
    PORTB = 0x00;

	// ADCSRA |= _BV(ADEN);

	// setPWM();

	// char *string;
}
