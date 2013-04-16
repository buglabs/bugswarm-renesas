/*-------------------------------------------------------------------------*
 * File:  EEPROM.c
 *-------------------------------------------------------------------------*
 * Description:
 *     RX62N Flash Driver.  Provides functions to read, write, and erase
 *     the internal EEPROM.  Used to store non-volatile information.
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include "../GSHAL/mstimer.h"
#include "drv/I2C.h"

#define EEPROM_SIZE     64000   // 64KB (512Kb)
#define EEPROM_ADDR     0xA0u   // Slave Address (Write)
#define EEPROM_A16      0x02    // A16 of memory address

#define EEPROM_TIMEOUT  20      // Millisecond Timeout on I2C bus

#define EEPROM_BYTES_PER_WRITE  32

/*******************************************************************************
 * Outline      : EEPROM_Open
 * Description  : 
 * Argument     : none
 * Return value : none
 *******************************************************************************/
void EEPROM_Open(void)
{
    /* Do nothing for now */
}

/*******************************************************************************
 * Outline      : EEPROM_ChipErase
 * Description  : This function enters a for loop, and erases blocks of data
 *                 EEPROM memory each iteration until all blocks have been erased.
 * Argument     : none
 * Return value : none
 *******************************************************************************/
uint8_t EEPROM_ChipErase(void)
{
    return 0;
}

/*******************************************************************************
 * Outline      : EEPROM_Write
 * Description  : This function writes the given contents to the
 *                 EEPROM, at the given location.
 * Argument     : offset -- Offset byte from start of EEPROM
 *                aData -- Pointer to bytes to write to EEPROM
 *                aSize -- number of bytes to write to EEPROM
 * Return value : 0 = success, else failure
 *******************************************************************************/
uint8_t EEPROM_Write(uint16_t offset, uint8_t *aData, uint16_t aSize)
{
    I2C_Request r;
    uint32_t timeout = MSTimerGet();
    uint8_t writeData[EEPROM_BYTES_PER_WRITE+2];
    uint16_t i, j, bytesToWrite;

    r.iAddr = EEPROM_ADDR>>1;
    r.iSpeed = 100; /* kHz */
    
    // Write Data in groups of size defined by EEPROM_BYTES_PER_WRITE
    for(i=0; i<aSize; i+=EEPROM_BYTES_PER_WRITE) {
      
        // Data Address in the EEPROM to write to
        writeData[0] = (uint8_t)(i + offset)<<8;
        writeData[1] = (uint8_t)(i + offset);
        
        for(j=0; j<EEPROM_BYTES_PER_WRITE; j++) {
            writeData[2+j] = aData[i+j];
        }
        
        if((aSize - i) < EEPROM_BYTES_PER_WRITE)
            bytesToWrite = aSize - i;
        else
            bytesToWrite = EEPROM_BYTES_PER_WRITE;
        
        r.iWriteData = writeData;
        r.iWriteLength = 2+bytesToWrite;
        r.iReadData = 0;
        r.iReadLength = 0;
    
        I2C_Start();
        I2C_Write(&r, 0);
        MSTimerDelay(10); // Part requires a 5ms to process a data write
        while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < EEPROM_TIMEOUT))
            {}
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write EEPROM 
 * Inputs:
 *      uint16_t    add,
        char *  pBuff
 * Outputs:
 *      
 *---------------------------------------------------------------------------*/
void EEPROM_WriteStr(uint16_t addr, char *pdata)
{
    /* Declare error flag */
    uint8_t send[256];
    I2C_Request r;
    uint16_t len = 2;

    send[0] = addr & 0xFF00;
    send[1] = addr & 0x00FF;

    while (*pdata != '\0')
    {
      send[len] = (uint8_t)*pdata;
      len++;
      pdata++;
    }

    uint32_t timeout = MSTimerGet();

    r.iAddr = EEPROM_ADDR>>1;
    r.iSpeed = 100; /* kHz */
    r.iWriteData = send;
    r.iWriteLength = len;
    r.iReadData = 0;
    r.iReadLength = 0;

    I2C_Start();
    I2C_Write(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}
}

/*---------------------------------------------------------------------------*
 * Routine:  EEPROM_Seq_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the value of the address and return the data .
 * Inputs:
 *      void
 * Outputs:
 *      uint16_t -- temperature with 4 bits of fraction and 12 bits of
 *          integer.
 *---------------------------------------------------------------------------*/
