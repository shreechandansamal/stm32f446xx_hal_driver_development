/*
 * rtc.c
 *
 *  Created on: Jun 16, 2026
 *      Author: chandan
 */

#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "board_pins.h"



/******************private helper function section**********************/
static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_config(void);
static void ds1307_write(uint8_t value,
			 	 	 	 uint8_t reg_addr);
static uint8_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t value);
static uint8_t bcd_to_binary(uint8_t value);

/***********************************************************************/

/*********************global variable section***************************/
I2C_Handle_t g_ds1307I2CHandle = {0};

/***********************************************************************/




/*********************************************************************
 * @fn                - ds1307_init
 *
 * @brief             - Initialize the ds1307 rtc chip according to
 *                      the user-supplied configuration parameters.
 *
 * @param[in]         - none
 *
 * @return            - state of 7th bit or clock halt(CH) bit
 * 						return 1: CH = 1; init failed
 * 						return 0: CH = 0; init success
 *
 * @Note              -
 *
 */
uint8_t ds1307_init(void)
{
	//1. initialize the i2c pins
	ds1307_i2c_pin_config();

	//2. initialize the i2c peripheral
	ds1307_i2c_config();

	//3. enable the I2C peripheral
	I2C_PeripheralControl(DS1307_I2C_PERI,
						  ENABLE);

	//4. make clock halt as 0 to enable the crystal of the rtc
	ds1307_write(0x00,
				 DS1307_ADDR_SEC);

	//5. read back clock halt bit(if 0 means init success)
	uint8_t clock_state = ds1307_read(DS1307_ADDR_SEC);

	return (clock_state >> 7) & 0x1;
}



/*********************************************************************
 * @fn                - ds1307_set_current_time
 *
 * @brief             - set current time
 *
 * @param[in]         - rtc_time
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void ds1307_set_current_time(RTC_time_t *rtc_time)
{
	/*seconds*/
	uint8_t seconds, hours;
	seconds = binary_to_bcd(rtc_time->seconds);
	seconds &= ~(1 << 7); //the 7th bit / CH bit should be clear
	ds1307_write(seconds, DS1307_ADDR_SEC);

	/*minutes*/
	ds1307_write(binary_to_bcd(rtc_time->minutes), DS1307_ADDR_MIN);

	/*hours*/
	hours = binary_to_bcd(rtc_time->hours);

	if(rtc_time->time_format == TIME_FORMAT_24HRS)
	{
		hours &= ~(1 << 6); //6th bit 0 means set 24 format
	}
	else
	{
		hours |= (1 << 6); //6th bit 1 means set 12 format
		hours = (rtc_time->time_format == TIME_FORMAT_12HRS_PM) ? hours | (1 << 5) : hours & ~(1 << 5);
	}
	ds1307_write(hours, DS1307_ADDR_HRS);

}



/*********************************************************************
 * @fn                - ds1307_set_current_date
 *
 * @brief             - set current date
 *
 * @param[in]         - rtc_date
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void ds1307_set_current_date(RTC_date_t *rtc_date)
{
	ds1307_write(binary_to_bcd(rtc_date->date),
							   DS1307_ADDR_DATE);
	ds1307_write(binary_to_bcd(rtc_date->day),
							   DS1307_ADDR_DAY);
	ds1307_write(binary_to_bcd(rtc_date->month),
							   DS1307_ADDR_MONTH);
	ds1307_write(binary_to_bcd(rtc_date->year),
							   DS1307_ADDR_YEAR);

}



/*********************************************************************
 * @fn                - ds1307_get_current_time
 *
 * @brief             - get current time
 *
 * @param[in]         - rtc_time
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void ds1307_get_current_time(RTC_time_t *rtc_time)
{
	/*seconds*/
	uint8_t seconds;
	seconds = ds1307_read(DS1307_ADDR_SEC);
	seconds &= ~(1 << 7); //the 7th bit / CH bit should be clear
	rtc_time->seconds = bcd_to_binary(seconds);

	/*minutes*/
	rtc_time->minutes = bcd_to_binary(ds1307_read(DS1307_ADDR_MIN));

	/*hours*/
	uint8_t hours;
	hours = ds1307_read(DS1307_ADDR_HRS);

	if(hours & (1 << 6))
	{
		/*12hrs format*/
		rtc_time->time_format = !((hours & (1 << 5)) == 0); // !1 = 0(AM), !0 = 1(PM)
		hours &= ~(0x3 << 5); /* clear 6 and 5, neither these bit will add to the hours
		  	  	  	  	  	   * the result will be wrong */
	}
	else
	{
		/*24hrs format*/
		rtc_time->time_format = TIME_FORMAT_24HRS;

	}
	rtc_time->hours = bcd_to_binary(hours);
}



