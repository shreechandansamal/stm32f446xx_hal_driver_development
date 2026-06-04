/*
 * 013_i2c_master_rx_testing_it.c
 *
 *  Created on: May 31, 2026
 *      Author: chandan
 *
 *      I2C1 Pin Configuration
 *      ======================
 *      SDA --> PB7
 *      SCL --> PB6
 *
 *      ALT function mode: AF4
 *
 *      Task: I2C Master(STM32) and I2C Slave(Arduino) communication
 *      (it will same as we did for test 012, but this time we will use  INTERRUPT  based API)
 *      When button on the master is pressed, master should read and display data from Arduino Slave connected.
 *      First master has to get the length of the data from the slave to read subsequent data from the slave.
 *      1. Use I2C SCL = 100KHz(Standard Mode)
 *      2. Use external pull up resistors(3.3Kohm or 4.7Kohm) for SDA and SCL lines.
 *
 *      Procedure to read the data from Arduino Slave:
 *      1. Master sends command code 0x51 to read the length(1Byte) of the data from the slave.
 *      2. Master sends command code 0x52 to read the complete data from the slave.
 *      3. After that Master has to display that data using semi-hosting or ITM console.
 *
 */

#include <string.h>
#include <stdio.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_i2c_driver.h"

#define MY_ADDR 		0x61
#define SLAVE_ADDR      0x38 //address of the connected slave into master

I2C_Handle_t I2C1Handle = {0};

//flag variable
uint8_t rxCompltFlag = RESET;
uint8_t buttonPressFlag = RESET;


//receive buffer(e.g. 32 bytes
uint8_t rcv_buf[32];


//software delay
void delay (void)
{
	for(uint32_t i = 0; i < 500000/2; i++);
}


void I2C1_GPIOInit(void)
{
	GPIO_Handle_t I2C1Pins = {0};
	I2C1Pins.pGPIOx = GPIOB;
	I2C1Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2C1Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2C1Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	I2C1Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF4;
	I2C1Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SDA
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&I2C1Pins);

	//SCL
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_Init(&I2C1Pins);

	//I2C IRQ Priority Configuration
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_EV, NVIC_IRQ_PRI1);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_ER, NVIC_IRQ_PRI0);
	//I2C IRQ Configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

}


void I2C1_Init(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM; //100KHz

	I2C_Init(&I2C1Handle);
}



void Onboard_ButtonInit(void)
{
	GPIO_Handle_t GpioBtn = {0};

	//Button Configuration
	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	GPIO_Init(&GpioBtn);

	//EXTI IRQ Priority Configuration
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI15_10, NVIC_IRQ_PRI15);
	//EXTI IRQ Configuration
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI15_10, ENABLE);
}


int main(void)
{
	uint8_t commandcode;
	uint8_t len;

	Onboard_ButtonInit();
	I2C1_GPIOInit();
	I2C1_Init();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1,ENABLE);

	//enable ACK, this can not be set before PE = 1(enable i2c peripheral)
	I2C_AckControl(I2C1, I2C_ACK_ENABLE);

	while(1)
	{
		//wait till button pressed
//		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		while(buttonPressFlag == RESET);
		delay();//for de-bounce

		//send first command code: 0x51
		commandcode = 0x51;
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_SR_ENABLE) != I2C_READY);
		//receive the response for 0x51 (length of complete data)
		while(I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_SR_ENABLE) != I2C_READY);

		//send second command code: 0x52
		commandcode = 0x52;
		while(I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_SR_ENABLE) != I2C_READY);
		//receive the response for 0x52 (read complete data)
		while(I2C_MasterReceiveDataIT(&I2C1Handle, rcv_buf, len, SLAVE_ADDR, I2C_SR_DISABLE) != I2C_READY);

		rxCompltFlag = RESET;

		/*this rcv_buf will be not terminated by null character because
		  at the time of transmission we actually don't receive the null
		  character from the slave but if we use %s here then rcv_buf has
		  to be terminated with null character, so need to give null
		  character at the end. by the help of length we can put the null
		  character at the end.*/
		while(rxCompltFlag != SET){} /* rxCompltFlag got SET while AppEvent == I2C_EV_RX_CMPLT
									  at the I2C_ApplicationEventCallback */
			rcv_buf[len+1] = '\0';
			printf("Received data from slave: %s",rcv_buf);
			rxCompltFlag = RESET;

			buttonPressFlag = RESET;
	}


	return 0;
}



//EV ISR Handler
void I2C1_EV_IRQHandler()
{
	I2C_EV_IRQHandling(&I2C1Handle);
}

//ER ISR Handler
void I2C1_ER_IRQHandler()
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

//EXTI ISR Handler
void EXTI15_10_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_PIN_NO_13);
	buttonPressFlag = SET;

}


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,
										 uint8_t AppEvent)
{
	if(AppEvent == I2C_EV_TX_CMPLT)
	{
		printf("Tx is Completed Successfully!\n");
	}
	else if(AppEvent == I2C_EV_RX_CMPLT)
	{
		printf("Rx is Completed Successfully!\n");
		rxCompltFlag = SET;
	}
	else if(AppEvent == I2C_EV_STOP)
	{
		printf("STOPF triggered!\n");
	}
	else if(AppEvent == I2C_ER_BERR)
	{

	}
	else if(AppEvent == I2C_ER_ARLO)
	{

	}
	else if(AppEvent == I2C_ER_AF)
	{
		printf("Error: Ack failure!\n");
		/* In master ack failure happens when slave fails
		 * to send ack for the byte sent from the master,
		 * Ack failure happens in the case of master then
		 * we can conclude something like
		 * a. slave device might have removed from the bus
		 * b. slave device might have gone bad
		 * c. slave may not want more data
		 *
		 * so, master should close the communication and
		 * shouldn't send more data to the slave */
		I2C_CloseTransmission(pI2CHandle);

		/* as we are not generating STOP condition in the
		 * I2C_CloseTransmission so we can call the STOP
		 * condition api*/
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		/* hang in infinite loop,
		 * here to doesn't allow other code to execute */
		while(1);

	}
	else if(AppEvent == I2C_ER_OVR)
	{

	}
	else if(AppEvent == I2C_ER_TIMEOUT)
	{

	}
}












