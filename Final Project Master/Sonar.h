/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Sonar.h
 * Description:      The Sonar sensor header file.
 */

#define GPIO_SONAR GPIOA
// #define GPIO_LED		GPIOB
// #define ECHO_LED		6
#define ECHO 1
#define PULLDOWN 0x04
#define INPUT 0x00UL
#define ALTERNATE 0x02
#define ECHO_TIM TIM2
#define TO_CM 58.0
#define TO_IN 148.0

#include "stm32f446xx.h"
#include <stdlib.h>
#include <stdio.h>

void Sonar_Init(void);
