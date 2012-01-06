// RDKRL78_spi.h
#ifndef _RDKRL78_SPI_H
#define _RDKRL78_SPI_H

#define SPI_LCD     0
#define SPI_SD      1
#define SPI_PMOD1   2
#define SPI_PMOD2   3

void SPI_Init();
void SPI_Send(uint8_t aDevice, uint8_t *aData, uint32_t aLength);
void SPI_SendReceive(uint8_t aDevice, uint8_t *aTXData, uint32_t aTXLength, uint8_t *aRXData);

#endif // _RSPI_H