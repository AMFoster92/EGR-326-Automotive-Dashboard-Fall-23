/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Debounce.h
 * Description:      The button debouncer header file.
 */

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include "Encoder.h"

#define PRESSED 0xF0000000
#define HELD 0xFFFFFFFF
#define RELEASED 0x00000000
#define MASK 0xFFF000FF

extern uint8_t part;

uint8_t DebounceSwitch(GPIO_TypeDef *gpio, uint32_t pin);

#endif
