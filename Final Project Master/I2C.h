/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             I2C.h
 * Description:      The I2C header file.
 */

#ifndef I2C_H
#define I2C_H

#include "stm32f446xx.h"

#define GPIO_I2C1 GPIOB
#define PIN_SDA1 9
#define PIN_SCL1 8
#define ALT_FUNC 0x02
#define AF4 0x04
#define OPEN_DRAIN 0x01
#define FAST_SPEED 0x02
#define PULLUP 0x01UL
#define RESET 0x8000
#define FREQ_16MHz 0x10
#define I2C1_ENABLE 0x01
#define STD_100kHz 0x050
#define TRISE_MAX 0x11 // Max Rise Time - 1000ns rise / 16MHz
#define ACK_BIT 0x400UL
#define START_GEN 0x100 // Generate Start Bit
#define STOP_GEN 0x200  // Generate Stop Bit
#define TXE_BIT 0x80
#define BTF_BIT 0x04
#define START_BIT 0x01
#define ADDR_BIT 0x02
#define RxNE_BIT 0x40
#define CLEAR_ACK ~(ACK_BIT)
#define I2C1_CLK 0x200000
#define BUSY 0x02
#define SLAVE_ADDR 0x11

// Slave Commands
#define UPDATE_TIME 0xF1
#define UPDATE_TEMP 0xF2
#define CHANGE_EMPH 0xF3
#define PROX_ON 0xF4
#define PROX_OFF 0xF5
#define UPDATE_SPEED 0xF6
#define UPDATE_ODO 0xF7
#define IWDG_KICK 0xE0
#define SET_SEC 0xE1
#define SET_MIN 0xE2
#define SET_HOUR 0xE3
#define SET_AMPM 0xE4
#define UPDATE_SPEED_DISP 0xE5
#define SET_DAY 0xE6
#define SET_MONTH 0xE7
#define SET_YEAR 0xE8
#define SET_DOW 0xE9
#define SET_LIGHT 0xF8
#define USER_MIN 0xDA
#define USER_HOUR 0xDB
#define USER_AMPM 0xDC
#define USER_DAY 0xDD
#define USER_MONTH 0xDE
#define USER_YEAR 0xDF
#define INC_VALUE 0xD0
#define DEC_VALUE 0xD1
#define END_ENTRY 0xD2
#define MENU 0xF9
#define MENU_NEXT 0xFA
#define MENU_PREV 0xFB
#define END_UPDATE 0xFF

void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_Write(uint8_t);
void I2C1_Send_Address(uint8_t);
void I2C1_Clear_Address(void);
void I2C1_Read(uint8_t, uint8_t[], uint8_t);
void I2C1_Device_Write(uint8_t, uint8_t, uint8_t);
void I2C1_Device_Write_STM(uint8_t, uint8_t);
void I2C1_Device_Read(uint8_t, uint8_t, uint8_t[], uint8_t);

#endif
