//*********************************************************************************************************************************************************************************************************************************************
// clsMatrixCtrl.h
//
// Class containing all the functionality to manage display Matrix
//
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSMATRIXCTRL_H
#define CLSMATRIXCTRL_H

#include <FastLED.h>
#include "Enums.h"

#include "clsVUData.h"
#include "clsColourCtrl.h"
#include "clsPixelBuffer.h"

#define MATRIX_DATA_PIN     A1
#define MATRIX_LEDS        260

class clsMatrixCtrl
{
  public:

    void begin(clsVUData *pVUData, clsColourCtrl *pClsColourCtrl);
    void displaySample(eMatrixPattern pPatternMode, int pPeakVal);
    void turnOffTheLights(void);

  private:

    CRGB leds[MATRIX_LEDS];
    clsPixelBuffer pixelBuffer;
    int localPeakStore[2][MATRIX_HEIGHT];

    clsVUData *VUData;

    eMatrixPattern previousPattern = MATRIXPATTERN_NOTSET;
    bool displayCurrentlyOn = false;

    clsColourCtrl *colourCtrl;

    int maxPixelBufSize = 0;
    int baseColour = 0;

    inline void setPixelVal(int pRow, int pCol, CRGB pPixel);
    inline CRGB getPixelVal(int pRow, int pCol);
    void resetLeds();
    void fadePixels(int pFadeSpeed);
    void shiftColumns(eLeftRight pLeftRight);
    void startUp(void);

    // PATTERN GENERATORS
    void patVU(eLeftRight pLeftRight);
    void patVUPeak(eLeftRight pLeftRight);
    void patVUHue(eLeftRight pLeftRight);
    void patVUHueRev(eLeftRight pLeftRight);
    void patVUSide(eLeftRight pLeftRight);
    void patVUSideBar(eLeftRight pLeftRight);
    void patVUPulsar(eLeftRight pLeftRight);
    void patPixBuffer(void);
};

#endif
