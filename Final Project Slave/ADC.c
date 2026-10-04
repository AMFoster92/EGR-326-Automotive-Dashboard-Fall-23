/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             ADC.c
 * Description:      The Analog to Digital Converter definition file.
 */

#include "ADC.h"

void ADC_Init(void)
{
	GPIOC->MODER |= 0x03 << 0;

	RCC->APB2ENR |= 1 << 8; // ENABLE ADC1 RCC
	ADC1->SMPR2 |= 0;		// Sampling Time: 3 cycles in Ch10
	ADC1->CR1 = ADCH10;		// CH10 Selected with Watchdog, 12 bit res
	ADC1->CR2 = 0;			// Reset
	ADC1->CR2 |= 0x801;		// ADC enabled, Single Conversion, Left align
	ADC1->SQR1 = 0;			// 1 conversion will take place
	ADC1->SQR3 = ADCH10;	// CH1 ON
}