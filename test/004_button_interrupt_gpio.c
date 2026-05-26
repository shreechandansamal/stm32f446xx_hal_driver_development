/*
 *  File Name: 004_button_interrupt_gpio.c
 *  Description: Button Interrupt Application
 *  Created on: Dec 11, 2024
 *  Author: chandan
 */




//Device specific header file
#include "stm32f446xx.h"

#define LOW 		   	   DISABLE
#define BTN_PRESSED        LOW

//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


int main(void){

	/***************************************************Start Led Init***********************************************/
	//Step1: Initiate the object of type GPIO_Handle_t
	GPIO_Handle_t GpioLed;

	//Step2: Push Pull Configuration for Led, nothing but we are initializing our custom variables(struct var)
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	//Step3: Enable the Clock for GPIOA
//	GPIO_PeriClockControl(GPIOA, ENABLE); // don't use here already mentioned in GPIO Init

	//Step4: Initialize the GPIO LED
	GPIO_Init(&GpioLed);
	/***************************************************End Led Init**************************************************/


	/*************************************************Start Button Init***********************************************/
	//Step1: Initiate the object of type GPIO_Handle_t
	GPIO_Handle_t GpioBtn;

	//Step2: Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;


	//Step3: Enable the Clock for GPIOC
//	GPIO_PeriClockControl(GPIOC, ENABLE); // don't use here already mentioned in GPIO Init

	//Step4: Initialize the GPIO Button
	GPIO_Init(&GpioBtn);
	/*************************************************End Button Init***********************************************/



	//Step5: IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);

	while(1);

}


//ISR function for EXTI2
void EXTI15_10_IRQHandler(void){
	delay();
	//here we have to call the driver supplied GPIO interrupt handling API
	GPIO_IRQHandling(GPIO_PIN_NO_13);
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}




