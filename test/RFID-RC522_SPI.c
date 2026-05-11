/*
 * RFID-RC522_SPI.c
 *
 *  Created on: May 7, 2026
 *      Author: chandan
 */

#include <stdio.h>
#include "stm32f446xx.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"

#define RC522_COMMAND_REG        0x01
#define RC522_COMMIEN_REG        0x02
#define RC522_COMMIRQ_REG        0x04
#define RC522_ERROR_REG          0x06
#define RC522_FIFO_DATA_REG      0x09
#define RC522_FIFO_LEVEL_REG     0x0A
#define RC522_CONTROL_REG        0x0C
#define RC522_BIT_FRAMING_REG    0x0D
#define RC522_MODE_REG           0x11
#define RC522_TXCONTROL_REG      0x14
#define RC522_TX_AUTO_REG        0x15
#define RC522_TMODE_REG          0x2A
#define RC522_TPRESCALER_REG     0x2B
#define RC522_TRELOAD_REG_H      0x2C
#define RC522_TRELOAD_REG_L      0x2D
#define RC522_VERSION_REG        0x37

#define PCD_IDLE                 0x00
#define PCD_TRANSCEIVE           0x0C
#define PCD_SOFTRESET            0x0F

#define PICC_REQIDL              0x26
#define PICC_ANTICOLL            0x93

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i++);
}

void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPI2Pins;

	SPI2Pins.pGPIOx = GPIOB;
	SPI2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPI2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_MODE_ALTFN_AF5;
	SPI2Pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPI2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPI2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPI2Pins);

	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&SPI2Pins);

	SPI2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPI2Pins);
}

void SPI2_Init(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV64;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	SPI_Init(&SPI2Handle);
}

void RC522_NSS_Init(void)
{
	GPIO_Handle_t NSSPin;

	NSSPin.pGPIOx = GPIOB;
	NSSPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	NSSPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	NSSPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	NSSPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	NSSPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&NSSPin);

	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_SET);
}

void RC522_RST_Init(void)
{
	GPIO_Handle_t RSTPin;

	RSTPin.pGPIOx = GPIOB;
	RSTPin.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	RSTPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	RSTPin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	RSTPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	RSTPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&RSTPin);

	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, GPIO_PIN_SET);
}






//NSS pin LOW to active the pins of RC522
void RC522_Select(void)
{
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_RESET);
}

//NSS pin HIGH to inactive the pins of RC522
void RC522_Unselect(void)
{
	GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_SET);
}

void RC522_WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t tx[2];
	uint8_t rx[2];

	tx[0] = (reg << 1) & 0x7E;
	tx[1] = value;

	RC522_Select();

	SPI_TransmitReceiveData(SPI2, tx, rx, 2);

	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY));

	RC522_Unselect();
}

uint8_t RC522_ReadReg(uint8_t reg)
{
	uint8_t tx[2];
	uint8_t rx[2];

	tx[0] = ((reg << 1) & 0x7E) | 0x80;
	tx[1] = 0xFF;

	RC522_Select();

	SPI_TransmitReceiveData(SPI2, tx, rx, 2);

	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BUSY));

	RC522_Unselect();

	return rx[1];
}

void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp;

	tmp = RC522_ReadReg(reg);

	RC522_WriteReg(reg, tmp | mask);
}

void RC522_AntennaOn(void)
{
	uint8_t temp;

	temp = RC522_ReadReg(RC522_TXCONTROL_REG);

	if(!(temp & 0x03))
	{
		RC522_SetBitMask(RC522_TXCONTROL_REG, 0x03);
	}
}

void RC522_Reset(void)
{
	RC522_WriteReg(RC522_COMMAND_REG, PCD_SOFTRESET);
	delay();
}

void RC522_Init(void)
{
	RC522_Reset();

	RC522_WriteReg(RC522_TMODE_REG, 0x8D);
	RC522_WriteReg(RC522_TPRESCALER_REG, 0x3E);
	RC522_WriteReg(RC522_TRELOAD_REG_L, 30);
	RC522_WriteReg(RC522_TRELOAD_REG_H, 0);

	RC522_WriteReg(RC522_TX_AUTO_REG, 0x40);
	RC522_WriteReg(RC522_MODE_REG, 0x3D);

	RC522_AntennaOn();
}

uint8_t RC522_Request(uint8_t *tagType)
{
	uint8_t status;
	uint8_t irq;
	uint8_t len;

	RC522_WriteReg(RC522_BIT_FRAMING_REG, 0x07);

	RC522_WriteReg(RC522_COMMIRQ_REG, 0x7F);

	RC522_WriteReg(RC522_FIFO_LEVEL_REG, 0x80);

	RC522_WriteReg(RC522_FIFO_DATA_REG, PICC_REQIDL);

	RC522_WriteReg(RC522_COMMAND_REG, PCD_TRANSCEIVE);

	RC522_SetBitMask(RC522_BIT_FRAMING_REG, 0x80);

	delay();

	irq = RC522_ReadReg(RC522_COMMIRQ_REG);

	if(!(irq & 0x30))
	{
		return 1;
	}

	len = RC522_ReadReg(RC522_FIFO_LEVEL_REG);

	if(len != 2)
	{
		return 1;
	}

	tagType[0] = RC522_ReadReg(RC522_FIFO_DATA_REG);
	tagType[1] = RC522_ReadReg(RC522_FIFO_DATA_REG);

	status = 0;

	return status;
}

uint8_t RC522_Anticollision(uint8_t *uid)
{
	uint8_t i;
	uint8_t check = 0;

	RC522_WriteReg(RC522_BIT_FRAMING_REG, 0x00);

	RC522_WriteReg(RC522_COMMIRQ_REG, 0x7F);

	RC522_WriteReg(RC522_FIFO_LEVEL_REG, 0x80);

	RC522_WriteReg(RC522_FIFO_DATA_REG, PICC_ANTICOLL);
	RC522_WriteReg(RC522_FIFO_DATA_REG, 0x20);

	RC522_WriteReg(RC522_COMMAND_REG, PCD_TRANSCEIVE);

	RC522_SetBitMask(RC522_BIT_FRAMING_REG, 0x80);

	delay();

	if(!(RC522_ReadReg(RC522_COMMIRQ_REG) & 0x30))
	{
		return 1;
	}

	for(i = 0 ; i < 5 ; i++)
	{
		uid[i] = RC522_ReadReg(RC522_FIFO_DATA_REG);
	}

	for(i = 0 ; i < 4 ; i++)
	{
		check ^= uid[i];
	}

	if(check != uid[4])
	{
		return 1;
	}

	return 0;
}

int main(void)
{
	uint8_t version;
	uint8_t tagType[2];
	uint8_t uid[5];

	SPI2_GPIOInit();

	RC522_NSS_Init();

	RC522_RST_Init();

	SPI2_Init();

	SPI_SSIConfig(SPI2, ENABLE);

	SPI_PeripheralControl(SPI2, ENABLE);

	delay();

	RC522_Init();

	version = RC522_ReadReg(RC522_VERSION_REG);

	printf("RC522 Version Reg = 0x%x\n", version);

	while(1)
	{
		if(RC522_Request(tagType) == 0)
		{
			printf("Card Detected\n");

			if(RC522_Anticollision(uid) == 0)
			{
				printf("UID : ");

				for(uint8_t i = 0 ; i < 5 ; i++)
				{
					printf("%02X ", uid[i]);
				}

				printf("\n");
			}
		}

		delay();
	}
}
