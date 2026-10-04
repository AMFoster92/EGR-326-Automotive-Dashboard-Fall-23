/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Unipolar Stepper Motor.h
 * Description:      The Unipolar Stepper Motor header file.
 */

#ifndef UNIPOLAR_STEPPER_H
#define UNIPOLAR_STEPPER_H
#include "stm32f446xx.h"

#define GPIO_UNIPOLAR_STEPPER GPIOA
#define UNI_INT1 8
#define UNI_INT2 9
#define UNI_INT3 10
#define UNI_INT4 11
#define UNI_MAX_STEPS 600
// #define MAX_SEQUENCES						512

#define SPEED_0 250 // delay times (milliseconds)
#define SPEED_1 200
#define SPEED_2 150
#define SPEED_3 100
#define SPEED_4 50
#define SPEED_5 25

extern uint8_t unipolar_motor_state;
extern uint8_t delay;

void UnipolarStepper_Init(void);

void UnipolarStep_Increment(void);
void UnipolarStep_Decrement(void);

void UnipolarStep_Motor(uint16_t, uint8_t);

void UnipolarMotor_Speed(uint8_t);

extern void SysTick_msdelay(uint16_t);

#endif
