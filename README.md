# XDKDataLogger

## Description

Using BOSCH XDK device to do measurement and logging it into the local database in a computer. XDKDataLogger will execute the measurement of its sensors and then send this data to a computer using UDP Protocol. Another program that written in python (XDKLoggerDB) will be executed to receive the data that are being sent by the XDK. This program is written fully in C programming language.

The use case of this device with this software is describe in the following use case diagram:

![alt text](https://github.com/AbiyyuMufti/XDKDataLogger/blob/master/images/USE%20CASE%20DIAGRAM.png "UML Use Case Diagram")

## Required tools
- BOSCH XDK
- Router
- PC with XDKLoggerDB Program
- XDK Workbench for compiling and flashing this program to XDK Device and for troubleshooting
Optional:
- Internet for SNTP time stamp.
- DS3231 RTC for real time clock time stamp.
- SD Card for Configuration.


## Features
### Sensor Measurement
The sensor inside the XDK will do the measurement, which measurement that will be executed can be configured by the user by inserting an SD Card before starting the measurement. The Bosch XDK has multiple sensors: Accelerometer, Gyroscope, Magnetometer, Humidity, Temperature, Pressure, and Noise sensor.
### UDP Data Transfer
The measurement data will be sent to a python program in a computer via WIFI with UDP protocol. The data transfer is fast and can also be done in different time configuration for each sensor. So, you can read all the measurement data at the same time and send it together, or you do the measurement for each sensor in different frequencies and send it separately.
### Time Stamp
To make the measurement clearer, the time stamp of measurement will also be sent from Bosch XDK. To get the time stamp you can use SNTP protocol to get time from Internet or using an external RTC Device that connected via I2C-Wire. If none of both options available, the XDK can also sent a dummy time that will sign the XDKLoggerDB Program to use its receiving time as the time stamp.
#### Requirement if Using SNTP
To use SNTP protocol, internet is prerequisite because without it you cannot synchronize to the network time.
#### Requirement if Using RTC
Real Time Clock DS3231 is compatible for this software. A simplified library for this RTC-Device in C programming language is also written inside this program. This library simplified the reading from the time value from DS3231 using I2C protocol.
### Configuration using SD Card
You can insert SD Card to the XDK before you turn it on. This SD Card should have a json file named “xdk_config.json” that stored information about the needed configuration.
This file can like be written as follow:
```
{
  "AllAtOnce" : false,
  "DefaultTime" : 10,
  "Acc" : {
    "active" : 0,
    "time" : 10, 
    "sensors" : 0
  },
  "Gyr" : {
    "active" : 0, 
    "time" : 10, 
    "sensors" : 0
  },
  "Mag" : {
    "active" : 1, 
    "time" : 20
  },
  "Env" : {
    "active" : 0, 
    "time" : 25
  },
  "Lig" : {
    "active" : 1, 
    "time" : 500 
  },
  "Aku" : {
    "active" : 1, 
    "time" : 10 
  }
}
```
With this file, you can choose which sensors should be active and how oft period the measurement data will be read. 
You can specified if the sensors measurement should be read at the same time with default time, or if each sensor measurement will be read differently.

**For each sensor:**
| Argument | Value | Note                                       |
|----------|:-----:| ------------------------------------------ |
| active   | 0     | Inaktive, sensor will not be setup         |
|          | 1     | Aktive                                     |
| time     | number| Time in milliseconds, > 10ms is recomended |
|          |       | fastes time tested is 2ms                  |
| sensor   | 0/1   | For Acc 0 --> using BMA280                 |
|          |       | For Acc 1 --> using BMI160                 |
|          |       | For Gyr 0 --> using BMG160                 |
|          |       | For Gyr 1 --> using BMI160                 |

If you didn’t use the SD card to do the configuration then the default program execution (all sensors with period of 100 ms for the measurement will be used).

The SD Card is only necessary during the activation of the XDK, after the measurement is running, you can eject the SD Card. If you need to reconfigure, just turn the XDK, change the xdk_config.json file inside the SD Card and put it inside the XDK before turning it on again.

*ISSUE: for some reason I cannot add another data inside this json, because after that the program will crash and cannot read the file anymore. This fix is needed to add for example the configuration of the WIFI Credential, UDP IP Address and Port, or SNTP URL that will be used.*

## Using Manual
1. If the XDK has not been flashed with this program, just flash it using XDK-Workbench Version 3.6.
2. Make sure the WIFI Credential, UDP IP Address are correct before flashing it to the device.
3. Turn the XDK of using its switch.
4. You can configure the XDK using an SD Card before turning it on.
5. Make sure that there are a router and a pc with XDKLoggerDB program inside it and connected to the router. 
6. Then Turn on the XDK. 

## XDK Program Flow
![XDKDataLogger Overview](https://github.com/AbiyyuMufti/XDKDataLogger/blob/master/images/Main%20Program.png)

XDKDataLogger is written in C Language and based on Real Time Operating System (FreeRTOS). 
This program can be simplified to 3 Functions that will be enqueued one after another: AppControllerSetup, AppControllerEnable, AppControllerFire.

The structure of this program can be described generally as the following diagram:

### AppControllerSetup
![AppControllerSetup](https://github.com/AbiyyuMufti/XDKDataLogger/blob/master/images/AppControllerSetup.png)

In this function, the necessary setup for the XDK will be executed. At the very first, the SD Card storage will be setup and enabled, so that the “xdk_config.json” file can be read at every start of the XDK after turning it on. The function setup_config_reader() is responsible of the reading the json file. If there are no SD Card detected, the default setup will be used for the entire program execution. 
After that, all other necessary configuration for I2C, connectivity (WIFI, UDP, SNTP), and sensors will be setup.
Lastly the next AppControllerEnable will be enqueued.

#### XDK Configuration and Parameter Handling
As explained before, you can configure the XDK using json file inside an SD Card. The file will be read during the setup and will be parsed. We are using JSON Format because XDK has a built-in utility that can use cJSON. The parsed data will then be stored in a struct called XDKConfig.
After that, before the setup of the sensor, the SensorSetup struct should be prepared. This struct is defined first hard coded for the default, but after the reading of json file will be modified. Then the SensorSetup struct will be passed as parameter for the function that responsible for the XDK sensor setup.

*SUGGESTED IMPROVEMENT: in theory you can also add the WIFI Credential, UDP IP and its Port, and other information inside the json file. Then the configuration for them will be much easier and the device become more portable. Unfortunately, an unexpected error occurs when the json text is much bigger than it is now. This makes the reading of json file impossible. A bug fix for this problem is needed.*

### AppControllerEnable
![AppControllerEnable](https://github.com/AbiyyuMufti/XDKDataLogger/blob/master/images/AppControllerEnable.png)
In this function the configured WIFI and sensors enabled. After that, the main tasks AppControllerFire will be created and then started. In case the SNTP for the TimeStamp are used, another task AppSynchSNTP is also necessary to be created and started. This task will synchronize the network time. Another Task for it is necessary, so the timestamp value can be pooled out inside AppControllerFire without fear to disrupt the measurement reading flow.

### AppControllerFire
![AppControllerFire](https://github.com/AbiyyuMufti/XDKDataLogger/blob/master/images/Main%20Program.png)
This is the main task of this program. A forever loop will be run in this function. In every loop the measurement will read depends on the reading time. At first it will be differentiated if the measurement is for all the active sensor at the same time or if each sensor has its own measurement time. The result of the reading measurement will be stored in a C string buffer. Because the sensor reading measurement depend on the reading time, the data will not be sent before its ready. After the data is ready the timestamp will also be appended to the buffer. Lastly the buffer will be sent via UDP protocol.

#### Getting Timestamp over SNTP
As explained before, if you are using SNTP Protocol, there are a need of a new task named AppSynchSNTP. Here the time will be pooled out from the network time and synchronize. 
The disadvantage using this protocol is that the internet is necessary and if the synchronization failed then the right time cannot be retrieved. Moreover, in HSKA, we will unfortunately using a router that do not have internet connection so using of SNTP is also not possible.

#### Getting Timestamp over RTC Using DS3231
An alternative of SNTP Protocol is using a Real Time Clock. A simple library that utilizes the I2C built-in functions from XDK are written to simplify the reading of DS3231 RTC. The file library is in Header file DS3231Handler.h and implemented in DS3231Handler.c
