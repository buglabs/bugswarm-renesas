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
#include <NVSettings.h> 
#include <system/Switch.h>
#include <drv/Glyph/lcd.h>
#include <mstimer.h>
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
NVSettings_t GNV_Setting;
char str_config_ssid[25];
uint8_t wifi_channel;
char    WiFiMAC[20];
char    WiFiMACStr[13];
extern char MRBuffer[ATLIBGS_RX_CMD_MAX_SIZE];
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

    /* Give the unit a little time to start up */
    /* (300 ms for GS1011 and 1000 ms for GS1500) */
    MSTimerDelay(1000);

    /* Check the link */
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Checking link\r\n");
#endif

    /* Wait for the banner (if any) */
    // MSTimerDelay(500);

    /* Clear out the buffers */
    AtLibGs_FlushRxBuffer();

    /* Send command to check */
    do {
        AtLibGs_FlushIncomingMessage();
        DisplayLCD(LCD_LINE8, "Checking...");
        r = AtLibGs_Check();
    
        if (r != ATLIBGS_MSG_ID_OK) {
            MSTimerDelay(1000);
            continue;  
        }          
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

#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Starting Limited AP: %s\n", mySSID);
#endif

    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();
    while (1) {
        DisplayLCD(LCD_LINE6, " Setting up");

        r = AtLibGs_IPSet(ATLIBGS_LIMITED_AP_IP, ATLIBGS_LIMITED_AP_MASK,
                ATLIBGS_LIMITED_AP_GATEWAY);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad IP!");
            MSTimerDelay(2000);
            continue;
        }
        r =AtLibGs_EnableRadio(1);                       // enable radio
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Radio Mode!");
            MSTimerDelay(2000);
            continue;
        }
        do {
           r = AtLibGs_Version();                        // check the GS version
        }while (ATLIBGS_MSG_ID_OK != r);

        if(strstr((const char *)MRBuffer, "3.4."))       // This is a GS1500 2.2 version
        {
          r = AtLibGs_ConfigAntenna(1);                  // set to PCB antenna
           if (r != ATLIBGS_MSG_ID_OK) {
              DisplayLCD(LCD_LINE6, "Configure Antenna Fail!");
              MSTimerDelay(2000);
              continue;
          }
#ifdef ATLIBGS_DEBUG_ENABLE
          ConsolePrintf("This is a GS1500\n");
#endif
        }       
