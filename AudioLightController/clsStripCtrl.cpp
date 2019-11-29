//*********************************************************************************************************************************************************************************************************************************************
// clsStripCtrl.cpp
//
// Class containing all the functionality to manage Matrix
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

#include "clsStripCtrl.h"

void clsStripCtrl::begin(clsVUData *pVUData, clsColourCtrl *pClsColourCtrl)
{
  trace("clsStripCtrl");

  // Seed the randomNumber...
  randomSeed(analogRead(A1));

  // Set pointers to the source array for the VU data...
  VUData = pVUData;
  pixelBuffer.begin();

  colourCtrl = pClsColourCtrl;

  FastLED.addLeds<NEOPIXEL, STRIP_DATA_PIN>(leds, STRIP_LEDS);
  // FastLED.setBrightness(colourCtrl->brightness);

  resetLeds();
  FastLED.show();
}

void clsStripCtrl::turnOffTheLights(void)
{
  if (displayCurrentlyOn == true) {
    resetLeds();
    FastLED.show();
    displayCurrentlyOn = false;
  }
}

unsigned long prevTimeStrip = 0;

void clsStripCtrl::displaySample(eStripPattern pPatternMode, int pPeakVal) {
  trace_hifreq("clsStripCtrl");

  bool usingPixelBuf = false;                 // Variable to check if we are using the pixelBuffer_Strip (Strip patterns only)
  byte endAction = PIX_STOP;
  byte howToDie = PIX_LIFETIME;

  displayCurrentlyOn = true;

  // Check if the pixelBuffer_Strip is being used, if not save time/effort and ignore it...
  switch (pPatternMode) {
    case ferriswheel:
      endAction = PIX_LOOP + PIX_SHRINK + PIX_FADE + PIX_SLOWDOWN;
      howToDie = PIX_STOPS + PIX_LIFETIME;
      usingPixelBuf = true;
      break;
    case ferriswheel_t:
      endAction = PIX_STOP;
      howToDie = PIX_LIFETIME;
      usingPixelBuf = true;
      break;
    case cyclon:
      endAction = PIX_BOUNCE + PIX_SHRINK + PIX_FADE + PIX_SLOWDOWN;
      howToDie = PIX_STOPS + PIX_DISAPPEARS + PIX_LIFETIME;
      usingPixelBuf = true;
      break;
    case sparkle:
      // endAction = PIX_BOUNCE + PIX_SHRINK + PIX_BRIGHTEN;
      endAction = PIX_BOUNCE + PIX_BRIGHTEN;
      howToDie = PIX_BURNOUT + PIX_LIFETIME;
      usingPixelBuf = true;
      break;
    case bounce:
      endAction = PIX_BOUNCE;
      howToDie = PIX_LIFETIME;
      usingPixelBuf = true;
      break;
    case swell:
      endAction = PIX_RANDOM + PIX_FADE;
      howToDie = PIX_LIFETIME + PIX_DISAPPEARS;
      usingPixelBuf = true;
      break;
    case strip_vu:
      resetLeds();
      patVU(LEFT);
      patVU(RIGHT);
      break;
    default:
      break;
  }

  if (usingPixelBuf == true) {
    resetLeds();
    pixelBuffer.movePixelsStrip();

    if (pPeakVal > 0 )
      pixelBuffer.addPixelToStrip(pPeakVal, colourCtrl, endAction, howToDie);

    patPixBuffer();
  }
  FastLED.show();
}

void clsStripCtrl::patVU(eLeftRight pLeftRight)
{
  trace_hifreq("clsStripCtrl");

  int total = 0;
  int displayPixel;

  // Smooth the downward transitions, otherwise display very "jumpy"...
  if ( VUData->VUDataTotal[pLeftRight] < VUData->VUDataTotal_LastMax[pLeftRight])
    VUData->VUDataTotal_LastMax[pLeftRight] = VUData->VUDataTotal_LastMax[pLeftRight] - 1;
  else
    VUData->VUDataTotal_LastMax[pLeftRight] = VUData->VUDataTotal[pLeftRight];

  total = map(VUData->VUDataTotal_LastMax[pLeftRight], 0, ((MATRIX_WIDTH * MATRIX_HEIGHT) / 4), 0, (STRIP_LEDS / 2));

  for (int pixel = 0; pixel < (STRIP_LEDS / 2); pixel++) {
    if (pLeftRight == LEFT)
      displayPixel = pixel;
    else
      displayPixel = (STRIP_LEDS - 1) - pixel;

    if (pixel > total)
      setPixelVal(displayPixel, CRGB(0, 0, 0));
    else if (pixel >= 32)
      setPixelVal(displayPixel, colourCtrl->getAccentColour());
    else if (pixel >= 24)
      setPixelVal(displayPixel, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET / 2));
    else
      setPixelVal(displayPixel, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET - 1));
  }
}

void clsStripCtrl::patPixBuffer(void)
{
  trace_hifreq("clsStripCtrl");

  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++) {
    if (pixelBuffer.pixels[i].pixelDead == false) {
      for (int j = 0; (j < pixelBuffer.pixels[i].width) && (pixelBuffer.pixels[i].pixelPos + j) < (STRIP_LEDS - 1); j++) {
        setPixelVal(pixelBuffer.pixels[i].pixelPos + j, pixelBuffer.pixels[i].colour);
      }
    }
  }
}

void clsStripCtrl::patVUHue(eLeftRight pLeftRight)
{
  trace_hifreq("clsStripCtrl");
  clsStripCtrl::patVUSideBar(pLeftRight);
}

void clsStripCtrl::patVUHueRev(eLeftRight pLeftRight)
{
  trace_hifreq("clsStripCtrl");
  clsStripCtrl::patVUSideBar(pLeftRight);
}

void clsStripCtrl::patVUSide(eLeftRight pLeftRight)
{
  trace_hifreq("clsStripCtrl");
  clsStripCtrl::patVUSideBar(pLeftRight);
}

void clsStripCtrl::patVUSideBar(eLeftRight pLeftRight)
{
  trace_hifreq("clsStripCtrl");

  CRGB pixel;

  int total = VUData->VUDataTotal[pLeftRight];

  // Exagerate the display...
  total = total * 1.5;

  if (total < VUData->VUDataTotal_LastMax[pLeftRight])
    total = VUData->VUDataTotal_LastMax[pLeftRight] - 1;

  for (int i = 0; i < total && i < (STRIP_LEDS / 2); i++) {
    pixel = colourCtrl->getColour(i);
    if (pLeftRight == LEFT)
      leds[i] = pixel;
    else
      leds[STRIP_LEDS - i - 1] = pixel;
  }

  VUData->VUDataTotal_LastMax[pLeftRight] = total;
}

void clsStripCtrl::patVUOsc(void)
{
  trace_hifreq("clsStripCtrl");

  patVUSideBar(LEFT);
  patVUSideBar(RIGHT);
}

inline void clsStripCtrl::setPixelVal(int pPixel, CRGB pPixelCol) {
  leds[pPixel] = pPixelCol;
}

inline CRGB clsStripCtrl::getPixelVal(int pPixel) {
  return leds[pPixel];
}

void clsStripCtrl::resetLeds() {
  for (int i = 0; i < STRIP_LEDS; i++)
    leds[i] = CHSV(0, 0, 0);
}
