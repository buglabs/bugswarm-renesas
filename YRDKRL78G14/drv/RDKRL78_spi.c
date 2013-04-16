// RDKRL78_spi.c
//#include <stdint.h>
#include <stdbool.h>
#include "r_cg_macrodriver.h"
//#include "r_cg_userdefine.h"
#include "r_cg_serial.h"
#include "r_cg_it.h"
#include "RDKRL78_spi.h"

extern volatile uint8_t G_CSI21_SendingData; //spi busy flag
extern volatile uint8_t G_CSI21_ReceivingData; //spi busy flag

uint8_t *SPI_CS_Port[] = {
    (uint8_t *)&P8,   // EINK-CS#       P80
    (uint8_t *)&P14,  // SD-CS          P142
    (uint8_t *)&P14,  // LCD-CS         P145
    (uint8_t *)&P8,   // PMOD2-CS       P83
    (uint8_t *)&P8    // APP-WIFI-CS    P84
};

uint8_t SPI2_CS_Pin[] = {
    0,   // EINK-CS#       P80
    2,   // SD-CS          P142
    5,   // LCD-CS         P145
    3,   // PMOD2-CS       P83
    4    // APP-WIFI-CS    P84
};

void IO_Reset(void)
{
    int i = 0;
    
	//#warning RESET-IO must be inverted for actual HW
	P13 |= (1<<0); // Assert P130 (#RESET-IO)
    for (i=0;i<12000;i++);
    P13 &= ~(1<<0);  // Deassert P130 (#RESET-IO)
    for (i=0;i<12000;i++);
}

void SPI2_Init(void)
{
    IO_Reset();
    R_SAU1_Create();
   // R_CSI21_Create();
    R_CSI21_Start(); 
}

void SPI_CS_Start(uint8_t aDevice)
{
	*SPI_CS_Port[aDevice] &= ~(1<<SPI2_CS_Pin[aDevice]);
}

void SPI_CS_End(uint8_t aDevice)
{
	*SPI_CS_Port[aDevice] |= (1<<SPI2_CS_Pin[aDevice]);
}

void SPI_Send(uint8_t aDevice, uint8_t *aData, uint32_t aLength)
{
    uint8_t noRXData;
	G_CSI21_SendingData = 1;
	G_CSI21_ReceivingData = 0;
	
	SPI_CS_Start(aDevice);

    R_CSI21_Send_Receive(aData, aLength, &noRXData);
    while(G_CSI21_SendingData);
    
    SPI_CS_End(aDevice);
}

uint8_t SPI_SendReceive(uint8_t aDevice, uint8_t *aTXData, uint32_t aTXLength, uint8_t *aRXData)
{
    uint16_t timeout;
	G_CSI21_SendingData = 1;
	G_CSI21_ReceivingData = 1;
	
	SPI_CS_Start(aDevice);
	
    if(aDevice == SPI_SD)
        delay_ms(1);
    
    R_CSI21_Send_Receive(aTXData, aTXLength, aRXData);
    
    timeout = 0;
    while(timeout < 1000)
    {
        if(!(G_CSI21_SendingData || G_CSI21_ReceivingData))
            break;
        
        timeout++;
    }
    
	SPI_CS_End(aDevice);
    
    if(timeout >= 1000)
        return 0;
    else
        return 1;
}

