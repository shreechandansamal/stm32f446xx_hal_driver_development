/*
 * AHT10_I2C.c
 *
 *  Created on: May 27, 2026
 *      Author: chandan
 *
 *      I2C1 Pin Configuration
 *      ======================
 *      SDA --> PB7
 *      SCL --> PB6
 *
 *      ALT function mode: AF4
 *
 *      Task:
 *      1. Read the AHT10 Sensor value with Repeated Start.
 *      2. use the formula of Relative humidity conversion and Temperature conversion
 *         to get the actual Humidity and Temperature.
 */

#include <string.h>
#include <stdio.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_i2c_driver.h"

#define SCB_CPACR   (*(volatile uint32_t*)0xE000ED88UL) //Co-processor Access Control Register FPU

#define MY_ADDR 		0x61U
#define SLAVE_ADDR      0x38U //AHT10 address

/* AHT10 Commands */
#define AHT10_CMD_INIT             0xE1U
#define AHT10_CMD_TRIGGER          0xACU

/* AHT10 Command Arguments */
#define AHT10_INIT_ARG1            0x08U
#define AHT10_INIT_ARG2            0x00U

#define AHT10_TRIGGER_ARG1         0x33U
#define AHT10_TRIGGER_ARG2         0x00U

/* AHT10 Status Bits */
#define AHT10_STATUS_BUSY_BIT      7U
#define AHT10_STATUS_CAL_BIT       3U

/* Delay Calibration */
#define DELAY_10MS_COUNT           12550U
#define DELAY_DEBOUNCE_COUNT       (500000U/2U)

/* Timeout */
#define AHT10_BUSY_TIMEOUT         1000U


I2C_Handle_t I2C1Handle = {0};


typedef struct
{
    float Humidity;
    float Temperature;

} AHT10_Data_t;



//software delay
static void delay_loop(volatile uint32_t count)
{
    while(count--);
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

static AHT10_Data_t AHT10_ExtractData(uint8_t *RxBuffer)
{
    AHT10_Data_t SensorData;

    uint32_t rawHumidity;
    uint32_t rawTemperature;

    /* Extract raw humidity */
    rawHumidity =
            ((uint32_t)RxBuffer[1] << 12)
          | ((uint32_t)RxBuffer[2] << 4)
          | ((RxBuffer[3] & 0xF0) >> 4);

    /* Convert humidity */
    SensorData.Humidity =
            ((float)rawHumidity / 1048576.0f) * 100.0f;

    /* Extract raw temperature */
    rawTemperature =
            ((uint32_t)(RxBuffer[3] & 0x0F) << 16)
          | ((uint32_t)RxBuffer[4] << 8)
          | RxBuffer[5];

    /* Convert temperature */
    SensorData.Temperature =
            ((float)rawTemperature / 1048576.0f) * 200.0f - 50.0f;

    return SensorData;
}


int main(void)
{
	SCB_CPACR |= ((3UL << 20U) | (3UL << 22U)); //set CP10 and CP11 for FPU

	uint8_t BusyStatus = 0U;
	uint32_t timeout = 0U;
	int hum_int = 0U;
	int hum_frac = 0U;
	int temp_int = 0U;
	int temp_frac = 0U;
	uint8_t RSPInitCommandCode = 0U;
	uint8_t RSPTrigCommandCode[6] = {0};
	uint8_t InitCommandCode[3] =
    {
        AHT10_CMD_INIT,
        AHT10_INIT_ARG1,
        AHT10_INIT_ARG2
    };
	uint8_t TrigCommandCode[3] =
    {
        AHT10_CMD_TRIGGER,
        AHT10_TRIGGER_ARG1,
        AHT10_TRIGGER_ARG2
    };

	AHT10_Data_t AHT10Data;

//	uint8_t len;

	Onboard_ButtonInit();
	I2C1_GPIOInit();
	I2C1_Init();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1,
						  ENABLE);

	//enable ACK, this can not be set before PE = 1(enable i2c peripheral)
	I2C_AckControl(I2C1,
				   I2C_ACK_ENABLE);

	while(1)
	{
		//wait till button pressed
		while(GPIO_ReadFromInputPin(GPIOC,
									GPIO_PIN_NO_13));

		delay_loop(DELAY_DEBOUNCE_COUNT);//for de-bounce



        /*************************************************************
         * AHT10 Initialization
         *************************************************************/
		//1. send InitCommandCode
		I2C_MasterSendData(&I2C1Handle,
						   InitCommandCode,
						   3,
						   SLAVE_ADDR,
						   I2C_SR_DISABLE);

		delay_loop(DELAY_10MS_COUNT);

		//receive the response for InitCommandCode
		I2C_MasterReceiveData(&I2C1Handle,
							  &RSPInitCommandCode,
							  1,
							  SLAVE_ADDR,
							  I2C_SR_DISABLE);

        /* Check Calibration Status */
        if(!(RSPInitCommandCode & (1U << AHT10_STATUS_CAL_BIT)))
        {
            printf("AHT10 Initialization Failed\r\n");
            continue;
        }
        else
        {
        	printf("AHT10 Initialization Success\r\n");
        }




        /*************************************************************
         * Trigger Measurement
         *************************************************************/
		//2. send TrigCommandCode
		I2C_MasterSendData(&I2C1Handle,
				           TrigCommandCode,
						   3,
						   SLAVE_ADDR,
						   I2C_SR_DISABLE);

//		delay_us(95000); //~76ms(below do while is better approach than this delay)

		timeout = AHT10_BUSY_TIMEOUT;

		do
		{
		    I2C_MasterReceiveData(&I2C1Handle,
		                          &BusyStatus,
		                          1,
		                          SLAVE_ADDR,
		                          I2C_SR_DISABLE);

		    timeout--;

		} while((BusyStatus & (1U << AHT10_STATUS_BUSY_BIT)) && timeout); /* test the busy bit only,
																			 till test got 0 it will
																			 never break the while*/
        if(timeout == 0U)
        {
            printf("AHT10 Busy Timeout\r\n");
            continue;
        }

		//receive the response for TrigCommandCode
		I2C_MasterReceiveData(&I2C1Handle,
							  RSPTrigCommandCode,
							  6,
							  SLAVE_ADDR,
							  I2C_SR_DISABLE);

		AHT10Data = AHT10_ExtractData(RSPTrigCommandCode);



		hum_int  = (int)AHT10Data.Humidity;
		hum_frac = (int)((AHT10Data.Humidity - hum_int) * 100);

		if(hum_frac < 0)
		{
		    hum_frac = -hum_frac;
		}

		printf("Humidity = %d.%02d %%\r\n",
		       hum_int,
		       hum_frac);

		temp_int  = (int)AHT10Data.Temperature;
		temp_frac = (int)((AHT10Data.Temperature - temp_int) * 100);

		if(temp_frac < 0)
		{
		    temp_frac = -temp_frac;
		}

		printf("Temperature = %d.%02d C\r\n",
		       temp_int,
		       temp_frac);

		printf("--------------------------------\r\n");


	}
	return 0;
}


