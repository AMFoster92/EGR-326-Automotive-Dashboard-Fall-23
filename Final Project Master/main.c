/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             main.c
 * Description:      The entry point and main loop of the system for the Master Controller.
 */

#include "GPIOInit.h"
#include "Button.h"
#include "Debounce.h"
#include "Encoder.h"
#include "GPIOInit.h"
#include "HallEffect.h"
#include "I2C.h"
#include "RTC.h"
#include "Sonar.h"
#include "SysTick.h"
#include "Timer.h"
#include "Watchdog.h"
#include "UnipolarStepperMotor.h"
#include "EEPROM.h"
#include <math.h>
#include <stdio.h>

#define LCD_MENU 0x00
#define LCD_TIME 0x01
#define LCD_SEC 0x02
#define LCD_MIN 0x03
#define LCD_HOUR 0x04
#define LCD_AMPM 0x05
#define LCD_DATE 0x06
#define LCD_DAY 0x07
#define LCD_MONTH 0x08
#define LCD_YEAR 0x09
#define LCD_DOW 0x0A
#define LCD_LIGHT 0x0B
#define LCD_SET_LIGHT 0x0C
#define LCD_DISP 0xFF

#define HOUR_FORMAT 12

#define MENU_LENGTH 4
#define MENU_GAMES 0x00
#define MENU_SET_TIME 0x01
#define MENU_SET_DATE 0x02
#define MENU_SETTINGS 0x03

#define SET_AUTO_BRIGHT 0x00
#define SET_MANUAL_BRIGHT 0x01

#define DIAMETER 6 * 12.00			  // inches
#define CIRCUMFERENCE DIAMETER * 3.14 // inches
#define TO_MPH 63360

#define PA5_LED_INIT GPIOA->MODER |= (0x01 << 10)
#define PA5_LED_ON GPIOA->ODR |= (1UL << 5)
#define PA5_LED_OFF GPIOA->ODR &= ~(1UL << 5)
#define PA5_LED_CHECK (GPIOA->ODR & (1UL))

#define SONAR_INTERVAL 1000
#define DELAY 250

uint8_t cnt = 0;
uint8_t encoder_ccw = 0;
uint8_t encoder_cw = 0;
uint8_t switch_flag = 0;

uint8_t time[TIME_SIZE] = {0};

uint8_t date[DATE_SIZE] = {0};
uint8_t date_time[RTC_SIZE] = {0};
uint8_t sec_dec[DEC_SIZE] = {0};
uint8_t min_dec[DEC_SIZE] = {0};
uint8_t hour_dec[TIME_SIZE + 1] = {0}; // tens, ones, am_pm, hour_fmt
uint8_t day_dec[DEC_SIZE] = {0};
uint8_t month_dec[DEC_SIZE] = {0};
uint8_t year_dec[TIME_SIZE + 1] = {0};
uint8_t dow_dec = 0;
uint8_t temp_upper = 0;
uint8_t temp_lower = 0;
uint8_t temp_C[TEMP_C_SIZE] = {0}; // sign, tens, ones, tenths, hundredths
uint8_t temp_F[TEMP_F_SIZE] = {0}; // sign, hundreds, tens, ones, tenths, hundredths
uint8_t F_not_C = 0;

uint8_t hall = 0;
uint8_t speed_measure = 0;
uint8_t check_rtc = 0;

double distance = 0.0;

uint8_t menu_timeout = 0;
uint8_t speed_timeout = 0;

uint8_t unipolar_motor_state = 0;
uint8_t delay = 0;

uint8_t sonar_timeout = 0;
uint8_t kick_dog = 0;
uint32_t speed_start = 0;
uint32_t speed_end = 0;

uint32_t btn_time = 0;
uint8_t btn_flag = 0;

uint8_t stopped = 0;

