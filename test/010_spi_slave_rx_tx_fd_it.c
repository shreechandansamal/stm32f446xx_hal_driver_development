/*
 * 010_slave_rx_tx_fd_it.c
 *
 *  Created on: Apr 27, 2026
 *      Author: chandan
 *
 * 		This application receives and prints the user message received from the Master peripheral in SPI interrupt mode
 * 		User sends the message through pressing the user button.
 * 		there are some(e.g. 5) pre-stored messages in condition case 0-5 different messages,
 * 		each time user button press do only increment the count till 5, if 5 count will be again 0.
 * 		in the condition we will check the count number according to the
 * 		Monitor the message received in the SWV itm data console
 *
 *      SPI2 Pin Configuration
 *      ======================
 *      MOSI --> PB15
 *      MISO --> PB14
 *      SCLK --> PB13
 *      NSS  --> PB12
 *      ALT function mode: AF5
 *
 *
 */


#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"
#include <string.h>

#define MAX_LEN 500

SPI_Handle_t SPI2handle;

char msg[] = "Hello Master! This is STM32 Slave...\n";
uint32_t msg_len = sizeof(msg);

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i++);
}

void delay_50us(void)
{
    for(uint32_t i = 0 ; i < 50 ; i++);
}

void delay_100us(void)
{
    for(uint32_t i = 0 ; i < 120 ; i++);
}

/* SPI GPIO */
void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPI2Pins = {0};

	SPI2Pins.pGPIOx = GPIOB;
	SPI2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPI2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF5;
	SPI2Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	SPI2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//MOSI
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI2Pins);

	//MISO
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPI2Pins);

	//SCLK
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI2Pins);

	//NSS
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&SPI2Pins);
}

/* SPI SLAVE INIT */
void SPI2_SlaveInit(void)
{

	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_SLAVE;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
//	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV4;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	SPI_Init(&SPI2handle);
}

/* BUTTON PC13 */
void Button_Init(void)
{
	GPIO_Handle_t GpioBtn = {0};

	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	GPIO_Init(&GpioBtn);
}

/* NOTIFY PIN PB1 */
void Notify_Init(void)
{
	GPIO_Handle_t gpio = {0};

	gpio.pGPIOx = GPIOB;
	gpio.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	GPIO_Init(&gpio);
}





void notify_master(void)
{
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, GPIO_PIN_RESET);
	delay_50us();
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, GPIO_PIN_SET); // falling edge
}





int main(void)
{
	SPI2_GPIOInit();
	SPI2_SlaveInit();
	Button_Init();
	Notify_Init();

	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, GPIO_PIN_SET);

	SPI_PeripheralControl(SPI2, ENABLE);

	uint8_t dummy;
	uint32_t i = 0;

	// Wait for NSS LOW (optional but recommended)
//	while(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12));

	while(1)
	{
		// wait for button press
		if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == GPIO_PIN_RESET)
		{
//			delay_50us();

//			notify_master(); // trigger master
			delay();

			i = 0;

			// 1. Pre-load FIRST byte
			SPI_SendData(SPI2, (uint8_t *)&msg[i], 1);
			i++;

			notify_master(); // trigger master

			while(i < msg_len)
			{
			    // 2. Wait for clock (master dummy)
			    SPI_ReceiveData(SPI2, &dummy, 1);

			    // 3. Send next byte
			    SPI_SendData(SPI2, (uint8_t *)&msg[i], 1);

			    i++;
			}

			// 4. Clear last RXNE (very important)
			SPI_ReceiveData(SPI2, &dummy, 1);

			// wait till SPI not busy
			while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY));
//			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, GPIO_PIN_SET);
		}
	}
	return 0;
}














