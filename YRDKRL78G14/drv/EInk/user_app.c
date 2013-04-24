#include "r_cg_macrodriver.h"
#include "user_app.h"
#include "eink_driver.h"
#include "eink_rdk_display.h"

char icons[NUM_SEGMENTS];

struct EINK_SCREEN_DEF* displayScreen = 0;
struct EINK_DISPLAY_TIMING displayParam;
uint8_t dispNum = 8;

void initEink(void)
{
  displayParam.timerInterval = 1;   //(ms)
  displayParam.picketFenceInterval = 20;    //(ms)
  displayParam.segmentUpdateInterval = 500;   //ms
  displayParam.updateType = EINK_SLIDE_SHOW_FV;
  displayParam.backgroundColor = 1;
  
  SetupRdkScreen();
  displayScreen = &rdkScreen;
  
  initEinkDisplayDriver(&displayParam);
}

void setLogo(int logo) {
	DisplayNumber (&dispNum,logo,TYPE_NUM, displayScreen);
	updateRequest=TRUE;
}

void clearLogo(int logo) {
	DisplayNumber (&dispNum,logo,TYPE_NUM|FORMAT_RETAIN_OTHER, displayScreen);
	updateRequest=TRUE;
}

uint8_t doDemo(uint8_t num)
{
  uint8_t retVal;
  switch(num)
  {
  case 1: // Normal function, Eink Logo ON, Power Icon ON
    DisplayNumber (&dispNum,2,TYPE_NUM, displayScreen);     // Clear all previous segments turned ON and Turn ON first ICON
    DisplayNumber (&dispNum,1,TYPE_NUM|FORMAT_RETAIN_OTHER, displayScreen);  // Turn ON second ICON making sure that first ICON is retained
    if(updateRequest==FALSE)
      updateRequest=TRUE;
    retVal = 0;
    
    break;
  case 2: // Halt state, Eink Logo ON, Moon Icon ON
    DisplayNumber (&dispNum,2,TYPE_NUM, displayScreen);
    DisplayNumber (&dispNum,5,TYPE_NUM|FORMAT_RETAIN_OTHER, displayScreen); 
    if(updateRequest==FALSE)
      updateRequest=TRUE;
    
    break;
  case 3: // Snooze state, Eink Logo ON, Clock Icon ON
    DisplayNumber (&dispNum,2,TYPE_NUM, displayScreen);
    DisplayNumber (&dispNum,3,TYPE_NUM|FORMAT_RETAIN_OTHER, displayScreen); 
    if(updateRequest==FALSE)
      updateRequest=TRUE;

    break;
  case 4: // Stop state, Eink Logo ON, Hand Icon ON
    DisplayNumber (&dispNum,2,TYPE_NUM, displayScreen);
    DisplayNumber (&dispNum,4,TYPE_NUM|FORMAT_RETAIN_OTHER, displayScreen); 
    if(updateRequest==FALSE)
      updateRequest=TRUE;
    
    retVal = 1;
    break;
  case 5: // Super cap
    break;
    
  default:
    break;
  }
  return retVal;
}