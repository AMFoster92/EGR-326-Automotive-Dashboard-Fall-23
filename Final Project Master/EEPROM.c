/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             EEPROM.c
 * Description:      The EEPROM definition file.
 */

#include "EEPROM.h"

// Store floating point value of odometer into six 8-bit unsigned integers
void EEPROM_Convert_Odo(double val, uint8_t *thousands, uint8_t *hundreds, uint8_t *tens, uint8_t *ones, uint8_t *decimal_upper, uint8_t *decimal_lower)
{
	uint32_t whole = 0;

	whole = (uint32_t)(floor(val));
	*thousands = (uint8_t)((whole / 1000));
	*hundreds = (uint8_t)((whole % 1000) / 100);
	*tens = (uint8_t)((whole % 100) / 10);
	*ones = (uint8_t)(whole % 10);
	*decimal_upper = (uint8_t)(floor((val * 100))) % 100;
	*decimal_lower = (uint8_t)(floor((val * 10000))) % 100;
}

// Store floating point value of speed into four 8-bit unsigned integers
void EEPROM_Convert_Speed(double val, uint8_t *hundreds, uint8_t *tens, uint8_t *ones, uint8_t *decimal_upper)
{
	uint32_t whole = 0;

	whole = (uint32_t)(floor(val));
	*hundreds = (uint8_t)((whole % 1000) / 100);
	*tens = (uint8_t)((whole % 100) / 10);
	*ones = (uint8_t)(whole % 10);
	*decimal_upper = (uint8_t)(floor((val * 10))) % 100;
}

// Write data to EEPROM
void EEPROM_Write(uint8_t addr, uint8_t offset, uint8_t data)
{
	I2C1_Device_Write(EEPROM_ADDR, addr + offset, data);
	SysTick_msdelay(10);
}

// Read data from EEPROM
void EEPROM_Read(uint8_t addr, uint8_t offset, uint8_t *buffer)
{
	I2C1_Device_Read(EEPROM_ADDR, addr + offset, buffer, 1);
	SysTick_msdelay(10);
}

void EEPROM_Write_RTC(uint8_t clock, uint8_t hour, uint8_t min, uint8_t sec,
					  uint8_t year, uint8_t month, uint8_t day)
{
	uint8_t clock_addr = 0xFF;

	switch (clock)
	{
	case 1:
		clock_addr = RTC_1_START;
		break;
	case 2:
		clock_addr = RTC_2_START;
		break;
	case 3:
		clock_addr = RTC_3_START;
		break;
	case 4:
		clock_addr = RTC_4_START;
		break;
	case 5:
		clock_addr = RTC_5_START;
		break;
	default:
		clock_addr = 0xFF;
		break;
	}

	if (clock_addr != 0xFF)
	{
		EEPROM_Write(clock_addr, HOUR_OFFSET, hour);
		EEPROM_Write(clock_addr, MIN_OFFSET, min);
		EEPROM_Write(clock_addr, SEC_OFFSET, sec);
		EEPROM_Write(clock_addr, YEAR_OFFSET, year);
		EEPROM_Write(clock_addr, MONTH_OFFSET, month);
		EEPROM_Write(clock_addr, DAY_OFFSET, day);
	}
}

