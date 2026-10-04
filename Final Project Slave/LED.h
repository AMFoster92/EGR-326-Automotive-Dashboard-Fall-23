/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             LED.h
 * Description:      The LED header file.
 */

#ifndef LED_H
#define LED_H

#include "stm32f446xx.h"

#define GPIO_LED GPIOC
#define PIN_RED 10
#define PIN_GREEN 2
#define PIN_BLUE 4
#define RESET_LED 6
#define OUTPUT 0x01UL
#define CLEAR 0xFFFF0000UL

void LED_Init(void);      // Initialized LED outputs
void LED_Blink(void);     // Turn LED ON and OFF
void LED_Shift_Fwd(void); // Shift LED output register left
void LED_Shift_Bck(void); // Shift LED output register right
void LED_Clear(void);     // Clear LED output register
void LED_On(void);

#endif
