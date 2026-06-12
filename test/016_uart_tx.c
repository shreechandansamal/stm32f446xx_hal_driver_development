/*
 * 016_uart_tx.c
 *
 *  Created on: Jun 7, 2026
 *      Author: chandan
 *
 *      Task:
 *      =====
 *      program to send some message over UART from STM32 board to Arduino board.
 *      Arduino board will display the message (on Arduino serial monitor) sent
 *      from the STM board.
 *
 *      Baudrate: 115200bps
 *      Frame format: 1 stop bits, 8 bits, no parity
 *
 *      USART2 Pin Configuration
 *      ======================
 *      TX --> PA2
 *
 *      ALT function mode: AF7
 */

#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"


char msg[1024] = "UART Tx testing...\n\r";

USART_Handle_t USART2Handle = {0};

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}


void USART2_Init(void)
{
	USART2Handle.pUSARTx = USART2;
	USART2Handle.USART_Config.USART_BaudRate = USART_STD_BAUD_115200;
	USART2Handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	USART2Handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	USART2Handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	USART2Handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	USART2Handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

	USART_Init(&USART2Handle);
}


void USART2_GPIOInit(void)
{
	GPIO_Handle_t USART2Pins = {0};

	USART2Pins.pGPIOx = GPIOA;
	USART2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	USART2Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	USART2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	USART2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	USART2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF7;

	//USART2 TX
	USART2Pins.GPIO_PinConfig.GPIO_PinNumber  = GPIO_PIN_NO_2;
	GPIO_Init(&USART2Pins);
}


void Onboard_ButtonInit(void)
{
	GPIO_Handle_t GpioBtn = {0};

	//Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	GPIO_Init(&GpioBtn);
}



int main(void)
{

	Onboard_ButtonInit();

	USART2_GPIOInit();

    USART2_Init();

    USART_PeripheralControl(USART2,
    						ENABLE);

    while(1)
    {
		//wait till button pressed
		while(GPIO_ReadFromInputPin(GPIOC,
									GPIO_PIN_NO_13));
		delay();//for de-bounce

		USART_SendData(&USART2Handle,
					   (uint8_t*)msg,
					   strlen(msg));

    }

	return 0;
}
