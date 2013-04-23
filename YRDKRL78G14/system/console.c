/*-------------------------------------------------------------------------*
 * File:  console.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <system/platform.h>
#include <drv/UART.h>

#define CONSOLE_ECHO 1

#ifndef CONSOLE_BUFFER_SIZE
    #error "CONSOLE_BUFFER_SIZE must be defined in platform.h"
#endif

/*---------------------------------------------------------------------------*
 * Routine:  ConsolePrintf
 *---------------------------------------------------------------------------*
 * Description:
 *      Output a formatted string of text to the console.
 * Inputs:
 *      const char *format -- Format of string to output
 *      ... -- 0 or more parameters
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConsolePrintf(const char *format, ...)
{
    static volatile uint8_t buffer[CONSOLE_BUFFER_SIZE];
    volatile uint8_t *p;
    va_list args;

    /* Start processing the arguments */
    va_start(args, format);

    /* Output the parameters into a string */
    //vsprintf((char *)buffer, format, args);
	vsnprintf((char *)buffer, CONSOLE_BUFFER_SIZE-1, format, args);

    /* Output the string to the console */
    p = buffer;
    while (*p) {
        if (*p == '\n')
            UART1_SendByte('\r');
        UART1_SendByte(*p);
        if (*p == '\n')
            while (!UART1_IsTransmitEmpty()) {
            }
        p++;
    }

    /* End processing of the arguments */
    va_end(args);
}

/*---------------------------------------------------------------------------*
 * Routine:  ConsoleSendString
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a string to the console.
 * Inputs:
 *      const char *string -- string of characters to output on console
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConsoleSendString(const char *string)
{
    const char *p = string;

    /* Process all characters in string */
    while (*p) {
        if (*p == '\n')
            UART0_SendByte('\r');
        UART0_SendByte(*p);
        if (*p == '\n')
            while (!UART0_IsTransmitEmpty()) {
            }
        p++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ConsoleReadLine
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a string to the console.
 * Inputs:
 *      const char *string -- string of characters to output on console
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ConsoleReadLine(char *string)
{
    char *p = string;

    while(1) {
        if(UART0_ReceiveByte((uint8_t *)p)) {
          
#if CONSOLE_ECHO
            UART0_SendByte((uint8_t)*p);
#endif
            
            if((*p == '\n')||(*p == '\r')) {
                *p = '\0';
                break;
            }
            p++;
        }
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  console.c
 *-------------------------------------------------------------------------*/
