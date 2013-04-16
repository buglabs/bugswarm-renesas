/*-------------------------------------------------------------------------*
 * File:  SAU.h
 *-------------------------------------------------------------------------*
 * Description:
 *     RL78 Serial Array Unit (SAU) definitions
 *-------------------------------------------------------------------------*/
#ifndef _SAU_H
#define _SAU_H

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum
{
    NONE = 0,
    FRAMING_ERROR = 1,
    PARITY_ERROR = 2,
    OVERRUN_ERROR = 3,
    UNKNOWN_ERROR = 4
}T_SAUStatusError;

/*-------------------------------------------------------------------------*
 * Macro definitions (Register bit)
 *-------------------------------------------------------------------------*/
/*
	Serial Mode Register mn (SMRmn)
*/
#define	_SAU_SMRMN_INITIALVALUE		0x0020U
/* Selection of macro clock (MCK) of channel n (CKSmn) */
#define _SAU_CLOCK_SELECT_CK00		0x0000U	/* operation clock CK0 set by PRS register */ 
#define _SAU_CLOCK_SELECT_CK01		0x8000U	/* operation clock CK1 set by PRS register */
/* Selection of transfer clock (TCLK) of channel n (CCSmn) */
#define _SAU_CLOCK_MODE_CKS		0x0000U	/* divided operation clock MCK specified by CKSmn bit */  
#define _SAU_CLOCK_MODE_TI0N		0x4000U	/* clock input from SCK pin (slave transfer in CSI mode) */
/* Selection of start trigger source (STSmn) */
#define _SAU_TRIGGER_SOFTWARE		0x0000U	/* only software trigger is valid */
#define _SAU_TRIGGER_RXD			0x0100U	/* valid edge of RXD pin */
/* Controls inversion of level of receive data of channel n in UART mode (SISmn0) */
#define _SAU_EDGE_FALL			0x0000U	/* falling edge is detected as the start bit */
#define _SAU_EDGE_RISING			0x0040U	/* rising edge is detected as the start bit */
/* Setting of operation mode of channel n (MDmn2, MDmn1) */
#define _SAU_MODE_CSI			0x0000U	/* CSI mode */
#define _SAU_MODE_UART			0x0002U	/* UART mode */
#define _SAU_MODE_IIC			0x0004U	/* simplified IIC mode */
/* Selection of interrupt source of channel n (MDmn0) */
#define _SAU_TRANSFER_END			0x0000U	/* transfer end interrupt */
#define _SAU_BUFFER_EMPTY			0x0001U	/* buffer empty interrupt */

/*
	Serial Communication Operation Setting Register mn (SCRmn)
*/
/* Setting of operation mode of channel n (TXEmn, RXEmn) */
#define _SAU_NOT_COMMUNICATION		0x0000U	/* does not start communication */
#define _SAU_RECEPTION			0x4000U	/* reception only */
#define _SAU_TRANSMISSION			0x8000U	/* transmission only */
#define _SAU_RECEPTION_TRANSMISSION	0xC000U	/* reception and transmission */
/* Selection of data and clock phase in CSI mode (DAPmn, CKPmn) */
#define _SAU_TIMING_1			0x0000U	/* type 1 */
#define _SAU_TIMING_2			0x1000U	/* type 2 */
#define _SAU_TIMING_3			0x2000U	/* type 3 */
#define _SAU_TIMING_4			0x3000U	/* type 4 */
/* Selection of masking of error interrupt signal (EOCmn) */
#define _SAU_INTSRE_MASK			0x0000U	/* masks error interrupt INTSREx */
#define _SAU_INTSRE_ENABLE			0x0400U	/* enables generation of error interrupt INTSREx */
/* Setting of parity bit in UART mode (PTCmn1 - PTCmn0) */
#define _SAU_PARITY_NONE			0x0000U	/* none parity */
#define _SAU_PARITY_ZERO			0x0100U	/* zero parity */
#define _SAU_PARITY_EVEN			0x0200U	/* even parity */
#define _SAU_PARITY_ODD			0x0300U	/* odd parity */
/* Selection of data transfer sequence in CSI and UART modes (DIRmn) */
#define _SAU_MSB				0x0000U	/* MSB */
#define _SAU_LSB				0x0080U	/* LSB */
/* Setting of stop bit in UART mode (SLCmn1, SLCmn0) */
#define _SAU_STOP_NONE			0x0000U	/* none stop bit */
#define _SAU_STOP_1			0x0010U	/* 1 stop bit */
#define _SAU_STOP_2			0x0020U	/* 2 stop bits */
/* Setting of data length in CSI and UART modes (DLSmn2 - DLSmn0) */
#define _SAU_LENGTH_9			0x0005U	/* 9-bit data length */
#define _SAU_LENGTH_7			0x0006U	/* 7-bit data length */
#define _SAU_LENGTH_8			0x0007U	/* 8-bit data length */

