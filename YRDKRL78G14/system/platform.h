/*-------------------------------------------------------------------------*
 * File:  platform.h
 *-------------------------------------------------------------------------*
 * Description:
 *     This file is referenced by the drivers for necessary compile time
 *     options.  For most users, another .h file holds the compile
 *     time information and settings (e.g., YRDKRX62N.h)
 *-------------------------------------------------------------------------*/
#ifndef PLATFORM_H_
#define PLATFORM_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "YRDKRL78G14.h"
#include <ior5f104pj.h>
#include <ior5f104pj_ext.h>
#include "intrinsics.h"
#include "drv\UART0.h"
#include "drv\UART2.h"
#include "system\EEPROM.h"

// The RL78 has 4KB of RAM
#define APP_MAX_RECEIVED_DATA           (1500)
#define ATLIBGS_TX_CMD_MAX_SIZE         (256)
#define ATLIBGS_RX_CMD_MAX_SIZE         (1500)
#define GAINSPAN_SPI_RX_BUFFER_SIZE     (1500)
#define GAINSPAN_SPI_TX_BUFFER_SIZE     (128)
#define CONSOLE_BUFFER_SIZE             (1500)
#define UART0_RX_BUFFER_SIZE            (8)
#define UART0_TX_BUFFER_SIZE            (1500)
#define UART2_RX_BUFFER_SIZE            (8)
#define UART2_TX_BUFFER_SIZE            (8)

#define POTENTIOMETER_CHANNEL            8   // ANI8
#define MICROPHONE_CHANNEL      		 5   // ANI5

// Error Code LED
#define ERROR_LED_ON()  P5 |= 0x10
#define ERROR_LED_OFF()  P5 &= ~0x10

// Console (UART0) driver linkage
#define Console_UART_Start(baud)              UART0_Start(baud)
#define Console_UART_Stop()                   UART0_Stop()
#define Console_UART_SendByte(aByte)          UART0_SendByte(aByte)
#define Console_UART_SendData(aData, aLen)    UART0_SendData(aData, aLen)
#define Console_UART_ReceiveByte(aByte)       UART0_ReceiveByte(aByte)
#define Console_UART_IsTransmitEmpty()        UART0_IsTransmitEmpty()

// Application Header (UART2) driver linkage
#define GainSpan_UART_Start(baud)             UART2_Start(baud)
#define GainSpan_UART_Stop()                  UART2_Stop()
#define GainSpan_UART_SendByte(aByte)         UART2_SendByte(aByte)
#define GainSpan_UART_SendData(aData, aLen)   UART2_SendData(aData, aLen)
#define GainSpan_UART_ReceiveByte(aByte)      UART2_ReceiveByte(aByte)
#define GainSpan_UART_IsTransmitEmpty()       UART2_IsTransmitEmpty()

#define NV_Open                               EEPROM_Open
#define NV_Erase                              EEPROM_Erase
#define NV_ChipErase                          EEPROM_ChipErase
#define NV_Write                              EEPROM_Write
#define NV_Read                               EEPROM_Read

#endif // PLATFORM_H_
/*-------------------------------------------------------------------------*
 * End of File:  platform.h
 *-------------------------------------------------------------------------*/
