/*
 * ConnectionHandler.h
 *
 *  Created on: 18 Jan 2021
 *      Author: abiyy
 */

#ifndef SOURCE_CONNECTIONHANDLER_H_
#define SOURCE_CONNECTIONHANDLER_H_

#include "XDK_WLAN.h"
#include "XDK_UDP.h"
#include "XDK_ServalPAL.h"
#include "XDK_SNTP.h"
#include "XDK_TimeStamp.h"

/* WLAN configurations ****************************************************** */
/**
 * WLAN_CONNECT_WPA_SSID is the SSID of the WIFI network you want to connect to.
 */
#define WLAN_SSID                       "HomeSweetHome"

/**
 * WLAN_CONNECT_WPA_PASS is the WPA/WPA2 passphrase (pre-shared key) of your WIFI network.
 */
#define WLAN_PSK                        "1bnAbdillah"

/**
 * WLAN_USERNAME is the Enterprise WIFI network username(unused if IS_ENTERPRISE_WIFI_SELECTED is false)
 */
#define WLAN_USERNAME                   "YourWifiUserName"

/**
 * IS_ENTERPRISE_WIFI_SELECTED is a boolean. If "true" then XDK device will connect to Enterprise WIFI Network and If "false" then XDK device will connect to Personal WIFI Network.
 */
#define IS_ENTERPRISE_WIFI_SELECTED     false


/* UDP configurations ****************************************************** */

/**
 * DEST_SERVER_IP is the destination server IP address of the web server we will send UDP payloads.
 * If you want to test this example without setting up your own server, you can use publicly available services.
 */
#define DEST_SERVER_IP                  XDK_NETWORK_IPV4(192, 168, 0, 73)

/**
 * DEST_SERVER_PORT is the UDP port to which we will send UDP payloads.
 */
#define DEST_SERVER_PORT                UINT16_C(5005)


/* SNTP configurations ****************************************************** */

/**
 * SNTP_SERVER_URL is the SNTP server URL.
 */
#define SNTP_SERVER_URL                     "0.de.pool.ntp.org"

/**
 * SNTP_SERVER_PORT is the SNTP server port number.
 */
#define SNTP_SERVER_PORT                    UINT16_C(123)

/**
 * Timeout for SNTP server time sync
 */
#define APP_RESPONSE_FROM_SNTP_SERVER_TIMEOUT           UINT32_C(10000)

/* utility functions ****************************************************** */

Retcode_T sendViaUDP(char * message, uint32_t size);
Retcode_T InspectAndActOnWlanNetworkStatus(void);
Retcode_T ConnectionSetup(CmdProcessor_T *cmdProcessor);
Retcode_T ConnectionEnable(void);
Retcode_T SyncSNTPTimeStamp(void);
char * getSNTPTime();


#endif /* SOURCE_CONNECTIONHANDLER_H_ */
