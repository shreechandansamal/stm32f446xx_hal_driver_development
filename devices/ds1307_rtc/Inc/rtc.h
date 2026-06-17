/*
 * rtc.h
 *
 *  Created on: Jun 16, 2026
 *      Author: chandan
 */

#ifndef DS1307_RTC_INC_RTC_H_
#define DS1307_RTC_INC_RTC_H_

/*salve address*/
#define DS1307_I2C_ADDRESS			0x68


/*register address*/
#define DS1307_ADDR_SEC				0x00
#define DS1307_ADDR_MIN				0x01
#define DS1307_ADDR_HRS				0x02
#define DS1307_ADDR_DAY				0x03
#define DS1307_ADDR_DATE			0x04
#define DS1307_ADDR_MONTH			0x05
#define DS1307_ADDR_YEAR			0x06


/*user-selectable configuration*/
typedef enum
{
    TIME_FORMAT_12HRS_AM,
    TIME_FORMAT_12HRS_PM,
    TIME_FORMAT_24HRS
}RTC_TimeFormat_t;

typedef enum
{
	SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
}RTC_Day_t;

typedef enum
{
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} RTC_Month_t;

/*hold time and date information*/
typedef struct
{
	uint8_t date;
	RTC_Month_t month;
	uint8_t year;
	RTC_Day_t day;
}RTC_date_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	RTC_TimeFormat_t time_format;
}RTC_time_t;





/*function prototypes*/

uint8_t ds1307_init(void);
void ds1307_set_current_time(RTC_time_t *rtc_time);
void ds1307_set_current_date(RTC_date_t *rtc_date);
void ds1307_get_current_time(RTC_time_t *rtc_time);
void ds1307_get_current_date(RTC_date_t *rtc_date);


#endif /* DS1307_RTC_INC_RTC_H_ */
