/*
 *  File Name: 002_inbuilt_led_button_gpio.c
 *
 *  Created on: Nov 28, 2024
 *      Author: chandan
 */

//Device specific header file
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"

#define LOW 		   DISABLE
#define BTN_PRESSED        LOW

//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


int main(void){

	GPIO_Handle_t GpioLed, GpioBtn;

	//Push Pull Configuration for Led, nothing but we are initializing our custom variables(struct var), you got it right?
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	//Enable the Clock for GPIOA
//	GPIO_PeriClockControl(GPIOA, ENABLE); //no need, done in the Init itself
	//Initialize the GPIO LED
	GPIO_Init(&GpioLed);

	//Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;


	//Enable the Clock for GPIOC
//	GPIO_PeriClockControl(GPIOC, ENABLE); //no need, done in the Init itself
	//Initialize the GPIO Button
	GPIO_Init(&GpioBtn);

	while(1){
//		if(!(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13))){
//			GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, 1);
//		}else{
//			GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, 0);
//		}

		if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == BTN_PRESSED){
			delay();//for de-bounce
			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		}

	}
	return 0;
}