/*
	Serial Output Level Register m (SOLm)
*/
/* Selects inversion of the level of the transmit data of channel n in UART mode */
#define _SAU_CHANNEL0_NORMAL		0x0000U	/* normal bit level */
#define _SAU_CHANNEL0_INVERTED		0x0001U	/* inverted bit level */
#define _SAU_CHANNEL1_NORMAL		0x0000U	/* normal bit level */
#define _SAU_CHANNEL1_INVERTED		0x0002U	/* inverted bit level */
#define _SAU_CHANNEL2_NORMAL		0x0000U	/* normal bit level */
#define _SAU_CHANNEL2_INVERTED		0x0004U	/* inverted bit level */
#define _SAU_CHANNEL3_NORMAL		0x0000U	/* normal bit level */
#define _SAU_CHANNEL3_INVERTED		0x0008U	/* inverted bit level */

/*
	Noise Filter Enable Register 0 (NFEN0)
*/
/* Use of noise filter */
#define _SAU_RXD3_FILTER_OFF			0x00U	/* noise filter off */
#define _SAU_RXD3_FILTER_ON			0x40U	/* noise filter on */
#define _SAU_RXD2_FILTER_OFF			0x00U	/* noise filter off */
#define _SAU_RXD2_FILTER_ON			0x10U	/* noise filter on */
#define _SAU_RXD1_FILTER_OFF			0x00U	/* noise filter off */
#define _SAU_RXD1_FILTER_ON			0x04U	/* noise filter on */
#define _SAU_RXD0_FILTER_OFF			0x00U	/* noise filter off */
#define _SAU_RXD0_FILTER_ON			0x01U	/* noise filter on */

/*
	Format of Serial Status Register mn (SSRmn)
*/
/* Communication status indication flag of channel n (TSFmn) */
#define _SAU_UNDER_EXECUTE			0x0040U	/* communication is under execution */
/* Buffer register status indication flag of channel n (BFFmn) */
#define _SAU_VALID_STORED			0x0020U	/* valid data is stored in the SDRmn register */
/* Framing error detection flag of channel n (FEFmn) */
#define _SAU_FRAM_ERROR			0x0004U	/* a framing error occurs during UART reception */
/* Parity error detection flag of channel n (PEFmn) */
#define _SAU_PARITY_ERROR			0x0002U	/* a parity error occurs during UART reception or ACK is not detected during I2C transmission */
/* Overrun error detection flag of channel n (OVFmn) */
#define _SAU_OVERRUN_ERROR			0x0001U	/* an overrun error occurs */

/*
	Serial Channel Start Register m (SSm)
*/
/* Operation start trigger of channel 0 (SSm0) */
#define _SAU_CH0_START_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH0_START_TRG_ON		0x0001U	/* sets SEm0 to 1 and enters the communication wait status */
/* Operation start trigger of channel 1 (SSm1) */
#define _SAU_CH1_START_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH1_START_TRG_ON		0x0002U	/* sets SEm1 to 1 and enters the communication wait status */
/* Operation start trigger of channel 2 (SSm2) */
#define _SAU_CH2_START_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH2_START_TRG_ON		0x0004U	/* sets SEm2 to 1 and enters the communication wait status */
/* Operation start trigger of channel 3 (SSm3) */
#define _SAU_CH3_START_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH3_START_TRG_ON		0x0008U	/* sets SEm3 to 1 and enters the communication wait status */

