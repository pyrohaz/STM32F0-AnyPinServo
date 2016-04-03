#include "APS.h"

APS_Servo list[APS_MAXPINS];
TIM_TimeBaseInitTypeDef TB;
RCC_ClocksTypeDef RC;
NVIC_InitTypeDef N;

volatile uint32_t oscnt = APS_SERVOPERIOD;
uint32_t Period;

void TIM14_IRQHandler(void){
	int32_t n;
	if(TIM_GetITStatus(APS_TIM, TIM_IT_Update)){
		TIM_ClearITPendingBit(APS_TIM, TIM_IT_Update);

		if(oscnt<=APS_SERVORIGHT){
			for(n = 0; n<APS_MAXPINS; n++){
				if(oscnt >= list[n].Pos && list[n].Init){
					GPIO_ResetBits(list[n].GPIO, list[n].Pin);
				}
			}
		}
		else if(oscnt == APS_SERVOPERIOD){
			oscnt = 0;
			for(n = 0; n<APS_MAXPINS; n++){
				if(list[n].Init){
					GPIO_SetBits(list[n].GPIO, list[n].Pin);
				}
			}
		}

		oscnt++;
	}
}

void APS_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	RCC_GetClocksFreq(&RC);

	uint32_t n;

	//Count from SERVOLEFT to SERVORIGHT in OVERSAMPLING steps minimum
	Period = (RC.PCLK_Frequency/(1000*1000))*(APS_SERVORIGHTUS-APS_SERVOLEFTUS)/APS_OVERSAMPLING;

	TB.TIM_ClockDivision = TIM_CKD_DIV1;
	TB.TIM_CounterMode = TIM_CounterMode_Up;
	TB.TIM_Prescaler = 0;
	TB.TIM_RepetitionCounter = 0;
	TB.TIM_Period = Period;
	TIM_TimeBaseInit(APS_TIM, &TB);
	TIM_ClearITPendingBit(APS_TIM, TIM_IT_Update);
	TIM_ITConfig(APS_TIM, TIM_IT_Update, ENABLE);

	N.NVIC_IRQChannel = TIM14_IRQn;
	N.NVIC_IRQChannelCmd = ENABLE;
	N.NVIC_IRQChannelPriority = 1;
	NVIC_Init(&N);

	for(n = 0; n<APS_MAXPINS; n++){
		list[n].Init = 0;
	}

	oscnt = 0;
	TIM_Cmd(APS_TIM, ENABLE);
}

int8_t APS_AddPin(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t IVal){
	uint32_t n;
	uint8_t pinexist = 0, maxpins = 1;

	//Check that pin doesn't already exist
	for(n = 0; n<APS_MAXPINS; n++){
		if(list[n].GPIO == GPIO && list[n].Pin == Pin) pinexist = 1;
		if(list[n].Init == 0) maxpins = 0;
	}

	if(pinexist) return AE_PINEXISTS;
	if(maxpins) return AE_NOPINSLEFT;

	//Add pin to list
	for(n = 0; n<APS_MAXPINS; n++){
		if(list[n].Init == 0){
			list[n].Pin = Pin;
			list[n].Pos = IVal;
			list[n].Init = 1;
			list[n].GPIO = GPIO;
			break;
		}
	}

	return AE_SUCCESS;
}

int8_t APS_DeletePin(GPIO_TypeDef * GPIO, uint16_t Pin){
	int32_t n;

	for(n = 0; n<APS_MAXPINS; n++){
		if(list[n].GPIO == GPIO && list[n].Pin == Pin){
			list[n].Init = 0;
			break;
		}
	}

	if(n == APS_MAXPINS) return AE_PINNOTFOUND;
	return AE_SUCCESS;
}

int8_t APS_SetPosition(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t Value){
	int32_t n;

	for(n = 0; n<APS_MAXPINS; n++){
		if(list[n].GPIO == GPIO && list[n].Pin == Pin){
			list[n].Pos = Value;
			break;
		}
	}

	if(n == APS_MAXPINS) return AE_PINNOTFOUND;
	return AE_SUCCESS;
}

int8_t APS_SetPositionPercent(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t Value){
	int32_t nval;

	//0->100%
	nval = ((Value+100)/2)%100;
	nval = ((100-nval)*APS_SERVOLEFT + nval*APS_SERVORIGHT)/100;
	return APS_SetPosition(GPIO, Pin, nval);
}

int8_t APS_SetPositionDegree(GPIO_TypeDef * GPIO, uint16_t Pin, int16_t Value){
	int32_t nval;

	nval = Value%360;
	nval = ((360-nval)*APS_SERVOLEFT + nval*APS_SERVORIGHT)/360;
	return APS_SetPosition(GPIO, Pin, nval);
}

uint8_t APS_GetStatus(void){
	return oscnt <= APS_SERVORIGHT;
}

void APS_WaitForUpdate(void){
	while(APS_GetStatus());
}
