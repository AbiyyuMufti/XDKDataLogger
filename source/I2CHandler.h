/*
 * I2CHandler.h
 *
 *  Created on: 23 Apr 2021
 *      Author: abiyy
 */

#ifndef SOURCE_I2CHANDLER_H_
#define SOURCE_I2CHANDLER_H_

#include "BCDS_Retcode.h"
#include "BCDS_CmdProcessor.h"


Retcode_T I2C_Initialize(CmdProcessor_T * handle);
Retcode_T I2C_DeInitiazle(void);
Retcode_T I2C_Read(uint8_t slaveAddr, uint8_t regAddr, uint8_t *readVal, uint8_t readLength);
Retcode_T I2C_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t *writeVal, uint8_t writeLength);


typedef struct
{
	bool address[127];
	uint8_t number;
} scannedI2CDevices;


void I2C_Scan(scannedI2CDevices * dev);


#endif /* SOURCE_I2CHANDLER_H_ */
