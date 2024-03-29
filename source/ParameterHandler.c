/*
 * ParameterHandler.c
 *
 *  Created on: 1 May 2021
 *      Author: abiyy
 */


#include "ParameterHandler.h"
/* system header files */
#include <stdio.h>
#include <string.h>
#include "BCDS_Assert.h"
#include "BCDS_CmdProcessor.h"

#include "XDK_Storage.h"
#include "XDK_LED.h"
#include "cJSON.h"
#include "BCDS_SDCard_Driver.h"
#include "BCDS_CmdProcessor.h"
#include "BSP_BoardType.h"
#include "XDK_Utils.h"
#include "FreeRTOS.h"
#include "timers.h"

#include "XdkAppInfo.h"
#undef BCDS_MODULE_ID  /* Module ID define before including Basics package*/
#define BCDS_MODULE_ID XDK_APP_MODULE_ID_APP_CONTROLLER

/* inline functions ********************************************************* */

#define TEST_FILENAME               "xdk_config.txt"	/**< Filename to open/write/read from SD-card */
#define FAT_FILE_SYSTEM             1 /** Macro to write data into SDCard either through FAT file system or SingleBlockWriteRead depends on the value **/
#define WRITEREAD_DELAY             UINT32_C(2000)   /**< Millisecond delay for WriteRead timer task */

/* Ram buffers
 * BUFFERSIZE should be between 512 and 1024, depending on available ram on efm32
 */
#define BUFFER_SIZE                 UINT16_C(1024)
#define SINGLE_BLOCK                UINT8_C(1)      /**< SD- Card Single block write or read */
#define DRIVE_ZERO                  UINT8_C(0)      /**< SD Card Drive 0 location */
#define SECTOR_VALUE                UINT8_C(6)      /**< SDC Disk sector value */
#define SINGLE_SECTOR_LEN           UINT32_C(1024)   /**< Single sector size in SDcard */


/* local functions ********************************************************** */

enum XDK_App_Retcode_E
{
    SDCARD_APP_ERROR = RETCODE_XDK_APP_FIRST_CUSTOM_CODE,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    FILE_OPEN_ERROR,
    FILE_LSEEK_ERROR,
    FILE_CLOSE_ERROR,
    SDCARD_INIT_FAILED
};


bool cJSON_ItemExists(cJSON *object, const char *string)
{
	cJSON *c=object->child;
	int nr = cJSON_GetArraySize(object);
	int ctr = 0;
	while (strcasecmp(c->string, string))
	{
		c=c->next;
		ctr++;
		if(ctr >= nr)
		{
			return 0;
		}
	}
	return 1;
}


Storage_Setup_T StorageSetupInfo =
        {
                .SDCard = true,
                .WiFiFileSystem = false
        };


Retcode_T setup_sd_card_storage(void)
{
	Retcode_T retcode;
    retcode = Storage_Setup(&StorageSetupInfo);
    if (RETCODE_OK == retcode)
    {
        retcode = LED_Setup();
    }
    return retcode;
}


Retcode_T enabling_sd_card_storage(void)
{
    Retcode_T retcode, fileDeleteRetcode = RETCODE_OK;
    retcode = Storage_Enable();
    if ((Retcode_T) RETCODE_STORAGE_SDCARD_NOT_AVAILABLE == Retcode_GetCode((retcode)))
    {
        /* This is only a warning error. So we will raise and proceed */
        Retcode_RaiseError(retcode);
        retcode = RETCODE_OK; /* SD card was not inserted */
    }
    if (RETCODE_OK == retcode)
    {
        retcode = LED_Enable();
    }
    return retcode;
}


/*
 */
void parse_json_config(XDKConfigs * setup, char * json_str)
{
    cJSON * root = cJSON_Parse(json_str);
    setup->all_at_once = cJSON_GetObjectItem(root, "AllAtOnce")->valueint;
    setup->default_time = cJSON_GetObjectItem(root, "DefaultTime")->valueint;

    const char * table[] = {"Acc", "Gyr", "Mag", "Env", "Lig", "Aku"};
    for (int i = 0; i < 6; i++)
    {
    	if(cJSON_ItemExists(root, table[i]))
    	{
    		cJSON * it = cJSON_GetObjectItem(root, table[i]);
    		setup->use_sensors[i] = (bool)cJSON_GetObjectItem(it, "active")->valueint;
    		setup->sensor_time[i] = (uint16_t)cJSON_GetObjectItem(it, "time")->valueint;
    		if (0 == strcasecmp(table[i], "Acc"))
    		{
    			setup->acc_type =  (bool)cJSON_GetObjectItem(it, "type")->valueint;
    		}
    		if (0 == strcasecmp(table[i], "Gyr"))
    		{
    			setup->gyr_type =  (bool)cJSON_GetObjectItem(it, "type")->valueint;
    		}
    	}
    }
    cJSON_Delete(root);
}


