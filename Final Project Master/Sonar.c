/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Sonar.c
 * Description:      The Sonar sensor initialization definition file.
 */

#include "Sonar.h"

void Sonar_Init(void)
{
	// Set alternate function
	GPIO_SONAR->MODER |= (ALTERNATE << ECHO * 2);
	GPIO_SONAR->MODER |= ALTERNATE;
	// GPIO_LED->MODER |= (ALTERNATE << ECHO_LED*2);
	// GPIO_SONAR->AFR[0] &= 0xF00000;
	GPIO_SONAR->AFR[0] |= 0x02;
	GPIO_SONAR->AFR[0] |= 0x10;
	// GPIO_LED->AFR[0] &= 0xF00000;
	// GPIO_LED->AFR[0] |= (0x12 << ECHO_LED*4);
}
