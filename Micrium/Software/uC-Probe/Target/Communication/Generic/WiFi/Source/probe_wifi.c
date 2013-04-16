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
* Filename      : probe_wifi.c
* Version       : V2.30
* Programmer(s) : FBJ
*                 BAN
*                 JPB
*********************************************************************************************************
* Note(s)       : (1) The abbreviations RX and TX refer to communication from the target's perspective.
*
*                 (2) The abbreviations RD and WR refer to reading data from the target memory and
*                     writing data to the target memory, respectively.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define    PROBE_WIFI_MODULE

#include  <probe_wifi.h>
#include  <ATCmdLib/AtCmdLib.h>
#include  <Apps.h>


/*
*********************************************************************************************************
*                                               ENABLE
*
* Note(s) : (1) See 'probe_wifi.h  ENABLE'.
*********************************************************************************************************
*/

#if (PROBE_COM_CFG_WIFI_EN == DEF_ENABLED)                     /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#if (PROBE_WIFI_CFG_SOCK_BLOCK_EN == DEF_ENABLED)
#define  PROBE_WIFI_RECVFROM_FLAGS                        0
#else
#define  PROBE_WIFI_RECVFROM_FLAGS             MSG_DONTWAIT
#endif


/*
*********************************************************************************************************
*                                         WIFI PACKET FORMAT
*
* Note(s):  (1) All packets include the following parts:
*
*                   (A)  4 1-byte start delimiters, forming the ASCII representation of "uCPr".  These
*                        are the constants PROBE_WIFI_PROTOCOL_RX_SD0-PROBE_WIFI_PROTOCOL_?X_SD4;
*                   (B)  1 2-byte length, the length of the data segment;
*                   (C)  1 1-byte packet counter.
*                   (D)  1 1-byte padding, unused;
*                   (E)  n   bytes of data; and
*                   (F)  1 1-byte checksum; and
*                   (G)  1 1-byte end delimiter, the character '/', which is the constant PROBE_WIFI_PROTOCOL_?X_ED.
*
*                                       +-------------------+-------------------+
*                                       |   'u'   |   'C'   |   'P'   |   'r'   |
*                                       +-------------------+-------------------+
*                                       |       Length      | PktCtr  | Padding |
*                                       +-------------------+-------------------+
*                                       |                  Data                 |   The data segment does not need to end on
*                                       |                   .                   |   a four-byte boundary, as might be inferred
*                                       |                   .                   |   from this diagram.
*                                       |                   .                   |
*                                       +-------------------+-------------------+
*                                       | Checksum|   '/'   |
*                                       +-------------------+
*********************************************************************************************************
*/

                                                                    /* ------------- INBOUND PACKET DELIMITERS ------------ */
#define  PROBE_WIFI_PROTOCOL_RX_SD0                    0x75u       /* (A) Start delimiters                                 */
#define  PROBE_WIFI_PROTOCOL_RX_SD1                    0x43u
#define  PROBE_WIFI_PROTOCOL_RX_SD2                    0x50u
#define  PROBE_WIFI_PROTOCOL_RX_SD3                    0x72u
#define  PROBE_WIFI_PROTOCOL_RX_ED                     0x2Fu       /* (E) End   delimiter                                  */

                                                                    /* ------------ OUTBOUND PACKET DELIMITERS ------------ */
#define  PROBE_WIFI_PROTOCOL_TX_SD0                    0x75u       /* (A) Start delimiters                                 */
#define  PROBE_WIFI_PROTOCOL_TX_SD1                    0x43u
#define  PROBE_WIFI_PROTOCOL_TX_SD2                    0x50u
#define  PROBE_WIFI_PROTOCOL_TX_SD3                    0x72u
#define  PROBE_WIFI_PROTOCOL_TX_ED                     0x2Fu       /* (E) End   delimiter                                  */


#define  PROBE_WIFI_CHKSUM_EN                     DEF_FALSE        /* DO NOT CHANGE                                        */


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

#define  PROBE_WIFI_CLIENT_IP_MAX_LEN                   32u


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

static  CPU_CHAR      ProbeWiFi_RxBuf[PROBE_WIFI_CFG_RX_BUF_SIZE + 10];
static  CPU_CHAR      ProbeWiFi_TxBuf[PROBE_WIFI_CFG_TX_BUF_SIZE + 10];

static  CPU_BOOLEAN   ProbeWiFi_ServerRunning;                  /* Flag indicating the UDP server has been started.     */
static  CPU_INT08U    ProbeWiFi_ConnSockID;                     /* Socket ID of connected or accepted socket.           */

                                                                /* UDP client IP address  used to send back response.   */
