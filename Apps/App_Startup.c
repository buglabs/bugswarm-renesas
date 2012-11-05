/*-------------------------------------------------------------------------*
 * File:  App_Startup.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <system/platform.h>
#include <Apps/NVSettings.h>
#include <system/Switch.h>
#include <drv/Glyph/lcd.h>
#include <system/mstimer.h>
#include <system/console.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
NVSettings_t G_nvsettings;
char str_config_ssid[22];
uint8_t wifi_channel;
char    WiFiMAC[20];
char    WiFiMACStr[13];
void    VirginCheck(void);
/*---------------------------------------------------------------------------*
 * Routine:  App_InitModule
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the mode by first checking if there is a link and either
 *      report or continue to the rest of the program.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_InitModule(void)
{
    ATLIBGS_MSG_ID_E r = ATLIBGS_MSG_ID_NONE;

    DisplayLCD(LCD_LINE7, " Preparing");
    DisplayLCD(LCD_LINE8, "  Init Mod");

    /* Give the unit a little time to start up */
    /* (300 ms for GS1011 and 1000 ms for GS1500) */
    MSTimerDelay(1000);

    /* Check the link */
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Checking link\r\n");
#endif

    /* Wait for the banner (if any) */
    MSTimerDelay(500);

    /* Clear out the buffers */
    AtLibGs_FlushRxBuffer();

    /* Send command to check */
    do {
        AtLibGs_FlushIncomingMessage();
        DisplayLCD(LCD_LINE8, "Checking...");
        r = AtLibGs_Check();
    } while (ATLIBGS_MSG_ID_OK != r);

    /* Send command to DISABLE echo */
    do {
        DisplayLCD(LCD_LINE8, "Echo Off...");
        r = AtLibGs_SetEcho(ATLIBGS_DISABLE);
    } while (ATLIBGS_MSG_ID_OK != r);

    /* Done */
    DisplayLCD(LCD_LINE7, "");
    DisplayLCD(LCD_LINE8, "");
}

/*---------------------------------------------------------------------------*
 * Routine:  App_StartupLimitedAP
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into a limited AP mode using the configuration in the
 *      default LimitedAP settings.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_StartupLimitedAP(char *mySSID)
{
    ATLIBGS_MSG_ID_E r;
	
	//DisplayLCD(LCD_LINE3, "Limited AP:");
	//DisplayLCD(LCD_LINE4, (uint8_t const *)mySSID);

#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Starting Limited AP: %s\n", ATLIBGS_LIMITED_AP_SSID);
#endif

    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();
    while (1) {
        DisplayLCD(LCD_LINE6, " Setting up");
           
        r =AtLibGs_EnableRadio(1);                       // enable radio
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_Mode(ATLIBGS_STATIONMODE_LIMITED_AP);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_IPSet(ATLIBGS_LIMITED_AP_IP, ATLIBGS_LIMITED_AP_MASK,
                ATLIBGS_LIMITED_AP_GATEWAY);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad IP!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_EnableDHCPServer();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad DHCPSrv!");
            AtLibGs_DisableDHCPServer();
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_Assoc(mySSID /*ATLIBGS_LIMITED_AP_SSID*/, 0,
                ATLIBGS_LIMITED_AP_CHANNEL);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "AP Failed!");
            MSTimerDelay(2000);
            continue;
        }
        break;
    }
    DisplayLCD(LCD_LINE6, "");
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Limited AP Started\n");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  App_WebProvisioning
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into web provisioning mode and wait for the user to
 *      connect with a web browser, change the settings, and click Save.
 *      The settings will then be parsed by the AtLibGs library and
 *      get saved into the nv settings.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_WebProvisioning(void)
{
    ATLIBGS_MSG_ID_E r;

    /* At power up, load up the default settings */
    if(NVSettingsLoad(&G_nvsettings))
       NVSettingsSave(&G_nvsettings);
    
    App_InitModule();
    while(1)
    {
       r = AtLibGs_GetMAC(WiFiMAC);
       if(r != ATLIBGS_MSG_ID_OK) 
       {
          DisplayLCD(LCD_LINE6, "Get MAC Failed!");
          MSTimerDelay(2000);
          continue;
       } 
       break;
    }; 
    
    if(r == ATLIBGS_MSG_ID_OK)
      AtLibGs_ParseGetMacResponse(WiFiMACStr);    
    strcpy(str_config_ssid, (char const*)ATLIBGS_ADK_SSID); 
    strcat(str_config_ssid, &WiFiMACStr[6]);                     // concatenate last 6 digis of MAC as SSID  

	DisplayLCD(LCD_LINE2, "   Connect  ");    
	DisplayLCD(LCD_LINE3, "to Wifi SSID");
    DisplayLCD(LCD_LINE4, (uint8_t const *)str_config_ssid);
    App_StartupLimitedAP(str_config_ssid);
    
    /* Before going into web provisioning, provide DNS to give a link. */
    /* The user can then go to http://webprov.gainspan.com/gsclient.html to get */
    /* access to the web provisioning screen. */
    while (1) {
#if 0
        AtLibGs_DisableDNSServer();
        r = AtLibGs_EnableDNSServer("webprov.gainspan.com");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad DNS!");
            MSTimerDelay(2000);
            continue;
        }
#endif
        r = AtLibGs_WebProv(",", ",");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad WebProv!");
            MSTimerDelay(2000);
            continue;
        }
        break;
    }
	DisplayLCD(LCD_LINE5, "   And     ");	
    DisplayLCD(LCD_LINE6, "Open Page: ");
    DisplayLCD(LCD_LINE7, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE8, (const uint8_t *) "1/prov.html");  
