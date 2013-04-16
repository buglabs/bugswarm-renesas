/*-------------------------------------------------------------------------*
 * File:  GainSpan_IO.c
 *-------------------------------------------------------------------------*
 * Description:
 *      This code puts the unit into a mode that allows the
 *      GainSpan Flash Program (gs_flashprogram) to program the
 *      GainSpan module through the Renesas RL78 RDK's serial port.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "platform.h"
#include "drv/SPI.h"
#include "GainSpan_IO.h"

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_IO_IsDataReady
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into programming mode by putting the control pin into
 *      program mode and mimicing the TX/RX lines on UART2 with the
 *      RX/TX lines of UART6.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
bool GainSpan_IO_IsDataReady(uint8_t channel)
{
    /* High true */
    if( (channel == SPI_APPHEADER_CHANNEL) && (P7 & (1<<7)) )     //  Changed to P7.7 for G14 board
        return true;
    else if( (channel == SPI_PMOD1_CHANNEL) && (P7 & (1<<6)) )
        return true;
    else if( (channel == SPI_PMOD2_CHANNEL) && (P7 & (1<<7)) )
        return true;
    else 
        return false;
}

/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_IO.c
 *-------------------------------------------------------------------------*/
