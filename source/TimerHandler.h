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

xTimerHandle sensorTimer, accTimers, magTimers, gyrTimers, ligTimers, envTimers, akuTimers;

void * AccelerometerCallback(xTimerHandle xTimer);
void * EnvironmentCallback(xTimerHandle xTimer);
void * MagnetometerCallback(xTimerHandle xTimer);
void * AmbientLightCallback(xTimerHandle xTimer);
void * GyroscopeCallback(xTimerHandle xTimer);
void * NoiseCallback(xTimerHandle xTimer);
void * SensorsCallback(xTimerHandle xTimer);

void createAndStartOneTimer(void);
void createAndStartTimers(void);

#endif /* SOURCE_TIMERHANDLER_H_ */
