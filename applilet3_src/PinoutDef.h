/*-------------------------------------------------------------------------*
 * File:  PinOutDef.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Pin out definition file
 *
 *-------------------------------------------------------------------------*/
#ifndef __PINOUTDEF_H
#define __PINOUTDEF_H


//#define LED1	P4_bit.no1    //   P4.1
//#define LED2	P4_bit.no2    //P4.2
//#define LED3	P4_bit.no3     //P4.3

#define BUTTON_enable	P13_bit.no0   //P13.0
#define BUTTON1		P5_bit.no1    //P5.1
#define	BUTTON2		P7_bit.no7    //P7.7
#define	BUTTON3		P3_bit.no1   //P3.1

#define	T_PWROFF	P5_bit.no0   //P5.0
#define M_PWROFF	P5_bit.no4   //P5.4
#define H_enable	P2_bit.no5   //P2.5
#define A_enable	P5_bit.no2   //P5.2
#define R_enable	P5_bit.no3   //P5.3
//#define WIFI_PWRON	P0_bit.no1   //P0.1
#define WIFI_PWRON	P4_bit.no3   //P4.3
#define LCD_PWROFF	P0_bit.no6   //P0.6
#define AX_PWROFF	P0_bit.no5   //P0.5
#define Backlite_on	P1_bit.no7    //P1.7

#define HS_ANI		gANI[1]
#define ALS_ANI		gANI[2]

#define LCD_RS		P0_bit.no0   //P0.0
#define LCD_RESET	P14_bit.no6    //P14.6
#define LCD_CS		P5_bit.no5    //P5.5

#define WIFI_CS		P14_bit.no7   //P14.7
#define WIFI_RESET	P13_bit.no0   //P1.6

#define R_in		P7_bit.no3   //P7.3
#define M_in		P7_bit.no6   //P7.6

#define LED1            P5_bit.no5
#define LED2            P6_bit.no2
#define LED3            P6_bit.no3


#endif
