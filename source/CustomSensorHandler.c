/*
 * CustomSensorHandler.c
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */


#include <stdio.h>
#include "CustomSensorHandler.h"
#include "XDK_Sensor.h"
#include "XdkSensorHandle.h"
#include "BCDS_BSP_Board.h"
#include "ConnectionHandler.h"
#include "math.h"
#include "arm_math.h"
#include "FreeRTOS.h"
#include "task.h"


const char * BMA280Holder = "\tBMA280;%ld;%ld;%ld";
const char * BMG160Holder = "\tBMG160;%ld;%ld;%ld";
const char * BMM150Holder = "\tBMM150;%ld;%ld;%ld";
const char * BME280Holder = "\tBME280;%lu;%.2f;%lu";
const char * MAX44009Holder = "\tMAX44009;%lu";
const char * AKU340Holder = "\tAKU340;%f;%.2f";

static Sensor_Setup_T SensorSetup =
{
		.CmdProcessorHandle = NULL,
		.Enable =
		{
				.Accel = true,
				.Mag = true,
				.Gyro = true,
				.Humidity = true,
				.Temp = true,
				.Pressure = true,
				.Light = true,
				.Noise = true,
		},
		.Config =
		{
				.Accel =
				{
						.Type = SENSOR_ACCEL_BMA280,
						.IsRawData = false,
						.IsInteruptEnabled = false,
				},
				.Gyro =
				{
						.Type = SENSOR_GYRO_BMG160,
						.IsRawData = false,
				},
				.Mag =
				{
						.IsRawData = false,
				},
				.Light =
				{
						.IsInteruptEnabled = false,
				},
				.Temp =
				{
						.OffsetCorrection = APP_TEMPERATURE_OFFSET_CORRECTION,
				},
		},
};/**< Sensor setup parameters */


const float aku340ConversionRatio = pow(10,(-38/20));


static float calcSoundPressure(float acousticRawValue)
{
    return (acousticRawValue/aku340ConversionRatio);
}


const float SplRatio = pow(10,(-38/20)) * 20e-6;


static float calcSoundPressureLevel(float magnitude)
{
    float spl;

    if(magnitude == 0)
    {
        spl = 25;
    }
    else
    {
        spl = (20*log10(magnitude/SplRatio)-20);
    }

    return spl;
}



Retcode_T xdkSensor_Setup(XDKConfigs * XDKSetup)
{
	SensorSetup.Enable.Accel = XDKSetup->use_sensors[0];
	SensorSetup.Enable.Gyro = XDKSetup->use_sensors[1];
	SensorSetup.Enable.Mag = XDKSetup->use_sensors[2];
	SensorSetup.Enable.Humidity = XDKSetup->use_sensors[3];
	SensorSetup.Enable.Temp =  XDKSetup->use_sensors[3];
	SensorSetup.Enable.Pressure = XDKSetup->use_sensors[3];
	SensorSetup.Enable.Light =  XDKSetup->use_sensors[4];
	SensorSetup.Enable.Noise =  XDKSetup->use_sensors[5];
	SensorSetup.Config.Accel.Type = (uint8_t)XDKSetup->acc_type;
	SensorSetup.Config.Gyro.Type = (uint8_t)XDKSetup->gyr_type;
	return Sensor_Setup(&SensorSetup);
}


Retcode_T xdkSensor_Enable(void)
{
	return Sensor_Enable();
}

