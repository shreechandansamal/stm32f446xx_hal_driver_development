/*
 * rtc_lcd.c
 *
 *  Created on: Jun 16, 2026
 *      Author: chandan
 */

#include <stdio.h>
#include "rtc.h"
#include "board_pins.h"

#define SYSTICK_TIM_CLK 16000000UL

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014UL;
	uint32_t *pSCSR = (uint32_t*)0xE000E010UL;

    /* calculation of reload value */
    uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1U;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFFUL);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1U << 1U); //Enables SysTick exception request:
    *pSCSR |= ( 1U << 2U);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1U << 0U); //enables the counter

}




void number_to_string(uint8_t num, char* buf)
{
	if(num < 10)
	{
		buf[0] = '0'; //e.g. num = 7, it would be 07
		buf[1] = num + 48; //add 48 to he number gives the ascii char of that number
	}
	else if(num >= 10 && num < 99)
	{
		buf[0] = (num / 10) + 48; //convert ascii to the tens place
		buf[1] = (num % 10) + 48; //convert ascii to the unit place
	}
}


// hh:mm:ss
char* time_to_string(RTC_time_t *rtc_time)
{
	static char buf[9]; // made buf as static neither while return buf cause dangling pointer

	buf[2] = ':';
	buf[5] = ':';

	number_to_string(rtc_time->hours, buf);
	number_to_string(rtc_time->minutes, &buf[3]);
	number_to_string(rtc_time->seconds, &buf[6]);

	buf[8] = '\0';

	return buf;
}


// dd/mm/yy
char* date_to_string(RTC_date_t *rtc_date)
{
	static char buf[9];

	buf[2] = '/';
	buf[5] = '/';

	number_to_string(rtc_date->date, buf);
	number_to_string(rtc_date->month, &buf[3]);
	number_to_string(rtc_date->year, &buf[6]);

	buf[8] = '\0';

	return buf;
}


char* get_day_of_week(uint8_t i)
{
	char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	return days[i - 1];
}








int main(void)
{
	RTC_time_t rtc_time;
	RTC_date_t rtc_date;


	printf("RTC Test\n");

	if(ds1307_init())
	{
		printf("RTC init has failed");
		while(1);
	}

	init_systick_timer(1);

	rtc_date.date = 16;
	rtc_date.day = TUESDAY;
	rtc_date.month = JUNE;
	rtc_date.year = 26; //can't write full year e.g. 2026, follow ds, 00-99

	rtc_time.hours = 9;
	rtc_time.minutes = 50;
	rtc_time.seconds = 1;
	rtc_time.time_format = TIME_FORMAT_12HRS_PM;

	ds1307_set_current_date(&rtc_date);
	ds1307_set_current_time(&rtc_time);

	ds1307_get_current_date(&rtc_date);
	ds1307_get_current_time(&rtc_time);

	char *am_pm;
	if(rtc_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (rtc_time.time_format) ? "PM" : "AM";
		printf("Current time = %s %s\n", time_to_string(&rtc_time), am_pm); //e.g. 09:56:40 PM

	}
	else
	{
		printf("Current time = %s\n", time_to_string(&rtc_time)); //e.g. 21:56:40
	}

	//date e.g. 16/06/26 <Tuesday>
	printf("Current date = %s <%s>\n", date_to_string(&rtc_date), get_day_of_week(rtc_date.day));

	while(1);

	return 0;
}


void SysTick_Handler(void)
{
	RTC_time_t rtc_time;
	RTC_date_t rtc_date;

	ds1307_get_current_time(&rtc_time);

	char *am_pm;
	if(rtc_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (rtc_time.time_format) ? "PM" : "AM";
		printf("Current time = %s %s\n", time_to_string(&rtc_time), am_pm); //e.g. 09:56:40 PM

	}
	else
	{
		printf("Current time = %s\n", time_to_string(&rtc_time)); //e.g. 21:56:40
	}

	ds1307_get_current_date(&rtc_date);
	//date e.g. 16/06/26 <Tuesday>
	printf("Current date = %s <%s>\n", date_to_string(&rtc_date), get_day_of_week(rtc_date.day));
}

