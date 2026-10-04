/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             SysTick.h
 * Description:      The SysTick implementation file.
 */

#include "SysTick.h"

// Systick Initilization Function
void SysTick_Init(void)
{                               // initialization of SysTick timer
    SysTick->CTRL = 0;          // disable SysTick during step
    SysTick->LOAD = 0x00FFFFFF; // max reload value
    SysTick->VAL = 0;           // any write to current clears it
    SysTick->CTRL = 0x00000005; // enable SysTick, 16MHz, No Interrupts
}

// Systick_msdelay, starts a SysTick timer that lasts for the value of msdelay seconds.
void SysTick_msdelay(uint16_t msdelay) // SysTick delay function
{
    SysTick->LOAD = ((msdelay * 16000) - 1); // delay for 1 ms* delay value
    SysTick->VAL = 0;                        // any write to CVR clears it
    while ((SysTick->CTRL & 0x00010000) == 0)
        ; // wait for flag to be SET
}
