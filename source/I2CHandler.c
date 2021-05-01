/*
 * I2CHandler.c
 *
 *  Created on: 23 Apr 2021
 *      Author: abiyy
 */

#include "I2CHandler.h"
#include "BCDS_I2CTransceiver.h"
#include "BSP_ExtensionPort.h"
#include "BCDS_MCU_I2C.h"
#include "BCDS_Basics.h"
#include "task.h"
#include <stdio.h>

static I2cTranceiverHandle_T I2CInstance;


/* This function is the callback function triggered on both success or error I2C transfer */
static void I2C_McuCallback(I2C_T i2c, struct MCU_I2C_Event_S event)
{
    if (I2CInstance.I2CHandle == i2c)
    {
        I2CTransceiver_LoopCallback(&I2CInstance, event);
    }
    else
    {
        Retcode_RaiseErrorFromIsr(RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM));
    }
}


Retcode_T I2C_Initialize(CmdProcessor_T * handle)
{
    BCDS_UNUSED(handle);

    Retcode_T retcode = RETCODE_OK;
    HWHandle_T HandleI2c = NULL;
    retcode = BSP_ExtensionPort_ConnectI2c();
    if (RETCODE_OK == retcode)
    {
        retcode = BSP_ExtensionPort_SetI2cConfig(BSP_EXTENSIONPORT_I2C_MODE, (uint32_t) BSP_EXTENSIONPORT_I2C_STANDARD_MODE, NULL);
    }
    if (RETCODE_OK == retcode)
    {
        /* Get the i2c  handle */
        HandleI2c = BSP_ExtensionPort_GetI2cHandle();
    }
    if (NULL == HandleI2c)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_I2C_Initialize(HandleI2c, I2C_McuCallback);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = I2CTransceiver_Init(&I2CInstance, HandleI2c);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = BSP_ExtensionPort_EnableI2c();
    }
    return retcode;
}


Retcode_T I2C_DeInitiazle(void)
{
    Retcode_T retcode = RETCODE_OK;
    HWHandle_T HandleI2c;
    /*Get the i2c  handle */
    HandleI2c = BSP_ExtensionPort_GetI2cHandle();
    if (NULL == HandleI2c)
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = MCU_I2C_Deinitialize(HandleI2c);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = I2CTransceiver_Deinit(&I2CInstance);
    }
    if (RETCODE_OK == retcode)
    {
        retcode = BSP_ExtensionPort_DisableI2c();
    }
    if (RETCODE_OK == retcode)
    {
        retcode = BSP_ExtensionPort_DisonnectI2c();
    }
    return retcode;
}


Retcode_T I2C_Read(uint8_t slaveAddr, uint8_t regAddr, uint8_t *readVal, uint8_t readLength)
{
	return I2CTransceiver_Read(&I2CInstance, slaveAddr, regAddr, readVal, readLength);
}


Retcode_T I2C_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t *writeVal, uint8_t writeLength)
{
	return I2CTransceiver_Write(&I2CInstance, slaveAddr, regAddr, writeVal, writeLength);
}


void I2C_Scan(scannedI2CDevices * dev)
{
	Retcode_T retcode;
	uint8_t getChipIdData;
	uint8_t devices = 0;
    for(uint8_t address = 1; address < 127; address++ )
    {
        retcode = I2C_Read(address, 0x00, &getChipIdData, UINT8_C(1));
        if(RETCODE_OK == retcode)
        {
        	dev->address[address] = 1;
        	printf("Get in adr: 0x%02X \n", address);
        	dev->number++;
        }
        else
        {
        	dev->address[address] = 0;
        }
    }
}