static  CPU_CHAR      ProbeWiFi_ClientIP[PROBE_WIFI_CLIENT_IP_MAX_LEN];
static  CPU_INT16U    ProbeWiFi_ClientPort;                     /* UDP client port number used to send back response.   */


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_BOOLEAN  ProbeWiFi_ServerInit(void);
static  CPU_INT16S   ProbeWiFi_RxPkt     (void);
static  CPU_INT16U   ProbeWiFi_ParseRxPkt(CPU_INT16S  rx_len);
static  CPU_INT16S   ProbeWiFi_TxStart   (CPU_INT16U  tx_len);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          ProbeWiFi_Init()
*
* Description : Initializes the Probe TCP-IP communication module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeWiFi_Init (void)
{
    ProbeWiFi_ServerRunning = DEF_FALSE;
    ProbeWiFi_OS_Init();
}


/*
*********************************************************************************************************
*                                          ProbeWiFi_Task()
*
* Description : Server task.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : ProbeWiFi_OS_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  ProbeWiFi_Task (void)
{
    CPU_INT16S   len;


    if (ProbeWiFi_ServerRunning == DEF_TRUE) {
        len = ProbeWiFi_RxPkt();                                /* Receive a UDP packet if any.                         */
        if (len > 0) {
            len = (CPU_INT16S)ProbeWiFi_ParseRxPkt(len);        /* Parse UDP packet & form a response.                  */
            if (len > 0) {                                      /* Send the response back to the UDP client.            */
                (void)ProbeWiFi_TxStart(len);
            }
        }
    } else {
        ProbeWiFi_ServerRunning = ProbeWiFi_ServerInit();       /* Start the UDP server.                                */
    }
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
*                                       ProbeWiFi_ServerInit()
*
* Description : Initialize the UDP server.
*
* Argument(s) : None.
*
* Return(s)   : The socket identifier, if NO errors.
*               -1,                    if an error occurred.
*
* Caller(s)   : ProbeWiFi_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  ProbeWiFi_ServerInit (void)
{
    CPU_BOOLEAN            init;
    ATLIBGS_MSG_ID_E       ret;
    ATLIBGS_NetworkStatus  network_status;

    ret = AtLibGs_UDPServer_Start( PROBE_WIFI_CFG_PORT, 
                                  &ProbeWiFi_ConnSockID);       /* Start a UDP server on port 9930.                     */

    if (ret != ATLIBGS_MSG_ID_OK) {
        init = DEF_FALSE;                                       /* Unable to start the UDP server.                      */
    } else {
        init = DEF_TRUE;                                        /* UDP server is up and running.                        */
    }

    do {
        ret = AtLibGs_GetNetworkStatus(&network_status);
    } while (ret != ATLIBGS_MSG_ID_OK);


    return (init);
}


/*
*********************************************************************************************************
*                                         ProbeWiFi_RxPkt()
*
* Description : Receive data from Probe.
*
* Argument(s) : none.
*
* Return(s)   : The length of the received packet or (if an error occurred) -1.
*
* Caller(s)   : ProbeWiFi_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT16S  ProbeWiFi_RxPkt (void)
{
    ATLIBGS_MSG_ID_E    rx_msg_id;
    ATLIBGS_UDPMessage  rx_udp_pkt;
    CPU_INT16S          len;


    App_PrepareIncomingData();
    rx_msg_id = AtLibGs_WaitForUDPMessage(250);

    if (rx_msg_id == ATLIBGS_MSG_ID_OK) {
                                                                /* Parses the UDP stream and fills the rx_udp_pkt.      */
        AtLibGs_ParseUDPData(G_received, G_receivedCount, &rx_udp_pkt);
        len = rx_udp_pkt.numBytes;
        if (len > 0 && len < PROBE_WIFI_CFG_RX_BUF_SIZE) {
            Mem_Copy(ProbeWiFi_RxBuf,
                     rx_udp_pkt.message,
                     rx_udp_pkt.numBytes);                      /* Copy UDP request message into the Probe Rx buffer.   */

            ProbeWiFi_ClientPort = rx_udp_pkt.port;             /* Pull out UDP client information.                     */
            Str_Copy_N(&ProbeWiFi_ClientIP[0],
                       &rx_udp_pkt.ip[0],
                        PROBE_WIFI_CLIENT_IP_MAX_LEN);
        } else {
            len = 0;
        }
    } else {
        len = 0;
    }

    return (len);
}


