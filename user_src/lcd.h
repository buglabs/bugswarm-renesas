#ifndef _LCD_H_
#define _LCD_H_

/*-------------------------------------------------------------------------*
 * Defines
 *-------------------------------------------------------------------------*/
#define LCD_H 		64
#define LCD_W		96

typedef enum {
	FONT_LOGOS = 2u,
	FONT_LARGE = 1u,
	FONT_SMALL = 0u
}LCDFONT;



uint8_t LCDCenter(uint8_t aStrLen);
uint8_t LCDRight(uint8_t aStrLen);

void LCDInit(void);
void LCDClear(void);
void LCDFont(LCDFONT font);
void LCDClearLine(uint8_t line);
void LCDString(const char *aStr, uint8_t aX, uint8_t aY);
void LCDStringPos(const char *aStr, uint8_t aPos, uint8_t aY);
void LCDStringLine(const char *aStr, uint8_t aX, uint8_t aLine);
void LCDStringLinePos(const char *aStr, uint8_t aPos, uint8_t aLine);
//void LCDStringAlign(const char *aStr, T_LCD_ALIGN aAlign, uint8_t aY);
void LCDChar(char aChar, uint8_t aX, uint8_t aLine);
void LCDCharPos(char aChar, uint8_t aPos, uint8_t aLine);
void LCDPrintf(uint8_t aLine, uint8_t aPos, char *aFormat, ...);
void LCDTest();
void LCDInvert();

#endif
