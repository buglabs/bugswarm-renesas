/*-------------------------------------------------------------------------*
 * File:  hwsetup.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Setup all the devices and ports for this hardware.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <system/platform.h>
#include "hwsetup.h"

/*---------------------------------------------------------------------------*
 * Routine:  HardwareSetup
 *---------------------------------------------------------------------------*
 * Description:
 *      Contains all the setup functions called at device restart
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void HardwareSetup(void)
{
    DI();
    PIOR0 = 0x00U;
    PIOR1 = 0x00U;
    ConfigureOutputPorts();
    ConfigureOperatingFrequency();
    ConfigureInterrupts();
    EnablePeripheralModules();
    CRC0CTL = 0x00U;
    IAWCTL = 0x00U;
    PMS = 0x00U;
    EI();
}

/*---------------------------------------------------------------------------*
 * Routine:  ConfigureOperatingFrequency
 *---------------------------------------------------------------------------*
 * Description:
 *      Configures the clock settings for each of the device clocks
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConfigureOperatingFrequency(void)
{
    volatile uint16_t w_count;
    uint8_t           temp_stab_set;
    uint8_t           temp_stab_wait; 

    /* Set fMX */
    CMC = _40_CGC_HISYS_OSC | _10_CGC_SUB_OSC | _01_CGC_SYSOSC_OVER10M | _00_CGC_SUBMODE_LOW;
    OSTS = _07_CGC_OSCSTAB_SEL18;
    MSTOP = 0U;
    temp_stab_set = _FF_CGC_OSCSTAB_STA18;
    
    do
    {
        temp_stab_wait = OSTC;
        temp_stab_wait &= temp_stab_set;
    }
    while (temp_stab_wait != temp_stab_set);
    
    /* Set fMAIN */
    MCM0 = 0U;
    /* Set fSUB */
    XTSTOP = 0U;

    /* Software wait 5us or more */
    for (w_count = 0U; w_count <= CGC_SUBWAITTIME; w_count++)
    {
        NOP();
    }
    
    OSMC = _00_CGC_SUBINHALT_ON | _00_CGC_RTC_CLK_FSUB;
    /* Set fCLK */
    CSS = 0U;
    /* Set fIH */
    HIOSTOP = 0U;
}

/*---------------------------------------------------------------------------*
 * Routine:  ConfigureOutputPorts
 *---------------------------------------------------------------------------*
 * Description:
 *      Configures the port and pin direction settings, and sets the
 *      pin outputs to a safe level.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConfigureOutputPorts(void)
{
    /* Set Initial Output Value */
    P14 = 0xDF; /* LCD_CS 0xFE */   /*G14 -> P145  */
    /* Set Output Port Modes */
    PM14 = 0xDF; /* LCD CS 0xFE */   /*G14 -> P145 */
    
#if 0
    P3 = 0xFE; /* SD CS */ 
    P7 = 0xFE; /* PMOD1_CS / PMOD2_CS / WIFI_CS */ 
    
    P5 = 0xC3; /* RLED1 / GLED1 / GLED2 / GLED3 */
    P6 = 0xF3; /* RLED2 / RLED3 */

    PM3 = 0xFE; /* SD CS */ 
    PM7 = 0xF1; /* PMOD1_CS / PMOD2_CS / WIFI_CS */ 
    
    PM5 = 0xC3; /* RLED1 / GLED1 / GLED2 / GLED3 */
    PM6 = 0xF3; /* RLED2 / RLED3 */
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  ConfigureInterrupts
 *---------------------------------------------------------------------------*
 * Description:
 *      Configures any system wide interrupts
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConfigureInterrupts(void)
{
    /* None yet */
}

/*---------------------------------------------------------------------------*
 * Routine:  EnablePeripheralModules
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables and configures peripheral devices on the MCU
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void EnablePeripheralModules(void)
{
    /* None yet */
}

/*-------------------------------------------------------------------------*
 * End of File:  hwsetup.c
 *-------------------------------------------------------------------------*/
