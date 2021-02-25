/*
 * ConnectionHandler.c
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#include <stdio.h>
#include "ConnectionHandler.h"
#include "BCDS_Assert.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_BSP_Board.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_WlanNetworkConfig.h"
#include "BCDS_WlanNetworkConnect.h"
#include "XDK_WLAN.h"
#include "XDK_UDP.h"
#include "XDK_ServalPAL.h"
#include "XDK_SNTP.h"
#include "XDK_TimeStamp.h"


static WLAN_Setup_T WLANSetupInfo =
        {
                .IsEnterprise = IS_ENTERPRISE_WIFI_SELECTED,
                .EnterpriseAuth = false,
                .IsHostPgmEnabled = false,
                .SSID = WLAN_SSID,
                .Username = WLAN_USERNAME,
                .Password = WLAN_PSK,
                .IsStatic = false,
                .IpAddr = 0UL,
                .GwAddr = 0UL,
                .DnsAddr = 0UL,
                .Mask = 0UL,
        };/**< WLAN setup parameters */


static SNTP_Setup_T SNTPSetupInfo =
        {
                .ServerUrl = SNTP_SERVER_URL,
                .ServerPort = SNTP_SERVER_PORT,
        };/**< SNTP setup parameters */



Retcode_T sendViaUDP(char * message, uint32_t size)
{
	Retcode_T retcode = RETCODE_OK;
	int16_t handle = 0;
	uint8_t data[size];
	memcpy(data, message, sizeof(data));
	retcode = UDP_Open(&handle);
	if (RETCODE_OK == retcode)
	{
		retcode = UDP_Send(handle, DEST_SERVER_IP, DEST_SERVER_PORT, data, sizeof(data));
	}
	if (RETCODE_OK != retcode)
	{
		UDP_Close(handle);
		return retcode;
	}
	retcode = UDP_Close(handle);
	return retcode;

}


Retcode_T InspectAndActOnWlanNetworkStatus(void)
{
    Retcode_T retcode = RETCODE_OK;
    WlanNetworkConnect_IpStatus_T nwStatus;

    nwStatus = WlanNetworkConnect_GetIpStatus();
    if (WLANNWCT_IPSTATUS_CT_AQRD != nwStatus)
    {
        static bool isSntpDisabled = false;
        if (false == isSntpDisabled)
        {
            retcode = SNTP_Disable();
        }
        if (RETCODE_OK == retcode)
        {
            isSntpDisabled = true;
            retcode = WLAN_Reconnect();
        }
        if (RETCODE_OK == retcode)
        {
            retcode = SNTP_Enable();
        }
        retcode = WLAN_Reconnect();
    }
    return retcode;
}


Retcode_T ConnectionSetup(CmdProcessor_T *cmdProcessor)
{
	//
	Retcode_T retcode = WLAN_Setup(&WLANSetupInfo);
    if (RETCODE_OK == retcode)
    {
    	retcode = ServalPAL_Setup(cmdProcessor);
    }
    if (RETCODE_OK == retcode)
    {
    	retcode = SNTP_Setup(&SNTPSetupInfo);
    }
    if (RETCODE_OK == retcode)
    {
    	retcode = UDP_Setup(UDP_SETUP_USE_CC31XX_LAYER);
    }
	return retcode;

}


Retcode_T ConnectionEnable(void)
{
    Retcode_T retcode = WLAN_Enable();
    if (RETCODE_OK == retcode)
    {
    	retcode = ServalPAL_Enable();
    }
    if (RETCODE_OK == retcode)
    {
    	retcode = SNTP_Enable();
    }
    if (RETCODE_OK == retcode)
    {
    	retcode = UDP_Enable();
    }
	return retcode;
}


static char timezoneISO8601format[40] = { 0 };


Retcode_T SyncSNTPTimeStamp(void)
{
	uint32_t timeStampDelta = 0;
	struct tm timeStampSystemRepresentation;
	static uint64_t timeStamp = 0;
	static bool timeSyncWithServer = true;
	Retcode_T retcode = RETCODE_OK;
	retcode = InspectAndActOnWlanNetworkStatus();
	if(RETCODE_OK == retcode)
	{
        if (true == timeSyncWithServer)
        {
            for (uint32_t sntpTimeSyncIndex = 0; ((timeStamp == 0) && (sntpTimeSyncIndex < UINT32_MAX)); sntpTimeSyncIndex++)
            {
                retcode = SNTP_GetTimeFromServer(&timeStamp, APP_RESPONSE_FROM_SNTP_SERVER_TIMEOUT);
                if ((RETCODE_OK != retcode) || (0UL == timeStamp))
                {
                    printf("AppControllerFire : SNTP server time was not synchronized for '%d' time. Retrying...\r\n", (int) (sntpTimeSyncIndex + 1));
                }
                else
                {
                    timeSyncWithServer = false;
                    timeStampDelta = 0UL;
                }
            }
        }
        else
        {
            retcode = SNTP_GetTimeFromSystem(&timeStamp, &timeStampDelta);
        }

        if (RETCODE_OK == retcode)
        {
        	retcode = TimeStamp_SecsToTm(timeStamp, &timeStampSystemRepresentation);
        }
        if (RETCODE_OK == retcode)
        {
        	retcode = TimeStamp_TmToIso8601(&timeStampSystemRepresentation, timezoneISO8601format, sizeof(timezoneISO8601format));
        }
	}
	return retcode;
}


char * getSNTPTime(void)
{
	return timezoneISO8601format;
}
