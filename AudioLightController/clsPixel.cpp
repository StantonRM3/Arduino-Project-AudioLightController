//*********************************************************************************************************************************************************************************************************************************************
// clsPixel.cpp
//
// Class containing all the functionality to manage Pixels
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

#include "clsPixel.h"

void clsPixel::initPixel_Strip(int pPeakLevel, clsColourCtrl *pColourCtrl, byte pEndAction, byte pHowToDie)
{
  trace_hifreq("clsPixel");

  pixelPos = 0;
  peakLevel = pPeakLevel;
  colourCtrl = pColourCtrl;
  endAction = pEndAction;
  howToDie = pHowToDie;
  numCycles = 0;

  pixelDead = false;
  pixelDir = Right;
  mySpeed = peakLevel / 20;        // The louder/higher the faster
  if (mySpeed <= 0)                // If peakLevel < previous divisor then we get 0 speed and the pixels never move (or more importantly reach the position where they die and leave space for the next...
    mySpeed = 1;

  width = peakLevel / 10; ;        // The louder/higher the wider
  if (width <= 0)                 // If peakLevel < previous divisor then we get 0 width and the pixels never move (or more importantly reach the position where they die and leave space for the next...
    width = 1;

  colour = pColourCtrl->getRandomColour();                  // Select random colour for this from the provided pColourCtrl

  debug_hifreq("Creating Pixel");
}

void clsPixel::initPixel_Matrix(int pPeakLevel, clsColourCtrl *pColourCtrl, int pRow, int pCol, eDirection pDirection)
{
  trace_hifreq("clsPixel");

  pixelRow = pRow;
  pixelCol = pCol;
  pixelDir = pDirection;

  peakLevel = pPeakLevel;

  colourCtrl = pColourCtrl;

  pixelDead = false;

  colour = pColourCtrl->getRandomColour();                  // Select random colour for this from the provided pColourCtrl

  debug_hifreq("Creating Pixel");
}

void clsPixel::printBinary(byte b) {
  for (int i = 7; i >= 0; i-- )
    Serial.print((b >> i) & 0X01);//shift and select first bit
}

bool clsPixel::pixelStep_Strip(void)
{
  trace_hifreq("clsPixel");

  bool retval = true;     // Returns true if the pixel moves, however false if the pixel needs to be destroyed...

  // So pixel will move up at its speed rating per cycle and down again once it reached the top at 1 step per cycle...
  if (pixelDir == Left)
    pixelPos = pixelPos - (mySpeed);
  else
    pixelPos = pixelPos + (mySpeed);

  // Has the pixel hit the end?...
  if ((pixelPos >= (STRIP_LEDS - 1)) || (pixelPos <= 0) ) {
    ++numCycles;

    // It has hit the end stop and now it will die...
    if ( endAction & PIX_STOP)
      pixelDead = true;
    else {
      //
      // Now check what to do with the item (LOOP, BOUNCE, or RANDOM)...
      //
      // It has hit the end stop so set it to the start/end of the strip...
      if (endAction & PIX_LOOP) {
        debug("Looping");
        if (pixelPos >= (STRIP_LEDS - 1))
          pixelPos = 0;
        else
          pixelPos = STRIP_LEDS - 1;
      }
      else if (endAction & PIX_BOUNCE) {
        debug("Bouncing");
        if ( pixelDir == Left) {
          pixelDir = Right;
          pixelPos = 0;
        }
        else {
          pixelDir = Left;
          pixelPos = STRIP_LEDS - 1;
        }
      }
      else if (endAction & PIX_RANDOM) {
        int rndVal = random(0, 10);

        // Arbitary value...
        if (rndVal < 5) {           // BOUNCE it...
          debug("Bouncing");
          if ( pixelDir == Left) {
            pixelDir = Right;
            pixelPos = 0;
          }
          else {
            pixelDir = Left;
            pixelPos = STRIP_LEDS - 1;
          }
        }
        else {                      // LOOP it...
          debug("Looping");
          if (pixelPos >= (STRIP_LEDS - 1))
            pixelPos = 0;
          else
            pixelPos = STRIP_LEDS - 1;
        }
      }

      //
      // Now check what to do with the item (SLOWDOWN, FADE, SHRINK, BRIGHTEN)...
      // Whilst we are at it, lets also check if it should be killed off
      //
      if (endAction & PIX_SLOWDOWN) {
        --mySpeed;
        if (mySpeed <= 0) {
          if (howToDie & PIX_STOPS)
            pixelDead = true;
          else
            mySpeed = 1;      // Send it on its way again...
        }
      }

      int fadeVal = 32;

      if (endAction & PIX_FADE) {
        colour.subtractFromRGB(fadeVal);
        if (!colour) {
          debug("FADED AWAY");
          pixelDead = true;
        }
      }

      if (endAction & PIX_BRIGHTEN) {
        colour.addToRGB(fadeVal);
        if (howToDie & PIX_BURNOUT) {
          if ((colour.red == 255) && (colour.green == 255) && (colour.red == 255)) {
            debug("BURNT OUT");
            pixelDead = true;
          }
        }
      }

      if (endAction & PIX_SHRINK) {
        --width;
        if (width == 0) {
          if (howToDie & PIX_DISAPPEARS) {
            debug("SHRUNK");
            pixelDead = true;
          }
          else
            width = 1;      // Send it on its way again...
        }
      }

      if (howToDie & PIX_LIFETIME) {
        if ( numCycles > lifespan) {
          debug("DIED OF OLD AGE");
          pixelDead = true;
        }
      }
    }
  }

  if (pixelDead == true) {
    debug("KILLING PIXEL");
    retval = false;
  }

  return retval;
}

bool clsPixel::pixelStep_Matrix(void)
{
  trace_hifreq("clsPixel");

  bool retval = true;     // Returns true if the pixel moves, however false if the pixel needs to be destroyed...

  switch (pixelDir) {
    case Up:
      pixelRow += 1;
      if (pixelRow >= MATRIX_HEIGHT)
        pixelDead = true;
      break;
    case Down:
      pixelRow -= 1;
      if (pixelRow < 0)
        pixelDead = true;
      break;
    case Left:
      pixelCol -= 1;
      if (pixelCol < 0)
        pixelDead = true;
      break;
    case Right:
      pixelCol += 1;
      if (pixelCol >= MATRIX_WIDTH)
        pixelDead = true;
      break;
  }

  if (pixelDead == true) {
    debug_hifreq("KILLING PIXEL");
    retval = false;
  }

  return retval;
}