/*
	Serial Channel Stop Register m (STm)
*/
/* Operation stop trigger of channel 0 (STm0) */
#define _SAU_CH0_STOP_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH0_STOP_TRG_ON		0x0001U	/* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 1 (STm1) */
#define _SAU_CH1_STOP_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH1_STOP_TRG_ON		0x0002U	/* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 2 (STm2) */
#define _SAU_CH2_STOP_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH2_STOP_TRG_ON		0x0008U	/* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 3 (STm3) */
#define _SAU_CH3_STOP_TRG_OFF		0x0000U	/* no trigger operation */
#define _SAU_CH3_STOP_TRG_ON		0x0008U	/* operation is stopped (stop trigger is generated) */

/*
	Format of Serial Flag Clear Trigger Register mn (SIRmn)
*/
/* Clear trigger of overrun error flag of channel n (OVCTmn) */
#define	_SAU_SIRMN_OVCTMN			0x0001U
/* Clear trigger of parity error flag of channel n (PECTmn) */
#define	_SAU_SIRMN_PECTMN			0x0002U
/* Clear trigger of framing error of channel n (FECTMN) */
#define	_SAU_SIRMN_FECTMN			0x0004U

/*
	Serial Output Enable Register m (SOEm)
*/
/* Serial output enable/disable of channel 0 (SOEm0) */
#define _SAU_CH0_OUTPUT_ENABLE		0x0001U	/* stops output by serial communication operation */
#define _SAU_CH0_OUTPUT_DISABLE		0x0000U	/* enables output by serial communication operation */
/* Serial output enable/disable of channel 1 (SOEm1) */
#define _SAU_CH1_OUTPUT_ENABLE		0x0002U	/* stops output by serial communication operation */
#define _SAU_CH1_OUTPUT_DISABLE		0x0000U	/* enables output by serial communication operation */
/* Serial output enable/disable of channel 2 (SOEm2) */
#define _SAU_CH2_OUTPUT_ENABLE		0x0004U	/* stops output by serial communication operation */
#define _SAU_CH2_OUTPUT_DISABLE		0x0000U	/* enables output by serial communication operation */
/* Serial output enable/disable of channel 3 (SOEm3) */
#define _SAU_CH3_OUTPUT_ENABLE		0x0008U	/* stops output by serial communication operation */
#define _SAU_CH3_OUTPUT_DISABLE		0x0000U	/* enables output by serial communication operation */

/*
	Serial Output Register m (SOm)
*/
/* Serial data output of channel 0 (SOm0) */
#define _SAU_CH0_DATA_OUTPUT_0		0x0000U	/* Serial data output value is "0" */
#define _SAU_CH0_DATA_OUTPUT_1		0x0001U	/* Serial data output value is "1" */
/* Serial data output of channel 1 (SOm1) */
#define _SAU_CH1_DATA_OUTPUT_0		0x0000U	/* Serial data output value is "0" */
#define _SAU_CH1_DATA_OUTPUT_1		0x0002U	/* Serial data output value is "1" */
/* Serial data output of channel 2 (SOm2) */
#define _SAU_CH2_DATA_OUTPUT_0		0x0000U	/* Serial data output value is "0" */
#define _SAU_CH2_DATA_OUTPUT_1		0x0004U	/* Serial data output value is "1" */
/* Serial data output of channel 3 (SOm3) */
#define _SAU_CH3_DATA_OUTPUT_0		0x0000U	/* Serial data output value is "0" */
#define _SAU_CH3_DATA_OUTPUT_1		0x0008U	/* Serial data output value is "1" */
/* Serial clock output of channel 0 (CKOm0) */
#define _SAU_CH0_CLOCK_OUTPUT_0		0x0000U	/* Serial clock output value is "0" */
#define _SAU_CH0_CLOCK_OUTPUT_1		0x0100U	/* Serial clock output value is "1" */
/* Serial clock output of channel 1 (CKOm1) */
#define _SAU_CH1_CLOCK_OUTPUT_0		0x0000U	/* Serial clock output value is "0" */
#define _SAU_CH1_CLOCK_OUTPUT_1		0x0200U	/* Serial clock output value is "1" */
/* Serial clock output of channel 2 (CKOm2) */
#define _SAU_CH2_CLOCK_OUTPUT_0		0x0000U	/* Serial clock output value is "0" */
#define _SAU_CH2_CLOCK_OUTPUT_1		0x0400U	/* Serial clock output value is "1" */
/* Serial clock output of channel 3 (CKOm3) */
#define _SAU_CH3_CLOCK_OUTPUT_0		0x0000U	/* Serial clock output value is "0" */
#define _SAU_CH3_CLOCK_OUTPUT_1		0x0800U	/* Serial clock output value is "1" */

