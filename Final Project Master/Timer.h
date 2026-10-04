/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Timer.h
 * Description:      The Timer header file.
 */

#ifndef TIMER_H
#define TIMER_H

#include "stm32f446xx.h"
#include "stdio.h"

#define MS 16000      // Milliseconds
#define US 16         // Microseconds
#define INV_PWM 0x070 // Inverted PWM
#define GPIO_PWM GPIOA
#define TRIG_PERIOD 60000 // Trigger period (microseconds)
#define ECHO_PERIOD 23200 // Echo period (microseconds)
#define PULSE 10          // Output Pulse Width (microseconds)

extern double distance;
extern uint8_t hall;
extern uint8_t check_rtc;
extern uint8_t menu_timeout;
extern uint8_t speed_timeout;
extern uint8_t sonar_timeout;
extern uint32_t speed_start;
extern uint32_t speed_end;
extern uint8_t hall;
extern uint8_t speed_measure;
extern uint8_t stopped;

void Timer_Init(void);
void TIM5_Ch1_Init(void);
void TIM2_Ch2_Init(void);
void TIM3_Ch1_Init(void);
void TIM4_Ch1_Init(void);
void TIM7_Ch1_Init(void);
void TIM8_Ch1_Init(void);
extern void TIM4_IRQHandler(void);
extern void TIM7_IRQHandler(void);
extern void TIM2_IRQHandler(void);
extern void TIM3_IRQHandler(void);
extern void TIM8_UP_TIM13_IRQHandler(void);

#endif
