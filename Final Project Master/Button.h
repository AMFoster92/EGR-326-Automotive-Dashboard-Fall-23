/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Button.h
 * Description:      The button header file.
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f446xx.h"

#define GPIO_BTN GPIOC
#define PIN_BTN 4
#define INPUT 0x00UL

#define PULLUP 0x01UL
#define DEBOUNCE 200
#define DEBOUNCE_TIMER TIM5

extern uint8_t btn_flag;
extern uint32_t btn_time;

void Button_Init(void);
uint8_t Check_Button(GPIO_TypeDef *, uint32_t pin);

extern void EXTI4_IRQHandler(void); // Increment number of presses

#endif
