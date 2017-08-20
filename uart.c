// This code waits for a character and transmits the character back (No interrupts)
 

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>                 // needed for uint8_t
#include <util/delay.h>
#include <string.h>


#define FOSC 1000000                       // Clock Speed
#define BAUD 4800                
#define MYUBRR (FOSC/16/BAUD -1)

#define ADC_PIN	0
#define ADC_THRESHOLD 1023

uint16_t adc_read(uint8_t adcx);

void sendChar(char tosend);
void sendString(char *tosend);

int main( void )
{
    /*Set baud rate */
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;
    
    UCSR0B = (1 << TXEN0)| (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); ;      // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // Set frame: 8data, 1 stp
    
    DDRD = 0b01000000;   //set all of port D as inputs except for TX
	
	ADCSRA |= _BV(ADEN);

	uint16_t num;
	char hex[5];
	
    while(1)
    {
		num = adc_read(ADC_PIN);
		itoa(num, hex, 10);

		sendString(hex);
    }   
}

void sendString(char *tosend)
{
	int i;
	for (i = 0; i < strlen(tosend); i++)
	{ 
		sendChar(tosend[i]);
	}
	sendChar('\n');
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
