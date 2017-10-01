#include <avr/io.h>
#include <stdint.h>                 // needed for uint8_t
#include <util/delay.h>

int main(void)
{
    DDRB = 0xFF; // all B as output
    PORTB = 0xFF; // all low

    while(1 == 1)
    {
	PORTB |= _BV(PB0);
    }
    return 0;

}
