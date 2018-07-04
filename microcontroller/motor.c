#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>

#include "uart.h"

#define PORT_X PORTB
#define PORT_Y PORTA
#define PORT_Z PORTA
#define delay 2.5 

const uint8_t MX_0[4] = {_BV(PB0), _BV(PB1), _BV(PB2), _BV(PB3)};
const uint8_t MX_1[4] = {_BV(PB3), _BV(PB2), _BV(PB1), _BV(PB0)};

const uint8_t MY_0[4] = {_BV(PA0), _BV(PA1), _BV(PA2), _BV(PA3)};
const uint8_t MY_1[4] = {_BV(PA3), _BV(PA2), _BV(PA1), _BV(PA0)};

const uint8_t MZ_0[4] = {_BV(PA4), _BV(PA5), _BV(PA6), _BV(PA7)};
const uint8_t MZ_1[4] = {_BV(PA7), _BV(PA6), _BV(PA5), _BV(PA4)};

void makeRotation(volatile uint8_t *port, const uint8_t *bits)
{
	int8_t i;
	for(i = 0; i < 4; i++)
	{
		*port |= bits[(i + 1) % 4] | bits[(i + 2) % 4];
		*port &= ~bits[i];
		_delay_ms(delay);
	}
}

uint8_t getDirection(uint8_t val)
{
  return val &= 1;
}

uint8_t getMotor(uint8_t val)
{
  val &= 0b00000110;
  return (val >> 1);
}

uint8_t getSteps(uint8_t val)
{
  val &= 0b11111000;
  return (val >> 3) + 1;
}

void rotateFromUart(uint8_t command)
{
	uint8_t motor, steps, direction, i = 0;
	motor = getMotor(command);
	steps = getSteps(command);
	direction = getDirection(command);
	if (motor == 0)
	{
		if(direction)
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_X, MX_1);
			}
		}
		else
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_X, MX_0);
			}
		}
		PORT_X &= ~(MX_0[0] | MX_0[1] | MX_0[2] | MX_0[3]); 
	}
	if (motor == 1)
	{
		if(direction)
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_Y, MY_1);
			}
		}
		else
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_Y, MY_0);
			}
		}
		PORT_Y &= ~(MY_0[0] | MY_0[1] | MY_0[2] | MY_0[3]); 
	}
	if (motor == 2)
	{
		if(direction)
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_Z, MZ_1);
			}
		}
		else
		{
			for(i = 0; i < steps; i++)
			{
				makeRotation(&PORT_Z, MZ_0);
			}
		}
		PORT_Z &= ~(MZ_0[0] | MZ_0[1] | MZ_0[2] | MZ_0[3]); 
	}
}

int main(void)
{
    DDRB = 0xFF & ~(1 << UART_RX); // all B as output
    DDRA = 0xFF;
    PORT_X = 0 | (1 << UART_RX); // all low
    PORT_Y = 0;
    PORT_Z = 0;
    
    rotateFromUart(0b11111001);
	rotateFromUart(0b11111011);
    rotateFromUart(0b11111101);
    
    uint8_t command;
    while(1)
    {
		command = uart_getc();
		rotateFromUart(command);
    }
    return 0;
}
