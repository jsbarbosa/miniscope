#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>

#define Mx0 _BV(PB0)
#define Mx1 _BV(PB1)
#define Mx2 _BV(PB2)
#define Mx3 _BV(PB3)

#define RIGHT 1
#define LEFT 0

void rotateX(uint8_t delay_ms, uint8_t direction)
{	
	delay_ms = 3;
	
	if(direction == RIGHT)
	{
		PORTB = Mx0;
		_delay_ms(delay_ms);
		PORTB = Mx1;
		_delay_ms(delay_ms);
		PORTB = Mx2;
		_delay_ms(delay_ms);
		PORTB = Mx3;
		_delay_ms(delay_ms);
	}
	else
	{
		PORTB = Mx3;
		_delay_ms(delay_ms);
		PORTB = Mx2;
		_delay_ms(delay_ms);
		PORTB = Mx1;
		_delay_ms(delay_ms);
		PORTB = Mx0;
		_delay_ms(delay_ms);
	}
}

int main(void)
{
    DDRB = 0xFF; // all B as output
    PORTB = 0x00; // all low

    while(1 == 1)
    {
		rotateX(10, LEFT);
    }
    return 0;
}
