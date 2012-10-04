/*-------------------------------------------------------------------------*
 * File:  SPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     SPI interrupt based driver for the RX62N's CSI10 peripheral.
 *-------------------------------------------------------------------------*/
#ifndef SPI_H_
#define SPI_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define SPI_NUM_CHANNELS          5

#define SPI_SDCARD_CHANNEL        0
#define SPI_APPHEADER_CHANNEL     1
#define SPI_LCD_CHANNEL           2
#define SPI_PMOD1_CHANNEL         3
#define SPI_PMOD2_CHANNEL         4

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void SPI_Init(uint32_t bitsPerSecond);
void SPI_ChannelSetup(
        uint8_t channel,
        bool csActiveHigh,
        bool csActivePerByte);
bool SPI_Transfer(
        uint8_t channel,
        uint32_t numBytes,
        const uint8_t *send_buffer,
        uint8_t *receive_buffer,
        void(*callback)(void));
bool SPI_IsBusy(uint8_t channel);
void SPI_DisableInterrupts(void);
void SPI_EnableInterrupts(void);

#endif // SPI_H_
/*-------------------------------------------------------------------------*
 * End of File:  SPI.h
 *-------------------------------------------------------------------------*/
