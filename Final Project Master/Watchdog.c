/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Watchdog.c
 * Description:      The Independent Watchdog initialization definition file.
 */

#include "Watchdog.h"

void IWDG_Init(void)
{
	IWDG->KR = IWDG_MOD;
	IWDG->PR |= IWDG_DIV32;
	IWDG->RLR |= 999;
	IWDG->KR = IWDG_RELOAD;
	IWDG->KR = IWDG_START;
}

// Stop kicking the dog
static void Kick_The_Dog(void)
{
	kick_dog = 1;
}

void IWDG_Kick(void)
{
	Kick_The_Dog();
}