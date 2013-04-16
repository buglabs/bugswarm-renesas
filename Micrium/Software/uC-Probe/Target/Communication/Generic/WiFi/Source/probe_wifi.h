/*
*********************************************************************************************************
*                                       uC/Probe Communication
*
*                         (c) Copyright 2007-2013; Micrium, Inc.; Weston, FL
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
*
* Filename      : probe_wifi.h
* Version       : V2.30
* Programmer(s) : FBJ
*                 BAN
*                 JPB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               PROBE_WIFI present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  PROBE_WIFI_PRESENT                                    /* See Note #1.                                         */
#define  PROBE_WIFI_PRESENT


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef    PROBE_WIFI_MODULE
#define   PROBE_WIFI_EXT
#else
#define   PROBE_WIFI_EXT  extern
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <probe_com.h>


/*
*********************************************************************************************************
*                                               ENABLE
*
* Note(s) : (1) Wi-Fi communication is enabled/disabled via the PROBE_COM_CFG_WIFI_EN configuration define
*               (see 'probe_com_cfg.h  COMMUNICATION MODULE CONFIGURATION').
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_WIFI_EN == DEF_ENABLED)                     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/

#ifndef  PROBE_WIFI_CFG_SOCK_BLOCK_EN
#define  PROBE_WIFI_CFG_SOCK_BLOCK_EN           DEF_ENABLED
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

PROBE_WIFI_EXT  CPU_CHAR             ProbeWiFi_RxBuf[PROBE_WIFI_CFG_RX_BUF_SIZE + 10];    /* Buf to hold rx'd data.           */

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         ProbeWiFi_Init   (void);

void         ProbeWiFi_Task   (void);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                  DEFINED IN OS'S  probe_wifi_os.c
*********************************************************************************************************
*/

void  ProbeWiFi_OS_Init(void);

void  ProbeWiFi_OS_Dly (CPU_INT16U  ms);


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  PROBE_WIFI_CFG_TASK_PRIO
#error  "PROBE_WIFI_CFG_TASK_PRIO              not #define'd in 'probe_com_cfg.h'"
#endif



#ifndef  PROBE_WIFI_CFG_TASK_STK_SIZE
#error  "PROBE_WIFI_CFG_TASK_STK_SIZE          not #define'd in 'probe_com_cfg.h'"
#endif




#ifndef  PROBE_WIFI_CFG_RX_BUF_SIZE
#error  "PROBE_WIFI_CFG_RX_BUF_SIZE            not #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 32   ]                     "
#error  "                                 [     &&  <= 65535]                     "

#elif  ((PROBE_WIFI_CFG_RX_BUF_SIZE > 65535) || \
        (PROBE_WIFI_CFG_RX_BUF_SIZE < 32   ))
#error  "PROBE_WIFI_CFG_RX_BUF_SIZE      illegally #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 32   ]                     "
#error  "                                 [     &&  <= 65535]                     "
#endif



#ifndef  PROBE_WIFI_CFG_TX_BUF_SIZE
#error  "PROBE_WIFI_CFG_TX_BUF_SIZE            not #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 32   ]                     "
#error  "                                 [     &&  <= 65535]                     "

#elif  ((PROBE_WIFI_CFG_TX_BUF_SIZE > 65535) || \
        (PROBE_WIFI_CFG_TX_BUF_SIZE < 32   ))
#error  "PROBE_WIFI_CFG_TX_BUF_SIZE      illegally #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 32   ]                     "
#error  "                                 [     &&  <= 65535]                     "
#endif



#ifndef  PROBE_WIFI_CFG_PORT
#error  "PROBE_WIFI_CFG_PORT                   not #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 1    ]                     "
#error  "                                 [     &&  <= 65535]                     "

#elif  ((PROBE_WIFI_CFG_PORT > 65535) || \
        (PROBE_WIFI_CFG_PORT < 1    ))
#error  "PROBE_WIFI_CFG_PORT             illegally #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be  >= 1    ]                     "
#error  "                                 [     &&  <= 65535]                     "
#endif



#ifndef  PROBE_WIFI_CFG_SOCK_BLOCK_EN
#error  "PROBE_WIFI_CFG_SOCK_BLOCK_EN          not #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]                  "
#error  "                                 [     || DEF_DISABLED]                  "

#elif  ((PROBE_WIFI_CFG_SOCK_BLOCK_EN != DEF_ENABLED) && \
        (PROBE_WIFI_CFG_SOCK_BLOCK_EN != DEF_DISABLED))
#error  "PROBE_WIFI_CFG_SOCK_BLOCK_EN    illegally #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be DEF_ENABLED ]                  "
#error  "                                 [     || DEF_DISABLED]                  "
#endif



#if     (PROBE_WIFI_CFG_SOCK_BLOCK_EN == DEF_DISABLED)
#ifndef  PROBE_WIFI_CFG_SOCK_WAIT_DLY
#error  "PROBE_WIFI_CFG_SOCK_WAIT_DLY          not #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be > 0]                           "

#elif   (PROBE_WIFI_CFG_SOCK_WAIT_DLY < 1)
#error  "PROBE_WIFI_CFG_SOCK_WAIT_DLY    illegally #define'd in 'probe_com_cfg.h'"
#error  "                                 [MUST be > 0]                           "
#endif
#endif


/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/

#endif


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : See 'MODULE  Note #1'.
*********************************************************************************************************
*/

#endif                                                          /* End of PROBE_WIFI module include (see Note #1).     */
