/*-------------------------------------------------------------------------*
 * File:  App_ProgramMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      This code puts the unit into a mode that allows the
 *      GainSpan Flash Program (gs_flashprogram) to program the
 *      GainSpan module through the Renesas RL78 RDK's serial port(on board DB9).
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
#include <mstimer.h>
#include "system/Switch.h"
extern void CloseLCD(void);

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_IO_ProgMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into programming mode by putting the control pin into
 *      program mode and mimicing the TX/RX lines on UART1 with the
 *      RX/TX lines of UART3. Set GS1011 GPIO27 high.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void GainSpan_IO_ProgMode(void)
{
    CloseLCD();
    SAU0EN = 0U;

    // WIFI PROGRAM Pin
    P5 |= 1<<7;        // P57 (WIFI-PGM) HIGH for program mode
    //PMC5 &= ~(1<<7);
    PM5 &= ~(1<<7);    // output mode

    /* Setup the pins to mimic TX from one side to the RX on the other side */

    /* DB9-RXD (P03) pin as input */
    PMC0 &= ~(1<<3);
    PM0 |= (1<<3);
    /* DB9-TXD (P02) pin as output */
    PMC0 &= ~(1<<2);
    P0 |= (1<<2);
    PM0 &= ~(1<<2);

    /* WIFI-RXD (P143) pin as input */
    PMC14 &= ~(1<<3);
    PM14 |= (1<<3);
    /* WIFI-TXD (P144) pin as output */
    PMC14 &= ~(1<<4);
    P14 |= (1<<4);
    PM14 &= ~(1<<4);

    // Reset GainSpan Module using EXTRST (P130)
    P13 |= (1<<0);
    MSTimerDelay(500);
  //  delay_ms(500);
    P13 &= ~(1<<0);

    InitialiseLCD();
    DisplayLCD(LCD_LINE1, "GS PROG MODE");
    DisplayLCD(LCD_LINE3, "Press SW3 to");
    DisplayLCD(LCD_LINE4, "RUN MODE");

    while (1) {

        // Copy WIFI-RXD (P143) to DB9-TXD (P02)
        P0_bit.no2 = P14_bit.no3;

        // Copy DB9-RXD (P03) to WIFI-TXD (P144)
        P14_bit.no4 = P0_bit.no3;
        if(Switch3IsPressed())
        {
          break;                         // stop Program Mode, switch to Run mode
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_IO_ProgMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into run mode by putting the control pin into
 *      program mode and mimicing the TX/RX lines on UART1 with the
 *      RX/TX lines of UART3. Then set GS1011 GPIO27 low.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/

void GainSpan_IO_RunMode(void)
{
    CloseLCD();
    SAU0EN = 0U;

    // WIFI PROGRAM Pin
    P5 &= ~(1<<7);        // P57 (WIFI-PGM) Low for run mode
    //PMC5 &= ~(1<<7);
    PM5 &= ~(1<<7);    // output mode

    // Reset GainSpan Module using EXTRST (P130)
    P13 |= (1<<0);
    MSTimerDelay(500);
 //   delay_ms(500);
    P13 &= ~(1<<0);

    InitialiseLCD();
    DisplayLCD(LCD_LINE1, "GS RUN MODE");
    DisplayLCD(LCD_LINE3, "Press SW2 to");
    DisplayLCD(LCD_LINE4, "PROG MODE");

    while (1) {

        // Copy WIFI-RXD (P143) to DB9-TXD (P02)
        P0_bit.no2 = P14_bit.no3;

        // Copy DB9-RXD (P03) to WIFI-TXD (P144)
        P14_bit.no4 = P0_bit.no3;

        if(Switch2IsPressed())
        {
          break;                       // stop Run mode, switch Program Mode
        }
    }
}
/*---------------------------------------------------------------------------*
 * Routine:  App_ProgramMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Run infinitively and switch between the Gainspan WiFi module Program
 *      and Run modes.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_ProgramMode(void)
{
    // IT Interrupt Priority HIGH
    ITPR1 = 0U;
	ITPR0 = 0U;
    while (1U)
	{
      GainSpan_IO_ProgMode();
      GainSpan_IO_RunMode();
	}
}

/*-------------------------------------------------------------------------*
 * End of File:  App_ProgramMode.c
 *-------------------------------------------------------------------------*/
