//*********************************************************************************************************************************************************************************************************************************************
// clsStripCtrl.h
//
// Class containing all the functionality to manage LED strip display
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSSTRIPCTRL_H
#define CLSSTRIPCTRL_H

#include <FastLED.h>
#include "Enums.h"

#include "clsVUData.h"
#include "clsColourCtrl.h"
#include "clsPixelBuffer.h"

#define STRIP_DATA_PIN      A2

class clsStripCtrl
{
  public:
    void begin(clsVUData *pVUData, clsColourCtrl *pClsColourCtrl);
    void displaySample(eStripPattern pPatternMode, int pPeakVal);
    void turnOffTheLights(void);

  private:
    CRGB leds[STRIP_LEDS];

    clsVUData *VUData;
    clsPixelBuffer pixelBuffer;

    // eStripPattern patternMode = STRIPPATTERN_NOTSET;
    clsColourCtrl *colourCtrl;

    bool displayCurrentlyOn = false;

    inline void setPixelVal(int pPixel, CRGB pPixelCol);
    inline CRGB getPixelVal(int pPixel);
    void resetLeds();

    // PATTERN GENERATORS
    void patVU(eLeftRight pLeftRight);
    void patVUPeak(eLeftRight pLeftRight);
    void patVUHue(eLeftRight pLeftRight);
    void patVUHueRev(eLeftRight pLeftRight);
    void patVUSide(eLeftRight pLeftRight);
    void patVUSideBar(eLeftRight pLeftRight);
    void patVUOsc(void);
    void patPixBuffer(void);
};

#endif
