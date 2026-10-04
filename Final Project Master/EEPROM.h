/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             EEPROM.h
 * Description:      The EEPROM header file.
 */

#include "stm32f446xx.h"
#include <math.h>
/// #include "USART.h"
#include <stdio.h>

#define GPIO_EEPROM
#define PIN_EEPROM
#define CONTROL_CODE 0x0A0
#define EEPROM_ADDR (CONTROL_CODE | (0x01 << 1))
#define EEPROM_W_ADDR (CONTROL_CODE | (EEPROM_ADDR << 1) | 0x00)
#define EEPROM_R_ADDR (CONTROL_CODE | (EEPROM_ADDR << 1) | 0x01)

#define RTC_1_START 0x00
#define RTC_2_START 0x18
#define RTC_3_START 0x30
#define RTC_4_START 0x48
#define RTC_5_START 0x60

#define HOUR_OFFSET 0x00
#define MIN_OFFSET 0x04
#define SEC_OFFSET 0x08
#define YEAR_OFFSET 0x0C
#define MONTH_OFFSET 0x10
#define DAY_OFFSET 0x14
#define TIME_SIZE 3
#define DATE_SIZE 4
#define DEC_SIZE 2

#define OFFSET_1000 0x00
#define OFFSET_100 0x01
#define OFFSET_10 0x02
#define OFFSET_1 0x03
#define OFFSET_DEC 0x04

#define ODOMETER 0x00
#define SPEED 0x0A

#define CUR_STATE 0x05
#define SUB_STATE 0x06
#define MENU_SELECT 0x07

#define PREV_DIST 0x08

#define BRIGHT_LEVEL 0x09

#define SLAVE_FLAGS 0x6E // Bits: EMPHASIS_LCD(0), , X, X, X, X, X,

#define SYSTEM_FLAGS 0x6F // Bits: PROXIMITY(0),

#define RTC_LENGTH 6

/*extern uint8_t RTC_1[RTC_LENGTH];
extern uint8_t RTC_2[RTC_LENGTH];
extern uint8_t RTC_3[RTC_LENGTH];
extern uint8_t RTC_4[RTC_LENGTH];
extern uint8_t RTC_5[RTC_LENGTH];
*/
extern uint8_t sec_dec[DEC_SIZE];
extern uint8_t min_dec[DEC_SIZE];
extern uint8_t hour_dec[TIME_SIZE + 1]; // tens, ones, am_pm, hour_fmt
extern uint8_t day_dec[DEC_SIZE];
extern uint8_t month_dec[DEC_SIZE];
extern uint8_t year_dec[TIME_SIZE + 1];

void EEPROM_Write(uint8_t, uint8_t, uint8_t);

void EEPROM_Read(uint8_t, uint8_t, uint8_t *);

void EEPROM_Write_RTC(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

void EEPROM_Read_RTC(uint8_t);

void EEPROM_Print_Time(void);

void EEPROM_Print_Date(void);

void EEPROM_Print_Date_Time(uint8_t);

void EEPROM_Convert_Odo(double val, uint8_t *thousands, uint8_t *hundreds, uint8_t *tens, uint8_t *ones, uint8_t *decimal_upper, uint8_t *decimal_lower);
void EEPROM_Convert_Speed(double val, uint8_t *hundreds, uint8_t *tens, uint8_t *ones, uint8_t *decimal_upper);

extern void I2C1_Device_Write(uint8_t, uint8_t, uint8_t);
extern void I2C1_Device_Read(uint8_t, uint8_t, uint8_t[], uint8_t);

extern void Min_Sec_Day_To_Dec(uint8_t, uint8_t[], uint8_t);
extern void Hour_To_Dec(uint8_t);
extern void Month_To_Dec(uint8_t);
extern void Year_To_Dec(uint8_t, uint8_t);
extern void Dow_To_Dec(uint8_t);

extern void SysTick_msdelay(uint16_t msdelay);