/*
	SAU Standby Control Register m (SSCm)
*/
/* SAU Standby Wakeup Control Bit (SWC) */
#define _SAU_CH0_SNOOZE_OFF		0x0000U	/* disable start function from STOP state of chip */
#define _SAU_CH0_SNOOZE_ON			0x0001U	/* enable start function from STOP state of chip */

/* SAU used flag */
#define _SAU_IIC_MASTER_FLAG_CLEAR		0x00U
#define _SAU_IIC_SEND_FLAG			0x01U
#define _SAU_IIC_RECEIVE_FLAG		0x02U
#define _SAU_IIC_SENDED_ADDRESS_FLAG		0x04U

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Serial Clock Select Register m (SPSm)
*/
/* Operating mode and clear mode selection (PRS003 - PRS000) */
#define _0000_SAU_CK00_FCLK_0            (0x0000U)    /* ck00 - fCLK */
#define _0001_SAU_CK00_FCLK_1            (0x0001U)    /* ck00 - fCLK/2^1 */
#define _0002_SAU_CK00_FCLK_2            (0x0002U)    /* ck00 - fCLK/2^2 */
#define _0003_SAU_CK00_FCLK_3            (0x0003U)    /* ck00 - fCLK/2^3 */
#define _0004_SAU_CK00_FCLK_4            (0x0004U)    /* ck00 - fCLK/2^4 */
#define _0005_SAU_CK00_FCLK_5            (0x0005U)    /* ck00 - fCLK/2^5 */
#define _0006_SAU_CK00_FCLK_6            (0x0006U)    /* ck00 - fCLK/2^6 */
#define _0007_SAU_CK00_FCLK_7            (0x0007U)    /* ck00 - fCLK/2^7 */
#define _0008_SAU_CK00_FCLK_8            (0x0008U)    /* ck00 - fCLK/2^8 */
#define _0009_SAU_CK00_FCLK_9            (0x0009U)    /* ck00 - fCLK/2^9 */
#define _000A_SAU_CK00_FCLK_10           (0x000AU)    /* ck00 - fCLK/2^10 */
#define _000B_SAU_CK00_FCLK_11           (0x000BU)    /* ck00 - fCLK/2^11 */
#define _000C_SAU_CK00_FCLK_12           (0x000CU)    /* ck00 - fCLK/2^12 */
#define _000D_SAU_CK00_FCLK_13           (0x000DU)    /* ck00 - fCLK/2^13 */
#define _000E_SAU_CK00_FCLK_14           (0x000EU)    /* ck00 - fCLK/2^14 */
#define _000F_SAU_CK00_FCLK_15           (0x000FU)    /* ck00 - fCLK/2^15 */
/* Operating mode and clear mode selection (PRS013 - PRS010) */
#define _0000_SAU_CK01_FCLK_0            (0x0000U)    /* ck01 - fCLK */
#define _0010_SAU_CK01_FCLK_1            (0x0010U)    /* ck01 - fCLK/2^1 */
#define _0020_SAU_CK01_FCLK_2            (0x0020U)    /* ck01 - fCLK/2^2 */
#define _0030_SAU_CK01_FCLK_3            (0x0030U)    /* ck01 - fCLK/2^3 */
#define _0040_SAU_CK01_FCLK_4            (0x0040U)    /* ck01 - fCLK/2^4 */
#define _0050_SAU_CK01_FCLK_5            (0x0050U)    /* ck01 - fCLK/2^5 */
#define _0060_SAU_CK01_FCLK_6            (0x0060U)    /* ck01 - fCLK/2^6 */
#define _0070_SAU_CK01_FCLK_7            (0x0070U)    /* ck01 - fCLK/2^7 */
#define _0080_SAU_CK01_FCLK_8            (0x0080U)    /* ck01 - fCLK/2^8 */
#define _0090_SAU_CK01_FCLK_9            (0x0090U)    /* ck01 - fCLK/2^9 */
#define _00A0_SAU_CK01_FCLK_10           (0x00A0U)    /* ck01 - fCLK/2^10 */
#define _00B0_SAU_CK01_FCLK_11           (0x00B0U)    /* ck01 - fCLK/2^11 */
#define _00C0_SAU_CK01_FCLK_12           (0x00C0U)    /* ck01 - fCLK/2^12 */
#define _00D0_SAU_CK01_FCLK_13           (0x00D0U)    /* ck01 - fCLK/2^13 */
#define _00E0_SAU_CK01_FCLK_14           (0x00E0U)    /* ck01 - fCLK/2^14 */
#define _00F0_SAU_CK01_FCLK_15           (0x00F0U)    /* ck01 - fCLK/2^15 */

