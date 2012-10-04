/*-------------------------------------------------------------------------*
 * File:  App_Connect.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Routines for connecting to the network.  Usually done at power up.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <HostApp.h>
#include <system/platform.h>
#include <system/console.h>
#include <CmdLib/AtCmdLib.h>
#include <CmdLib/GainSpan_SPI.h>
#include <system/mstimer.h>
#include <drv/Glyph/lcd.h>
#include "Apps.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E App_Connect(ATLIBGS_WEB_PROV_SETTINGS *wp)
{
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
    char ip[20];
    char subnet[20];
    char gateway[20];

    while (1) {
        DisplayLCD(LCD_LINE7, " Connecting ");
        DisplayLCD(LCD_LINE8, "");

        AtLibGs_DisAssoc();

        AtLibGs_FlushIncomingMessage();

        // Set the mode of operation
        rxMsgId = AtLibGs_Mode(wp->station);
        if (rxMsgId != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            DisplayLCD(LCD_LINE6, "");
            continue;
        }

        // Turn on DHCP?
        if (wp->dhcp_enable) {
            rxMsgId = AtLibGs_DHCPSet(1);
            if (rxMsgId != ATLIBGS_MSG_ID_OK) {
                DisplayLCD(LCD_LINE8, "Bad DHCP!");
                MSTimerDelay(2000);
                DisplayLCD(LCD_LINE6, "");
                continue;
            }
        } else {
            // Disable, set settings manually
            DisplayLCD(LCD_LINE8, "DHCP On...");
            rxMsgId = AtLibGs_DHCPSet(0);
            if (rxMsgId != ATLIBGS_MSG_ID_OK) {
                DisplayLCD(LCD_LINE8, "Bad DHCP!");
                MSTimerDelay(2000);
                DisplayLCD(LCD_LINE6, "");
                continue;
            }

            // Setup the IP address
            AtLibGs_IPv4AddressToString(&wp->ip, ip);
            AtLibGs_IPv4AddressToString(&wp->subnet, subnet);
            AtLibGs_IPv4AddressToString(&wp->gateway, gateway);
            rxMsgId = AtLibGs_IPSet(ip, subnet, gateway);
            if (rxMsgId != ATLIBGS_MSG_ID_OK) {
                DisplayLCD(LCD_LINE8, "Bad IP!");
                MSTimerDelay(2000);
                DisplayLCD(LCD_LINE6, "");
                continue;
            }
        }

        if (wp->security != ATLIBGS_SMOPEN) {
            /* Store the PSK value. This call takes might take few seconds to return */
            do {
                DisplayLCD(LCD_LINE8, "Setting PSK");
                rxMsgId = AtLibGs_CalcNStorePSK(wp->ssid, wp->password);
                if (rxMsgId != ATLIBGS_MSG_ID_OK) {
                    DisplayLCD(LCD_LINE8, "Bad PSK!");
                    MSTimerDelay(2000);
                    DisplayLCD(LCD_LINE8, "");
                    continue;
                }
            } while (ATLIBGS_MSG_ID_OK != rxMsgId);
        }

        DisplayLCD(LCD_LINE8, "");

        /* Associate to a particular AP specified by SSID  */
        rxMsgId = AtLibGs_Assoc(wp->ssid, "", wp->channel);
        if (ATLIBGS_MSG_ID_OK != rxMsgId) {
            /* Association error - we can retry */
#ifdef ATLIBGS_DEBUG_ENABLE
            ConsolePrintf("\n Association error - retry now \n");
#endif
            DisplayLCD(LCD_LINE7, "** Failed **");
            MSTimerDelay(2000);
            DisplayLCD(LCD_LINE7, "");
            continue;
        } else {
            /* Association success */
            AtLibGs_SetNodeAssociationFlag();
            DisplayLCD(LCD_LINE7, " Connected");
            MSTimerDelay(500);
            DisplayLCD(LCD_LINE7, "");
        }
        break;
    }
    return rxMsgId;
}

/*-------------------------------------------------------------------------*
 * End of File:  App_Connect.c
 *-------------------------------------------------------------------------*/
