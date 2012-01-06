/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved. */
/* Code written for Renesas by Future Designs, Inc. www.teamfdi.com */

/******************************************************************************
* File Name : YRDKRL78_RSPI0.c
* Version : 1.00
* Device(s) : RSPI0 Communications
* Tool-Chain : Glyph - The Generic API for Graphics SPI API version 1.00
* H/W Platform : RL78 Group CPU
* Description : Glyph API RL78 RSPI Transmitter 0 Operation channel 3
* Routines for SPI code running on RL78 MCU, RSPI0.
* This program uses RSPI0.  The Chip Select for the flash device (LCD)
* is set to PC_2.
*       Manufacturer: Renesas
*       Communications Protocol: SPI 3-Wire
*                                Transmitt only
*                                MASTER
*       Transmitter Number: 0
*       channel: 3
*       Chip Select Port: PC.2
*       RSPI Clock Speed: 25MHz
******************************************************************************
* History :
******************************************************************************/

/******************************************************************************
Includes “YRDKRL78 RSPI Includes”
******************************************************************************/
#include "CG_macrodriver.h"
#include "CG_serial.h"
#include "CG_system.h"
#include "YRDKRL78_LCD.h"
#include "RDKRL78_spi.h"

extern volatile UCHAR G_SPI_SendingData; //spi busy flag

/******************************************************************************
* ID : 60.0
* Outline : YRDKRL78_RSPIOpen
* Include : YRDKRL78_RSPI0.h
* Function Name: YRDKRL78_RSPIOpen
* Description : Open and setup the communications channel RSPI0.
* Argument : aHandle - the Glyph handle to setup for the LCD and Communications.
* Return Value : 0=success, not 0= error
* Calling Functions : GlyphCommOpen
******************************************************************************/
T_glyphError YRDKRL78_RSPIOpen(T_glyphHandle aHandle)
{
    SPI_Init();
    
    return GLYPH_ERROR_NONE ;
}

/******************************************************************************
* ID : 66.0
* Outline : SetSendCommand
* Include : YRDKRL78_RSPI0.h
* Function Name: SetSendCommand
* Description : Set the RS line of the LCD.  The RS line is connected to 
* output port P5.1.  Setting this output to 0 will allow data flow in the LCD.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendCommand(void)
{
    P1 &= ~(1<<5); // Assert P15 (LCD RS)
}

/******************************************************************************
* ID : 67.0
* Outline : SetSendData
* Include : YRDKRL78_RSPI0.h
* Function Name: SetSendData
* Description : Clear the RS line of the LCD.  The RS line is connected to
* output port P5.1.  Setting this output to 1 will stop data flow in the LCD.
* When data flow is stopped in the LCD the data is internally copied to the
* data buffer for use.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendData(void)
{
    P1 |= (1<<5); // Deassert P15 (LCD RS)
}

/******************************************************************************
* ID : 70.0
* Outline : YRDKRL78_CommandSend
* Include : YRDKRL78_RSPI0.h
* Function Name: YRDKRL78_CommandSend
* Description : Send a command define to the LCD.  The defines for all commands
* available to send to the ST7579 LCD controller are defined in the header
* for this file RL78_LCD.h.  The nCommand parameter should always be
* a define from that location.
* Argument : cCommand - 8 bits of data to be used as a command to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void YRDKRL78_CommandSend(int8_t cCommand)
{
    SetSendCommand() ;

    SPI_Send(SPI_LCD, (uint8_t *)&cCommand, 1);

    SetSendData() ;
}

/******************************************************************************
* ID : 71.0
* Outline : YRDKRL78_DataSend
* Include : YRDKRL78_RSPI0.h
* Function Name: YRDKRL78_DataSend
* Description : Send 8 bits of data to the LCD RAM at the current location.
* The current location is determined and set with local functions.
* Argument : cCommand - 8 bits of data to be sent to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void YRDKRL78_DataSend(int8_t cData)
{
    SetSendData() ;

    SPI_Send(SPI_LCD, (uint8_t *)&cData, 1);

    SetSendData() ;
}