/*
    Serial Mode Register mn (SMRmn)
*/
#define _0020_SAU_SMRMN_INITIALVALUE     (0x0020U)
/* Selection of macro clock (MCK) of channel n (CKSmn) */
#define _0000_SAU_CLOCK_SELECT_CK00      (0x0000U)    /* operation clock CK0 set by PRS register */ 
#define _8000_SAU_CLOCK_SELECT_CK01      (0x8000U)    /* operation clock CK1 set by PRS register */
/* Selection of transfer clock (TCLK) of channel n (CCSmn) */
#define _0000_SAU_CLOCK_MODE_CKS         (0x0000U)    /* divided operation clock MCK specified by CKSmn bit */  
#define _4000_SAU_CLOCK_MODE_TI0N        (0x4000U)    /* clock input from SCK pin (slave transfer in CSI mode) */
/* Selection of start trigger source (STSmn) */
#define _0000_SAU_TRIGGER_SOFTWARE       (0x0000U)    /* only software trigger is valid */
#define _0100_SAU_TRIGGER_RXD            (0x0100U)    /* valid edge of RXD pin */
/* Controls inversion of level of receive data of channel n in UART mode (SISmn0) */
#define _0000_SAU_EDGE_FALL              (0x0000U)    /* falling edge is detected as the start bit */
#define _0040_SAU_EDGE_RISING            (0x0040U)    /* rising edge is detected as the start bit */
/* Setting of operation mode of channel n (MDmn2, MDmn1) */
#define _0000_SAU_MODE_CSI               (0x0000U)    /* CSI mode */
#define _0002_SAU_MODE_UART              (0x0002U)    /* UART mode */
#define _0004_SAU_MODE_IIC               (0x0004U)    /* simplified IIC mode */
/* Selection of interrupt source of channel n (MDmn0) */
#define _0000_SAU_TRANSFER_END           (0x0000U)    /* transfer end interrupt */
#define _0001_SAU_BUFFER_EMPTY           (0x0001U)    /* buffer empty interrupt */