#if 0
    do {
        DisplayLCD(LCD_LINE7, "IP: ???.???.");
        DisplayLCD(LCD_LINE8, "    ???.???");
        r = AtLibGs_GetNetworkStatus(&network_status);
    } while (ATLIBGS_MSG_ID_OK != r);

    sprintf(text, "IP: " _F8_ "." _F8_ ".",
            network_status.addr.ipv4[0], network_status.addr.ipv4[1]);
    DisplayLCD(LCD_LINE7, (uint8_t *)text);
    sprintf(text, "    " _F8_ "." _F8_, network_status.addr.ipv4[2],
            network_status.addr.ipv4[3]);
    DisplayLCD(LCD_LINE8, (uint8_t *)text);
#endif
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Web Provisioning ON\n");
#endif

    /* Now wait for a list of responses until we get a blank line */
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Waiting for web provisioning response...\n");
#endif
    AtLibGs_GetWebProvSettings(&G_nvsettings.webprov, 0);

    /* Save the above settings */
    NVSettingsSave(&G_nvsettings);
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Web provisioning complete.\n");
#endif

	ClearLCD();
	DisplayLCD(LCD_LINE1, "*SSID CONFIG");
    DisplayLCD(LCD_LINE2, "  COMPLETE  ");
	if (!G_nvsettings.webprov.dhcp_enable) {
		DisplayLCD(LCD_LINE4, "WARN:       ");
		DisplayLCD(LCD_LINE5, "DHCP IS OFF ");
		DisplayLCD(LCD_LINE6, "CHK SETTINGS");
	}
    DisplayLCD(LCD_LINE7, "Press RESET");
    //while (1)
    //    {}
}

