/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             HallEffect.c
 * Description:      The Hall Effect Sensor initialization definition file.
 */

#include "HallEffect.h"

void Hall_Init(void)
{
	GPIO_HALL->MODER |= (0x02 << PIN_HALL * 2);
	GPIO_HALL->PUPDR |= (0x01 << PIN_HALL * 2);

	GPIO_HALL->AFR[0] |= (0x02 << PIN_HALL * 4);

	/*SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PA;

	EXTI->IMR |= 1 << PIN_HALL;
	EXTI->RTSR |= 0 << PIN_HALL;
	EXTI->FTSR |= 1 << PIN_HALL;

	NVIC_EnableIRQ(EXTI9_5_IRQn);*/
}

uint8_t Hall_Read(void)
{
	uint8_t reading = 0x00; // Default return 0

	if (GPIO_HALL->IDR & (1 << PIN_HALL))
	{
		reading = 0x01; // If hall sensor pin high, return 1
	}

	return reading;
}

void EXTI9_5_IRQHandler(void)
{
	hall = 1; // Set hall effect flag

	EXTI->PR |= (1 << PIN_HALL);
}