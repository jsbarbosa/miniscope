#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>

#define Mx0 _BV(PB0)
#define Mx1 _BV(PB1)
#define Mx2 _BV(PB2)
#define Mx3 _BV(PB3)

#define My0 _BV(PB4)
#define My1 _BV(PB5)
#define My2 _BV(PB6)
#define My3 _BV(PB7)

#define Mz0 _BV(PD2)
#define Mz1 _BV(PD3)
#define Mz2 _BV(PD4)
#define Mz3 _BV(PD5)

#define XLEFT 0x01
#define XRIGHT 0x02

#define YLEFT 0xA1
#define YRIGHT 0xA2

#define ZLEFT 0xB1
#define ZRIGHT 0xB2

#define RIGHT 1
#define LEFT 0

// UART
#define FOSC 1000000 	// Clock Speed
#define BAUD 4800
#define MYUBRR (FOSC/16/BAUD -1)
#define TIMEOUT 100

void delay(void)
{
	_delay_ms(3);
	//_delay_us(500);
}

void rotateX(uint8_t direction)
{	
	if(direction == RIGHT)
	{
		PORTB = Mx0;
		delay();
		PORTB = Mx1;
		delay();
		PORTB = Mx2;
		delay();
		PORTB = Mx3;
		delay();
	}
	else
	{
		PORTB = Mx3;
		delay();
		PORTB = Mx2;
		delay();
		PORTB = Mx1;
		delay();
		PORTB = Mx0;
		delay();
	}
}

void rotateY(uint8_t direction)
{	
	if(direction == RIGHT)
	{
		PORTB = My0;
		delay();
		PORTB = My1;
		delay();
		PORTB = My2;
		delay();
		PORTB = My3;
		delay();
	}
	else
	{
		PORTB = My3;
		delay();
		PORTB = My2;
		delay();
		PORTB = My1;
		delay();
		PORTB = My0;
		delay();
	}
}

void rotateZ(uint8_t direction)
{		
	if(direction == RIGHT)
	{
		PORTD = Mz0;
		delay();
		PORTD = Mz1;
		delay();
		PORTD = Mz2;
		delay();
		PORTD = Mz3;
		delay();
	}
	else
	{
		PORTD = Mz3;
		delay();
		PORTD = Mz2;
		delay();
		PORTD = Mz1;
		delay();
		PORTD = Mz0;
		delay();
	}
}

void initUART(void)
{
	/*Set baud rate */
    UBRR0H = (MYUBRR >> 8);
    UBRR0L = MYUBRR;

    UCSR0B = (1 << TXEN0)| (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); ;      // Enable receiver and transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);    // Set frame: 8data, 1 stp

	DDRD &= 0b11111110; //set all of port D as inputs except for TX
}

uint8_t getChar(void)
{
	uint16_t i;
	for(i = 0; i < TIMEOUT; i++)
	{
		if (UCSR0A & (1<<RXC0)) return (char) UDR0;
    }
    return 0;
}

void sendChar(uint8_t tosend)
{
	while (( UCSR0A & (1<<UDRE0))  == 0){};
	UDR0 = tosend;
}

int main(void)
{
    DDRB = 0xFF; // all B as output
    PORTB = 0x00; // all low
    
    DDRD = 0xFF; // all B as output
    PORTD = 0x00; // all low
        
    initUART();
    
    uint8_t command;

    while(1 == 1)
    {
		command = getChar();
		
		if(command != 0)
		{
			if(command == XRIGHT)
			{
				rotateX(RIGHT);
			}
			else if(command == XLEFT)
			{
				rotateX(LEFT);
			}
			
			else if(command == YRIGHT)
			{
				rotateY(RIGHT);
			}
			else if(command == YLEFT)
			{
				rotateY(LEFT);
			}
			
			else if(command == ZRIGHT)
			{
				rotateZ(RIGHT);
			}
			else if(command == ZLEFT)
			{
				rotateZ(LEFT);
			}
			
			PORTB = 0x00;
			PORTD = 0x00;
			sendChar(0xFF);
		}
    }
    return 0;
}
