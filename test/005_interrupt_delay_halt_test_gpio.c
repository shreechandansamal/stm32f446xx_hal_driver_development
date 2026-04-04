/*
 *  File Name: 005_interrupt_delay_halt_test_gpio.c
 *
 *  Created on: Feb 18, 2026
 *      Author: chandan
 */

//Device specific header file
#include "stm32f446xx.h"

#define LOW 		   DISABLE
#define BTN_PRESSED        LOW

#define NUM_LEDS 8

// LED pins from PC5 to PC12
uint8_t led_pins[NUM_LEDS] = {
        GPIO_PIN_NO_5,
        GPIO_PIN_NO_6,
        GPIO_PIN_NO_7,
        GPIO_PIN_NO_8,
        GPIO_PIN_NO_9,
        GPIO_PIN_NO_10,
        GPIO_PIN_NO_11,
        GPIO_PIN_NO_12
};

// simple delay
void delay(void)
{
    for(uint32_t i = 0; i < 500000; i++);
}

int main(void)
{
    GPIO_Handle_t GpioLed;

    // Enable clock for GPIOC
    GPIO_PeriClockControl(GPIOC, ENABLE);

    // Common configuration for all LEDs
    GpioLed.pGPIOx = GPIOC;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // Initialize all LED pins
    for(int i = 0; i < NUM_LEDS; i++)
    {
        GpioLed.GPIO_PinConfig.GPIO_PinNumber = led_pins[i];
        GPIO_Init(&GpioLed);
    }



    	/***************************************************Start Led Init***********************************************/
    	//Step1: Initate the object of type GPIO_Handle_t
    	GPIO_Handle_t GpioLedInt;

    	//Step2: Push Pull Cofiguration for Led, nothing but we are initializing our custom variables(struct var)
    	GpioLedInt.pGPIOx = GPIOA;
    	GpioLedInt.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    	GpioLedInt.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    	GpioLedInt.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    	GpioLedInt.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    	GpioLedInt.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    	//Step3: Enable the Clock for GPIOA
    	GPIO_PeriClockControl(GPIOA, ENABLE);

    	//Step4: Initialize the GPIO LED
    	GPIO_Init(&GpioLedInt);
    	/***************************************************End Led Init**************************************************/







    	/*************************************************Start Button Init***********************************************/
    	//Step1: Initate the object of type GPIO_Handle_t
    	GPIO_Handle_t GpioBtn;

    	//Step2: Button Configuration
    	GpioBtn.pGPIOx = GPIOC;
    	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
    	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;


    	//Step3: Enable the Clock for GPIOC
    	GPIO_PeriClockControl(GPIOC, ENABLE);

    	//Step4: Initialize the GPIO Button
    	GPIO_Init(&GpioBtn);
    	/*************************************************End Button Init***********************************************/



    	//Step5: IRQ configurations
    	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, NVIC_IRQ_PRI15);
    	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);


    while(1)
    {
        for(int i = 0; i < NUM_LEDS; i++)
        {
            // Turn ON current LED
            GPIO_WriteToOutputPin(GPIOC, led_pins[i], SET);
            delay();

            // Turn OFF current LED
            GPIO_WriteToOutputPin(GPIOC, led_pins[i], RESET);
        }
    }

    return 0;
}


//ISR function for EXTI2
void EXTI15_10_IRQHandler(void){
	delay();
	//here we have to call the driver supplied GPIO interrupt handeling API
	GPIO_IRQHandling(GPIO_PIN_NO_13);
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}





































