/*
*******************************************************************************
* Copyright(C) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation. 
*
* This device driver was created by Applilet3 for RL78/G13
* 16-Bit Single-Chip Microcontrollers
* Filename:	CG_timer_user.c
* Abstract:	This file implements device driver for TAU module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	8/26/2011
*******************************************************************************
*/

/*
*******************************************************************************
** Include files
*******************************************************************************
*/
#include "CG_macrodriver.h"
#include "CG_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include <stdint.h>
#include <math.h>
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
/* Start user code for global. Do not edit comment generated here */

#define  PI	(float)3.141592654
#define	CARRIER_FREQ		45056uL
#define SAMPLING_CONSTANT	32000000/CARRIER_FREQ


float G_Angle, G_DeltaTheta;
uint8_t G_SpeakerInit = 0;
const float SAMPLE_CONSTANT_HALF = (float)SAMPLING_CONSTANT/2.0f;

/* End user code. Do not edit comment generated here */

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTTM00 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTTM00_vect
__interrupt void MD_INTTM00(void)
{
	/* Start user code. Do not edit comment generated here */
  
    static unsigned short data;
	static float tmp1;
	
	G_Angle += G_DeltaTheta;
	if (G_Angle >= (2.0*PI))
		G_Angle = G_Angle-(2.0*PI);	// wrap angle

	data = (unsigned short)SAMPLE_CONSTANT_HALF;		// 0 output = 50%

	tmp1 = ( sinf(G_Angle) * SAMPLE_CONSTANT_HALF ) - 1; // Now scale the frequency value
	
	data += (unsigned short)tmp1;

	if ( (data == 0) || (data >= SAMPLING_CONSTANT) )
		data = 1;
    
    TDR01 = (USHORT)data;
    TDR02 = (USHORT)data;
  
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTTM04 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTTM04_vect
__interrupt void MD_INTTM04(void)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

void SpeakerSetFreq(uint32_t aFreq)
{
	G_Angle = 0.0f;
	
	// angle traversed each sample
	G_DeltaTheta = 2.0*PI*aFreq;
    G_DeltaTheta /= (float)CARRIER_FREQ;
}

void SpeakerON(uint8_t left, uint8_t right)
{
    TDR00 = SAMPLING_CONSTANT;
	TDR01 = 1;
    TDR02 = 1;
    
    TAU0_Channel0_Start();
    
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
    TAU0_Channel0_Stop();
}

/* End user code. Do not edit comment generated here */
