//*********************************************************************************************************************************************************************************************************************************************
// clsPixel.h
//
// Class containing all the functionality to remember pixel direction, speed, etc for use with Ring Buffer
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSPIXEL_H
#define CLSPIXEL_H

#include "Enums.h"

#include "clsColourCtrl.h"

class clsPixel
{
  public:
    void initPixel_Strip(int pPeakLevel, clsColourCtrl *pColourCtrl, byte pEndAction, byte pHowToDie);
    void initPixel_Matrix(int pPeakLevel, clsColourCtrl *pColourCtrl, int pRow, int pCol, eDirection pDirection);
    bool pixelStep_Strip(void);
    bool pixelStep_Matrix(void);

    // General variables...
    bool pixelDead = false;       // Check if the pixel is dead or not.
    CRGB colour;                  // Selected pixel colour pColourCtrl

    // Strip specific variables...
    int pixelPos;                 // Current pixel position...
    int width = 0;                // Derive width from peakLevel
    int mySpeed = 0;              // Derive speed from peakLevel

    // Matrix specific variables...
    int pixelRow,                 // Current pixel row and column...
        pixelCol;

  private:
    void printBinary(byte b);

    // Matrix specific variables...
    eDirection pixelDir = Right;  // Where the pixels going...

    // Strip specific variables...
    byte endAction;               // Bitwise variable holding the action to take when a pixel hits an end stop (0 or STRIP_LEDS)
    byte howToDie;                // Bitwise variable holding how the pixel will die/be killed...
    int numCycles = 0,            // Number of time the pixel has hit an end stop (Linked to PIX_LIFETIME)...
        lifespan = 10;            // Hard set the lifespan to max 10 cycles, but may feed this in as init paramemeter...

    eUpDown myDirection = UP;
    bool loopAround = false;      // Loop these pixels around when they reach the end, and reduce mySpeed....

    // General variables...
    clsColourCtrl *colourCtrl;
    int peakLevel = 0;            // Max Level of Pixel
};

#endif
