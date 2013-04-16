/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                           (c) Copyright 2010-2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                       APPLICATION CONFIGURATION
*
*                                            Renesas RL78
*                                               on the
*                                    RL78G14-RDK Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00.00
* Programmer(s) : MD
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  APP_CFG_PROBE_COM_EN                       DEF_DISABLED
#define  APP_CFG_SERIAL_EN                          DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO                            2u

#define  APP_CFG_TASK_LEDODD_PRIO                           6u
#define  APP_CFG_TASK_LEDEVEN_PRIO                          7u

#define  OS_TASK_TMR_PRIO                         (OS_LOWEST_PRIO - 2u)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE                    256u

#define  APP_CFG_TASK_LEDODD_STK_SIZE                   128u
#define  APP_CFG_TASK_LEDEVEN_STK_SIZE                  128u


/*
*********************************************************************************************************
*                                            TASK STACK SIZES LIMIT
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE_PCT_FULL             90u
#define  APP_CFG_TASK_START_STK_SIZE_LIMIT              (APP_CFG_TASK_START_STK_SIZE * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))  / 100u

#define  APP_CFG_TASK_LEDODD_STK_SIZE_PCT_FULL            90u
#define  APP_CFG_TASK_LEDODD_STK_SIZE_LIMIT             (APP_CFG_TASK_LEDODD_STK_SIZE * (100u - APP_CFG_TASK_LEDODD_STK_SIZE_PCT_FULL))  / 100u

#define  APP_CFG_TASK_LEDEVEN_STK_SIZE_PCT_FULL           90u
#define  APP_CFG_TASK_LEDEVEN_STK_SIZE_LIMIT            (APP_CFG_TASK_LEDEVEN_STK_SIZE * (100u - APP_CFG_TASK_LEDEVEN_STK_SIZE_PCT_FULL))  / 100u


/*
*********************************************************************************************************
*                                             uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_ARG_CHK_EXT_EN                 DEF_DISABLED
#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN                DEF_DISABLED
#define  LIB_MEM_CFG_ALLOC_EN                       DEF_ENABLED
#define  LIB_MEM_CFG_HEAP_SIZE                      5u * 1024u
#define  LIB_STR_CFG_FP_EN                          DEF_DISABLED


/*
*********************************************************************************************************
*                                               SERIAL
*
* Note(s) : (1) Configure SERIAL_CFG_MAX_NBR_IF to the number of interfaces (i.e., UARTs) that will be
*               present.
*
*           (2) Configure SERIAL_CFG_RD_BUF_EN to enable/disable read buffer functionality.  The serial
*               core stores received data in the read buffer until the user requests it, providing a
*               reliable guarantee against receive overrun.
*
*           (3) Configure SERIAL_CFG_WR_BUF_EN to enable/disable write buffer functionality.  The serial
*               core stores line driver transmit data in the write buffer while the serial interface is
*               transmitting application data.
*
*           (4) Configure SERIAL_CFG_ARG_CHK_EXT_EN to enable/disable extended argument checking
*               functionality.
*
*           (5) Configure SERIAL_CFG_TX_DESC_NBR to allow multiple transmit operations (i.e., Serial_Wr,
*               Serial_WrAsync) to be queued.
*********************************************************************************************************
*/

#define  SERIAL_CFG_MAX_NBR_IF                             2u   /* See Note #1.                                         */

#define  SERIAL_CFG_RD_BUF_EN                       DEF_ENABLED /* See Note #2.                                         */

#define  SERIAL_CFG_WR_BUF_EN                       DEF_ENABLED /* See Note #3.                                         */

#define  SERIAL_CFG_ARG_CHK_EXT_EN                  DEF_ENABLED /* See Note #4.                                         */

#define  SERIAL_CFG_TX_DESC_NBR                            1u   /* See Note #5.                                         */


/*
*********************************************************************************************************
*                                    uC/SERIAL APPLICATION CONFIGURATION
*********************************************************************************************************
*/

#if (APP_CFG_PROBE_COM_EN == DEF_ENABLED)
#define  APP_SERIAL_CFG_TRACE_EN                    DEF_DISABLED
#else
#define  APP_SERIAL_CFG_TRACE_EN                    DEF_ENABLED
#endif

#define  APP_SERIAL_CFG_TRACE_PORT_NAME             "UART1"


/*
*********************************************************************************************************
*                                       uC/PROBE APPLICATION CONFIGURATION
*********************************************************************************************************
*/

#define  APP_CFG_PROBE_SERIAL_PORT_NAME             "UART1"


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                                0
#define  TRACE_LEVEL_INFO                               1
#define  TRACE_LEVEL_DBG                                2
#define  TRACE_LEVEL_LOG                                3

#include <cpu.h>
void  App_SerPrintf (CPU_CHAR  *p_fmt, ...);

#define  APP_CFG_TRACE_LEVEL                    TRACE_LEVEL_DBG
#define  APP_CFG_TRACE                          App_SerPrintf

#define  BSP_CFG_TRACE_LEVEL                    TRACE_LEVEL_OFF
#define  BSP_CFG_TRACE                          printf

#define  APP_TRACE_INFO(x)               ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

#define  BSP_TRACE_INFO(x)               ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO) ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)                ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)  ? (void)(BSP_CFG_TRACE x) : (void)0)

#endif