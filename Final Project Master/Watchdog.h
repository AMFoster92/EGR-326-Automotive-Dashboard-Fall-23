/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Watchdog.h
 * Description:      The Independent Watchdog header file.
 */

#include "stm32f446xx.h"

#define IWDG_START 0x0CCCC
#define IWDG_RELOAD 0x0AAAA
#define IWDG_MOD 0x05555
#define IWDG_DIV32 0x03
#define GPIO_IWDG GPIOC
#define IWDG_BTN 2

extern uint8_t kick_dog;

void IWDG_Init(void);

static void Kick_The_Dog(void);

void IWDG_Kick(void);
