//*********************************************************************************************************************************************************************************************************************************************
// clsColourCtrl.h
//
// Class containing all the functionality to manage LED colours and display brightness, etc, etc...
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSCOLOURCTRL_H
#define CLSCOLOURCTRL_H

#include <FastLED.h>
#include "Enums.h"

#define MAX_RANDOM_COLOUR_PALLET  32

class clsColourCtrl
{
  public:
    void begin(void);    // eState pColourMode);
    
    void setColourPallet(bool pDisplayMode, eColours pColourMode);
    int setBaselineColour(int pIndex, eWavePattern pWavePattern);
        
    CRGB getColour(int pIndex);
    CRGB getRandomColour(void);
    CRGB getAccentColour(void);
    CRGB getNextColour(void);    // Used with setBaselineColour...

  private:

    CRGB colourPallette[MAX_RANDOM_COLOUR_PALLET];
    CRGB accentColour;

    int numberOfColourInPallette = MAX_RANDOM_COLOUR_PALLET - 1;
    
    int baselineColourIndex = 0;
    eUpDown nextColourDirection = UP;
    
    eColours currentColourMode = COLOUR_NOTSET;

    // Refer the to HUE Scale for values:  https://en.wikipedia.org/wiki/Hue#/media/File:HueScale.svg
    int     startColour,
            endColour,
            saturation,
            brightness;
    int fadeIdx;
};

#endif
