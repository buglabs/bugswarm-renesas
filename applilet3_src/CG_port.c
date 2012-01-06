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
* Filename:	CG_port.c
* Abstract:	This file implements device driver for PORT module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	5/26/2011
*******************************************************************************
*/

/*
*******************************************************************************
** Include files
*******************************************************************************
*/
#include "CG_macrodriver.h"
#include "CG_port.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the Port I/O.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_Init(void)
{
  
        
        P0 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn6_OUTPUT_0;
	P1 = _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _20_Pn5_OUTPUT_1 | _01_Pn0_OUTPUT_1;
	P2 = _00_Pn5_OUTPUT_0;
        P3 = _01_Pn0_OUTPUT_1;
	P4 = _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0;
	P5 = _00_Pn0_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0 | _00_Pn4_OUTPUT_0 | _00_Pn5_OUTPUT_0;
	P6 = _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0;
       	P7 = _02_Pn1_OUTPUT_1 | _04_Pn2_OUTPUT_1;
	P13 = _00_Pn0_OUTPUT_0;
	P14 = _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0;
	PMC14 = _00_PMCn7_DI_ON | _7F_PMC14_DEFAULT;
	PM0 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | _20_PMn5_NOT_USE | _00_PMn6_MODE_OUTPUT | _80_PM0_DEFAULT;
	
        PM1 = _00_PMn0_MODE_OUTPUT | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT;
	PM2 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | _00_PMn5_MODE_OUTPUT | _40_PMn6_NOT_USE | _80_PMn7_NOT_USE;
        PM3 = _00_PMn0_MODE_OUTPUT | _02_PMn1_NOT_USE | _FC_PM3_DEFAULT;
	
        PM4 = _01_PMn0_NOT_USE | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _F0_PM4_DEFAULT;
	
        PM5 = _00_PMn0_MODE_OUTPUT | _02_PMn1_NOT_USE | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT | _C0_PM5_DEFAULT;
	PM6 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _00_PMn2_MODE_OUTPUT | _08_PMn3_MODE_INPUT | _F0_PM6_DEFAULT;
	PM14 = _01_PMn0_MODE_INPUT | _02_PMn1_MODE_INPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT | _3C_PM14_DEFAULT;
     	PM7 = _01_PMn0_NOT_USE | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | _20_PMn5_NOT_USE | _40_PMn6_NOT_USE | _80_PMn7_NOT_USE;
       
        
}
#if 0
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes P130 to output mode by software.
**
**  Parameters:
**	initialvalue: initial output 1 or 0
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_ChangeP130Output(BOOL initialvalue)
{
	if (initialvalue)
	{
		P13 |= _01_Pn0_OUTPUT_1;
	}
	else
	{
		P13 &= (UCHAR)~_01_Pn0_OUTPUT_1;
	}
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes P146 to input mode by software.
**
**  Parameters:
**	enablepu: Pull-up enable or not
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_ChangeP146Input(BOOL enablepu)
{
	if (enablepu)
	{
		PU14 |= _40_PUn6_PULLUP_ON;
	}
	else
	{
		PU14 &= (UCHAR)~_40_PUn6_PULLUP_ON;
	}
	PM14 |= _40_PMn6_MODE_INPUT;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes P146 to output mode by software.
**
**  Parameters:
**	initialvalue: initial output 1 or 0
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_ChangeP146Output(BOOL initialvalue)
{
	if (initialvalue)
	{
		P14 |= _40_Pn6_OUTPUT_1;
	}
	else
	{
		P14 &= (UCHAR)~_40_Pn6_OUTPUT_1;
	}
	PM14 &= (UCHAR)~_40_PMn6_MODE_INPUT;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes P147 to input mode by software.
**
**  Parameters:
**	enablepu: Pull-up enable or not
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_ChangeP147Input(BOOL enablepu)
{
	if (enablepu)
	{
		PU14 |= _80_PUn7_PULLUP_ON;
	}
	else
	{
		PU14 &= (UCHAR)~_80_PUn7_PULLUP_ON;
	}
	PMC14 &= (UCHAR)~_80_PMCn7_NOT_USE;
	PM14 |= _80_PMn7_MODE_INPUT;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes P147 to output mode by software.
**
**  Parameters:
**	initialvalue: initial output 1 or 0
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_ChangeP147Output(BOOL initialvalue)
{
	if (initialvalue)
	{
		P14 |= _80_Pn7_OUTPUT_1;
	}
	else
	{
		P14 &= (UCHAR)~_80_Pn7_OUTPUT_1;
	}
	PMC14 &= (UCHAR)~_80_PMCn7_NOT_USE;
	PM14 &= (UCHAR)~_80_PMn7_MODE_INPUT;
}
#endif
/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
