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
* File Name    : r_cg_timer_user.c
* Version      : Applilet3 for RL78/G14 V1.01.01 [11 Oct 2011]
* Device(s)    : R5F104PJ
* Tool-Chain   : IAR Systems iccrl78
* Description  : This file implements device driver for TAU module.
* Creation Date: 7/3/2012
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

#define  PI	(float)3.141592654
#define	CARRIER_FREQ		45056uL
#define SAMPLING_CONSTANT	32000000/CARRIER_FREQ

uint8_t G_IRToggle = 0;
float G_Angle, G_DeltaTheta;
uint8_t G_SpeakerInit = 0;
const float SAMPLE_CONSTANT_HALF = (float)SAMPLING_CONSTANT/2.0f;

volatile uint8_t G_IR_rxBitBuffer = 0;
volatile uint8_t G_IR_ReceivingData = 0;
volatile uint8_t G_IR_rxDataByte = 0;
volatile uint32_t G_IR_BitTimeoutCount = 0;
volatile uint8_t G_IR_BitCount = 0;
volatile uint32_t G_IR_ByteTimeoutCount = 0;

extern volatile uint8_t G_IR_ifReadBit;
extern volatile uint32_t G_IR_CLKCount;   // IR Input counter

volatile int G_AudioPWMOutCount = 0;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function is INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTTM00_vect
__interrupt static void r_tau0_channel0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    P0 &= ~(1<<1);
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function is INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTTM01_vect
__interrupt static void r_tau0_channel1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    G_AudioPWMOutCount++;
    P0 |= 1<<1;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt
* Description  : This function is INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTTM03_vect
__interrupt static void r_tau0_channel3_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    
    if(G_IR_BitCount == 8)
    {
        G_IR_BitCount = 0;
        G_IR_ReceivingData = 0;
        G_IR_rxDataByte = G_IR_rxBitBuffer;
        G_IR_ifReadBit = 0;
    }
  
    G_IR_BitTimeoutCount++;
    
    if(G_IR_BitTimeoutCount > 25)
    {
        if(G_IR_ifReadBit == 1)
        {
            G_IR_ifReadBit = 0;
            G_IR_rxBitBuffer <<= 1;  // Data Bit 1
            G_IR_BitCount++;
        }
        
        G_IR_CLKCount = 0;
        G_IR_BitTimeoutCount = 0;
    }
    
    G_IR_ByteTimeoutCount++;
    if(G_IR_ByteTimeoutCount > 500000)
    {
        // Reset Data
        G_IR_BitCount = 0;
        G_IR_rxBitBuffer = 0;
        G_IR_ifReadBit = 0;
    }
    
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_tmr_rd1_interrupt
* Description  : This function is INTTRD1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
#pragma vector = INTTRD1_vect
__interrupt static void r_tmr_rd1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */


void SpeakerSetFreq(uint32_t aFreq)
{
    uint32_t period;
  
    period = 32000000/aFreq;
    
    TDR01 = period/2;
    TDR02 = period/2;
    TDR00 = period;
}

void SpeakerON(uint8_t left, uint8_t right)
{
    R_TAU0_Channel0_Start();
    
    PMC1 &= ~(3<<6);
    G_AudioPWMOutCount = 0;
    
    if(!right)
    {
        // Turn off right
        TT0 |= _0002_TAU_CH1_STOP_TRG_ON;
        TOE0 &= ~_0002_TAU_CH1_OUTPUT_ENABLE;
    }
    
    if(!left) 
    {
        // Turn off left
        TT0 |= _0004_TAU_CH2_STOP_TRG_ON;
	    TOE0 &= ~_0004_TAU_CH2_OUTPUT_ENABLE;
    }
}

void SpeakerOFF()
{
    R_TAU0_Channel0_Stop();
}

void R_TMR_RD1_ChangeDuty(uint8_t ratio)
{
	uint32_t reg = 0;
	
	if (ratio > 100U)
	{
		ratio = 100U;
	}
	reg = TRDGRA1;
	reg = (reg + 1) * ratio / 100U;
    TRDGRD1 = reg;
}

/* End user code. Do not edit comment generated here */
