/*
 * 017_uart_case_exchange.c
 *
 *  Created on: Jun 8, 2026
 *      Author: chandan
 *
 *
 *      Task:
 *      =====
 *      write a program for stm32 board which transmit different messages to arduino board over UART communication.
 *
 *      for every message STM32 board sends, arduino code will change the case of alphabets(lower case to higher and
 *      vice-versa) and sends message back to the stm32 board.
 *
 *      The stm32 board should capture the replay from the arduino board and display using ITM or semi hosting.
 *
 *
 *      Baudrate: 115200bps
 *      Frame format: 1 stop bits, 8 bits, no parity
 *
 *      USART1 Pin Configuration
 *      ======================
 *      RX --> PA10
 *      TX --> PA9
 *
 *      ALT function mode: AF7
 */

#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"



//we have 3 different messages that we transmit to arduino
//you can by all means add more messages
char *msg[3] = {"hihihihihihi123", "Hello How are you ?" , "Today is Monday !"};

//reply from arduino will be stored here
char rx_buf[1024] = {0};

USART_Handle_t USART1Handle = {0};


//This flag indicates reception completion
uint8_t rxCmplt = RESET;

uint8_t g_data = 0;



void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}


void USART1_Init(void)
{
	USART1Handle.pUSARTx = USART1;
	USART1Handle.USART_Config.USART_BaudRate = USART_STD_BAUD_115200;
	USART1Handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	USART1Handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	USART1Handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	USART1Handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	USART1Handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;

	USART_Init(&USART1Handle);
}


void USART1_GPIOInit(void)
{
	GPIO_Handle_t USART1Pins = {0};

	USART1Pins.pGPIOx = GPIOA;
	USART1Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	USART1Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	USART1Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	USART1Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	USART1Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF7;

	//USART2 TX
	USART1Pins.GPIO_PinConfig.GPIO_PinNumber  = GPIO_PIN_NO_9;
	GPIO_Init(&USART1Pins);

	//USART2 RX
	USART1Pins.GPIO_PinConfig.GPIO_PinNumber  = GPIO_PIN_NO_10;
	GPIO_Init(&USART1Pins);
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
	uint32_t cnt = 0;

	Onboard_ButtonInit();
	USART1_GPIOInit();
    USART1_Init();

    USART_IRQInterruptConfig(IRQ_NO_USART1,
    						 ENABLE);

    USART_PeripheralControl(USART1,
    						ENABLE);

    printf("Application is running\n");

    //do forever
    while(1)
    {
		//wait till button pressed
		while(GPIO_ReadFromInputPin(GPIOC,
									GPIO_PIN_NO_13));
		delay();//for de-bounce

		// Next message index ; make sure that cnt value doesn't cross 2
		cnt = cnt % 3;

		//First lets enable the reception in interrupt mode
		//this code enables the receive interrupt
		while(USART_ReceiveDataIT(&USART1Handle,
								  (uint8_t*)rx_buf,
								  strlen(msg[cnt])) != USART_READY);

		//Send the msg indexed by cnt in blocking mode
    	USART_SendData(&USART1Handle,
    				  (uint8_t*)msg[cnt],
					  strlen(msg[cnt]));

    	printf("Transmitted : %s\n",msg[cnt]);


    	//Now lets wait until all the bytes are received from the arduino .
    	//When all the bytes are received rxCmplt will be SET in application callback
    	while(rxCmplt != SET);

    	//just make sure that last byte should be null otherwise %s fails while printing
    	rx_buf[strlen(msg[cnt]) + 1] = '\0';

    	//Print what we received from the arduino
    	printf("Received    : %s\n",rx_buf);

    	//invalidate the flag
    	rxCmplt = RESET;

    	//move on to next message indexed in msg[]
    	cnt ++;
    }
	return 0;
}





void USART1_IRQHandler(void)
{
	USART_IRQHandling(&USART1Handle);
}





void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,
									uint8_t AppEvent)
{
   if(AppEvent == USART_EV_RX_CMPLT)
   {
			rxCmplt = SET;

   }
   else if (AppEvent == USART_EV_TX_CMPLT)
   {
	   ;
   }
}








