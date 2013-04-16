/*-------------------------------------------------------------------------*
 * File:  UART0.h
 *-------------------------------------------------------------------------*
 * Description:
 *     FIFO driven UART0 driver for RL78.
 *-------------------------------------------------------------------------*/
#ifndef _UART_H
#define _UART_H

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RL78G14RDK_UART_Start(uint32_t UART0_baud, uint32_t UART1_baud);

void Create_UART0(uint32_t UART0_baud);
void UART0_Stop(void);
bool UART0_ReceiveByte(uint8_t *aByte);
bool UART0_SendByte(uint8_t aByte);
uint32_t UART0_SendData(const uint8_t *aData, uint32_t aLen);
void UART0_SendDataBlock(const uint8_t *aData, uint32_t aLen);
bool UART0_IsTransmitEmpty(void);
void UART0_SetBaudRate(uint32_t baud);

void Create_UART1(uint32_t UART1_baud);
void UART1_Stop(void);
bool UART1_ReceiveByte(uint8_t *aByte);
bool UART1_SendByte(uint8_t aByte);
uint32_t UART1_SendData(const uint8_t *aData, uint32_t aLen);
void UART1_SendDataBlock(const uint8_t *aData, uint32_t aLen);
bool UART1_IsTransmitEmpty(void);
void UART1_SetBaudRate(uint32_t baud);


#endif // _UART_H
/*-------------------------------------------------------------------------*
 * End of File:  UART0.h
 *-------------------------------------------------------------------------*/
