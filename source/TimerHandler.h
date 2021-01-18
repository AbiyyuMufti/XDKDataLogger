/*
 * TimerHandler.h
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#ifndef SOURCE_TIMERHANDLER_H_
#define SOURCE_TIMERHANDLER_H_

#define TIMERBLOCKTIME UINT32_C(0xffff)
#define TIMER_AUTORELOAD_ON pdTRUE
#define TIMER_AUTORELOAD_OFF pdFALSE
#define SECONDS(x) ((portTickType) (x * 1000) / portTICK_RATE_MS)
#define MILLISECONDS(x) ((portTickType) x / portTICK_RATE_MS)


#endif /* SOURCE_TIMERHANDLER_H_ */
