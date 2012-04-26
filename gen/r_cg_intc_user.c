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
* Copyright (C) 2010, 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_intc_user.c
* Version      : CodeGenerator for RL78/G13 V1.03.01 [11 Oct 2011]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for INTC module.
* Creation Date: 4/26/2012
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt INTP0 r_intc0_interrupt
#pragma interrupt INTP1 r_intc1_interrupt
#pragma interrupt INTP2 r_intc2_interrupt
/* Start user code for pragma. Do not edit comment generated here */
//Number of milliseconds to debounce the switches - ignoring interrupts in between.
#define DEBOUNCE_TIME		50
//number of samples to take of switch when determining it's state
#define DEBOUNCE_SAMPLES	50
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intc.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_cg_it.h"
#include <stdio.h>
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
unsigned long intc0_last = 0;
unsigned long intc1_last = 0;
unsigned long intc2_last = 0;
uint8_t intc0_stat = 1;
uint8_t intc1_stat = 1;
uint8_t intc2_stat = 1;
uint8_t intc0_old = 1;
uint8_t intc1_old = 1;
uint8_t intc2_old = 1;
int sum = 0;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_intc0_interrupt
* Description  : This function is INTP0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_intc0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    if (millis > intc0_last + DEBOUNCE_TIME){
	intc0_stat = !intc0_old | ((P13 & (1 << 7))?1:0);
	intc0_last = millis;
	printf("BANG INTC0 %u\r\n", intc0_stat);
	intc0_old = intc0_stat;
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc1_interrupt
* Description  : This function is INTP1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_intc1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    if (millis > intc1_last + DEBOUNCE_TIME){
	intc1_stat = !intc1_old | ((P5 & (1 << 0))?1:0);
	intc1_last = millis;
	printf("BANG INTC1 %u\r\n", intc1_stat);
	intc1_old = intc1_stat;
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_intc2_interrupt
* Description  : This function is INTP2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_intc2_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
    if (millis > intc2_last + DEBOUNCE_TIME){
	intc2_stat = !intc2_old | ((P5 & (1 << 1))?1:0);
	intc2_last = millis;
	printf("BANG INTC2 %u\r\n", intc2_stat);
	intc2_old = intc2_stat;
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