/*---------------------------------------------------------------------------*
 * Routine:  App_StartWPS
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into WPS pushbutton mode. After pushing the button on the 
 *      AP the unit will retrieve its SSID and pass phrase, then connect to it
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_StartWPS(void)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    ATLIBGS_NetworkStatus network;
    AtLibGs_WPSResult result;
    char text[20];
    
    while (1) {
        // Ensure we are not connected to any network (from previous runs)
        AtLibGs_DisAssoc();

        /* Pushbutton WPS demo */
        /* Use Wi-Fi Protected Setup (WPS) FW */
        /* turn on DHCP client */
        AtLibGs_DHCPSet(1);

        /* set to connect to AP mode */
        AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE);
        DisplayLCD(LCD_LINE5, "  Push the  ");
        DisplayLCD(LCD_LINE6, "button on AP");

        /* push the button on the AP so the GS module can connect */
        while (AtLibGs_StartWPSPUSH(&result) != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE8, " Not found!");
            MSTimerDelay(1000);
            DisplayLCD(LCD_LINE8, " Retrying...");
            MSTimerDelay(1000);
            DisplayLCD(LCD_LINE8, "");
        }

        /* Connect to AP (found from pushbutton) after setting pass phrase */
        AtLibGs_SetPassPhrase(result.password);
        AtLibGs_Assoc(result.ssid, "", result.channel);

        rxMsgId = AtLibGs_GetNetworkStatus(&network);
        if (rxMsgId != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE8, "Bad Network!");
            MSTimerDelay(2000);
            DisplayLCD(LCD_LINE8, "");
            continue;
        } else {
            strncpy(text, network.ssid, 12);
            DisplayLCD(LCD_LINE4, (const uint8_t *)text);
        }
        break;
    }

    DisplayLCD(LCD_LINE5, "");
    DisplayLCD(LCD_LINE6, "");
}
/*---------------------------------------------------------------------------*
 * Routine:  App_StartupADKDemo
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into a limited AP mode, enable XML parse, and setup mDNS
 *      
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_StartupADKDemo(void)
{
    ATLIBGS_MSG_ID_E r;

   // DisplayLCD(LCD_LINE3, "Limited AP");
   // DisplayLCD(LCD_LINE4, ATLIBGS_LIMITED_AP_SSID);   
   // VirginCheck();   
#if 1                     
    DisplayLCD(LCD_LINE3, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE4, (const uint8_t *) "1/rdk.html");  
#else   
    DisplayLCD(LCD_LINE3, (const uint8_t *) "192.168.1.1/");
    DisplayLCD(LCD_LINE4, (const uint8_t *) "rdk.html"); 
#endif
    
    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();
    while (1) {
        DisplayLCD(LCD_LINE6, " Setting up");
        
        r =AtLibGs_EnableRadio(1);                       // enable radio
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            continue;
        }  
#if 0       
        r = AtLibGs_ConfigAntenna(1);
         if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Configure Antenna Fail!");
            MSTimerDelay(2000);
            continue;
        }  
#endif       
        r = AtLibGs_Mode(ATLIBGS_STATIONMODE_LIMITED_AP);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_IPSet(ATLIBGS_ADK_IP, ATLIBGS_ADK_MASK,
                ATLIBGS_ADK_GATEWAY);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad IP!");
            MSTimerDelay(2000);
            continue;
        }
        AtLibGs_DisableDHCPServer();
        r = AtLibGs_EnableDHCPServer();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad DHCPSrv!");
            MSTimerDelay(2000);
            continue;
        }
 #if 0             
        r = AtLibGs_SetRegulatoryDomain(ATLIBGS_REGDOMAIN_TELEC);
         if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Set Domain Fail");
            MSTimerDelay(2000);
            continue;
        }
#endif        
         r = AtLibGs_GetMAC(WiFiMAC);
         if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Get MAC Failed!");
            MSTimerDelay(2000);
            continue;
        } 
        
        if(r == ATLIBGS_MSG_ID_OK)
           AtLibGs_ParseGetMacResponse(WiFiMACStr);
        
        strcpy(str_config_ssid, (char const*)ATLIBGS_ADK_SSID); 
        strcat(str_config_ssid, &WiFiMACStr[6]);                     // concatenate last 6 digis of MAC as SSID                          
        DisplayLCD(LCD_LINE1, (const uint8_t *)str_config_ssid);   
        #ifdef ATLIBGS_DEBUG_ENABLE
            ConsolePrintf("Starting Limited AP: %s\n",str_config_ssid);
        #endif
        r = AtLibGs_Assoc(str_config_ssid /*ATLIBGS_ADK_SSID*/, 0,
                ATLIBGS_ADK_CHANNEL);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "AP Failed!");
            MSTimerDelay(2000);
            continue;
        }        
        r = AtLibGs_WebServer(1, "", "", "", "");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Server Failed!");
            MSTimerDelay(2000);
            continue;
        }       
        r = AtLibGs_SetXMLParse(1);;
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "XML Failed!");
            MSTimerDelay(2000);
            continue;
        }  
