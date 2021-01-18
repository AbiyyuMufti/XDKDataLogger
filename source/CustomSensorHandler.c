/*
 * CustomSensorHandler.c
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */


#include "CustomSensorHandler.h"
#include "XDK_Sensor.h"
#include "XdkSensorHandle.h"
#include "BCDS_BSP_Board.h"
#include "ConnectionHandler.h"
#include "math.h"
#include "arm_math.h"

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


Retcode_T xdkSensor_Setup(void)
{
	return Sensor_Setup(&SensorSetup);
}


Retcode_T xdkSensor_Enable(void)
{
	return Sensor_Enable();
}


Retcode_T readSensorValues(void)
{
    Sensor_Value_T sensorValue;

    memset(&sensorValue, 0x00, sizeof(sensorValue));

    Retcode_T retcode = Sensor_GetData(&sensorValue);

    return retcode;
}


Retcode_T readAcc(char * buffer, int * size)
{
	Accelerometer_XyzData_T bma280 = {INT32_C(0), INT32_C(0), INT32_C(0)};
	memset(&bma280, 0, sizeof(Accelerometer_XyzData_T));
	Retcode_T retcode = Sensor_GetAccel(&bma280);
	if (RETCODE_OK == retcode)
	{
		size = sprintf(buffer, "BMA280:%s;%ld;%ld;%ld", getSNTPTime(), (long int) bma280.xAxisData, (long int) bma280.yAxisData, (long int) bma280.zAxisData);
	}
	return retcode;
}


Retcode_T readGyr(char * buffer, int * size)
{
	Retcode_T retcode;
	Gyroscope_XyzData_T bmg160 = {INT32_C(0), INT32_C(0), INT32_C(0)};
	memset(&bmg160, 0, sizeof(CalibratedGyro_DpsData_T));
	retcode = Sensor_GetGyro(&bmg160);
	if(RETCODE_OK == retcode)
	{
		int size = sprintf(buffer, "BMG160;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bmg160.xAxisData, (long int) bmg160.yAxisData, (long int) bmg160.zAxisData);
	}
	return retcode;
}


Retcode_T readMag(char * buffer, int * size)
{
	Retcode_T retcode;
	Magnetometer_XyzData_T bmm150 = {INT32_C(0), INT32_C(0), INT32_C(0),INT32_C(0)};
	retcode = Sensor_GetMag(&bmm150);

	if(RETCODE_OK == retcode)
	{
		size = sprintf(buffer, "BMM150;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bmm150.xAxisData, (long int) bmm150.yAxisData, (long int) bmm150.zAxisData);
	}
	return retcode;
}


Retcode_T readEnv(char * buffer, int * size)
{
	Retcode_T retcode;
	Environmental_Data_T bme280 = { INT32_C(0), INT32_C(0), INT32_C(0) };
	retcode = Sensor_GetEnvironmental(&bme280);
	if(RETCODE_OK == retcode)
	{
		size = sprintf(buffer, "BME280;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bme280.pressure, (long int) bme280.temperature, (long int) bme280.humidity);
	}
	return retcode;
}


Retcode_T readLight(char * buffer, int * size)
{
	Retcode_T retcode;
	uint32_t max44009 = INT32_C(0);

	retcode = Sensor_GetLight(&max44009);

	if(RETCODE_OK == retcode)
	{
		size = sprintf(buffer, "MAX44009;%s;%u", getSNTPTime(), (unsigned int) max44009);
	}
	return retcode;
}


const float aku340ConversionRatio = pow(10,(-38/20));


static float calcSoundPressure(float acousticRawValue)
{
    return (acousticRawValue/aku340ConversionRatio);
}


const float SplRatio = pow(10,(-38/20)) * 20e-6;


static float calcSoundPressureLevel(float magnitude)
{
    float spl;

    if(magnitude == 0){
        spl = 25;
    }

    else {
        spl = (20*log10(magnitude/SplRatio)-20);
    }

    return spl;
}


Retcode_T readNoise(char * buffer, int * size)
{
	Retcode_T retcode;
	float acousticData, sp, spl;
	double si;

	retcode = Sensor_GetNoise(&acousticData);
    spl = calcSoundPressureLevel(acousticData);

	if(RETCODE_OK == retcode)
	{
		size = sprintf(buffer, "AKU340;%s;%f;%f;", getSNTPTime(), acousticData, spl);
	}
	return retcode;
}