#if 0             
        r = AtLibGs_SetRegulatoryDomain(ATLIBGS_REGDOMAIN_TELEC);
         if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Set Domain Fail");
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
        r = AtLibGs_Assoc(mySSID /*ATLIBGS_LIMITED_AP_SSID*/, 0,
                ATLIBGS_LIMITED_AP_CHANNEL);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "AP Failed!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_EnableDHCPServer();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Try DHCPSrv!");
            AtLibGs_DisableDHCPServer();
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
 * Routine:  App_aClientConnection
 *---------------------------------------------------------------------------*
 * Description:
 *      Connect an AP with static or dynamitc IP
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_aClientConnection(void)
{
    ATLIBGS_MSG_ID_E rxMsgId; static ATLIBGS_AUTHMODE_E WEPMode=ATLIBGS_AUTHMODE_OPEN_WEP;
    
    /*  load up the settings */
    if(NVSettingsLoad(&GNV_Setting))
       NVSettingsSave(&GNV_Setting);
   
 /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();
    while (1) 
    {  
      rxMsgId = AtLibGs_Mode(ATLIBGS_STATIONMODE_INFRASTRUCTURE);
      if (rxMsgId != ATLIBGS_MSG_ID_OK) 
      {
          DisplayLCD(LCD_LINE6, "Bad Mode!");
          MSTimerDelay(2000);
          continue;
      }        
      // Enable DHCP
      //DisplayLCD(LCD_LINE8, "DHCP On...");
      rxMsgId = AtLibGs_DHCPSet(1);
      if (rxMsgId != ATLIBGS_MSG_ID_OK) 
      {
          DisplayLCD(LCD_LINE6, "Cmad Fail!");
          MSTimerDelay(2000);
          continue;
      }
      break;
    }
    
    // setup security
    if(strlen(GNV_Setting.webprov.ssid) > 0)
    { 
  
      if(GNV_Setting.webprov.security == ATLIBGS_PROVSECU_WEP)
      {
          do {
            DisplayLCD(LCD_LINE8, " Setting WEP");
            rxMsgId = AtLibGs_SetWEP1((int8_t*)GNV_Setting.webprov.password);
          } while (ATLIBGS_MSG_ID_OK != rxMsgId); 
          DisplayLCD(LCD_LINE8, " WEP Set");      
      }
      else if(GNV_Setting.webprov.security == ATLIBGS_PROVSECU_WPA_PER)
      {
          do {
            //DisplayLCD(LCD_LINE8, " Setting PSK");
            rxMsgId = AtLibGs_CalcNStorePSK(GNV_Setting.webprov.ssid, GNV_Setting.webprov.password);
          } while (ATLIBGS_MSG_ID_OK != rxMsgId); 
          //DisplayLCD(LCD_LINE8, " PSK Set");
      }
      else if(GNV_Setting.webprov.security == ATLIBGS_PROVSECU_WPA_ENT)
      {
       /* Set AT+WAUTH=0 for WPA or WPA2  */
           do {
             DisplayLCD(LCD_LINE8, "       " );
             rxMsgId = AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_NONE_WPA);
           } while (ATLIBGS_MSG_ID_OK != rxMsgId);
         /* Security Configuration */
           do {
             DisplayLCD(LCD_LINE8, "Set Security");
             rxMsgId = AtLibGs_SetSecurity(ATLIBGS_SMAUTO);
           } while (ATLIBGS_MSG_ID_OK != rxMsgId);  
       }
     }
     else
     {  
                                            // use default security
     }
        
     while(1)
     {
       /* Associate to a particular AP specified by SSID  */
        if (strlen(GNV_Setting.webprov.ssid) > 0)
        {
          //DisplayLCD(LCD_LINE8, (const uint8_t *)GNV_Setting.webprov.ssid);
          rxMsgId = AtLibGs_Assoc(GNV_Setting.webprov.ssid,NULL,HOST_APP_AP_CHANNEL);
        }
        else
        {
          //DisplayLCD(LCD_LINE8, HOST_APP_AP_SSID);
          rxMsgId = AtLibGs_Assoc(HOST_APP_AP_SSID, NULL, HOST_APP_AP_CHANNEL);
        }
    
        if (ATLIBGS_MSG_ID_OK != rxMsgId) 
        {
          /* Association error - we can retry */
           if(GNV_Setting.webprov.security == ATLIBGS_PROVSECU_WEP)
           {
             if(WEPMode==ATLIBGS_AUTHMODE_OPEN_WEP)
             {
               rxMsgId = AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_SHARED_WEP);// Potenially it's a WEP shared AP
               WEPMode=ATLIBGS_AUTHMODE_SHARED_WEP;
             }
             else
             {
               rxMsgId = AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_OPEN_WEP);// Potenially it's a WEP shared AP
               WEPMode=ATLIBGS_AUTHMODE_OPEN_WEP;         
             }
           }
      #ifdef HOST_APP_DEBUG_ENABLE
              ConsolePrintf("\n Association error - retry now \n");
      #endif
          //DisplayLCD(LCD_LINE7, " Connecting..");
          MSTimerDelay(2000);
          DisplayLCD(LCD_LINE7, "");
        } 
        else 
        {
          /* Association success */       
          //DisplayLCD(LCD_LINE1, (const uint8_t *) "Connected to");
          //DisplayLCD(LCD_LINE2, (const uint8_t *) GNV_Setting.webprov.ssid);   
          
          //AtLibGs_GetIPAddress((uint8_t*) str_config_ssid);           // we just reuse the buffer to get IP address
          
          //DisplayLCD(LCD_LINE4, (const uint8_t *)str_config_ssid);
          //DisplayLCD(LCD_LINE5, (const uint8_t *)&str_config_ssid[12]);
          break;
        }  
     }
}

