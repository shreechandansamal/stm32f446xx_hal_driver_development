/*
 * 006_spi_tx_testing.c
 *
 *  Created on: Apr 18, 2026
 *      Author: chandan
 *
 *      Exercise:
 *      1. Test SPI_SendData API to send the string "Hello world" and use the below configurations
 *      a. use SPI Full duplex mode
 *      b. SPI-2 Master mode
 *      c. use DFF = 0
 *      d. use Software Slave Management(SSM = 1)
 *      e. SCK speed = 4MHz, fclk = 16MHz
 *
 *      SPI2 Pin Configuration
 *      ======================
 *      MOSI --> PB15
 *      MISO --> PB14  //No Rxing, So Disable it
 *      SCLK --> PB13
 *      NSS  --> PB12  //No Slave So Disable it(Enable the SSM as Software Slave Management)
 *      ALT function mode: AF5
 *
 *
 *      In this exercise master is not going to receive anything for the slave. so you may not configure the MISO pin.
 *
 *      Note.
 *      Slave doesn't know how many bytes of data master is going to send. so master first sends the number bytes info which
 *      slave is going to receive next.
 *
 */

#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"

//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


void SPI2_GPIOInit(void){
	GPIO_Handle_t SPI2Pins;

	SPI2Pins.pGPIOx = GPIOB;
	SPI2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPI2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF5;
	SPI2Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	SPI2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//MOSI
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI2Pins);

	//MISO (No Rxing, So Disable it)
//	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
//	GPIO_Init(&SPI2Pins);

	//SCLK
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI2Pins);

	//NSS (No Slave So Disable it)
//	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
//	GPIO_Init(&SPI2Pins);

}


void SPI2_Init(void){
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV4; //generates sclk of 4MHz
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN; //software slave management enabled for NSS pin

	SPI_Init(&SPI2Handle);

}



void GPIO_ButtonInit(void){
	GPIO_Handle_t GpioBtn;
	//Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;


	//Enable the Clock for GPIOC
//	GPIO_PeriClockControl(GPIOC, ENABLE);
	//Initialize the GPIO Button
	GPIO_Init(&GpioBtn);
}




int main(){

	char user_data[] = "Hello Team, I am 'That 32Bit Guy',who works for 32bit processor. I Love to Low Level. I Build Devices. I Help People to ease there life. Apart I love to read Books, Comics. And as well as I love Hardware Design and Mechanical Design. It's really fascinate to me to build the things. And I am really Happy to Share my Story, Thank U :)";

	GPIO_ButtonInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInit();

	//this function is used to initialize the SPI2 peripheral parameters
	SPI2_Init();

	//this makes NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI2, ENABLE);

	while(1){

		//wait till button pressed
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		delay();//for de-bounce

		//enable SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);// function returns 1 means SPI busy, 0 means not busy break loop

		//first send length information
		uint16_t dataLen = strlen(user_data);
		SPI_SendData(SPI2, (uint8_t *)&dataLen, 2);

		//to send data
		SPI_SendData(SPI2, (uint8_t *)user_data, dataLen); //passing base address of user_data = &user_data[0]

		//let confirm SPI2 is not BUSY
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY));

		//disable SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);

	}
	return 0;
}


