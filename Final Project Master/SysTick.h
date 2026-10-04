/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             SysTick.h
 * Description:      The SysTick header file.
 */

#ifndef SYSTICK_H
#define SYSTICK_H
#include "stm32f446xx.h"

void SysTick_Init(void);
void SysTick_msdelay(uint16_t msdelay);

#endif
