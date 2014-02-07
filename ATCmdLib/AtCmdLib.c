/*-------------------------------------------------------------------------*
 * File:  AtCmdLib.c
 *-------------------------------------------------------------------------*
 * Description:
 *      The GainSpan AT Command Library (AtCmdLib) provides the functions
 *      that send AT commands to a GainSpan node and looks for a response.
 *      Parse commands are provided to interpret the response data.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> /* for sprintf(), strstr(), strlen() , strtok() and strcpy()  */
#include <stdlib.h>
#include <ctype.h>
#include "HostApp.h"
#include "AtCmdLib.h"
//#include <system/console.h>
#include "mstimer.h"
#include <system/platform.h>
int atoi ( const char * str );
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define ATLIB_RESPONSE_HANDLE_TIMEOUT   100000 /* ms */

#ifndef ATLIBGS_TX_CMD_MAX_SIZE
#error "ATLIBGS_TX_CMD_MAX_SIZE must be defined in platform.h"
#endif
#ifndef ATLIBGS_RX_CMD_MAX_SIZE
#error "ATLIBGS_RX_CMD_MAX_SIZE must be defined in platform.h"
#endif

const char  str_URL[32] = "/gainspan/profile/mcu";
const char  str_rootTag[16]= "renesas_tla"; // "nxp_tla"; //"renesas_tla";

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* Receive buffer to save async and response message from S2w App Node */
char MRBuffer[ATLIBGS_RX_CMD_MAX_SIZE];
uint16_t MRBufferIndex = 0;

/* Flag to indicate whether S2w Node is currently associated or not */
static uint8_t nodeAssociationFlag = false;
static uint8_t nodeResetFlag = false; /* Flag to indicate whether S2w Node has rebooted after initialisation  */

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
void AtLibGs_FlushRxBuffer(void);

