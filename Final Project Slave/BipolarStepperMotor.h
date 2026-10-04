/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             BipolarStepperMotor.h
 * Description:      The Bipolar Stepper Motor header file.
 */

#ifndef BIPOLAR_STEPPER_H
#define BIPOLAR_STEPPER_H
#include "stm32f446xx.h"

#define GPIO_BIPOLAR_STEPPER GPIOC
#define BIPOLAR_INT1 10
#define BIPOLAR_INT2 11
#define BIPOLAR_INT3 12
#define BIPOLAR_INT4 13
#define BIPOLAR_MAX_STEPS 1000
#define IWDG_RELOAD 0x0AAAA
#define MAX_POS 160
// #define MAX_SEQUENCES

static uint8_t bipolar_motor_state;
static uint8_t cur_pos;

void BipolarStepper_Set_Pos(uint8_t pos);
void BipolarStepper_Init(void);

void BipolarStep_Increment(void);
void BipolarStep_Decrement(void);

void BipolarStep_Motor(uint16_t, uint8_t);

extern void delayMS(uint16_t);

#endif