/*
*********************************************************************************************************
*                                       uC/Probe Communication
*
*                         (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
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
*                                        COMMUNICATION: Wi-Fi
*                                        Micrium uC/OS-II PORT
*
* Filename      : probe_wifi_os.c
* Version       : V2.30
* Programmer(s) : FBJ
*                 BAN
*********************************************************************************************************
* Note(s)       : (1) This file is the uC/OS-II layer for the uC/Probe Wi-Fi Communication Module.
*
*                 (2) Assumes uC/OS-II V2.87+ is included in the project build.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <probe_wifi.h>
#include  <ucos_ii.h>


/*
*********************************************************************************************************
*                                               ENABLE
*
* Note(s) : (1) See 'probe_wifi.h  ENABLE'.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_WiFi_EN == DEF_ENABLED)                     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  ProbeWiFi_OS_TaskStk[PROBE_WIFI_CFG_TASK_STK_SIZE];   /* Stack for WiFi server task.                 */


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  ProbeWiFi_OS_Task(void *p_arg);                          /* WiFi server task.                           */


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        ProbeWiFi_OS_Init()
*
* Description : Initialize the UDP server task for Probe communication.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeWiFi_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeWiFi_OS_Init (void)
{
    INT8U  err;


#if (OS_TASK_CREATE_EXT_EN > 0)
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreateExt( ProbeWiFi_OS_Task,
                           (void *)0,
                          &ProbeWiFi_OS_TaskStk[PROBE_WIFI_CFG_TASK_STK_SIZE - 1],/* Set Top-Of-Stack.                    */
                           PROBE_WIFI_CFG_TASK_PRIO,
                           PROBE_WIFI_CFG_TASK_PRIO,
                          &ProbeWiFi_OS_TaskStk[0],                                /* Set Bottom-Of-Stack.                 */
                           PROBE_WIFI_CFG_TASK_STK_SIZE,
                           (void *)0,                                               /* No TCB extension.                    */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);              /* Enable stack checking + clear stack. */
    #else
    err = OSTaskCreateExt( ProbeWiFi_OS_Task,
                           (void *)0,
                          &ProbeWiFi_OS_TaskStk[0],                                /* Set Top-Of-Stack.                    */
                           PROBE_WIFI_CFG_TASK_PRIO,
                           PROBE_WIFI_CFG_TASK_PRIO,
                          &ProbeWiFi_OS_TaskStk[PROBE_WIFI_CFG_TASK_STK_SIZE - 1],/* Set Bottom-Of-Stack.                 */
                           PROBE_WIFI_CFG_TASK_STK_SIZE,
                           (void *)0,                                               /* No TCB extension.                    */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);              /* Enable stack checking + clear stack. */
    #endif
#else
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreate( ProbeWiFi_OS_Task,
                        (void *)0,
                       &ProbeWiFi_OS_TaskStk[PROBE_WIFI_CFG_TASK_STK_SIZE - 1],
                        PROBE_WIFI_CFG_TASK_PRIO);
    #else
    err = OSTaskCreate( ProbeWiFi_OS_Task,
                        (void *)0,
                       &ProbeWiFi_OS_TaskStk[0],
                        PROBE_WIFI_CFG_TASK_PRIO);
    #endif
#endif

#if (OS_VERSION < 287)
#if (OS_TASK_NAME_SIZE > 1)
    OSTaskNameSet(PROBE_WIFI_CFG_TASK_PRIO, (INT8U *)"Probe Wi-Fi", &err);
#endif
#else
#if (OS_TASK_NAME_EN   > 0)
    OSTaskNameSet(PROBE_WIFI_CFG_TASK_PRIO, (INT8U *)"Probe Wi-Fi", &err);
#endif
#endif
}


/*
*********************************************************************************************************
*                                         ProbeWiFi_OS_Dly()
*
* Description : Delay the UDP server task.
*
* Argument(s) : ms          Number of milliseconds for which the UDP task should be delayed.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeWiFi_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeWiFi_OS_Dly (CPU_INT16U  ms)
{
    INT32U  dly_ticks;


    dly_ticks = OS_TICKS_PER_SEC * ((INT32U)ms + 500L / OS_TICKS_PER_SEC) / 1000L;
    OSTimeDly(dly_ticks);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        ProbeWiFi_OS_Task()
*
* Description : Receives and transmits packets.
*
* Argument(s) : p_arg       Argument passed to ProbeWiFi_OS_Task() by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  ProbeWiFi_OS_Task (void *p_arg)
{
   (void)&p_arg;
    ProbeWiFi_Task();
}


/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/

#endif