int main(void)
{
	// SONAR VARIABLES
	uint32_t start = 0;		  // Stores start time of echo pulse in microseconds
	uint32_t end = 0;		  // Stores end time of echo pulse in microseconds
	uint32_t pulse_width = 0; // Stores width of echo pulse in microseconds
	double prev_dist = 0;	  // STORE IN EEPROM, Stores previously calculated distance for error checking
	uint8_t sampled = 0;
	uint8_t proximity = 0; // STORE IN EEPROM SYSTEM FLAGS, BIT 0
	uint8_t prox_sent = 0;
	uint32_t sonar_time = 0;
	uint32_t sonar_last = 0;

	// ODOMETER
	double odometer = 0.0; // STORE IN EEPROM as 5 bytes: THOUSANDS, HUNDREDS, TENS, ONES, DECIMAL
	// uint32_t odo_whole = 0;
	uint8_t odo_thousands = 0;
	uint8_t odo_hundreds = 0;
	uint8_t odo_tens = 0;
	uint8_t odo_ones = 0;
	uint8_t odo_decimal_upper = 0;
	uint8_t odo_decimal_lower = 0;

	// CONTROL STATE VARIABLES
	uint8_t cur_state = LCD_DISP;  // STORE IN EEPROM
	uint8_t sub_state = cur_state; // STORE IN EEPROM
	uint8_t menu_select = 0;	   // STORE IN EEPROM

	// TIME & DATE VARIABLES
	uint8_t temp_time[TIME_SIZE] = {0};
	// uint8_t temp_sec = 0;
	uint8_t temp_min = 0;
	uint8_t temp_hour = 0;
	uint8_t temp_ampm = 0;
	uint8_t temp_date[DATE_SIZE] = {0};
	uint8_t temp_day = 0;
	uint8_t temp_month = 0;
	uint8_t temp_year = 0;
	// uint8_t temp_dow = 0;
	// uint8_t time_changed = 0;
	// uint8_t cur_min = 0;

	// FOR USER INPUT
	uint8_t user_min = 0;
	uint8_t user_hour = 0;
	uint8_t user_ampm = 0;
	uint8_t user_day = 0;
	uint8_t user_month = 0;
	uint8_t user_year = 0;

	// SPEEDOMETER
	double cur_speed = 0; // STORE IN EEPROM as 4 bytes: HUNDREDS, TENS, ONES, TENTHS
	double prev_speed = cur_speed;
	uint8_t speed_first_read = 0;
	uint32_t speed_time = 0;
	double speed_rpm = 0;
	uint8_t speed_valA = 0;
	uint8_t speed_valB = 0;
	double speed_validate = -1;
	uint8_t speed_val_pos = 0;
	double speed_test = 0;
	uint8_t speed_hundreds = 0;
	uint8_t speed_tens = 0;
	uint8_t speed_ones = 0;
	uint8_t speed_tenths = 0;
	uint8_t speed_sampled = 0;

	// SETTINGS
	uint8_t system_flags = 0;
	uint8_t slave_flags = 0;
	uint8_t flag_data = 0;

	// SLAVE_STATES
	uint8_t emphasis_LCD = 0; // STORE IN EEPROM SLAVE_FLAGS, BIT 0

	// EEPROM VARIABLES

	uint8_t time_set = 0;
	uint8_t rtc[10] = {00, 13, 2, 1, 1, 6, 10, 1, 23, 6}; // ss, mm, hh, 12 hour_fmt, am_pm, day, month, century, year, dow

	uint32_t unipolar_time = 0;
	uint32_t unipolar_step = 0;

	menu_timeout = 0;
	check_rtc = 0;
	kick_dog = 0;

	__disable_irq();
	GPIO_Init();
	SysTick_Init();
	Timer_Init();
	I2C1_Init();
	Encoder_Init();
	Hall_Init();
	Sonar_Init();
	UnipolarStepper_Init();
	IWDG_Init();
	Button_Init();

	SysTick_msdelay(5000);
	SysTick_msdelay(5000);
	// BipolarStepper_Init();

	PA5_LED_INIT;
	PA5_LED_ON;

	// initialize temp time variables
	if (!time_set)
	{
		Set_Date_Time(rtc[0], rtc[1], rtc[2], rtc[3], rtc[4], rtc[5], rtc[6], rtc[7], rtc[8], rtc[9]);
		time_set = 1;
	}
	SysTick_msdelay(500);
	Read_Date_Time();
	Read_Temp();

	// temp_sec = time[TIME_SIZE-TIME_SIZE];
	temp_min = time[TIME_SIZE - 2];
	temp_hour = time[TIME_SIZE - 1];
	temp_ampm = hour_dec[2];

	temp_day = date[DATE_SIZE - DATE_SIZE];
	temp_month = date[DATE_SIZE - 3];
	temp_year = date[DATE_SIZE - 2];
	// temp_dow = date[DATE_SIZE-1];

	SysTick_msdelay(10);

	// INITIALIZE SYSTEM FLAGS
	// EEPROM_Write(SYSTEM_FLAGS, 0x00, 0);

	EEPROM_Read(SYSTEM_FLAGS, 0x00, &system_flags);

	system_flags = flag_data;

	if (system_flags & 1)
	{
		proximity = 1;
		prox_sent = 1;
		I2C1_Device_Write_STM(SLAVE_ADDR << 1, PROX_ON);
	}

	// INITIALIZE SLAVE FLAGS
	// EEPROM_Write(SLAVE_FLAGS, 0x00, 0);

	EEPROM_Read(SLAVE_FLAGS, 0x00, &slave_flags);

	if (slave_flags & 1)
	{
		emphasis_LCD = 1;
		I2C1_Device_Write_STM(SLAVE_ADDR << 1, CHANGE_EMPH);
		// I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
	}

	// INITIALIZE SONAR DISTANCE
	// EEPROM_Read(PREV_DIST, 0x00, (uint8_t*)(&prev_dist));

	// UPDATE LCD TIME VARIABLES
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_HOUR);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[0]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MIN);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[0]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_DAY);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[0]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_AMPM);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[2]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MONTH);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[0]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_YEAR);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[0]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[2]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[3]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_TEMP);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, temp_F[0]);
	// I2C1_Device_Write_STM(SLAVE_ADDR << 1, temp_F[1]);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, temp_F[2] + 0x30);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, temp_F[3] + 0x30);
	printf("TIME\n");

	PA5_LED_OFF;
	// SysTick_msdelay(10);

	// initialize speedometer
	cur_speed = 0.0;

	EEPROM_Convert_Speed(cur_speed, &speed_hundreds, &speed_tens, &speed_ones, &speed_tenths);

	/*
	EEPROM_Write(SPEED, OFFSET_100, speed_hundreds);
	EEPROM_Write(SPEED, OFFSET_10, speed_tens);
	EEPROM_Write(SPEED, OFFSET_1, speed_ones);
	EEPROM_Write(SPEED, OFFSET_DEC, speed_tenths);
	*/

	EEPROM_Read(SPEED, OFFSET_100, &speed_hundreds);
	EEPROM_Read(SPEED, OFFSET_10, &speed_tens);
	EEPROM_Read(SPEED, OFFSET_1, &speed_ones);
	EEPROM_Read(SPEED, OFFSET_DEC, &speed_tenths);

	cur_speed = (speed_hundreds) * 100 + (speed_tens * 10) + speed_ones + (speed_tenths / 10);
	printf("SPEED\n");
	printf("%d %d %d %d %f\n", speed_hundreds, speed_tens, speed_ones, speed_tenths, cur_speed);

	// initialize speedometer
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_SPEED);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_ones + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_tens + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_hundreds + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_tenths + 1);

	// read/initialize odometer
	odometer = 0.0;

	EEPROM_Convert_Odo(odometer, &odo_thousands, &odo_hundreds, &odo_tens, &odo_ones, &odo_decimal_upper, &odo_decimal_lower);
	/*
	EEPROM_Write(ODOMETER, OFFSET_1000, odo_thousands);
	EEPROM_Write(ODOMETER, OFFSET_100, odo_hundreds);
	EEPROM_Write(ODOMETER, OFFSET_10, odo_tens);
	EEPROM_Write(ODOMETER, OFFSET_1, odo_ones);
	EEPROM_Write(ODOMETER, OFFSET_DEC, odo_decimal_upper);
	*/

	EEPROM_Read(ODOMETER, OFFSET_1000, &odo_thousands);
	EEPROM_Read(ODOMETER, OFFSET_100, &odo_hundreds);
	EEPROM_Read(ODOMETER, OFFSET_10, &odo_tens);
	EEPROM_Read(ODOMETER, OFFSET_1, &odo_ones);
	EEPROM_Read(ODOMETER, OFFSET_DEC, &odo_decimal_upper);

	odometer = (odo_thousands * 1000) + (odo_hundreds * 100) +
			   (odo_tens * 10) + odo_ones + (odo_decimal_upper / 100.0);

	I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_ODO);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_ones + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_tens + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_hundreds + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_thousands + 1);
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_decimal_upper + 1);
	// I2C1_Device_Write_STM(SLAVE_ADDR<<1, odo_decimal_lower);
	printf("ODO\n");
	// initialize display options from eeprom

	// UPDATE LCD DISPLAY
	I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
	printf("UPDATE LCD\n");

	SysTick_msdelay(10);
	sonar_timeout = 0;
	TIM8->CR1 |= 0x01;

	__enable_irq();

	while (1)
	{
		if (!kick_dog)
		{
			IWDG->KR = IWDG_RELOAD;

			if (btn_flag)
			{
				if ((TIM3->CNT - btn_time) > 250)
				{
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, IWDG_KICK);
					IWDG->KR = IWDG_RELOAD;
					IWDG_Kick();
					btn_flag = 0;
				}
			}

			// TEST MOTOR
			if (unipolar_time > TIM3->CNT)
			{
				unipolar_step = unipolar_time - TIM3->CNT;
			}
			else
			{
				unipolar_step = TIM3->CNT - unipolar_time;
			}

			if (unipolar_step > DELAY)
			{
				unipolar_time = TIM3->CNT;
				if (unipolar_motor_state < 0x04)
				{
					UnipolarStep_Increment();
					unipolar_motor_state++;
				}
				else
				{
					unipolar_motor_state = 0x00;
					UnipolarStep_Increment();
					unipolar_motor_state++;
				}
			}

			if (menu_timeout)
			{
				if (!PA5_LED_CHECK)
				{
					PA5_LED_OFF;
				}
				else
				{
					PA5_LED_ON;
				}

				cur_state = LCD_DISP;
				sub_state = LCD_DISP;
				menu_timeout = 0;
				TIM7->CR1 &= ~(0x01UL);

				Read_Date_Time();
				SysTick_msdelay(10);

				I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_HOUR);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[0]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[1]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MIN);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[0]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[1]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_DAY);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[0]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[1]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_AMPM);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[2]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MONTH);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[0]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[1]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_YEAR);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[0]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[1]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[2]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[3]);
				I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
			}

			// SONAR
			if (sonar_last > TIM3->CNT)
			{
				sonar_time = sonar_last - TIM3->CNT;
			}
			else
			{
				sonar_time = TIM3->CNT - sonar_last;
			}

			if (sonar_time > SONAR_INTERVAL)
			{
				TIM2->CNT = 0;
				TIM2->CR1 |= 1;
				TIM5->CR1 |= 1;

				/*
				if(!PA5_LED_CHECK)
				{
					PA5_LED_OFF;
				}
				else
				{
					PA5_LED_ON;
				}*/

				while (!(TIM2->SR & 4) && !sonar_timeout)
				{
				} // Wait for rising edge
				// if(TIM2->SR & 4)
				//{
				// if(!cur_edge)
				{
					start = TIM2->CCR2; // Store start time
					TIM2->SR = 0;		// Clear flags
					TIM2->CCER = 0x30;	// Set to detect falling edge
										// cur_edge = 1;
				}

				while (!(TIM2->SR & 4) && !sonar_timeout)
				{
				} // Wait for falling edge
				// else if(cur_edge)
				{
					end = TIM2->CCR2;  // Store end time
					TIM2->SR = 0;	   // Clear flags
					TIM2->CCER = 0x10; // Set to detect rising edge
					// cur_edge = 0;
					sampled = 1;
				}

				TIM5->CR1 &= ~1UL;
				TIM2->CR1 &= ~1UL;

				if (sonar_timeout)
				{
					printf("TIMEOUT\n");
					SysTick_msdelay(10);
					sonar_timeout = 0;
					sampled = 0;
				}
				//}

				if (sampled)
				{
					sampled = 0;

					pulse_width = (end - start);			  // Calc pulse width
					distance = (double)(pulse_width) / TO_IN; // Calc distance in inches

					if (floor(distance) != floor(prev_dist))
					{
						// If erroneous reading, set distance to previous value
						if ((distance > 157.48) || (distance < 0.787402))
						{
							distance = prev_dist;
						}
						// Otherwise update previous value to current distance
						else
						{
							prev_dist = distance;
						}

						if (distance < 15.0)
						{
							proximity = 1;

							system_flags |= proximity << 1;

							EEPROM_Write(SYSTEM_FLAGS, 0x00, system_flags);
						}
						else
						{
							proximity = 0;
						}

						printf("%f\n", distance);
						if (proximity && !prox_sent)
						{
							prox_sent = 1;
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, PROX_ON);
						}
						else if (!proximity && prox_sent)
						{
							prox_sent = 0;
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, PROX_OFF);
						}

						EEPROM_Write(PREV_DIST, 0x00, (uint8_t)(prev_dist));

						sonar_last = TIM3->CNT;
					}
				}
			}

			// HALL
			{
				if (hall)
				{
					TIM8->CNT = 0;
					TIM8->CR1 |= 0x01;
					hall = 0;

					speed_end = speed_start;
					speed_start = TIM3->CCR1;

					if (speed_start > speed_end)
					{
						speed_time = speed_start - speed_end;
					}
					else
					{
						speed_time = (0xFFFF - speed_end) + speed_start;
					}
					if (speed_time > 10)
					{
						// if(TIM3->SR & 2)

						/*
						if(!PA5_LED_CHECK)
						{
							PA5_LED_OFF;
						}
						else
						{
							PA5_LED_ON;
						}
						*/

						// convert time to rpm to mph
						speed_rpm = ((60.0 / speed_time) * 1000.0);

						speed_test = ((speed_rpm * CIRCUMFERENCE * 60.0) / TO_MPH);

						if (speed_test < 0.1)
						{
							speed_valA = (uint8_t)(speed_test * 100);
						}
						else if (speed_test < 1)
						{
							speed_valA = (uint8_t)(speed_test * 10);
						}
						else if (speed_test > 1)
						{
							speed_valA = (uint8_t)(speed_test);
						}

						if (speed_validate < 0)
						{
							speed_val_pos++;
							speed_valB = speed_valA;
							speed_validate = speed_test;
						}
						else if (speed_val_pos > 0 && (speed_valA == speed_valB))
						{
							speed_val_pos = 0;
							prev_speed = cur_speed;
							cur_speed = speed_test;
							speed_validate = -1;
							speed_sampled = 1;

							if (((uint16_t)(cur_speed * 10) != (uint16_t)(prev_speed * 10)) && speed_first_read)
							{

								EEPROM_Convert_Speed(cur_speed, &speed_hundreds, &speed_tens, &speed_ones, &speed_tenths);

								// write speed to eeprom

								EEPROM_Write(SPEED, OFFSET_100, speed_hundreds);
								EEPROM_Write(SPEED, OFFSET_10, speed_tens);
								EEPROM_Write(SPEED, OFFSET_1, speed_ones);
								EEPROM_Write(SPEED, OFFSET_DEC, speed_tenths);

								// initialize speedometer
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_SPEED);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_ones + 1);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_tens + 1);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_hundreds + 1);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, speed_tenths + 1);

								prev_speed = cur_speed;
							}
						}
						else if (speed_valA == speed_valB)
						{
							speed_val_pos++;
						}
						else
						{
							speed_validate = -1;
							speed_val_pos = 0;
						}
						printf("%d\t%d\n", speed_valA, speed_valB);

						if (speed_first_read)
						{
							odometer += ((CIRCUMFERENCE / 2) / 63360);

							EEPROM_Convert_Odo(odometer, &odo_thousands, &odo_hundreds, &odo_tens, &odo_ones, &odo_decimal_upper, &odo_decimal_lower);

							EEPROM_Write(ODOMETER, OFFSET_1000, odo_thousands);
							EEPROM_Write(ODOMETER, OFFSET_100, odo_hundreds);
							EEPROM_Write(ODOMETER, OFFSET_10, odo_tens);
							EEPROM_Write(ODOMETER, OFFSET_1, odo_ones);
							EEPROM_Write(ODOMETER, OFFSET_DEC, odo_decimal_upper);

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_ODO);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_ones + 1);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_tens + 1);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_hundreds + 1);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_thousands + 1);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, odo_decimal_upper + 1);
						}

						speed_measure = 0;

						// update slave odometer
						if (speed_sampled)
						{
							printf("%.02f\t%.02f\t%.02f\n\n", speed_rpm, cur_speed, odometer);
							speed_sampled = 0;
						}
						speed_first_read = 1;
					}
				}

				else if (stopped)
				{
					cur_speed = 0;
					EEPROM_Convert_Speed(cur_speed, &speed_hundreds, &speed_tens, &speed_ones, &speed_tenths);

					// write speed to eeprom

					EEPROM_Write(SPEED, OFFSET_100, speed_hundreds);
					EEPROM_Write(SPEED, OFFSET_10, speed_tens);
					EEPROM_Write(SPEED, OFFSET_1, speed_ones);
					EEPROM_Write(SPEED, OFFSET_DEC, speed_tenths);

					// write to lcd
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, UPDATE_SPEED);
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, 1);
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, 1);
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, 1);
					I2C1_Device_Write_STM(SLAVE_ADDR << 1, 1);

					TIM8->CR1 &= ~(0x01UL);
					stopped = 0;
					speed_first_read = 0;
				}
			}

			// RTC
			{
				// check if minute passed
				if (check_rtc)
				{
					if (!PA5_LED_CHECK)
					{
						PA5_LED_OFF;
					}
					else
					{
						PA5_LED_ON;
					}
					if (cur_state == LCD_DISP)
					{

						// read RTC
						Read_Date_Time();

						// if new minute update slave
						if (temp_min != time[TIME_SIZE - 2])
						{
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MIN);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[1]);
							temp_min = time[TIME_SIZE - 2];

							if (temp_hour != time[TIME_SIZE - 1])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_HOUR);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[0]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[1]);
								temp_hour = time[TIME_SIZE - 1];
							}

							if (temp_ampm != hour_dec[2])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[2]);
								temp_ampm = hour_dec[2];
							}

							if (temp_day != date[DATE_SIZE - DATE_SIZE])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_DAY);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[0]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[1]);
								temp_day = date[DATE_SIZE - DATE_SIZE];
							}

							if (temp_month != date[DATE_SIZE - 3])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MONTH);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[0]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[1]);
								temp_month = date[DATE_SIZE - 3];
							}

							if (temp_year != date[DATE_SIZE - 2])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_YEAR);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[0]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[1]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[2]);
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[3]);
								temp_year = date[DATE_SIZE - 2];
							}

							/*if(temp_dow != date[DATE_SIZE-1])
							{
								I2C1_Device_Write_STM(SLAVE_ADDR << 1, dow_dec);
							}*/

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);

							TIM4->ARR = 60000 - 1;
							TIM4->CNT = 0;
							TIM4->EGR = 1;
							TIM4->SR = 0;
						}
						// otherwise check again after 1 second
						else
						{
							TIM4->ARR = 1000 - 1;
							TIM4->CNT = 0;
							TIM4->EGR = 1;
							TIM4->SR = 0;
						}
						check_rtc = 0;
					}
				}
			}

			// ENCODER
			{
				if (switch_flag)
				{
					if (!PA5_LED_CHECK)
					{
						PA5_LED_OFF;
					}
					else
					{
						PA5_LED_ON;
					}
					switch_flag = 0;
					TIM7->CNT = 0;

					while (!DebounceSwitch(GPIO_ENCODER, PIN_SW))
					{
						switch_flag = 0;
					}

					if (cur_state == LCD_DISP)
					{
						cur_state = LCD_MENU;
						I2C1_Device_Write_STM(SLAVE_ADDR << 1, MENU);
						menu_select = 0;
						TIM7->CR1 |= 0x01;
					}

					else if (cur_state == LCD_MENU)
					{
						if (menu_select == MENU_SET_TIME)
						{
							cur_state = LCD_TIME;
							// sub_state = LCD_SEC;
							sub_state = LCD_MIN;

							user_min = ((min_dec[0] - 0x30) * 10) + (min_dec[1] - 0x30);
							user_hour = ((hour_dec[0] - 0x30) * 10) + (hour_dec[1] - 0x30);
							user_ampm = hour_dec[2];

							for (int i = 0; i < TIME_SIZE; i++)
							{
								temp_time[i] = time[i];

								printf("%x - %x\n", temp_time[i], time[i]);
							}

							temp_time[0] = Min_Sec_Day_Year_To_BCD(0);

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_MIN);

							// set current time values to temp variables
						}
						else if (menu_select == MENU_SET_DATE)
						{
							cur_state = LCD_DATE;
							// sub_state = LCD_SEC;
							sub_state = LCD_YEAR;

							user_day = ((day_dec[0] - 0x30) * 10) + (day_dec[1] - 0x30);
							user_month = ((month_dec[0] - 0x30) * 10) + (month_dec[1] - 0x30);
							user_year = ((year_dec[2] - 0x30) * 10) + (year_dec[0] - 0x30);

							for (int i = 0; i < DATE_SIZE; i++)
							{
								temp_date[i] = date[i];

								printf("%x - %x\n", temp_date[i], date[i]);
							}

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_YEAR);

							// set current time values to temp variables
						}

						/*else if(menu_select == MENU_SET_BRIGHTNESS)
						{
							cur_state = LCD_LIGHT;
							sub_state = SET_AUTO_BRIGHT;
						}*/
					}

					else if (cur_state == LCD_TIME)
					{
						/*if(sub_state == LCD_SEC)
						{
							sub_state = LCD_MIN;
						}
						else*/
						if (sub_state == LCD_MIN)
						{
							sub_state = LCD_HOUR;

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_HOUR);
						}
						else if (sub_state == LCD_HOUR)
						{
							// sub_state = LCD_AMPM;
							// cur_state = LCD_DISP;
							// cur_state = LCD_DISP;
							sub_state = LCD_AMPM;

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_AMPM);
							// I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
						}
						else if (sub_state == LCD_AMPM)
						{
							cur_state = LCD_DISP;
							sub_state = LCD_DISP;
							TIM1->CR1 &= ~(0x01UL);

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
							Write_Time(temp_time[TIME_SIZE - TIME_SIZE], temp_time[TIME_SIZE - 2], temp_time[TIME_SIZE - 1]);
							Read_Date_Time();
							// temp_sec = time[TIME_SIZE-TIME_SIZE];
							temp_min = time[TIME_SIZE - 2];
							temp_hour = time[TIME_SIZE - 1];
							temp_ampm = hour_dec[2];
						}
					}

					else if (cur_state == LCD_DATE)
					{
						if (sub_state == LCD_YEAR)
						{
							sub_state = LCD_MONTH;
							TIM7->CNT = 0;

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_MONTH);
						}
						else if (sub_state == LCD_MONTH)
						{
							sub_state = LCD_DAY;
							TIM7->CNT = 0;

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, USER_DAY);
						}
						else if (sub_state == LCD_DAY)
						{
							// sub_state = LCD_DOW;
							cur_state = LCD_DISP;
							sub_state = LCD_DISP;
							TIM1->CR1 &= ~(0x01UL);

							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_ENTRY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
							Write_Date(temp_date[DATE_SIZE - DATE_SIZE], temp_date[DATE_SIZE - 3], temp_date[DATE_SIZE - 2], temp_date[DATE_SIZE - 1]);
							Read_Date_Time();
							temp_day = date[DATE_SIZE - DATE_SIZE];
							temp_month = date[DATE_SIZE - 3];
							temp_year = date[DATE_SIZE - 2];
							// temp_dow = date[DATE_SIZE-1];
						}
						/*else if(sub_state == LCD_DOW)
						{
							cur_state = LCD_MENU;
							sub_state = LCD_MENU;

							//set RTC to updated time/date
						}
						*/
					}
					/*
					else if(cur_state == LCD_LIGHT)
					{
						if(sub_state == SET_AUTO_BRIGHT)
						{
							brightness_override = 1;
						}
						else if(sub_state == SET_MANUAL_BRIGHT)
						{
							brightness_override = 0;
						}
					}
					*/
				}

				if (encoder_cw)
				{
					if (!PA5_LED_CHECK)
					{
						PA5_LED_OFF;
					}
					else
					{
						PA5_LED_ON;
					}

					encoder_cw = 0;
					TIM7->CNT = 0;

					if (cur_state == LCD_DISP)
					{
						emphasis_LCD = ~emphasis_LCD;
						slave_flags |= emphasis_LCD << 1;
						EEPROM_Write(SLAVE_FLAGS, 0x00, slave_flags);
						I2C1_Device_Write_STM(SLAVE_ADDR << 1, CHANGE_EMPH);
						I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
					}
					else if (cur_state == LCD_MENU)
					{
						menu_select++;
						if (menu_select > MENU_LENGTH - 1)
						{
							menu_select = 0;
						}

						I2C1_Device_Write_STM(SLAVE_ADDR << 1, MENU_NEXT);
					}
					else if (cur_state == LCD_TIME)
					{
						if (sub_state == LCD_MIN)
						{
							user_min++;

							if (user_min > 59)
							{
								user_min = 0;
							}

							temp_time[TIME_SIZE - 2] = Min_Sec_Day_Year_To_BCD(user_min);
							Min_Sec_Day_To_Dec(temp_time[TIME_SIZE - 2], min_dec, DEC_SIZE);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MIN);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_HOUR)
						{
							user_hour++;

							if (user_hour > HOUR_FORMAT)
							{
								user_hour = 1;
							}

							temp_time[TIME_SIZE - 1] = Hour_To_BCD(user_hour, (hour_dec[3] - 0x30), user_ampm);
							Hour_To_Dec(temp_time[TIME_SIZE - 1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_HOUR);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_AMPM)
						{
							// change AM/PM
							if (user_ampm == 0x30)
							{
								user_ampm = 0x32;
							}
							else if (user_ampm == 0x32)
							{
								user_ampm = 0x30;
							}

							temp_time[TIME_SIZE - 1] = Hour_To_BCD(user_hour, (hour_dec[3] - 0x30), (user_ampm - 0x30));
							Hour_To_Dec(temp_time[TIME_SIZE - 1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_AMPM);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[2]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
					}
					else if (cur_state == LCD_DATE)
					{
						if (sub_state == LCD_DAY)
						{
							user_day++;

							// determine max day based on month
							if (user_day > 31)
							{
								user_day = 1;
							}
							else if (user_day > 30 && (user_month == 4 || user_month == 6 || user_month == 9 || user_month == 11))
							{
								user_day = 1;
							}
							else if (user_day > 28 && user_month == 2)
							{
								if (user_year % 4 != 0)
								{
									user_day = 1;
								}
								else if (user_day > 29)
								{
									user_day = 1;
								}
							}

							temp_date[DATE_SIZE - DATE_SIZE] = Min_Sec_Day_Year_To_BCD(user_day);
							Min_Sec_Day_To_Dec(temp_date[DATE_SIZE - DATE_SIZE], day_dec, DEC_SIZE);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_DAY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_MONTH)
						{
							user_month++;

							if (user_month > 12)
							{
								user_month = 1;
							}

							temp_date[DATE_SIZE - 3] = Month_To_BCD(user_month, 1);
							Month_To_Dec(temp_date[DATE_SIZE - 3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MONTH);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_YEAR)
						{
							user_year++;

							if (user_year > 99)
							{
								user_year = 99;
							}

							temp_date[DATE_SIZE - 2] = Min_Sec_Day_Year_To_BCD(user_year);
							Year_To_Dec(temp_date[DATE_SIZE - 2], temp_date[DATE_SIZE - 3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_YEAR);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[2]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
					}
				}

				if (encoder_ccw)
				{
					if (!PA5_LED_CHECK)
					{
						PA5_LED_OFF;
					}
					else
					{
						PA5_LED_ON;
					}

					encoder_ccw = 0;
					TIM7->CNT = 0;

					if (cur_state == LCD_DISP)
					{
						emphasis_LCD = ~emphasis_LCD;
						slave_flags |= emphasis_LCD << 1;
						EEPROM_Write(SLAVE_FLAGS, 0x00, slave_flags);
						I2C1_Device_Write_STM(SLAVE_ADDR << 1, CHANGE_EMPH);
						I2C1_Device_Write_STM(SLAVE_ADDR << 1, END_UPDATE);
					}
					else if (cur_state == LCD_MENU)
					{
						menu_select--;
						if (menu_select > MENU_LENGTH - 1)
						{
							menu_select = 0;
						}

						I2C1_Device_Write_STM(SLAVE_ADDR << 1, MENU_PREV);
					}
					else if (cur_state == LCD_TIME)
					{
						if (sub_state == LCD_MIN)
						{
							user_min--;

							if (user_min > 59)
							{
								user_min = 59;
							}

							temp_time[TIME_SIZE - 2] = Min_Sec_Day_Year_To_BCD(user_min);
							Min_Sec_Day_To_Dec(temp_time[TIME_SIZE - 2], min_dec, DEC_SIZE);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MIN);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, min_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, DEC_VALUE);
						}
						else if (sub_state == LCD_HOUR)
						{
							user_hour--;

							if (user_hour < 1)
							{
								user_hour = HOUR_FORMAT;
							}

							temp_time[TIME_SIZE - 1] = Min_Sec_Day_Year_To_BCD(user_hour);
							Min_Sec_Day_To_Dec(temp_time[TIME_SIZE - 1], hour_dec, DEC_SIZE);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_HOUR);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, DEC_VALUE);
						}
						else if (sub_state == LCD_AMPM)
						{
							// change AM/PM
							if (user_ampm == 0x30)
							{
								user_ampm = 0x32;
							}
							else if (user_ampm == 0x32)
							{
								user_ampm = 0x30;
							}

							temp_time[TIME_SIZE - 1] = Hour_To_BCD(user_hour, (hour_dec[3] - 0x30), (user_ampm - 0x30));
							Hour_To_Dec(temp_time[TIME_SIZE - 1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_AMPM);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, hour_dec[2]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, DEC_VALUE);
						}
					}

					else if (cur_state == LCD_DATE)
					{
						if (sub_state == LCD_DAY)
						{
							user_day--;

							// determine max day based on month
							if (user_day < 1 && (user_month == 4 || user_month == 6 || user_month == 9 || user_month == 11))
							{
								user_day = 30;
							}
							else if (user_day < 1 && user_month == 2)
							{
								if (user_year % 4 != 0)
								{
									user_day = 28;
								}
								else if (user_day < 1)
								{
									user_day = 29;
								}
							}
							else if (user_day < 1)
							{
								user_day = 31;
							}

							temp_date[DATE_SIZE - DATE_SIZE] = Min_Sec_Day_Year_To_BCD(user_day);
							Min_Sec_Day_To_Dec(temp_date[DATE_SIZE - DATE_SIZE], day_dec, DEC_SIZE);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_DAY);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, day_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_MONTH)
						{
							user_month--;

							if (user_month < 1)
							{
								user_month = 12;
							}

							temp_date[DATE_SIZE - 3] = Month_To_BCD(user_month, 1);
							Month_To_Dec(temp_date[DATE_SIZE - 3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_MONTH);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, month_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
						else if (sub_state == LCD_YEAR)
						{
							user_year--;

							if (user_year > 99)
							{
								user_year = 00;
							}

							temp_date[DATE_SIZE - 2] = Min_Sec_Day_Year_To_BCD(user_year);
							Year_To_Dec(temp_date[DATE_SIZE - 2], temp_date[DATE_SIZE - 3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, SET_YEAR);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[0]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[1]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[2]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, year_dec[3]);
							I2C1_Device_Write_STM(SLAVE_ADDR << 1, INC_VALUE);
						}
					}
				}
			}
		}
	}
}