/*---------------------------------------------------------------------------*
 * Routine:  App_WebProvisioning_OverAirPush
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into web provisioning mode and over air push mode download mode ,
 *      then wait for the user to connect with a web browser, or app, change the settings, and 
 *      click Save or upgrade the WLan, App, webpage.
 *      The settings will then be parsed by the AtLibGs library and
 *      get saved into the nv settings.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_WebProvisioning_OverAirPush(void)
{
    ATLIBGS_MSG_ID_E r;

    /* At power up, load up the default settings */
    if(NVSettingsLoad(&GNV_Setting))
       NVSettingsSave(&GNV_Setting);
    
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
    strcpy(str_config_ssid, (char const*)ATLIBGS_LIMITED_AP_SSID); 
    strcat(str_config_ssid, &WiFiMACStr[6]);                     // concatenate last 6 digis of MAC as SSID  
    
    DisplayLCD(LCD_LINE1, "Limited AP:");
    DisplayLCD(LCD_LINE2, (uint8_t const *)str_config_ssid);

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
#if 1 // mDNS_ENABLED
        // now start mNDS service 
        r = AtLibGs_StartMDNS();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS1 Failed!");
            MSTimerDelay(2000);
            continue;
        }        
        r = AtLibGs_RegisterMDNSHost("Renesas","local");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS2 Failed!");
            MSTimerDelay(2000);
            continue;
        }             
        r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER_RPOV,"","_http","_tcp","local","80","", "path=/prov.html");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS3 Failed!");
            MSTimerDelay(2000);
            continue;
        }
        r = AtLibGs_RegisterMDNSService("ota-fwu","","_http","_tcp","local","80","","path=/otafu.html");
        if (r != ATLIBGS_MSG_ID_OK) {
           DisplayLCD(LCD_LINE6, "MDNS3-2 Failed!");
            MSTimerDelay(2000);
            continue;
        }
        
        //MSTimerDelay(1000);                  // put some delay here to wait for previous response
        r = AtLibGs_AnnounceMDNS();
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS4 Failed!");
            MSTimerDelay(2000);
            continue;
        }             
 #endif       
        break;
    }
  //  DisplayLCD(LCD_LINE6, "WebProv ON");
    DisplayLCD(LCD_LINE4, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE5, (const uint8_t *) "1/prov.html");
    
    DisplayLCD(LCD_LINE7, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE8, (const uint8_t *) "1/otafu.html");  

#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Web Provisioning ON\n");
#endif

    /* Now wait for a list of responses until we get a blank line */
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Waiting for web provisioning response...\n");
#endif
    AtLibGs_GetWebProvSettings(&GNV_Setting.webprov, 0);

    /* Save the above settings */
    NVSettingsSave(&GNV_Setting);
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Web provisioning complete.\n");
#endif

    DisplayLCD(LCD_LINE6, "WebProv Done");
    DisplayLCD(LCD_LINE7, "");
    DisplayLCD(LCD_LINE8, "Press RESET");
    while (1)
        {}
}