#if 1 // mDNS_ENABLED
        // now start mNDS service 
        r = AtLibGs_StartMDNS();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS1 Failed!");
            MSTimerDelay(2000);
            continue;
        }        
        r = AtLibGs_RegisterMDNSHost("xyz","local");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS2 Failed!");
            MSTimerDelay(2000);
            continue;
        }             
        r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER,"","_http","_tcp","local","80","path=/gainspan/profile/mcu");
      //  r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER,"","_http","_tcp","local","80","path=/rdk.html");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS3 Failed!");
            MSTimerDelay(2000);
            continue;
        }          
        r = AtLibGs_AnnounceMDNS();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS4 Failed!");
            MSTimerDelay(2000);
            continue;
        }
#endif
        break;    
    }
    DisplayLCD(LCD_LINE6, "");
    MSTimerDelay(2000);
    AtLibGs_FlushIncomingMessage();
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("ADK Demo Started\n");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  App_OverTheAirProgramming
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into over the air programming mode after connecting to an
 *      access point in infrastructure mode.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_OverTheAirProgramming(void)
{
    char buf[12] = "           ";
    int i;

    /* OTA Firmware update demo */
    /* Connect to AP first, then do FW upgrade with PC application */
    AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE); /* set to connect to AP mode */
    AtLibGs_DHCPSet(1); /* turn on DHCP client */
    AtLibGs_SetPassPhrase(ATLIBGS_AP_SEC_PSK);
    AtLibGs_Assoc(ATLIBGS_AP_SSID, 0, 0);
    AtLibGs_EnableRadio(1); /* radio always on */

    DisplayLCD(LCD_LINE6, "Start Update");
    DisplayLCD(LCD_LINE7, "on server IP");
    /* print last 12 (of 15) char of server IP address */

    for (i = 0; i < 12; i++) {
        buf[i] = (char)ATLIBGS_FWUPGRADE_SERVER[i + 3];
    }
    DisplayLCD(LCD_LINE8, (uint8_t *)buf);

    /* start FW update on remote port 8010 with 100 retries */
    AtLibGs_FWUpgrade(ATLIBGS_FWUPGRADE_SERVER, 8010, 3000, "100");
}

/*---------------------------------------------------------------------------*
 * Routine:  App_OverTheAirProgrammingPushMetheod
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into over the air programming mode in Push Method after connecting to an
 *      access point in infrastructure mode.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_OverTheAirProgrammingPushMetheod(void)
{
    ATLIBGS_MSG_ID_E r;

    App_InitModule();
     
    while(1)
    {
       r = AtLibGs_GetMAC(WiFiMAC);
       if(r != ATLIBGS_MSG_ID_OK) 
       {
          DisplayLCD(LCD_LINE6, "Get MAC Failed!");
          MSTimerDelay(2000);
          continue;
       } 
       break;
    }; 
    
    if(r == ATLIBGS_MSG_ID_OK)
      AtLibGs_ParseGetMacResponse(WiFiMACStr);    
    strcpy(str_config_ssid, (char const*)ATLIBGS_ADK_SSID); 
    strcat(str_config_ssid, &WiFiMACStr[6]);                     // concatenate last 6 digis of MAC as SSID  
    
    App_StartupLimitedAP(str_config_ssid);
    
    r = AtLibGs_WebProv(",", ",");
    while(r != ATLIBGS_MSG_ID_OK) {
        DisplayLCD(LCD_LINE6, "Bad WebProv!");
        MSTimerDelay(2000);
        continue;
    }
    DisplayLCD(LCD_LINE6, "Download ON");   
    DisplayLCD(LCD_LINE7, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE8, (const uint8_t *) "1/otafu.html");  
    while(1)
      ;
}

/*---------------------------------------------------------------------------*
 * Routine:  App_ProgramMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into programming mode by putting the control pin into
 *      program mode and mimicing the TX/RX lines on SCI2 with the
 *      RX/TX lines of SCI6.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_Startup(void)
{
    /* At power up, load up the default settings */
    if(NVSettingsLoad(&G_nvsettings))
       NVSettingsSave(&G_nvsettings);

    /* Initialize the module now that a mode is chosen (above) */
    App_InitModule();
