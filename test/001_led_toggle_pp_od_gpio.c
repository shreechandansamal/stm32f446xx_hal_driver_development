/*
 *  File Name: 001_led_toggle_pp_od_gpio.c
 *
 *  Created on: Nov 28, 2024
 *      Author: chandan
 */

//Device specific header file
#include "stm32f446xx.h"

//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


int main(void){

	GPIO_Handle_t GpioLed;

	//Push Pull Configuration, nothing but we are initializing our custom variables(struct var), you got it right?
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	//Open drain Configuration
//	GpioLed.pGPIOx = GPIOA;
//	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
//	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
//	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;


	//Enable the Clock for GPIOA
//	GPIO_PeriClockControl(GpioLed.pGPIOx, ENABLE); not needed API called in the Init of the GPIO
	//Initialize the GPIO
	GPIO_Init(&GpioLed);

	while(1){
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		delay();
	}
	return 0;
}
