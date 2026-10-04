/*
 * Name:             Aaron Foster
 * Course:           EGR 326 - Embedded System Design
 * Project:          Final Project - Speedometer
 * File:             BipolarStepperMotor.c
 * Description:      The Bipolar Stepper Motor definition file.
 */

#include "BipolarStepperMotor.h"
#include "stdio.h"

void BipolarStepper_Set_Pos(uint8_t pos)
{
	uint8_t dir = 0;

	if (pos)
	{
		// find quantity and direction of rotation
		if (cur_pos > pos) // ccw
		{
			cur_pos = cur_pos - pos;
			dir = 1;
		}
		else // cw
		{
			cur_pos = pos - cur_pos;
		}

		BipolarStep_Motor(cur_pos, dir);
	}
	else
	{
		BipolarStep_Motor(cur_pos, 1);
	}
	cur_pos = pos;
}

void BipolarStepper_Init(void)
{
	GPIO_BIPOLAR_STEPPER->MODER |= 0x01 << BIPOLAR_INT1 * 2;
	GPIO_BIPOLAR_STEPPER->MODER |= 0x01 << BIPOLAR_INT2 * 2;
	GPIO_BIPOLAR_STEPPER->MODER |= 0x01 << BIPOLAR_INT3 * 2;
	GPIO_BIPOLAR_STEPPER->MODER |= 0x01 << BIPOLAR_INT4 * 2;

	// BipolarStep_Motor(600,0);

	bipolar_motor_state = 0x00;
	cur_pos = 180;
}

void BipolarStep_Increment(void)
{
	switch (bipolar_motor_state)
	{
	case 0x00:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT4);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT1;
		// printf("1\n");
		break;

	case 0x01:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT1);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT3;
		// printf("2\n");
		break;

	case 0x02:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT3);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT2;
		// printf("3\n");
		break;

	case 0x03:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT2);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT4;
		// printf("4\n");
		break;
	}
}
void BipolarStep_Decrement(void)
{
	switch (bipolar_motor_state)
	{
	case 0x00:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT1);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT4;
		// printf("4\n");
		break;

	case 0x01:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT4);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT2;
		// printf("3\n");
		break;

	case 0x02:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT2);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT3;
		// printf("2\n");
		break;

	case 0x03:
		GPIO_BIPOLAR_STEPPER->ODR &= ~(1UL << BIPOLAR_INT3);
		GPIO_BIPOLAR_STEPPER->ODR |= 1 << BIPOLAR_INT1;
		// printf("1\n");
		break;
	}
}

void BipolarStep_Motor(uint16_t steps, uint8_t dir)
{
	for (int s = 0; s < steps; s++)
	{
		IWDG->KR = IWDG_RELOAD;
		if (dir)
		{

			for (int i = 0; i < 4; i++)
			{
				BipolarStep_Increment();
				delayMS(4);
				bipolar_motor_state++;
			}
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				BipolarStep_Decrement();
				delayMS(4);
				bipolar_motor_state++;
			}
		}

		bipolar_motor_state = 0x00;
	}
	GPIO_BIPOLAR_STEPPER->ODR &= ~(0x3C00UL);
}