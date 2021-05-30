/*
 * ParameterHandler.h
 *
 *  Created on: 25 Feb 2021
 *      Author: abiyy
 */

#ifndef SOURCE_PARAMETERHANDLER_H_
#define SOURCE_PARAMETERHANDLER_H_

#include "XDK_Storage.h"
#include "XDK_LED.h"
#include "cJSON.h"
#include "BCDS_SDCard_Driver.h"
#include "BCDS_CmdProcessor.h"
#include "BSP_BoardType.h"


#define ALLATONCE true
#define DEFTIME 1000
#define ACCTIME 10
#define GYRTIME 10
#define MAGTIME 50
#define ENVTIME 40
#define LIGTIME 800
#define AKUTIME 100

typedef struct {
	bool all_at_once;
	uint16_t default_time;
	bool use_sensors[6];
	uint16_t sensor_time[6];
	bool acc_type;
	bool gyr_type;
	char * SSDI;
	char * PSW;
} XDKConfigs;

//extern XDKConfigs XDKSetup;

Retcode_T setup_sd_card_storage(void);
Retcode_T enabling_sd_card_storage(void);
void read_xdk_config(void);
void sd_card_manager(void);
Retcode_T setup_config_reader(XDKConfigs * XDKSetup);

#endif /* SOURCE_PARAMETERHANDLER_H_ */
