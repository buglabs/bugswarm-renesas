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

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_IO_ProgMode
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
void GainSpan_IO_ProgMode(void)
{
    // WIFI GPIO Pin
    P14 |= 1<<7;        // P147 (WIFI-GPIO26) HIGH for program mode
    PMC14 &= ~(1<<7);
    PM14 &= ~(1<<7);    // output mode

    /* Setup the pins to mimic TX from one side to the RX on the other side */
    /* RxD0 (P11) pin as input */
    /* TxD0 (P12) pin as output */
    /* TxD2 (P13) pin as output */
    /* RxD2 (P14) pin as input */
    PM1 = 0xF2;

    while (1) {
        // Copy RXD2 (P14) to TXD0 (P12)
        P1_bit.no2 = P1_bit.no4;

        // Copy RXD0 (P11) to TXD2 (P13)
        P1_bit.no3 = P1_bit.no1;
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_IO.c
 *-------------------------------------------------------------------------*/
