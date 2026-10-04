/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Hall.h
 * Description:      The Hall Effect sensor header file.
 */

#ifndef HALL_H
#define HALL_H

#include "stm32f446xx.h"

#define GPIO_HALL GPIOA
#define PIN_HALL 6
#define MOTOR_DIAMETER 1 // motor gear radius in inches
#define TIRE_RADIUS 18   // tire radius of 1.5 feet in inches

extern uint8_t hall;

void Hall_Init(void);

uint8_t Hall_Read(void);

extern void EXTI9_5_IRQHandler(void);

#endif