//*********************************************************************************************************************************************************************************************************************************************
// clsPixelRingBuffer.cpp
//
// Class containing ring buffer to store and manage dynamic pixels
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

#include "clsPixelBuffer.h"

void clsPixelBuffer::begin(void)
{
  trace("clsPixelBuffer");

  // Force all the pixels as dead to enable them to be loaded...
  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++)
    pixels[i].pixelDead = true;
}

bool clsPixelBuffer::addPixelToStrip(int pPeakLevel, clsColourCtrl *pColourCtrl, byte pEndAction, byte pHowToDie)
{
  trace_hifreq("clsPixelBuffer");

  bool retval = false;

  // If theres space in the buffer, add pixel to the buffer...
  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++) {
    // We have found a slot...
    if (pixels[i].pixelDead == true) {
      pixels[i].initPixel_Strip(pPeakLevel, pColourCtrl, pEndAction, pHowToDie);
      return true;
      break;
    }
  }
  return retval;
}

bool clsPixelBuffer::addPixelToMatrix(int pPeakLevel, clsColourCtrl *pColourCtrl, int pRow, int pCol, eDirection pDirection, int pMaxPixels)
{
  trace_hifreq("clsPixelBuffer");

  // If theres space in the buffer, add pixel to the buffer...
  for (int i = 0; (i < MAX_PIXELS_BUFFERSIZE) && (i < pMaxPixels); i++) {
    // We have found a slot...
    if (pixels[i].pixelDead == true) {
      pixels[i].initPixel_Matrix(pPeakLevel, pColourCtrl, pRow, pCol, pDirection);
      return true;
    }
  }
  return false;
}

void clsPixelBuffer::movePixelsStrip(void)
{
  trace_hifreq("clsPixelBuffer");

  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++) {
    if (pixels[i].pixelDead == false) {
      pixels[i].pixelStep_Strip();
    }
  }
}

void clsPixelBuffer::movePixelsMatrix(void)
{
  trace_hifreq("clsPixelBuffer");

  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++) {
    if (pixels[i].pixelDead == false) {
      pixels[i].pixelStep_Matrix();
    }
  }
}
