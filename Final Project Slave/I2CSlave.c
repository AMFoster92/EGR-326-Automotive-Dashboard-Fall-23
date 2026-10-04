/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             I2CSlave.c
 * Description:      The I2C Slave definition file.
 */

#include "I2CSlave.h"

void I2C1Slave_Init(void)
{
	GPIO_I2C1->MODER |= (ALT_FUNC << PIN_SCL1 * 2);
	GPIO_I2C1->MODER |= (ALT_FUNC << PIN_SDA1 * 2);

	GPIO_I2C1->AFR[1] |= (AF4 << (PIN_SCL1 - 8) * 4);
	GPIO_I2C1->AFR[1] |= (AF4 << (PIN_SDA1 - 8) * 4);

	GPIO_I2C1->OTYPER |= (OPEN_DRAIN << PIN_SCL1);
	GPIO_I2C1->OTYPER |= (OPEN_DRAIN << PIN_SDA1);

	GPIO_I2C1->PUPDR |= (PULLUP << PIN_SCL1 * 2);
	GPIO_I2C1->PUPDR |= (PULLUP << PIN_SDA1 * 2);

	// GPIO_I2C1->OSPEEDR |= (FAST_SPEED << PIN_SCL1 * 2);
	// GPIO_I2C1->OSPEEDR |= (FAST_SPEED << PIN_SDA1 * 2);

	RCC->APB1ENR |= I2C1_CLK;

	I2C1->CR1 |= RESET;
	I2C1->CR1 = 0;

	// I2C1->CR2 = FREQ_16MHz;

	// I2C1->CCR	= STD_100kHz;

	// I2C1->TRISE = TRISE_MAX;

	I2C1->CR1 |= I2C1_ENABLE;

	I2C1->OAR1 |= (SLAVE_ADDR << 1);
}

void I2C1_Start(void)
{
	I2C1->CR1 |= ACK_BIT;
	I2C1->CR1 |= START_GEN;
	while (!(I2C1->SR1 & 1))
		;
}

void I2C1_Stop(void)
{
	I2C1->CR1 |= STOP_GEN;
}

void I2C1_Write(uint8_t data)
{
	while (!(I2C1->SR1 & TXE_BIT))
		;
	I2C1->DR = data;
}

void I2C1_Send_Address(uint8_t addr)
{
	I2C1->DR = addr;
	while (!(I2C1->SR1 & ADDR_BIT))
		;
}

void I2C1_Clear_Address(void)
{
	uint8_t clear_addr = 0x0;

	clear_addr = (uint8_t)I2C1->SR1;
	clear_addr = (uint8_t)I2C1->SR2;
}

void I2C1_Read(uint8_t addr, uint8_t *buffer, uint8_t size)
{
	uint8_t remainder = size;
	uint8_t data = 0x00;

	I2C1_Send_Address(addr);

	if (remainder > 1)
	{
		while (remainder > 2)
		{
			while (!(I2C1->SR1 & RxNE_BIT))
				;
			buffer[size - remainder] = (uint8_t)(I2C1->DR);

			remainder--;
		}

		while (!(I2C1->SR1 & RxNE_BIT))
			;
		buffer[size - remainder] = (uint8_t)(I2C1->DR);

		remainder--;
	}

	I2C1->CR1 &= CLEAR_ACK;
	I2C1_Clear_Address();
	I2C1_Stop();

	while (!(I2C1->SR1 & RxNE_BIT))
		;
	data = (uint8_t)(I2C1->DR);

	buffer[size - remainder] = data;
}

void I2C1_Device_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
	while (I2C1->SR2 & BUSY)
		;
	I2C1_Start();
	I2C1_Send_Address(addr);
	I2C1_Clear_Address();
	I2C1_Write(reg);
	I2C1_Write(data);
	while (!(I2C1->SR1 & BTF_BIT))
		;
	I2C1_Stop();
}

void I2C1_Device_Read(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t size)
{
	while (I2C1->SR2 & BUSY)
		;
	I2C1_Start();
	I2C1_Send_Address(addr);
	I2C1_Clear_Address();
	I2C1_Write(reg);
	while (!(I2C1->SR1 & TXE_BIT))
		;
	I2C1_Start();
	I2C1_Read((addr | 1), buffer, size);
	// I2C1_Stop();
}

void I2C1_Receive_Data(void)
{
}
