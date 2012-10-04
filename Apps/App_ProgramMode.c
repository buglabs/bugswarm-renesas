/*-------------------------------------------------------------------------*
 * File:  App_ProgramMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      This code puts the unit into a mode that allows the
 *      GainSpan Flash Program (gs_flashprogram) to program the
 *      GainSpan module through the Renesas RX62N RDK's serial port.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <HostApp.h>
#include <system/platform.h>
#include <system/GainSpan_IO.h>
#include <drv/Glyph/lcd.h>
#include <drv/Timer.h>

/*---------------------------------------------------------------------------*
 * Routine:  App_ProgramMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into programming mode by putting the control pin into
 *      program mode and mimicing the TX/RX lines on SCI2 with the
 *      RX/TX lines of SCI6.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_ProgramMode(void)
{
    DisplayLCD(LCD_LINE1, "GS PROGRAM");
    Timer_Stop();

    // Enter WAB Programming Mode - Will not exit
    GainSpan_IO_ProgMode();
}

/*-------------------------------------------------------------------------*
 * End of File:  App_ProgramMode.c
 *-------------------------------------------------------------------------*/
