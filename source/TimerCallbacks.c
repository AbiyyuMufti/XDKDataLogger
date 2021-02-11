/*
 * TimerCallbacks.c
 *
 *  Created on: 26 Jan 2021
 *      Author: abiyy
 */

#include "TimerHandler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "BCDS_Assert.h"
#include "XDK_Utils.h"

void * AccelerometerCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readAcc(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer);
	}
}


void * EnvironmentCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readEnv(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer3);
	}
}


void * MagnetometerCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readMag(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer2);
	}
}


void * AmbientLightCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readLight(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer4);
	}
}


void * GyroscopeCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readGyr(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer1);
	}
}


void * NoiseCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150] = {0};
	int size = 0;
	retcode = readNoise(buffer, &size);
	if (RETCODE_OK == retcode)
	{
		sendViaUDP(buffer, size);
		//printf("%s;\n", buffer5);
	}
}


void * SensorsCallback(xTimerHandle xTimer)
{
	(void) xTimer;
	Retcode_T retcode = RETCODE_OK;
	char buffer[150*6] = {0};
	int size = 0;
	retcode = readSensorValues(buffer, &size);
	sendViaUDP(buffer, size);
	printf("%s\n", buffer);
}

void createAndStartTimer(void)
{
	sensorTimer = xTimerCreate((const char * const) "Sen", MILLISECONDS(100),
			TIMER_AUTORELOAD_ON, NULL, SensorsCallback);
	if(NULL == sensorTimer) { assert(pdFAIL); return; }
	BaseType_t timerResult = xTimerStart(sensorTimer, TIMERBLOCKTIME);
	if(pdTRUE != timerResult) { assert(pdFAIL); return; }
}

void createAndStartTimers(void)
{
	accTimers = xTimerCreate((const char * const) "Acc", MILLISECONDS(100),
			TIMER_AUTORELOAD_ON, NULL, AccelerometerCallback);
	if(NULL == accTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult1 = xTimerStart(accTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult1) { assert(pdFAIL); return; }

	magTimers = xTimerCreate((const char * const) "Mag", MILLISECONDS(500),
			TIMER_AUTORELOAD_ON, NULL, MagnetometerCallback);
	if(NULL == magTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult2 = xTimerStart(magTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult2) { assert(pdFAIL); return; }

	gyrTimers = xTimerCreate((const char * const) "Gyr", MILLISECONDS(100),
			TIMER_AUTORELOAD_ON, NULL, GyroscopeCallback);
	if(NULL == gyrTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult3 = xTimerStart(gyrTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult3) { assert(pdFAIL); return; }

	ligTimers = xTimerCreate((const char * const) "Lgh", MILLISECONDS(800),
			TIMER_AUTORELOAD_ON, NULL, AmbientLightCallback);
	if(NULL == ligTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult4 = xTimerStart(ligTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult4) { assert(pdFAIL); return; }

	envTimers = xTimerCreate((const char * const) "Env", MILLISECONDS(400),
			TIMER_AUTORELOAD_ON, NULL, EnvironmentCallback);
	if(NULL == envTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult5 = xTimerStart(envTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult5) { assert(pdFAIL); return; }

	akuTimers = xTimerCreate((const char * const) "Aku", MILLISECONDS(100),
			TIMER_AUTORELOAD_ON, NULL, NoiseCallback);
	if(NULL == akuTimers) { assert(pdFAIL); return; }
	BaseType_t timerResult6 = xTimerStart(akuTimers, TIMERBLOCKTIME);
	if(pdTRUE != timerResult6) { assert(pdFAIL); return; }
}
