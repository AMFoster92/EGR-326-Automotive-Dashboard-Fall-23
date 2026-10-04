/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             RTC.c
 * Description:      The Real Time Clock definition file.
 */

#include "RTC.h"

void Read_Time(void)
{
	I2C1_Device_Read(RTC_ADDR, SEC_ADDR, &time[TIME_SIZE - TIME_SIZE], 1);
	I2C1_Device_Read(RTC_ADDR, MIN_ADDR, &time[TIME_SIZE - 2], 1);
	I2C1_Device_Read(RTC_ADDR, HOUR_ADDR, &time[TIME_SIZE - 1], 1);

	Min_Sec_Day_To_Dec(time[TIME_SIZE - TIME_SIZE], sec_dec, DEC_SIZE);
	Min_Sec_Day_To_Dec(time[TIME_SIZE - 2], min_dec, DEC_SIZE);
	Hour_To_Dec(time[TIME_SIZE - 1]);
}

void Write_Time(uint8_t sec, uint8_t min, uint8_t hour)
{
	I2C1_Device_Write(RTC_ADDR, SEC_ADDR, sec);
	I2C1_Device_Write(RTC_ADDR, MIN_ADDR, min);
	I2C1_Device_Write(RTC_ADDR, HOUR_ADDR, hour);
}

void Read_Date(void)
{
	I2C1_Device_Read(RTC_ADDR, DATE_ADDR, &date[DATE_SIZE - DATE_SIZE], 1);
	I2C1_Device_Read(RTC_ADDR, MONTH_ADDR, &date[DATE_SIZE - 3], 1);
	I2C1_Device_Read(RTC_ADDR, YEAR_ADDR, &date[DATE_SIZE - 2], 1);
	I2C1_Device_Read(RTC_ADDR, DOW_ADDR, &date[DATE_SIZE - 1], 1);

	Min_Sec_Day_To_Dec(date[DATE_SIZE - DATE_SIZE], day_dec, DEC_SIZE);
	Month_To_Dec(date[DATE_SIZE - 3]);
	Year_To_Dec(date[DATE_SIZE - 2], date[DATE_SIZE - 3]);
	Dow_To_Dec(date[DATE_SIZE - 1]);
}

void Write_Date(uint8_t day, uint8_t month, uint8_t year, uint8_t dow)
{
	I2C1_Device_Write(RTC_ADDR, DATE_ADDR, day);
	I2C1_Device_Write(RTC_ADDR, MONTH_ADDR, month);
	I2C1_Device_Write(RTC_ADDR, YEAR_ADDR, year);
	I2C1_Device_Write(RTC_ADDR, DOW_ADDR, dow);
}

void Read_Date_Time(void)
{
	Read_Time();
	Read_Date();
}

void Set_Date_Time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t hour_fmt, uint8_t am_pm,
				   uint8_t day, uint8_t month, uint8_t century, uint8_t year, uint8_t dow)
{
	Write_Time(Min_Sec_Day_Year_To_BCD(sec), Min_Sec_Day_Year_To_BCD(min), Hour_To_BCD(hour, hour_fmt, am_pm));
	Write_Date(Min_Sec_Day_Year_To_BCD(day), Month_To_BCD(month, century), Min_Sec_Day_Year_To_BCD(year), dow);
}

uint8_t Min_Sec_Day_Year_To_BCD(uint8_t dec)
{
	uint8_t BCD = 0x00;

	BCD |= ((dec / 10) << 4);
	BCD |= (dec % 10);

	return BCD;
}

uint8_t Hour_To_BCD(uint8_t hour, uint8_t hour_fmt, uint8_t am_pm)
{
	uint8_t BCD = 0x00;

	if (hour_fmt)
	{
		BCD |= (1 << 6);

		if (am_pm)
		{
			BCD |= (1 << 5);
		}
		BCD |= ((hour / 10) << 4);
		BCD |= (hour % 10);
	}
	else
	{
		if (hour >= 20)
		{
			BCD |= ((hour / 20) << 5);
		}
		else
		{
			BCD |= ((hour / 10) << 4);
		}

		BCD |= (hour % 10);
	}

	return BCD;
}

uint8_t Month_To_BCD(uint8_t month, uint8_t century)
{
	uint8_t BCD = 0x00;

	BCD |= ((month / 10) << 4);
	BCD |= (month % 10);

	if (century)
	{
		BCD |= (1 << 7);
	}

	return BCD;
}

