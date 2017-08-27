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
#define HALFPOINTS 12

uint16_t adc_read(uint8_t adcx);
void nextX(int current);
void nextY(int current);
void setPWM(void);

void sendChar(char tosend);
void sendString(char *tosend, char delimiter);

void sendMeasurent(int x, int y, uint16_t value);

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
	
	setPWM();

	int i, dx = 50, delayx = 5, delayy = 0;

	uint16_t num;
	
    int xc, yc;

    while(1)
    {
		/*
		for(i = 0; i < 6; i++)
		{
			num = adc_read(i);
			itoa(num, hex, 10);

			sendString(hex, MULDELIMETER);
		}
		itoa(current, hex, 10);
		sendString(hex, MULDELIMETER);
		sendChar(BREAKLINE);*/
		
		yc = -HALFPOINTS;
		
		while(yc <= HALFPOINTS)
		{
			while(xc <= HALFPOINTS)
			{
				nextX(xc);
				sendMeasurent(xc, yc, rand()%1023);
				xc += 1;
				_delay_ms(delayx);
			}
			
			nextY(yc);
			yc += 1;
			
			while(xc >= -HALFPOINTS)
			{
				xc -= 1;
				nextX(xc);
				sendMeasurent(xc, yc, rand()%1023);
				_delay_ms(delayx);
			}
			
			nextY(yc);
			yc += 1;
		}
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

void nextX(int current)
{
    PORTB &= 0b11001111;

    if (current < 0){PORTB |= (1 << DDB4);}
    else{PORTB |= (1 << DDB5);}
    
    OCR1A = 127*abs(current)/HALFPOINTS;
}

void nextY(int current)
{
    PORTB &= 0b00111111;

    if (current < 0){PORTB |= (1 << DDB6);}
    else{PORTB |= (1 << DDB7);}
    
    OCR2A = 127*abs(current)/HALFPOINTS;
}

void setPWM(void)
{
	/* SET PWM */

	//DDRB |=  (1 << DDB1) | (1 << DDB2) | (1 << DDB3); // PB1, PB2, PB3 is now an output

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	TCCR2A |= (1 << COM2A1) | (1 << COM2B1);
	// set none-inverting mode

	TCCR1A |= (1 << WGM11) | (1 << WGM10);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	// set fast PWM Mode

	TCCR1B |= (1 << CS10);
	TCCR2B |= (1 << CS20);
	// set prescaler to 8 and starts PWM
}

void sendMeasurent(int x, int y, uint16_t value)
{
	char hex[5];
	
	itoa(x, hex, 10);
	sendString(hex, MULDELIMETER);
	
	itoa(y, hex, 10);
	sendString(hex, MULDELIMETER);
	
	itoa(value, hex, 10);
	sendString(hex, MULDELIMETER);
	
	sendChar(BREAKLINE);
}
