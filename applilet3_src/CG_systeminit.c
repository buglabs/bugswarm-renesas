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
* Filename:	CG_systeminit.c
* Abstract:	This file implements system initializing function.
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
#include "CG_system.h"
#include "CG_port.h"
#include "CG_int.h"
#include "CG_serial.h"
#include "CG_ad.h"
#include "CG_timer.h"
#include "CG_it.h"
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

int __low_level_init(void);
void systeminit(void);

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes every macro.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void systeminit(void)
{
	PIOR = 0x00U;
	CG_ReadResetSource();
	PORT_Init();
	CLOCK_Init();
	SAU0_Init();
	SAU1_Init();
	IICA0_Init();
    	//AD_Init();
	//TAU0_Init();
	//IT_Init();
	//INTP_Init();
	CRC0CTL = 0x00U;
	IAWCTL = 0x00U;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes hardware setting.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
int __low_level_init(void)
{
	DI();
	systeminit();
	EI();
	
	return MD_TRUE;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