/*********************************************************************
 * @fn                - ds1307_get_current_date
 *
 * @brief             - get current date
 *
 * @param[in]         - rtc_date
 *
 * @return            - none
 *
 * @Note              -
 *
 */
void ds1307_get_current_date(RTC_date_t *rtc_date)
{
	rtc_date->day =  bcd_to_binary(ds1307_read(DS1307_ADDR_DAY));
	rtc_date->date = bcd_to_binary(ds1307_read(DS1307_ADDR_DATE));
	rtc_date->month = bcd_to_binary(ds1307_read(DS1307_ADDR_MONTH));
	rtc_date->year = bcd_to_binary(ds1307_read(DS1307_ADDR_YEAR));
}


/*********************************************************************
 * @fn                - ds1307_i2c_pin_config
 *
 * @brief             - initialize the i2c pins
 *
 * @param[in]         - none
 *
 * @return            - none
 *
 * @Note              - helper function
 *
 */
static void ds1307_i2c_pin_config(void)
{
	GPIO_Handle_t sda_pin, scl_pin = {0};

	/*
	 * I2C1_SCL --> PB6
	 * I2C1_SDA --> PB7
	 */
	sda_pin.pGPIOx = DS1307_I2C_PORT;
	sda_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	sda_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	sda_pin.GPIO_PinConfig.GPIO_PinAltFunMode = DS1307_I2C_SDA_ALT_MODE;
	sda_pin.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	sda_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	sda_pin.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_PIN;
	GPIO_Init(&sda_pin);

	scl_pin.pGPIOx = DS1307_I2C_PORT;
	scl_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	scl_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	scl_pin.GPIO_PinConfig.GPIO_PinAltFunMode = DS1307_I2C_SCL_ALT_MODE;
	scl_pin.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	scl_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	scl_pin.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_PIN;
	GPIO_Init(&scl_pin);

}


/*********************************************************************
 * @fn                - ds1307_i2c_config
 *
 * @brief             - initialize the i2c configuration
 *
 * @param[in]         - none
 *
 * @return            - none
 *
 * @Note              - helper function
 *
 */
static void ds1307_i2c_config(void)
{
	g_ds1307I2CHandle.pI2Cx = DS1307_I2C_PERI;
	g_ds1307I2CHandle.I2C_Config.I2C_ACKControl = DS1307_I2C_ACK_CTRL;
	g_ds1307I2CHandle.I2C_Config.I2C_SCLSpeed = DS1307_I2C_SPEED; //100KHz
	I2C_Init(&g_ds1307I2CHandle);
}



/*********************************************************************
 * @fn                - ds1307_write
 *
 * @brief             - write the value to the register of rtc
 *
 * @param[in]         - value
 * @param[in]         - reg_addr
 *
 * @return            - none
 *
 * @Note              - helper function
 *
 */
static void ds1307_write(uint8_t value,
			 	 	 	 uint8_t reg_addr)
{
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = value;

	/*send I2C data*/
	I2C_MasterSendData(&g_ds1307I2CHandle,
					   tx,
					   2,
					   DS1307_I2C_ADDRESS,
					   I2C_SR_DISABLE);

}



/*********************************************************************
 * @fn                - ds1307_read
 *
 * @brief             - read the value from the register of rtc
 *
 * @param[in]         - reg_addr
 *
 * @return            - none
 *
 * @Note              - helper function
 *
 */
static uint8_t ds1307_read(uint8_t reg_addr)
{
	uint8_t data;

	I2C_MasterSendData(&g_ds1307I2CHandle,
					   &reg_addr,
					   1,
					   DS1307_I2C_ADDRESS,
					   I2C_SR_DISABLE);

	I2C_MasterReceiveData(&g_ds1307I2CHandle,
						  &data,
						  1,
						  DS1307_I2C_ADDRESS,
						  I2C_SR_DISABLE);

	return data;
}



/*********************************************************************
 * @fn                - binary_to_bcd
 *
 * @brief             - convert binary value to bcd value
 *
 * @param[in]         - value: binary value
 *
 * @return            - bcd value
 *
 * @Note              - helper function
 *
 */
static uint8_t binary_to_bcd(uint8_t value)
{
	uint8_t m , n, bcd;

	bcd = value;
	if(value >= 10)
	{
		m = value /10;
		n = value % 10;
		bcd = (m << 4) | n ;
	}

	return bcd;
}



/*********************************************************************
 * @fn                - bcd_to_binary
 *
 * @brief             - convert bcd to binary value
 *
 * @param[in]         - value: bcd value
 *
 * @return            - binary value
 *
 * @Note              - helper function
 *
 */
static uint8_t bcd_to_binary(uint8_t value)
{
	uint8_t m , n;

	m = (uint8_t) ((value >> 4 ) * 10);
	n =  value & (uint8_t)0x0F;
	return (m + n);
}