/*
    Serial Communication Operation Setting Register mn (SCRmn)
*/
/* Setting of operation mode of channel n (TXEmn, RXEmn) */
#define _0000_SAU_NOT_COMMUNICATION      (0x0000U)    /* does not start communication */
#define _4000_SAU_RECEPTION              (0x4000U)    /* reception only */
#define _8000_SAU_TRANSMISSION           (0x8000U)    /* transmission only */
#define _C000_SAU_RECEPTION_TRANSMISSION (0xC000U)    /* reception and transmission */
/* Selection of data and clock phase in CSI mode (DAPmn, CKPmn) */
#define _0000_SAU_TIMING_1               (0x0000U)    /* type 1 */
#define _1000_SAU_TIMING_2               (0x1000U)    /* type 2 */
#define _2000_SAU_TIMING_3               (0x2000U)    /* type 3 */
#define _3000_SAU_TIMING_4               (0x3000U)    /* type 4 */
/* Selection of masking of error interrupt signal (EOCmn) */
#define _0000_SAU_INTSRE_MASK            (0x0000U)    /* masks error interrupt INTSREx */
#define _0400_SAU_INTSRE_ENABLE          (0x0400U)    /* enables generation of error interrupt INTSREx */
/* Setting of parity bit in UART mode (PTCmn1 - PTCmn0) */
#define _0000_SAU_PARITY_NONE            (0x0000U)    /* none parity */
#define _0100_SAU_PARITY_ZERO            (0x0100U)    /* zero parity */
#define _0200_SAU_PARITY_EVEN            (0x0200U)    /* even parity */
#define _0300_SAU_PARITY_ODD             (0x0300U)    /* odd parity */
/* Selection of data transfer sequence in CSI and UART modes (DIRmn) */
#define _0000_SAU_MSB                    (0x0000U)    /* MSB */
#define _0080_SAU_LSB                    (0x0080U)    /* LSB */
/* Setting of stop bit in UART mode (SLCmn1, SLCmn0) */
#define _0000_SAU_STOP_NONE              (0x0000U)    /* none stop bit */
#define _0010_SAU_STOP_1                 (0x0010U)    /* 1 stop bit */
#define _0020_SAU_STOP_2                 (0x0020U)    /* 2 stop bits */
/* Setting of data length in CSI and UART modes (DLSmn2 - DLSmn0) */
#define _0005_SAU_LENGTH_9               (0x0005U)    /* 9-bit data length */
#define _0006_SAU_LENGTH_7               (0x0006U)    /* 7-bit data length */
#define _0007_SAU_LENGTH_8               (0x0007U)    /* 8-bit data length */

/*
    Serial Output Level Register m (SOLm)
*/
/* Selects inversion of the level of the transmit data of channel n in UART mode */
#define _0000_SAU_CHANNEL0_NORMAL        (0x0000U)    /* normal bit level */
#define _0001_SAU_CHANNEL0_INVERTED      (0x0001U)    /* inverted bit level */
#define _0000_SAU_CHANNEL1_NORMAL        (0x0000U)    /* normal bit level */
#define _0002_SAU_CHANNEL1_INVERTED      (0x0002U)    /* inverted bit level */
#define _0000_SAU_CHANNEL2_NORMAL        (0x0000U)    /* normal bit level */
#define _0004_SAU_CHANNEL2_INVERTED      (0x0004U)    /* inverted bit level */
#define _0000_SAU_CHANNEL3_NORMAL        (0x0000U)    /* normal bit level */
#define _0008_SAU_CHANNEL3_INVERTED      (0x0008U)    /* inverted bit level */

/*
    Noise Filter Enable Register 0 (NFEN0)
*/
/* Use of noise filter */
#define _00_SAU_RXD3_FILTER_OFF          (0x00U)      /* noise filter off */
#define _40_SAU_RXD3_FILTER_ON           (0x40U)      /* noise filter on */
#define _00_SAU_RXD2_FILTER_OFF          (0x00U)      /* noise filter off */
#define _10_SAU_RXD2_FILTER_ON           (0x10U)      /* noise filter on */
#define _00_SAU_RXD1_FILTER_OFF          (0x00U)      /* noise filter off */
#define _04_SAU_RXD1_FILTER_ON           (0x04U)      /* noise filter on */
#define _00_SAU_RXD0_FILTER_OFF          (0x00U)      /* noise filter off */
#define _01_SAU_RXD0_FILTER_ON           (0x01U)      /* noise filter on */

/*
    Format of Serial Status Register mn (SSRmn)
*/
/* Communication status indication flag of channel n (TSFmn) */
#define _0040_SAU_UNDER_EXECUTE          (0x0040U)    /* communication is under execution */
/* Buffer register status indication flag of channel n (BFFmn) */
#define _0020_SAU_VALID_STORED           (0x0020U)    /* valid data is stored in the SDRmn register */
/* Framing error detection flag of channel n (FEFmn) */
#define _0004_SAU_FRAM_ERROR             (0x0004U)    /* a framing error occurs during UART reception */
/* Parity error detection flag of channel n (PEFmn) */
#define _0002_SAU_PARITY_ERROR           (0x0002U)    /* a parity error occurs or ACK is not detected */
/* Overrun error detection flag of channel n (OVFmn) */
#define _0001_SAU_OVERRUN_ERROR          (0x0001U)    /* an overrun error occurs */

