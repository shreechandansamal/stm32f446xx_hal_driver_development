/*
 *  File Name: 003_external_led_button_gpio.c
 *
 *  Created on: Jan 27, 2025
 *  Updated on: Oct 01, 2025
 *      Author: chandan
 */

//Device specific header file
#include "stm32f446xx.h"

#define LOW 		   DISABLE
#define BTN_PRESSED        LOW

//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


int main(void){

	GPIO_Handle_t GpioLed, GpioBtn;

	//Push Pull Cofiguration for Led, nothing but we are initializing our custom variables(struct var), you got it right?
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	//Enable the Clock for GPIOA
	GPIO_PeriClockControl(GPIOA, ENABLE);
	//Initialize the GPIO LED
	GPIO_Init(&GpioLed);

	//Button Configuration
	GpioBtn.pGPIOx = GPIOB;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;


	//Enable the Clock for GPIOB
	GPIO_PeriClockControl(GPIOB, ENABLE);
	//Initialize the GPIO Button
	GPIO_Init(&GpioBtn);

	while(1){

		if(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12) == BTN_PRESSED){
			delay();//for debounce
			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
		}

	}
	return 0;
}

