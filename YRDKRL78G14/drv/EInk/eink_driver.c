#include "eink_driver.h"
#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "math.h"

#include "r_cg_timer.h"

// From eink_control.c
const uint8_t allZeros[NUM_SEGMENTS] = {0};
uint8_t changingState = 0;
// counter to make sure that a global update is done every few
uint8_t updateNum;
// the state of the FSM
uint8_t curState;
// The next state of the FSM
uint8_t nextState;
// the step in updating
uint8_t updateStep = 0;
// current display
uint8_t curDisplay[NUM_SEGMENTS] = {0};
// what the display should show next
uint8_t nextDisplay[NUM_SEGMENTS] = {0};
// the display type
uint8_t updateType = EINK_PICKET_FENCE;
// the default
uint8_t updateTypeDefault = EINK_SLIDE_SHOW_FV;

uint8_t updateRequest = FALSE;

extern uint8_t updateComplete;

// direction, 0 is forward (wh bg), 1 is reverse (bl bg)
uint8_t einkDir = 0;
uint8_t einkNextDir = 0;

//From timing.c

int16_t delayFsmTicks = 0;
int16_t segDelayHolder = SEG_DELAY;
int16_t picketShortHolder = PICKET_SHORT;

uint8_t delayFsmFlag = TRUE;

extern int16_t segDelayHolder;
extern int16_t picketShortHolder;

// From eink_display.c
uint8_t dpLocation = 1;
uint8_t swapAvailable = 1;
uint8_t dummy001 = 0;
uint8_t mappingSwapped = 0;

uint8_t* displayBuffer = NULL;
uint8_t  num_bytes_in_displayBuffer;


//Local function declarations

void ClearDisplay (uint8_t dir);
void UpdateScreen (void);
void ChangeEinkState (uint8_t next);
void DisplayNumber (uint8_t * pnumber, uint8_t location, uint8_t formatCode, struct EINK_SCREEN_DEF* this_screen);
void cycleNextDisplayArray(void);


uint32_t f10Pow(uint16_t x, uint8_t y);
uint8_t chooseMapping(uint8_t noOfSegs, struct EINK_SCREEN_DEF* screen1);
uint8_t offsetArrayByN(uint8_t* dispArray, uint8_t offset, uint16_t sizeArray);
uint8_t swapNeeded(struct EINK_SCREEN_DEF* this_screen, uint8_t location);

// This is function which physically changes the pin voltages.
void SetOutput (uint8_t *newData, uint8_t dir)
{
    //    EINK_P01 = newData[1] ^ dir;
    EINK_P02 = newData[2] ^ dir;
    EINK_P03 = newData[3] ^ dir;
    EINK_P04 = newData[4] ^ dir;
    EINK_P05 = newData[5] ^ dir;
    EINK_P06 = newData[6] ^ dir;
    EINK_P07 = newData[7] ^ dir;    
    EINK_P08 = newData[8] ^ dir;

}

void ClearDisplay(uint8_t dir)
{
    
}

