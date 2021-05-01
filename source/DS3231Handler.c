/*
 * DS3231Handler.c
 *
 *  Created on: 23 Apr 2021
 *      Author: abiyy
 */


#include "DS3231Handler.h"
#include "I2CHandler.h"
#include "time.h"
#include <stdio.h>

#define DS3231_ADDRESS 0x68
#define DS3231_TIMEREG 0x00
#define DS3231_ALARM1 0x07    ///< Alarm 1 register
#define DS3231_ALARM2 0x0B    ///< Alarm 2 register
#define DS3231_CONTROL 0x0E   ///< Control register
#define DS3231_STATUSREG 0x0F
#define DS3231_TEMPERATUREREG 0x11


static uint8_t encode(int value)
{
    int low;

    low = (value % 10);

    return ((((value - low) / 10) << 4) | low);
}


static int decode(uint8_t value)
{
    return (10 * (value >> 4) + (value & 0x0f));
}


Retcode_T ds3231_get_time(RTC_Time * time)
{
	Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);

	uint8_t date_time[7];
	uint8_t chipId = 0U;

	if(RETCODE_OK != I2C_Read(DS3231_ADDRESS, DS3231_TIMEREG, &chipId, UINT8_C(1)))
		retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);

	if(RETCODE_OK == I2C_Read(DS3231_ADDRESS, DS3231_TIMEREG, date_time, UINT8_C(7)))
	{
		time->seconds = decode(date_time[0]);
		time->minutes = decode(date_time[1]);
		time->hours = (20 * ((date_time[2] >> 5) & 0x01) + decode(date_time[2] & 0x1f));
		time->dow = date_time[3];
		time->day = decode(date_time[4]);
		time->month = decode(date_time[5] & 0x7f);
		time->year = decode(date_time[6]) + 2000;
		retcode = RETCODE_OK;
	}
	else
	{
		retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
	}

	return retcode;
}


Retcode_T ds3231_set_time(RTC_Time * time)
{
	Retcode_T retcode = RETCODE_OK;
	uint8_t date_time[7];
	uint8_t status = 0;

	if(RETCODE_OK == retcode)
	{
		date_time[0] = encode(time->seconds);
		date_time[1] = encode(time->minutes);
		date_time[2] = encode(time->hours);
		date_time[3] = time->dow;
		date_time[4] = encode(time->day);
		date_time[5] = encode(time->month);
		date_time[6] = encode(time->year - 2000);
		retcode = I2C_Write(DS3231_ADDRESS, DS3231_TIMEREG, date_time, sizeof(date_time));
	}

	if(RETCODE_OK == retcode)
	{
		retcode = I2C_Read(DS3231_ADDRESS, DS3231_STATUSREG, &status, 1);
	}

	if(RETCODE_OK == retcode)
	{
		status &= ~0x80;
		retcode = I2C_Write(DS3231_ADDRESS, DS3231_STATUSREG, &status, 1);
	}

	return retcode;
}


Retcode_T convert_rtc_to_iso8601(RTC_Time * rtc_time, char * iso8601DateTime, uint8_t bufferSize)
{
	static const uint8_t DATETIME_STRING_MIN_LENGTH = 21;
	Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
	struct tm time_struct;
	char str[21] = {0};
	if((NULL == rtc_time) || (NULL == iso8601DateTime))
	{
		retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
	}
	else if (DATETIME_STRING_MIN_LENGTH > bufferSize)
	{
		retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
	}
	else
	{
		time_struct.tm_year = rtc_time->year - 1900;
		time_struct.tm_mon = rtc_time->month - 1;
		time_struct.tm_mday = rtc_time->day;
		time_struct.tm_wday = rtc_time->dow - 1;
		time_struct.tm_hour = rtc_time->hours;
		time_struct.tm_min = rtc_time->minutes;
		time_struct.tm_sec = rtc_time->seconds;
		strftime(str, 30, "%Y-%m-%dT%H:%M:%SZ", &time_struct);
		(void)memcpy(iso8601DateTime, str, (size_t)DATETIME_STRING_MIN_LENGTH);
		retcode = RETCODE_OK;
	}
	return retcode;
}


bool ds3231_lost_power(void)
{
	uint8_t status = 0;
	I2C_Read(DS3231_ADDRESS, DS3231_STATUSREG, &status, UINT8_C(1));
	return (status >> 7);
}


static uint8_t conv2d(const char *p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return (10 * v) + (*++p - '0');
}


static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};
	if (y >= 2000U)
		y -= 2000U;
	uint16_t days = d;
	for (uint8_t i = 1; i < m; ++i)
		days += pgm_read_byte(daysInMonth + i - 1);
	if (m > 2 && y % 4 == 0)
		++days;
	return days + 365 * y + (y + 3) / 4 - 1;
}


void get_compile_date_time(RTC_Time * c_time)
{
	char * date = __DATE__;
	char * time = __TIME__;
	uint8_t yOff, m, d, hh, mm, ss;
	yOff = conv2d(date + 9);
	switch (date[0]) {
	case 'J':
		m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;
	case 'F':
	    m = 2; break;
	case 'A':
		m = date[2] == 'r' ? 4 : 8; break;
	case 'M':
	    m = date[2] == 'r' ? 3 : 5; break;
	case 'S':
		m = 9; break;
	case 'O':
		m = 10; break;
	case 'N':
		m = 11; break;
	case 'D':
		m = 12; break;
	}
	d = conv2d(date + 4);
	hh = conv2d(time);
	mm = conv2d(time + 3);
	ss = conv2d(time + 6);
	uint16_t day = date2days(yOff + 2000, m, d);
	day = (day + 6) % 7;

	c_time->seconds = ss;
	c_time->minutes = mm;
	c_time->hours = hh;
	c_time->day = d;
	c_time->dow = day == 0 ? 7 : day;
	c_time->month = m;
	c_time->year = yOff + 2000;
}


Retcode_T ds3231_initialize(void)
{
	Retcode_T retcode = RETCODE_OK;
	printf("%s %s \n", __DATE__, __TIME__);
	if(ds3231_lost_power())
	{
		RTC_Time time;
		get_compile_date_time(&time);
		char str[21] = {0};
		convert_rtc_to_iso8601(&time, str, 21);
		printf("Set time to %s \n", str);
		retcode = ds3231_set_time(&time);
	}
	return retcode;
}

bool ds3231_exists(void)
{
    scannedI2CDevices scannedDevices;
    I2C_Scan(&scannedDevices);
    if(scannedDevices.number > 0)
    {
    	return scannedDevices.address[DS3231_ADDRESS];
    }
    else
    {
    	return 0;
    }
}