/*
*********************************************************************************************************
*                                        ProbeWiFi_TxStart()
*
* Description : Sends a response packet to Probe.
*
* Argument(s) : tx_len      Length of the transmit packet.
*
* Return(s)   : The length of the received packet, if NO errors.
*               -1,                                if an error occurred.
*
* Caller(s)   : ProbeWiFi_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static CPU_INT16S  ProbeWiFi_TxStart (CPU_INT16U tx_len)
{
    ATLIBGS_MSG_ID_E  tx_msg_id;
    CPU_INT16S        len;


    tx_msg_id = AtLibGs_SendUDPData(ProbeWiFi_ConnSockID,
                                    (uint8_t *)ProbeWiFi_TxBuf,
                                    tx_len,
                                    ATLIBGS_CON_UDP_SERVER,
                                    ProbeWiFi_ClientIP,
                                    ProbeWiFi_ClientPort);

    if (tx_msg_id == ATLIBGS_MSG_ID_OK) {
        len = tx_len;
    } else {
        len = 0;
    }

    return (len);
}


/*
*********************************************************************************************************
*                                       ProbeWiFi_ParseRxPkt()
*
* Description : Parse received packet & form respose packet.
*
* Argument(s) : rx_len      Length of the received packet.
*
* Return(S)   : The number of bytes in the data segment of the packet to transmit in response.
*
* Caller(s)   : ProbeWiFi_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  ProbeWiFi_ParseRxPkt (CPU_INT16S  rx_len)
{
    CPU_INT16U  len;
    CPU_INT16U  rx_len_pkt;
    CPU_INT08U  pkt_ctr;


    if (rx_len <= 10) {                                         /* Pkt is too short.                                    */
        return ((CPU_INT16U)0);
    }

                                                                /* Start delims bad.                                    */
    if ((ProbeWiFi_RxBuf[0] != PROBE_WIFI_PROTOCOL_RX_SD0) ||
        (ProbeWiFi_RxBuf[1] != PROBE_WIFI_PROTOCOL_RX_SD1) ||
        (ProbeWiFi_RxBuf[2] != PROBE_WIFI_PROTOCOL_RX_SD2) ||
        (ProbeWiFi_RxBuf[3] != PROBE_WIFI_PROTOCOL_RX_SD3)) {
        return ((CPU_INT16U)0);
    }

                                                                /* Len in pkt does not match pkt len.                   */
    rx_len_pkt = ((CPU_INT16U)ProbeWiFi_RxBuf[4] | ((CPU_INT16U)ProbeWiFi_RxBuf[5] << 8));
    if (rx_len_pkt != rx_len - 10) {
        return ((CPU_INT16U)0);
    }

    pkt_ctr = ProbeWiFi_RxBuf[6];

    len     = ProbeCom_ParseRxPkt((void     *)&ProbeWiFi_RxBuf[8],
                                  (void     *)&ProbeWiFi_TxBuf[8],
                                  (CPU_INT16U) rx_len_pkt,
                                  (CPU_INT16U) PROBE_WIFI_CFG_TX_BUF_SIZE);

    if (len == 0u) {
        return ((CPU_INT16U)0);
    }

                                                                /* Start delims.                                        */
    ProbeWiFi_TxBuf[0]           = PROBE_WIFI_PROTOCOL_TX_SD0;
    ProbeWiFi_TxBuf[1]           = PROBE_WIFI_PROTOCOL_TX_SD1;
    ProbeWiFi_TxBuf[2]           = PROBE_WIFI_PROTOCOL_TX_SD2;
    ProbeWiFi_TxBuf[3]           = PROBE_WIFI_PROTOCOL_TX_SD3;
    ProbeWiFi_TxBuf[4]           = (len & 0xFF);               /* Two-byte pkt len.                                    */
    ProbeWiFi_TxBuf[5]           = (len >> 8);
    ProbeWiFi_TxBuf[6]           = pkt_ctr;                    /* Packet counter.                                      */
    ProbeWiFi_TxBuf[7]           = 0;

    ProbeWiFi_TxBuf[8 + len]     = 0;                          /* Checksum.                                            */
    ProbeWiFi_TxBuf[8 + len + 1] = PROBE_WIFI_PROTOCOL_TX_ED;  /* End delim.                                           */

    return (len + 10u);
}


/*
*********************************************************************************************************
*                                              ENABLE END
*
* Note(s) : See 'ENABLE  Note #1'.
*********************************************************************************************************
*/

#endif
