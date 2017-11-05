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

#define XLEFT 0xA0
#define XRIGHT 0xA1

#define YLEFT 0xB0
#define YRIGHT 0xB1

#define ZLEFT 0xC0
#define ZRIGHT 0xC1

#define RIGHT 1
#define LEFT 0

// UART
#define FOSC 1000000 	// Clock Speed
#define BAUD 4800
#define MYUBRR (FOSC/16/BAUD -1)
#define TIMEOUT 100

uint16_t XROT, YROT, ZROT;

void delay(void)
{
	_delay_ms(2);
	_delay_us(500);
}

void rotateX(uint8_t direction)
{	
	uint16_t i;
	for(i = 0; i < XROT; i++)
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
}

void rotateY(uint8_t direction)
{	
	uint16_t i;
	for(i = 0; i < YROT; i++)
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
}

void rotateZ(uint8_t direction)
{	uint16_t i;
	for(i = 0; i < ZROT; i++)
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
    
    XROT = 1;
    YROT = 1;
    ZROT = 1;
        
    initUART();
    
    uint8_t command;

    while(1 == 1)
    {
		command = getChar();
		
		if(command != 0)
		{
			if((command >= XLEFT) & (command < YLEFT))
			{
				command -= XLEFT;
				if(command <= 1)
				{
					rotateX(command);
				}
				else if(command == 2)
				{
					XROT = 1;
				}
				else
				{
					XROT = 64*(command - 2);
				}
			}
			else if((command >= YLEFT) & (command < ZLEFT))
			{
				command -= YLEFT;
				if(command <= 1)
				{
					rotateY(command);
				}
				else if(command == 2)
				{
					YROT = 1;
				}
				else
				{
					YROT = 64*(command - 2);
				}
			}
			if(command >= ZLEFT)
			{
				command -= ZLEFT;
				if(command <= 1)
				{
					rotateZ(command);
				}
				else if(command == 2)
				{
					ZROT = 1;
				}
				else
				{
					ZROT = 64*(command - 2);
				}
			}
			PORTB = 0x00;
			PORTD = 0x00;
			sendChar(0xFF);
		}
    }
    return 0;
}