void read_xdk_config(void)
{
	uint8_t json_str[BUFFER_SIZE]; /* Temporary buffer for read file */
	Storage_Read_T readCredentials =
	{
			.FileName = TEST_FILENAME,
			.ReadBuffer = json_str,
			.BytesToRead = 1024,
			.ActualBytesRead = 0UL,
			.Offset = 0UL,
	};
	XDKConfigs XDKSetup;
    Retcode_T retcode = RETCODE_OK;
    retcode = Storage_Read(STORAGE_MEDIUM_SD_CARD, &readCredentials);
    if(RETCODE_OK == retcode)
    {
    	parse_json_config(&XDKSetup, json_str);
    }
}


void sd_card_manager(void)
{
    Retcode_T retcode = RETCODE_OK, ledRetcode = RETCODE_OK;
    bool sdcardEject = false, status = false, sdcardInsert = false;
    while (1)
    {
        retcode = Storage_IsAvailable(STORAGE_MEDIUM_SD_CARD, &status);
        if ((RETCODE_OK == retcode) && (true == status))
        {
            sdcardInsert = true;
            printf("SD card is inserted in XDK\n\r");
            ledRetcode = LED_On(LED_INBUILT_RED);
            if (RETCODE_OK != ledRetcode)
            {
                printf("SD card is inserted LED indication failure XDK\n\r");
            }
            if (sdcardEject == true)
            {
                retcode = Storage_Disable(STORAGE_MEDIUM_SD_CARD);
                if (RETCODE_OK == retcode)
                {
                    retcode = Storage_Enable();
                }
                if (RETCODE_OK == retcode)
                {
                    sdcardEject = false;
                }
            }
            read_xdk_config();
        }
        else
        {
            if (Retcode_GetCode(retcode) == (Retcode_T) RETCODE_STORAGE_SDCARD_UNINITIALIZED)
            {
                printf("\r\n SD card is not inserted in XDK\n\r");
                retcode = Storage_Enable();
            }
            else
            {
                if (true == sdcardInsert)
                {
                    sdcardEject = true;
                }
                sdcardInsert = false;
                printf("\r\nSD card is removed from XDK\n\r");
                ledRetcode = LED_Off(LED_INBUILT_RED);
                if (RETCODE_OK != ledRetcode)
                {
                    printf("SD card is not inserted LED indication failure XDK\n\r");
                }

            }
        }
        if (RETCODE_OK != retcode)
        {
        	printf("Raise\n");
            Retcode_RaiseError(retcode);
        }
        vTaskDelay(pdMS_TO_TICKS(WRITEREAD_DELAY));
    }
}


Retcode_T setup_config_reader(XDKConfigs * XDKSetup)
{
	Retcode_T retcode = RETCODE_FAILURE, ledRetcode = RETCODE_OK;
	bool sdcardEject = false, status = false, sdcardInsert = false;
	retcode = Storage_IsAvailable(STORAGE_MEDIUM_SD_CARD, &status);
	uint8_t json_str[BUFFER_SIZE]; /* Temporary buffer for read file */
	if ((RETCODE_OK == retcode) && (true == status))
	{
		Storage_Read_T readCredentials =
		{
				.FileName = TEST_FILENAME,
				.ReadBuffer = json_str,
				.BytesToRead = 512,
				.ActualBytesRead = 0UL,
				.Offset = 0UL,
		};
	    Retcode_T retcode = RETCODE_OK;
	    retcode = Storage_Read(STORAGE_MEDIUM_SD_CARD, &readCredentials);
	}
	if (RETCODE_OK == retcode)
	{
		parse_json_config(XDKSetup, json_str);
	}
	else
	{
		XDKSetup->all_at_once = ALLATONCE;
		XDKSetup->default_time = DEFTIME;
		XDKSetup->use_sensors[0] = true;
		XDKSetup->use_sensors[1] = true;
		XDKSetup->use_sensors[2] = true;
		XDKSetup->use_sensors[3] = true;
		XDKSetup->use_sensors[4] = true;
		XDKSetup->use_sensors[5] = true;
		retcode = RETCODE_OK;
	}
	return retcode;
}
