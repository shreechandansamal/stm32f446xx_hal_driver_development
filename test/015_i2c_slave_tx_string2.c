/*
 * 015_i2c_slave_tx_string2.c
 *
 *  Created on: Jun 4, 2026
 *      Author: chandan
 *
 *      I2C1 Pin Configuration
 *      ======================
 *      SDA --> PB7
 *      SCL --> PB6
 *
 *      ALT function mode: AF4
 *
 *      Task: I2C Master(Arduino) and I2C Slave(STM32) communication (communicate more than 32 byte data)
 *      Arduion Master should read an display data from STM32 Slave connected. First master has to get the length of the data
 *      from the slave to read subsequent data from the slave.
 *      1. Use I2C SCL = 100KHz(Standard Mode)
 *      2. Use external pull up resistors(3.3Kohm or 4.7Kohm) for SDA and SCL lines or internal pull up can be taken care.
 *
 *
 *      Note: Slave code is only written in Interrupt Mode so to use the API and application callbacks we need to first
 *      enable the Interrupt Control Bits by calling the "I2C_SlaveEnableDisableCallbackEvents" and without calling that
 *      interrupt will not be generated for I2C EV or ERR for slave.
 *
 */


#include <string.h>
#include <stdio.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_i2c_driver.h"


#define SLAVE_ADDR      0x38
#define MY_ADDR 		SLAVE_ADDR

uint32_t data_len = 0U;

I2C_Handle_t I2C1Handle = {0};


//transmit buffer
uint8_t tx_buf[] = "HiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHi...123456";

uint8_t commandcode = 0U;

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
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_EV,
						  NVIC_IRQ_PRI1);
	I2C_IRQPriorityConfig(IRQ_NO_I2C1_ER,
						  NVIC_IRQ_PRI0);
	//I2C IRQ Configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV,
						   ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER,
						   ENABLE);

}


void I2C1_Init(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM; //100KHz

	I2C_Init(&I2C1Handle);
}




int main(void)
{
	data_len = strlen((char*)tx_buf);

	I2C1_GPIOInit();
	I2C1_Init();

	I2C_SlaveEnableDisableCallbackEvents(I2C1,
										 ENABLE);

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1,
						  ENABLE);

	//enable ACK, this can not be set before PE = 1(enable i2c peripheral)
	I2C_AckControl(I2C1,
				   I2C_ACK_ENABLE);

	while(1);

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


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,
										 uint8_t AppEvent)
{
	/* static variables are kind of global variable
	 * so, memory for these variable will not allocated
	 * in the stack, instead of memory will allocate in the
	 * global space.
	 * this function has not while loop so this will not stay
	 * run forever but still even if the function exits
	 * memory for these variables will not be deallocated
	 * and we can access it from this function
	 *
	 * Note: but only thing is we can't access these variables
	 * from other function, even though they are in global
	 * space we can't access these variables from other
	 * function because of used static keyword.
	 * so, static mean private to that function but memory
	 * allocated is actually in global space.
	 */
	static uint32_t Cnt = 0U;
	static uint32_t w_ptr = 0U;

	if(AppEvent == I2C_EV_DATA_REQ)
	{
		//Master wants some data, slave has to send it
		if(commandcode == 0x51U)
		{
			//Here we are sending 4 bytes of length information
			I2C_SlaveSendData(I2C1,
							  ((data_len >> ((Cnt % 4U) * 8U)) & 0xFFU));
		    Cnt++;
		}
		else if(commandcode == 0x52U)
		{
			//sending Tx_buf contents indexed by w_ptr variable
			I2C_SlaveSendData(I2C1,
							  tx_buf[w_ptr++]);
		}
	}
	else if(AppEvent == I2C_EV_DATA_RCV)
	{
		//Data is waiting for the slave to read, slave has to read it
		commandcode = I2C_SlaveReceiveData(pI2CHandle->pI2Cx);
	}
	else if(AppEvent == I2C_ER_AF)
	{
		/* This happens only during slave txing.
		 * Master has sent the NACK. so slave should understand that
		 * master doesn't need more data
		 */
		//if the current active code is 0x52 then don't invalidate
		if(!(commandcode == 0x52U))
		{
			commandcode = 0XFFU;
		}
		else
		{
			// ADDED LINE: this line is needed in order not to lose one byte per I2C transaction
			// since we write one byte to DR register but we receive NACK and that byte is never transmitted
			w_ptr--;
		}
		//reset the cnt variable because its end of transmission
		Cnt = 0U;

		//Slave concludes it sent all the bytes when w_ptr reaches data_len
		if(w_ptr >= (data_len))
		{
			w_ptr = 0U;
			commandcode = 0xFFU;
		}

	}
	else if(AppEvent == I2C_EV_STOP)
	{
		/* This happens only during slave reception.
		 * Master has ended the I2C communication with the slave.
		 */
		Cnt = 0U;
	}
}



















