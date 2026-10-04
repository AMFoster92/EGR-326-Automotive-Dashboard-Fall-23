/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             UnipolarStepperMotor.c
 * Description:      The Unipolar Stepper Motor initialization definition file.
 */

#include "UnipolarStepperMotor.h"
#include "stdio.h"

void UnipolarStepper_Init(void)
{
	GPIO_UNIPOLAR_STEPPER->MODER |= 0x01 << UNI_INT1 * 2;
	GPIO_UNIPOLAR_STEPPER->MODER |= 0x01 << UNI_INT2 * 2;
	GPIO_UNIPOLAR_STEPPER->MODER |= 0x01 << UNI_INT3 * 2;
	GPIO_UNIPOLAR_STEPPER->MODER |= 0x01 << UNI_INT4 * 2;

	unipolar_motor_state = 0x00;
}

void UnipolarStep_Increment(void)
{
	switch (unipolar_motor_state)
	{
	case 0x00:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT4);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT1);
		// printf("1\n");
		break;

	case 0x01:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT1);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT2);
		// printf("2\n");
		break;

	case 0x02:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT2);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT3);
		// printf("3\n");
		break;

	case 0x03:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT3);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT4);
		// printf("4\n");
		break;
	}
}
void UnipolarStep_Decrement(void)
{
	switch (unipolar_motor_state)
	{
	case 0x00:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT1);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT4);
		// printf("4\n");
		break;

	case 0x01:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT4);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT3);
		// printf("3\n");
		break;

	case 0x02:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT3);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT2);
		// printf("2\n");
		break;

	case 0x03:
		GPIO_UNIPOLAR_STEPPER->ODR &= ~(1 << UNI_INT2);
		GPIO_UNIPOLAR_STEPPER->ODR |= (1 << UNI_INT1);
		// printf("1\n");
		break;
	}
}

void UnipolarStep_Motor(uint16_t steps, uint8_t dir)
{
	for (int s = 0; s < steps; s++)
	{
		if (dir)
		{
			for (int i = 0; i < 4; i++)
			{
				UnipolarStep_Increment();
				SysTick_msdelay(50);
				unipolar_motor_state++;
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				UnipolarStep_Decrement();
				SysTick_msdelay(50);
				unipolar_motor_state++;
			}
		}

		unipolar_motor_state = 0x00;
	}
	GPIO_UNIPOLAR_STEPPER->ODR &= ~(0x0FFUL);
}

void UnipolarMotor_Speed(uint8_t speed)
{
	switch (speed)
	{
	case 1:
		delay = SPEED_1;
		break;

	case 2:
		delay = SPEED_2;
		break;

	case 3:
		delay = SPEED_3;
		break;

	case 4:
		delay = SPEED_4;
		break;

	case 5:
		delay = SPEED_5;
		break;

	default:
		delay = SPEED_0;
	}
}