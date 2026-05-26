/*
 * 008_slave_rx_tx_fd.c
 *
 *  Created on: Apr 19, 2026
 *      Author: chandan
 *
 *      Exercise: SPI Master and SPI Slave command & response based communication.
 *      when the user button on the master pressed, master send a command to the slave and slave responds as per the command
 *      implementation.
 *
 *      a. use SPI Full duplex mode
 *      b. SPI-2 Slave mode
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
	SPI2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//MOSI
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_Init(&SPI2Pins);

	//MISO
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_Init(&SPI2Pins);

	//SCLK
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_Init(&SPI2Pins);

	//NSS (IMPORTANT → pull-up)
	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_Init(&SPI2Pins);

}

// SPI2 init (SLAVE)
void SPI2_Init(void){
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_SLAVE;
//	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV4; //not needed because slave doesn't produce clock
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI; //hardware slave management enabled for NSS pin

	SPI_Init(&SPI2Handle);

}


// LED GPIO
void GPIO_LedInit(void){
	GPIO_Handle_t GpioLed;
	//Button Configuration
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;


	//Enable the Clock for GPIOC
//	GPIO_PeriClockControl(GPIOA, ENABLE);
	//Initialize the GPIO Button
	GPIO_Init(&GpioLed);
}


// BUTTON GPIO
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

//	uint8_t dummy_write = 0xff;
//	uint8_t dummy_read;

    uint8_t commandcode;
    uint8_t dummy_read;
    uint8_t ackbyte;
    uint8_t rx_buf[2];

	printf("SPI Slave running...\n");

	GPIO_LedInit();
	SPI2_GPIOInit();
	SPI2_Init();

	printf("SPI Init. done\n");

	//enable SPI2 peripheral(SPE)
	SPI_PeripheralControl(SPI2, ENABLE);

	while(1){

		//1.CMD_LED_CTRL  	<pin no>     <value>)

			//a.wait until command arrives
			SPI_ReceiveData(SPI2, &commandcode, 1);

			//b.verify ack or nack
        	if(commandcode == COMMAND_LED_CTRL)
        	    ackbyte = 0xF5; // ack
        	else
        	    ackbyte = 0xA5; // nack

        	//c.preload ack before master clocks next byte
        	SPI_SendData(SPI2, &ackbyte, 1);

        	//d.do dummy read to clear off the RXNE
        	SPI_ReceiveData(SPI2, &dummy_read, 1);

        	//e.wait for receive the pin number with value
            if(ackbyte == 0xF5)
            {
                SPI_ReceiveData(SPI2, rx_buf, 2);

                uint8_t pin = rx_buf[0];
                uint8_t value = rx_buf[1];

                if(value == LED_ON)
                    GPIO_WriteToOutputPin(GPIOA, pin, SET);
                else
                    GPIO_WriteToOutputPin(GPIOA, pin, RESET);
                printf("COMMAND_LED_CTRL Executed from Slave Side\n");
            }

		//end of COMMAND_LED_CTRL


        //2. CMD_SENOSR_READ   <analog pin number(1) >

            //a.RECEIVE COMMAND
            SPI_ReceiveData(SPI2, &commandcode, 1);

            //b.DECIDE ACK/NACK
            if(commandcode == COMMAND_SENSOR_READ)
                ackbyte = 0xF5;
            else
                ackbyte = 0xA5;

            //c.PRELOAD ACK (important)
            SPI_SendData(SPI2, &ackbyte, 1);

            //d.DUMMY READ (master clocks to get ACK)
            SPI_ReceiveData(SPI2, &dummy_read, 1);

            //e.IF ACK → RECEIVE ARGUMENT
            if(ackbyte == 0xF5)
            {
                //receive analog pin number
                SPI_ReceiveData(SPI2, rx_buf, 1);

                uint8_t analog_pin = rx_buf[0];

                //f.READ SENSOR (simulate for now)
                uint8_t sensor_value = 0;

                //Replace this with real ADC later
                if(analog_pin == 0)
                    sensor_value = 0x37;   // dummy value
                else if(analog_pin == 1)
                    sensor_value = 0x88;
                else
                    sensor_value = 0x11;

                //g.PRELOAD DATA (VERY IMPORTANT STEP)
                SPI_SendData(SPI2, &sensor_value, 1);

                //h.DUMMY READ (master will clock to fetch data)
                SPI_ReceiveData(SPI2, &dummy_read, 1);
            }




	}
	return 0;
}





