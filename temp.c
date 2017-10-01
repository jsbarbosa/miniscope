

uint16_t first(void)
{
	return 0x00000101;
}

uint16_t second(void)
{
	return 0x00001001;
}

uint16_t third(void)
{
	return 0x00001010;
}

uint16_t fourth(void)
{
	return 0x00000110;
}

void turn(uint16_t *PB, uint8_t direction, uint8_t delay_ms)
{
	delay_ms = 100;
	if (direction == 1)
	{
		*PB = first();
		_delay_ms(delay_ms);
		*PB = second();
		_delay_ms(delay_ms);
		*PB = third();
		_delay_ms(delay_ms);
		*PB = fourth();
		_delay_ms(delay_ms);
	}
}