/*---------------------------<AT command list >--------------------------------------------------------------------------
 _________________________________________________________________________________________________________________________
 AT Command                                                                   Description and AT command library API Name
 _________________________________________________________________________________________________________________________

 ATE<0|1>                                                                             Disable/enable echo
 API Name: AtLibGs_SetEcho

 AT&W<0|1>                                                                            Save Settings to profile 0/1
 API Name: AtLibGs_SaveProfile

 ATZ<0|1>                                                                             Load Settings from profile 0/1
 API Name: AtLibGs_LoadProfile

 AT+EXTPA=<1/0>                                                                       Enable/disable external PA.
 API Name: AtLibGs_EnableExternalPA

 AT+WSYNCINTRL=<interval>                                                             configure the sync loss interval in TBTT interval
 API Name: AtLibGs_SyncLossInterval

 AT+PSPOLLINTRL=<interval>                                                            configure the Ps poll interval in Seconds
 API Name: AtLibGs_PSPollInterval

 AT+BDATA=<0/1>                                                                       Bulk data reception enable/disable
 API Name: AtLibGs_BData

 AT&F                                                                                 Reset to factory defaults
 API Name:AtLibGs_ResetFactoryDefaults

 AT+WM=<0|1>                                                                          Set mode to Infrastructure (0) or ad-hoc (1)
 API Name: AtLibGs_Mode

 AT+WA=<SSID>[[,<BSSID>][,<Ch>]]                                                      Associate to network or form ad-hoc network
 API Name: AtLibGs_Assoc

 AT+WD or ATH                                                                         Disassociate from current network
 API Name:AtLibGs_DisAssoc

 AT+WRSSI=?                                                                           Query the current RSSI value
 API Name: AtLibGs_GetRssi

 AT+WWPA=passphrase                                                                   Set WPA passphrase (8 - 63 chars)
 API Name: AtLibGs_SetPassPhrase

 AT+WRETRY=n                                                                          Set the wireless retry count
 API Name: AtLibGs_SetWRetryCount

 AT+WRXACTIVE=<0|1>                                                                   Enable/disable the radio
 API Name: AtLibGs_EnableRadio

 AT+WRXPS=<0|1>                                                                       Enable/disable 802.11 power save
 API Name: AtLibGs_EnablePwSave

 AT+WP=<power>                                                                        Set the transmit power
 API Name: AtLibGs_SetTxPower

 AT+NMAC=<?>/<mac>                                                                    Get/Set MAC address and store in flash
 API Name: AtLibGs_GetMAC / AtLibGs_MACSet

 AT+NDHCP=<0|1>                                                                       Disable/Enable DHCP
 API Name: AtLibGs_DHCPSet

 AT+NSET=<IP>,<NetMask>,<Gateway>                                                     Configure network address
 API Name: AtLibGs_IPSet

 AT+NCTCP=<IP>,<Port>                                                                 Open TCP client
 API Name: AtLibGs_TCPClientStart

 AT+NCUDP=<IP>,<RemotePort>,[<LocalPort>                                              Open UDP client
 API Name: AtLibGs_UDPClientStart

 AT+NSTCP=<Port>                                                                      Open TCP server on Port
 API Name: AtLibGs_TCPServer_Start

 AT+NSUDP=<Port>                                                                      Open UDP server on Port
 API Name: AtLibGs_UDPServer_Start

 AT+NCLOSE=cid                                                                        Close the specified connection
 API Name: AtLibGs_Close

 AT+NCLOSEALL                                                                         Close all connections
 API Name: AtLibGs_CloseAll

 AT+WAUTO=<Mode>,<SSID>,<bssid>,[Channel]                                             Configure auto connect wireless settings
 API Name: AtLibGs_StoreWAutoConn

 AT+NAUTO=<Type>,<Protocol>,<DestIP>,<DestPort>                                       Configure auto connection
 Client(0)/server(1), protocol UDP(0)/TCP(1),and host.
 API Name: AtLibGs_StoreNAutoConn

 ATC<0|1>                                                                             Disable/enable auto connection
 API Name: AtLibGs_EnableAutoConnect

 +++                                                                                  Switch to Command mode while in auto-connect mode
 API Name: AtLibGs_SwitchFromAutoToCmd

 ATSparam=value                                                                       Set configuration parameters
 Network connection timeout (0) / Auto associate timeout (1)
 TCP connection timeout (2) / Association retry count (3)
 Nagle algorithm wait time (4)
 API Name: AtLibGs_StoreATS

 AT+PSDPSLEEP                                                                         Enable deep sleep
 API Name: AtLibGs_EnableDeepSleep

 AT+PSSTBY=<n>[,<delay time>,<alarm1-pol>,<alarm2-pol>]                               Standby request for n milliseconds
 API Name: AtLibGs_GotoSTNDBy

 AT+STORENWCONN                                                                       Store the nw context
 API Name: AtLibGs_StoreNwConn

 AT+RESTORENWCONN                                                                     Restore the nw context
 API Name: AtLibGs_ReStoreNwConn

 AT+FWUP=<SrvIp>,<SrvPort>,<SrcPort>                                                  Start FW Upgrade
 API Name: AtLibGs_FWUpgrade

 AT+WPAPSK=<SSID>,<PassPhrase>                                                        Calculate and store the PSK
 API Name: AtLibGs_CalcNStorePSK

 AT+NSTAT=?                                                                           Display current network context
 API Name: AtLibGs_GetNetworkStatus

 AT+VER=?                                                                             Get the Version Info
 API Name: AtLibGs_Version

 AT+DNSLOOKUP=<URL>,[<RETRY>,<TIMEOUT-S>]                                             Get the ip from host name
 API Name: AtLibGs_DNSLookup

 AT+DNSSET=<DNS1-IP>,[<DNS2-IP>]                                                      Set static DNS IP addresses
 API Name: AtLibGs_SetDNSServerIP

 AT+MCSTSET=<0/1>                                                                     enable/disable the multicast recv
 API Name: AtLibGs_MultiCast

 AT+BCHKSTRT=<Measure interval>                                                       Start the batt check
 API Name: AtLibGs_BatteryChkStart


 AT+BCHKSTOP                                                                          Stop the batt check
 API Name: AtLibGs_BatteryChkStop

 AT+ERRCOUNT=?                                                                        Get the error counts
 API Name: AtLibGs_GetErrCount

 AT+SETTIME=<dd/mm/yyyy>,<HH:MM:SS>                                                   Set the system time
 API Name: AtLibGs_SetTime

 AT+WWPS=<1/2>,<wps pin>                                                              Associate to an AP using WPS.
 1 - Push Button mathod.
 2 - PIN mathod. Provide <wps pin> only in case of PIN mathod
 API Name: AtLibGs_StartWPSPUSH / AtLibGs_StartWPSPIN


 AT&Y<0|1>                                                                            Set default power on profile to profile 0/1
 API Name: Not Available

 AT&V                                                                                 Output current configuration
 API Name: Not Available

 AT+WS[=<SSID>[,<BSSID>][,<Ch>][,<ScanTime>]]                                         Perform wireless scan
 API Name: Not Available

 AT+WRATE=?                                                                           Query the current WiFi rate used
 API Name: Not Available

 AT+WWEPn=<key>                                                                       Set WEP key (10 or 26 hex digits) with index n (1-4)
 API Name: Not Available

 AT+WAUTH=<authmode>                                                                  Set authmode (1->open,2->shared)
 API Name: Not Available

 AT+WSTATUS                                                                           Display current Wireless Status
 API Name: Not Available

 AT+NMAC2=<?>/<mac>                                                                   Get/Set MAC address and store in RTC
 API Name: Not Available

 AT+SETSOCKOPT=<cid>,<type>,<parameter>,<value>,<length                               Set options of a socket specified by cid
 API Name: Not Available

 ATA                                                                                  Initiate AutoConnect
 API Name: Not available

 ATA2                                                                                 Initiate AutoConnect-tcp/udp level
 API Name: Not available

 ATO                                                                                  Return to Auto Data mode
 API Name: Not available

 ATI<n>                                                                               Display identification number n's text
 API Name: AtLibGs_GetInfo

 AT+WPSK=<PSK>                                                                        Store the PSK
 API Name: Not available

 AT+CID=?                                                                             Display The CID info
 API Name: Not available

 AT+BCHK=<?>/<Measure interval>                                                       Get/Set batt check param
 API Name: Not available

 AT+BATTVALGET                                                                        Get the latest battery value stored in RTC
 API Name: Not available


 AT+BATTLVLSET=<Warning Level>,<warning Freq>,<Standby Level>                         Set batt warning level, frequency of reporting warning
 and batt standby levl
 API Name: Not available

 AT+PING=<Ip>,<Trails>,<Interval>,<Len>,<TOS>,<TTL>,<PAYLAOD(16 Bytes)>               Starts Ping
 API Name: Not available

 AT+TRACEROUTE=<Ip>,<Interval>,<MaxHops>,<MinHops>,<TOS>                              Starts Trace route
 API Name: Not available


 AT+GETTIME=?                                                                         Get the system time in Milli-seconds since Epoch(1970)
 API Name: Not available

 AT+DGPIO=<GPIO_PIN>,<1-SET/0-RESET>                                                  Set/reset a gpio pin
 API Name: Not available

 AT+TCERTADD=<name>,<format>,<size>,<location>\n\r<ESC>W<data of size above>          Provisions a certificate.
 format-binary/ascii(0/1),location-FLASH/RAM.
 Follow the escape sequence to send data.
 API Name: Not available

 AT+TCERTDEL=<name>                                                                   Delete a certificate
 API Name: Not available


 AT+WEAPCONF=<outer authtype>,<inner authtype>,<user name>,<password>                 Configure auth type,user name and password for EAP
 API Name: Not available

 AT+WEAP=<type>,<format>,<size>,<location>\n\r<ESC>W<data of size above>              Provision certificate for EAP TLS.
 Type-CA/CLIENT/PUB_KEY(0/1/2),
 format-binary/ascii(0/1),location- flash/RAM(0/1).
 Follow the escape sequence to send data.
 API Name: Not available

 AT+SSLOPEN=<cid>,<name>                                                              Opens a ssl connection. name-Name of certificate to use
 API Name: Not available

 AT+SSLCLOSE=<cid>                                                                    Close a SSL connection
 API Name: Not available

 AT+HTTPOPEN=<hostName/ip addr>,[<port>,<secured/non secured>,<certificate name>]     Opens a http/https connection
 API Name: Not available

 AT+HTTPCLOSE=<cid>                                                                   Closes a http connection
 API Name: Not available
 AT+HTTPSEND=<cid>,<Method>,<TimeOut>,<Page>[,<Size>]\n\r<ESC>H<data of size above>
 Send a Get or POST request.Method- GET/HEAD/POST(1/2/3)
 Follow the escape sequence to send data.
 API Name: Not available

 AT+HTTPCONF=<Param>,<Value>                                                          Configures http parameters.
 API Name: Not available

 AT+WEBPROV=<user name>,<passwd>,<ip addr><subnet mask> <gateway>                      start web server. username passwd are used for authentication
 The server is atarted with the given ip addr, subnetmask
 and gateway
 API Name: Not available

 AT+WEBPROV=<user name>,<passwd>                                                       start web server. username passwd are used for authentication
 API Name: Not available

 AT+WEBLOGOADD=<size>                                                                  add webserver logo of size <size>. After issuing
 the command, send <esc> followed by l/L   and
 send the content of the logo file
 API Name: Not available

 AT+NRAW=<0/1/2>                                                                      Enable Raw Packet transmission.
 API Name: Not available

 ATV<0|1>                                                                             Disable/enable verbose responses
 API Name: Not Available
 _________________________________________________________________________________________________________________________*/

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Check
 *---------------------------------------------------------------------------*
 * Description:
 *      Send command:
 *          AT
 *      and wait for response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Check(void)
{
    return AtLibGs_CommandSendString("\r\nAT\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEcho
 *---------------------------------------------------------------------------*
 * Description:
 *      Send command to turn on or off the character echo:
 *          ATE<0|1>
 *      and wait for response.
 * Inputs:
 *      uint8_t mode -- 1=to turn on echo, 0=turn off echo
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEcho(uint8_t mode)
{
    char cmd[10];

    sprintf(cmd, "ATE" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the network MAC address for this module in flash.
 *      Sends the command:
 *          AT+NMAC=<mac address>
 *      and waits for a response.
 *      <mac address> is the format "00:11:22:33:44:55"
 * Inputs:
 *      uint8_t mode -- 1=to turn on echo, 0=turn off echo
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetMAC(char *pAddr)
{
    char cmd[30];

    sprintf(cmd, "AT+NMAC=%s\r\n", pAddr);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetMAC2
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the network MAC2 address for this module in non-volatile RAM.
 *      Sends the command:
 *          AT+NMAC=<mac address>
 *      and waits for a response.
 *      <mac address> is the format "00:11:22:33:44:55"
 * Inputs:
 *      uint8_t mode -- 1=to turn on echo, 0=turn off echo
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetMAC2(char *pAddr)
{
    char cmd[30];

    sprintf(cmd, "AT+NMAC2=%s\r\n", pAddr);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the network MAC address for this module.
 *      Sends the command:
 *          AT+NMAC=?
 *      and waits for a response.
 * Inputs:
 *      char *mac -- returned mac found (if any)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetMAC(char *mac)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1];

    rxMsgId = AtLibGs_CommandSendString("AT+NMAC=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1))
        strcpy(mac, lines[0]);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetMAC2
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the network MAC address for this module.
 *      Sends the command:
 *          AT+NMAC=?
 *      and waits for a response.
 * Inputs:
 *      char *mac -- returned mac found (if any)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetMAC2(char *mac)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1];

    rxMsgId = AtLibGs_CommandSendString("AT+NMAC2=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1))
        strcpy(mac, lines[0]);

    return rxMsgId;
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WEP1
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the WEP key for association
 *      and waits for a response.
 * Inputs:
 *      int8_t *pWEP -- Pass phrase (10-26 characters)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- response type
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWEP1( int8_t *pWEP)
{
    ATLIBGS_MSG_ID_E rxMsgId;  char cmd[40];

    /* Construct the AT command */
    sprintf(cmd, "AT+WWEP1=%s\r\n", pWEP);

    /* Send command to S2w App node */
    rxMsgId = AtLibGs_CommandSendString(cmd);

    return rxMsgId;
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CalcNStorePSK
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the network MAC address for this module.
 *      Sends the command:
 *          AT+WPAPSK=<SSID>,<PassPhrase>
 *      and waits for a response.
 * Inputs:
 *      char *pSsid -- SSID (1 to 32 characters)
 *      char *pPsk -- Pass phrase (8 to 63 characters)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CalcNStorePSK(char *pSsid, char *pPsk)
{
    char cmd[50];

    sprintf(cmd, "AT+WPAPSK=%s,%s\r\n", pSsid, pPsk);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WlanConnStat
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the network network status for this module.
 *      Sends the command:
 *          AT+NSTAT=?
 *      and waits for a response.
 *      Use the routine AtLibGs_ParseNodeIpAddress() or
 *      AtLibGs_ParseRssiResponse() to parse the returned data for RSSI or
 *      AtLibGs_ParseWlanConnStat() to parse for the wireless connection status.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WlanConnStat(void)
{
    return AtLibGs_CommandSendString("AT+NSTAT=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DHCPSet
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable or disable DHCP client.
 *      Sends the command:
 *          AT+NDHCP=<0|1>
 *      and waits for a response.
 * Inputs:
 *      uint8_t mode -- 0=disable DHCP, 1=enable DHCP
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DHCPSet(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+NDHCP=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Assoc
 *---------------------------------------------------------------------------*
 * Description:
 *      Associate to network or form ad-hoc network
 *      Sends the command:
 *          AT+WA=<SSID>[[,<BSSID>][,<Ch>]]
 *      and waits for a response.
 * Inputs:
 *      char *pSsid -- SSID to connect to (1 to 32 characters)
 *      char *pBssid -- Ad-hoc network id, or 0 for none
 *      char channel -- Channel of network, 0 for any
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Assoc(char *pSsid, char *pBssid, uint8_t channel)
{
    char cmd[100];

    if (channel) {
        sprintf(cmd, "AT+WA=%s,%s," _F8_ "\r\n", pSsid, (pBssid) ? pBssid : "",
                channel);
    } else {
        sprintf(cmd, "AT+WA=%s\r\n", pSsid);
    }

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TCPClientStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Open TCP client connection.
 *      Sends the command:
 *          AT+NCTCP=<IP>,<Port>
 *      and waits for a response.
 *      <IP> is in the format "12:34:56:78"
 *      <Port> is the number of the port connection.
 * Inputs:
 *      char *pRemoteTcpSrvIp -- IP address string in format 12:34:56:78
 *      char *pRemoteTcpSrvPort -- Port string
 *      uint8_t *cid -- CID of client connection opened
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TCPClientStart(
        char *pRemoteTcpSrvIp,
        int16_t pRemoteTcpSrvPort,
        uint8_t *cid)
{
    char cmd[80];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *result;

    sprintf(cmd, "AT+NCTCP=%s," _F16_ "\r\n", pRemoteTcpSrvIp,
            pRemoteTcpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((result = strstr(MRBuffer, "CONNECT")) != NULL) {
            /* Succesfull connection done for TCP client */
            *cid = atoi(result + 8);
        } else {
            /* Not able to extract the CID */
            *cid = ATLIBGS_INVALID_CID;
            rxMsgId = ATLIBGS_MSG_ID_ERROR;
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UDPClientStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Open UDP client
 *      Sends the command:
 *          AT+NCUDP=<IP>,<RemotePort>,[<LocalPort>]
 *      and waits for a response.
 *      <IP> is in the format "12:34:56:78"
 *      <RemotePort> is the number of the port connection.
 *      <LocalPort> is the port on this machine.
 *      Use AtLibGs_ParseUDPClientCid() afterward to parse the response.
 * Inputs:
 *      char *pRemoteUdpSrvIp -- IP address string in format 12:34:56:78
 *      char *pRemoteUdpSrvPort -- Remote connection port string
 *      char *pUdpLocalPort -- Local port string
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UDPClientStart(
        char *pRemoteUdpSrvIp,
        char *pRemoteUdpSrvPort,
        char *pUdpLocalPort)
{
    char cmd[80];

    sprintf(cmd, "AT+NCUDP=%s,%s,%s\r\n", pRemoteUdpSrvIp, pRemoteUdpSrvPort,
            pUdpLocalPort);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CloseAll
 *---------------------------------------------------------------------------*
 * Description:
 *      Close all open connections.
 *      Sends the command:
 *          AT+NCLOSEALL
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CloseAll(void)
{
    return AtLibGs_CommandSendString("AT+NCLOSEALL\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryChkStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the batt check.
 *      Sends the command:
 *          AT+BCHKSTRT=<Measure interval>
 *      and waits for a response.
 * Inputs:
 *      uint16_t interval -- Interval of 1..100
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStart(uint16_t interval)
{
    char cmd[20];

    sprintf(cmd, "AT+BCHKSTRT=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GotoSTNDBy
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the batt check.
 *      Sends the command:
 *          AT+PSSTBY=x[,<DELAY TIME>,<ALARM1 POL>,<ALARM2 POL>]
 *      and waits for a response.
 * Inputs:
 *      uint32_t *nsec -- Standby time in milliseconds
 *      uint32_t delay -- Delay time in milliseconds before going into
 *                          standby.
 *      uint8_t alarm1_Pol -- polarity of pin 31 that will trigger an alarm
 *      uint8_t alarm2_Pol -- polarity of pin 36 that will trigger an alarm
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GotoSTNDBy(
        char *nsec,
        uint32_t delay,
        uint8_t alarm1_Pol,
        uint8_t alarm2_Pol)
{
    char cmd[100];

    sprintf(cmd, "AT+PSSTBY=%s," _F32_ "," _F8_ "," _F8_ "\r\n", nsec, delay,
            alarm1_Pol, alarm2_Pol);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetPassPhrase
 *---------------------------------------------------------------------------*
 * Description:
 *      Set WPA passphrase (8 - 63 chars).
 *      Sends the command:
 *          AT+WWPA=<passphrase>
 *      and waits for a response.
 * Inputs:
 *      char *pPhrase -- Passphrase string (8 - 63 characters)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetPassPhrase(char *pPhrase)
{
    char cmd[50];

    sprintf(cmd, "AT+WWPA=%s\r\n", pPhrase);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetPassPhrase
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable deep sleep.
 *      Sends the command:
 *          AT+PSDPSLEEP
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDeepSleep(void)
{
    return AtLibGs_CommandSendString("AT+PSDPSLEEP\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreNwConn
 *---------------------------------------------------------------------------*
 * Description:
 *      Store the network context.
 *      Sends the command:
 *          AT+STORENWCONN
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreNwConn(void)
{
    return AtLibGs_CommandSendString("AT+STORENWCONN\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReStoreNwConn
 *---------------------------------------------------------------------------*
 * Description:
 *      Restore the network context.
 *      Sends the command:
 *          AT+STORENWCONN
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReStoreNwConn(void)
{
    return AtLibGs_CommandSendString("AT+RESTORENWCONN\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IPSet
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure network IP address.
 *      Sends the command:
 *          AT+NSET=<IP>,<NetMask>,<Gateway>
 *      and waits for a response.
 *      <IP> is in the format ##:##:##:##
 *      <Netmask> is in the format ##:##:##:##
 *      <Gateway> is in the format ##:##:##:##
 * Inputs:
 *      char *pIpAddr -- IP Address string
 *      char *pSubnet -- Subnet mask string
 *      char *pGateway -- Gateway string
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_IPSet(char *pIpAddr, char *pSubnet, char *pGateway)
{
    char cmd[60];

    sprintf(cmd, "AT+NSET=%s,%s,%s\r\n", pIpAddr, pSubnet, pGateway);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SaveProfile
 *---------------------------------------------------------------------------*
 * Description:
 *      Save Settings to profile 0 or 1.
 *      Sends the command:
 *          AT&W<0|1>
 *      and waits for a response.
 * Inputs:
 *      uint8_t profile -- profile 0 or 1
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SaveProfile(uint8_t profile)
{
    char cmd[20];

    sprintf(cmd, "AT&W" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_LoadProfile
 *---------------------------------------------------------------------------*
 * Description:
 *      Load Settings from profile 0 or 1.
 *      Sends the command:
 *          ATZ<0|1>
 *      and waits for a response.
 * Inputs:
 *      uint8_t profile -- profile 0 or 1
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_LoadProfile(uint8_t profile)
{
    char cmd[10];

    sprintf(cmd, "ATZ" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ResetFactoryDefaults
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset factory defaults.
 *      Sends the command:
 *          AT&F
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ResetFactoryDefaults(void)
{
    return AtLibGs_CommandSendString("AT&F\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetRssi
 *---------------------------------------------------------------------------*
 * Description:
 *      Query the current RSSI value.
 *      Sends the command:
 *          AT+WRSSI=?
 *      and waits for a response.
 *      Call AtLibGs_ParseRssiResponse() to get the final result.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetRssi(void)
{
    return AtLibGs_CommandSendString("AT+WRSSI=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisAssoc
 *---------------------------------------------------------------------------*
 * Description:
 *      Disassociate from current network.
 *      Sends the command:
 *          AT+WD
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc(void)
{
    return AtLibGs_CommandSendString("AT+WD\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FWUpgrade
 *---------------------------------------------------------------------------*
 * Description:
 *      Initiate a firmware update to the given server.
 *      Sends the command:
 *          AT+FWUP=<SrvIp>,<SrvPort>,<SrcPort>
 *      and waits for a response.
 * Inputs:
 *      char *pSrvip -- IP address of the firmware upgrade server string
 *      uint16_t srvport -- server port number to be used for firmware
 *                              upgrade string
 *      uint16_t srcPort -- adapter port number to be used for firmware
 *                              upgrade.
 *      char *pSrcIP -- Retry is the number of times the node will repeat
 *                              the firmware upgrade attempt if failures
 *                              are encountered.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_FWUpgrade(
        char *pSrvip,
        uint16_t srvport,
        uint16_t srcPort,
        char *pSrcIP)
{
    char cmd[80];

    sprintf(cmd, "AT+FWUP=%s," _F16_ "," _F16_ ",%s\r\n", pSrvip, srvport,
            srcPort, pSrcIP);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryChkStop
 *---------------------------------------------------------------------------*
 * Description:
 *      Stop the battery check.
 *      Sends the command:
 *          AT+BCHKSTOP
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatteryCheckStop(void)
{
    return AtLibGs_CommandSendString("AT+BCHKSTOP\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_MultiCast
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable or disable multicast reception.
 *      Sends the command:
 *          AT+MCSTSET=<0|1>
 *      and waits for a response.
 * Inputs:
 *      uint8_t mode -- 0=disable multicast reception,
 *                      1=enable multicast reception
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_MultiCast(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+MCSTSET=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Version
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the Version Info.
 *      Sends the command:
 *          AT+VER=?
 *      and waits for a response.
 * Inputs:
 *      uint8_t mode -- 0=disable multicast reception,
 *                      1=enable multicast reception
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Version(void)
{
    return AtLibGs_CommandSendString("AT+VER=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Mode
 *---------------------------------------------------------------------------*
 * Description:
 *      Set mode to Infrastructure or Ad-Hoc
 *      Sends the command:
 *          AT+WM=<0|1|2>
 *      and waits for a response.
 * Inputs:
 *      ATLIBGS_STATIONMODE_E mode -- Mode of station operation
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Mode(ATLIBGS_STATIONMODE_E mode)
{
    char cmd[30];

    sprintf(cmd, "AT+WM=" _F16_ "\r\n", (uint16_t)mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UDPServer_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Open UDP server on given port.
 *      Sends the command:
 *          AT+NSUDP=<Port>
 *      and waits for a response.
 *      The <Port> for the UDP Server to receive packets.
 *      Call AtLibGs_ParseUDPServerStartResponse() to parse the response
 *      into a connection id.
 * Inputs:
 *      uint16_t pUdpSrvPort -- Port on server string.
 *      uint8_t *cid -- Pointer to returned CID for this session.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code, ATLIBGS_MSG_ID_OK if CID found.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UDPServer_Start(uint16_t pUdpSrvPort, uint8_t *cid)
{
    char cmd[20];
    char * result = NULL;
    ATLIBGS_MSG_ID_E rxMsgId;

    sprintf(cmd, "AT+NSUDP=" _F16_ "\r\n", pUdpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((result = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
            *cid = result[8] - '0';
        } else if (((result = strstr((const char *)MRBuffer, "DISASSOCIATED"))
                != NULL) || (strstr((const char *)MRBuffer, "SOCKET FAILURE")
                != NULL)) {
            /* Failed  */
            return ATLIBGS_MSG_ID_DISCONNECT;
        } else {
            /* Failed  */
            return ATLIBGS_MSG_ID_ERROR;
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TCPServer_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Set mode to Infrastructure or Ad-Hoc
 *      Sends the command:
 *          AT+NSTCP=<Port>
 *      and waits for a response.
 *      The <Port> for the TCP Server to receive packets.
 *      Call AtLibGs_ParseTcpServerStartResponse() to parse the response
 *      into a connection id.
 * Inputs:
 *      uint16_t pTcpSrvPort -- TCP port string
 *      uint8_t *cid -- Connection ID returned
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TCPServer_Start(uint16_t pTcpSrvPort, uint8_t *cid)
{
    char cmd[20];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *pSubStr = NULL;

    sprintf(cmd, "AT+NSTCP=" _F16_ "\r\n", pTcpSrvPort);

    rxMsgId = AtLibGs_CommandSendString(cmd);

    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if ((pSubStr = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
            *cid = pSubStr[8] - '0';
        } else if (((strstr((const char *)MRBuffer, "DISASSOCIATED")) != NULL)
                || (strstr((const char *)MRBuffer, "SOCKET FAILURE") != NULL)) {
            /* Failed  */
            return ATLIBGS_MSG_ID_DISCONNECT;
        } else {
            /* Failed  */
            return ATLIBGS_MSG_ID_ERROR;
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DNSLookup
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the IP number from host name.
 *      Sends the command:
 *          AT+DNSLOOKUP=<name>
 *      and waits for a response.
 *      <name> is the URL of the address to lookup.
 *      Call AtLibGs_ParseDNSLookupResponse() to parse the response.
 *      TODO: Needs to return the parsed lookup or an error.
 * Inputs:
 *      char *pUrl -- URL to parse.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DNSLookup(char *pUrl)
{
    char cmd[30];

    sprintf(cmd, "AT+DNSLOOKUP=%s\r\n", pUrl);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close a previously opened connection
 *      Sends the command:
 *          AT+NCLOSE=<cid>
 *      and waits for a response.
 *      <cid> is the connection id
 * Inputs:
 *      uint8_t cid -- Connection id
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Close(uint8_t cid)
{
    char cmd[20];

    sprintf(cmd, "AT+NCLOSE="_F8_ "\r\n", cid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWRetryCount
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the wireless retry count.
 *      Sends the command:
 *          AT+WRETRY=<n>
 *      and waits for a response.
 *      <n> is the number of retries (default is 5)
 * Inputs:
 *      uint16_t count -- Number of wireless retries
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWRetryCount(uint16_t count)
{
    char cmd[20];

    sprintf(cmd, "AT+WRETRY=" _F16_ "\r\n", count);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetErrCount
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the error counts
 *      Sends the command:
 *          AT+ERRCOUNT=?
 *      and waits for a response.
 * Inputs:
 *      uint16_t *errorCount -- Number of errors
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetErrCount(uint32_t *errorCount)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1];

    rxMsgId = AtLibGs_CommandSendString("AT+ERRCOUNT=?\r\n");
    /* TODO: Parse commands here! */
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && AtLibGs_ParseIntoLines(MRBuffer,
            lines, 1)) {
        sscanf(lines[0], _F32_, errorCount);
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableRadio
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable or disable the radio.
 *      Sends the command:
 *          AT+WRXACTIVE=<mode>
 *      and waits for a response.
 *      <mode> is 0 when disabling the radio, 1 when enabled the radio.
 * Inputs:
 *      uint8_t mode -- 0 to disable the radio, 1 to enable the radio
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableRadio(uint8_t mode)
{
    char cmd[30];

    sprintf(cmd, "AT+WRXACTIVE=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnablePwSave
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable or disable the 802.11 power save mode.
 *      Sends the command:
 *          AT+WRXPS=<mode>
 *      and waits for a response.
 *      <mode> is 0 to disable power save mode, 1 to enable power save.
 *          The default is enabled.
 * Inputs:
 *      uint8_t mode -- 0 to disabe power save mode, 1 to enable
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnablePwSave(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+WRXPS=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTime
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the system time.
 *      Sends the command:
 *          AT+SETTIME=<date>,<time>
 *      and waits for a response.
 *      <date> is in the format "dd/mm/yyyy"
 *      <time> is in the format "HH:MM:SS"
 * Inputs:
 *      char *pDate -- Date string in format "dd/mm/yyyy"
 *      char *pTime -- Time string in format "HH:MM:SS"
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTime(char *pDate, char *pTime)
{
    char cmd[50];

    sprintf(cmd, "AT+SETTIME=%s,%s\r\n", pDate, pTime);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableExternalPA
 *---------------------------------------------------------------------------*
 * Description:
 *      This command enables or disables the external PA (patch antenna).
 *      Sends the command:
 *          AT+EXTPA=<mode>
 *      and waits for a response.
 *      <mode> is 1 to enable the external PA, or 0 to disable external PA.
 *      NOTE: If enabled, this command forces the adapter to standby and
 *          comes back immediately causing all configured parameters and
 *          network connections to be lost.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableExternalPA(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+EXTPA=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ConfigAntenna
 *---------------------------------------------------------------------------*
 * Description:
 *      This command is for Antenna Configuration **
 *        Not Supported on the GS1011.
 *    1 = PCB antenna
 *    2 = UFL Antenna
 * 
 * Inputs:
 *    1 or 2
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ConfigAntenna(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+ANTENNA=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SyncLossInterval
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the sync loss interval in TBTT interval.
 *      Sends the command:
 *          AT+WSYNCINTRL=<interval>
 *      and waits for a response.
 *      <interval> is the sync loss interval with a range of 1 to 65535.
 *          Default value is 30.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SyncLossInterval(uint16_t interval)
{
    char cmd[30];

    sprintf(cmd, "AT+WSYNCINTRL=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_PSPollInterval
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the Association Keep Alive Timer (PS) poll interval in
 *      seconds.
 *      Sends the command:
 *          AT+PSPOLLINTRL=<interval>
 *      and waits for a response.
 *      <interval> is the number of seconds to keep alive.  Default is 45.
 *          Range is 0 to 65535 seconds.  A value of 0 disables.
 * Inputs:
 *      uint16_t interval
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_PSPollInterval(uint16_t interval)
{
    char cmd[30];

    sprintf(cmd, "AT+PSPOLLINTRL=" _F16_ "\r\n", interval);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTxPower
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the transmit power.
 *      Sends the command:
 *          AT+WP=<power>
 *      and waits for a response.
 *      <power> is the transmit power.
 *      Transmit power can be 0 to 7 for internal PA GS101x with a default of
 *      0.  Transmit power can be 2 to 15 for external PA GS101x with a
 *      default power of 2.
 * Inputs:
 *      uint8_t power -- Power level (see above notes)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTxPower(uint8_t power)
{
    char cmd[20];

    sprintf(cmd, "AT+WP=" _F8_ "\r\n", power);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetDNSServerIP
 *---------------------------------------------------------------------------*
 * Description:
 *      Set static DNS IP address(es).
 *      Sends the command:
 *          AT+WSYNCINTRL=<dns1>[,<dns2>]
 *      and waits for a response.
 *      <dns1> is the primary DNS address in "##.##.##.##" format.
 *      <dns2> is the optional secondary DNS address in "##.##.##.##" format.
 * Inputs:
 *      char *pDNS1 -- Primary DNS address string in "##.##.##.##" format.
 *      char *pDNS2 -- Secondary DNS address string in "##.##.##.##" format.
 *          If NULL, no secondary DNS address is used.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetDNSServerIP(char *pDNS1, char *pDNS2)
{
    char cmd[30];

    if (pDNS2 == NULL) {
        sprintf(cmd, "AT+DNSSET=%s", pDNS1);
    } else {
        sprintf(cmd, "AT+DNSSET=%s,%s", pDNS1, pDNS2);
    }

    return AtLibGs_CommandSendString(cmd);

}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableAutoConnect
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable or enable auto connection.
 *      Sends the command:
 *          AT+ATC<mode>
 *      and waits for a response.
 *      NOTE: The resulting change only takes effect on the next reboot or
 *          an ATA command.
 * Inputs:
 *      uint8_t mode -- 0 to disable auto connect, 1 to enable auto connect.
 *          Default is disabled.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableAutoConnect(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "ATC" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SwitchFromAutoToCmd
 *---------------------------------------------------------------------------*
 * Description:
 *      Sends the command:
 *          AT+WSYNCINTRL=<interval>
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
void AtLibGs_SwitchFromAutoToCmd(void)
{
    App_Write("+++", 3);
    App_DelayMS(1000);
    App_Write("\r\n", 2);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreWAutoConn
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the auto connection wireless parameters for the current profile.
 *      Sends the command:
 *          AT+WSYNCINTRL=<interval>
 *      and waits for a response.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreWAutoConn(char * pSsid, uint8_t channel)
{
    char cmd[50];

    sprintf(cmd, "AT+WAUTO=0,%s,," _F8_ "\r\n", pSsid, channel);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreNAutoConn
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure auto connection network parameters (for client connection).
 *      Sends the command:
 *          AT+NAUTO=0,0,<ip addr>,<port>
 *      and waits for a response.
 *      <ip addr> is the IP address of the remote server
 *      <port> is the port address of the remote server
 * Inputs:
 *      char *pIpAddr -- IP address string in format "##.##.##.##"
 *      int16_t pRmtPort -- Port address of remote system
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreNAutoConn(char *pIpAddr, int16_t pRmtPort)
{
    char cmd[50];

    sprintf(cmd, "AT+NAUTO=0,0,%s," _F16_ "\r\n", pIpAddr, pRmtPort);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StoreATS
 *---------------------------------------------------------------------------*
 * Description:
 *      Store network and configuration parameters.  See documentation
 *      for configuration parameters.
 *          0=Network Connection Timeout
 *              Maximum time to establish auto connection in 10 ms
 *              intervals.  Default is 1000 or 10 seconds.
 *          1=Auto Associate Timeout
 *              Maximum time to associate to desired wireless network in
 *              10 ms intervals.  Default is 1000 or 10 seconds.
 *          2=TCP Connection Timeout
 *              Maximum time to establish TCP client connection in
 *              10 ms intervals.  Default is 500 or 5 seconds.
 *          3=Association Retry Count
 *              Not currently supported
 *          4=Nagle Algorithm Wait Time
 *              Maximum time for serial data sent in Auto Connect Mode
 *              to be buffered in 10 ms intervals.  Default is 10 or 100 ms.
 *          5=Scan Time
 *              Maximum time for scanning in one radio chanenel in
 *              milliseconds.  Default is 20 or 20 ms.
 *      Sends the command:
 *          AT+ATS<param>=<time>
 *      and waits for a response.
 *      <param> is the numeric parameter from the above.
 *      <time> is the value to be used for the given <param> (see above).
 * Inputs:
 *      uint8_t param -- Timing parameter to set
 *      uint8_t value -- Timing value to use
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StoreATS(uint8_t param, uint8_t value)
{
    char cmd[30];

    sprintf(cmd, "ATS" _F8_ "=" _F8_ "\r\n", param, value);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BData
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable or disable bulk data reception.
 *      Sends the command:
 *          AT+BDATA=<mode>
 *      and waits for a response.
 *      <mode> is 1 to enable, or 0 to disable.
 * Inputs:
 *      uint8_t mode -- 0 to disable bulk data reception, or 1 to enable.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BData(uint8_t mode)
{
    char cmd[20];

    sprintf(cmd, "AT+BDATA=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseWPSResult
 *---------------------------------------------------------------------------*
 * Description:
 *      This internal helper function takes the response from WPS
 *      (either button or pin) and parses the data into the fields.
 * Inputs:
 *      AtLibGs_WPSResult *result -- Resulting WPS answer, if any
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
static void AtLibGs_ParseWPSResult(AtLibGs_WPSResult *result)
{
    char * lines[3];
    char * tokens[2];

    // Clear the result
    memset(result, 0, sizeof(result));

    // Parse the items and stuff them into the structure
    if (AtLibGs_ParseIntoLines(MRBuffer, lines, 3) >= 3) {
        if (AtLibGs_ParseIntoTokens(lines[0], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "SSID") == 0)
                strcpy(result->ssid, tokens[1]);
        }

        if (AtLibGs_ParseIntoTokens(lines[1], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "CHANNEL") == 0)
                result->channel = atoi(tokens[1]);
        }

        if (AtLibGs_ParseIntoTokens(lines[2], '=', tokens, 2) >= 2) {
            if (strcmp(tokens[0], "PASSPHRASE") == 0)
                strcpy(result->password, tokens[1]);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartWPSPUSH
 *---------------------------------------------------------------------------*
 * Description:
 *      Associate to an access point (AP) using Wi-Fi Protected Setup (WPS)
 *      using the push button method.
 *      Sends the command:
 *          AT+WWPS=1
 *      and waits for a response.
 *      The network then connects to the network.
 * Inputs:
 *      AtLibGs_WPSResult *result -- Resulting WPS answer, if any
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartWPSPUSH(AtLibGs_WPSResult *result)
{
    ATLIBGS_MSG_ID_E rxMsgId;

    /* send pushbutton command */
    rxMsgId = AtLibGs_CommandSendString("AT+WWPS=1\r\n");

    /* wait for valid responce then parse the ssid, channel, and passphrase */
    /* might be 20 seconds from buton push till response */
    if (rxMsgId == ATLIBGS_MSG_ID_OK)
        AtLibGs_ParseWPSResult(result);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartWPSPIN
 *---------------------------------------------------------------------------*
 * Description:
 *      Associate to an access point (AP) using Wi-Fi Protected Setup (WPS)
 *          using the Personal Identification Number (PIN) method.
 *      Sends the command:
 *          AT+WWPS=2,<pin>
 *      and waits for a response.
 *      <pin> is a unique PIN.
 *      AtLibGs_WPSResult *result -- Resulting WPS answer, if any
 * Inputs:
 *      char *pin -- PIN string to pass in when doing WPS
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartWPSPIN(char *pin, AtLibGs_WPSResult *result)
{
    char cmd[50];
    ATLIBGS_MSG_ID_E rxMsgId;

    sprintf(cmd, "AT+WWPS=2,%s", pin);
    rxMsgId = AtLibGs_CommandSendString(cmd);

    /* wait for valid responce then parse the ssid, channel, and passphrase */
    if (rxMsgId == ATLIBGS_MSG_ID_OK)
        AtLibGs_ParseWPSResult(result);

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIntoLines
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert the given text into a list of pointers to the
 *      lines in that text and null terminate each line.
 * Inputs:
 *      char *text -- Pointer to text to parse into lines
 *      char *pLines[] -- Pointer to an array of lines
 *      uint16_t maxLines -- Maximum number of lines to return
 * Outputs:
 *      uint16_t -- Number of lines found (up to maximum number)
 *---------------------------------------------------------------------------*/
uint16_t AtLibGs_ParseIntoLines(char *text, char *pLines[], uint16_t maxLines)
{
    char *p = text;
    uint8_t c;
    int mode = 0;
    uint16_t numLines = 0;

    /* Walk through all the characters and determine where the lines are */
    while ((*p) && (numLines < maxLines)) {
        c = *p;
        switch (mode) {
            case 0: /* looking for start */
                /* Skip any line feeds -- we start a line with a \n */
                if (c == '\n')
                    break;
                pLines[numLines] = (char *)p;
                mode = 1;
                /* Fall into looking for a \n character immediately */
            case 1:
                /* Did we find the end of the line? */
                if ((c == '\n') || (c == '\r')) {
                    /* Null terminate the line and go to the next line */
                    *p = '\0';
                    numLines++;
                    mode = 0;
                } else {
                    /* looking at normal characters here */
                }
                break;
        }
        p++;
    }

    return numLines;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIntoTokens
 *---------------------------------------------------------------------------*
 * Description:
 *      Converts a line into a list of tokens.  Skips spaces before a token
 *      starts.
 * Inputs:
 *      char *pLines[] -- Pointer to an array of lines
 *      uint8_t deliminator -- Character that separates the fields
 * Outputs:
 *      uint16_t -- Number of lines found (up to maximum number)
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseIntoTokens(
        char *line,
        char deliminator,
        char *tokens[],
        uint8_t maxTokens)
{
    char *p = line;
    char c;
    int mode = 0;
    uint8_t numTokens = 0;
    char *lastNonWhitespace;

    /* Walk through all the characters and determine where the tokens are */
    /* while placing end of token characters on each token */
    while (numTokens < maxTokens) {
        c = *p;
        switch (mode) {
            case 0: /* looking for start */
                tokens[numTokens] = p;
                mode = 1;
                /* Fall into looking for a non-white space character immediately */
            case 1:
                /* Skip any white space at the beginning (by staying in this state) */
                if (isspace(c))
                    break;
                tokens[numTokens] = p;
                lastNonWhitespace = p;
                mode = 2;
                /* Fall into mode 2 if not a white space and process immediately */
            case 2:
                /* Did we find the end of the token? */
                if ((c == deliminator) || (c == '\0')) {
                    /* Null terminate the token after the last non-whitespace and */
                    /* go back to finding the next token */
                    lastNonWhitespace[1] = '\0';
                    numTokens++;
                    mode = 0;
                } else {
                    /* looking at normal characters here */
                    if (!isspace(c))
                        lastNonWhitespace = p;
                }
                break;
        }
        /* Reached end of string */
        if (c == '\0')
            break;
        p++;
    }

    return numTokens;
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_ParseUDPClientCid
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned for a UDP Client connection.
 *      If a CONNECT <id> is returned, return <id>.  Otherwise, respond with
 *      ATLIBGS_INVALID_CID.
 * Inputs:
 *    void
 * Outputs:
 *      uint8_t -- Returned connection id or ATLIBGS_INVALID_CID.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseUDPClientCid(void)
{
    uint8_t cid;
    char *result = NULL;

    if ((result = strstr((const char *)MRBuffer, "CONNECT")) != NULL) {
        /* Succesfull connection done for UDP client */
        cid = result[ATLIBGS_UDP_CLIENT_CID_OFFSET_BYTE] - '0';
    } else {
        /* Not able to extract the CID */
        cid = ATLIBGS_INVALID_CID;
    }

    return cid;
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_ParseWlanConnStat
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command and determines if there is a wireless association.
 * Inputs:
 *    void
 * Outputs:
 *      uint8_t -- Returns true if associated, else false.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseWlanConnStat(void)
{
    char *pSubStr;

    /* Check whether response message contains the following */
    /* string "BSSID=00:00:00:00:00:00" */
    pSubStr = strstr((const char *)MRBuffer, "BSSID=00:00:00:00:00:00");

    if (pSubStr) {
        /* Not been associated */
        return false;
    } else {
        /* Already associated */
        return true;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLib_ParseGetMacResponse
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned after doing a AtLibGs_GetMAC()
 *      command.  The passed in MAC Id is compared to the returned MAC
 *      address.  If the returned MAC address is "00:00:00:00:00:00" or
 *      the MAC address does not match the passed in MAC address, false
 *      is returned.  If the MAC address returned matches the passed in
 *      MAC address, true is returned.
 * Inputs:
 *      int8_t *pRefNodeMacId -- MAC address to compare to
 * Outputs:
 *      uint8_t -- Returns true if matching MAC address and not 0's,
 *          else false.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseGetMacResponse(char *pMAC) 
{
    char *pSubStr;
    char currNodeMac[20] = "00:00:00:00:00:00";

    /* Set the node MAC address, If the MAC is already set, */
    /* then skip the flash write operation */
    pSubStr = strstr((const char *)MRBuffer, ":");

    if (pSubStr) {
        /* Setup a copy to compare with */
        memcpy(currNodeMac, (pSubStr - 2), 17);
        currNodeMac[17] = '\0';
        
        pSubStr = &currNodeMac[0];
        while (*pSubStr != '\0')
        {
          if (*pSubStr != ':')
          {
               *pMAC = *pSubStr;
               pMAC++;
          }
          pSubStr++;
        }
          
        return true;

    } else {
        /* Failed to get MAC address information */
        return false;
    }
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_ParseNodeIPv4Address
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command.  The result is turned into an IP address.
 * Inputs:
 *      ATLIBGS_IPv4 *ip -- Passed in IP address to compare
 * Outputs:
 *      uint8_t -- Returns 0 if invalid IP address, 1 if valid ip address
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseNodeIPv4Address(ATLIBGS_IPv4 *ip)
{
    char *pSubStr;
    char ipstr[20];

    pSubStr = strstr((const char *)MRBuffer, "IP addr=");
    if (pSubStr) {
        strcpy((char *)ipstr, strtok((pSubStr + 8), ": "));
        if (ipstr[0] == '0') {
            /* Failed */
            return 0;
        }
    } else {
        /* Failed */
        return 0;
    }

    /* Success */
    return 1;

}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIPv4Address
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the the given line in "###.###.###.###" format into a IPV4
 *      address.
 * Inputs:
 *      const char *line -- Line to parse
 *      ATLIBGS_IPv4 *ip -- IP address parsed
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void AtLibGs_ParseIPv4Address(const char *line, ATLIBGS_IPv4 *ip)
{
    const char *p = line;
    uint8_t n;

    *ip[0] = *ip[1] = *ip[2] = *ip[3] = 0;
    for (n = 0; (n < 4) && p; n++) {
        *ip[n] = atoi(p);
        p = strchr(p, '.');
        if (!p)
            break;
        p++;
    }
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_ParseRssiResponse
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned after doing a AtLibGs_WlanConnStat()
 *      command.  The RSSI value is returned by reference.
 * Inputs:
 *      int16_t *pRSSI -- Returned passed in value
 * Outputs:
 *      uint8_t -- Returns 1 if RSSI value found and valid, else 0.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseRssiResponse(int16_t *pRssi)
{
    char *pSubStr;

    if ((pSubStr = strstr((const char *)MRBuffer, "-")) != NULL) {
        *pRssi = atoi((pSubStr));

        return 1;
    } else {
        return 0;
    }
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_ParseDNSLookupResponse
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses the last line returned after doing a AtLibGs_DNSLookup()
 *      command.  If connected, parses the TCP server connection id.
 *      Otherwise, no connection is returned.
 * Inputs:
 *      int16_t *pConnId -- Returned connection id (if any)
 * Outputs:
 *      uint8_t -- Returns 1 if connected, else 0.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ParseDNSLookupResponse(char *ipAddr)
{
    char *pSubStr = NULL;

    pSubStr = strstr((const char *)MRBuffer, "IP:");
    if (pSubStr) {
        strcpy(ipAddr, (pSubStr + 3));

        return 1; /* Success */
    } else {
        return 0; /* Failed  */
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_CommandSendString
 *---------------------------------------------------------------------------*
 * Description:
 *      Sends a raw command to the module and waits for a response.  If
 *      data is returned, it is collected into MRBuffer.
 * Inputs:
 *      char *aString -- String to send
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_CommandSendString(char *aString)
{
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf(">%s\n", aString);
#endif

    /* Now send the command to S2w App node */
    App_Write((char *)aString, strlen(aString));

    /* Wait for the response while collecting data into the MRBuffer */
    return AtLibGs_ResponseHandle();
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DataSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send data to the module.
 * Inputs:
 *      const char *pTxData -- Data to send
 *      uint16_t dataLen -- Length of data to send
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_DataSend(const void *pTxData, uint16_t dataLen)
{
    App_Write(pTxData, dataLen);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SendTCPData
 *---------------------------------------------------------------------------*
 * Description:
 *      Send data to the given TCP connection.  The data is converted into
 *      the following byte format:
 *          <ESC><'S'><cid><N bytes><ESC><'E'>
 *              <ESC> is the escape character 0x1B
 *              <'S'> is the letter 'S'
 *              <cid> is the connection ID.
 *              <N bytes> is a number of bytes
 *              <'E'> is the letter 'E'
 * Inputs:
 *      uint8_t cid -- Connection ID
 *      const char *pTxData -- Data to send to the TCP connection
 *      uint16_t dataLen -- Length of data to send
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- ATLIBGS_MSG_ID_OK if properly sent, else
 *              error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SendTCPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen)
{
    char cmd[20];
    ATLIBGS_MSG_ID_E rxMsgId;

    if (cid == ATLIBGS_INVALID_CID)
        return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;

    /* Construct the data start indication message */
    sprintf(cmd, "%c%c" _F8_, ATLIBGS_ESC_CHAR, 'S', cid);

    /* Now send the data START indication message  to S2w node */
    App_Write(cmd, 3);

    /* Look for an immediate OK or Fail response */
    rxMsgId = AtLibGs_ResponseHandle();
    if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK) {
        /* Now send the actual data */
        AtLibGs_DataSend(txBuf, dataLen);

        /* Construct the data start indication message */
        sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

        /* Now send the data END indication message  to S2w node */
        App_Write(cmd, 2);

        rxMsgId = AtLibGs_ResponseHandle();
        if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
            rxMsgId = ATLIBGS_MSG_ID_OK;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SendUDPData
 *---------------------------------------------------------------------------*
 * Description:
 *      Send data to the given UDP connection.  The data is converted into
 *      the following byte format when UDP client:
 *          <ESC><'S'><cid><N bytes><ESC><'E'>
 *      When UDP server, transmit in the following format:
 *          <ESC><'U'><cid><ip><port><N bytes><ESC><'E'>
 *      <ESC> is the escape character 0x1B
 *      <'S'> is the letter 'S'
 *      <cid> is the connection ID.
 *      <N bytes> is a number of bytes
 *      <'E'> is the letter 'E'
 *      <'U'> is the letter 'U'
 *      <ip> is the ip number of the client
 *      <port> is the port number of the client
 * Inputs:
 *      uint8_t cid -- Connection ID
 *      const char *pTxData -- Data to send to the TCP connection
 *      uint16_t dataLen -- Length of data to send
 *      ATLIBGS_CON_TYPE conType -- Type of connection
 *          (ATLIBGS_CON_UDP_SERVER or ATLIBGS_CON_UDP_CLIENT)
 *      char *pUdpClientIP -- String with client IP number
 *      uint16_t udpClientPort -- Port id of client
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SendUDPData(
        uint8_t cid,
        const void *txBuf,
        uint16_t dataLen,
        ATLIBGS_CON_TYPE conType,
        const char *pUdpClientIP,
        uint16_t udpClientPort)
{
    char cmd[30];
    ATLIBGS_MSG_ID_E rxMsgId;

    if (ATLIBGS_INVALID_CID != cid) {
        /* Construct the data start indication message */
        if (ATLIBGS_CON_UDP_SERVER == conType) {
            /* <ESC> < U>  <cid> <ip address><:> <port numer><:> <data> <ESC> < E> */
            sprintf(cmd, "%c%c" _F8_ "%s:" _uF16_ ":", ATLIBGS_ESC_CHAR, 'U',
                    cid, pUdpClientIP, udpClientPort);
        } else {
            /* <ESC> < S>  <cid>  <data> <ESC> < E> */
            sprintf(cmd, "%c%c" _F8_, ATLIBGS_ESC_CHAR, 'S', cid);
        }

        /* Now send the data START indication message  to S2w node */
        App_Write(cmd, strlen(cmd));
        rxMsgId = AtLibGs_ResponseHandle();
        if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK) {

            /* Now send the actual data */
            AtLibGs_DataSend(txBuf, dataLen);

            /* Construct the data start indication message  */
            sprintf(cmd, "%c%c", ATLIBGS_ESC_CHAR, 'E');

            /* Now send the data END indication message  to S2w node */
            App_Write(cmd, strlen(cmd));
        }
        rxMsgId = AtLibGs_ResponseHandle();
        if (rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
            rxMsgId = ATLIBGS_MSG_ID_OK;

    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BulkDataTransfer
 *---------------------------------------------------------------------------*
 * Description:
 *      Send bulk data to a current transfer.  Bulk data is transferred in
 *      the following format:
 *          <ESC><'Z'><cid><data length><N bytes><ESC><'E'>
 *      <ESC> is the escape character 0x1B
 *      <'Z'> is the letter 'Z'
 *      <cid> is the connection ID
 *      <data length> is 4 ASCII characters with the data length
 *      <N bytes> is a number of bytes, <= 1400 bytes
 *      <'E'> is the letter 'E'
 * Inputs:
 *      uint8_t cid -- Connection ID
 *      const char *pTxData -- Data to send to the TCP connection
 *      uint16_t dataLen -- Length of data to send
 *      ATLIBGS_CON_TYPE conType -- Type of connection
 *          (ATLIBGS_CON_UDP_SERVER or ATLIBGS_CON_UDP_CLIENT)
 *      char *pUdpClientIP -- String with client IP number
 *      uint16_t udpClientPort -- Port id of client
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_BulkDataTransfer(uint8_t cid, const void *pData, uint16_t dataLen)
{
    /*<Esc> <Z> <Cid> <Data Length xxxx 4 ascii char> <data> */
    char digits[5];
    char cmd[20];

    /* Construct the bulk data start indication message  */
    AtLibGs_ConvertNumberTo4DigitASCII(dataLen, digits);
    sprintf(cmd, "%c%c" _F8_ "%s", ATLIBGS_ESC_CHAR, 'Z', cid, digits);

    /* Now send the bulk data START indication message  to S2w node */
    App_Write(cmd, strlen(cmd));

    /* wait for GS1011 to process above command LeZ */
    /* TODO: Why the delay? */
    //App_DelayMS(1000);

    /* Now send the actual data */
    App_Write(pData, dataLen);
}

ATLIBGS_MSG_ID_E AtLibGs_BulkDataTransferTest(uint8_t cid, uint16_t dataLen)
{
    /*<Esc> <Z> <Cid> <Data Length xxxx 4 ascii char> <data> */
    char digits[5]; int i;
    char cmd[20], TxData;  ATLIBGS_MSG_ID_E rxMsgId;

    /* Construct the bulk data start indication message  */
    AtLibGs_ConvertNumberTo4DigitASCII(dataLen, digits);
    sprintf(cmd, "%c%c" _F8_ "%s", ATLIBGS_ESC_CHAR, 'Z', cid, digits);

    /* Now send the bulk data START indication message  to S2w node */
    App_Write(cmd, strlen(cmd));

    /* wait for GS1011 to process above command LeZ */
    /* TODO: Why the delay? */
    // App_DelayMS(1000);
    
    rxMsgId = AtLibGs_ResponseHandle();
    if(rxMsgId == ATLIBGS_MSG_ID_ESC_CMD_OK)
    {
        rxMsgId = ATLIBGS_MSG_ID_OK;
           /* Now send the actual data */
        for(i=0; i<dataLen; i++)
        {
          TxData = i & 0xFF;
          App_Write(&TxData, 1);
        }
    }
    
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_checkEOFMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      This functions is used to check the completion of Commands
 *      This function will be called after receiving each line.
 * Inputs:
 *      const char *pBuffer -- Line of data to check
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code, ATLIBGS_MSG_ID_NONE is returned
 *          if the passed line is not identified.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_checkEOFMessage(const char *pBuffer)
{
    const char *p;
    uint8_t numSpaces;

    if ((strstr((const char *)pBuffer, "OK") != NULL)) {
        return ATLIBGS_MSG_ID_OK;
    } else if ((strstr((const char *)pBuffer, "ERROR") != NULL)) {
        return ATLIBGS_MSG_ID_ERROR;
    } else if ((strstr((const char *)pBuffer, "INVALID INPUT") != NULL)) {
        return ATLIBGS_MSG_ID_INVALID_INPUT;
    } else if ((strstr((const char *)pBuffer, "DISASSOCIATED") != NULL)) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        return ATLIBGS_MSG_ID_DISASSOCIATION_EVENT;
    } else if ((strstr((const char *)pBuffer, "ERROR: IP CONFIG FAIL") != NULL)) {
        return ATLIBGS_MSG_ID_ERROR_IP_CONFIG_FAIL;
    } else if (strstr((const char *)pBuffer, "ERROR: SOCKET FAILURE") != NULL) {
        /* Reset the local flags */
        return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;
    } else if ((strstr((const char *)pBuffer, "APP Reset-APP SW Reset"))
            != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();
        return ATLIBGS_MSG_ID_APP_RESET;
    } else if ((strstr((const char *)pBuffer, "DISCONNECT")) != NULL) {
        /* Reset the local flags */
        return ATLIBGS_MSG_ID_DISCONNECT;
    } else if ((strstr((const char *)pBuffer, "Disassociation Event")) != NULL) {
        /* reset the association flag */
        AtLibGs_ClearNodeAssociationFlag();
        return ATLIBGS_MSG_ID_DISASSOCIATION_EVENT;
    } else if ((strstr((const char *)pBuffer, "Out of StandBy-Alarm")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_STBY_ALARM;
    } else if ((strstr((const char *)pBuffer, "Out of StandBy-Timer")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_STBY_TIMER;
    } else if ((strstr((const char *)pBuffer, "UnExpected Warm Boot")) != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();
        return ATLIBGS_MSG_ID_UNEXPECTED_WARM_BOOT;
    } else if ((strstr((const char *)pBuffer, "Out of Deep Sleep")) != NULL) {
        return ATLIBGS_MSG_ID_OUT_OF_DEEP_SLEEP;
    } else if ((strstr((const char *)pBuffer, "Serial2WiFi APP")) != NULL) {
        /* Reset the local flags */
        AtLibGs_ClearNodeAssociationFlag();
        AtLibGs_SetNodeResetFlag();
        return ATLIBGS_MSG_ID_WELCOME_MSG;
    } else if ((pBuffer[0] == 'A') && (pBuffer[1] == 'T')
            && (pBuffer[2] == '+')) {
        /* Handle the echoed back AT Command, if Echo is enabled.  "AT+" . */
        return ATLIBGS_MSG_ID_NONE;
    } else if (strstr((const char *)pBuffer, "CONNECT ") != NULL) {
        /* Determine if this CONNECT line is of the type CONNECT <server id> <cid> <ip> <port> */
        /* by counting spaces in between the words.  A standard CONNECT <cid> is not the same */
        p = pBuffer;
        numSpaces = 0;
        while ((*p) && (*p != '\n')) {
            if (*p == ' ')
                numSpaces++;
            if (numSpaces >= 4)
                return ATLIBGS_MSG_ID_TCP_SERVER_CONNECT;
            p++;
        }
    }

    return ATLIBGS_MSG_ID_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReceiveDataHandle
 *---------------------------------------------------------------------------*
 * Description:
 *      Handle data coming in on a TCP or UDP connection.  Process all
 *      non-blocking data reads and return.
 * Inputs:
 *      uint32_t timeout -- time in milliseconds before this command times out
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code.  ATLIBGS_MSG_ID_OK if data is
 *          received.  ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout occurred.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReceiveDataHandle(uint32_t timeout)
{
    uint8_t rxData;
    uint32_t start = MSTimerGet();
    ATLIBGS_MSG_ID_E rxMsgId;

    /* Read one byte at a time - Use non-blocking call */
    while (1) {
        /* Has it taken too much time? */
        if (MSTimerDelta(start) >= timeout) {
            /* Yes, timeout */
            rxMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
            break;
        }

        /* See if there is any data to process */
        if (App_Read(&rxData, 1, 0)) {
            /* Process this byte */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId != ATLIBGS_MSG_ID_NONE)
                break;
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForUDPMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits for and parses a UDP message into CID, IP address, Port Number, and
 *      the message itself.
 * Inputs:
 *      uint32_t timeout -- Timeout in milliseconds, or 0 for no timeout
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT on timeout,
 *          or ATLIBGS_MSG_ID_OK if a message is returned
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForUDPMessage(uint32_t timeout)
{
    uint8_t rxData;
    uint32_t start = MSTimerGet();

    /* wait until message received */
    while (1) {
        /* Read one byte at a time - Use non-blocking call */
        while (App_Read(&rxData, 1, 0)) {
            /* Restart the timeout */
            start = MSTimerGet();

            /* Process the received data */
            if (AtLibGs_ReceiveDataProcess(rxData) == ATLIBGS_MSG_ID_DATA_RX) {
                return ATLIBGS_MSG_ID_OK;
            }
        }

        /* Is there a timeout (if any defined)? */
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForTCPMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 * Inputs:
 *      ATLIBGS_TCPConnection *connection -- Connection structure to fill
 *      uint32_t timeout -- Timeout in milliseconds, 0 for no timeout
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_DATA_RX if TCP message found.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPConnection(
        ATLIBGS_TCPConnection *connection,
        uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();
    char *p;
    char *tokens[6];
    uint16_t numTokens;

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_TCP_SERVER_CONNECT) {
                // Now parse out the TCP connection information
                p = strstr(MRBuffer, "CONNECT");
                numTokens = AtLibGs_ParseIntoTokens(p, ' ', tokens, 6);
                if (numTokens >= 5) {
                    // Parse the connection information
                    connection->server_cid = atoi(tokens[1]);
                    connection->cid = atoi(tokens[2]);
                    strcpy(connection->ip, tokens[3]);
                    connection->port = atoi(tokens[4]);
                    return rxMsgId;
                } else {
                    return ATLIBGS_MSG_ID_ERROR;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForTCPMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 * Inputs:
 *      uint32_t timeout -- Timeout in milliseconds, 0 for no timeout
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_DATA_RX if TCP message found.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForTCPMessage(uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_DATA_RX)
                return rxMsgId;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WaitForHTTPMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits for and parses a TCP message into CID and the message itself.
 *      If data is received, it is passed to the routine
 *      App_ProcessIncomingData.
 * Inputs:
 *      uint32_t timeout -- Timeout in milliseconds, 0 for no timeout
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX if HTTP message found.
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WaitForHTTPMessage(uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    uint8_t rxData;
    uint32_t start = MSTimerGet();

    /* wait until message received or timeout*/
    while (1) {
        if ((MSTimerDelta(start) >= timeout) && (timeout))
            return ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;

        while (App_Read(&rxData, 1, 0)) {
            /* If we got data, reset the timeout */
            start = MSTimerGet();

            /* Process the received data */
            rxMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (rxMsgId == ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX)
                return rxMsgId;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseTCPData
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses a packet of data returned from the TCP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForTCPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 * Inputs:
 *      uint8_t *received -- pointer to received bytes
 *      uint8_t length -- number of bytes received
 *      ATLIBGS_TCPMessage *msg -- pointer to structure to fill with
 *          TCP message info.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 *---------------------------------------------------------------------------*/
void AtLibGs_ParseTCPData(
        const uint8_t *received,
        uint16_t length,
        ATLIBGS_TCPMessage *msg)
{
    uint8_t *p = (uint8_t *)received;

    if (length >= 1) {
        msg->cid = p[0] - '0';
        msg->numBytes = length - 1;
        msg->message = &p[1];
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseUDPData
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses a packet of data returned from the UDP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForUDPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 * Inputs:
 *      uint8_t *received -- pointer to received bytes
 *      uint8_t length -- number of bytes received
 *      ATLIBGS_UDPMessage *msg -- pointer to structure to fill with
 *          UDP message info.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 *---------------------------------------------------------------------------*/
void AtLibGs_ParseUDPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_UDPMessage *msg)
{
    int i;
    int n;
    char port[6];

    if (length >= 1) {
        /* set CID */
        msg->cid = received[0] - '0';

        /* clear IP array */
        for (n = 0; n <= 16; n++)
            msg->ip[n] = 0x00;

        /* Read in the IP address (16 bytes) */
        for (i = 0; i < 16; i++) {
            if (received[i + 1] != 0x20) {
                /* save IP to array */
                msg->ip[i] = received[i + 1];
            } else {
                /* ignore spaces */
                msg->ip[i] = '\0';
                break;
            }
        }
        i = i + 2;

        for (n = 0; n < 5; n++) {
            if (received[i] != 0x09) {
                /* save port to array */
                port[n] = received[i++];
            } else {
                /* ignore tab */
                break;
            }
        }
        port[n] = '\0';
        msg->port = atoi(port);
        /* save message */
        msg->message = (uint8_t *)(received + i + 1);
        msg->numBytes = length - (i + 1);
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseHTTPData
 *---------------------------------------------------------------------------*
 * Description:
 *      Parses a packet of data returned from the HTTP stream.  Use this
 *      function after a successful response from AtLibGs_WaitForHTTPMessage()
 *      and the data is caught by App_ProcessIncomingData().
 * Inputs:
 *      uint8_t *received -- pointer to received bytes
 *      uint8_t length -- number of bytes received
 *      ATLIBGS_HTTPMessage *msg -- pointer to structure to fill with
 *          TCP message info.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- returns ATLIBGS_MSG_ID_RESPONSE_TIMEOUT if timeout,
 *          or ATLIBGS_MSG_ID_GENERAL_MESSAGE if TCP message found.
 *---------------------------------------------------------------------------*/
void AtLibGs_ParseHTTPData(
        const uint8_t *received,
        uint8_t length,
        ATLIBGS_HTTPMessage *msg)
{
    uint8_t *p = (uint8_t *)received;

    if (length >= 1) {
        msg->cid = p[0] - '0';
        msg->numBytes = length - 1;
        msg->message = &p[1];
    } else {
        // No data to report
        msg->numBytes = 0;
        msg->message = 0;
    }
}

#if 1
int stricmp(const char *s1, const char *s2)
{
	const uint8_t *us1 = (const uint8_t *)s1, *us2 = (const uint8_t *)s2;

	while (tolower(*us1) == tolower(*us2)) {
		if (*us1++ == '\0')
			return (0);
		us2++;
	}
	return (tolower(*us1) - tolower(*us2));
}

char  *pDebugMsg;
char  at_cmd_buf[128];

char *strrev(char *str) {
	char *p1, *p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}

	return str;
}

void 
AtLib_ConvertNumberTo4DigitASCII(uint32_t myNum, int8_t *pStr)
{/* Its equivalent of  sprintf("%04d",myNum) */

	/* TBD: Find better way to convert number to 4 digit ASCII value */

	uint8_t	digit1 ;
	uint8_t	digit2 ;
	uint8_t	digit3 ;
	uint8_t	digit4 ;

	digit1 = myNum/1000;
	digit2 = (myNum%1000)/100;
	digit3 = ((myNum%1000)%100)/10;
	digit4 = ((myNum%1000)%100)%10;

	sprintf((char *)pStr,"%d%d%d%d",digit1,digit2,digit3,digit4);
	
}

char *itoa(int n, char *s, int b) {
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i=0, sign;
    
	if ((sign = n) < 0)
		n = -n;

	do {
		s[i++] = digits[n % b];
	} while ((n /= b) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	return strrev(s);
}

void AtLib_GSLinkSendValue( int8_t *pTag, uint8_t cid, int32_t value)
{
    uint32_t command_length;   int8_t cDataLen[5];
    char dataBuf[20], cData[5];
    
    itoa(value, cData, 10);
    
    sprintf (&(dataBuf[0]),"%s%c%s", pTag,':',cData); 
    command_length = strlen(dataBuf);      /* Get command length */
    AtLib_ConvertNumberTo4DigitASCII(command_length, cDataLen);
     
    sprintf ( &(at_cmd_buf[0]),"%c%c%c%s%s",ATLIBGS_ESC_CHAR,'G', cid,cDataLen, dataBuf); 
    command_length = strlen(at_cmd_buf);
    
    //printf("\n\r%s", at_cmd_buf);
    //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
	/* Now send the bulk data START indication message  to S2w node */
	#ifdef USE_SPI
	    AtLibGs_DataSend((uint8_t *) &at_cmd_buf[0],command_length);
	#else
	   GS_UARTTransfer((uint8_t *) &at_cmd_buf[0],command_length);
	#endif
}
extern int16_t		gAccData[3];
extern float		gTemp, gTemp_F, gTemp_C;
extern uint8_t      gTempMode;
extern uint16_t		gAmbientLight;
extern uint8_t		gSetLight_onoff;
//extern const char  str_URL[32];
//extern const char  str_rootTag[16];
// extern enum { TEMP_MODE_F, TEMP_MODE_C };

void AtLib_GSLinkSendTempValue( int8_t *pTag, uint8_t cid)
{
    uint32_t command_length;   char  dataBuf[25];
    int8_t cDataLen[5];

#if 0    
   // itoa(value, cData, 10);  
    if(gTempMode == TEMP_MODE_C) 
    {
        sprintf (&(dataBuf[0]),"%s%c%.1fC", pTag,':',gTemp_C); 
    }
    else
    {   
       // itoa(gTemp_F, cData, 10);
        sprintf (&(dataBuf[0]),"%s%c%.1fF", pTag,':',gTemp_F);       
    }
#endif
    sprintf (&(dataBuf[0]),"%s%c%.1fF", pTag,':',gTemp_F);   
    command_length = strlen((const char *)dataBuf);      /* Get command length */
    AtLib_ConvertNumberTo4DigitASCII(command_length, cDataLen);
     
    sprintf ( &(at_cmd_buf[0]),"%c%c%c%s%s",ATLIBGS_ESC_CHAR,'G', cid,cDataLen, dataBuf); 
    command_length = strlen(at_cmd_buf);
    
    //printf("\n\r%s", at_cmd_buf);
    //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
	/* Now send the bulk data START indication message  to S2w node */
	#ifdef USE_SPI
	    AtLibGs_DataSend((uint8_t *) &at_cmd_buf[0],command_length);
	#else
	   GS_UARTTransfer((uint8_t *) &at_cmd_buf[0],command_length);
	#endif
}

void AtLib_GSLinkSend3Value( int8_t *pTag, uint8_t cid, int32_t value1, int32_t value2, int32_t value3)
{
    uint32_t command_length;   char cData1[5], cData2[5], cData3[5];
    int8_t dataBuf[20], cDataLen[5];
    
    itoa(value1, cData1, 10);
    itoa(value2, cData2, 10);
    itoa(value3, cData3, 10);
    
    sprintf ((char *)&(dataBuf[0]),"%s%c%s,%s,%s", pTag,':',cData1, cData2, cData3); 
    command_length = strlen((const char *)dataBuf);      /* Get command length */
    AtLib_ConvertNumberTo4DigitASCII(command_length, cDataLen);
     
    sprintf ( &(at_cmd_buf[0]),"%c%c%c%s%s",ATLIBGS_ESC_CHAR,'G',cid,cDataLen, dataBuf); 
    command_length = strlen(at_cmd_buf);
    
    // printf("\n\r%s", at_cmd_buf);
    //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
	/* Now send the bulk data START indication message  to S2w node */
	#ifdef USE_SPI
	    AtLibGs_DataSend((uint8_t *) &at_cmd_buf[0],command_length);
	#else
	   GS_UARTTransfer((uint8_t *) &at_cmd_buf[0],command_length);
	#endif
}

void AtLib_GSLinkPostDoneAck( uint8_t cid )
{  
    uint32_t command_length; 
    
    sprintf ( &(at_cmd_buf[0]),"%c%c%c%s%s",ATLIBGS_ESC_CHAR,'G', cid, "0009", "Post:Done"); 
    command_length = strlen(at_cmd_buf);
    
    //printf("\n\r%s", at_cmd_buf);
    //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
	/* Now send the bulk data START indication message  to S2w node */
	#ifdef USE_SPI
	    AtLibGs_DataSend((uint8_t *) &at_cmd_buf[0],command_length);
	#else
	   GS_UARTTransfer((uint8_t *) &at_cmd_buf[0],command_length);
	#endif
}

void AtLib_GSLinkGetPostResp(uint8_t cid, uint8_t gslinkType, int32_t TimeOut, uint8_t *pURL, uint8_t *pHdTag, uint8_t numOfValue)
{ /*<Esc> <Z> <Cid> <Data Length xxxx 4 ascii char> <data>     */

	uint32_t command_length;   char cTimeout[5];  uint8_t TxCRD=0xD; 
    
    itoa(TimeOut, cTimeout, 10);
    
    sprintf (&(at_cmd_buf[0]),"%s%c,%x,%s,%s,%s,%x","AT+XMLSEND=",cid, gslinkType, cTimeout, pURL, pHdTag, numOfValue); 
     
    command_length = strlen(at_cmd_buf); /* Get command length */

    // printf("\n\r%s", at_cmd_buf);
    //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
	/* Now send the bulk data START indication message  to S2w node */
	#ifdef USE_SPI
	    AtLibGs_DataSend((uint8_t *) &at_cmd_buf[0],command_length);
        AtLibGs_DataSend(&TxCRD,1);                                       // send CR to end the AT command
	#else
	   GS_UARTTransfer((uint8_t *) &at_cmd_buf[0],command_length);
	#endif
       
	/* Now send the actual value numOfValue times */
       
     //AtLib_GSLinkSendValue( "temp", cid, gTemp_F);
       
     if(gslinkType == GSLINK_GET_RESP)
     {
       AtLib_GSLinkSendTempValue("temp", cid);
       AtLib_GSLinkSendValue( "light", cid, gAmbientLight);
       AtLib_GSLinkSend3Value( "acc", cid, gAccData[0], gAccData[1], gAccData[2]); 
       AtLib_GSLinkSendValue( "leds", cid, gSetLight_onoff);
     }
     else if(gslinkType == GSLINK_POST_RESP)
     {
        AtLib_GSLinkPostDoneAck(cid);
     }
}


#endif

const char ledTag[5]="leds";
const char ssidTag[5]="ssid";
const char channelTag[5]="chanl";
int valueLen;

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReceiveDataProcess
 *---------------------------------------------------------------------------*
 * Description:
 *      Process individually received characters
 * Inputs:
 *      uint8_t rxData -- Character to process
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReceiveDataProcess(uint8_t rxData)
{
    /* receive data handling state */
    static ATLIBGS_RX_STATE_E receive_state = ATLIBGS_RX_STATE_START;
    static uint16_t specialDataLen = 0;
    static uint8_t  rxCurrentCid=0, rxPOSTCid=0, escStatus=0, GetValue=0;
    static uint8_t specialDataLenCharCount = 0, GSLinkType=0;
    
    static  char *pPostTag, PostTag[8], *pPostValue, PostValue[20]; 
    
    uint16_t datalencount;

    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;

#ifdef ATLIBGS_DEBUG_ENABLE
    if ((isprint(rxData)) || (isspace(rxData)))
        ConsolePrintf("%c", rxData);
#endif    

    /* Process the received data */
    switch (receive_state) {
        case ATLIBGS_RX_STATE_START:
            switch (rxData) {
                case ATLIBGS_CR_CHAR:
                case ATLIBGS_LF_CHAR:
                    /* CR and LF at the begining, just ignore it */
                    MRBufferIndex = 0;
                    break;

                case ATLIBGS_ESC_CHAR:
                    /* ESCAPE sequence detected */
                    receive_state = ATLIBGS_RX_STATE_ESCAPE_START;
                    MRBufferIndex = 0;
                    specialDataLen = 0;
                    break;

                default:
                    /* Not start of ESC char, not start of any CR or NL */
                    MRBufferIndex = 0;
                    MRBuffer[MRBufferIndex] = rxData;
                    MRBufferIndex++;
                    receive_state = ATLIBGS_RX_STATE_CMD_RESP;
                    break;
            }
            break;

        case ATLIBGS_RX_STATE_CMD_RESP:
            if (ATLIBGS_LF_CHAR == rxData) {
                /* LF detected - Messages from S2w node are terminated with LF/CR character */
                MRBuffer[MRBufferIndex] = rxData;

                /* terminate string with NULL for strstr() */
                MRBufferIndex++;
                MRBuffer[MRBufferIndex] = '\0';
                rxMsgId = AtLibGs_checkEOFMessage(MRBuffer);

                if (ATLIBGS_MSG_ID_NONE != rxMsgId) {
                    /* command echo or end of response detected */
                    /* Now reset the  state machine */
                    receive_state = ATLIBGS_RX_STATE_START;
                }
            } else if (ATLIBGS_ESC_CHAR == rxData) {
                /* Defensive check - This should not happen */
                receive_state = ATLIBGS_RX_STATE_START;
                /* end of message hits here             */
                static bool esc2 = true;
                if (esc2 == true) {
                    esc2 = false;
                    return ATLIBGS_MSG_ID_GENERAL_MESSAGE;
                } else {
                    esc2 = true;
                }

            } else {
                MRBuffer[MRBufferIndex] = rxData;
                MRBufferIndex++;

                if (MRBufferIndex >= ATLIBGS_RX_CMD_MAX_SIZE) {
                    /* Message buffer overflow. Something seriousely wrong. */
                    MRBufferIndex = 0;

                    /* Now reset the  state machine */
                    receive_state = ATLIBGS_RX_STATE_START;
                }
            }
            break;

        case ATLIBGS_RX_STATE_ESCAPE_START:
            if (ATLIBGS_DATA_MODE_BULK_START_CHAR_H == rxData) {
                /* HTTP Bulk data handling start */
                /* <Esc>H<1 Byte - CID><4 bytes - Length of the data><data> */
                receive_state = ATLIBGS_RX_STATE_HTTP_RESPONSE_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_BULK_START_CHAR_Z == rxData) {
                /* Bulk data handling start */
                /* <Esc>Z<Cid><Data Length xxxx 4 ascii char><data>   */
                receive_state = ATLIBGS_RX_STATE_BULK_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_NORMAL_START_CHAR_S == rxData) {
                /* Start of data */
                /* ESC S  cid  <----data --- > ESC E  */
                receive_state = ATLIBGS_RX_STATE_DATA_HANDLE;
            } else if (ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL == rxData) {
                /* Start of raw data  */
                /* ESC R : datalen : <----data --- >
                 Unlike other data format, there is no ESC E at the end .
                 So extract datalength to find out the incoming data size */
                receive_state = ATLIBGS_RX_STATE_RAW_DATA_HANDLE;
                specialDataLen = 0;
            } else if (ATLIBGS_DATA_MODE_ESC_OK_CHAR_O == rxData) {
                /* ESC command response OK */
                /* Note: No need to take any action. Its just an data reception */
                /* acknowledgement S2w node */
                receive_state = ATLIBGS_RX_STATE_START;
                rxMsgId = ATLIBGS_MSG_ID_ESC_CMD_OK;
            } else if (ATLIBGS_DATA_MODE_ESC_FAIL_CHAR_F == rxData) {
                /* ESC command response FAILED */
                /* Note: Error reported from S2w node, you can use it */
                /* for debug purpose. */
                receive_state = ATLIBGS_RX_STATE_START;
                rxMsgId = ATLIBGS_MSG_ID_ESC_CMD_FAIL;
            } else if (ATLIBGS_DATA_MODE_UDP_START_CHAR_u == rxData) {
                /* Start of UDP data */
                /* ESC u  cid  <----data with info---> ESC E  */
                receive_state = ATLIBGS_RX_STATE_DATA_HANDLE;
            } else if( ATLIBGS_DATA_MODE_GSLINK_START_CHAR_K == rxData) {               // 1.GSLink K: <Esc>K<Cid><Data Length xxxx 4 ascii char><  
              receive_state = ATLIBGS_RX_STATE_DATA_CID;
              escStatus= ATLIBGS_DATA_MODE_GSLINK_START_CHAR_K;
              specialDataLenCharCount=0;
            } else if( ATLIBGS_DATA_MODE_GSLINK_START_CHAR_G == rxData) {              // * GSLink G:  <Esc>G<Data Length><Tag>:<value>
             
              receive_state = ATLIBGS_RX_STATE_GLINK_DATA_LEN;                     // * Go to get the data length
              escStatus= ATLIBGS_DATA_MODE_GSLINK_START_CHAR_G;
              specialDataLenCharCount=0;
            } else {
                /* ESC sequence parse error !  */
                /* Reset the receive buffer */
                receive_state = ATLIBGS_RX_STATE_START;
            }
            break;
	    case ATLIBGS_RX_STATE_DATA_CID:
          rxCurrentCid=rxData;
          if(escStatus==ATLIBGS_DATA_MODE_GSLINK_START_CHAR_K)
          {
             receive_state = ATLIBGS_RX_STATE_GLINK_DATA_LEN;                         // 2. GSLink CID
             // escStatus = 0;
          }
          else
             receive_state = ATLIBGS_RX_STATE_DATA_HANDLE;
          break;
        
        case ATLIBGS_RX_STATE_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);

            /* Keep receiving data till you get ESC E */
            do {
                App_Read(&rxData, 1, 1);

                /* Is this char an ESC? */
                while (rxData == ATLIBGS_ESC_CHAR) {
                    /* What is the next character? */
                    App_Read(&rxData, 1, 1);
                    /* Is it an 'E'? */
                    if (rxData == ATLIBGS_DATA_MODE_NORMAL_END_CHAR_E) {
                        /* End of data detected */
                        receive_state = ATLIBGS_RX_STATE_START;
                        rxMsgId = ATLIBGS_MSG_ID_DATA_RX;
                        return rxMsgId;
                    }
                    /* Go ahead and store the ESC character */
                    App_ProcessIncomingData(ATLIBGS_ESC_CHAR);

                    /* Repeat if the second charater was an ESC char */
                }

                /* Store whatever character we were left holding */
                App_ProcessIncomingData(rxData);
            } while (receive_state != ATLIBGS_RX_STATE_START);
            break;

        case ATLIBGS_RX_STATE_HTTP_RESPONSE_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);

            /* Get HTTP response length */
            specialDataLen = 0;
            for (datalencount = 0; datalencount < 4; datalencount++) {
                App_Read(&rxData, 1, 1);
                specialDataLen = (specialDataLen * 10) + ((rxData) - '0');
            }

            /* Now read actual data */
            while (specialDataLen--) {
                App_Read(&rxData, 1, 1);
                App_ProcessIncomingData(rxData);
            }

            receive_state = ATLIBGS_RX_STATE_START;
            rxMsgId = ATLIBGS_MSG_ID_HTTP_RESPONSE_DATA_RX;
            break;

        case ATLIBGS_RX_STATE_BULK_DATA_HANDLE:
            /* Store the CID */
            App_ProcessIncomingData(rxData);

            /* Get HTTP response length */
            specialDataLen = 0;
            for (datalencount = 0; datalencount < 4; datalencount++) {
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);
                specialDataLen = (specialDataLen * 10) + ((rxData) - '0');
            }

            /* Now read actual data */
            while (specialDataLen--) {
                /* Read one byte at a time */
                App_Read(&rxData, 1, 1);
                App_ProcessIncomingData(rxData);
            }
            receive_state = ATLIBGS_RX_STATE_START;
            break;

          case ATLIBGS_RX_STATE_GLINK_DATA_LEN:                                   // 3.  GSLink data length                                             
              specialDataLen = (specialDataLen * 10) + ((rxData) - '0');
              specialDataLenCharCount++;
              if(specialDataLenCharCount >= ATLIBGS_GSLINK_DATA_LEN_STRING_SIZE) {
                
                  if(escStatus == ATLIBGS_DATA_MODE_GSLINK_START_CHAR_K)
                  {
                     receive_state  = ATLIBGS_RX_STATE_GLINK_DATA_TYPE;      
                  }
                  else if(escStatus == ATLIBGS_DATA_MODE_GSLINK_START_CHAR_G)
                  {
                      if(specialDataLen==0)
                      {
                         // This is the end of G Post sequence
                         AtLib_GSLinkGetPostResp(rxPOSTCid, GSLINK_POST_RESP, 100, (uint8_t *)str_URL, (uint8_t *)str_rootTag, 1);  
                         rxPOSTCid = 0;
                         receive_state  = ATLIBGS_RX_STATE_START;                    // GS POSE command is completed.          
                      }
                      else
                      {
                        receive_state  = ATLIBGS_RX_STATE_GSLINK_DATA_HANDLE;       // ESC G sequence, skip Type check
                        pPostTag = PostTag;                                 // Post: prepare to receive the data tag
                      }
                  }
                  pDebugMsg = at_cmd_buf; 
              }
              break;
          case ATLIBGS_RX_STATE_GLINK_DATA_TYPE:                           // 4.  get GSLink Type
               GSLinkType = rxData - 0x30;                                 // convert to int
               if(GSLinkType == GSLINK_POST_XML)
                 rxPOSTCid = rxCurrentCid;                                 // need to save this CID, since ESC G sequence doesn't have a CID
               specialDataLen--;
               *pDebugMsg++ = rxData;
               receive_state  = ATLIBGS_RX_STATE_GSLINK_DATA_HANDLE;       // we are ready to get data
               break;
              
          case ATLIBGS_RX_STATE_GSLINK_DATA_HANDLE:                             
              specialDataLen--;
              *pDebugMsg++ = rxData;
              if(escStatus == ATLIBGS_DATA_MODE_GSLINK_START_CHAR_G)
              {
                 if(rxData==':')
                 {
                   *pPostTag = NULL;                                    // Post: the end of the tag string
                   GetValue = ESC_G_VALUE_ENABLE;                       // Post: get value 
                   pPostValue = PostValue;                              // Post: start receiving the value
                   valueLen = 0;
                 }
                 else if(GetValue == ESC_G_VALUE_ENABLE)
                 {
                   *pPostValue++ = rxData;                               //  Receiving the value string
                   ++valueLen;
                   if(specialDataLen == 0)
                   {  
                     *pPostValue = NULL;                                 // Post: the end of the value string
                     if(!strcmp(ledTag, PostTag))
                     {
                       gSetLight_onoff = PostValue[0] - 0x30;             // Get LED settting value, covert it from ascii format
                     }
                     else if(!strcmp(ssidTag, PostTag))
                     {
                         EEPROM_Write(8, (uint8_t*)PostValue, valueLen);  // write the SSID to eeprom
                     }
                     else if(!strcmp(channelTag, PostTag))
                     {
                         EEPROM_Write(30, (uint8_t*)PostValue, valueLen);  // write the channel number to eeprom
                     }
                     GetValue = 0;
                   }
                 }
                 else
                   *pPostTag++ = rxData;                                 // get the tag
              }
              if(specialDataLen == 0) 
              {
      #ifdef TIME_OUT_TEST            
                  clrTimeOutChk();
      #endif
                  pDebugMsg = 0;
                  if(GSLinkType == GSLINK_GET_XML)                       // this is GET request, send data
                  {
                     AtLib_GSLinkGetPostResp(rxCurrentCid, GSLINK_GET_RESP, 100, (uint8_t *)str_URL, (uint8_t *)str_rootTag, 4); // 4 value to be sent
                  }  
                  receive_state = ATLIBGS_RX_STATE_START;
                  escStatus = 0;                                       // this ESC sequence finished, let's clean it
                  //R_UART0_Send(at_cmd_buf, strlen(at_cmd_buf));
              };
              break;  
        case ATLIBGS_RX_STATE_RAW_DATA_HANDLE:
            if (ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL == rxData) {
                specialDataLenCharCount = 0;
            }

            /* Now reset the buffer and state machine */
            receive_state = ATLIBGS_RX_STATE_START;

            do {
                /* extracting the rx data length*/
                /* Read one byte at a time */
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);

                if (rxData != ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL) {
                    specialDataLen = (specialDataLen * 10) + ((rxData) - '0');
                    specialDataLenCharCount++;
                }
            } while ((rxData != ATLIBGS_DATA_MODE_RAW_INDICATION_CHAR_COL)
                    && (specialDataLenCharCount < 4));

            /* Now read actual data */
            while (specialDataLen) {
                /* Read one byte at a time - blocking call */
                App_Read(&rxData, 1, 1);
                specialDataLen--;
                App_ProcessIncomingData(rxData);
            }

            receive_state = ATLIBGS_RX_STATE_START;
            break;

        default:
            /* This case will not be executed */
            break;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ResponseHandle
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for a response after sending a command.  Keep parsing the
 *      data until a response is found.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ResponseHandle(void)
{
    ATLIBGS_MSG_ID_E responseMsgId;
    uint8_t rxData;
    uint32_t timeout = MSTimerGet();
    uint8_t gotData = 0;

    /* Reset the message ID */
    responseMsgId = ATLIBGS_MSG_ID_NONE;

    /* Now process the response from S2w App node */
    while (ATLIBGS_MSG_ID_NONE == responseMsgId) {
        gotData = 1;
        /* Read one byte at a time - non-blocking call, block here */
        timeout = MSTimerGet();
        while (!App_Read(&rxData, 1, 0)) {
            if (MSTimerDelta(timeout) >= ATLIB_RESPONSE_HANDLE_TIMEOUT) {
                gotData = 0;
                responseMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
                break;
            }
        }
        if (gotData) {
            /* Process the received data */
            responseMsgId = AtLibGs_ReceiveDataProcess(rxData);
            if (responseMsgId != ATLIBGS_MSG_ID_NONE) {
                /* Message successfully received from S2w App node */
                break;
            }
        }
    }

    return responseMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ProcessRxChunk
 *---------------------------------------------------------------------------*
 * Description:
 *      Process a group of received bytes looking for a response message.
 * Inputs:
 *      const char *rxBuf -- Pointer to bytes
 *      uint16_t bufLen -- Number of bytes in receive buffer
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ProcessRxChunk(const void *rxBuf, uint16_t bufLen)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    const uint8_t *rx = (uint8_t *)rxBuf;

    rxMsgId = ATLIBGS_MSG_ID_NONE;

    /* Parse the received data and check whether any valid message present in the chunk */
    while (bufLen) {
        /* Process the received data */
        rxMsgId = AtLibGs_ReceiveDataProcess(*rx);
        if (rxMsgId != ATLIBGS_MSG_ID_NONE) {
            /* Message received from S2w App node */
            break;
        }
        rx++;
        bufLen--;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FlushIncomingMessage
 *---------------------------------------------------------------------------*
 * Description:
 *      Read bytes until no more come in for 100 ms.
 * Inputs:
 *    void
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_FlushIncomingMessage(void)
{
    /* This function will read all incoming data until nothing happens */
    /* for 100 ms */
    uint8_t rxData;
    uint32_t start;

    /* Read one byte at a time - non-blocking call */
    start = MSTimerGet();
    while (MSTimerDelta(start) < 100) {
        if (App_Read(&rxData, 1, 0)) {
            start = MSTimerGet();
#ifdef ATLIBGS_DEBUG_ENABLE
            /* If required you can print the received characters on debug UART port. */
            ConsolePrintf("%c", rxData);
#endif
        }
    };
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetNodeResetFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the module reset flag.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void AtLibGs_SetNodeResetFlag(void)
{
    nodeResetFlag = true;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ClearNodeResetFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the module reset flag.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void AtLibGs_ClearNodeResetFlag(void)
{
    nodeResetFlag = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IsNodeResetDetected
 *---------------------------------------------------------------------------*
 * Description:
 *      Return flag telling if module has been reset.
 * Inputs:
 *      void
 * Outputs:
 *      uint8_t -- true if reset detected, else false
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_IsNodeResetDetected(void)
{
    return nodeResetFlag;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IsNodeAssociated
 *---------------------------------------------------------------------------*
 * Description:
 *      Return flag that tells if module is associated with a network.
 * Inputs:
 *    void
 * Outputs:
 *      uint8_t -- true if associated with network, else false.
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_IsNodeAssociated(void)
{
    return nodeAssociationFlag;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetNodeAssociationFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the flag for associated to network.
 * Inputs:
 *    void
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_SetNodeAssociationFlag(void)
{
    nodeAssociationFlag = true;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ClearNodeAssociationFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the flag for associated to network.
 * Inputs:
 *    void
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_ClearNodeAssociationFlag(void)
{
    nodeAssociationFlag = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ConvertNumberTo4DigitASCII
 *---------------------------------------------------------------------------*
 * Description:
 *      Equivalent of sprintf("%04d"), number to convert a number to
 *      four characters.
 * Inputs:
 *      uint16_t myNum -- Number to convert to text
 *      char *pStr -- Place to store characters
 * Outputs:
 *      int32_t -- result of comparison.  0 if matches.  <0 if s1 before s2.
 *          >0 if s1 after s2.
 *---------------------------------------------------------------------------*/
void AtLibGs_ConvertNumberTo4DigitASCII(uint16_t myNum, char *pStr)
{
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    uint8_t digit4;

    digit1 = myNum / 1000;
    digit2 = (myNum % 1000) / 100;
    digit3 = ((myNum % 1000) % 100) / 10;
    digit4 = ((myNum % 1000) % 100) % 10;

    sprintf((char *)pStr, _F8_ _F8_ _F8_ _F8_, digit1, digit2, digit3, digit4);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FlushRxBuffer
 *---------------------------------------------------------------------------*
 * Description:
 *      Flush by clearing the receiving buffer (MRBuffer).
 * Inputs:
 *    void
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_FlushRxBuffer(void)
{
    /* Reset the response receive buffer */
    /* TODO: What do we do here now? */
    MRBufferIndex = 0;
    memset(MRBuffer, '\0', ATLIBGS_RX_CMD_MAX_SIZE);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare the AtLib.
 * Inputs:
 *    void
 * Outputs:
 *    void
 *---------------------------------------------------------------------------*/
void AtLibGs_Init(void)
{
    /* Reset the command receive buffer */
    AtLibGs_FlushRxBuffer();

    /* Reset the flags */
    nodeAssociationFlag = false;
    nodeResetFlag = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Send command to get information about the device:
 *          ATIn
 *      where n is one of the following:
 *          ATLIBGS_ID_INFO_OEM = returns OEM name, (e.g., "GainSpan")
 *          ATLIBGS_ID_INFO_HARDWARE_VERSION = returns hardware version/model
 *              (e.g., "GS1011A1")
 *          ATLIBGS_ID_INFO_SOFTWARE_VERSION = returns version number
 *              (e.g., "2.3.5")
 * Inputs:
 *      uint8_t mode -- 1=to turn on echo, 0=turn off echo
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetInfo(
        ATLIBGS_ID_INFO_E info,
        char *infotext,
        int16_t maxchars)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char cmd[20];
    char *lines[5];
    uint16_t numLines;

    sprintf(cmd, "ATI" _F8_ "\r\n", info);

    rxMsgId = AtLibGs_CommandSendString(cmd);

    infotext[0] = 0;
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 5);
        if (numLines >= 2)
            strncpy((char *)infotext, lines[0], maxchars);
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetScanTimes
 *---------------------------------------------------------------------------*
 * Description:
 *      Send command to get WiFi scan times:
 *          AT+WST=?
 *      The response will be
 *          MinScanTime=<min>
 *          MaxScanTime=<max>
 *          OK
 *      Parse this data and return.
 * Inputs:
 *      uint16_t *min -- Minimum time in milliseconds
 *      uint16_t *Max -- Maximum time in milliseconds
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetScanTimes(uint16_t *min, uint16_t *max)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[5];
    uint16_t numLines;

    *min = 0;
    *max = 0;
    rxMsgId = AtLibGs_CommandSendString("AT+WST=?\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 5);
        if (numLines >= 2) {
            if ((strncmp((char *)(lines[0]), "MinScanTime=", 12) == 0)
                    && (strncmp((char *)(lines[1]), "MaxScanTime=", 12) == 0)) {
                *min = atoi(lines[0] + 12);
                *max = atoi(lines[1] + 12);
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetScanTimes
 *---------------------------------------------------------------------------*
 * Description:
 *      Send command to set WiFi scan times:
 *          AT+WST=<min>,<max>
 *      The response will be
 *          OK
 * Inputs:
 *      uint16_t min -- Minimum time in milliseconds
 *      uint16_t Max -- Maximum time in milliseconds
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetScanTimes(uint16_t min, uint16_t max)
{
    char cmd[30];

    sprintf(cmd, "AT+WST=" _F16_ "," _F16_ "\r\n", min, max);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetDefaultProfile
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the default profile (0 or 1).
 *      Sends the command:
 *          AT&Y<0|1>
 *      and waits for a response.
 * Inputs:
 *      uint8_t profile -- index of profile to use
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetDefaultProfile(uint8_t profile)
{
    char cmd[20];

    sprintf(cmd, "AT&Y" _F8_ "\r\n", profile);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetDefaultProfile
 *---------------------------------------------------------------------------*
 * Description:
 *      Gets the profile settings of one of the stored or active profiles.
 *      The profile setting is returned as a single string of all the
 *      AT command settings.
 *      Sends the command:
 *          AT&V
 *      and waits for a response.
 * Inputs:
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetProfile(
        ATLIBGS_PROFILE_E profile,
        char *text,
        uint16_t maxChars)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[50];
    uint16_t numLines;
    uint16_t offset = 0;
    uint16_t i;
    int mode;
    int len;
    static const char *sections[3] = {
            "ACTIVE PROFILE",
            "STORED PROFILE 0",
            "STORED PROFILE 1" };

    rxMsgId = AtLibGs_CommandSendString("AT&V\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 50);
        mode = 0;
        text[0] = '\0';
        for (i = 0; (i < numLines) && (mode != -1) && (offset < maxChars); i++) {
            switch (mode) {
                case 0: /* mode 0 = looking for matching profile */
                    if (strcmp(lines[i], sections[profile]) == 0) {
                        /* Found a match, start appending data */
                        mode = 1;
                    }
                    break;
                case 1:
                    if (strcmp(lines[i], "") == 0) {
                        /* Stop processing the data */
                        mode = -1;
                    } else {
                        /* Append a space if we added text */
                        if (offset) {
                            text[offset++] = ' ';
                            text[offset] = '\0';
                        }
                        /* How long is the new data? */
                        len = strlen(lines[i]);
                        /* Will this string fit? */
                        if ((len + offset) < maxChars) {
                            strcpy(text + offset, lines[i]);
                            offset += len;
                        } else {
                            /* ran out of room, stop here */
                            mode = -1;
                        }
                    }
                    break;
            }
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetRegulatoryDomain
 *---------------------------------------------------------------------------*
 * Description:
 *      Gets the current regulatory domain in use.
 *      Sends the command:
 *          AT+WREGDOMAIN=?
 *      and waits for a response.  The response is then parsed for errors
 *      and the regulatory domain used
 * Inputs:
 *      ATLIBGS_REGDOMAIN_E *regDomain -- Regulatory domain in use
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetRegulatoryDomain(ATLIBGS_REGDOMAIN_E *regDomain)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+WREGDOMAIN=?\r\n");
    *regDomain = ATLIBGS_REGDOMAIN_UNKNOWN;
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
        if (strcmp(lines[0], "REG_DOMAIN=FCC") == 0)
            *regDomain = ATLIBGS_REGDOMAIN_FCC;
        else if (strcmp(lines[0], "REG_DOMAIN=ETSI") == 0)
            *regDomain = ATLIBGS_REGDOMAIN_ETSI;
        else if (strncmp(lines[0], "REG_DOMAIN=TELEC", 16) == 0)
            *regDomain = ATLIBGS_REGDOMAIN_TELEC;
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetRegulatoryDomain
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the regulatory domain in use.
 *      Sends the command:
 *          AT+WREGDOMAIN=n
 *      where n is
 *          0 for FCC,
 *          1 for ETSI, or
 *          2 for TELEC
 *      and waits for a response.
 * Inputs:
 *      ATLIBGS_REGDOMAIN_E *regDomain -- Regulatory domain to use
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetRegulatoryDomain(ATLIBGS_REGDOMAIN_E regDomain)
{
    char cmd[30];
    sprintf(cmd, "AT+WREGDOMAIN=" _F8_ "\r\n", regDomain);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_NetworkScan
 *---------------------------------------------------------------------------*
 * Description:
 *      Scans for a list of networks.
 *      Sends the command:
 *          AT+WS=<SSID>,<channel>,<scantime>
 *      and waits for a response.
 *      The SSID, channel, and scantime values are optional.
 * Inputs:
 *      const char *SSID -- 0 for complete network list, or string for
 *          particular SSID.
 *      uint8_t channel -- Channel to perform scan on, or 0 for all channels
 *      uint16_t scantime -- Millisecond scan time
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_NetworkScan(
        const char *SSID,
        uint8_t channel,
        uint16_t scantime,
        ATLIBGS_NetworkScanEntry *entries,
        uint8_t maxEntries,
        uint8_t *numEntries)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[50];
    int numLines;
    char text[50];
    char *tokens[20];
    uint8_t numTokens;
    int i;
    ATLIBGS_NetworkScanEntry *entry = entries;
    char cmd[60];

    *numEntries = 0;

    strcpy(cmd, "AT+WS=");
    if (SSID != NULL)
        strcat(cmd, SSID);
    strcat(cmd, ",");
    if (channel) {
        sprintf(text, _F8_, channel);
        strcat(cmd, text);
    }
    strcat(cmd, ",");
    if (scantime != 0) {
        sprintf(text, _F8_, scantime);
        strcat(cmd, text);
    }
    strcat(cmd, "\r\n");
    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 50);
        /* Skip the first line and parse all the entries */
        for (i = 1; i < numLines; i++) {
            if (*numEntries >= maxEntries)
                break;
            numTokens = AtLibGs_ParseIntoTokens(lines[i], ',', tokens, 20);
            if (numTokens == 6) {
                /* Got a line, store it in the structure */
                strncpy(entry->bssid, tokens[0], ATLIBGS_BSSID_MAX_LENGTH);
                strncpy(entry->ssid, tokens[1], ATLIBGS_SSID_MAX_LENGTH);
                entry->channel = atoi(tokens[2]);
                if (strcmp(tokens[3], "ADHOC") == 0)
                    entry->station = ATLIBGS_STATIONMODE_AD_HOC;
                else
                    entry->station = ATLIBGS_STATIONMODE_INFRASTRUCTURE;
                entry->signal = atoi(tokens[4]);
                entry->security = AtLibGs_ParseSecurityMode(tokens[5]);
                entry++;
                (*numEntries)++;
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisAssoc2
 *---------------------------------------------------------------------------*
 * Description:
 *      Dissociate from the current network
 *      Sends the command:
 *          ATH
 * Inputs:
 *     None
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisAssoc2(void)
{
    return AtLibGs_CommandSendString("ATH\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetTransmitRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns the current wireless transmit rate
 *      Sends the command:
 *          AT+WRATE=?
 * Inputs:
 *     None
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetTransmitRate(void)
{
    return AtLibGs_CommandSendString("AT+WRATE=?\r\n");
    /* TODO: Need to parse wireless rate! */
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_GetClientInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the information about the clients associated to the adapter
 *       when it act as a Limited AP.
 *      Sends the command:
 *          AT+APCLIENTINFO=?
 *     example output
 *          No.Of Stations Connected=1
 *          No     MacAddr                   IP
 *          1      2c:44:01:c5:e7:df         192.168.1.2
 * Inputs:
 *     None
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetClientInfo(void)
{
    char *lines[3];
    ATLIBGS_MSG_ID_E rxMsgId;
    rxMsgId = AtLibGs_CommandSendString("AT+APCLIENTINFO=?\r\n");

    /* TODO: What is this to do? */
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 3) >= 1)) {
#ifdef ATLIBGS_DEBUG_ENABLE
        /* do something with the client info */
        ConsolePrintf("L1: %s\r\n", lines[0]);
        ConsolePrintf("L2: %s\r\n", lines[1]);
        ConsolePrintf("L3: %s\r\n", lines[2]);
#endif
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetAuthentictionMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the Authentication mode when using WEP, set to 0 for WPA/WPA2
 *      Sends the command:
 *          AT+WAUTH=N
 * Inputs:
 *     N=0(None/WPA), N=1(Open WEP), N=2(Shared WEP)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetAuthentictionMode(ATLIBGS_AUTHMODE_E authmode)
{
    char cmd[20];

    sprintf(cmd, "AT+WAUTH=" _F8_ "\r\n", authmode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWEPKey
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets WEP keys 1-4, key must be 10 or 26 HEX digits
 *      Sends the command:
 *          AT+WWEPn=<key> where <key> is the 10 or 26 digit WEP key
 * Inputs:
 *     n = 1-4, 10 or 26 HEX digit key (use NULL terminated char array)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWEPKey(ATLIBGS_WEPKEY_E keynum, char key[])
{
    char cmd[20];

    sprintf(cmd, "AT+WWEP" _F8_ "=%s\r\n", keynum, key);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetWPAKey
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the WPA2 pre-shared key to the <PSK>. After the PSK has been
 *      entered, the commands AT&W (to save the relevant profile) and AT&Y
 *      (to ensure that the profile containing the new PSK is the default
 *      profile) should be issued. The PSK will then be available when the
 *      adapter awakens from Standby. Refer to sections 0 and 4.6.3 for more
 *      information on profile management.
 *      Sends the command:
 *          AT+WPSK=<key> where <key> is the 32 byte NULL terminated key
 * Inputs:
 *       32 byte NULL terminated char array
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetWPAKey(char key[])
{
    char cmd[20];
    sprintf(cmd, "AT+WPSK=%s\r\n", key);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetListenBeaconInterval
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the Listen Beacon Interval
 *      Sends the command:
 *        AT+WIEEEPSPOLL=n[,I]: n is 0 or 1, and I is a 16 bit int iff n == 1
 * Inputs:
 *     n, 16bit unsigned int
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetListenBeaconInterval(bool enable, uint16_t interval)
{
    char cmd[30];
    if (enable) {
        sprintf(cmd, "AT+WIEEEPSPOLL=1," _F16_ "\r\n", interval);
    } else {
        sprintf(cmd, "AT+WIEEEPSPOLL=0\r\n");
    }
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTransmitRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the Transmit rate
 *      Sends the command:
 *          AT+WRATE=n, where n is the integer corresponding to the rate
 * Inputs:
 *     8bit unsigned int [0(Auto),2(1mbps),4(2mbps),11(5.5mbps),22(11mbps)]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitRate(ATLIBGS_TRANSRATE_E transrate)
{
    char cmd[20];
    sprintf(cmd, "AT+WRATE=" _F8_ "\r\n", transrate);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableDHCPServer
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables the DHCP server
 *      Sends the command:
 *          AT+DHCPSRVR=1
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDHCPServer(void)
{
    return AtLibGs_CommandSendString("AT+DHCPSRVR=1\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisableDHCPServer
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables the DHCP server
 *      Sends the command:
 *          AT+DHCPSRVR=0
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisableDHCPServer(void)
{
    return AtLibGs_CommandSendString("AT+DHCPSRVR=0\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DisableDNSServer
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables the DNS server
 *      Sends the command:
 *          AT+DNS=0
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DisableDNSServer()
{
    return AtLibGs_CommandSendString("AT+DNS=0\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableDNSServer
 *---------------------------------------------------------------------------*
 * Description:
 *      Disables the DNS server
 *      Sends the command:
 *          AT+DNS=1,<server> where <server> is the NULL terminated server
 * Inputs:
 *      NULL terminated char array
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableDNSServer(char server[])
{
    char cmd[50];

    sprintf(cmd, "AT+DNS=1,%s\r\n", server);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEAPConfiguration
 *---------------------------------------------------------------------------*
 * Description:
 *    Configures EAP security
 *    Sends the command:
 *     AT+WEAPCONF=<Outer Authentication>,<Inner Authentication>,<user>,<pass>
 * Inputs:
 *      Outer Auth, Inner Auth, username, password (both 1-32 char ASCII str)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEAPConfiguration(
        ATLIBGS_INOUAUTH_E oa,
        ATLIBGS_INOUAUTH_E ia,
        char user[],
        char pass[])
{
    char cmd[80];

    sprintf(cmd, "AT+WEAPCONF=" _F8_ "," _F8_ ",%s,%s\r\n", oa, ia, user, pass);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_AddSSLCertificate
 *---------------------------------------------------------------------------*
 * Description:
 *    Adds an SSL certificate, with ASCII name, bin/hex format, size, and
 *    location (flash/ram). To add the certificate you need to...
 *    Sends the command:
 *     AT+TCERTADD=<name>,<Inner Authentication>,<user>,<pass>
 * Inputs:
 *      name (char ASCII str), binary(0)/hex(1), 16bit uint size, flash(0)/ram(1)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_AddSSLCertificate(
        char name[],
        bool hex,
        uint16_t size,
        bool ram)
{
    char cmd[80];

    sprintf(cmd, "AT+TCERTADD=%s," _F8_ "," _F16_ "," _F8_ "\r\n", name, hex,
            size, ram);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_DeleteSSLCertificate
 *---------------------------------------------------------------------------*
 * Description:
 *    Deletes named SSL Certificate
 *    Sends the command:
 *     AT+TCERTDEL=<name>
 * Inputs:
 *      name (char ASCII str)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_DeleteSSLCertificate(char name[])
{
    char cmd[50];

    /* TODO: Does this require a comma? */
    sprintf(cmd, "AT+TCERTDEL=%s,\r\n", name);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetSecurity
 *---------------------------------------------------------------------------*
 * Description:
 *    Sets the Security mode
 *    Sends the command:
 *     AT+WSEC=n
*                  0       Auto
 *                  1       Open
 *                  2       Wep
 *                  4       WPA
 *                  8       WPA2
 *                  16      WPA  - Enterprise
 *                  32      WPA2 - Enterprise
 *                  64      WPA2 - acs +tkip security
 *      and waits for a response.
 * Inputs:
 *      Security mode (see ATLIBGS_SECURITYMODE_E for values)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetSecurity(ATLIBGS_SECURITYMODE_E security)
{
    char cmd[20];

    sprintf(cmd, "AT+WSEC=" _F16_ "\r\n", security);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SSLOpen
 *---------------------------------------------------------------------------*
 * Description:
 *    Opens SSL Connection
 *    Sends the command:
 *     AT+SSLOPEN=<cid>,<cert name>
 * Inputs:
 *      cid, certificate name (from AtLibGs_AddSSLCertificate)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SSLOpen(uint8_t cid, char name[])
{
    char cmd[50];

    sprintf(cmd, "AT+SSLOPEN=" _F8_ ",%s\r\n", cid, name);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SSLClose
 *---------------------------------------------------------------------------*
 * Description:
 *    Closes SSL Connection
 *    Sends the command:
 *     AT+SSLCLOSE=<cid>,<cert name>
 * Inputs:
 *      cid
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SSLClose(uint8_t cid)
{
    char cmd[20];

    sprintf(cmd, "AT+SSLCLOSE=" _F16_ "\r\n", cid);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPConf
 *---------------------------------------------------------------------------*
 * Description:
 *    Configures HTTP client
 *    Sends the command:
 *     AT+HTTPCONF=<Param>,<Value>
 * Inputs:
 *      HTTP Params, char string
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPConf(ATLIBGS_HTTPCLIENT_E param, char value[])
{
    char cmd[30];

    sprintf(cmd, "AT+HTTPCONF=" _F16_ ",", param);
    App_Write(cmd, strlen(cmd));
    App_Write(value, strlen(value));
    App_Write("\r\n", 2);
#ifdef ATLIBGS_DEBUG_ENABLE
    ConsolePrintf(">%s" _F16_ ",%s\n", cmd, param, value);
#endif
    /* Wait for the response while collecting data into the MRBuffer */
    return AtLibGs_ResponseHandle();
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPConfDel
 *---------------------------------------------------------------------------*
 * Description:
 *    Deletes config param from HTTP client
 *    Sends the command:
 *     AT+HTTPCONFDEL=<Param>
 * Inputs:
 *      HTTP Params
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPConfDel(ATLIBGS_HTTPCLIENT_E param)
{
    char cmd[30];

    sprintf(cmd, "AT+HTTPCONFDEL=" _F16_ "\r\n", param);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPClose
 *---------------------------------------------------------------------------*
 * Description:
 *    Closes the HTTP Client connection
 *    Sends the command:
 *     AT+HTTPCLOSE=<cid>
 * Inputs:
 *      cid
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPClose(uint8_t cid)
{
    char cmd[30];

    /* TODO: Is CID a character or a number? */
    sprintf(cmd, "AT+HTTPCLOSE=" _F8_ "\r\n", cid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RawETHFrameConf
 *---------------------------------------------------------------------------*
 * Description:
 *    Delet
 *    Sends the command:
 *     AT+NRAW=n
 * Inputs:
 *      Raw ethernet frame enable flag
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RawETHFrameConf(ATLIBGS_RAW_ETH_E enable)
{
    char cmd[20];

    sprintf(cmd, "AT+NRAW=" _F8_ "\r\n", enable);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BulkDataTrans
 *---------------------------------------------------------------------------*
 * Description:
 *    Enables/Disables bulk transfer mode
 *    Sends the command:
 *     AT+BDATA=n
 * Inputs:
 *      0 (disable)/ 1 (enable)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BulkDataTrans(bool enable)
{
    char cmd[20];

    sprintf(cmd, "AT+BDATA=" _F8_ "\r\n", enable);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetBatteryCheckFreq
 *---------------------------------------------------------------------------*
 * Description:
 *    Sets the battery check freq
 *    Sends the command:
 *     AT+BCHK=n
 * Inputs:
 *      uint16_t freq -- Number of seconds between battery readings
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetBatteryCheckFreq(uint16_t freq)
{
    char cmd[20];

    sprintf(cmd, "AT+BCHK=" _F16_ "\r\n", freq);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetBatteryCheckFreq
 *---------------------------------------------------------------------------*
 * Description:
 *    Sets the battery check freq
 *    Sends the command:
 *     AT+BCHK=n
 * Inputs:
 *      uint16_t *freq -- Number of seconds between battery readings
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryCheckFreq(uint16_t *freq)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[1];

    rxMsgId = AtLibGs_CommandSendString("AT+BCHK=?\r\n");

    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        if (AtLibGs_ParseIntoLines(MRBuffer, lines, 1) >= 1) {
            *freq = atoi(lines[0]);
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatteryValGet
 *---------------------------------------------------------------------------*
 * Description:
 *    Sets the battery check freq
 *    Sends the command:
 *     AT+BATTVALGET
 *    This command should return a message with the latest value,
 *    e.g. Battery Value: 3.4 V, followed by the standard command response.
 *    If this command is issued before issuing the command to start battery
 *    checks, it returns ERROR or 1, depending on the current verbose setting.
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetBatteryValue(void)
{
    /* TODO: Need to return full value */
    return AtLibGs_CommandSendString("AT+BATTVALGET\r\n");
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_EnterDeepSleep
 *---------------------------------------------------------------------------*
 * Description:
 *    Enters Deep sleep mode for givin amount of milliseconds
 *    Sends the command:
 *     AT+PSDPSLEEP=<milliseconds>
 * Inputs:
 *      milliseconds
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnterDeepSleep(uint32_t milliseconds)
{
    char cmd[30];
    /* TODO: Does this really take a number of miliseconds? */
    sprintf(cmd, "AT+PSDPSLEEP=" _F32_ "\r\n", milliseconds);
    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_AssocPowerSaveMode
 *---------------------------------------------------------------------------*
 * Description:
 *    Configure 802.11 Power Save Mode to be used during the association.
 *    Sends the command:
 *     AT+WAPSM=<value>
 * Inputs:
 *      ATLIBGS_ASSOCPOWMODE_E (modes 0-3)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_AssocPowerSaveMode(ATLIBGS_ASSOCPOWMODE_E mode)
{
    char cmd[20];

    sprintf(cmd, "AT+WAPSM=" _F8_ "\r\n", mode);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetTransmitPower
 *---------------------------------------------------------------------------*
 * Description:
 *    Sets the internal/external transmit power level
 *    Sends the command:
 *       AT+WP=<value>
 * Inputs:
 *      power level (int 0-7, ext 2-15)**Lower number == higher PWR**
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetTransmitPower(uint8_t power)
{
    char cmd[20];

    sprintf(cmd, "AT+WP=" _F8_ "\r\n", power);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartAutoConnect
 *---------------------------------------------------------------------------*
 * Description:
 *    Start Auto Connect, including association.
 *    Sends the command:
 *       ATA
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATA\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ConfigAPAutoConnect
 *---------------------------------------------------------------------------*
 * Description:
 *    Start Auto Connect using existing association.
 *    Sends the command:
 *     ATA2
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ConfigAPAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATA2\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ReturnAutoConnect
 *---------------------------------------------------------------------------*
 * Description:
 *    Return to a previous Auto Connect session; returns an error if no such
 *    session exists.
 *    Sends the command:
 *     ATO
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_ReturnAutoConnect(void)
{
    return AtLibGs_CommandSendString("ATO\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WebProv
 *---------------------------------------------------------------------------*
 * Description:
 *    Provisioning through web pages
 *    Sends the command:
 *     AT+WEBPROV=
 * Inputs:
 *    username, password (NULL terminated char array, 16 char limit)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WebProv(char username[], char password[])
{
    char cmd[80];

    sprintf(cmd, "AT+WEBPROV=%s,%s\r\n", username, password);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WebLogoAdd
 *---------------------------------------------------------------------------*
 * Description:
 *    The adapter supports adding the Logo that will appear on the web pages
 *      used for provisioning. <size> is measured in bytes and the maximum size
 *      is 1788 bytes. This command is typically done at the manufacturing line
 *      in the factory. This command can be done only once. There is no command
 *      to delete the Logo. This command returns standard command response
 *      (section 4) to the serial interface.
 *    Sends the command:
 *     AT+WEBLOGOADD=<size>
 *           <Esc>L<Actual File content>
 * Inputs:
 *      size
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WebLogoAdd(uint16_t size)
{
    char cmd[30];

    if (0) {
        sprintf(cmd, "AT+WEBLOGOADD=" _F16_ "\r\n", size);
        return AtLibGs_CommandSendString(cmd);
        /* TODO: Need to send file contents! */
    } else
        /* normally dissabled since can only run once */
        return ATLIBGS_MSG_ID_OK;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFAsyncFrameRX
 *---------------------------------------------------------------------------*
 * Description:
 *     Enable the asynchronous frame reception
 *    Sends the command:
 *     AT+RFRXSTART=<channel>
 * Inputs:
 *      channel
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFAsyncFrameRX(uint16_t channel)
{
    char cmd[20];

    sprintf(cmd, "AT+RFRXSTART=" _F16_ "\r\n", channel);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFStop
 *---------------------------------------------------------------------------*
 * Description:
 *     Stop any of the RF tests transmission/reception
 *    Sends the command:
 *     AT+RFSTOP
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFStop(void)
{
    char cmd[30];

    sprintf(cmd, "AT+RFSTOP\r\n");

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_SPIConf
 *---------------------------------------------------------------------------*
 * Description:
 *     The command to set the SPI clock phase and clock polarity parameter.
 *      If clock polarity is 0, then inactive state of serial clock is low.
 *      If clock polarity is 1, then inactive state of serial clock is high.
 *      If clock phase is 0, then data is captured on the first toggling edge of the
 *      serial clock (clock phase zero), after the falling edge of slave select signal.
 *      If clock phase is 1, then data is captured on the second edge of the serial
 *      clock (clock phase 180), after the falling edge of slave select signal.
 *      Default is clock polarity 0 and clock phase 0. The new SPI parameters take
 *      effect after node reset/restart. However, they are stored in RAM and will be
 *      lost when power is lost unless they are saved to a profile using AT&W
 *      (section 4.6.1). The profile used in that command must also be set as the
 *      power-on profile using AT&Y (section 4.6.3).
 *    Sends the command:
 *     AT+SPICONF=<polarity>,<phase>
 * Inputs:
 *      ATLIBGS_SPI_POLARITY_E polarity -- polarity of SPI transmissions
 *      ATLIBGS_SPI_PHASE_E phase -- phase of SPI transmissions
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SPIConf(
        ATLIBGS_SPI_POLARITY_E polarity,
        ATLIBGS_SPI_PHASE_E phase)
{
    char cmd[30];

    sprintf(cmd, "AT+SPICONF=" _F8_ "," _F8_ "\r\n", polarity, phase);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Resets the module
 *      Sends the command:
 *          AT+RESET
 * Inputs:
 *      void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Reset(void)
{
    return AtLibGs_CommandSendString("AT+RESET\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_MemoryTrace
 *---------------------------------------------------------------------------*
 * Description:
 *     Sends memory trace to serial interface
 *    Sends the command:
 *     AT+MEMTRACE
 * Inputs:
 *      ? (does't appear to work at all)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_MemoryTrace(void)
{
    /* TODO: Can we parse this? */
    return AtLibGs_CommandSendString("AT+MEMTRACE\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartFirmwareUpdate
 *---------------------------------------------------------------------------*
 * Description:
 *     Starts the firmware update on either the APP FWs, the WLAN FW, or all 3
 *    Sends the command:
 *     AT+SOTAFWUPSTART=<firmwaretoupdate>
 * Inputs:
 *      ATLIBGS_FIRMWARE_BINARIES_E bins --
 *      enum to set which firmwares are being updated
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartFirmwareUpdate(ATLIBGS_FIRMWARE_BINARIES_E bins)
{
    char cmd[30];

    /* TODO: The flags were not sent in the previous version, should they? */
    sprintf(cmd, "AT+SOTAFWUPSTART=" _F16_ "\r\n", bins);

    return AtLibGs_CommandSendString("AT+SOTAFWUPSTART=\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetTime
 *---------------------------------------------------------------------------*
 * Description:
 *     Gets the system time. shown formatted<dd/mm/yyyy>,<HH:MM:SS>,
 *     followed by system time in milliseconds since epoch(1970)
 *    Sends the command:
 *      AT+GETTIME=?
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetTime(void)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+GETTIME=?\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
        /* TODO: Parse the time! */
        //ConsolePrintf("Time: %s\r\n", lines[0]);
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetGPIO
 *---------------------------------------------------------------------------*
 * Description:
 *     Sets 1 of 4 GPIOs high or low, unless it is being used (for SPI/UART)
 *    Sends the command:
 *     AT+DGPIO=<GPIO number>,0/1
 * Inputs:
 *      GPIO number, high/low (1/0) setting
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetGPIO(
        ATLIBGS_GPIO_PIN_E gpio,
        ATLIBGS_GPIO_STATE_E state)
{
    char cmd[20];

    sprintf(cmd, "AT+DGPIO=" _F16_ "," _F8_ "\r\n", gpio, state);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableEnhancedAsync
 *---------------------------------------------------------------------------*
 * Description:
 *     Sets enhanced asynchronous notifications on if true.
 *    Sends the command:
 *     AT+ASYNCMSGFMT=n, where n is 0/1
 *     NOTE: THe AtLibGs library does not directly support enhanced
 *      asynchronous notifications.
 * Inputs:
 *      bool on -- If true then enable, else disable.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableEnhancedAsync(bool on)
{
    char cmd[20];

    sprintf(cmd, "AT+ASYNCMSGFMT=" _F8_ "\r\n", (on) ? 1 : 0);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_EnableVerbose
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets verbose on or off
 *      Sends the command:
 *        ATVn, where n is 0/1
 * Inputs:
 *      bool on -- If true then enable, else disable.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_EnableVerbose(bool on)
{
    char cmd[20];

    sprintf(cmd, "ATV" _F8_ "\r\n", (on) ? 1 : 0);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetNetworkConfiguration
 *---------------------------------------------------------------------------*
 * Description:
 *     Gets the current network configuration status
 *    Sends the command:
 *      AT+WSTATUS
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkConfiguration(void)
{
    char *lines[2];
    ATLIBGS_MSG_ID_E rxMsgId;

    rxMsgId = AtLibGs_CommandSendString("AT+WSTATUS\r\n");
    if ((rxMsgId == ATLIBGS_MSG_ID_OK) && (AtLibGs_ParseIntoLines(MRBuffer,
            lines, 2) >= 1)) {
#ifdef ATLIBGS_DEBUG_ENABLE
        /* do something with the status */
        ConsolePrintf("Network Config Status: %s\r\n", lines[0]);
        /* TODO: Parse this response! */
#endif
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_FWUpgradeConfig
 *---------------------------------------------------------------------------*
 * Description:
 *    Configures parameters for FW update
 *    Sends the command:
 *       AT+SOTAFWUPCONF=<param>,<value>
 * Inputs:
 *      param, value(string)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_FWUpgradeConfig(
        ATLIBGS_FWUPGRADEPARAM_E param,
        char value[])
{
    char cmd[80];

    sprintf(cmd, "AT+SOTAFWUPCONF=" _F8_ ",%s\r\n", param, value);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetCIDInfo
 *---------------------------------------------------------------------------*
 * Description:
 *     Returns list of CIDs configured
 *    Sends the command:
 *     AT+CID=?
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetCIDInfo(void)
{
    /* TODO: Parse CID response! */
    return AtLibGs_CommandSendString("AT+CID=?\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_BatterySetWarnLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the battery warning/standby level to enable the adaptor’s
 *      internal battery measuring logic
 *    Sends the command:
 *     AT+BATTLVLSET=
 * Inputs:
 *     uint16_t level -- warning level (mV)
 *     uint16_t freq -- warning freq
 *     uint16_t standby -- standby level (mV)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_BatterySetWarnLevel(
        uint16_t level,
        uint16_t freq,
        uint16_t standby)
{
    char cmd[30];

    sprintf(cmd, "AT+BATTLVLSET=" _F16_ "," _F16_ "," _F16_ "\r\n", level,
            freq, standby);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetEAPCertificate
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure certificate for EAP-TLS
 *      Sends the command:
 *         AT+WEAP=< Type >,< Format >,< Size >,< Location >
 * Inputs:
 *      CA certificate(0)/ Client certificate(1)/ Private Key(2),
 *          Binary(0)/Hex(1), size of the file to be transferred, Flash(0)/Ram(1)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetEAPCertificate(
        ATLIBGS_EAPTLS_E cert,
        ATLIBGS_EAPTLS_E binhex,
        uint16_t size,
        ATLIBGS_EAPTLS_E flashram)
{
    char cmd[50];

    sprintf(cmd, "AT+WEAP=" _F8_ "," _F8_ "," _F16_ "," _F8_ "\r\n", cert,
            binhex, size, flashram);

    return AtLibGs_CommandSendString(cmd);
    /* TODO: Send certificate <ESC> W sequence after this! */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_Ping
 *---------------------------------------------------------------------------*
 * Description:
 *    Sends a ping (section 4.16.13 in programmer guide)
 *    Sends the command:
 *       AT+PING=<Ip>,[[<Trails>],[<timeout>],[<Len>],[<TOS>],[<TTL>],[<PAYLOAD>]]
 *    Example output:
 *      Pinging for 192.168.0.100 with 56 bytes of data
 *      OK
 *      Reply from 192.168.0.100: bytes=56 time=118 ms TTL 30
 *      Ping Statistics for 192.168.0.100:
 *           Packets: Sent = 1, Received = 1, Lost = 0 percent
 *      Approximate round trip times in milliseconds
 *           Minimum = 2ms, Maximum = 118ms, Average = 26ms
 * Inputs:
 *      <Ip>,[[<Trails>],[<timeout>],[<Len>],[<TOS>],[<TTL>],[<PAYLOAD>]]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_Ping(
        char ip[],
        uint16_t trails,
        uint16_t timeout,
        uint16_t len,
        uint8_t tos,
        uint8_t ttl,
        char payload[])
{
    char cmd[80];

    sprintf(cmd,
            "AT+PING=%s" _F16_ "," _F16_ "," _F16_ "," _F8_ "," _F8_ ",%s\r\n",
            ip, trails, timeout, len, tos, ttl, payload);

    return AtLibGs_CommandSendString(cmd);
    /* TODO: Need some type of callback per ping or at least the summary */
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_TraceRoute
 *---------------------------------------------------------------------------*
 * Description:
 *     Starts a trace route sequence
 *     Sends the command:
 *        AT+TRACEROUTE=<Ip>,[[<Interval>],[<MaxHops>],[<MinHops>],[<TOS>]]
 * Inputs:
 *      <Ip>,[[<Interval>],[<MaxHops>],[<MinHops>],[<TOS>]]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_TraceRoute(
        char ip[],
        uint16_t interval,
        uint8_t maxhops,
        uint8_t minhops,
        uint8_t tos)
{
    char cmd[80];

    sprintf(cmd, "AT+TRACEROUTE=%s" _F16_ "," _F8_ "," _F8_ "," _F8_ "\r\n",
            ip, interval, maxhops, minhops, tos);
    return AtLibGs_CommandSendString(cmd);
    /* TODO: Need some type of callback per traceroute or at least the summary */
}

/*---------------------------------------------------------------------------*
 * TODO: Routine:  AtLibGs_SNTPsync
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets adapter time using namer server
 *    Sends the command:
 *     AT+NTIMESYNC= <Enable>,<Server IP>,<Timeout>,<Periodic>,[<frequency>]
 * Inputs:
 *      <Enable>,<Server IP>,<Timeout>,<Periodic>,[<frequency>]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SNTPsync(
        bool enable,
        char ip[],
        uint8_t timeout,
        bool periodic,
        uint32_t frequency)
{
    char cmd[30];

    sprintf(cmd, "AT+NTIMESYNC=" _F8_ ",%s," _F8_ "," _F8_ "," _F32_ "\r\n",
            enable, ip, timeout, periodic, frequency);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetSocketOptions
 *---------------------------------------------------------------------------*
 * Description:
 *      Configures a socket by CID
 *    Sends the command:
 *     AT+SETSOCKOPT=<CID>,<Type>,<Parameter>,<Value>,<Length>
 * Inputs:
 *      ATLIBGS_SOCKET_OPTION_TYPE_E type -- Type of socket option
 *          (socket, IP, TCP)
 *      ATLIBGS_SOCKET_OPTION_PARAM_E param -- Parameter type being set
 *          (TCP MAXRT, TCP_KA, SO_KA, TCO_KA_CNT)
 *      uint32_t value -- Value to be set in seconds
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetSocketOptions(
        uint8_t cid,
        ATLIBGS_SOCKET_OPTION_TYPE_E type,
        ATLIBGS_SOCKET_OPTION_PARAM_E param,
        uint32_t value)
{
    char cmd[80];
    const uint16_t length = 4;

    sprintf(
            cmd,
            "AT+SETSOCKOPT=" _F8_ "," _F16_ "," _F16_ "," _F32_ "," _F16_ "\r\n",
            cid, type, param, value, length);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPOpen
 *---------------------------------------------------------------------------*
 * Description:
 *      Open and HTTP client connection
 *      Sends the command:
 *          AT+HTTPOPEN=<host >[, <Port Number>, <SSL Flag>, <certificate name>,
 *          <proxy>,<Connection Timeout>]
 *     returns cid of http client on success
 * Inputs:
 *      char host[] -- Host address to connect to
 *      uint16_t port -- Port on host to connect to
 *      bool sslenabled -- true if using a SSL certificate
 *      char certname[] -- name of certificate (if any)
 *      char proxy[] -- name of proxy (if any)
 *      uint32_t timeout -- Amount of time to try opening (in seconds)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPOpen(
        char host[],
        uint16_t port,
        bool sslenabled,
        char certname[],
        char proxy[],
        uint16_t timeout,
        uint8_t *cid)
{
    char cmd[80];
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[4];
    uint8_t numLines;

    sprintf(cmd, "AT+HTTPOPEN=%s," _F16_ "," _F8_ ",%s,%s," _F16_ "\r\n", host,
            port, (sslenabled) ? 1 : 0, certname, proxy, timeout);

    rxMsgId = AtLibGs_CommandSendString(cmd);
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 4);
        if (numLines >= 2) {
            // Convert the returned CID into a number
            *cid = atoi(lines[1]);
        }
    }
    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_HTTPSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Get/post data on HTTP client connection
 *      Sends the command:
 *          AT+HTTPSEND=<CID>,<Type>,<Timeout>,<Page>[,Size of the content]
 * Inputs:
 *      <CID>,<Type>,<Timeout>,<Page>[,Size of the content]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_HTTPSend(
        uint8_t cid,
        ATLIBGS_HTTPSEND_E type,
        uint16_t timeout,
        char page[],
        uint16_t size,
        const void *txBuf)
{
    char cmd[50];
    ATLIBGS_MSG_ID_E msg = ATLIBGS_MSG_ID_INVALID_INPUT;
    if (ATLIBGS_INVALID_CID != cid) {
#ifdef ATLIBGS_DEBUG_ENABLE
        ConsolePrintf(">AT+HTTPSEND=" _F8_ "," _F8_ "," _F16_ ",", cid, type,
                timeout);
        ConsolePrintf("%s," _F16_ "\r\n", page, size);
#endif
        sprintf(cmd, "AT+HTTPSEND=" _F8_ "," _F8_ "," _F16_ ",", cid, type,
                timeout);
        App_Write(cmd, strlen(cmd));
        App_Write(page, strlen(page));
        sprintf(cmd, "," _F16_ "\r\n", size);
        App_Write(cmd, strlen(cmd));
        msg = AtLibGs_ResponseHandle();
        if (msg == ATLIBGS_MSG_ID_OK) {
            /* Construct the data start indication message */
            sprintf(cmd, "%c%c" _F8_, ATLIBGS_ESC_CHAR, 'H', cid);
            /* Now send the data START indication message  to S2w node */
            App_Write(cmd, 3);
            /* Now send the actual data */
            AtLibGs_DataSend(txBuf, size);
        }
    }
    return msg;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_UnsolicitedTXRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Enables unsolicited data transmission
 *    Sends the command:
 *     AT+UNSOLICITEDTX=<Frame Control>,<Sequence Control>,<Channel>,<Rate>,<WmmInfo>,
 *             <Receiver Mac>,<Bssid of AP>,<Frame Length>
 * Inputs:
 *      <Frame Control>,<Sequence Control>,<Channel>,<Rate>,<WmmInfo>,
 *          <Receiver Mac>,<Bssid of AP>,<Frame Length>
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_UnsolicitedTXRate(
        uint16_t frame,
        uint16_t seq,
        uint8_t chan,
        ATLIBGS_UNSOLICITEDTX_E rate,
        uint32_t wmm,
        char mac[],
        char bssid[],
        uint16_t len)
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+UNSOLICITEDTX=" _F16_ "," _F16_ "," _F8_ "," _F8_ "," _F32_ ",%s,%s," _F16_ "\r\n",
            frame, seq, chan, rate, wmm, mac, bssid, len);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFAsyncFrameTX
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable asynch frame transmission
 *    Sends the command:
 *     AT+RFFRAMETXSTART=<Channel>,<Power>,<Rate>,<No.Of.Times>,<Fr.Intrvel>,<FrameControl>,
 * <DurationId>,<Sequence Control>,<frameLen>,<Preamble>,<Scrambler>[,<DstMac>,<Src Mac>]
 * Inputs:
 *      <Channel>,<Power>,<Rate>,<No.Of.Times>,<Fr.Intrvel>,<FrameControl>,
 * <DurationId>,<Sequence Control>,<frameLen>,<Preamble>,<Scrambler>[,<DstMac>,<Src Mac>]
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFAsyncFrameTX(
        uint8_t channel,
        uint8_t power,
        ATLIBGS_RFFRAMETXSTART_E rate,
        uint16_t times,
        uint16_t interval,
        uint8_t frameControl,
        uint16_t durationId,
        uint16_t sequenceControl,
        uint16_t frameLen,
        bool shortPreamble,
        bool scramblerOff,
        char dstMac[],
        char srcMac[])
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+RFFRAMETXSTART=" _F8_ "," _F8_ "," _F8_ "," _F16_ "," _F16_ "," _F8_ "," _F16_ "," _F16_ "," _F16_ "," _F8_ "," _F8_ ",%s,%s\r\n",
            channel, power, rate, times, interval, frameControl, durationId,
            sequenceControl, frameLen, shortPreamble, scramblerOff, dstMac,
            srcMac);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RFWaveTXStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable modulated/unmodulated wave transmission
 *    Sends the command:
 *     AT+RFWAVETXSTART=<Modulated>,<Channel>,<Rate>,<PreambleLong>,<ScamblerOff>,
 *     <Cont.Tx>,<Power>,<Ssid>
 * Inputs:
 *      <Modulated>,<Channel>,<Rate>,<PreambleLong>,<ScamblerOff>,<Cont.Tx>,
 *     <Power>,<Ssid>
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RFWaveTXStart(
        bool isModulated,
        uint16_t channel,
        ATLIBGS_RFWAVETXSTART_E rate,
        bool longPreamble,
        bool scramblerOn,
        bool continousWaveRX,
        uint8_t power,
        char ssid[])
{
    char cmd[120];

    sprintf(
            cmd,
            "AT+RFWAVETXSTART=" _F8_ "," _F16_ "," _F8_ "," _F8_ "," _F8_ "," _F8_ "," _F8_ "%s\r\n",
            isModulated, channel, rate, longPreamble, scramblerOn,
            continousWaveRX, power, ssid);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetNetworkStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the network status and return in a structure
 * Inputs:
 *      ATLIBGS_NetworkStatus *pStatus -- Pointer to structure status to fill.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetNetworkStatus(ATLIBGS_NetworkStatus *pStatus)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char *lines[10];
    uint8_t numLines;
    char *tokens[10];
    uint8_t numTokens;
    char *values[2];
    uint8_t numValues;
    uint8_t i, t;
    uint8_t rx = 0;

    memset(pStatus, 0, sizeof(*pStatus));
    rxMsgId = AtLibGs_CommandSendString("AT+NSTAT=?\r\n");
    if (rxMsgId == ATLIBGS_MSG_ID_OK) {
        numLines = AtLibGs_ParseIntoLines(MRBuffer, lines, 10);
        for (i = 0; i < numLines; i++) {
            numTokens = AtLibGs_ParseIntoTokens(lines[i], ' ', tokens, 10);
            for (t = 0; t < numTokens; t++) {
                numValues = AtLibGs_ParseIntoTokens(tokens[t], '=', values, 2);
                if (numValues == 2) {
                    if (strcmp(values[0], "MAC") == 0) {
                        strcpy(pStatus->mac, values[1]);
                    } else if (strcmp(tokens[t], "WSTATE") == 0) {
                        if (strcmp(values[1], "CONNECTED") == 0)
                            pStatus->connected = 1;
                    } else if (strcmp(values[0], "BSSID") == 0) {
                        strcpy(pStatus->bssid, values[1]);
                    } else if (strcmp(values[0], "SSID") == 0) {
                        strncpy(pStatus->ssid, values[1] + 1, strlen(values[1])
                                - 2);
                    } else if (strcmp(values[0], "CHANNEL") == 0) {
                        pStatus->channel = atoi(values[1]);
                    } else if (strcmp(values[0], "RSSI") == 0) {
                        pStatus->signal = atoi(values[1]);
                    } else if (strcmp(values[0], "SECURITY") == 0) {
                        pStatus->security
                                = AtLibGs_ParseSecurityMode(values[1]);
                    } else if (strcmp(values[0], /* IP */"addr") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->addr);
                    } else if (strcmp(values[0], "SubNet") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->subnet);
                    } else if (strcmp(values[0], "Gateway") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->gateway);
                    } else if (strcmp(values[0], "DNS1") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->dns1);
                    } else if (strcmp(values[0], "DNS2") == 0) {
                        AtLibGs_ParseIPAddress(values[1], &pStatus->dns2);
                    } else if (strcmp(values[0], "Count") == 0) {
                        if (rx) {
                            pStatus->rxCount = atoi(values[1]);
                        } else {
                            pStatus->txCount = atoi(values[1]);
                        }
                    }
                } else if (numValues == 1) {
                    if (strcmp(values[0], "Rx") == 0) {
                        rx = 1;
                    } else if (strcmp(values[0], "Tx") == 0) {
                        rx = 0;
                    }
                }
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseSecurityMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert a security mode string into a security enumerated type.
 * Inputs:
 *      const char *string -- String with security mode
 * Outputs:
 *      ATLIBGS_SECURITYMODE_E -- Final generated error code
 *---------------------------------------------------------------------------*/
ATLIBGS_SECURITYMODE_E AtLibGs_ParseSecurityMode(const char *string)
{
    if (strcmp(string, "WPA2-PERSONAL") == 0) {
        return ATLIBGS_SMWPA2PSK;
    } else if (strcmp(string, "WPA-PERSONAL") == 0) {
        return ATLIBGS_SMWPAPSK;
    } else if (strcmp(string, "WPA-ENTERPRISE") == 0) {
        return ATLIBGS_SMWPAE;
    } else if (strcmp(string, "WPA2-ENTERPRISE") == 0) {
        return ATLIBGS_SMWPA2E;
    } else if (strncmp(string, "WEP", 3) == 0) {
        return ATLIBGS_SMWEP;
    } else if (strcmp(string, "NONE") == 0) {
        return ATLIBGS_SMOPEN;
    }
    return ATLIBGS_SM_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_ParseIPAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Take the given string and parse into ip numbers
 * Inputs:
 *      const char *string -- String with IP number
 *      ATLIBGS_IP *ip -- Structure to receive parsed IP number
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void AtLibGs_ParseIPAddress(const char *string, ATLIBGS_IP *ip)
{
    int v1, v2, v3, v4;

    /* Currently only parses ipv4 addresses */
    sscanf(string, _F8_ "." _F8_ "." _F8_ "." _F8_, &v1, &v2, &v3, &v4);
    ip->ipv4[0] = v1;
    ip->ipv4[1] = v2;
    ip->ipv4[2] = v3;
    ip->ipv4[3] = v4;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetWebProvSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for web provisioning settings.
 * Inputs:
 *      ATLIBGS_WEB_PROV_SETTINGS *wp -- Structure to receive web provision
 *          response settings.
 *      uint32_t timeout -- Maximum time to wait in milliseconds until
 *          complete, or 0 for no timeout
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_GetWebProvSettings(
        ATLIBGS_WEB_PROV_SETTINGS *wp,
        uint32_t timeout)
{
    ATLIBGS_MSG_ID_E rxMsgId;
    char line[80 + 1];
    uint16_t len;
    uint8_t c;
    char *p;
    bool done;
    uint32_t start = MSTimerGet();

    /* Example response (SPI style): */
    /*    SSID=FDIOutsideG */
    /*    CHNL=11 */
    /*    CONN_TYPE=0 */
    /*    MODE=0 */
    /*    SECURITY=3 */
    /*    PSK_PASS_PHRASE=FDIFDIFDI */
    /*    DHCP_ENBL=0 */
    /*    STATIC_IP=192.168.25.100 */
    /*    SUBNT_MASK=192.168.25.101 */
    /*    GATEWAY_IP=192.168.25.102 */
    /*    AUTO_DNS_ENBL=0 */
    /*    PRIMERY_DNS_IP=192.168.25.103 */
    /*    SECNDRY_DNS_IP=192.168.25.103 */
    /*      */
    /*    APP Reset-APP SW Reset */

    done = false;
    while (!done) {
        /* Read a line */
        len = 0;
        while (1) {
            /* Out of time? */
            if ((MSTimerDelta(start) >= timeout) && (timeout != 0)) {
                /* Timeout! */
                rxMsgId = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
                done = true;
                break;
            }
            /* Block waiting for a character */
            if (App_Read(&c, 1, 0)) {
                /* Ignore \r */
                if (c == '\r')
                    continue;
                if (c == '\n') {
                    /* Cap the end of the string and restart the string length */
                    line[len] = '\0';

                    /* Blank line?  Then we're done */
                    if ((len == 0) || (strncmp(line, "APP Reset", 9) == 0)) {
                        done = true;
                        break;
                    }
                    len = 0;

                    /* Got a line with some data */
                    /* Parse and only process if there are two sides to this */
                    p = strchr(line, '=');
                    if (p) {
                        /* Have two parts, left and right.  Null the equal */
                        *p = '\0';
                        p++;
#ifdef ATLIBGS_DEBUG_ENABLE
                        ConsolePrintf("%s -> [%s]\n", line, p);
#endif
                        /* Look at the field and setup (in order they will appear): */
                        if (strcmp(line, "SSID") == 0) {
                            strncpy(wp->ssid, p, ATLIBGS_SSID_MAX_LENGTH);
                        } else if (strcmp(line, "CHNL") == 0) {
                            wp->channel = atoi(p);
                        } else if (strcmp(line, "CONN_TYPE") == 0) {
                            wp->conn_type = atoi(p);
                        } else if (strcmp(line, "MODE") == 0) {
                            wp->station = (ATLIBGS_STATIONMODE_E)atoi(p);
                        } else if (strcmp(line, "SECURITY") == 0) {
                            wp->security = (ATLIBGS_PROVSECURITY_E)atoi(p);
                        } else if (strcmp(line, "PSK_PASS_PHRASE") == 0 || strcmp(line, "WEP_KEY") == 0 ) {
                            strncpy(wp->password, p,
                                    ATLIBGS_PASSWORD_MAX_LENGTH);
                        } else if (strcmp(line, "DHCP_ENBL") == 0) {
                            wp->dhcp_enable = atoi(p);
                            if (wp->dhcp_enable) {
                                done = true;
                                break;
                            }
                        } else if (strcmp(line, "STATIC_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->ip);
                        } else if (strcmp(line, "SUBNT_MASK") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->subnet);
                        } else if (strcmp(line, "GATEWAY_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->gateway);
                        } else if (strcmp(line, "AUTO_DNS_ENBL") == 0) {
                            wp->auto_dns_enable = atoi(p);
                        } else if (strcmp(line, "PRIMERY_DNS_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->dns1);
                        } else if (strcmp(line, "SECNDRY_DNS_IP") == 0) {
                            AtLibGs_ParseIPv4Address(p, &wp->dns2);
                            /* This is the last one we'll get! */
                            done = true;
                            break;
                        }
                    }
                } else {
                    /* Append the character to the line */
                    if (len < 80)
                        line[len++] = c;
                }
            }
        }
    }

    return rxMsgId;
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_IPv4AddressToString
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert a IPv4 address into a string in the format "###.###.###.###"
 * Inputs:
 *      ATLIBGS_IPv4 *ip -- IP address to convert
 *      char *string -- String to receive the IP address.
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void AtLibGs_IPv4AddressToString(ATLIBGS_IPv4 *ip, char *string)
{
    uint8_t v1, v2, v3, v4;
 
    v1 =(*ip)[0]; // extract each address
    v2 =(*ip)[1];
    v3 =(*ip)[2];
    v4 =(*ip)[3];
    sprintf(string, "%d.%d.%d.%d", v1, v2, v3, v4); // format the string
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetXMLParse
 *---------------------------------------------------------------------------*
 * Description:
 *      enable or disable the XML Parser
 *      Sends the command:
 *          AT+XMLPARSE=<0|1>
 *      and waits for a response.
 * Inputs:
 *      Enable or Disable XML Parse
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetXMLParse(uint8_t mode)
{
    char cmd[30];

    sprintf(cmd, "AT+XMLPARSE=" _F16_ "\r\n", (uint16_t)mode);

    return AtLibGs_CommandSendString(cmd);
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_WebServer
 *---------------------------------------------------------------------------*
 * Description:
 *    Setup a web server
 *    Sends the command:
 *     AT+WEBSERVER=
 * Inputs:
 *    mode, username, password, SSL, idleTimeOut (NULL terminated char array, 16 char limit)
 * Comment: When a string parameter is not used, just past empty string ""
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_WebServer(uint8_t mode, char username[], char password[], char SSL[], char idleTimeOut[])
{
    char cmd[80];

    sprintf(cmd, "AT+WEBSERVER=%d,%s,%s,%s,%s\r\n", mode,username, password,SSL,idleTimeOut);

    return AtLibGs_CommandSendString(cmd);
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_StartMDNS
 *---------------------------------------------------------------------------*
 * Description:
 *      To start mDNS
 *      Sends the command:
 *          AT+MDNSSTART
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_StartMDNS(void)
{
    return AtLibGs_CommandSendString("AT+MDNSSTART\r\n");
}
/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_AnnounceMDNS
 *---------------------------------------------------------------------------*
 * Description:
 *      To announce mDNS
 *      Sends the command:
 *          AT+MDNSANNOUNCE
 * Inputs:
 *    void
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_AnnounceMDNS(void)
{
    return AtLibGs_CommandSendString("AT+MDNSANNOUNCE\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RegisterMDNSHost
 *---------------------------------------------------------------------------*
 * Description:
 *    Register the host name for the mdns.
 *    Sends the command:
 *     AT+MDNSHNREG=
 * Inputs:
 *    hostname, domain (NULL terminated char array)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_RegisterMDNSHost(char hostname[], char domain[])
{
    char cmd[80];

    sprintf(cmd, "AT+MDNSHNREG=%s,%s\r\n", hostname, domain);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_RegisterMDNSService
 *---------------------------------------------------------------------------*
 * Description:
 *    Register service to mdns.
 *    Sends the command:
 *     AT+MDNSSRVREG=
 * Inputs:
 *    
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E  AtLibGs_RegisterMDNSService(char *pServerName, char *pServerSubType, char *pServiceType, \
                                             char *pProtocol,  char *pDomain, char *pPort, char *pDefaultKey, char *pKey1  )
{
	char cmd[128];

	/* Construct the AT command */
    sprintf(cmd,"AT+MDNSSRVREG=%s,%s,%s,%s,%s,%s,%s,%s\r\n",pServerName,pServerSubType,pServiceType,pProtocol,pDomain,pPort, pDefaultKey, pKey1);
	
	return  AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetServerURI
 *---------------------------------------------------------------------------*
 * Description:
 *    This API modify the default adapter uri to the new one. The default URI is /gainspan/profile/mcu.
 *     Also reserved URIs like /gainspan/system cannot be used.
 *    
 * Inputs:
 *   URI (NULL terminated char array)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetServerURI(char* URI)
{
    char cmd[80];

    sprintf(cmd, "AT+URIRECV=%s\r\n", URI);

    return AtLibGs_CommandSendString(cmd);
}

/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_GetIPAddress(uint8_t* ipAddr)
 *---------------------------------------------------------------------------*
 * Description:
 *    This will send at+nstat=? and waits for a response.
 *    Then parse the response and get ip address
 * Inputs:
 *   URI (NULL terminated char array)
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
uint8_t AtLibGs_ReadValue(const char *strKey, char *strVal, char delimiter)
{
  char *pSubStr = NULL, *pStartChar = NULL, *pEndChar = NULL;
  uint8_t length = 0;

  pSubStr = strstr ((const char *) MRBuffer, strKey);
  if (pSubStr)
  {
      pStartChar = strchr ((const char *) pSubStr, '=') + 1;
      pEndChar = strchr ((const char *) pStartChar, delimiter);

      if (pStartChar && pEndChar)
	  {
	    length = pEndChar - pStartChar;
	    memcpy (strVal, pStartChar, length);
	    strVal[length] = 0;	// Terminate the string
	  }
  }
  return length;
}

uint8_t AtLibGs_ParseIpAddress(char *IpAddr)
{
   return AtLibGs_ReadValue("IP addr=", IpAddr, ' ');
}

bool AtLibGs_GetIPAddress(uint8_t* ipAddr)
{
   if(AtLibGs_WlanConnStat() != ATLIBGS_MSG_ID_OK)
   {
        return false;
   }

   return AtLibGs_ParseIpAddress((char*)ipAddr);
}


/*---------------------------------------------------------------------------*
 * Routine:  AtLibGs_SetUARBaud
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the S2W UART port baud rate.
 *      Sends the command:
 *          ATB=baudrate
 *      and waits for a response.
 *      
 * Inputs:
 *      Allowed baud rates include: 9600, 19200, 38400, 57600, 115200, 230400,460800 and 921600.
 * Outputs:
 *      ATLIBGS_MSG_ID_E -- error code
 *---------------------------------------------------------------------------*/
ATLIBGS_MSG_ID_E AtLibGs_SetUARBaud(char *pUARTBaud)
{
    char cmd[30];

    sprintf(cmd, "ATB=%s\r\n", pUARTBaud);

    return AtLibGs_CommandSendString(cmd);
}
/*-------------------------------------------------------------------------*
 * End of File:  AtCmdLib.c
 *-------------------------------------------------------------------------*/

