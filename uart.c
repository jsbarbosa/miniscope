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
#define HALFPOINTSADDR 0x01
#define DELAYXADDR 0x02
#define DELAYYADDR 0x03

#define READ 0x32
#define WRITE 0x16

#define TAKEIMAGE 0x20
#define STOP 0x40

void delay_ms(uint8_t count);

uint16_t adc_read(uint8_t adcx);
uint8_t takeImage(void);
void nextX(int current);
void nextY(int current);
void setPWM(void);

char *getString(uint8_t len);
uint16_t from8to16(uint8_t first, uint8_t second);

char getChar(void);
void sendChar(char tosend);
void sendString(char *tosend, char delimiter);

uint8_t sendMeasurent(int x, int y, uint16_t value);

uint8_t HALFPOINTS, DELAYX, DELAYY;

int main(void)
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
	
	char *string;
	
	uint16_t val;
	
	HALFPOINTS = 10;
	DELAYX = 0;
	DELAYY = 0;

    while(1)
    {
		string = getString(3);
		
		if(string[0] == WRITE)
		{
			if(string[1] == HALFPOINTSADDR)
			{
				HALFPOINTS = string[2];
			}
			else if (string[1] == DELAYXADDR)
			{
				DELAYX = string[2];
			}
			else if (string[1] == DELAYYADDR)
			{
				DELAYY = string[2];
			}
			else
			{
				val = from8to16(string[1], string[2]);
				
				if (val == TAKEIMAGE)
				{
					takeImage();
				}
			}
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

char getChar(void)
{
	while (!(UCSR0A & _BV(RXC0)));
	return (char) UDR0;
}

char *getString(uint8_t len)
{
	uint8_t i;
	char temp[len];
	
	for(i = 0; i<len; i++)
	{
		temp[i] = getChar();
	}
	return temp;
}

uint16_t from8to16(uint8_t first, uint8_t second)
{
	uint16_t x = (first << 8) | second;
	return x;
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

uint8_t sendMeasurent(int x, int y, uint16_t value)
{
	char hex[5], *temp;
	
	uint16_t received;
	
	while (1)
	{
		itoa(x, hex, 10);
		sendString(hex, MULDELIMETER);
		
		itoa(y, hex, 10);
		sendString(hex, MULDELIMETER);
		
		itoa(value, hex, 10);
		sendString(hex, MULDELIMETER);
		
		sendChar(BREAKLINE);
		
		temp = getString(3);
		received = from8to16(temp[1], temp[2]);
		
		if((temp[0] == WRITE) & (received == STOP))
		{
			return 0;
		}
		
		else if((temp[0] == READ) & (received == value))
		{
			return 1;
		}
		
		
	}
}

uint8_t takeImage(void)
{
    int xc = -HALFPOINTS, yc = -HALFPOINTS, stop = 1;
			
	while((yc <= HALFPOINTS) & stop)
	{
		while((xc <= HALFPOINTS) & stop)
		{
			nextX(xc);
			if(sendMeasurent(xc, yc, adc_read(0)))
			{
				xc += 1;
			}
			else
			{
				return 0;
			}
			delay_ms(DELAYX);
		}
		
		nextY(yc);
		yc += 1;
		xc -= 1;
		delay_ms(DELAYY);
		
		while((xc >= -HALFPOINTS) & stop)
		{
			nextX(xc);
			if(sendMeasurent(xc, yc, adc_read(0)))
			{
				xc -= 1;
			}
			else
			{
				return 0;
			}
			delay_ms(DELAYX);
		}
		
		xc += 1;
		
		nextY(yc);
		yc += 1;
		delay_ms(DELAYY);
	}
	return 1;
}

void delay_ms(uint8_t count)
{
  while(count--) 
  {
    _delay_ms(1);
  }
}