#if 0
    /* Grab switch 1 state at startup before we init the module (which */
    /* can take longer than people want to hold the button). */
    sw1 = Switch1IsPressed();
    sw2 = Switch2IsPressed();
    sw3 = Switch3IsPressed();

    /* Show the mode immediately before initialization */
    if (sw1) {
        DisplayLCD(LCD_LINE3, "SW1 Pressed!");
      } else if (sw2) {
          DisplayLCD(LCD_LINE3, "SW2 Pressed!");
      } else if (sw3) {
          DisplayLCD(LCD_LINE3, "SW3 Pressed!");
    }
    /* Initialize the module now that a mode is chosen (above) */
    App_InitModule();

    /* Was switch1 held? */
    if (sw1) {
        /* Yes, then go into Limited AP point */
        App_StartupLimitedAP();

        /* Now go into web provisioning mode */
        App_WebProvisioning();
    } else if (sw2) {
        App_StartWPS();

        /* Now go into web provisioning mode */
        App_WebProvisioning();
    } else if (sw3) {
        /* User wants to do over the air programming */
        App_OverTheAirProgramming();
    }
    else
#endif
      App_StartupADKDemo();
}

#if 0

const uint8_t GainspanSignature[]="Gainspan";
#define GAINSPAN_SIGNATURE_ADDR   sizeof(NVSettings_t)  //  0
#define GAINSPAN_SIGNATURE_LEN    8

#define GAINSPAN_SSID_MAX_LEN     20
#define GAINSPAN_SSID_ADDR       (GAINSPAN_SIGNATURE_ADDR + GAINSPAN_SIGNATURE_LEN)

#define GAINSPAN_CHANNEL_ADDR    (GAINSPAN_SSID_ADDR + GAINSPAN_SSID_MAX_LEN)    
#define GAINSPAN_CHANNEL_MAX_LEN  1

void VirginCheck(void)
{
   uint8_t cBuffer[20];
   
   //EEPROM_Test();  
   memset((uint8_t *)cBuffer, 0x00, 19);
   
   
   // EEPROM_Write(GAINSPAN_SIGNATURE_ADDR, "12345678", GAINSPAN_SIGNATURE_LEN);
    EEPROM_Read(GAINSPAN_SIGNATURE_ADDR, cBuffer, GAINSPAN_SIGNATURE_LEN);
   
   EEPROM_Read(GAINSPAN_SIGNATURE_ADDR, cBuffer, GAINSPAN_SIGNATURE_LEN);
   EEPROM_Read(98, cBuffer, GAINSPAN_SIGNATURE_LEN);
   cBuffer[GAINSPAN_SIGNATURE_LEN+1]=0;
   
   if(strcmp((char const *)GainspanSignature, (char const*)cBuffer))                       // write signature "Gainspan"
   {
     EEPROM_Write(GAINSPAN_SIGNATURE_ADDR, "Gainspan", GAINSPAN_SIGNATURE_LEN);
     EEPROM_Read(GAINSPAN_SIGNATURE_ADDR, cBuffer, GAINSPAN_SIGNATURE_LEN);
     strcpy(str_config_ssid, (char const*)ATLIBGS_ADK_SSID);  
     AtLibGs_GetMAC((char *)cBuffer);
     
     strncat(str_config_ssid, (char const*)&cBuffer[9],2);
     strncat(str_config_ssid, (char const*)&cBuffer[12],2);
     strncat(str_config_ssid, (char const*)&cBuffer[15],2);
     
     EEPROM_Write(GAINSPAN_SSID_ADDR, str_config_ssid, GAINSPAN_SSID_MAX_LEN); 
   }
#if 1 
   memset(str_config_ssid, 0x00, GAINSPAN_SSID_MAX_LEN);
   EEPROM_Read(GAINSPAN_SSID_ADDR, str_config_ssid, GAINSPAN_SSID_MAX_LEN);                       // read SSID from EEPROM
  
   EEPROM_Read(GAINSPAN_CHANNEL_ADDR, &wifi_channel, GAINSPAN_CHANNEL_MAX_LEN);                   // read WiFi channel from EEPROM

    if(wifi_channel<1 || wifi_channel>14)                                                     // something wrong, set to default channel
    {
      wifi_channel = 6;
      EEPROM_Write(GAINSPAN_CHANNEL_ADDR, &wifi_channel, GAINSPAN_CHANNEL_MAX_LEN);
    }
#endif
} 
#endif
/*-------------------------------------------------------------------------*
 * End of File:  App_Startup.c
 *-------------------------------------------------------------------------*/
