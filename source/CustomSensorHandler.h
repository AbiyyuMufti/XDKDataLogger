/*
 * CustomSensorHandler.h
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#ifndef SOURCE_CUSTOMSENSORHANDLER_H_
#define SOURCE_CUSTOMSENSORHANDLER_H_

#include "XDK_Sensor.h"
#include "ParameterHandler.h"

#define APP_TEMPERATURE_OFFSET_CORRECTION   (-3459)


Retcode_T xdkSensor_Setup(XDKConfigs * XDKSetup);
Retcode_T xdkSensor_Enable(void);

Retcode_T readSensorValuesInDiffTime(char * buffer, XDKConfigs * XDKSetup, bool * ready);
Retcode_T readSensorValues(char * buffer, XDKConfigs * XDKSetup, bool * ready);

Retcode_T readAcc(char * buffer);
Retcode_T readGyr(char * buffer);
Retcode_T readMag(char * buffer);
Retcode_T readEnv(char * buffer);
Retcode_T readLight(char * buffer);
Retcode_T readNoise(char * buffer);



#endif /* SOURCE_CUSTOMSENSORHANDLER_H_ */