// This function deals with the timing for toggling the pins
void UpdateScreen (void)
{
    uint8_t tempData[NUM_SEGMENTS];
    uint8_t i;

    switch (updateType) {
    case EINK_SLIDE_SHOW_FV:
        switch (updateStep) {
        case 0:
            // first, push the foreground image into the background
            SetOutput(curDisplay, !einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 1:
            // now pull out the foreground image
            SetOutput(nextDisplay, einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 2:

            // fall thru: anything about 1
        default:
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }

            break;
        }

        break; // EINK_SLIDE_SHOW_FV

    case EINK_SLIDE_SHOW_RV:
        switch (updateStep) {
        case 0:
            // first, pull the background into the foreground
            SetOutput(curDisplay, !einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 1:
            // now pull out the foreground image
            SetOutput(nextDisplay, einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 2:

            // fall thru: anything above 1
        default:
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }

            break;
        }

        break; // EINK_SLIDE_SHOW_RV

    case EINK_PICKET_FENCE:

        // case fades out the nonblack and in the new black
        if (updateStep == NUM_FENCE) {
            // complete by setting all to [einkDir]
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }
        }
        else {
            if (updateStep & 0x01) {
                // in the odd steps, fade out the black
                for (i = 0; i < NUM_SEGMENTS; i++) {
                    tempData[i] = !(curDisplay[i] > nextDisplay[i]);
                }

                SetOutput(tempData, einkDir);
                TOP_PLANE = !einkDir;
                // hold it
                SET_FSM_DELAY(picketShortHolder);       //ONR: Changed here.
            }
            else {
                // now set all the new blacks to black
                for (i = 0; i < NUM_SEGMENTS; i++) {
                    tempData[i] = (nextDisplay[i] > curDisplay[i]);
                }

                SetOutput(tempData, einkDir);
                TOP_PLANE = einkDir;
                // hold it
                SET_FSM_DELAY(picketShortHolder);       //ONR: Changed here.
            }

            updateStep++;
            ChangeEinkState (FSM_WAIT);
        }

        break;

    case EINK_GLOBAL_FV:
        switch (updateStep) {
        case 0:
            // first, push the foreground image into the background
            SetOutput(curDisplay, !einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 1:
            // now pull out the foreground image
            SetOutput((uint8_t *)allZeros, einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 2:
            // now pull out the foreground image
            SetOutput(nextDisplay, einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        default:
            // last step
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }

            break;
        }

        break; // EINK_GLOBAL_FV

    case EINK_GLOBAL_RV:
        switch (updateStep) {
        case 0:
            SetOutput(curDisplay, !einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 1:
            // now pull out the foreground image
            SetOutput((uint8_t *)allZeros, !einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 2:
            // now pull out the foreground image
            SetOutput(nextDisplay, einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        default:
            // last step
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }

            break;
        }

        break; // EINK_GLOBAL_RV

    case EINK_INVERT:
        switch (updateStep) {
        case 0:
            // first, push the foreground image into the background
            SetOutput(curDisplay, !einkDir);
            TOP_PLANE = einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 1:
            // now pull out the foreground image
            SetOutput((uint8_t *)allZeros, einkDir);
            TOP_PLANE = !einkDir;
            // switch the direction
            einkDir = einkNextDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        case 2:
            // now pull out the foreground image
            SetOutput(nextDisplay, einkDir);
            TOP_PLANE = !einkDir;
            // hold it
            SET_FSM_DELAY(segDelayHolder);
            ChangeEinkState (FSM_WAIT);
            // increment step number
            updateStep++;
            break;

        default:
            // last step
            SetOutput((uint8_t *)allZeros, 0);
            TOP_PLANE = 0;
            // since finished
            updateStep = 0;
            // move to idle state
            ChangeEinkState (FSM_UPDATE_COMPLETE);

            // update the curDisplay
            for (i = 0; i < NUM_SEGMENTS; i++) {
                curDisplay[i] = nextDisplay[i];
            }

            break;
        }

        break;

    default:
        // todo: add some fault case here
        break;
    }

    // done
}

// Change the state of the state machine
void ChangeEinkState (uint8_t next)
{
    curState = next;
}

// Finite state machine, called each time thru main. Used to update and keep the screen in check
void EinkDisplayFSM (void)
{
    switch (curState) {
    case FSM_IDLE:

        // just chill here, do nothing. Wait for something
        if (updateRequest == TRUE) {
            ChangeEinkState (FSM_START_UPDATE);
            updateRequest = FALSE;
            changingState = TRUE;
        }

        break;

    case FSM_WAIT:

        // this is waiting for a delay to finish. Wait, then move on
        if (delayFsmFlag == TRUE) {
            UpdateScreen();
        }

        break;

    case FSM_START_UPDATE:
        
        if ( einkDir != einkNextDir) {
            // use the inversion wave
            updateType = EINK_INVERT;
        }
        else if (updateNum >= 10) {
            updateType = EINK_GLOBAL_FV;
        }

        if (updateType >= EINK_GLOBAL_FV) {
            // reset count since refreshing screen
            updateNum = 0;
        }

        UpdateScreen();
        break;

    case FSM_UPDATE_COMPLETE:
        updateNum++;
        // allow requests for new changes
        changingState = FALSE;
        // reset the update type
        updateType = updateTypeDefault;
        // go back to idle and wait
        ChangeEinkState (FSM_IDLE);
        break;

    default:
        break;
    }
}

// call this every x miliseconds from an interrupt
void TimerTick (void)
{
    // timer is enabled when it is set to false.
    if (delayFsmFlag == FALSE) {
        if (--delayFsmTicks <= 0) {
            delayFsmFlag = TRUE;
        }
    }
}

// From eink_display.c



void DisplayNumber (uint8_t * pnumber, uint8_t location, uint8_t formatCode, struct EINK_SCREEN_DEF* this_screen)
{
    /* Local Variables */
    uint32_t number;
    uint8_t subtractor;
    uint8_t lcdRamLocationCode;
    uint8_t bit_index;
    uint8_t err = FALSE;
    int8_t i = 0;
    uint64_t max_lcd_value = 0;
    uint8_t calculated_no_of_pins = 0;
    uint8_t field_offset = 0;

    // Release DisplayBuffer if it was allocated any memory (never forget book keeping)
    if (displayBuffer != NULL) {
        free(displayBuffer);
    }

    // Check to see if the number of segments supported by the display code is
    // compatible with the current screen definition.
    for (i = 0; i < this_screen->numDisplayZones; i++) {
        calculated_no_of_pins += (this_screen->displayZoneInformation[2 * i + 1] * this_screen->displayZoneInformation[2 * i + 0]);
    }

    if (this_screen->numPinConnections > NUM_SEGMENTS || calculated_no_of_pins > NUM_SEGMENTS) {
        err = TRUE;
    }
    else {
        err = FALSE;
        //Allocate bytes to Display buffer
        //Number of digits in field                                       //Number of segments used by digits in field
        num_bytes_in_displayBuffer = this_screen->displayZoneInformation[2 * location + 1];
        displayBuffer = malloc(num_bytes_in_displayBuffer);

        if (displayBuffer == NULL || num_bytes_in_displayBuffer == 0) { // Check if bytes were really allocated and if the field can really display the data
            err = TRUE;     // This should hardly be true
            free(displayBuffer);  // Because malloc somehow can allocate 0 bytes :|
        }
        else {
            err = FALSE;
            memset(displayBuffer, LCD_BLANK, num_bytes_in_displayBuffer); // Set LCD characters to blank(watch will show wrong value)
        }
    }

    if (!(formatCode & FORMAT_RETAIN_OTHER)) {
        // start by clearing next
        memset(nextDisplay, 0, NUM_SEGMENTS);
    }

    if (err == FALSE) {
        /* Code Start */
        if ((formatCode & FORMAT_TYPE) == TYPE_BLANK) {
            // do nothing, since already blank
        }
        else if ((formatCode & FORMAT_TYPE) == TYPE_MSG) {
            for (i = 0; i < num_bytes_in_displayBuffer; i++) {
                displayBuffer[i] = pnumber[i];
            }

            //Calculate field offset here
            for (i = 0; i < location; i++) {
                field_offset += (this_screen->displayZoneInformation[2 * i + 1] * this_screen->displayZoneInformation[2 * i + 0]);
            }

            // Select the correct segment mapping here by using less than equation rather than not equal to
            if (swapNeeded(this_screen, location) == TRUE && swapAvailable) {
                uint8_t* temp;
                temp = (uint8_t*)this_screen->defaultAsciiToSegmentMap;
                this_screen->defaultAsciiToSegmentMap = this_screen->alternateAsciiToSegmentMap;
                this_screen->alternateAsciiToSegmentMap = temp;
                this_screen->currMapSegs = this_screen->displayZoneInformation[2 * location + 0];

                if (mappingSwapped == TRUE) {
                    mappingSwapped = FALSE;
                }
                else {
                    mappingSwapped = TRUE;
                }
            }

            // Map the display segments
            for (i = 0; i < num_bytes_in_displayBuffer; i++) {
                // up to "num_bytes_in_displayBuffer" digits to be changed
                for (bit_index = 0; bit_index < this_screen->displayZoneInformation[2 * location + 0]; bit_index++) {
                    // N Segments in each digit
                    lcdRamLocationCode = this_screen->pinToSegmentConnectionInformation[(i * this_screen->displayZoneInformation[2 * location + 0]) + bit_index + field_offset];

                    if (this_screen->displayZoneInformation[2 * location + 0] > 8) {
                        uint16_t mappingFound = 0;
                        mappingFound = *(uint16_t*) & (this_screen->defaultAsciiToSegmentMap[2 * displayBuffer[i]]); // Multiply by two because we are now reading 16 bit mappings for ASCII table

                        if ((mappingFound) & (0x8000 >> bit_index)) {
                            nextDisplay[lcdRamLocationCode] = 1;
                        }
                        else {
                            nextDisplay[lcdRamLocationCode] = 0;
                        }
                    }
                    else {
                        uint8_t mappingFound = 0;
                        dummy001 = mappingFound = this_screen->defaultAsciiToSegmentMap[displayBuffer[i]];

                        if ((mappingFound) & (0x80 >> bit_index)) {
                            nextDisplay[lcdRamLocationCode] = 1;
                        }
                        else {
                            nextDisplay[lcdRamLocationCode] = 0;
                        }
                    }
                }
            }
        }
        else {
            if ((formatCode & FORMAT_TYPE) == TYPE_NUM) {
                if (formatCode & FORMAT_NUM_16)
                {
                  number = *(uint16_t *)pnumber;
                }
                else if(formatCode & FORMAT_NUM_32)
                {
                  number = *(uint32_t*)pnumber;
                }
                else 
                {
                  number = *(uint8_t*)pnumber;
                }

                // Calculates the maximum value that can be displayed and saturate number
                max_lcd_value = (uint32_t)(f10Pow(10, (num_bytes_in_displayBuffer)) - 1);

                if (number > max_lcd_value) {
                    number = max_lcd_value;
                }

                for (i = 0; i < num_bytes_in_displayBuffer; i++) {
                    uint32_t divisor;
                    divisor = (uint32_t)(f10Pow(10, (num_bytes_in_displayBuffer - 1) - i));
                    subtractor = (number / divisor);
                    displayBuffer[i] = subtractor;
                    number = number - (subtractor * divisor);
                }

                // Convert to ASCII for look up.
                for (i = 0; i < num_bytes_in_displayBuffer; i++) {
                    displayBuffer[i] += 0x30;         //Integer to ASCII Conversion
                }

                // Take care of Leading Zero Blanking
                if (formatCode & FORMAT_LZ_BLANK) {
                    for (i = 0; i < num_bytes_in_displayBuffer-dpLocation; i++) {
                        if (displayBuffer[i] == 0x30) {
                            displayBuffer[i] = LCD_BLANK;
                        }
                        else {
                            break;
                        }
                    }
                }
            }

            //Calculate field offset here
            for (i = 0; i < location; i++) {
                field_offset += (this_screen->displayZoneInformation[2 * i + 1] * this_screen->displayZoneInformation[2 * i + 0]);
            }

            // Select the correct segment mapping here by using less than equation rather than not equal to
            if (swapNeeded(this_screen, location) == TRUE && swapAvailable) {
                uint8_t* temp;
                temp = (uint8_t*)this_screen->defaultAsciiToSegmentMap;
                this_screen->defaultAsciiToSegmentMap = this_screen->alternateAsciiToSegmentMap;
                this_screen->alternateAsciiToSegmentMap = temp;
                this_screen->currMapSegs = this_screen->displayZoneInformation[2 * location + 0];

                if (mappingSwapped == TRUE) {
                    mappingSwapped = FALSE;
                }
                else {
                    mappingSwapped = TRUE;
                }
            }

            // Map the display segments
            for (i = 0; i < num_bytes_in_displayBuffer; i++) {
                // up to "num_bytes_in_displayBuffer" digits to be changed
                for (bit_index = 0; bit_index < this_screen->displayZoneInformation[2 * location + 0]; bit_index++) {  // N Segments in each digit
                    lcdRamLocationCode = this_screen->pinToSegmentConnectionInformation[(i * this_screen->displayZoneInformation[2 * location + 0]) + bit_index + field_offset];

                    if (this_screen->displayZoneInformation[2 * location + 0] > 8) {
                        uint16_t mappingFound = 0;
                        /*
                        mappingFound = (uint16_t)this_screen->defaultAsciiToSegmentMap[displayBuffer[i]];
                        */
                        mappingFound = *(uint16_t*) & (this_screen->defaultAsciiToSegmentMap[2 * displayBuffer[i]]); // Multiply by two because we are now reading 16 bit mappings for ASCII table

                        if ((mappingFound) & (0x8000 >> bit_index)) {
                            nextDisplay[lcdRamLocationCode] = 1;
                        }
                        else {
                            nextDisplay[lcdRamLocationCode] = 0;
                        }
                    }
                    else {
                        uint8_t mappingFound = 0;
                        dummy001 = mappingFound = this_screen->defaultAsciiToSegmentMap[displayBuffer[i]];

                        if ((mappingFound) & (0x80 >> bit_index)) {
                            nextDisplay[lcdRamLocationCode] = 1;
                        }
                        else {
                            nextDisplay[lcdRamLocationCode] = 0;
                        }
                    }
                }
            }
        }
    }
    else {
        // Error reporting can be done here
    }
}

uint32_t f10Pow(uint16_t x, uint8_t y)
{
    uint32_t result = 1;

    for (; y > 0; y--) {
        result *= x;
    }

    return result;
}

uint8_t offsetArrayByN(uint8_t* dispArray, uint8_t offset, uint16_t sizeArray)
{
    uint8_t err = TRUE;
    uint8_t* dummyArray = NULL;
    dummyArray = (uint8_t*)malloc(sizeArray);

    if (dummyArray == NULL) {
        return err;
    }

    //Copy the bottom offset values to the top
    memcpy(dummyArray, ((uint8_t*)dispArray + sizeArray - offset), offset);
    memcpy(dummyArray + offset, dispArray, sizeArray - offset);
    memcpy(dispArray, dummyArray, sizeArray);
    free(dummyArray);
    err = FALSE;
    return err;
}

void cycleNextDisplayArray(void)
{
    offsetArrayByN(nextDisplay, 1, sizeof(nextDisplay));
}

uint8_t swapNeeded(struct EINK_SCREEN_DEF* this_screen, uint8_t location)
{
    int16_t diffDefault = 0;
    int16_t diffAlternate = 0;
    diffDefault = this_screen->asciiToSegmentMappingInformation[0] - this_screen->displayZoneInformation[2 * location + 0];    // Default Mapping
    diffAlternate = this_screen->asciiToSegmentMappingInformation[2] - this_screen->displayZoneInformation[2 * location + 0];    // Alternate Mapping

    if (diffDefault > 0) {
        //Default mapping can support
        if (mappingSwapped == TRUE) { // Check if we are using the alternate mapping
            return TRUE;    // if we are using the alternate mapping, request a swap.
        }
        else {
            return FALSE;    // We are already using the default mapping
        }
    }
    else {
        if (diffAlternate > 0) {
            // Alternate mapping can support
            if (mappingSwapped == TRUE) {
                return FALSE;
            }
            else {
                return TRUE;
            }
        }
    }

    return FALSE;
}

// Eink Driver Initialization routine

void initEinkDisplayDriver(struct EINK_DISPLAY_TIMING *currentTiming)
{
  EINK_P01_DIR = DIR_OUTPUT_ON;
  EINK_P02_DIR = DIR_OUTPUT_ON;
  EINK_P03_DIR = DIR_OUTPUT_ON;
  EINK_P04_DIR = DIR_OUTPUT_ON;
  EINK_P05_DIR = DIR_OUTPUT_ON;
  EINK_P06_DIR = DIR_OUTPUT_ON;
  EINK_P07_DIR = DIR_OUTPUT_ON;
  EINK_P08_DIR = DIR_OUTPUT_ON;

  
  segDelayHolder = currentTiming->segmentUpdateInterval/currentTiming->timerInterval;
  picketShortHolder = currentTiming->picketFenceInterval/currentTiming->timerInterval;
  updateType = currentTiming->updateType;
  updateNum = 10;
  
  // set the registers to indicate blank:
  for (int i = 0; i < NUM_SEGMENTS; i++) {
      nextDisplay[i] = 0;
  }
  
  einkNextDir = currentTiming->backgroundColor;
  updateRequest = 1;
};


// This function builds on the DisplayNumber routine. Display will be updated since updateRequest is set to TRUE.
uint8_t DisplayOnEinkScreen(uint8_t * pnumber, uint8_t location, uint8_t formatCode, struct EINK_SCREEN_DEF* this_screen, struct EINK_DISPLAY_TIMING *currentTiming)
{
  DisplayNumber (pnumber,location,formatCode,this_screen);

  if(!changingState)
  {
    segDelayHolder = currentTiming->segmentUpdateInterval/currentTiming->timerInterval;
    picketShortHolder = currentTiming->picketFenceInterval/currentTiming->timerInterval;
    updateType = currentTiming->updateType;
    
    if(updateRequest==FALSE)
      updateRequest = TRUE;
    
    return TRUE;
  }
  else
    return FALSE;
}