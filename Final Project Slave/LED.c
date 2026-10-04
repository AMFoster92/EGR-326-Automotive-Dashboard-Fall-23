/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             LED.c
 * Description:      The LED definition file.
 */

#include "LED.h"
#include <stdio.h>

static uint8_t color = 0;

// Initializes the LED I/O ports.
void LED_Init(void)
{
	GPIO_LED->MODER |= OUTPUT << PIN_RED * 2;
	// GPIO_LED->MODER |= OUTPUT << PIN_GREEN * 2;
	// GPIO_LED->MODER |= OUTPUT << PIN_BLUE * 2;

	GPIO_LED->BSRR = CLEAR;
}

// Turns LED ON and OFF
void LED_Blink(void)
{
	/*if(part == 1)
	{
	GPIO_LED->ODR ^= 0x01;
	}
	else
	{
		if(color == 1)
		{
			GPIO_LED->ODR ^= 0x01;
		}
		else if(color == 2)
		{
			GPIO_LED->ODR ^= 0x04;
		}
		else
		{
			GPIO_LED->ODR ^= 0x10;
		}
	}*/
}

// Changes the active LED color by shifting the output data register left.
void LED_Shift_Fwd(void)
{
	// If no LED is on...
	if (!(GPIO_LED->ODR | 0))
	{
		// Turn on first color
		GPIO_LED->ODR = 0x01;
		color = 1;
	}
	else
	{
		// Shift output two bits to the left
		GPIO_LED->ODR <<= 2;

		color++;

		// Loop back to first color if on last color
		if (color == 4)
		{
			GPIO_LED->ODR = 0x01;
			color = 1;
		}
		printf("ODR: %x\tCOLOR:%d\n", GPIO_LED->ODR, color);
	}
}

// Changes the active LED color by shifting the output data register right.
void LED_Shift_Bck(void)
{
	// If no LED is on...
	if (!(GPIO_LED->ODR | 0))
	{
		// Turn on last color
		GPIO_LED->ODR = 0x10;
		color = 3;
	}
	else
	{
		// Shift the output two bits to the right
		GPIO_LED->ODR >>= 2;
		color--;

		// Loop back to the last color if on the first color
		if (color == 0)
		{
			GPIO_LED->ODR = 0x10;
			color = 3;
		}
	}
}

// Turns off and resets the active LED color
void LED_Clear(void)
{
	GPIO_LED->ODR = 0;
	color = 0;
}

void LED_On(void)
{
	if (color != 0)
		GPIO_LED->ODR = 0x01 << (color - 1) * 2;
}
