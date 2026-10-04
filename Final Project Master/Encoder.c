/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Encoder.c
 * Description:      The rotary encoder definition file.
 */

#include "Encoder.h"

void Encoder_Init(void)
{
	RCC->APB2ENR |= (1 << 14);

	// Set I/O
	GPIO_ENCODER->MODER &= ~(0x00UL << PIN_CLK * 2);
	GPIO_ENCODER->MODER &= ~(0x00UL << PIN_DT * 2);
	GPIO_ENCODER->MODER &= ~(0x00UL << PIN_SW * 2);
	// GPIO_ENCODER->MODER |= (0x01UL << PIN_CCWLED * 2);
	// GPIO_ENCODER->MODER |= (0x01UL << PIN_CWLED * 2);

	// Pullup Inputs
	GPIO_ENCODER->PUPDR |= 0x01UL << PIN_CLK * 2;
	GPIO_ENCODER->PUPDR |= 0x01UL << PIN_DT * 2;
	GPIO_ENCODER->PUPDR |= 0x01UL << PIN_SW * 2;

	// Configure Interrupts
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PB;

	// Unmask interrupt, and check both edges for knob
	EXTI->IMR |= (1 << PIN_CLK) | (1 << PIN_SW);
	EXTI->RTSR |= (1 << PIN_CLK) | (1 << PIN_SW);
	EXTI->FTSR |= (1 << PIN_CLK) | (1 << PIN_SW);

	// Enable interrupts
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);

	// Get initial encoder state
	prev |= (GPIO_ENCODER->IDR & (1 << PIN_DT)) | (GPIO_ENCODER->IDR & (1 << PIN_CLK));
}

void EXTI0_IRQHandler(void)
{
	if (!encoder_cw && !encoder_ccw)
	{
		// Get interrupt time
		// uint32_t time = TIM5->CNT;
		TIM5->CNT = 0;
		TIM5->CR1 |= 0x01;

		// Wait for 1ms to allow inputs to settle
		while (TIM5->CNT < 1000)
		{
			// printf("%d\n", TIM5->CNT);
		}

		TIM5->CNT = 0;
		TIM5->CR1 &= ~(0x01UL);

		// Get current pin statuses
		cur = GPIO_ENCODER->IDR & 0x03;

		// If state changed
		if (cur ^ prev)
		{
			// Check if clockwise rotation
			if ((!(cur ^ 0x01)) || (!(cur ^ 0x02)))
			{
				encoder_cw = 1;
			}

			// Check if counter-clockwise rotation
			else if ((!(cur ^ 0x00)) || (!(cur ^ 0x03)))
			{
				encoder_ccw = 1;
			}

			// Increase count
			// cnt++;

			// Store current state as previous
			// prev = cur;
		}
	}
	// Clear input interrupt
	EXTI->PR |= (1 << PIN_CLK);
}

void EXTI2_IRQHandler(void)
{
	// Set switch flag
	switch_flag = 1;

	// Clear input interrupt
	EXTI->PR |= (1 << PIN_SW);
}