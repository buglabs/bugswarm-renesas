/*-------------------------------------------------------------------------*
 * File:  App_Exosit.c
 *-------------------------------------------------------------------------*
 * Description:
 *     This demo reads the temperature and potentiometer and sends the
 *     data to the renesas.exosite.com website using an HTTP connection.
 *     Data is sent every 20 seconds.
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
#include <ATCmdLib/AtCmdLib.h>
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include <system/eeprom.h>
#include <drv/Glyph/lcd.h>
#include <mstimer.h>
#include <system/console.h>
#include "Apps.h"
#include "led.h"
#include "NVSettings.h"
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define EXOSITE_DEMO_UPDATE_INTERVAL            1500 // ms

#define DATA_TO_SEND_TO_SERVER_ALL_LINES \
        "POST /api:v1/stack/alias HTTP/1.1\r\n" \
        "Host: m2.exosite.com\r\n" \
        "X-Exosite-CIK: %s\r\n" \
        "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n" \
        "Content-Length: %d\r\n\r\n"   

#define GET_DATA_ALL_LINES \
        "GET /api:v1/stack/alias?%s HTTP/1.1\r\n" \
        "Host: m2.exosite.com\r\n" \
        "X-Exosite-CIK: %s\r\n" \
        "Accept: application/x-www-form-urlencoded; charset=utf-8\r\n\r\n"

#define DEVICE_ACTIVATE_ALL_LINE \
        "POST /provision/activate HTTP/1.1\r\n" \
        "Host: m2.exosite.com\r\n" \
        "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n" \
        "Accept: text/plain; charset=utf-8\r\n" \
        "Content-Length: %d\r\n\r\n"

/* IP Address of the remote TCP Server */
#define EXOSITE_DEMO_REMOTE_TCP_SRVR_IP     "173.255.209.28" // m2.exosite.com
#define EXOSITE_DEMO_REMOTE_TCP_SRVR_PORT   80

#define CIK_ADDRESS   130
//#define DefinedCIK                          "Input User-Defined-CIK Here!"
#define ExositeAppVersion                   "   v1.00   "
#define CloudAccessLimite                   100
typedef enum {
    EXOSITE_ACTIVATION = 1,
    EXOSITE_WRITE,
    EXOSITE_READ
} Exosite_State;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static int16_t G_adc_int[2] = { 0, 0 };
static char G_temp_int[2] = { 0, 0 };

static char G_activated = 0;
static char UserCIK[41];
static char myCIK[41];
static char G_command[ATLIBGS_TX_CMD_MAX_SIZE];
static char WifiMAC[17];
static Exosite_State es = EXOSITE_ACTIVATION;
extern char WiFiMAC[];

static char Wretry=0;
static char Rretry=0;
extern NVSettings_t GNV_Setting;
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  TemperatureReading
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of a temperature and show it on the LCD display.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void TemperatureReading(void)
{
  char lcd_buffer[20];

  // Temperature sensor reading
  int16_t temp;
  temp = Temperature_Get()>>3;
  // Get the temperature and show it on the LCD
  G_temp_int[0] = (int16_t)temp / 16 - 2;
  G_temp_int[1] = (int16_t)((temp & 0x000F) * 10) / 16;

  /* Display the contents of lcd_buffer onto the debug LCD */
  sprintf((char *)lcd_buffer, "TEMP: %d.%d C", G_temp_int[0], G_temp_int[1]);
  DisplayLCD(LCD_LINE3, (const uint8_t *)lcd_buffer);
}

/*---------------------------------------------------------------------------*
 * Routine:  PotentiometerReading
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of the potentiometer and show it on the LCD display.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void PotentiometerReading(void)
{
  char lcd_buffer[20];

  // Temperature sensor reading
  int32_t percent;
  percent = Potentiometer_Get();
  G_adc_int[0] = (int16_t)(percent / 10);
  G_adc_int[1] = (int16_t)(percent % 10);

  sprintf((char *)lcd_buffer, " POT: %d.%d ", G_adc_int[0], G_adc_int[1]);
  /* Display the contents of lcd_buffer onto the debug LCD */
  DisplayLCD(LCD_LINE4, (const uint8_t *)lcd_buffer);
}

