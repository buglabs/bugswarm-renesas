/**
 * @file
 *
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief CONFIG, Configurations for how things are built
 *
 * @section Description
 * These are the conifguration parameters for building the code
 */


#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "../gen/r_cg_macrodriver.h"
//#include "../gen/r_systeminit.h"
#include "../gen/r_cg_port.h"
//#include "../applilet3_src/CG_int.h"
#include "../gen/r_cg_serial.h"
//#include "../gen/r_cg_ad.h"
//#include "../gen/r_cg_timer.h"
//#include "../applilet3_src/CG_rtc.h"
/* Start user code for include. Do not edit comment generated here */
//#include "../applilet3_src/Preamble.h"
//#include "../applilet3_src/PinoutDef.h"
/* End user code. Do not edit comment generated here */
#include "../gen/r_cg_userdefine.h"
/* Module Selection for when we have to deal with a specific module feature */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WIFI_PWRON	P4_bit.no3   //P4.3
#define WIFI_RESET	P13_bit.no0   //P1.6

#endif


