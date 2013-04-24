#ifndef _EINK_DRIVER_H
#define _EINK_DRIVER_H

#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

typedef unsigned long long uint64_t;

// for use with a 80 pin RL78/G13
#define TRUE  (1)
#define FALSE (0)

#define DIR_OUTPUT_ON   0
#define DIR_OUTPUT_OFF  1

// Connector Related information
#define NUM_SEGMENTS    8

// Default Pin Locations
#define TOP_PLANE       EINK_P01
#define TOP_PLANE_DIR   EINK_P01_DIR

// Pin Register
#define EINK_P01		P1_bit.no2
#define EINK_P02		P8_bit.no6
#define EINK_P03		P8_bit.no7
#define EINK_P04		P8_bit.no0
#define EINK_P05		P14_bit.no7
#define EINK_P06		P15_bit.no3
#define EINK_P07		P15_bit.no4
#define EINK_P08		P8_bit.no1


// Mode/Direction Registers
#define EINK_P01_DIR		PM1_bit.no2
#define EINK_P02_DIR		PM8_bit.no6
#define EINK_P03_DIR		PM8_bit.no7
#define EINK_P04_DIR		PM8_bit.no0
#define EINK_P05_DIR		PM14_bit.no7
#define EINK_P06_DIR		PM15_bit.no3
#define EINK_P07_DIR		PM15_bit.no4
#define EINK_P08_DIR		PM8_bit.no1


//FORMATTING OPTIONS
#define FORMAT_TYPE             0x0F
#define TYPE_BLANK              0x00
#define TYPE_NUM                0x01
#define TYPE_MSG                0x02
#define TYPE_SCROLL             0x03
#define TYPE_OTHER              0x0F

#define FORMAT_NUM_32           0x10
#define FORMAT_RETAIN_OTHER     0x20
#define FORMAT_LZ_BLANK         0x40
#define FORMAT_NUM_16           0x80

#define LCD_BLANK               27

#define NUM_FENCE           5
#define TOTAL_DRIVE_TIME    (1000)          // In milliseconds
#define DRIVE_TIME_IN_5MS   (TOTAL_DRIVE_TIME/5)
#define PICKET_SHORT        (DRIVE_TIME_IN_5MS / NUM_FENCE)
#define SEG_DELAY           (PICKET_SHORT*10)

#define TWO_SECOND_COUNTS   400
#define SET_FSM_DELAY(a)        {delayFsmTicks = a; delayFsmFlag = FALSE;}

// Update styles
enum {
    EINK_SLIDE_SHOW_FV,
    EINK_SLIDE_SHOW_RV,
    EINK_PICKET_FENCE,
    EINK_GLOBAL_FV,
    EINK_GLOBAL_RV,
    EINK_INVERT
};

// states of the FSM
enum {
    FSM_IDLE,
    FSM_WAIT,
    FSM_START_UPDATE,
    FSM_UPDATE_COMPLETE
};

struct EINK_SCREEN_DEF {
    uint8_t SCR_NAME[6];
    uint16_t numDisplayZones;
    uint8_t* displayZoneInformation;
    uint8_t numPinConnections;
    uint8_t* pinToSegmentConnectionInformation;
    uint8_t numValidAsciiToSegmentMappings;
    uint8_t *asciiToSegmentMappingInformation;
    uint8_t currMapSegs;
    uint8_t* alternateAsciiToSegmentMap;
    uint8_t* defaultAsciiToSegmentMap;
};

struct EINK_DISPLAY_TIMING 
{
  uint8_t timerInterval;
  uint16_t picketFenceInterval;
  uint16_t segmentUpdateInterval;
  uint8_t updateType;
  uint8_t backgroundColor;

};

extern int16_t delayFsmTicks;
extern uint8_t delayFsmFlag;
extern uint8_t twoSecondFlag;
extern uint8_t mappingSwapped;
extern uint8_t curDisplay[NUM_SEGMENTS];
extern uint8_t nextDisplay[NUM_SEGMENTS];
extern uint8_t updateRequest;

extern uint8_t changingState;
extern uint8_t updateType;
extern uint8_t updateNum;

/****Driver functions****/
// Place this function in a timer/ main routine
void TimerTick(void);       
// Place this function in a timer/ main routine
void EinkDisplayFSM (void); 
// Call this function once for initialization only. It should setup the screen.
void initEinkDisplayDriver(struct EINK_DISPLAY_TIMING *currentTiming);  
// Call this function to update which segments to turn ON. Set updateRequest to "1" to perform a physical update to screen.
void DisplayNumber (uint8_t * pnumber, uint8_t location, uint8_t formatCode, struct EINK_SCREEN_DEF* this_screen);  
// This function builds on DisplayNumber
uint8_t DisplayOnEinkScreen(uint8_t * pnumber, uint8_t location, uint8_t formatCode, struct EINK_SCREEN_DEF* this_screen, struct EINK_DISPLAY_TIMING *currentTiming);

#endif //_EINK_DRIVER_H