/*
 * CustomSensorHandler.h
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#ifndef SOURCE_CUSTOMSENSORHANDLER_H_
#define SOURCE_CUSTOMSENSORHANDLER_H_

#include "XDK_Sensor.h"

#define APP_TEMPERATURE_OFFSET_CORRECTION   (-3459)


Retcode_T xdkSensor_Setup(void);
Retcode_T xdkSensor_Enable(void);

Retcode_T readSensorValues(void);
Retcode_T readAcc(char * buffer, int * size);
Retcode_T readGyr(char * buffer, int * size);
Retcode_T readMag(char * buffer, int * size);
Retcode_T readEnv(char * buffer, int * size);
Retcode_T readLight(char * buffer, int * size);
Retcode_T readNoise(char * buffer, int * size);




#endif /* SOURCE_CUSTOMSENSORHANDLER_H_ */