#if 0
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
#endif
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
void App_StartupADKDemo(uint8_t isLimiteAPmode)
{
    ATLIBGS_MSG_ID_E r;

   // DisplayLCD(LCD_LINE3, "Limited AP");
   // DisplayLCD(LCD_LINE4, ATLIBGS_LIMITED_AP_SSID);  
   /* Initialize the module now that a mode is chosen (above) */
    App_InitModule();
  
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Starting Limited AP: %s\n", ATLIBGS_LIMITED_AP_SSID);
#endif

    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();
    while (1) {
        DisplayLCD(LCD_LINE6, " Setting up");
        
        if(isLimiteAPmode)                                                               // Run the limited AP mode
        {
          DisplayLCD(LCD_LINE3, (const uint8_t *) "192.168.240.");
          DisplayLCD(LCD_LINE4, (const uint8_t *) "1/rdk.html");
          r = AtLibGs_GetMAC(WiFiMAC);
          if(r != ATLIBGS_MSG_ID_OK)
          {
              DisplayLCD(LCD_LINE6, "Get MAC Failed!");
              MSTimerDelay(2000);
              continue;
           }
  
          if(r == ATLIBGS_MSG_ID_OK)
            AtLibGs_ParseGetMacResponse(WiFiMACStr);
          strcpy(str_config_ssid, (char const*)ATLIBGS_LIMITED_AP_SSID);
          strcat(str_config_ssid, &WiFiMACStr[6]);                       // concatenate last 6 digis of MAC as SSID
          DisplayLCD(LCD_LINE1, (const uint8_t *)str_config_ssid); 
          App_StartupLimitedAP(str_config_ssid);
        }                                                              
        else                                                             //  // run the client mode   
        {
          App_aClientConnection();    
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
        r = AtLibGs_RegisterMDNSHost(ATLIBGS_ADK_MDNS_SERVER,"local");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS2 Failed!");
            MSTimerDelay(2000);
            continue;
        }             
       // r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER,"","_http","_tcp","local","80","","path=/rdk.html,api=gs_profile_mcu:1.0.0:/gainspan/profile/mcu");
        r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER,"","_http","_tcp","local","80","","path=/rdk.html");
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
 * Routine:  App_StartupProbeDemo
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the unit into a limited AP mode.
 *      
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_StartupProbeDemo(uint8_t isLimiteAPmode)
{
    ATLIBGS_MSG_ID_E r;
    char lcd_line[32];

    /* Initialize the module now that a mode is chosen (above) */
    App_InitModule();

    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc();

    while (1) {
        DisplayLCD(LCD_LINE6, " Setting up");
        
        if(isLimiteAPmode)                                                               // Run the limited AP mode
        {
          r = AtLibGs_GetMAC(WiFiMAC);
          if(r != ATLIBGS_MSG_ID_OK)
          {
              DisplayLCD(LCD_LINE6, "Get MAC Failed!");
              MSTimerDelay(2000);
              continue;
          }
  
          if(r == ATLIBGS_MSG_ID_OK)
            AtLibGs_ParseGetMacResponse(WiFiMACStr);

          strcpy(str_config_ssid, (char const*)ATLIBGS_LIMITED_AP_SSID);
          strcat(str_config_ssid, &WiFiMACStr[6]);                       // concatenate last 6 digis of MAC as SSID
          DisplayLCD(LCD_LINE6, (const uint8_t *)str_config_ssid); 
          App_StartupLimitedAP(str_config_ssid);
          sprintf(&lcd_line[0], "SSID: %s", str_config_ssid);
          DisplayLCD(LCD_LINE6, (const uint8_t *)lcd_line); 
        }                                                              
        else                                                             //  // run the client mode   
        {
          // connect to WiFi router    
        }
        break;    
    }
    MSTimerDelay(2000);
    AtLibGs_FlushIncomingMessage();
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Probe Demo Started\n");
#endif
}
#if 0
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

    /* At power up, load up the default settings */
    if(NVSettingsLoad(&GNV_Setting))
       NVSettingsSave(&GNV_Setting);
    
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
    strcpy(str_config_ssid, (char const*)ATLIBGS_LIMITED_AP_SSID); 
    strcat(str_config_ssid, &WiFiMACStr[6]);                     // concatenate last 6 digis of MAC as SSID  
    
    App_StartupLimitedAP(str_config_ssid);
    
    /* Before going into web provisioning, provide DNS to give a link. */
    /* The user can then go to http://webprov.gainspan.com/gsclient.html to get */
    /* access to the web provisioning screen. */
    while (1) 
    {
        r = AtLibGs_WebProv(",", ",");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad WebProv!");
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
        r = AtLibGs_RegisterMDNSHost("Renesas","local");
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "MDNS2 Failed!");
            MSTimerDelay(2000);
            continue;
        }             
        r = AtLibGs_RegisterMDNSService(ATLIBGS_ADK_MDNS_SERVER_OTAFU,"","_http","_tcp","local","80","", "path=/otafu.html");
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
       
    DisplayLCD(LCD_LINE6, "Download ON");   
    DisplayLCD(LCD_LINE7, (const uint8_t *) "192.168.240.");
    DisplayLCD(LCD_LINE8, (const uint8_t *) "1/otafu.html");   

    AtLibGs_GetWebProvSettings(&GNV_Setting.webprov, 0);

    /* Save the above settings */
    NVSettingsSave(&GNV_Setting);
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf("Web provisioning complete.\n");
#endif
    DisplayLCD(LCD_LINE6, "WebProv Done");
    DisplayLCD(LCD_LINE7, "");
    DisplayLCD(LCD_LINE8, "Press RESET");

    while (1)
        {}
}
const char *pdata="0123456789ABCDEFGHIJ";
void App_StartupTCPClinet(void)
{
    ATLIBGS_MSG_ID_E r; uint8_t cid, c;
    
    App_InitModule();
    AtLibGs_DisAssoc();
    while(1)
    {
        r =AtLibGs_EnableRadio(1);                       // enable radio
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bad Mode!");
            MSTimerDelay(2000);
            continue;
        }    
        r =  AtLibGs_DHCPSet(1);     
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "DHCP!");
            MSTimerDelay(2000);
            continue;
        }     
        r = AtLibGs_BData(1);
        if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "Bulk fail!");
            MSTimerDelay(2000);
            continue;
        } 
       r = AtLibGs_Assoc("GuLou", "", 0);
       if (r != ATLIBGS_MSG_ID_OK) {
            DisplayLCD(LCD_LINE6, "AP Failed!");
            MSTimerDelay(2000);
            continue;
        }
       
       r = AtLibGs_TCPClientStart("192.168.3.112", 8010, &cid);
       if (r != ATLIBGS_MSG_ID_OK) {
                DisplayLCD(LCD_LINE7, "No Connect!");
                MSTimerDelay(2000);
                DisplayLCD(LCD_LINE7, "");
                continue;
        }
        if (cid == ATLIBGS_INVALID_CID) {
            DisplayLCD(LCD_LINE7, "No CID!");
            MSTimerDelay(2000);
            DisplayLCD(LCD_LINE7, "");
            continue;
        }
      
      DisplayLCD(LCD_LINE7, "Connected!");
      break;
    }
   
   while(1)
   {
     AtLibGs_BulkDataTransfer(cid, pdata, 20);
     if(App_Read(&c, 1, 0)) 
        AtLibGs_ReceiveDataProcess(c);
     MSTimerDelay(500);
   }
}
#endif

