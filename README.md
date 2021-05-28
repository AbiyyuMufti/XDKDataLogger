# XDKDataLogger

## Description

Using Bosch XDK Measurement and logging it in to the database.

The global program flow of this project is describe in the folowing image:

![Global Program Flow](images\GlobalProgramFlow.png?raw=true "Program Flow XDK Data Logger and Python Program to Store into Database")

The XDK will measure the read its sensor data and pack it into a simplyfied data packet and will be sent via udp protocol wirelessly to a PC. 
In the PC a Python Script should run a program that retrieve the data and store it into the local database.

// In this file, a simplyfied documentation about the

 

## Using Manual

## XDK Program Flow

XDKDataLogger is written in C Language and its structured as follow
![XDKDataLogger Overview](images\XDKDataLoggerProgramOverview.png?raw=true)

The program can be divided in to 3 processes
1st Process is to read the measurement and send it via udp
2nd Process is to get the time from the SNTP or from the RTC Clock
3rd Process is to get the configuration from the SD Card and store the value inside sd card as csv 



### AppControllerSetup
### AppControllerEnable
### AppControllerFire
### Timer Tasks
### SNTP Sync Tasks
### Real Time Clock DS3231
### Parameter Configuration in JSON

## 


