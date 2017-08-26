// This code waits for a character and transmits the character back (No interrupts)

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>                 // needed for uint8_t
#include <util/delay.h>
#include <string.h>


#define FOSC 1000000                       // Clock Speed
#define BAUD 4800
#define MYUBRR (FOSC/16/BAUD -1)

#define ADC_THRESHOLD 1023

#define MULDELIMETER ','
#define BREAKLINE '\n'

uint16_t adc_read(uint8_t adcx);
void nextX(uint8_t current);

void sendChar(char tosend);
void sendString(char *tosend, char delimiter);

int main( void )
{
    /*Set baud rate */
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;

    UCSR0B = (1 << TXEN0)| (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); ;      // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // Set frame: 8data, 1 stp

    DDRD = 0b01000000;   //set all of port D as inputs except for TX
    DDRB = 0xFF;
    PORTB = 0x00;

	ADCSRA |= _BV(ADEN);

	int i, delay = 50;

	uint16_t num;

    uint8_t current = 0;
	char hex[5];

    while(1)
    {
		for(i = 0; i < 6; i++)
		{
			num = adc_read(i);
			itoa(num, hex, 10);

			sendString(hex, MULDELIMETER);
		}
		sendChar(BREAKLINE);

        nextX(current);
        current = (current + 1)%100;

		_delay_ms(delay);
    }
}

void sendString(char *tosend, char delimiter)
{
	int i;
	for (i = 0; i < strlen(tosend); i++)
	{
		sendChar(tosend[i]);
	}
	sendChar(delimiter);
}

void sendChar(char tosend)
{
	while (( UCSR0A & (1<<UDRE0))  == 0){};
	UDR0 = tosend;
}

uint16_t adc_read(uint8_t adcx)
{
	ADMUX	&=	0xf0;
	ADMUX	|=	adcx;

	ADCSRA |= _BV(ADSC);

	while ( (ADCSRA & _BV(ADSC)) );

	return ADC;
}

void nextX(uint8_t current)
{
    PORTB &= 0xfc;

    if(current == 49){PORTB |= 0;}
    else if (current < 49){PORTB |= 1;}
    else{PORTB |= 2;}
}

// void nextY(uint8_t current)
// {
//     if (current < 49)
//     {
//
//     }
//     else if(current == 49)
//     {
//
//     }
//     else
//     {
//
//     }
// }