/*---------------------------------------------------------------------------*
 * Routine:  RSSIReading
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of the RSSI level with the WiFi and show it on
 *      the LCD display.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void RSSIReading(void)
{
  int16_t rssi;
  char line[20];
  int rssiFound = 0;

  if (AtLibGs_IsNodeAssociated()) {
    if (AtLibGs_GetRssi() == ATLIBGS_MSG_ID_OK) {
      if (AtLibGs_ParseRssiResponse(&rssi)) {
        sprintf(line, "RSSI: %d", rssi);
        DisplayLCD(LCD_LINE5, (const uint8_t *)line);
        rssiFound = 1;
      }
    }
  }

  if (!rssiFound) {
    DisplayLCD(LCD_LINE5, "RSSI: ----");
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  UpdateReadings
 *---------------------------------------------------------------------------*
 * Description:
 *      Takes a reading of temperature and potentiometer and show
 *      on the LCD.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void UpdateReadings(void)
{
    TemperatureReading();
    PotentiometerReading();
    DisplayLCD(LCD_LINE7, "");
    DisplayLCD(LCD_LINE8, "");
}

/*---------------------------------------------------------------------------*
 * Routine:  WIFI_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initial setting + DHCP and show status on the LCD.
 *
 * Inputs:
 *      void
 * Outputs:
 *      ATLIBGS_MSG_ID_E
 *---------------------------------------------------------------------------*/
