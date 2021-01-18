/*
 * TimerHandler.h
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#ifndef SOURCE_TIMERHANDLER_H_
#define SOURCE_TIMERHANDLER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define TIMERBLOCKTIME UINT32_C(0xffff)
#define TIMER_AUTORELOAD_ON pdTRUE
#define TIMER_AUTORELOAD_OFF pdFALSE
#define SECONDS(x) ((portTickType) (x * 1000) / portTICK_RATE_MS)
#define MILLISECONDS(x) ((portTickType) x / portTICK_RATE_MS)

xTimerHandle accTimers, magTimers, gyrTimers, ligTimers, envTimers, akuTimers;

void * AccelerometerCallback(xTimerHandle xTimer);
void * EnvironmentCallback(xTimerHandle xTimer);
void * MagnetometerCallback(xTimerHandle xTimer);
void * AmbientLightCallback(xTimerHandle xTimer);
void * GyroscopeCallback(xTimerHandle xTimer);
void * NoiseCallback(xTimerHandle xTimer);

void createAndStartTimer(void)
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


#endif /* SOURCE_TIMERHANDLER_H_ */
