/*
 * 009_master_tx_rx_fd_it.c
 *
 *  Created on: Apr 27, 2026
 *      Author: chandan
 *
 *
 * 		-This master application receives and prints the user message received from the Slave peripheral in SPI interrupt mode
 * 		 User sends the message through pressing the slave user button, then transmitter(slave) trigger the interrupt
 * 		 (High to Low) means slave have message and wants to transmit then receiver(master) comes to know that there is
 * 		 message pending at the transmitter side and then it produces clock on the SPI pin to fetch the data.
 * 		-Monitor the message received in the SWV itm data console.
 *
 *      SPI2 Pin Configuration
 *      ======================
 *      MOSI --> PB15
 *      MISO --> PB14
 *      SCLK --> PB13
 *      NSS  --> PB12
 *      ALT function mode: AF5
 *
 *      INT --> PB1
 *
 *
 */




#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"



#define MAX_LEN 500

char RcvBuff[MAX_LEN];

//volatile char ReadByte;
char ReadByte;

volatile uint8_t rcvStop = 0;

/*This flag will be set in the interrupt handler of the slave interrupt GPIO */
volatile uint8_t dataAvailable = 0;

//make it global to access other functions in this file
SPI_Handle_t SPI2handle = {0};




void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}



// GPIO pin initialization for SPI2
void SPI2_GPIOInit(void){
	GPIO_Handle_t SPI2Pins = {0};

	SPI2Pins.pGPIOx = GPIOB;
	SPI2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPI2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF5;
	SPI2Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
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



// SPI2 init (MASTER)
void SPI2_Init(void)
{
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV4;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI; //Hardware slave management enabled for NSS pin

	SPI_Init(&SPI2handle);
}



/*This function configures the gpio pin over which SPI peripheral issues data available interrupt */
void Slave_GPIO_InterruptPinInit(void)
{
	GPIO_Handle_t SPI2IntPin = {0};
//	memset(&spiIntPin,0,sizeof(spiIntPin)); // works same as GPIO_Handle_t spiIntPin = {0}

	//this is led gpio configuration
	SPI2IntPin.pGPIOx = GPIOB;
	SPI2IntPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	SPI2IntPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT; //edge detection, interrupt triggered on falling edge
	SPI2IntPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	SPI2IntPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_Init(&SPI2IntPin);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI1, NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI1, ENABLE); //Interrupt enable

}













int main(void)
{

	uint8_t dummy = 0xff;

	Slave_GPIO_InterruptPinInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInit();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Init();

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/
	SPI_SSOEConfig(SPI2, ENABLE);

	SPI_IRQInterruptConfig(IRQ_NO_SPI2, ENABLE);

	while(1){

		rcvStop = 0;

		while(!dataAvailable); //wait till data available interrupt from transmitter device(slave)

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI1, DISABLE);

		//enable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, ENABLE);


		while(!rcvStop)
		{
			/* fetch the data from the SPI peripheral byte by byte in interrupt mode */
			while ( SPI_SendDataIT(&SPI2handle, &dummy, 1) == SPI_BUSY_IN_TX);
			while ( SPI_ReceiveDataIT(&SPI2handle, (uint8_t *)&ReadByte, 1) == SPI_BUSY_IN_RX );
		}


		// confirm SPI is not busy
		while( SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY) );

		//Disable the SPI2 peripheral
		SPI_PeripheralControl(SPI2, DISABLE);

		printf("Rcvd data = %s\n",RcvBuff);

		dataAvailable = 0;

		GPIO_IRQInterruptConfig(IRQ_NO_EXTI1, ENABLE);


	}

	return 0;

}



/* Runs when a data byte is received from the peripheral over SPI*/
void SPI2_IRQHandler(void)
{

	SPI_IRQHandling(&SPI2handle);
}



void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEvent)
{
	static uint32_t i = 0;
	/* In the RX complete event , copy data in to rcv buffer . '\0' indicates end of message(rcvStop = 1) */
	if(AppEvent == SPI_EVENT_RX_CMPLT)
	{
				RcvBuff[i++] = ReadByte;
				if(ReadByte == '\0' || ( i == MAX_LEN)){
					rcvStop = 1;
					RcvBuff[i-1] = '\0';
					i = 0;
				}
	}

}

/* Slave data available interrupt handler */
void EXTI1_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_PIN_NO_1);
	dataAvailable = 1;
}










