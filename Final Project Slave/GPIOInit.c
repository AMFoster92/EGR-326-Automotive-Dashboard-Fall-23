/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             GPIOInit.c
 * Description:      The GPIO initialization definition file.
 */

#include "GPIOInit.h"

void GPIO_Init(void)
{
	RCC_Init(GPIOA);
	REG_Init(GPIOA);

	RCC_Init(GPIOB);
	REG_Init(GPIOB);

	RCC_Init(GPIOC);
	REG_Init(GPIOC);

	RCC_Init(GPIOD);
	REG_Init(GPIOD);
}

void RCC_Init(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA)
	{
		RCC->AHB1ENR |= 0x1;
	}
	if (gpio == GPIOB)
	{
		RCC->AHB1ENR |= 0x2;
	}
	if (gpio == GPIOC)
	{
		RCC->AHB1ENR |= 0x4;
	}
	if (gpio == GPIOD)
	{
		RCC->AHB1ENR |= 0x8;
	}
}

void REG_Init(GPIO_TypeDef *gpio)
{
	if (gpio == GPIOA)
	{
		gpio->MODER &= ~0xC00UL;
		gpio->PUPDR |= 0x64000000UL;
	}
	else if (gpio == GPIOB)
	{
		gpio->MODER &= 0x280UL;
		gpio->PUPDR |= 0x100UL;
	}
	else
	{
		gpio->MODER &= ~0x0UL;
		gpio->PUPDR |= 0x0UL;
	}
	gpio->BSRR |= 0xFFFF0000UL;
}