/*
    Serial Channel Start Register m (SSm)
*/
/* Operation start trigger of channel 0 (SSm0) */
#define _0000_SAU_CH0_START_TRG_OFF      (0x0000U)    /* no trigger operation */
#define _0001_SAU_CH0_START_TRG_ON       (0x0001U)    /* sets SEm0 to 1 and enters the communication wait status */
/* Operation start trigger of channel 1 (SSm1) */
#define _0000_SAU_CH1_START_TRG_OFF      (0x0000U)    /* no trigger operation */
#define _0002_SAU_CH1_START_TRG_ON       (0x0002U)    /* sets SEm1 to 1 and enters the communication wait status */
/* Operation start trigger of channel 2 (SSm2) */
#define _0000_SAU_CH2_START_TRG_OFF      (0x0000U)    /* no trigger operation */
#define _0004_SAU_CH2_START_TRG_ON       (0x0004U)    /* sets SEm2 to 1 and enters the communication wait status */
/* Operation start trigger of channel 3 (SSm3) */
#define _0000_SAU_CH3_START_TRG_OFF      (0x0000U)    /* no trigger operation */
#define _0008_SAU_CH3_START_TRG_ON       (0x0008U)    /* sets SEm3 to 1 and enters the communication wait status */

/*
    Serial Channel Stop Register m (STm)
*/
/* Operation stop trigger of channel 0 (STm0) */
#define _0000_SAU_CH0_STOP_TRG_OFF       (0x0000U)    /* no trigger operation */
#define _0001_SAU_CH0_STOP_TRG_ON        (0x0001U)    /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 1 (STm1) */
#define _0000_SAU_CH1_STOP_TRG_OFF       (0x0000U)    /* no trigger operation */
#define _0002_SAU_CH1_STOP_TRG_ON        (0x0002U)    /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 2 (STm2) */
#define _0000_SAU_CH2_STOP_TRG_OFF       (0x0000U)    /* no trigger operation */
#define _0004_SAU_CH2_STOP_TRG_ON        (0x0004U)    /* operation is stopped (stop trigger is generated) */
/* Operation stop trigger of channel 3 (STm3) */
#define _0000_SAU_CH3_STOP_TRG_OFF       (0x0000U)    /* no trigger operation */
#define _0008_SAU_CH3_STOP_TRG_ON        (0x0008U)    /* operation is stopped (stop trigger is generated) */

/*
    Format of Serial Flag Clear Trigger Register mn (SIRmn)
*/
/* Clear trigger of overrun error flag of channel n (OVCTmn) */
#define _0001_SAU_SIRMN_OVCTMN           (0x0001U)
/* Clear trigger of parity error flag of channel n (PECTmn) */
#define _0002_SAU_SIRMN_PECTMN           (0x0002U)
/* Clear trigger of framing error of channel n (FECTMN) */
#define _0004_SAU_SIRMN_FECTMN           (0x0004U)

/*
    Serial Output Enable Register m (SOEm)
*/
/* Serial output enable/disable of channel 0 (SOEm0) */
#define _0001_SAU_CH0_OUTPUT_ENABLE      (0x0001U)    /* stops output by serial communication operation */
#define _0000_SAU_CH0_OUTPUT_DISABLE     (0x0000U)    /* enables output by serial communication operation */
/* Serial output enable/disable of channel 1 (SOEm1) */
#define _0002_SAU_CH1_OUTPUT_ENABLE      (0x0002U)    /* stops output by serial communication operation */
#define _0000_SAU_CH1_OUTPUT_DISABLE     (0x0000U)    /* enables output by serial communication operation */
/* Serial output enable/disable of channel 2 (SOEm2) */
#define _0004_SAU_CH2_OUTPUT_ENABLE      (0x0004U)    /* stops output by serial communication operation */
#define _0000_SAU_CH2_OUTPUT_DISABLE     (0x0000U)    /* enables output by serial communication operation */
/* Serial output enable/disable of channel 3 (SOEm3) */
#define _0008_SAU_CH3_OUTPUT_ENABLE      (0x0008U)    /* stops output by serial communication operation */
#define _0000_SAU_CH3_OUTPUT_DISABLE     (0x0000U)    /* enables output by serial communication operation */

