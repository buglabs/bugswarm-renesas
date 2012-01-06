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
* Filename:	CG_int.h
* Abstract:	This file implements device driver for INT module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	8/26/2011
*******************************************************************************
*/

#ifndef _MDINT_
#define _MDINT_
/*
*******************************************************************************
**  Register bit define
*******************************************************************************
*/

/*
	External Interrupt Rising Edge Enable Register 0 (EGP0)
*/
/* INTPn pin valid edge selection (EGPn) */
#define _01_INTP0_EDGE_RISING_SEL		0x01U	/* rising edge selected for INTP0 pin */
#define _00_INTP0_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_RISING_SEL		0x02U	/* rising edge selected for INTP1 pin */
#define _00_INTP1_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP1 pin*/
#define _04_INTP2_EDGE_RISING_SEL		0x04U	/* rising edge selected for INTP2 pin */
#define _00_INTP2_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_RISING_SEL		0x08U	/* rising edge selected for INTP3 pin */
#define _00_INTP3_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_RISING_SEL		0x10U	/* rising edge selected for INTP4 pin */
#define _00_INTP4_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_RISING_SEL		0x20U	/* rising edge selected for INTP5 pin */
#define _00_INTP5_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_RISING_SEL		0x40U	/* rising edge selected for INTP6 pin */
#define _00_INTP6_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_RISING_SEL		0x80U	/* rising edge selected for INTP7 pin */
#define _00_INTP7_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP7 pin */

/*
	External Interrupt Falling Edge Enable Register 0 (EGN0)
*/
/* INTPn pin valid edge selection (EGNn) */
#define _01_INTP0_EDGE_FALLING_SEL		0x01U	/* falling edge selected for INTP0 pin */
#define _00_INTP0_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP0 pin */
#define _02_INTP1_EDGE_FALLING_SEL		0x02U	/* falling edge selected for INTP1 pin */
#define _00_INTP1_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP1 pin */
#define _04_INTP2_EDGE_FALLING_SEL		0x04U	/* falling edge selected for INTP2 pin */
#define _00_INTP2_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP2 pin */
#define _08_INTP3_EDGE_FALLING_SEL		0x08U	/* falling edge selected for INTP3 pin */
#define _00_INTP3_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP3 pin */
#define _10_INTP4_EDGE_FALLING_SEL		0x10U	/* falling edge selected for INTP4 pin */
#define _00_INTP4_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP4 pin */
#define _20_INTP5_EDGE_FALLING_SEL		0x20U	/* falling edge selected for INTP5 pin */
#define _00_INTP5_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP5 pin */
#define _40_INTP6_EDGE_FALLING_SEL		0x40U	/* falling edge selected for INTP6 pin */
#define _00_INTP6_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP6 pin */
#define _80_INTP7_EDGE_FALLING_SEL		0x80U	/* falling edge selected for INTP7 pin */
#define _00_INTP7_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP7 pin */

/*
	External Interrupt Rising Edge Enable Register 1 (EGP1)
*/
/* INTPn pin valid edge selection (EGPn) */
#define _01_INTP8_EDGE_RISING_SEL		0x01U	/* rising edge selected for INTP8 pin */
#define _00_INTP8_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP8 pin */
#define _02_INTP9_EDGE_RISING_SEL		0x02U	/* rising edge selected for INTP9 pin */
#define _00_INTP9_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP9 pin*/
#define _04_INTP10_EDGE_RISING_SEL		0x04U	/* rising edge selected for INTP10 pin */
#define _00_INTP10_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP10 pin */
#define _08_INTP11_EDGE_RISING_SEL		0x08U	/* rising edge selected for INTP11 pin */
#define _00_INTP11_EDGE_RISING_UNSEL		0x00U	/* rising edge not selected for INTP11 pin*/

