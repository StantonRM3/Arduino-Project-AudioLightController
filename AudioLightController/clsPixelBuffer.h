//*********************************************************************************************************************************************************************************************************************************************
// clsPixelRingBuffer.h
//
// Class containing all the functionality to manage pixels direction, speed, etc for use with Ring Buffer
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSPIXELBUFFER_H
#define CLSPIXELBUFFER_H

#include "Enums.h"
#include "clsPixel.h"

#define MAX_PIXELS_BUFFERSIZE   16

class clsPixelBuffer
{
  public:
    clsPixel pixels[MAX_PIXELS_BUFFERSIZE];

    void begin(void);
    bool addPixelToStrip(int pPeakLevel, clsColourCtrl *pColourCtrl, byte pEndAction, byte pHowToDie);
    void movePixelsStrip(void);
    bool addPixelToMatrix(int pPeakLevel, clsColourCtrl *pColourCtrl, int pRow, int pCol, eDirection pDirection, int pMaxPixels);
    void movePixelsMatrix(void);

  private:

};

#endif