int16_t EEPROM_Seq_Read(uint16_t addr,uint8_t *pdata, uint16_t r_lenth)
{
    uint8_t target_address[2];
    uint32_t timeout = MSTimerGet();
    I2C_Request r;
    int16_t result = 0;

    target_address[0] = addr & 0xFF00;
    target_address[1] = addr & 0x00FF;     

    r.iAddr = EEPROM_ADDR >> 1;
    r.iSpeed = 100;
    r.iWriteData = target_address;
    r.iWriteLength = 2;
    r.iReadData = pdata;
    r.iReadLength = r_lenth;
    I2C_Write(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}
    I2C_Read(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}

    result = 1;

    return result;
}

/*******************************************************************************
 * Outline      : EEPROM_Read
 * Description  : This function writes the given contents to the
 *                 EEPROM, at the given location.
 * Argument     : offset -- Offset byte from start of EEPROM
 *                aData -- Pointer to bytes to write to EEPROM
 *                aSize -- number of bytes to write to EEPROM
 * Return value : 0 = success, else failure
 *******************************************************************************/
uint8_t EEPROM_Read(uint16_t offset, uint8_t *aData, uint16_t aSize)
{
    uint8_t writeData[2];
    uint32_t timeout = MSTimerGet();
    I2C_Request r;

    writeData[0] = (uint8_t)offset<<8;
    writeData[1] = (uint8_t)offset;
    
    r.iAddr = EEPROM_ADDR>>1;
    r.iSpeed = 100;
    r.iWriteData = writeData;
    r.iWriteLength = 2;
    r.iReadData = aData;
    r.iReadLength = aSize;
    
    I2C_Start();
    I2C_Write(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < EEPROM_TIMEOUT))
        {}
    I2C_Read(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < EEPROM_TIMEOUT))
        {}

    return 0;
}

/*******************************************************************************
 * Outline      : EEPROM_Erase
 * Description  : This function enters a for loop, and erases blocks of data
 *                 EEPROM memory that cover the given address range.  This
 *                 routine is not smart enough to keep the existing data
 *                 in a block that is erased.
 * Argument     : none
 * Return value : none
 *******************************************************************************/
uint8_t EEPROM_Erase(uint16_t offset, uint16_t aSize)
{
    I2C_Request r;
    uint32_t timeout = MSTimerGet();
    uint8_t writeData[EEPROM_BYTES_PER_WRITE+2];
    uint16_t bytesToWrite, i, j;

    r.iAddr = EEPROM_ADDR>>1;
    r.iSpeed = 100; /* kHz */
    
    // Write Data in groups of size defined by EEPROM_BYTES_PER_WRITE
    for(i=0; i<aSize; i+=EEPROM_BYTES_PER_WRITE) {
      
        // Data Address in the EEPROM to write to
        writeData[0] = (uint8_t)(i + offset)<<8;
        writeData[1] = (uint8_t)(i + offset);
        
        for(j=0; j<EEPROM_BYTES_PER_WRITE; j++) {
            writeData[2+j] = 0x00;
        }
        
        if((aSize - i) < EEPROM_BYTES_PER_WRITE)
            bytesToWrite = aSize - i;
        else
            bytesToWrite = EEPROM_BYTES_PER_WRITE;
        
        r.iWriteData = writeData;
        r.iWriteLength = 2+bytesToWrite;
        r.iReadData = 0;
        r.iReadLength = 0;
    
        I2C_Start();
        I2C_Write(&r, 0);
        MSTimerDelay(5); // Part requires a 5ms to process a data write
        while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
            {}
    }
    
    return 0;
}

#define EEPROM_TEST_DATA_ADDR       98
#define EEPROM_TEST_BUFFER_SIZE     8
bool EEPROM_Test()
{
    uint8_t writeData[EEPROM_TEST_BUFFER_SIZE];
    uint8_t readData[EEPROM_TEST_BUFFER_SIZE];
    uint16_t i;
    
    for(i=0; i<EEPROM_TEST_BUFFER_SIZE; i++){
        writeData[i] = i;
    }
    
    EEPROM_Read(EEPROM_TEST_DATA_ADDR, readData, EEPROM_TEST_BUFFER_SIZE);
    EEPROM_Write(EEPROM_TEST_DATA_ADDR, writeData, EEPROM_TEST_BUFFER_SIZE);
    EEPROM_Read(EEPROM_TEST_DATA_ADDR, readData, EEPROM_TEST_BUFFER_SIZE);
    
    for(i=0; i<EEPROM_TEST_BUFFER_SIZE; i++){
        if(writeData[i] != readData[i]){
            return false;
        }
    }
    
    return true;
}

/*-------------------------------------------------------------------------*
 * End of File:  EEPROM.c
 *-------------------------------------------------------------------------*/
