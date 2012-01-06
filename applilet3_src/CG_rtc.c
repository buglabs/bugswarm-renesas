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
* Filename:	CG_rtc.c
* Abstract:	This file implements device driver for RTC module.
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
#include "CG_rtc.h"
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
**	This function initializes the RTC module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void RTC_Init(void)
{
	RTCEN = 1U;		/* supply RTC clock */
	RTCE = 0U;		/* disable RTC clock operation */
	RTCMK = 1U;		/* disable INTRTC interrupt */
	RTCIF = 0U;		/* clear INTRTC interrupt flag */
	RTCC0 = _00_RTC_RTC1HZ_DISABLE | _00_RTC_12HOUR_SYSTEM | _00_RTC_INTRTC_NOT_GENERATE;
	/* Set real-time clock */
	SEC = _00_RTC_COUNTER_SEC;
	MIN = _00_RTC_COUNTER_MIN;
	HOUR = _12_RTC_COUNTER_HOUR;
	WEEK = _06_RTC_COUNTER_WEEK;
	DAY = _01_RTC_COUNTER_DAY;
	MONTH = _01_RTC_COUNTER_MONTH;
	YEAR = _00_RTC_COUNTER_YEAR;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function enables the real-time clock.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void RTC_CounterEnable(void)
{
	RTCE = 1U;	/* enable RTC clock operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function reads the results of real-time clock and store them in the variables.
**
**  Parameters:
**	counterreadval: the current real-time clock value(BCD code)
**
**  Returns:
**	MD_OK
**	MD_BUSY1
**	MD_BUSY2
**
**-----------------------------------------------------------------------------
*/
MD_STATUS RTC_CounterGet(struct RTCCounterValue *counterreadval)
{
	MD_STATUS status = MD_OK;
	UINT i;
	
	RTCC1 |= _01_RTC_COUNTER_PAUSE;
	/* Wait 10 us */
	for( i=0U; i<RTC_WAITTIME; i++ )
	{
		NOP();
	}
	if (RWST == 0U)
	{
		status = MD_BUSY1;
	}
	else
	{
		counterreadval->Sec = SEC;
		counterreadval->Min = MIN;
		counterreadval->Hour = HOUR;
		counterreadval->Week = WEEK;
		counterreadval->Day = DAY;
		counterreadval->Month = MONTH;
		counterreadval->Year = YEAR;
		RTCC1 &= (UCHAR)~_01_RTC_COUNTER_PAUSE;
		/* Wait 10 us */
		for( i=0U; i<RTC_WAITTIME; i++ )
		{
			NOP();
		}
		if (RWST == 1U)
		{
			status = MD_BUSY2;
		}
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function changes the real-time clock value.
**
**  Parameters:
**	counterwriteval: the expected real-time clock value(BCD code)
**
**  Returns:
**	MD_OK
**	MD_BUSY1
**	MD_BUSY2
**
**-----------------------------------------------------------------------------
*/
MD_STATUS RTC_CounterSet(struct RTCCounterValue counterwriteval)
{
	MD_STATUS status = MD_OK;
	UINT i;
	
	RTCC1 |= _01_RTC_COUNTER_PAUSE;
	/* Wait 10 us */
	for( i=0U; i<RTC_WAITTIME; i++ )
	{
		NOP();
	}
	if (RWST == 0U)
	{
		status = MD_BUSY1;
	}
	else
	{
		SEC = counterwriteval.Sec;
		MIN = counterwriteval.Min;
		HOUR = counterwriteval.Hour;
		WEEK = counterwriteval.Week;
		DAY = counterwriteval.Day;
		MONTH = counterwriteval.Month;
		YEAR = counterwriteval.Year;
		RTCC1 &= (UCHAR)~_01_RTC_COUNTER_PAUSE;
		/* Wait 10 us */
		for( i=0U; i<RTC_WAITTIME; i++ )
		{
			NOP();
		}
		if (RWST == 1U)
		{
			status = MD_BUSY2;
		}
	}

	return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
