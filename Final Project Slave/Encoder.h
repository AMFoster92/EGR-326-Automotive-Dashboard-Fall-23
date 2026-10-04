/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             Encoder.h
 * Description:      The rotary encoder header file.
 */

#include "stm32f446xx.h"
#include <stdio.h>
#define GPIO_ENCODER GPIOA
#define PIN_CLK 0
#define PIN_DT 1
#define PIN_SW 4
#define PIN_CWLED 8
#define PIN_CCWLED 6

static uint32_t cur;
static uint32_t prev;

extern uint8_t cnt;
extern uint8_t encoder_ccw;
extern uint8_t encoder_cw;
extern uint8_t switch_flag;

void Encoder_Init(void);
void EXTI0_IRQHandler(void); // CLK interrupt for rotation
void EXTI4_IRQHandler(void); // SW interrupt for switch

extern uint8_t DebounceEncoder(GPIO_TypeDef *, uint32_t);