Retcode_T readSensorValuesInDiffTime(char * buffer, XDKConfigs * XDKSetup, bool * ready)
{
	Retcode_T retcode = RETCODE_OK;

    char BMA280Buffer[150] = {0}, BMG160Buffer[150] = {0}, BMM150Buffer[150] = {0};
    char BME280Buffer[150] = {0}, MAX44009Buffer[150] = {0}, AKU340Buffer[150] = {0};
    static uint32_t lastTime[6] = {0};
    bool thisTurn[6];
    *ready = false;
    uint32_t current = xTaskGetTickCount();
    for (int i = 0; i < 6; i++)
    {
    	thisTurn[i] = ((current - lastTime[i]) >= XDKSetup->sensor_time[i]);
    }

	if (RETCODE_OK == retcode && (SensorSetup.Enable.Accel && ((current - lastTime[0]) >= XDKSetup->sensor_time[0])))
	{
		readAcc(BMA280Buffer);
		strcat(buffer, BMA280Buffer);
		lastTime[0] = xTaskGetTickCount();
		*ready = true;
	}
	if (RETCODE_OK == retcode && (SensorSetup.Enable.Gyro && ((current - lastTime[1]) >= XDKSetup->sensor_time[2])))
	{
		readGyr(BMG160Buffer);
		strcat(buffer, BMG160Buffer);
		lastTime[1] = xTaskGetTickCount();
		*ready = true;
	}
	if (RETCODE_OK == retcode && (SensorSetup.Enable.Mag &&((current - lastTime[2]) >= XDKSetup->sensor_time[2])))
	{
		readMag(BMM150Buffer);
    	strcat(buffer, BMM150Buffer);
    	lastTime[2] = xTaskGetTickCount();
    	*ready = true;
	}

	if (RETCODE_OK == retcode && (SensorSetup.Enable.Pressure && ((current - lastTime[3]) >= XDKSetup->sensor_time[3])))
	{
		readEnv(BME280Buffer);
		strcat(buffer, BME280Buffer);
		lastTime[3] = xTaskGetTickCount();
		*ready = true;
	}

	if (RETCODE_OK == retcode && (SensorSetup.Enable.Light && ((current - lastTime[3]) >= XDKSetup->sensor_time[3])))
	{
		readLight(MAX44009Buffer);
		strcat(buffer, MAX44009Buffer);
		lastTime[4] = xTaskGetTickCount();
		*ready = true;
	}
	if (RETCODE_OK == retcode && (SensorSetup.Enable.Noise && ((current - lastTime[3]) >= XDKSetup->sensor_time[3])))
	{
		readNoise(AKU340Buffer);
    	strcat(buffer, AKU340Buffer);
    	lastTime[5] = xTaskGetTickCount();
    	*ready = true;
	}
	return retcode;
}


Retcode_T readSensorValues(char * buffer, XDKConfigs * XDKSetup, bool * ready)
{
	Retcode_T retcode = RETCODE_FAILURE;
	static uint32_t lastTime = 0;
	uint32_t current = xTaskGetTickCount();
	* ready = false;
	if ((current - lastTime) >= XDKSetup->default_time)
	{
	    Sensor_Value_T sensorValue;
	    memset(&sensorValue, 0x00, sizeof(sensorValue));

	    char BMA280Buffer[150] = {0}, BMG160Buffer[150] = {0}, BMM150Buffer[150] = {0};
	    char BME280Buffer[150] = {0}, MAX44009Buffer[150] = {0}, AKU340Buffer[150] = {0};

	    retcode = Sensor_GetData(&sensorValue);

	    if (RETCODE_OK == retcode)
	    {
	    	float acousticData, spl;
	    	acousticData = sensorValue.Noise;
	    	spl = calcSoundPressureLevel(acousticData);
	    	if (SensorSetup.Enable.Accel)
	    	{
	    		sprintf(BMA280Buffer, BMA280Holder, (long int) sensorValue.Accel.X, (long int) sensorValue.Accel.Y, (long int) sensorValue.Accel.Z);
	    		strcat(buffer, BMA280Buffer);
	    		* ready = true;
	    	}
	    	if (SensorSetup.Enable.Gyro)
	    	{
	    		sprintf(BMG160Buffer, BMG160Holder, (long int) sensorValue.Gyro.X, (long int) sensorValue.Gyro.Y, (long int) sensorValue.Gyro.Z);
	    		strcat(buffer, BMG160Buffer);
	    		* ready = true;
	    	}
	    	if (SensorSetup.Enable.Mag)
	    	{
	        	sprintf(BMM150Buffer, BMM150Holder, (long int) sensorValue.Mag.X, (long int) sensorValue.Mag.Y, (long int) sensorValue.Mag.Z);
	        	strcat(buffer, BMM150Buffer);
	        	* ready = true;
	    	}

	    	if ((SensorSetup.Enable.Humidity && SensorSetup.Enable.Temp) && SensorSetup.Enable.Pressure)
	    	{
	    		sprintf(BME280Buffer, BME280Holder, (unsigned long int) sensorValue.Pressure, sensorValue.Temp/1000.00, (unsigned long int) sensorValue.RH);
	    		strcat(buffer, BME280Buffer);
	    		* ready = true;
	    	}

	    	if (SensorSetup.Enable.Light)
	    	{
	    		sprintf(MAX44009Buffer, MAX44009Holder, (unsigned long int) sensorValue.Light);
	    		strcat(buffer, MAX44009Buffer);
	    		* ready = true;
	    	}
	    	if (SensorSetup.Enable.Noise)
	    	{
	        	sprintf(AKU340Buffer, AKU340Holder, acousticData, spl);
	        	strcat(buffer, AKU340Buffer);
	        	* ready = true;
	    	}
	    }
	    lastTime = xTaskGetTickCount();
	}
	return retcode;
}


