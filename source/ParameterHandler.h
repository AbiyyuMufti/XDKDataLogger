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


Retcode_T setup_sd_card_storage(void);
Retcode_T enabling_sd_card_storage(void);
void read_xdk_config(void);
void sd_card_manager(void);



#endif /* SOURCE_PARAMETERHANDLER_H_ */
