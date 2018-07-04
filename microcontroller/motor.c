#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>

#include "uart.h"

#define PORT_X PORTB
#define PORT_Y PORTA
#define PORT_Z PORTA
#define delay 2.5 

uint8_t MX[4] = {_BV(PB3), _BV(PB2), _BV(PB1), _BV(PB0)};
uint8_t MY[4] = {_BV(PA3), _BV(PA2), _BV(PA1), _BV(PA0)};
uint8_t MZ[4] = {_BV(PA7), _BV(PA6), _BV(PA5), _BV(PA4)};

void makeRotation(volatile uint8_t *port, uint8_t *bits, uint8_t dir)
{
	int8_t i;
	if(dir)
	{
		for(i = 0; i < 4; i++)
		{
			*port |= bits[(i + 1) % 4] | bits[(i + 2) % 4];
			*port &= ~bits[i];
			_delay_ms(delay);
		}
	}
	else
	{
		for(i = 3; i >= 0; i--)
		{
			*port |= bits[(i + 1) % 4] | bits[(i + 2) % 4];
			*port &= ~bits[i];
			_delay_ms(delay);
		}
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
		for(i = 0; i < steps; i++)
		{
			makeRotation(&PORT_X, MX, direction);
		}
		PORT_X &= ~(MX[0] | MX[1] | MX[2] | MX[3]); 
	}
	if (motor == 1)
	{
		for(i = 0; i < steps; i++)
		{
			makeRotation(&PORT_Y, MY, direction);
		}
		PORT_Y &= ~(MY[0] | MY[1] | MY[2] | MY[3]); 
	}
	if (motor == 2)
	{
		for(i = 0; i < steps; i++)
		{
			makeRotation(&PORT_Z, MZ, direction);
		}
		PORT_Z &= ~(MZ[0] | MZ[1] | MZ[2] | MZ[3]); 
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