Retcode_T readAcc(char * buffer)
{
	Accelerometer_XyzData_T bma280 = {INT32_C(0), INT32_C(0), INT32_C(0)};
	memset(&bma280, 0, sizeof(Accelerometer_XyzData_T));
	Retcode_T retcode = Sensor_GetAccel(&bma280);
	if (RETCODE_OK == retcode)
	{
		sprintf(buffer, BMA280Holder, (long int) bma280.xAxisData, (long int) bma280.yAxisData, (long int) bma280.zAxisData);
	}
	return retcode;
}


Retcode_T readGyr(char * buffer)
{
	Retcode_T retcode;
	Gyroscope_XyzData_T bmg160 = {INT32_C(0), INT32_C(0), INT32_C(0)};
	memset(&bmg160, 0, sizeof(CalibratedGyro_DpsData_T));
	retcode = Sensor_GetGyro(&bmg160);
	if(RETCODE_OK == retcode)
	{
		sprintf(buffer, BMG160Holder, (long int) bmg160.xAxisData, (long int) bmg160.yAxisData, (long int) bmg160.zAxisData);
	}
	return retcode;
}


Retcode_T readMag(char * buffer)
{
	Retcode_T retcode;
	Magnetometer_XyzData_T bmm150 = {INT32_C(0), INT32_C(0), INT32_C(0),INT32_C(0)};
	retcode = Sensor_GetMag(&bmm150);

	if(RETCODE_OK == retcode)
	{
		sprintf(buffer, BMM150Holder, (long int) bmm150.xAxisData, (long int) bmm150.yAxisData, (long int) bmm150.zAxisData);
	}
	return retcode;
}


Retcode_T readEnv(char * buffer)
{
	Retcode_T retcode;
	Environmental_Data_T bme280 = { INT32_C(0), INT32_C(0), INT32_C(0) };
	retcode = Sensor_GetEnvironmental(&bme280);
	if(RETCODE_OK == retcode)
	{
		sprintf(buffer, BME280Holder, (long int) bme280.pressure, bme280.temperature / 1000.00, (long int) bme280.humidity);
	}
	return retcode;
}


Retcode_T readLight(char * buffer)
{
	Retcode_T retcode;
	uint32_t max44009 = INT32_C(0);

	retcode = Sensor_GetLight(&max44009);

	if(RETCODE_OK == retcode)
	{
		sprintf(buffer, MAX44009Holder, (unsigned int) max44009);
	}
	return retcode;
}


Retcode_T readNoise(char * buffer)
{
	Retcode_T retcode;
	float acousticData, spl;

	retcode = Sensor_GetNoise(&acousticData);
    spl = calcSoundPressureLevel(acousticData);

	if(RETCODE_OK == retcode)
	{
		sprintf(buffer, AKU340Holder, acousticData, spl);
	}
	return retcode;
}