int16_t G_Extra2B=1;
ATLIBGS_MSG_ID_E WIFI_init(int16_t showMessage)
{
  ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;

 // Check the link
#ifdef HOST_APP_DEBUG_ENABLE
    ConsolePrintf("Checking link\r\n");
#endif

  AtLibGs_Init();
  // Wait for the banner
  MSTimerDelay(500);

  /* Send command to check */
  do {
    AtLibGs_FlushIncomingMessage();
    DisplayLCD(LCD_LINE8, "Checking...");
    rxMsgId = AtLibGs_Check();
  } while (ATLIBGS_MSG_ID_OK != rxMsgId);
   
  do {   
       rxMsgId = AtLibGs_SetEcho(0);               // disable Echo
  }while (ATLIBGS_MSG_ID_OK != rxMsgId);
  
  do {                                               
       rxMsgId = AtLibGs_Version();                // check the GS version
    }while (ATLIBGS_MSG_ID_OK != rxMsgId);
#if 0  
  if(strstr((const char *)MRBuffer, "2.3."))       // still debug why receive 2 extra bytes: ESC S
  {
    G_Extra2B = 2;
  }
#endif
  
  do{  
       rxMsgId = AtLibGs_EnableRadio(1);                       // enable radio
  }while(rxMsgId != ATLIBGS_MSG_ID_OK);
  
  /* Get MAC Address & Show */
  rxMsgId = AtLibGs_GetMAC(WiFiMAC);    
  if (rxMsgId == ATLIBGS_MSG_ID_OK)
    AtLibGs_ParseGetMacResponse(WifiMAC);
  if (showMessage > 0) {  
    DisplayLCD(LCD_LINE5, "MAC ADDRESS");   
    DisplayLCD(LCD_LINE6, (const uint8_t *)WifiMAC);
    DisplayLCD(LCD_LINE2, (const uint8_t *)ExositeAppVersion);
    MSTimerDelay(2000);
    DisplayLCD(LCD_LINE2, "            ");
  }
  
  do {
    AtLibGs_FlushIncomingMessage();
    DisplayLCD(LCD_LINE8, "Disassociate");
    rxMsgId = AtLibGs_DisAssoc();
  } while (ATLIBGS_MSG_ID_OK != rxMsgId);
    
    // Enable DHCP
  do { 
    DisplayLCD(LCD_LINE8, "DHCP On...");
    rxMsgId = AtLibGs_DHCPSet(1);
  } while (ATLIBGS_MSG_ID_OK != rxMsgId);
 
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
          DisplayLCD(LCD_LINE8, " Setting PSK");
          rxMsgId = AtLibGs_CalcNStorePSK(GNV_Setting.webprov.ssid, GNV_Setting.webprov.password);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId); 
        DisplayLCD(LCD_LINE8, " PSK Set");
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
#ifdef HOST_APP_SEC_WEP
        // Set AT+WAUTH=2 for WEP
        do {
          DisplayLCD(LCD_LINE8, " WEP AUTH " );
          rxMsgId = AtLibGs_SetAuthentictionMode(2);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
        // Set WEP
        do {
          rxMsgId = AtLibGs_SetWEP1(HOST_APP_AP_SEC_WEP);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
          /* Security Configuration */
        do {
          rxMsgId = AtLibGs_SetSecurity(2);        // WEP
          } while (ATLIBGS_MSG_ID_OK != rxMsgId);
#endif
    
#ifdef HOST_APP_SEC_PSK
        /* Store the PSK value. This call takes might take few seconds to return */
        do {
          DisplayLCD(LCD_LINE8, "Setting PSK");
          rxMsgId = AtLibGs_CalcNStorePSK(HOST_APP_AP_SSID, HOST_APP_AP_SEC_PSK);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
#endif

#ifdef HOST_APP_SEC_OPEN
        /* Store the PSK value. This call takes might take few seconds to return */
        do {
          DisplayLCD(LCD_LINE8, "No Security" );
          rxMsgId = AtLibGs_SetAuthentictionMode(1);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
#endif

#ifdef HOST_APP_WPA
        // Set AT+WAUTH=0 for WPA or WPA2
        do {
          DisplayLCD(LCD_LINE8, "   WPA   " );
          rxMsgId = AtLibGs_SetAuthentictionMode(0);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
      
        /* Store the PSK value. This call takes might take few seconds to return */
        do {
          DisplayLCD(LCD_LINE8, "Setting PSK");
          rxMsgId = AtLibGs_CalcNStorePSK(HOST_APP_AP_SSID, HOST_APP_AP_SEC_PSK);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);   
      
        /* Security Configuration */
        do {
          DisplayLCD(LCD_LINE8, "   WPA   ");
          rxMsgId = AtLibGs_SetSecurity(4);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
#endif

#ifdef HOST_APP_WPA2
        // Set AT+WAUTH=0 for WPA or WPA2
        do {
          DisplayLCD(LCD_LINE8, "  WPA2   " );
          rxMsgId = AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_NONE_WPA);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
          
        /* Store the PSK value. This call takes might take few seconds to return */
        do {
          DisplayLCD(LCD_LINE8, "Setting PSK");
          rxMsgId = AtLibGs_CalcNStorePSK(HOST_APP_AP_SSID, HOST_APP_AP_SEC_PSK);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);   

        /* Security Configuration */
        do {
          DisplayLCD(LCD_LINE8, "  Set WPA  ");
          rxMsgId = AtLibGs_SetSecurity(ATLIBGS_SMWPA2PSK);
        } while (ATLIBGS_MSG_ID_OK != rxMsgId);
#endif
      }
      /* Clera MAC Address and show Exosite */
      DisplayLCD(LCD_LINE6, "  EXOSITE  ");
      DisplayLCD(LCD_LINE5, "           ");

/*
  // If need,Set the MAC Address
  do {
    DisplayLCD(LCD_LINE8, "Set MAC...");
    rxMsgId = AtLibGs_MACSet(HOST_APP_GS_NODE_MAC_ID);
  } while (ATLIBGS_MSG_ID_OK != rxMsgId);
*/ 
      
  return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  WIFI_Associate
 *---------------------------------------------------------------------------*
 * Description:
 *      Association and show result on the LCD
 * Inputs:
 *      void
 * Outputs:
 *      ATLIBGS_MSG_ID_E
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E WIFI_Associate(void)
{
  ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
  static ATLIBGS_AUTHMODE_E WEPMode=ATLIBGS_AUTHMODE_OPEN_WEP;

  DisplayLCD(LCD_LINE7, " Connecting ");
 
  /* Associate to a particular AP specified by SSID  */
  if (strlen(GNV_Setting.webprov.ssid) > 0)
  {
    DisplayLCD(LCD_LINE8, (const uint8_t *)GNV_Setting.webprov.ssid);
    rxMsgId = AtLibGs_Assoc(GNV_Setting.webprov.ssid,NULL,HOST_APP_AP_CHANNEL);
  }
  else
  {
    DisplayLCD(LCD_LINE8, HOST_APP_AP_SSID);
    rxMsgId = AtLibGs_Assoc(HOST_APP_AP_SSID, NULL, HOST_APP_AP_CHANNEL);
  }
  if (ATLIBGS_MSG_ID_OK != rxMsgId) {
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
    DisplayLCD(LCD_LINE7, " Connecting..");
    MSTimerDelay(2000);
    DisplayLCD(LCD_LINE7, "");
  } else {
    /* Association success */
    AtLibGs_SetNodeAssociationFlag();
    DisplayLCD(LCD_LINE7, " Connected ");
    MSTimerDelay(2000);
    DisplayLCD(LCD_LINE7, "");
  }

  return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine: Get Device CIK 
 *---------------------------------------------------------------------------*
 * Description:
 *      Read CIK from EEPROM address 0x0400
 * Inputs:
 *      void
 * Outputs:
 *      int16_t  result > 0   ==> success
 *               result <=0   ==> fail
 *---------------------------------------------------------------------------*/
int16_t GetUserCIK(void)
{
  int16_t result = 0;
  int16_t index = 0;
  
  // Hard code CIK method1 :
  //strcpy(UserCIK, DefinedCIK);
  //result = 1;                     //Remeber remove Read CIK from EEPROM
  
  // Hard code CIK method2 :
  //EEPROM_Write(CIK_ADDRESS, "f2345ced572caeefa724442f3b07ecde2703432d",40);
    
  // Read CIK from EEPROM
  result =  EEPROM_Seq_Read(CIK_ADDRESS,(uint8_t*)UserCIK, 40);

  if( result > 0) {
    // verify CIK 
    while(index < 40) {
      if (((UserCIK[index] >= 0x30) && (UserCIK[index] <= 0x39)) ||
          ((UserCIK[index] >= 0x61) && (UserCIK[index] <= 0x7A)) ) {
        index++;
      } else {
        index = 40;
        result = 0;
      }
    }
  }
  
  return result; 
}

/*---------------------------------------------------------------------------*
 * Routine: Store Device CIK 
 *---------------------------------------------------------------------------*
 * Description:
 *      Write CIK at EEPROM address 0x0400
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void StoreCIK(void)
{
  EEPROM_Write(CIK_ADDRESS,(uint8_t*)myCIK,40);
}

/*---------------------------------------------------------------------------*
 * Routine: Set Device CIK 
 *---------------------------------------------------------------------------*
 * Description:
 *      Set CIK at EEPROM address 0x0400
 * Inputs:
 *      char *pcik 
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
/*
void SetCIK(char *pcik)
{
  EEPROM_WriteStr(5,pcik);
} */
/*---------------------------------------------------------------------------*
 * Routine: Activate device 
 *---------------------------------------------------------------------------*
 * Description:
 *      Request a CIK from Exosite Cloud to activate device
 * Inputs:
 *      void
 * Outputs:
 *      int16_t  result > 0   ==> success
 *               result <=0   ==> fail
 *---------------------------------------------------------------------------*/
void DeviceActivation(void)
{
  char content[256];
  ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
  //Get MAC address
  rxMsgId = AtLibGs_GetMAC(WiFiMAC);

  if (rxMsgId == ATLIBGS_MSG_ID_OK)
    AtLibGs_ParseGetMacResponse(WifiMAC);

  if (strlen(WifiMAC) >0)
    sprintf(content, "vendor=%s&model=%s&sn=%s&osn=Micrium-Ex3&osv=3.01.2",
            "renesas", "rl78g14",WifiMAC);

  sprintf(G_command, DEVICE_ACTIVATE_ALL_LINE, strlen(content));
  strcat(G_command, content);
}

void Exosite_Write(char *pContent)
{
  sprintf(G_command, DATA_TO_SEND_TO_SERVER_ALL_LINES,myCIK, strlen(pContent));
  strcat(G_command, pContent);
}

void Exosite_Read(char *pContent)
{
  sprintf(G_command, GET_DATA_ALL_LINES, pContent,myCIK);
}

/*---------------------------------------------------------------------------*
 * Routine:  ParseGet
 *---------------------------------------------------------------------------*
 * Description:
 *      Below is a GET receive data
 *      the value next '=' is we want 
 *========================================
 *HTTP/1.1 200 OK
 *Date: Fri, 09 Mar 2012 xx:xx:xx GMT
 *Server: misultin/0.8.1-exosite
 *Connection: Keep-Alive
 *Content-Length: 10
 *
 *led_ctrl=1
 *========================================
 * Inputs:
 *      char *pData
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ParseGet(char *pData)
{
  char *pGet;

  pGet = strchr(pData,'='); 
  if (pGet) {
    pGet++;
    
    if (*pGet == '1') {
      /* ON LED */
     // P5  &= 0x03U;
     //  P6  &= 0xF3U;
      led_all_on();
    } else { 
      /* OFF LED */                
    //  P5  |= 0x3CU;
    //  P6  |= 0x0CU;
      led_all_off();
    }
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  ParseCIK
 *---------------------------------------------------------------------------*
 * Description:
 *      Below is a CIK receive data
 *      the Last line 40 chars is we want 
 *========================================
 *HTTP/1.1 200 OK
 *Date: Fri, 09 Mar 2012 08:55:47 GMT
 *Server: misultin/0.8.1-exosite
 *Connection: Keep-Alive
 *Content-Length: 40
 *Content-Type: text/plain; charset=utf-8
 *
 *c0b0d02aa0228fba01f9829081987b52d1a11029
 *========================================
 * Inputs:
 *      char *pData
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ParseCIK(char *pData)
{
  char *pCIK;
  
  pCIK = strstr(pData,"\r\n\r\n");
  
  if(pCIK) {
    pCIK+=4;
    
    for (uint8_t i =0; i<40; i++)
      myCIK[i]=*(pCIK+i);
                            
    myCIK[40]='\0';
    StoreCIK();
    MSTimerDelay(200);
    // Read CIK back  
    if (GetUserCIK() > 0) {
      es = EXOSITE_WRITE;
      G_activated = 1;
    }
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  ParseReceiveData
 *---------------------------------------------------------------------------*
 * Description:
 *      Parse "HTTP/1.1 XXX"  
 * Inputs:
 *      char *pData
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void ParseReceiveData(char* pData)
{
  static uint8_t UnauthCount = 0;
  
  if (strcmp(pData,"200") == 0) {
    DisplayLCD(LCD_LINE7, " Connected! ");
    pData+= 8;
    UnauthCount = 0;
    Rretry = 0;
    if (!G_activated)
      ParseCIK(pData);
    else
      ParseGet(pData);
  } else if (strcmp(pData, "204") == 0) {
    // Exosite Write OK
    if (es == EXOSITE_WRITE)
    {
      DisplayLCD(LCD_LINE7, " Connected! ");
      DisplayLCD(LCD_LINE8, " Write OK! ");
      UnauthCount=0;
      Wretry = 0;
    }
    else
    {
      Rretry++;
      if (Rretry > 20)
        DisplayLCD(LCD_LINE8, "ledctrl lost");
      else if (Rretry % 5 == 4)
        DisplayLCD(LCD_LINE8, " Read Retry ");      
    }
  } else if (strcmp(pData, "401") == 0) {
    /* Wrong CIK */
    UnauthCount++;

    if (UnauthCount > 3) {
      es = EXOSITE_ACTIVATION;
      G_activated = 0;
      UnauthCount=0;
    } 
  } else if (strcmp(pData, "409") == 0) {
      UnauthCount=5;
  } else { 
      DisplayLCD(LCD_LINE7, "             ");
      switch(es)
      {
      case EXOSITE_ACTIVATION: 
           DisplayLCD(LCD_LINE8, "  Activation ");
           break;
      case EXOSITE_WRITE:
           Wretry++;
           if (Wretry > 3)
             DisplayLCD(LCD_LINE8, " Write fail ");
           else
             DisplayLCD(LCD_LINE8, " Write Retry ");
           break;     
      case EXOSITE_READ:
           Rretry++;
           if (Rretry > 20 )
             DisplayLCD(LCD_LINE8, " Read fail ");
           else if (Rretry % 5 == 4)
             DisplayLCD(LCD_LINE8, " Read Retry");
           break;
      }
  }
  strcpy(pData,"100");
}

/*---------------------------------------------------------------------------*
 * Routine:  App_Exosite
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of temperature and potentiometer and send to the
 *      Exosite Cloud using a TCP connection.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_Exosite(void)
{
  uint8_t cid = 0;
  uint8_t ping = 0;
  uint32_t start;
  uint32_t end;
  uint32_t postTime;
  uint32_t count;
  uint8_t RWcount = 10;
  uint8_t cloundCound =CloudAccessLimite;
  char *pRx1,*pRx2;
#ifdef HOST_APP_TCP_DEBUG 
  static uint16_t parsererror = 0;
  static uint8_t updateError = 1;
#endif 
//SetCIK(DefinedCIK);

  ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
  static char content[256];
  // Give the unit a little time to start up
  // (300 ms for GS1011 and 1000 ms for GS1500)
  MSTimerDelay(1000);
  
  NVSettingsLoad(&GNV_Setting);

  rxMsgId = WIFI_init(1);  // Show MAC address and Version
  //if (rxMsgId != ATLIBGS_MSG_ID_OK) 
  WIFI_Associate();
   
  if (GetUserCIK() > 0){
    strcpy(myCIK,UserCIK);
    es = EXOSITE_WRITE;
    G_activated = 1;
    RWcount = 0;
  } else {
    DisplayLCD(LCD_LINE8, " Activating");
  }
  DisplayLCD(LCD_LINE2, (const uint8_t *)WifiMAC);
  while (1) {
    // Do we need to connect to the AP?
    if (!AtLibGs_IsNodeAssociated())
      WIFI_Associate();

    // Send data
    if (AtLibGs_IsNodeAssociated()) {        // Send data if
      RSSIReading();
      DisplayLCD(LCD_LINE8, " Sending...");

      rxMsgId = AtLibGs_TCPClientStart(EXOSITE_DEMO_REMOTE_TCP_SRVR_IP,
               EXOSITE_DEMO_REMOTE_TCP_SRVR_PORT, &cid);
      if (ATLIBGS_MSG_ID_OK != rxMsgId) {
        /* TCP connection error */
#ifdef HOST_APP_DEBUG_ENABLE
        ConsolePrintf("\n TCP Connection ERROR !\n");
#endif
        AtLibGs_FlushIncomingMessage();
        AtLibGs_CloseAll();
        DisplayLCD(LCD_LINE7, "         ");
        AtLibGs_ClearNodeAssociationFlag();
#ifdef HOST_APP_TCP_DEBUG        
        updateError = 1;
        parsererror++;
#endif        
        MSTimerDelay(3000);
        rxMsgId = WIFI_init(0);
        MSTimerDelay(3000);
        continue;
      }

      /* Extract the CID from the response */
      MSTimerDelay(300);
      //cid = AtLib_ParseTcpClientCid();

      if (ATLIBGS_INVALID_CID == cid) {
        /* TCP connection response parsing error */
#ifdef HOST_APP_DEBUG_ENABLE
        ConsolePrintf("\nTCP connection response parsing error!\n");
#endif
        // Eat the extra data and start over
        AtLibGs_FlushIncomingMessage();
      //  cid = AtLib_ParseTcpClientCid();
        
        if (ATLIBGS_INVALID_CID == cid) {
#ifdef HOST_APP_TCP_DEBUG
          if (!updateError) {
            updateError = 1;
            parsererror++;
          }
#endif
          AtLibGs_CloseAll();
          DisplayLCD(LCD_LINE7, "          ");
        }
        
        continue;
      }
      while (cloundCound-- >0){
        
        start = MSTimerGet();
        UpdateReadings();
       
        App_PrepareIncomingData();
        postTime = MSTimerGet();

        while (MSTimerDelta(postTime) < 5000)     // Receive while loop, wait up to 5 seconds for a response
        { 
          switch(es)
          {
            case EXOSITE_ACTIVATION:
                 DeviceActivation();
                 break;   
            case EXOSITE_WRITE:
    #ifdef HOST_APP_TCP_DEBUG
                 if (updateError) {
                   sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d&ect=%d\r\n",
                           G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
                           ping,parsererror);
                   updateError = 0;
                 } else {
                   sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d\r\n",
                           G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
                           ping);
                 }
    #else
                 sprintf(content, "temp=%d.%d&adc1=%d.%d&ping=%d\r\n",
                         G_temp_int[0],G_temp_int[1], G_adc_int[0], G_adc_int[1],
                         ping);
    #endif
                 ping++;
                 if (ping >= 100)
                   ping = 0;
                 Exosite_Write(content);
                 break;
            case EXOSITE_READ:
                 sprintf(content, "led_ctrl");
                 Exosite_Read(content);
                 break;
          }
          /* Use bulk Transfer */
          //AtLib_BulkDataTransfer(cid, (uint8_t *)G_command, strlen(G_command));

          /* Use normal Transfer */
          AtLibGs_SendTCPData(cid, (uint8_t *)G_command, strlen(G_command));

          rxMsgId = AtLibGs_ReceiveDataHandle(3000);

  #ifdef HOST_APP_DEBUG_ENABLE
          if (G_receivedCount > 0) {
            ConsolePrintf((const char *)G_received + 2);
            ConsolePrintf("\r\n");
          }
  #endif
          // Rx timeout 
  //        if( ATLIBGS_MSG_ID_RESPONSE_TIMEOUT == rxMsgId)
  //        {
  //         cloundCound = 0;
  //          break;
  //        }
  //        else
          DisplayLCD(LCD_LINE7, " Connected! ");
          
          /* Check Receive data */ 
          if (G_receivedCount > 17) {
            pRx1 = strstr((const char *)G_received,"HTTP/1.1");
            pRx2 = strstr(pRx1+6,"TTP/1.1");

            G_received[8+G_Extra2B] = '\0';                       //  G_received[2 + 8] = '\0';  
            G_received[12+G_Extra2B] = '\0';                      //  G_received[2 + 12] = '\0';   

              /* HTTP Result */
            if (pRx1)  
            {
              ParseReceiveData(pRx1+9);
              
              if (pRx2){
                pRx2+=8;
                *(pRx2+3) ='\0';
                ParseReceiveData(pRx2);
              }    
            }
          }

          if (G_activated) {
            RWcount++;
            if ( RWcount > 2)
              RWcount=0;
                  
            if ( RWcount == 0)
              es = EXOSITE_WRITE;   
            else
              es = EXOSITE_READ;
           }
          break;
      } // Receive while loop end    
      
     /* Wait a little bit after server disconnecting client before connecting again*/
      while (1){
         end = MSTimerGet();
         if ((start + EXOSITE_DEMO_UPDATE_INTERVAL - end)
             < EXOSITE_DEMO_UPDATE_INTERVAL) {
           count = start + EXOSITE_DEMO_UPDATE_INTERVAL - end;
           if (count > 250){
             count = 250;
             MSTimerDelay(count);
             UpdateReadings();
            }
          } else {
             break;
          }
       }     
     }
     cloundCound =CloudAccessLimite;
     AtLibGs_Close(cid);
     DisplayLCD(LCD_LINE5, "           ");
     DisplayLCD(LCD_LINE4, "           ");
     DisplayLCD(LCD_LINE3, "           ");      
     DisplayLCD(LCD_LINE7, "           ");
    }       // Send data if END
  }
}
