/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Timer.c
 * Description:      The Timer initialization definition file.
 */

#include "Timer.h"
#include <stdlib.h>

void Timer_Init(void)
{
	TIM5_Ch1_Init();
	TIM2_Ch2_Init();
	TIM3_Ch1_Init();
	TIM4_Ch1_Init();
	TIM7_Ch1_Init();

	TIM8_Ch1_Init();
}

// Configure timer to send Trigger pulse
void TIM5_Ch1_Init(void)
{
	RCC->APB1ENR |= 0x08UL;
	TIM5->PSC = US - 1; // Set to 1us per tick
	TIM5->ARR = TRIG_PERIOD - 1;
	TIM5->CNT = 0;
	TIM5->EGR = 1;
	TIM5->CCMR1 = INV_PWM;
	TIM5->CCR1 = PULSE - 1;
	TIM5->CCER |= 0x01;
	// TIM5->CR1 |= 0x01;
}

// Configure timer to measure Echo pulse width
void TIM2_Ch2_Init(void)
{
	RCC->APB1ENR |= 0x01UL; // Enable clock
	TIM2->PSC = US - 1;		// Set to 1us per tick
	TIM2->ARR = ECHO_PERIOD - 1;
	TIM2->EGR = 1;
	TIM2->CNT = 0;
	// TIM2->CR1 |= 0x01;
	TIM2->SR = 0;
	TIM2->DIER = 0x03;
	TIM2->CCMR1 |= 0x4100; // Configure as input capture
	TIM2->CCER |= 0x10;	   // Set to detect rising edge
	// TIM2->CR1 |= 0x01;						//Enable clock

	NVIC_EnableIRQ(TIM2_IRQn);
}

// Configure timer to be used for determining speed
void TIM3_Ch1_Init(void)
{
	RCC->APB1ENR |= 0x02UL;
	TIM3->PSC = MS - 1;
	TIM3->ARR = 0xFFFF - 1;
	TIM3->CCMR1 = 0x0041;
	TIM3->CCER |= 0x01;
	TIM3->EGR = 1;
	TIM3->CNT = 0;
	TIM3->CR1 |= 0x01;
	TIM3->SR = 0;
	TIM3->DIER = 0x02;

	NVIC_EnableIRQ(TIM3_IRQn);

	/*
	TIM3->ARR = 2000 - 1;
	TIM3->EGR = 1;
	TIM3->CNT = 0;
	TIM3->CR1 |= 0x01;
	TIM3->SR = 0;
	TIM3->DIER = 0x01;

	NVIC_EnableIRQ(TIM3_IRQn);
	*/
}

// Configure timer to check RTC on interrupt
void TIM4_Ch1_Init(void)
{
	RCC->APB1ENR |= (0x04);
	TIM4->PSC = MS - 1;
	TIM4->ARR = 60000 - 1;
	TIM4->EGR = 1;
	TIM4->CNT = 0;
	TIM4->CR1 |= 0x01;
	TIM4->SR = 0;
	TIM4->DIER = 0x01;
	TIM4->CR1 = 0x01;

	NVIC_EnableIRQ(TIM4_IRQn);
}

// Configure timer to timeout menu Ch1
void TIM7_Ch1_Init(void)
{
	RCC->APB1ENR |= (0x20);
	TIM7->PSC = MS - 1;
	TIM7->ARR = 60000 - 1;
	TIM7->EGR = 1;
	TIM7->CNT = 0;
	// TIM7->CR1 |= 0x01;
	TIM7->SR = 0;
	TIM7->DIER = 0x01;
	// TIM1->CR1 = 0x01;

	NVIC_EnableIRQ(TIM7_IRQn);
}

// Configure timer to check if motion stopped.
void TIM8_Ch1_Init(void)
{
	RCC->APB2ENR |= 0x02UL; // Enable clock
	TIM8->PSC = MS - 1;		// Set to 1us per tick
	TIM8->ARR = 2000 - 1;
	// TIM8->CCMR1 = 0x41;					//Configure as input capture
	// TIM8->CCER |= 0x1;						//Set to detect rising edge
	TIM8->EGR = 1;
	TIM8->CNT = 0;
	TIM8->SR = 0;
	TIM8->DIER = 0x01;
	// TIM8->CR1 |= 0x01;						//Enable clock

	NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
}

void TIM4_IRQHandler(void)
{
	check_rtc = 1;

	TIM4->SR = 0;
}

void TIM7_IRQHandler(void)
{
	menu_timeout = 1;

	TIM7->SR = 0;
}

void TIM2_IRQHandler(void)
{

	if ((TIM2->SR & 1))
	{
		sonar_timeout = 1;
	}

	TIM2->SR = 0;
}

void TIM3_IRQHandler(void)
{
	//	uint32_t temp_time = 0;
	//
	//	if(speed_start > TIM3->CCR1)
	//	{
	//		temp_time = speed_start-TIM3->CCR1;
	//	}
	//	else
	//	{
	//		temp_time = TIM3->CCR1 - speed_start;
	//	}
	//	if(!speed_measure && (temp_time > 10))
	if (!hall)
	{
		speed_measure = 1;
		hall = 1;
	}

	TIM3->SR = 0;
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	stopped = 1;

	TIM8->SR = 0;
}