void Min_Sec_Day_To_Dec(uint8_t BCD, uint8_t dec[], uint8_t size)
{
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;

	tens = ((BCD & 0xF0) >> 4);
	ones = (BCD & 0x0F);

	dec[size - size] = tens + 0x30;
	dec[size - 1] = ones + 0x30;
}
void Hour_To_Dec(uint8_t BCD)
{
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;
	uint8_t am_pm = 0x00;
	uint8_t hour_fmt = 0x00;

	hour_fmt = ((BCD & 0x40) >> 4);

	if (hour_fmt)
	{
		tens = ((BCD & 0x10) >> 4);
		ones = (BCD & 0x0F);
		am_pm = ((BCD & 0x20) >> 4);
	}
	else
	{
		tens = (BCD >> 4);
		ones = (BCD & 0x0F);
	}

	hour_dec[0] = tens + 0x30;
	hour_dec[1] = ones + 0x30;
	hour_dec[2] = am_pm + 0x30;
	hour_dec[3] = hour_fmt + 0x30;
}

void Month_To_Dec(uint8_t BCD)
{
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;

	tens = ((BCD & 0x10) >> 4);
	ones = (BCD & 0x0F);

	month_dec[0] = tens + 0x30;
	month_dec[1] = ones + 0x30;
}

void Year_To_Dec(uint8_t BCD_year, uint8_t BCD_century)
{
	uint8_t century = 0x00;
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;

	century = ((BCD_century & 0x80) >> 4);
	tens = ((BCD_year & 0xF0) >> 4);
	ones = (BCD_year & 0x0F);

	if (century)
	{
		year_dec[0] = 0x02 + 0x30;
		year_dec[1] = 0x00 + 0x30;
	}
	else
	{
		year_dec[0] = 0x01 + 0x30;
		year_dec[1] = 0x09 + 0x30;
	}

	year_dec[2] = tens + 0x30;
	year_dec[3] = ones + 0x30;
}

void Dow_To_Dec(uint8_t BCD)
{
	uint8_t dow = 0x00;

	dow = (BCD & 0x07);

	dow_dec = dow;
}

