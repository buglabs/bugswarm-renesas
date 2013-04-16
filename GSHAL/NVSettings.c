/*-------------------------------------------------------------------------*
 * File:  NVSettings.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <system/platform.h>
#include "NVSettings.h"
#include "HostApp.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
uint8_t NVSettingsLoad(NVSettings_t *settings)
{
    uint8_t failed;

    NV_Open();
    failed = NV_Read(0, (uint8_t *)settings, sizeof(*settings));
    if (!failed)
        if (NVSettingsChecksum(settings) != settings->checksum)
            failed = 1;

    // If the above fails, make sure to re-initialize the data
    if (failed)
        NVSettingsInit(settings);

    return failed;
}

uint8_t NVSettingsSave(NVSettings_t *settings)
{
    uint8_t failed;

    // Setup the checksum
    settings->checksum = NVSettingsChecksum(settings);
    
    // Open up the flash and save the settings
    NV_Open();
    NV_Erase(0, sizeof(*settings));
    failed = NV_Write(0, (uint8_t *)settings, sizeof(*settings));

    return failed;
}

void NVSettingsInit(NVSettings_t *settings)
{
    memset((void *)settings, 0, sizeof(*settings));
    strcpy(settings->webprov.ssid, ATLIBGS_AP_SSID);
    settings->webprov.channel = ATLIBGS_AP_CHANNEL;
#if defined(ATLIBGS_SEC_PSK)
        strcpy(settings->webprov.password, ATLIBGS_AP_SEC_PSK);
        settings->webprov.security = ATLIBGS_SMWPA2PSK;
#else
        settings->webprov.password[0] = '\0';
        settings->webprov.security = ATLIBGS_SMOPEN;
#endif
    settings->webprov.station = ATLIBGS_STATIONMODE_INFRASTRUCTURE;
    settings->webprov.dhcp_enable = 1;
    settings->webprov.tcpIPClientHostIP = 1;
}

uint32_t NVSettingsChecksum(const NVSettings_t *settings)
{
    uint32_t checksum = 0;
    uint16_t i;

    // Sum the bytes
    for (i=0; i<sizeof(*settings)-sizeof(settings->checksum); i++)
        checksum += ((uint8_t *)settings)[i];

    // Return an inverted number
    return ~checksum;
}

/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.c
 *-------------------------------------------------------------------------*/
