/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_intc_user.c
* Version      : Applilet3 for RL78/G14 V1.01.01 [11 Oct 2011]
* Device(s)    : R5F104PJ
* Tool-Chain   : IAR Systems iccrl78
* Description  : This file implements device driver for INTC module.
* Creation Date: 7/3/2012
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
volatile uint8_t G_ISRButtonFlag = 0;   // ISR Button Flag

volatile uint32_t G_IR_CLKCount = 0;   
volatile uint8_t G_IR_ifReadBit = 0;

extern volatile uint8_t G_IR_rxBitBuffer;
extern volatile uint32_t G_IR_BitTimeoutCount;   // IR timer count
extern volatile uint8_t G_IR_BitCount;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_intc1_interrupt
* Description  : This function is INTP1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP1_vect
__interrupt static void r_intc1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc2_interrupt
* Description  : This function is INTP2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP2_vect
__interrupt static void r_intc2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc3_interrupt
* Description  : This function is INTP3 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP3_vect
__interrupt static void r_intc3_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
#if 0
/***********************************************************************************************************************
* Function Name: r_intc4_interrupt
* Description  : This function is INTP4 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP4_vect
__interrupt static void r_intc4_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
  
    G_IR_CLKCount++;
    G_IR_BitTimeoutCount = 0;
    if(G_IR_CLKCount == 15)
    {
        G_IR_CLKCount = 0;
        
        if(G_IR_ifReadBit == 0)
            G_IR_ifReadBit = 1;    // Burst
        else
        {
            G_IR_ifReadBit = 0; 
            G_IR_rxBitBuffer <<= 1;  // Data Bit 1
            G_IR_rxBitBuffer |= 1;
            G_IR_BitCount++;
        }
    }
  
    /* End user code. Do not edit comment generated here */
}
#endif
/***********************************************************************************************************************
* Function Name: r_intc7_interrupt
* Description  : This function is INTP7 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP7_vect
__interrupt static void r_intc7_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc8_interrupt
* Description  : This function is INTP8 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP8_vect
__interrupt static void r_intc8_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    G_ISRButtonFlag = 2; // SW1 pressed
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc9_interrupt
* Description  : This function is INTP9 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP9_vect
__interrupt static void r_intc9_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    G_ISRButtonFlag = 3; // SW2 pressed
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc10_interrupt
* Description  : This function is INTP10 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTP10_vect
__interrupt static void r_intc10_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    G_ISRButtonFlag = 1; // SW3 pressed
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