void EEPROM_Read_RTC(uint8_t clock)
{
	uint8_t *hour = NULL;
	uint8_t *min = NULL;
	uint8_t *sec = NULL;
	uint8_t *year = NULL;
	uint8_t *month = NULL;
	uint8_t *day = NULL;

	uint8_t clock_addr = 0xFF;

	/*switch(clock)
	{
		case 1:
			hour = &RTC_1[0];
			min = &RTC_1[1];
			sec = &RTC_1[2];
			year = &RTC_1[3];
			month = &RTC_1[4];
			day = &RTC_1[5];
			break;
		case 2:
			hour = &RTC_2[0];
			min = &RTC_2[1];
			sec = &RTC_2[2];
			year = &RTC_2[3];
			month = &RTC_2[4];
			day = &RTC_2[5];
			break;
		case 3:
			hour = &RTC_3[0];
			min = &RTC_3[1];
			sec = &RTC_3[2];
			year = &RTC_3[3];
			month = &RTC_3[4];
			day = &RTC_3[5];
			break;
		case 4:
			hour = &RTC_4[0];
			min = &RTC_4[1];
			sec = &RTC_4[2];
			year = &RTC_4[3];
			month = &RTC_4[4];
			day = &RTC_4[5];
			break;
		case 5:
			hour = &RTC_5[0];
			min = &RTC_5[1];
			sec = &RTC_5[2];
			year = &RTC_5[3];
			month = &RTC_5[4];
			day = &RTC_5[5];
			break;
		default:
			hour = NULL;
	  min = NULL;
	  sec = NULL;
	  year = NULL;
	  month = NULL;
	  day = NULL;
	}



	switch(clock)
	{
		case 1:
			clock_addr = RTC_1_START;
			break;
		case 2:
			clock_addr = RTC_2_START;
			break;
		case 3:
			clock_addr = RTC_3_START;
			break;
		case 4:
			clock_addr = RTC_4_START;
			break;
		case 5:
			clock_addr = RTC_5_START;
			break;
		default:
			clock_addr = 0xFF;
			break;
	}
	*/
	if (hour != NULL && min != NULL && sec != NULL && year != NULL && month != NULL && day != NULL)
	{
		EEPROM_Read(clock_addr, HOUR_OFFSET, hour);
		EEPROM_Read(clock_addr, MIN_OFFSET, min);
		EEPROM_Read(clock_addr, SEC_OFFSET, sec);
		EEPROM_Read(clock_addr, YEAR_OFFSET, year);
		EEPROM_Read(clock_addr, MONTH_OFFSET, month);
		EEPROM_Read(clock_addr, DAY_OFFSET, day);
	}
}

void EEPROM_Print_Time(void)
{
	/*USART2_Write_Str(hour_dec, DEC_SIZE);
USART2_Write_Char(':');
USART2_Write_Str(min_dec, DEC_SIZE);
USART2_Write_Char(':');
USART2_Write_Str(sec_dec, DEC_SIZE);
USART2_Write_Char(' ');

if(hour_dec[3])
{
	if(hour_dec[2])
	{
		USART2_Write_Char('P');
	}
	else
	{
		USART2_Write_Char('A');
	}

	USART2_Write_Char('M');
}

USART2_Write_Char(0x0A);
USART2_Write_Char(0x0D);
}

void EEPROM_Print_Date(void)
{
USART2_Write_Char(' ');
USART2_Write_Str(month_dec, DEC_SIZE);
USART2_Write_Char('-');
USART2_Write_Str(day_dec, DEC_SIZE);
USART2_Write_Char('-');
USART2_Write_Str(year_dec, TIME_SIZE+1);
USART2_Write_Char(0x0A);
USART2_Write_Char(0x0D);
}

void EEPROM_Print_Date_Time(uint8_t clock)
{
uint8_t *hour = NULL;
uint8_t *min = NULL;
uint8_t *sec = NULL;
uint8_t *year = NULL;
uint8_t *month = NULL;
uint8_t *day = NULL;


switch(clock)
{
	case 1:
		hour = &RTC_1[0];
		min = &RTC_1[1];
		sec = &RTC_1[2];
		year = &RTC_1[3];
		month = &RTC_1[4];
		day = &RTC_1[5];
		break;
	case 2:
		hour = &RTC_2[0];
		min = &RTC_2[1];
		sec = &RTC_2[2];
		year = &RTC_2[3];
		month = &RTC_2[4];
		day = &RTC_2[5];
		break;
	case 3:
		hour = &RTC_3[0];
		min = &RTC_3[1];
		sec = &RTC_3[2];
		year = &RTC_3[3];
		month = &RTC_3[4];
		day = &RTC_3[5];
		break;
	case 4:
		hour = &RTC_4[0];
		min = &RTC_4[1];
		sec = &RTC_4[2];
		year = &RTC_4[3];
		month = &RTC_4[4];
		day = &RTC_4[5];
		break;
	case 5:
		hour = &RTC_5[0];
		min = &RTC_5[1];
		sec = &RTC_5[2];
		year = &RTC_5[3];
		month = &RTC_5[4];
		day = &RTC_5[5];
		break;
	default:
		hour = NULL;
  min = NULL;
  sec = NULL;
  year = NULL;
  month = NULL;
  day = NULL;
}

Min_Sec_Day_To_Dec(*sec, sec_dec, DEC_SIZE);
Min_Sec_Day_To_Dec(*min, min_dec, DEC_SIZE);
Hour_To_Dec(*hour);

Min_Sec_Day_To_Dec(*day, day_dec, DEC_SIZE);
Month_To_Dec(*month);
Year_To_Dec(*year, *month);

EEPROM_Print_Date();
EEPROM_Print_Time();
*/
}
