/*
 * DS3231Handler.h
 *
 *  Created on: 23 Apr 2021
 *      Author: abiyy
 */

#ifndef SOURCE_DS3231HANDLER_H_
#define SOURCE_DS3231HANDLER_H_


#include "I2CHandler.h"
#include "BCDS_Retcode.h"
#include "BCDS_CmdProcessor.h"
#include "XDK_TimeStamp.h"

typedef enum
{
	SUNDAY = 1,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
} RTC_DOW;


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
} RTC_Month;


typedef struct rtc_tm
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	RTC_DOW dow;
	uint8_t day;
	RTC_Month month;
	uint16_t year;
} RTC_Time;


typedef enum
{
	ONCE_PER_SECOND = 0x0F,
	SECONDS_MATCH = 0x0E,
	MINUTES_SECONDS_MATCH = 0x0C,
	HOURS_MINUTES_SECONDS_MATCH = 0x08,
	DATE_HOURS_MINUTES_SECONDS_MATCH = 0x0,
  	DAY_HOURS_MINUTES_SECONDS_MATCH = 0x10
} al1;


typedef enum
{
	ONCE_PER_MINUTE = 0x0E,
	MINUTES_MATCH = 0x0C,
	HOURS_MINUTES_MATCH = 0x08,
	DATE_HOURS_MINUTES_MATCH = 0x0,
	DAY_HOURS_MINUTES_MATCH = 0x10
} al2;


typedef enum
{
	OUT_OFF = 0x00,
	OUT_INT = 0x04,
	OUT_1Hz = 0x40,
	OUT_1024Hz = 0x48,
	OUT_4096Hz = 0x50,
	OUT_8192Hz = 0x58
} INT_SQW;


RTC_Time c_time, c_alarm1, c_alarm2;

Retcode_T ds3231_get_time(RTC_Time * time);
Retcode_T ds3231_set_time(RTC_Time * time);
Retcode_T convert_rtc_to_iso8601(RTC_Time * rtc_time, char * iso8601DateTime, uint8_t bufferSize);
bool ds3231_lost_power(void);
Retcode_T ds3231_initialize(void);
bool ds3231_exists(void);

#endif /* SOURCE_DS3231HANDLER_H_ */