/*void Print_Date_Time(void)
{
	//Read_Date_Time();

	Print_Time();
	Print_Date();
	//USART2_Write_Char(0x0A);
	//USART2_Write_Char(0x0D);
}

void Print_Dow(uint8_t dow)
{
	switch(dow)
	{
		case 0x1:
			USART2_Write_Str(sunday, MFSUN_SIZE);
			break;

		case 0x2:
			USART2_Write_Str(monday, MFSUN_SIZE);
			break;

		case 0x3:
			USART2_Write_Str(tuesday, TUES_SIZE);
			break;

		case 0x4:
			USART2_Write_Str(wednesday, WED_SIZE);
			break;

		case 0x5:
			USART2_Write_Str(thursday, THSAT_SIZE);
			break;

		case 0x6:
			USART2_Write_Str(friday, MFSUN_SIZE);
			break;

		case 0x7:
			USART2_Write_Str(saturday, THSAT_SIZE);
			break;
	}

	USART2_Write_Str(day_str, MFSUN_SIZE);
}

void Print_Date(void)
{
	Read_Date();

	USART2_Write_Char(dow_dec);
	Print_Dow(dow_dec);
	USART2_Write_Char(' ');
	USART2_Write_Str(month_dec, DEC_SIZE);
	USART2_Write_Char('-');
	USART2_Write_Str(day_dec, DEC_SIZE);
	USART2_Write_Char('-');
	USART2_Write_Str(year_dec, TIME_SIZE+1);
	USART2_Write_Char(0x0A);
	USART2_Write_Char(0x0D);
}

void Print_Time(void)
{
	Read_Time();

	USART2_Write_Str(hour_dec, DEC_SIZE);
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
*/
void Read_Temp(void)
{
	I2C1_Device_Read(RTC_ADDR, TEMP_U_ADDR, &temp_upper, 1);
	I2C1_Device_Read(RTC_ADDR, TEMP_L_ADDR, &temp_lower, 1);

	Temp_to_Dec(temp_upper, temp_lower);
	C_to_F(temp_C, TEMP_F_SIZE);
}
/*void Print_Temp(void)
{
	Read_Temp();

	USART2_Write_Char('T');
	USART2_Write_Char('E');
	USART2_Write_Char('M');
	USART2_Write_Char('P');
	USART2_Write_Char('E');
	USART2_Write_Char('R');
	USART2_Write_Char('A');
	USART2_Write_Char('T');
	USART2_Write_Char('U');
	USART2_Write_Char('R');
	USART2_Write_Char('E');
	USART2_Write_Char(':');
	USART2_Write_Char(' ');

	if(F_not_C)
	{
		USART2_Write_Char(temp_F[0]);
		if(temp_F[1] & 0xF)
		{
			USART2_Write_Char(temp_F[1] + 0x30);
		}
		USART2_Write_Char(temp_F[2] + 0x30);
		USART2_Write_Char(temp_F[3] + 0x30);
		USART2_Write_Char('.');
		USART2_Write_Char(temp_F[4] + 0x30);
		USART2_Write_Char(temp_F[5] + 0x30);
		USART2_Write_Char(0xF8);
		USART2_Write_Char('F');
		USART2_Write_Char(0x0A);
		USART2_Write_Char(0x0D);
	}
	else
	{
		USART2_Write_Char(temp_C[0]);
		if(temp_C[1] & 0xF)
		{
			USART2_Write_Char(temp_C[1] + 0x30);
		}
		USART2_Write_Char(temp_C[2] + 0x30);
		USART2_Write_Char('.');
		USART2_Write_Char(temp_C[3] + 0x30);
		USART2_Write_Char(temp_C[4] + 0x30);
		USART2_Write_Char(0xF8);
		USART2_Write_Char('C');
		USART2_Write_Char(0x0A);
		USART2_Write_Char(0x0D);
	}
}
*/
void C_to_F(uint8_t temp_c[], uint8_t size)
{
	double temp = temp_c[1] * 10 + temp_c[2] + temp_c[3] * .1 + temp_c[4] * .01;
	uint8_t sign = 0x00;
	uint8_t hundreds = 0x00;
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;
	uint8_t frac_tens = 0x00;
	uint8_t frac_ones = 0x00;

	if (!(temp_c[0] ^ 0x2D))
	{
		temp = temp * -1;
	}

	temp = ((temp * 9) / 5) + 32;

	if (temp < 0)
	{
		sign = 0x2D;
		temp = temp * -1;
	}
	else
	{
		sign = 0x2B;
	}

	temp = temp * 100;

	hundreds = (uint8_t)((uint32_t)temp / 10000);
	tens = (uint8_t)(((uint32_t)temp / 1000) % 10);
	ones = (uint8_t)(((uint32_t)temp / 100) % 10);
	frac_tens = (uint8_t)(((uint32_t)temp / 10) % 10);
	frac_ones = (uint8_t)((uint32_t)temp % 10);

	temp_F[size - size] = sign;
	temp_F[size - 5] = hundreds;
	temp_F[size - 4] = tens;
	temp_F[size - 3] = ones;
	temp_F[size - 2] = frac_tens;
	temp_F[size - 1] = frac_ones;
}

void Temp_to_Dec(uint8_t temp_u, uint8_t temp_l)
{
	uint8_t sign = 0x00;
	uint8_t tens = 0x00;
	uint8_t ones = 0x00;
	uint8_t frac_tens = 0x00;
	uint8_t frac_ones = 0x00;
	uint8_t temp = 0x00;

	if (temp_u & 0x80)
	{
		sign = 0x2D;

		temp = ((temp_u ^ 0xFF) + 1);
	}
	else
	{
		sign = 0x2B;

		temp = temp_u;
	}

	tens = ((temp & 0x70) >> 4);
	ones = (temp & 0x0F);

	if (ones > 0x09)
	{
		tens += 1;
		ones -= 10;
	}

	if (!(temp_l ^ 0x01))
	{
		frac_ones = 0x05;
	}
	else if (!(temp_l ^ 0x10))
	{
		frac_tens = 0x05;
	}
	else if (!(temp_l ^ 0x11))
	{
		frac_tens = 0x07;
		frac_ones = 0x05;
	}

	temp_C[0] = sign;
	temp_C[1] = tens;
	temp_C[2] = ones;
	temp_C[3] = frac_tens;
	temp_C[4] = frac_ones;
}