/*
    Serial Output Register m (SOm)
*/
/* Serial data output of channel 0 (SOm0) */
#define _0000_SAU_CH0_DATA_OUTPUT_0      (0x0000U)    /* Serial data output value is "0" */
#define _0001_SAU_CH0_DATA_OUTPUT_1      (0x0001U)    /* Serial data output value is "1" */
/* Serial data output of channel 1 (SOm1) */
#define _0000_SAU_CH1_DATA_OUTPUT_0      (0x0000U)    /* Serial data output value is "0" */
#define _0002_SAU_CH1_DATA_OUTPUT_1      (0x0002U)    /* Serial data output value is "1" */
/* Serial data output of channel 2 (SOm2) */
#define _0000_SAU_CH2_DATA_OUTPUT_0      (0x0000U)    /* Serial data output value is "0" */
#define _0004_SAU_CH2_DATA_OUTPUT_1      (0x0004U)    /* Serial data output value is "1" */
/* Serial data output of channel 3 (SOm3) */
#define _0000_SAU_CH3_DATA_OUTPUT_0      (0x0000U)    /* Serial data output value is "0" */
#define _0008_SAU_CH3_DATA_OUTPUT_1      (0x0008U)    /* Serial data output value is "1" */
/* Serial clock output of channel 0 (CKOm0) */
#define _0000_SAU_CH0_CLOCK_OUTPUT_0     (0x0000U)    /* Serial clock output value is "0" */
#define _0100_SAU_CH0_CLOCK_OUTPUT_1     (0x0100U)    /* Serial clock output value is "1" */
/* Serial clock output of channel 1 (CKOm1) */
#define _0000_SAU_CH1_CLOCK_OUTPUT_0     (0x0000U)    /* Serial clock output value is "0" */
#define _0200_SAU_CH1_CLOCK_OUTPUT_1     (0x0200U)    /* Serial clock output value is "1" */
/* Serial clock output of channel 2 (CKOm2) */
#define _0000_SAU_CH2_CLOCK_OUTPUT_0     (0x0000U)    /* Serial clock output value is "0" */
#define _0400_SAU_CH2_CLOCK_OUTPUT_1     (0x0400U)    /* Serial clock output value is "1" */
/* Serial clock output of channel 3 (CKOm3) */
#define _0000_SAU_CH3_CLOCK_OUTPUT_0     (0x0000U)    /* Serial clock output value is "0" */
#define _0800_SAU_CH3_CLOCK_OUTPUT_1     (0x0800U)    /* Serial clock output value is "1" */

/*
    SAU Standby Control Register m (SSCm)
*/
/* SAU Standby Wakeup Control Bit (SWC) */
#define _0000_SAU_CH0_SNOOZE_OFF         (0x0000U)    /* disable start function from STOP state of chip */
#define _0001_SAU_CH0_SNOOZE_ON          (0x0001U)    /* enable start function from STOP state of chip */

/* SAU used flag */
#define _00_SAU_IIC_MASTER_FLAG_CLEAR    (0x00U)
#define _01_SAU_IIC_SEND_FLAG            (0x01U)
#define _02_SAU_IIC_RECEIVE_FLAG         (0x02U)
#define _04_SAU_IIC_SENDED_ADDRESS_FLAG  (0x04U)

/*
    Input switch control register (ISC)
*/
/* Channel 0 SSI00 input setting in CSI communication and slave mode (SSIE00) */
#define _00_SAU_SSI00_UNUSED             (0x00U)      /* disables SSI00 pin input */
#define _80_SAU_SSI00_USED               (0x80U)      /* enables SSI00 pin input */


/******************************************************************************
**  Macro define
*******************************************************************************
*/
#define _CE00_UART1_RECEIVE_DIVISOR		0xCE00U
#define _CE00_UART1_TRANSMIT_DIVISOR	        0xCE00U
#define _CE00_UART0_RECEIVE_DIVISOR		0xCE00U
#define _CE00_UART0_TRANSMIT_DIVISOR	        0xCE00U
#endif // _SAU_H
/*-------------------------------------------------------------------------*
 * End of File:  SAU.h
 *-------------------------------------------------------------------------*/
