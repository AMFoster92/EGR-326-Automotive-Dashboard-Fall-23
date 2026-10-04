/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Debounce.c
 * Description:      The button debouncer definition file.
 */

#include "Debounce.h"
#include <stdio.h>

// Debounces the forward button when released.
uint8_t DebounceSwitch(GPIO_TypeDef *gpio, uint32_t pin)
{
	static uint32_t SW_State = 0x0; // Debounce state

	// Current Debounce State + Button Pin State = updated FWD_State
	SW_State = (uint32_t)((SW_State << 1) | (uint32_t)(!(gpio->IDR & 1 << pin)));

	// printf("BTN1:%x\n", FWD_State);
	// if pattern matches, button debounced return 1
	/*if (SW_State == PRESSED)
	{
		return 0x01;
	}*/

	//	if (FWD_State == HELD)
	//	{
	//		return 0x02;
	//	}
	//
	if (SW_State == RELEASED)
	{
		return 0x03;
	}

	return 0x00;
}
