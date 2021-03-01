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



Retcode_T xdkSensor_Setup(void)
{
	return Sensor_Setup(&SensorSetup);
}


Retcode_T xdkSensor_Enable(void)
{
	return Sensor_Enable();
}


Retcode_T readSensorValues(char * buffer, int * size)
{
    Sensor_Value_T sensorValue;

    memset(&sensorValue, 0x00, sizeof(sensorValue));

    Retcode_T retcode = Sensor_GetData(&sensorValue);
    int size1, size2, size3, size4, size5, size6;
    char buffer1[150] = {0}, buffer2[150] = {0}, buffer3[150] = {0}, buffer4[150] = {0}, buffer5[150] = {0}, buffer6[150] = {0};
    const char * holder1 = "BMA280;%ld;%ld;%ld";
    const char * holder2 = "BMG160;%ld;%ld;%ld";
    const char * holder3 = "BMM150;%ld;%ld;%ld";
    const char * holder4 = "BME280;%lu;%f;%lu";
    const char * holder5 = "MAX44009;%lu";
    const char * holder6 = "AKU340;%f;%f";
    char timeSNTP[50] = {0};

    int timeSize = sprintf(timeSNTP, "%s", getSNTPTime());
    //printf("%s\n", timeSNTP);
    if(RETCODE_OK == retcode)
    {
    	float acousticData, spl;
    	acousticData = sensorValue.Noise;
    	spl = calcSoundPressureLevel(acousticData);

    	size1 = sprintf(buffer1, holder1, (long int) sensorValue.Accel.X, (long int) sensorValue.Accel.Y, (long int) sensorValue.Accel.Z);
    	size2 = sprintf(buffer2, holder2, (long int) sensorValue.Gyro.X, (long int) sensorValue.Gyro.Y, (long int) sensorValue.Gyro.Z);
    	size3 = sprintf(buffer3, holder3, (long int) sensorValue.Mag.X, (long int) sensorValue.Mag.Y, (long int) sensorValue.Mag.Z);
    	size4 = sprintf(buffer4, holder4, (unsigned long int) sensorValue.Pressure, sensorValue.Temp, (unsigned long int) sensorValue.RH);
    	size5 = sprintf(buffer5, holder5, (unsigned long int) sensorValue.Light);

    	size6 = sprintf(buffer6, holder6, acousticData, spl);
    	* size = sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s\t%s", timeSNTP, buffer1, buffer2, buffer3, buffer4, buffer5, buffer6);
    	//printf("Endsize: %d\n", * size);
    	//2021-01-26T02:55:20Z;
    	//BMA280;9;-7;1020;
    	//BMG160;244;-122;0;
    	//BMM150;-17;-3;-54;
    	//BME280;100309;22961.000000;60;
    	//MAX44009;2880;
    	//AKU340;0.001030;34.233944
    }
    return retcode;
}


//Retcode_T readSensorsValues()


Retcode_T readAcc(char * buffer, int * size)
{
	Accelerometer_XyzData_T bma280 = {INT32_C(0), INT32_C(0), INT32_C(0)};
	memset(&bma280, 0, sizeof(Accelerometer_XyzData_T));
	Retcode_T retcode = Sensor_GetAccel(&bma280);
	if (RETCODE_OK == retcode)
	{
		*size = sprintf(buffer, "BMA280;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bma280.xAxisData, (long int) bma280.yAxisData, (long int) bma280.zAxisData);
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
		*size = sprintf(buffer, "BMG160;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bmg160.xAxisData, (long int) bmg160.yAxisData, (long int) bmg160.zAxisData);
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
		*size = sprintf(buffer, "BMM150;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bmm150.xAxisData, (long int) bmm150.yAxisData, (long int) bmm150.zAxisData);
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
		*size = sprintf(buffer, "BME280;%s;%ld;%ld;%ld", getSNTPTime(), (long int) bme280.pressure, (long int) bme280.temperature, (long int) bme280.humidity);
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
		*size = sprintf(buffer, "MAX44009;%s;%u", getSNTPTime(), (unsigned int) max44009);
	}
	return retcode;
}


Retcode_T readNoise(char * buffer, int * size)
{
	Retcode_T retcode;
	float acousticData, spl;

	retcode = Sensor_GetNoise(&acousticData);
    spl = calcSoundPressureLevel(acousticData);

	if(RETCODE_OK == retcode)
	{
		*size = sprintf(buffer, "AKU340;%s;%f;%f", getSNTPTime(), acousticData, spl);
	}
	return retcode;
}
