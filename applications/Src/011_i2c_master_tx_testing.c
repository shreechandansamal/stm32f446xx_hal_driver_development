/*
 * 011_i2c_master_tx_testing.c
 *
 *  Created on: May 22, 2026
 *      Author: chandan
 *
 *      SPI2 Pin Configuration
 *      ======================
 *      SDA --> PB7
 *      SCL --> PB6
 *
 *      ALT function mode: AF4
 */


#include <string.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_i2c_driver.h"

#define MY_ADDR 		0x61
#define SLAVE_ADDR      0x38 //address of the connected slave into master

I2C_Handle_t I2C1Handle = {0};

//user data
char user_data[] = "Hello from I2C Master Side Tx";


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
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

	GPIO_Init(&GpioBtn);
}




int main(void)
{

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
		while(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13));
		delay();//for de-bounce

		//send user data
		I2C_MasterSendData(&I2C1Handle, (uint8_t *)user_data, strlen(user_data), SLAVE_ADDR);
	}
	return 0;
}


