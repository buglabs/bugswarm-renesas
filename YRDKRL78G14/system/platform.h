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
#include "drv\UART.h"
#include "system\EEPROM.h"

// The RL78 has 4KB of RAM
#define APP_MAX_RECEIVED_DATA           (256)
#define ATLIBGS_TX_CMD_MAX_SIZE         (256)
#define ATLIBGS_RX_CMD_MAX_SIZE         (512)
#define GAINSPAN_SPI_RX_BUFFER_SIZE     (256)
#define GAINSPAN_SPI_TX_BUFFER_SIZE     (128)
#define CONSOLE_BUFFER_SIZE             (256)
#define UART0_RX_BUFFER_SIZE            (64)
#define UART0_TX_BUFFER_SIZE            (64)
#define UART1_RX_BUFFER_SIZE            (128)
#define UART1_TX_BUFFER_SIZE            (128)

#define POTENTIOMETER_CHANNEL            8   // ADC_CHANNEL_4

// Error Code LED
#define ERROR_LED_ON()  P5 |= 0x10
#define ERROR_LED_OFF()  P5 &= ~0x10

// Console (UART1) driver linkage
#define Console_UART_Start(baud)              UART1_Start(baud)
#define Console_UART_Stop()                   UART1_Stop()
#define Console_UART_SendByte(aByte)          UART1_SendByte(aByte)
#define Console_UART_SendData(aData, aLen)    UART1_SendData(aData, aLen)
#define Console_UART_ReceiveByte(aByte)       UART1_ReceiveByte(aByte)
#define Console_UART_IsTransmitEmpty()        UART1_IsTransmitEmpty()

// Application Header (UART3) driver linkage
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
