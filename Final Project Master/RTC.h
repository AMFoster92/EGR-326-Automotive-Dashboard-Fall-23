/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             RTC.h
 * Description:      The Real Time Clock header file.
 */

#include "stm32f446xx.h"
#include "I2C.h"
// #include "USART.h"

#define GPIO_RTC GPIO_I2C1
#define SDA PIN_SDA
#define SCL PIN_SCL
#define SEC_ADDR 0x00
#define MIN_ADDR 0x01
#define HOUR_ADDR 0x02
#define DOW_ADDR 0x03
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06
#define TIME_SIZE 3
#define DATE_SIZE 4
#define DEC_SIZE 2
#define RTC_SIZE TIME_SIZE + DATE_SIZE
#define RTC_ADDR (0x68 << 1)
#define MFSUN_SIZE 3
#define TUES_SIZE 4
#define WED_SIZE 6
#define THSAT_SIZE 5
#define TEMP_U_ADDR 0x11 // sign bit 7, integer part bit 0-6
#define TEMP_L_ADDR 0x12 // fractional part, bit 6-7
#define TEMP_C_SIZE 5
#define TEMP_F_SIZE 6

extern uint8_t time[TIME_SIZE];
extern uint8_t date[DATE_SIZE];
extern uint8_t date_time[RTC_SIZE];
extern uint8_t sec_dec[DEC_SIZE];
extern uint8_t min_dec[DEC_SIZE];
extern uint8_t hour_dec[TIME_SIZE + 1]; // tens, ones, am_pm, hour_fmt
extern uint8_t day_dec[DEC_SIZE];
extern uint8_t month_dec[DEC_SIZE];
extern uint8_t year_dec[TIME_SIZE + 1];
extern uint8_t dow_dec;
extern uint8_t temp_upper;
extern uint8_t temp_lower;
extern uint8_t temp_C[TEMP_C_SIZE]; // sign, tens, ones, tenths, hundredths
extern uint8_t temp_F[TEMP_F_SIZE]; // sign, hundreds, tens, ones, tenths, hundredths
extern uint8_t F_not_C;

static uint8_t monday[MFSUN_SIZE] = {'M', 'O', 'N'};
static uint8_t tuesday[TUES_SIZE] = {'T', 'U', 'E', 'S'};
static uint8_t wednesday[WED_SIZE] = {'W', 'E', 'D', 'N', 'E', 'S'};
static uint8_t thursday[THSAT_SIZE] = {'T', 'H', 'U', 'R', 'S'};
static uint8_t friday[MFSUN_SIZE] = {'F', 'R', 'I'};
static uint8_t saturday[THSAT_SIZE] = {'S', 'A', 'T', 'U', 'R'};
static uint8_t sunday[MFSUN_SIZE] = {'S', 'U', 'N'};
static uint8_t day_str[MFSUN_SIZE] = {'D', 'A', 'Y'};

void Read_Time(void);
void Write_Time(uint8_t, uint8_t, uint8_t);
void Read_Date(void);
void Write_Date(uint8_t, uint8_t, uint8_t, uint8_t);
void Read_Date_Time(void);
void Set_Date_Time(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

uint8_t Min_Sec_Day_Year_To_BCD(uint8_t);
uint8_t Hour_To_BCD(uint8_t, uint8_t, uint8_t);
uint8_t Month_To_BCD(uint8_t, uint8_t);

void Min_Sec_Day_To_Dec(uint8_t, uint8_t[], uint8_t);
void Hour_To_Dec(uint8_t);
void Month_To_Dec(uint8_t);
void Year_To_Dec(uint8_t, uint8_t);
void Dow_To_Dec(uint8_t);

/*void Print_Date_Time(void);
void Print_Date(void);
void Print_Time(void);
void Print_Dow(uint8_t);*/

void Read_Temp(void);
// void Print_Temp(void);
void C_to_F(uint8_t[], uint8_t);
void Temp_to_Dec(uint8_t, uint8_t);