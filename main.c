#include "APS.h"

GPIO_InitTypeDef G;

//Timekeeping functions
volatile uint32_t MSec = 0;

void SysTick_Handler(void){
	MSec++;
}

//Delay function (milliseconds)
void Delay(uint32_t T){
	uint32_t MSS = MSec;
	while((MSec-MSS)<T) __NOP;
}

int main(void)
{
	//Enable GPIO clock
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	//Intialize AnyPinServo library
	APS_Init();

	//Set PA0 and PA1 as outputs
	G.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	G.GPIO_Mode = GPIO_Mode_OUT;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
	G.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &G);

	//Add PA0 and PA1 to the servo list
	uint8_t res;
	res = APS_AddPin(GPIOA, GPIO_Pin_0, APS_SERVOMIDDLE);
	if(res != AE_SUCCESS){
		//Do something with result
	}

	res = APS_AddPin(GPIOA, GPIO_Pin_1, APS_SERVOMIDDLE);
	if(res != AE_SUCCESS){
		//Do something with result
	}

	//Initialize millisecond counter
	SysTick_Config(SystemCoreClock/1000);

	int16_t pos = 0;
	int8_t inc = 1;
	while(1){
		//Create a "triangle wave" control signal for PA0
		pos += inc;
		if(pos >= 100){
			inc = -inc;
			pos = 100;
		}
		else if(pos<-100){
			pos = -100;
			inc = -inc;
		}

		//Set PA0 servo position to "pos"
		APS_SetPositionPercent(GPIOA, GPIO_Pin_0, pos);

		//Set PA1 servo to 0 degrees
		APS_SetPositionDegree(GPIOA, GPIO_Pin_1, 0);

		//Wait for cycle to end
		APS_WaitForUpdate();

		//Pointless delay...
		Delay(1);
	}
}
