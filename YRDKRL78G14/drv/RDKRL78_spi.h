// RDKRL78_spi.h
#ifndef _RDKRL78_SPI_H
#define _RDKRL78_SPI_H

#define SPI_EINK        0
#define SPI_SD          1
#define SPI_LCD         2
#define SPI_PMOD2       3
#define SPI_APP_WIFI    4

void SPI2_Init();
void SPI_Send(uint8_t aDevice, uint8_t *aData, uint32_t aLength);
uint8_t SPI_SendReceive(uint8_t aDevice, uint8_t *aTXData, uint32_t aTXLength, uint8_t *aRXData);

#endif // _RSPI_H