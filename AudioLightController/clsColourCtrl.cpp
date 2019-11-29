//*********************************************************************************************************************************************************************************************************************************************
// clsColourCtrl.cpp
//
// Class containing all the functionality to manage Matrix and LED Strip colour pallets
//
//*********************************************************************************************************************************************************************************************************************************************
// #define DEBUG                           TRUE
// #define DEBUG_HIFREQ                    TRUE
// #define TRACE                           TRUE
// #define TRACE_HIFREQ                    TRUE
// #define INFO                            TRUE
#define ERROR                           TRUE
#define WARNING                         TRUE
#include "Debug.h"

#include "clsColourCtrl.h"

void clsColourCtrl::begin(void)       // eState pColourMode)
{
  trace("clsColourCtrl");

  // Seed the randomNumber...
  randomSeed(analogRead(A0));

  // FastLED.setBrightness(brightness);
}

void clsColourCtrl::setColourPallet(bool pDisplayMode, eColours pColourMode) {
  trace("IN>>> Display=%d, ColourMode=%d", pDisplayMode, pColourMode);

  // If colours have changed, then regenerate the colour pallette...
  if (pColourMode != currentColourMode) {
    if (pDisplayMode == true) {
      saturation = 255;
      brightness = 128;

      accentColour = CRGB(255, 255, 255);

      switch (pColourMode) {
        case COLOUR_NOTSET:
          break;
        case red:
          debug("REDREDRED");
          startColour = HUE_RED;
          endColour = HUE_YELLOW;
          break;
        case blue:
          debug("BLUEBLUEBLUE");
          startColour = HUE_BLUE;
          endColour = HUE_PURPLE;
          break;
        case green:
          debug("GREENGREENGREEN");
          startColour = HUE_GREEN;
          endColour = HUE_AQUA;
          break;
        case yellow:
          startColour = HUE_YELLOW;
          endColour = HUE_GREEN;
          accentColour = CRGB(HUE_ORANGE, saturation, brightness);
          break;
        case pink:
          startColour = HUE_PINK;
          endColour = HUE_RED;
          break;
        case purple:
          startColour = HUE_BLUE;
          endColour = HUE_PINK;
          break;
        case white:
          startColour = 0;
          endColour = 255;
          accentColour = CRGB(HUE_RED, saturation, brightness);
          saturation = 64;
          break;
        case red_blue:
          startColour = HUE_BLUE;
          endColour = HUE_RED;
          break;
        case green_yellow:
          startColour = HUE_YELLOW;
          endColour = HUE_GREEN;
          break;
        case blue_green:
          startColour = HUE_GREEN;
          endColour = HUE_BLUE;
          break;
        case rainbow:
          startColour = 0;
          endColour = 255;
          break;
        case pastel:
          startColour = 0;
          endColour = 255;
          saturation = 196;
          break;
      }

      debug("Going to fill_gradient.. %d to %d", startColour, endColour );

      fill_gradient(colourPallette, 0, CHSV(startColour, saturation , brightness), numberOfColourInPallette, CHSV(endColour, saturation, brightness), FORWARD_HUES);
      fadeIdx = 100;

      currentColourMode = pColourMode;
      debug("Done with fill_gradient..");
    }
    else {
      debug("Light OFF..");
      for (int i = 0; i <= numberOfColourInPallette; i++)
        colourPallette[i] = CRGB(0, 0, 0);
      accentColour = CRGB(0, 0, 0);
    }
  }

  trace("<<<OUT");
}

CRGB clsColourCtrl::getColour(int pIndex)
{
  trace_hifreq("clsColourCtrl");

  if (pIndex > numberOfColourInPallette)
  {
    error("Index out of range (%d)", pIndex);
    return (colourPallette[0]);
  }
  else
    return (colourPallette[pIndex]);
}

CRGB clsColourCtrl::getRandomColour(void)
{
  trace_hifreq("clsColourCtrl");
  return (colourPallette[random(0, numberOfColourInPallette)]);
}

CRGB clsColourCtrl::getAccentColour(void)
{
  trace_hifreq("clsColourCtrl");
  return (accentColour);
}

// Used with the getNextColour function...
// Returns:
//    newBaselineColour set to pIndex or new value if out of range
//

int localColourIdx = 0;

int clsColourCtrl::setBaselineColour(int pPrevIndex, eWavePattern pWavePattern)
{
  int newBaselineColour = pPrevIndex;

  if ( nextColourDirection == UP) {
    if (pPrevIndex >=  numberOfColourInPallette) {
      newBaselineColour = numberOfColourInPallette - 1;       // Default it first (good for most scenarios....
      switch (pWavePattern) {
        case Triangle:    newBaselineColour = 0;        break;
        case Sawtooth:    nextColourDirection = DOWN;   break;
        case Sinewave:    nextColourDirection = DOWN;   break;
        case Randomise:   break;
      }
    }
    else
      newBaselineColour = pPrevIndex + 1;
  }
  else {
    if (pPrevIndex <= 0) {
      newBaselineColour = 0;                                  // Default it first (good for most scenarios....
      switch (pWavePattern) {
        case Triangle:    newBaselineColour = numberOfColourInPallette - 1;   break;
        case Sawtooth:    nextColourDirection = UP;                           break;
        case Sinewave:    nextColourDirection = UP;                           break;
        case Randomise:   break;
      }
    }
    else
      newBaselineColour = pPrevIndex - 1;
  }

  debug_hifreq("basecolour = %d (%d) - dir = %d", newBaselineColour, pPrevIndex, nextColourDirection);

  localColourIdx = newBaselineColour;
  return newBaselineColour;
}


CRGB clsColourCtrl::getNextColour(void)    // Used with setBaselineColour...
{
  if ( nextColourDirection == UP) {
    ++localColourIdx;
    if (localColourIdx >=  numberOfColourInPallette)
      localColourIdx = numberOfColourInPallette;
  }
  else {
    --localColourIdx;
    if (localColourIdx < 0)
      localColourIdx = 0;
  }

  debug_hifreq("localColourIdx = %d", localColourIdx);

  return colourPallette[localColourIdx];
}