extern ATLIBGS_MSG_ID_E AtLibGs_BulkDataTransferTest(uint8_t cid, uint16_t dataLen);
#define MAX_TX_BUF_SIZE 1400
int LimitedAP_TCP_SereverBulkMode(void)				
{
    ATLIBGS_MSG_ID_E rxMsgId;
    ATLIBGS_TCPConnection connection;
    char  infotext[32], mac[22]; uint8_t rxData, cid;
    unsigned long start;
    
  //  GS_MCU_HWInit();
   // GS_NodeReset();
 
    ConsolePrintf("DW-Wireless Testing\r\n");
    DisplayLCD(LCD_LINE2, "DW Wireless");
    
    App_InitModule();
    /* Try to disassociate if not already associated */
    AtLibGs_DisAssoc(); 
	do
    {
        rxMsgId = AtLibGs_SetEcho(0);
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
   
    do
    {
        rxMsgId = AtLibGs_GetMAC(mac);
     }while(ATLIBGS_MSG_ID_OK != rxMsgId);
    
    do
    {
     AtLibGs_GetInfo( ATLIBGS_ID_INFO_SOFTWARE_VERSION, infotext, 31);
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
    
    do   /* Disable DHCP */
    {
        rxMsgId =  AtLibGs_DHCPSet(0); 
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);   
     /* Set the static IP address */
    do
    {
       rxMsgId = AtLibGs_SetTransmitRate(ATLIBGS_TR1MBPS);
     }while(ATLIBGS_MSG_ID_OK != rxMsgId); 
    
    do
    {
        rxMsgId = AtLibGs_IPSet("192.168.1.1","255.255.255.0", "192.168.1.1"); 
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
    
    DisplayLCD(LCD_LINE3, "Server:");
    DisplayLCD(LCD_LINE4, "192.168.1.1");

#if 1    
    do  /* Enable Radio */
    {
        rxMsgId =  AtLibGs_EnableRadio(1);
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
#endif    
    do  /* Enable Bulk Transfer */
    {
        rxMsgId =  AtLibGs_BData(1);
  
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
    
    do  /* Set Limited AP mode */
    {     
         rxMsgId =  AtLibGs_Mode(ATLIBGS_STATIONMODE_LIMITED_AP);
         
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
       
    do  /* Associate to AP */
    {
        rxMsgId =  AtLibGs_Assoc("DW_Wireless11", 0, 10);
  
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
        
    do   /* Enable DHCP Sever  */
    {
        rxMsgId = AtLibGs_EnableDHCPServer();
        if (rxMsgId != ATLIBGS_MSG_ID_OK) {
  //          DisplayLCD(LCD_LINE6, "Try DHCPSrv!");
            rxMsgId = AtLibGs_DisableDHCPServer();
            if (rxMsgId == ATLIBGS_MSG_ID_OK)
            {
               rxMsgId = AtLibGs_EnableDHCPServer();
               if (rxMsgId != ATLIBGS_MSG_ID_OK)
               {
                 MSTimerDelay(2000);
                 continue;
               }
            }
        }
    } while(ATLIBGS_MSG_ID_OK != rxMsgId);
       
        /* Open 3 TCP Clinet Socktes. */
    do /* Start TCP Client 1*/
    {
        rxMsgId = AtLibGs_TCPServer_Start(8010, &cid);
        //rxMsgId = AtLibGs_TcpClientStart("192.168.3.100","8010");
        
        DisplayLCD(LCD_LINE5, "TCP Port:");
        DisplayLCD(LCD_LINE6, "8010");
        DisplayLCD(LCD_LINE7, "");
        DisplayLCD(LCD_LINE8, "");
  
    }while(ATLIBGS_MSG_ID_OK != rxMsgId);
    
   
   while (1)
   {
      rxMsgId = AtLibGs_WaitForTCPConnection(&connection,  100000);
      if(rxMsgId == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT)
      {
         ConsolePrintf("Server CID: %d\r\n", connection.server_cid);
         ConsolePrintf("Client CID: %d\r\n", connection.cid);
         ConsolePrintf("Client Port: %u\r\n", connection.port);     
         ConsolePrintf("Client IP: %s\r\n", connection.ip);
         
         DisplayLCD(LCD_LINE7, "TCP Client:");
         DisplayLCD(LCD_LINE8, (uint8_t *)connection.ip);
         break;
      }   
   }
   start = MSTimerGet();
   while(1)
   {
      if(App_Read(&rxData, 1, 0)==1)          // process incoming data
      {
        AtLibGs_ReceiveDataProcess(rxData);
      }
   
      if(MSTimerDelta(start) >= 500)           // every 500 ms, send TCP data
      {
         // rxMsgId = AtLibGs_SendTCPData(connection.cid, TxDataBuf, MAX_TX_BUF_SIZE);
         
         rxMsgId = AtLibGs_BulkDataTransferTest(connection.cid, MAX_TX_BUF_SIZE);
         
         if(rxMsgId != ATLIBGS_MSG_ID_OK) 
         {
            ConsolePrintf("Send Fail!");
            MSTimerDelay(2000);
            AtLibGs_Close(cid);
            break;
         } 
         start = MSTimerGet();     
      } 
     
     // MSTimerDelay(500);  
   }
   
   ConsolePrintf("Stopped!");
   while(1)
     ; 
}

/*-------------------------------------------------------------------------*
 * End of File:  App_Startup.c
 *-------------------------------------------------------------------------*/
