#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>
#include "uart.h"

#define PORT_X PORTB
#define PORT_Y PORTA
#define PORT_Z PORTA
#define delay 2.5

uint8_t MX[4] = {_BV(PB0), _BV(PB1), _BV(PB2), _BV(PB3)};
uint8_t MY[4] = {_BV(PA0), _BV(PA1), _BV(PA2), _BV(PA3)};
uint8_t MZ[4] = {_BV(PA4), _BV(PA5), _BV(PA6), _BV(PA7)};

void makeRotation(volatile uint8_t *port, uint8_t *bits, uint8_t steps, uint8_t dir)
{
	int16_t i;
	if(dir)
	{
		for(i = 0; i < 4*steps; i++)
		{
			*port |= bits[i%4];
			_delay_ms(delay);
			*port ^= bits[i%4];
		}
	}
	else
	{
		for(i = 4*steps; i > 0; i--)
		{
			*port |= bits[i%4];
			_delay_ms(delay);
			*port ^= bits[i%4];
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
  return (val >> 3);
}

void rotateFromUart(uint8_t command)
{
	uint8_t motor;
	motor = getMotor(command);
	if (motor == 0)
	{
		makeRotation(&PORT_X, MX, getSteps(command), getDirection(command));
	}
	if (motor == 1)
	{
		makeRotation(&PORT_Y, MY, getSteps(command), getDirection(command));
	}
	if (motor == 2)
	{
		makeRotation(&PORT_Z, MZ, getSteps(command), getDirection(command));
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
    _delay_ms(1000);
	rotateFromUart(0b11111011);
    _delay_ms(1000);
    rotateFromUart(0b11111101);
    
    uint8_t command;
    while(1)
    {
		command = uart_getc();
		rotateFromUart(command);
    }
    return 0;
}
