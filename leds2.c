#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>

#define ADC_PIN	0
#define ADC_THRESHOLD 1023

#define F_CPU 1000000UL
#define BAUD 960

#include <util/setbaud.h>

void uart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

	#if USE_2X
    UCSR0A |= _BV(U2X0);
	#else
    UCSR0A &= ~(_BV(U2X0));
	#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
    {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char uart_getchar(FILE *stream) 
{
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

uint16_t adc_read(uint8_t adcx);

int main(void)
{	
	uart_init();
	stdout = &uart_output;
    stdin  = &uart_input;
	
	DDRB = 0xFF; 
	
	ADCSRA |= _BV(ADEN);
	
	while (1)
	{
		_delay_ms(500);
        printf("You wrote\n");
	}

	return (0); //this line is never actually reached
}

uint16_t adc_read(uint8_t adcx)
{
	ADMUX	&=	0xf0;
	ADMUX	|=	adcx;

	ADCSRA |= _BV(ADSC);

	while ( (ADCSRA & _BV(ADSC)) );

	return ADC;
}

void charge(uint8_t adcpin)
{
	int delta = 200;
	int first = delta;
	int second = 2*delta;
	int third = 3*delta;
	int fourth = 4*delta;
	int fifth = 5*delta;
	uint16_t value = adc_read(adcpin);
	
	if(value > fifth)
	{
		PORTB = 0b00111110;
	}
	else if(value > fourth)
	{
		PORTB = 0b00011110;
	}
	else if(value > third)
	{
		PORTB = 0b00001110;
	}
	else if(value > second)
	{
		PORTB = 0b00000110;
	}
	else if(value > first)
	{
		PORTB = 0b00000010;
	}
	else
	{
		PORTB = 0x00;
	}
}