/*
	External Interrupt Falling Edge Enable Register 1 (EGN1)
*/
/* INTPn pin valid edge selection (EGNn) */
#define _01_INTP8_EDGE_FALLING_SEL		0x01U	/* falling edge selected for INTP8 pin */
#define _00_INTP8_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP8 pin */
#define _02_INTP9_EDGE_FALLING_SEL		0x02U	/* falling edge selected for INTP9 pin */
#define _00_INTP9_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP9 pin */
#define _04_INTP10_EDGE_FALLING_SEL		0x04U	/* falling edge selected for INTP10 pin */
#define _00_INTP10_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP10 pin */
#define _08_INTP11_EDGE_FALLING_SEL		0x08U	/* falling edge selected for INTP11 pin */
#define _00_INTP11_EDGE_FALLING_UNSEL		0x00U	/* falling edge not selected for INTP11 pin */

/*
	Key Return Mode Register (KRM)
*/
/* Key interrupt mode control (KRMn) */
#define _01_KR0_SIGNAL_DETECT_ON		0x01U	/* detect KR0 signal */
#define _00_KR0_SIGNAL_DETECT_OFF		0x00U	/* not detect KR0 signal */
#define _02_KR1_SIGNAL_DETECT_ON		0x02U	/* detect KR1 signal */        
#define _00_KR1_SIGNAL_DETECT_OFF		0x00U	/* not detect KR1 signal */
#define _04_KR2_SIGNAL_DETECT_ON		0x04U	/* detect KR2 signal */        
#define _00_KR2_SIGNAL_DETECT_OFF		0x00U	/* not detect KR2 signal */
#define _08_KR3_SIGNAL_DETECT_ON		0x08U	/* detect KR3 signal */        
#define _00_KR3_SIGNAL_DETECT_OFF		0x00U	/* not detect KR3 signal */
#define _10_KR4_SIGNAL_DETECT_ON		0x10U	/* detect KR4 signal */        
#define _00_KR4_SIGNAL_DETECT_OFF		0x00U	/* not detect KR4 signal */
#define _20_KR5_SIGNAL_DETECT_ON		0x20U	/* detect KR5 signal */        
#define _00_KR5_SIGNAL_DETECT_OFF		0x00U	/* not detect KR5 signal */
#define _40_KR6_SIGNAL_DETECT_ON		0x40U	/* detect KR6 signal */        
#define _00_KR6_SIGNAL_DETECT_OFF		0x00U	/* not detect KR6 signal */
#define _80_KR7_SIGNAL_DETECT_ON		0x80U	/* detect KR7 signal */        
#define _00_KR7_SIGNAL_DETECT_OFF		0x00U	/* not detect KR7 signal */
/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
enum MaskableSource 
{
	INT_WDTI, INT_LVI, INT_INTP0, INT_INTP1, INT_INTP2, INT_INTP3, INT_INTP4, INT_INTP5,
	INT_ST2_CSI20_IIC20, INT_SR2_CSI21_IIC21, INT_SRE2, INT_DMA0, INT_DMA1, INT_ST0_CSI00_IIC00,
	INT_SR0_CSI01_IIC01, INT_SRE0_TM01H, INT_ST1_CSI10_IIC10, INT_SR1_CSI11_IIC11, INT_SRE1_TM03H,
	INT_IICA0, INT_TM00, INT_TM01, INT_TM02, INT_TM03, INT_AD, INT_RTC, INT_IT, INT_KR, INT_TM04 = 31U,
	INT_TM05, INT_TM06, INT_TM07, INT_INTP6, INT_INTP7, INT_INTP8, INT_INTP9, INT_INTP10,
	INT_INTP11, INT_MD = 45U, INT_FL = 47U
};
/*
*******************************************************************************
**  Function define
*******************************************************************************
*/
void INTP_Init(void);
void INTP0_Enable(void);
void INTP0_Disable(void);
void INTP1_Enable(void);
void INTP1_Disable(void);
void INTP2_Enable(void);
void INTP2_Disable(void);
__interrupt void MD_INTP0(void);
__interrupt void MD_INTP1(void);
__interrupt void MD_INTP2(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
