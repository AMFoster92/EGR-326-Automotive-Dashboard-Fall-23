/*
 * Name:             Aaron Foster
 * Course:           EGR 227 - Microcontroller Programming and Applications
 * Project:          Final Project - Speedometer
 * File:             GPIOInit.h
 * Description:      The GPIO initialization header file.
 */

#ifndef GPIOINIT_H
#define GPIOINIT_H

#include "stm32f446xx.h"
#include <stdint.h>

void GPIO_Init(void);

void RCC_Init(GPIO_TypeDef *gpio);

void REG_Init(GPIO_TypeDef *gpio);

#endif
