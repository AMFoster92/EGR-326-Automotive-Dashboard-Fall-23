/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Button.c
 * Description:      The button definition file.
 */

#include "Button.h"
// #include "Debounce.h"
#include <stdio.h>

// Initialize button pins as input with pullup.
void Button_Init(void)
{
	// RCC->APB2ENR |= (1<<14);	//Enable clock

	GPIO_BTN->MODER |= INPUT << PIN_BTN * 2;

	GPIO_BTN->PUPDR |= PULLUP << PIN_BTN * 2;

	// Configure Interrupts
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PC;

	EXTI->IMR |= (1UL << PIN_BTN);	 // Unmask interrupt
	EXTI->RTSR &= ~(1UL << PIN_BTN); // Set Rising Edge Trigger
	EXTI->FTSR |= (1UL << PIN_BTN);	 // Set Falling Edge Trigger

	// Enable Interrupt Vectors
	NVIC_EnableIRQ(EXTI4_IRQn);
}

// Check the current state of the button.
// If pressed return 1, else return 0.
uint8_t Check_Button(GPIO_TypeDef *button, uint32_t pin)
{
	if (!(button->IDR & 1 << pin))
	{
		return 0x01;
	}
	return 0x00;
}

void EXTI4_IRQHandler(void)
{
	btn_flag = 1;
	btn_time = TIM7->CNT;

	// Clear interrupt flag
	EXTI->PR |= (1 << PIN_BTN);
}
