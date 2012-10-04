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
* File Name    : r_cg_port_user.c
* Version      : Applilet3 for RL78/G14 V1.01.01 [11 Oct 2011]
* Device(s)    : R5F104PJ
* Tool-Chain   : IAR Systems iccrl78
* Description  : This file implements device driver for PORT module.
* Creation Date: 7/3/2012
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

const T_GPIOPort PORT0 = {
    (unsigned char *)&P0,
    (unsigned char *)&PM0,
};

const T_GPIOPort PORT1 = {
    (unsigned char *)&P1,
    (unsigned char *)&PM1,
};

const T_GPIOPort PORT2 = {
    (unsigned char *)&P2,
    (unsigned char *)&PM2,
};

const T_GPIOPort PORT3 = {
    (unsigned char *)&P3,
    (unsigned char *)&PM3,
};

const T_GPIOPort PORT4 = {
    (unsigned char *)&P4,
    (unsigned char *)&PM4,
};

const T_GPIOPort PORT5 = {
    (unsigned char *)&P5,
    (unsigned char *)&PM5,
};

const T_GPIOPort PORT6 = {
    (unsigned char *)&P6,
    (unsigned char *)&PM6,
};

const T_GPIOPort PORT7 = {
    (unsigned char *)&P7,
    (unsigned char *)&PM7,
};

const T_GPIOPort PORT10 = {
    (unsigned char *)&P10,
    (unsigned char *)&PM10,
};

const T_GPIOPort PORT12 = {
    (unsigned char *)&P12,
    (unsigned char *)&PM12,
};

const T_GPIOPort PORT13 = {
    (unsigned char *)&P13,
    0
};

const T_GPIOPort PORT14 = {
    (unsigned char *)&P14,
    (unsigned char *)&PM14,
};

const T_GPIOPort PORT15 = {
    (unsigned char *)&P15,
    (unsigned char *)&PM15,
};

const T_GPIOPort *GPIOTable[] = {
	&PORT0,
	&PORT1,
	&PORT2,
	&PORT3,
	&PORT4,
	&PORT5,
	&PORT6,
	&PORT7,
	0,
    0,
    &PORT10,
    0,
    &PORT12,
    &PORT13,
    &PORT14,
    &PORT15
};

/* End user code. Do not edit comment generated here */

/* Start user code for adding. Do not edit comment generated here */

void GPIO_OuputMode(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPMn &= ~(1<<aPin);
}

void GPIO_SetHigh(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPn |= (1<<aPin);
}

void GPIO_SetLow(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPn &= ~(1<<aPin);
}

void GPIO_InputMode(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPMn |= (1<<aPin);
}

uint8_t GPIO_Read(unsigned char aPort, unsigned char aPin)
{
    return (*GPIOTable[aPort]->iPn & (1<<aPin)) >> aPin;
}

/* End user code. Do not edit comment generated here */
