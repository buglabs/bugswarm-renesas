/*-------------------------------------------------------------------------*
 * File:  App_PassThroughSPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <HostApp.h>
#include <system/platform.h>
#include <mstimer.h>
#include <drv/Glyph/lcd.h>
#include <GainSpan_SPI.h>
#include <ATCmdLib/AtCmdLib.h>

/*---------------------------------------------------------------------------*
 * Routine:  App_PassThroughSPI
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit in a pass through mode where SCI2 bytes are sent to
 *      SCI6 and visa-versa.  Additionally, the baud rate of the module
 *      on SCI6 is raised to 115200 baud.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_PassThroughSPI(void)
{
    uint8_t c;
    const uint8_t msg[] = "PassThroughSPI:\r\n";

    ClearLCD();
    DisplayLCD(LCD_LINE1, "EVALUATE GS");
    DisplayLCD(LCD_LINE3, "CONNECT DB9");
    DisplayLCD(LCD_LINE4, "BAUD: 115200");
    DisplayLCD(LCD_LINE5, " TEST AT");
    DisplayLCD(LCD_LINE6, " COMMANDS");

    AtLibGs_SetEcho(ATLIBGS_ENABLE);
    Console_UART_SendData(msg, sizeof(msg));
    while (1) {
        if (Console_UART_ReceiveByte(&c))
            while (!GainSpan_SPI_SendByte(c))
                GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
        if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, &c))
            if (c != GAINSPAN_SPI_CHAR_IDLE)
                while (!Console_UART_SendByte(c))
                    {}
        GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  App_PassThroughSPI.c
 *-------------------------------------------------------------------------*/
