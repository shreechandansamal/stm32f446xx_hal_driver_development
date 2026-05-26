/*
 * 007_master_tx_rx_fd.c
 *
 *  Created on: Apr 19, 2026
 *      Author: chandan
 *
 *      Exercise: SPI Master and SPI Slave command & response based communication.
 *      when the user button on the master pressed, master send a command to the slave and slave responds as per the command
 *      implementation.
 *
 *      a. use SPI Full duplex mode
 *      b. SPI-2 Master mode
 *      c. use DFF = 0
 *      d. use Hardware Slave Management(SSM = 0)
 *      e. SCK speed = 4MHz, fclk = 16MHz
 *
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
 *      Commands Tx and Rx
 *      ==================
 *      Master------------Master sends a command----------->Slave
 *        |                                                   |
 *        |<-------Slave Response(ACK or NACK Byte)-----------| NACK 0xA5, ACK 0xF5
 *        |                                                   |
 *        IF ACK                                              |
 *        |----------One or More Command Arguments----------->|
 *        |                                                   |
 *        IF NACK                                         Take action
 *        |                                               according
 *        |--------Display Error Message                  to command
 *        |                                                   |
 *        |<------Slave Response for Data Read Command--------|
 *
 *     Command formats
 *     ===============
 *
 *     <command_code(1)>    <arg1>  <arg2>
 *
 *     1. CMD_LED_CTRL      <pin no>    <value>
 *     		                <pin no> digital pin number of the slave (1 byte)
 *     		                <value> 1 = ON, 0 = OFF (1byte)
 *     		                Slave Action: control the digital pin ON or OFF
 *     		                Slave returns: Nothing
 *
 *     2. CMD_SENSOR_READ   <analog pin number>
 *                          <analog pin number> Analog pin number of the slave
 *                          Slave Action: Slave should read the analog value of the supplied pin
 *                          Slave Returns: 1 byte of analog read value
 *
 *     3. CMD_LED_READ      <pin no>
 *                          <pin no> digital pin number of the slave
 *                          Slave Action: Read the status of the supplied pin number.
 *                          Slave Returns: 1 bytes of led status. 1 = ON, 0 = OFF
 *
 *     4. CMD_PRINT         <len>  <message>
 *                          <len> 1 byte of length information of the message to follow
 *                          <message> message of 'len' bytes
 *                          Slave Action: Receive the message and display via serial port
 *                          Slave Returns: Nothing
 *
 *     5. CMD_ID_READ
 *                         No arguments for this command
 *                         Slave Returns: 10bytes of board id string
 *
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"

//command codes (these are the slave command codes slave recognizes)
#define COMMAND_LED_CTRL      		0x50
#define COMMAND_SENSOR_READ      	0x51
#define COMMAND_LED_READ      		0x52
#define COMMAND_PRINT      			0x53
#define COMMAND_ID_READ      		0x54

#define LED_ON                      1
#define LED_OFF                     0

//analog pins of slaves
#define ANALOG_PIN0 	            0
#define ANALOG_PIN1 	            1
#define ANALOG_PIN2 	            2
#define ANALOG_PIN3 	            3
#define ANALOG_PIN4 	            4

//slave on-board led
#define LED_PIN                     5




//software delay
void delay (void){
	for(uint32_t i = 0; i < 500000/2; i++);
}


 // GPIO pin initialization for SPI2
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
void SPI2_Init(void){
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV4; //generates sclk of 4MHz
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI; //hardware slave management enabled for NSS pin

	SPI_Init(&SPI2Handle);

}

// LED GPIO
//void GPIO_LedInit(void){
//	GPIO_Handle_t GpioLed;

//	GpioLed.pGPIOx = GPIOA;
//	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
//	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
//	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
//
//	//Enable the Clock for GPIOC
//	GPIO_PeriClockControl(GPIOA, ENABLE);
//	//Initialize the GPIO Button
//	GPIO_Init(&GpioLed);
//}


void GPIO_ButtonInit(void){
	GPIO_Handle_t GpioBtn;
	//Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;


	//Enable the Clock for GPIOC
	GPIO_PeriClockControl(GPIOC, ENABLE);
	//Initialize the GPIO Button
	GPIO_Init(&GpioBtn);
}


uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{

	if(ackbyte == (uint8_t)0xF5)
	{
		//ack
		return 1;
	}
	//nack
	return 0;
}




int main(){

	uint8_t commandcode = COMMAND_LED_CTRL;
	uint8_t ackbyte;
	uint8_t tx_buf[2];
	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

	printf("SPI Master running...\n");

	GPIO_ButtonInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInit();

	//this function is used to initialize the SPI2 peripheral parameters
	SPI2_Init();

	printf("SPI Init. done\n");

//	//this makes NSS signal internally high and avoids MODF error
//	SPI_SSIConfig(SPI2, ENABLE); //commented because now we are using hw SSM no need SSI bit to high the signal internally

	/*
	 * making SSOE 1 does NSS output enable.
	 * the NSS pin is automatically managed by the hardware.
	 * i.e. when SPE=1, NSS will be pulled to low
	 * and NSS pin will be high when SPE=0
	 */
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1){

		//wait till button pressed
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		delay();//for de-bounce

		//enable SPI2 peripheral(SPE)
		SPI_PeripheralControl(SPI2, ENABLE);


		//1.CMD_LED_CTRL  	<pin no>     <value>

			//a.send command code
			SPI_SendData(SPI2, &commandcode, 1);

			//b.do dummy read to clear off the RXNE (SPI,master and slave sends 1 byte also receive 1 byte so need to clear)
			SPI_ReceiveData(SPI2,&dummy_read,1);

			//c.Send some dummy bits (1 byte) to fetch the response (ackbyte) from the slave
			SPI_SendData(SPI2, &dummy_write, 1);

			//d.read the ack byte received
			SPI_ReceiveData(SPI2,&ackbyte,1);

			//e.Verify ackbyte, if success send pin number and ON/OFF
			if( SPI_VerifyResponse(ackbyte))
			{
				tx_buf[0] = LED_PIN;
				tx_buf[1] = LED_ON;

				//send arguments
				SPI_SendData(SPI2,tx_buf,2);
				// dummy read
//				SPI_ReceiveData(SPI2,tx_buf,2);
				printf("COMMAND_LED_CTRL Executed from Master Side\n");
			}
			//end of COMMAND_LED_CTRL



			//2. CMD_SENOSR_READ   <analog pin number(1) >


			//wait till button pressed
			while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
			delay();//for de-bounce

			commandcode = COMMAND_SENSOR_READ;

			//send command
			SPI_SendData(SPI2,&commandcode,1);

			//do dummy read to clear off the RXNE
			SPI_ReceiveData(SPI2,&dummy_read,1);


			//Send some dummy byte to fetch the response from the slave
			SPI_SendData(SPI2,&dummy_write,1);

			//read the ack byte received
			SPI_ReceiveData(SPI2,&ackbyte,1);

			if( SPI_VerifyResponse(ackbyte))
			{
				tx_buf[0] = ANALOG_PIN0;

				//send arguments
				SPI_SendData(SPI2,tx_buf,1); //sending one byte of

				//do dummy read to clear off the RXNE
				SPI_ReceiveData(SPI2,&dummy_read,1);

				//insert some delay so that slave can ready with the data
				delay();

				//Send some dummy bits (1 byte) fetch the response from the slave
				SPI_SendData(SPI2,&dummy_write,1);

				uint8_t analog_read;
				SPI_ReceiveData(SPI2,&analog_read,1);
				printf("COMMAND_SENSOR_READ %d\n",analog_read);
			}



		//let confirm SPI2 is not BUSY
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY));

		//disable SPI2 peripheral(SPE)
		SPI_PeripheralControl(SPI2, DISABLE);

	}
	return 0;
}




















