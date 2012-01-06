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
* Filename:	CG_rtc.h
* Abstract:	This file implements device driver for RTC module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	5/26/2011
*******************************************************************************
*/

#ifndef _MDRTC_
#define _MDRTC_
/*
*******************************************************************************
**  Register bit define
*******************************************************************************
*/
/*
	Peripheral enable register 0 (PER0)
*/
/* Control of real-time clock (RTC) input clock (RTCEN) */
#define _00_RTC_CLOCK_STOP			0x00U	/* stops supply of input clock */
#define _01_RTC_CLOCK_SUPPLY			0x01U	/* supplies input clock */

/*
	Real-time clock register 0 (RTCC0)
*/
/* Real-time clock operation control (RTCE) */
#define _00_RTC_COUNTER_STOP			0x00U	/* stops clock operation */
#define _80_RTC_COUNTER_START			0x80U	/* starts clock operation */
/* Selection of 12-/24-hour system (AMPM) */
#define _08_RTC_HOURSYSTEM_CLEAR		0x08U	/* clear the AMPM bit */
#define _08_RTC_RTCC0_AMPM			0x08U	/* AMPM bit status detect */
#define _00_RTC_12HOUR_SYSTEM			0x00U	/* 12-hour system (a.m. and p.m.. are displayed.) */
#define _08_RTC_24HOUR_SYSTEM			0x08U	/* 24-hour system */
/* Interrupt (INTRTC) selection (CT2 - CT0) */
#define _88_RTC_INTRTC_CLEAR			0x88U	/* clear INTRTC */
#define _00_RTC_INTRTC_NOT_GENERATE		0x00U	/* does not use constant-period interrupt function */
#define _01_RTC_INTRTC_CLOCK_0			0x01U	/* once per 0.5s (synchronized with second count up) */
#define _02_RTC_INTRTC_CLOCK_1			0x02U	/* once per 1 s (same time as second count up) */
#define _03_RTC_INTRTC_CLOCK_2			0x03U	/* once per 1 m (second 00 of every minute) */
#define _04_RTC_INTRTC_CLOCK_3			0x04U	/* once per 1 hour (minute 00 and second 00 of every hour) */
#define _05_RTC_INTRTC_CLOCK_4			0x05U	/* once per 1 day (hour 00,minute 00, and second 00 of every day) */
#define _06_RTC_INTRTC_CLOCK_5			0x06U	/* once per 1 month (Day 1, hour 00a.m., minute 00, and second 00 of every month) */
/* RTC1HZ pin output control (CLOE1) */
#define _00_RTC_RTC1HZ_DISABLE			0x00U	/* disables output of RTC1HZ pin (1 kHz) */
#define _20_RTC_RTC1HZ_ENABLE			0x20U	/* enable output of RTC1HZ pin (1 kHz) */

/*
	Real-time clock control register 1 (RTCC1)
*/
/* Alarm operation control (WALE) */
#define _00_RTC_ALARM_DISABLE			0x00U	/* alarm match operation is invalid */
#define _80_RTC_ALARM_ENABLE			0x80U	/* alarm match operation is valid */
/* Control of alarm interrupt function operation (WALIE) */
#define _00_RTC_ALARM_INT_DISABLE		0x00U	/* does not generate interrupt on matching of alarm */
#define _40_RTC_ALARM_INT_ENABLE		0x40U	/* generates interrupt on matching of alarm */
/* Alarm detection status flag (WAFG) */
#define _00_RTC_ALARM_MISMATCH			0x00U	/* alarm mismatch */
#define _10_RTC_ALARM_MATCH			0x10U	/* detection of matching of alarm */
/* Constant-period interrupt status flag (RIFG) */
#define _00_RTC_INTC_NOTGENERATE_FALG		0x00U	/* constant-period interrupt is not generated */
#define _08_RTC_INTC_GENERATE_FLAG		0x08U	/* generated constant-period interrupt */
/* Wait status flag of real-time clock (RWST) */
#define _00_RTC_COUNTER_OPERATING		0x00U	/* clock is operating */
#define _01_RTC_COUNTER_STOP			0x02U	/* Mode to read or write clock value */
/* Wait control of real-time clock (RWAIT) */
#define _00_RTC_COUNTER_SET			0x00U	/* sets clock operation */
#define _01_RTC_COUNTER_PAUSE			0x01U	/* stops SEC to YEAR counters. Mode to read or write counter value */

/*
	Watch error correction register (SUBCUD)
*/
/* Setting of watch error correction timing (DEV) */
#define _00_RTC_CORRECTION_EVERY20S		0x00U	/* corrects watch error when the second digits are at 00, 20, or 40 */
#define _80_RTC_CORRECTION_EVERY60S		0x80U	/* corrects watch error when the second digits are at 00 */
/* Setting of watch error correction method (F6) */
#define _00_RTC_CORRECTION_INCREASE		0x00U	/* increase */
#define _40_RTC_CORRECTION_DECREASE		0x40U	/* decrease */
/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
#define _00_RTC_COUNTER_YEAR			0x00U
#define _01_RTC_COUNTER_MONTH			0x01U
#define _01_RTC_COUNTER_DAY			0x01U
#define _06_RTC_COUNTER_WEEK			0x06U
#define _12_RTC_COUNTER_HOUR			0x12U
#define _00_RTC_COUNTER_MIN			0x00U
#define _00_RTC_COUNTER_SEC			0x00U
#define	RTC_WAITTIME				320U
struct RTCCounterValue 
{
	UCHAR Sec;
	UCHAR Min;
	UCHAR Hour;
	UCHAR Day;
	UCHAR Week;
	UCHAR Month;
	UCHAR Year;
};
struct RTCAlarmValue
{
	UCHAR Alarmwm;
	UCHAR Alarmwh;
	UCHAR Alarmww;
};
enum RTCHourSystem
{ 
	HOUR12, HOUR24
};
enum RTCCorectionTiming
{
	EVERY20S, EVERY60S
};
enum RTCINTPeriod
{
	HALFSEC = 1U, ONESEC, ONEMIN, ONEHOUR, ONEDAY, ONEMONTH
};
enum RTCINTInterval
{
	INTERVAL0, INTERVAL1, INTERVAL2, INTERVAL3, INTERVAL4, INTERVAL5, INTERVAL6
};
/*
*******************************************************************************
**  Function define
*******************************************************************************
*/
void RTC_Init(void);
void RTC_CounterEnable(void);
MD_STATUS RTC_CounterGet(struct RTCCounterValue *counterreadval);
MD_STATUS RTC_CounterSet(struct RTCCounterValue counterwriteval);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
