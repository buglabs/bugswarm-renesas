/*-------------------------------------------------------------------------*
 * File:  NVSettings.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef NVSETTINGS_H_
#define NVSETTINGS_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "HostApp.h"
#include "NVSettings.h"
#include <system/platform.h>
#include <ATCmdLib/AtCmdLib.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        ATLIBGS_WEB_PROV_SETTINGS webprov;

        // This field must be last
        uint32_t checksum;
} NVSettings_t ;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
uint8_t NVSettingsLoad(NVSettings_t *settings);
uint8_t NVSettingsSave(NVSettings_t *settings);
uint32_t NVSettingsChecksum(const NVSettings_t *settings);
void NVSettingsInit(NVSettings_t *settings);

#define GAINSPAN_SIGNATURE_ADDR   sizeof(NVSettings_t)  //  0
#define GAINSPAN_SIGNATURE_LEN    8

#define GAINSPAN_SSID_MAX_LEN     20
#define GAINSPAN_SSID_ADDR       (GAINSPAN_SIGNATURE_ADDR + GAINSPAN_SIGNATURE_LEN)

#define GAINSPAN_CHANNEL_ADDR    (GAINSPAN_SSID_ADDR + GAINSPAN_SSID_MAX_LEN)    
#define GAINSPAN_CHANNEL_MAX_LEN  1

#endif // NVSETTINGS_H_
/*-------------------------------------------------------------------------*
 * End of File:  NVSettings.h
 *-------------------------------------------------------------------------*/
