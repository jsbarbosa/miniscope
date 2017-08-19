 #include <avr/io.h> //Defines pins, ports, etc.

#include <util/delay.h>

int main(void) {

DDRB |= (1 << PB0); //Data Direction Register B: writing a 1 to the bit enables output

while (1) {
PORTB= 0b00000001; //turns on LED attached to port PB0
_delay_ms(1000); //creates a 1-second delay
PORTB= 0b00000000; //Turns off all B pins, including PB0
_delay_ms(1000); //creates another 1-second delay
}

return (0); //this line is never actually reached
}