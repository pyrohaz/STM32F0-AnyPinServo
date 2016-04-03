#ifndef ANYPINSERVO_H_
#define ANYPINSERVO_H_

/*
 * A library allowing any GPIO pin to be used to drive a servo
 *
 * Author: Harris Shallcross
 * Year: 3/4/2016
 *
 *
 *Code and example descriptions can be found on my blog at:
 *www.hsel.co.uk
 *
 *The MIT License (MIT)
 *Copyright (c) 2016 Harris Shallcross
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_tim.h>
#include <stm32f0xx_misc.h>

/*
 * APS Library Definitions
 */

//Control timer used
#define APS_TIM             TIM14

//Maximum pins allowable by the library
//NOTE: Increasing this will increase the interrupt runtime!
#define APS_MAXPINS         8

//Oversampling done by the library. This is equivalent to the
//amount of steps between fully left and fully right.
//NOTE: Increasing this increases the fire rate of the interrupts
//which may increase latencies elsewhere in the program.
#define APS_OVERSAMPLING		20

//Servo timings in us
#define APS_SERVOLEFTUS			1000
#define APS_SERVORIGHTUS		2000
#define APS_SERVOPERIODUS		20000

#define APS_SERVOPERIOD			(APS_SERVOPERIODUS/((APS_SERVORIGHTUS-APS_SERVOLEFTUS)/APS_OVERSAMPLING))
#define APS_SERVOLEFT			(APS_SERVOLEFTUS/((APS_SERVORIGHTUS-APS_SERVOLEFTUS)/APS_OVERSAMPLING))
#define APS_SERVOMIDDLE			((APS_SERVOLEFTUS + APS_SERVORIGHTUS)/(2*(APS_SERVORIGHTUS-APS_SERVOLEFTUS)/APS_OVERSAMPLING))
#define APS_SERVORIGHT			(APS_SERVORIGHTUS/((APS_SERVORIGHTUS-APS_SERVOLEFTUS)/APS_OVERSAMPLING))

//Function returns
#define AE_PINEXISTS	-1
#define AE_NOPINSLEFT	-2
#define AE_PINNOTFOUND	-3
#define AE_SUCCESS		0

typedef struct{
	GPIO_TypeDef * GPIO;
    uint8_t Init;
    uint16_t Pin, Pos;
} APS_Servo;

/*
 * Name: void APS_Init(void)
 * Usage: Initialize the AnyPinServo Library, enable the interrupt timer
 * and pre-initialize the servo pin list
 * Returns: Nothing
 */
void APS_Init(void);

/*
 * Name: int8_t APS_AddPin(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t IVal)
 * Usage: Add a pin to the Servo list, allowing its usage by the library
 * Returns: A function return value defined above, negative values indicate
 * errors.
 */
int8_t APS_AddPin(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t IVal);

/*
 * Name: int8_t APS_DeletePin(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t IVal)
 * Usage: Remove a pin to the Servo list, freeing its usage from the library
 * Returns: A function return value defined above, negative values indicate
 * errors.
 */
int8_t APS_DeletePin(GPIO_TypeDef *GPIO, uint16_t Pin);

/*
 * Name: int8_t APS_SetPosition(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value)
 * Usage: Set the position (absolute) of a predefined servo pin. Should be a value
 * between APS_SERVOLEFT and APS_SERVORIGHT
 * Returns: A function return value defined above, negative values indicate
 * errors.
 */
int8_t APS_SetPosition(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value);

/*
 * Name: int8_t APS_SetPositionPercent(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value)
 * Usage: Set the position of a predefined servo pin as a percentage. -100% is equivalent
 * to fully left whereas 100% is equivalent to fully right
 * Returns: A function return value defined above, negative values indicate
 * errors.
 */
int8_t APS_SetPositionPercent(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value);

/*
 * Name: int8_t APS_SetPositionDegree(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value)
 * Usage: Set the position of a predefined servo pin as a percentage. 0 degrees is equivalent
 * to fully left whereas 359 degrees is equivalent to fully right
 * Returns: A function return value defined above, negative values indicate
 * errors.
 */
int8_t APS_SetPositionDegree(GPIO_TypeDef *GPIO, uint16_t Pin, int16_t Value);

/*
 * Name: void APS_WaitForUpdate(void)
 * Usage: Waits until the Servo library has finished the cycle. Pin values for the
 * next cycle can be loaded once this function no-longer blocks.
 * Returns: Nothing
 */
void APS_WaitForUpdate(void);

/*
 * Name: uint8_t APS_GetStatus(void)
 * Usage: Returns the current state of the library. If the cycle is complete, returns 0.
 * Otherwise, returns 1 while cycle is being done
 * Returns: Nothing
 */
uint8_t APS_GetStatus(void);

#endif
