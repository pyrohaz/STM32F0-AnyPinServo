#ifndef ANYPINSERVO_H_
#define ANYPINSERVO_H_

typedef enum{
	GPIOA = 0,
	GPIOB = 1,
	GPIOC = 2,
	GPIOD = 3
} APS_GPIO;

void APS_Init(void);
void APS_AddPin(APS_GPIO Gpio, uint16_t Pin);
uint8_t APS_DeletePin(APS_GPIO Gpio, uint16_t Pin);

#endif
