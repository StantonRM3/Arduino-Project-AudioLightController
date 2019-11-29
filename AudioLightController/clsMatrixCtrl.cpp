//*********************************************************************************************************************************************************************************************************************************************
// clsMatrixCtrl.cpp
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

#include "clsMatrixCtrl.h"

void clsMatrixCtrl::begin(clsVUData *pVUData, clsColourCtrl *pColourCtrl)
{
  trace("clsMatrixCtrl");

  // Seed the randomNumber...
  randomSeed(analogRead(A0));

  // Set pointers to the source array for the VU data...
  //  left = pLeft;
  //  right = pRight;
  VUData = pVUData;
  pixelBuffer.begin();

  colourCtrl = pColourCtrl;

  FastLED.addLeds<NEOPIXEL, MATRIX_DATA_PIN>(leds, MATRIX_LEDS);
  // FastLED.setBrightness(colourCtrl->brightness);

  resetLeds();

  FastLED.show();
}

void clsMatrixCtrl::turnOffTheLights(void)
{
  if (displayCurrentlyOn == true) {
    resetLeds();
    FastLED.show();
    displayCurrentlyOn = false;
  }
}

// Slow down the basecolour change...
#define BASECOLOUR_CYCLECOUNT 10
int basecolourMultiplier = 0;

void clsMatrixCtrl::displaySample(eMatrixPattern pPatternMode, int pPeakVal) {
  trace_hifreq("clsMatrixCtrl:displaySample");

  bool usingPixelBuf = false;                // Variable to check if we are using the pixelBuffer_Matrix (Matrix patterns only)

  // Force LED reset if pattern has changed to prevent lit leds remaining until overwritten by some of the patterns...
  if (pPatternMode != previousPattern) {
    previousPattern = pPatternMode;
    resetLeds();
  }

  // The display is now on...
  displayCurrentlyOn = true;

  // Check if the pixelBuffer_Matrix is being used, if not save time/effort and ignore it...
  switch (pPatternMode) {
    case tartan:
      maxPixelBufSize = 8;    // Set the maximumum number of matrix pixels...
      usingPixelBuf = true;
      break;
    case starlight:
      maxPixelBufSize = 8;    // Set the maximumum number of matrix pixels...
      usingPixelBuf = true;
      break;
    case scope:
      maxPixelBufSize = 16;   // Set the maximumum number of matrix pixels...
      usingPixelBuf = true;
      break;
    default:
      break;
  }

  if (usingPixelBuf == true) {
    int startRow = 0, startCol = 0;

    pixelBuffer.movePixelsMatrix();

    if (pPatternMode == scope) // We add vu level (It is not triggered...)
    {
      startRow = map(VUData->VUDataTotal[0], 0, 32, 0, 7);
      pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, 0, Right, maxPixelBufSize);
    }
    else if (pPeakVal > 0 ) {
      // If the pattern is Tartan, work out a direction and starting position, and add the single pixel
      if (pPatternMode == tartan) {
        eDirection pixDir = (eDirection) random(0, 4);
        switch (pixDir) {
          case Up:
            startCol = random(0, MATRIX_WIDTH);
            break;
          case Down:
            startRow = MATRIX_HEIGHT - 1;
            startCol = random(0, MATRIX_WIDTH);
            break;
          case Left:
            startCol = MATRIX_WIDTH - 1;
            startRow = random(0, MATRIX_HEIGHT);
            break;
          case Right:
            startRow = random(0, MATRIX_HEIGHT);
            break;
        }
        pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, startCol, pixDir, maxPixelBufSize);
      }
      else { // At the moment it must be starlight...
        startRow = random(0, MATRIX_HEIGHT);
        startCol = random(0, MATRIX_WIDTH);

        // Now add 4 pixels going in different directions...
        pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, startCol, Up, maxPixelBufSize);
        pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, startCol, Down, maxPixelBufSize);
        pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, startCol, Left, maxPixelBufSize);
        pixelBuffer.addPixelToMatrix(pPeakVal, colourCtrl, startRow, startCol, Right, maxPixelBufSize);
      }
    }
  }

  // TODO: What was this all about???
  if (++basecolourMultiplier >= BASECOLOUR_CYCLECOUNT) {
    ++baseColour;
    basecolourMultiplier = 0;
  }

  switch (pPatternMode) {
    case MATRIXPATTERN_NOTSET:
    case ledmatrix_off:
      resetLeds();
      break;
    case matrix_vu:
      resetLeds();
      patVU(LEFT);
      patVU(RIGHT);
      break;
    case vupeak:
      resetLeds();
      patVUPeak(LEFT);
      patVUPeak(RIGHT);
      break;
    case huebars:
      resetLeds();
      patVUHue(LEFT);
      patVUHue(RIGHT);
      break;
    case huebars_r:
      resetLeds();
      patVUHueRev(LEFT);
      patVUHueRev(RIGHT);
      break;
    case pulsar:
      resetLeds();
      patVUPulsar(LEFT);
      patVUPulsar(RIGHT);
      break;
    case sideways:
      // resetLeds();
      patVUSide(LEFT);
      patVUSide(RIGHT);
      break;
    case sidepulse:
      fadePixels(8);
      patVUSideBar(LEFT);
      patVUSideBar(RIGHT);
      break;
    case scope:
      fadePixels(32);
      patPixBuffer();
      break;
    case tartan:
      fadePixels(8);
      patPixBuffer();
      break;
    case starlight:
      fadePixels(16);
      patPixBuffer();
      break;
  }
  FastLED.show();
}

void clsMatrixCtrl::patVU(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVU");

  int displayCol;

  for (int col = 0; col < (MATRIX_WIDTH / 2); col++)
  {
    // Smooth the downwards column...
    if (VUData->VUData[pLeftRight][col] < VUData->VUData_LastMax[pLeftRight][col])
      VUData->VUData[pLeftRight][col] = VUData->VUData_LastMax[pLeftRight][col] - 1;

    if (pLeftRight == LEFT)
      displayCol = col;
    else
      displayCol = (MATRIX_WIDTH - 1) - col;

    for (int row = 0; (row < VUData->VUData[pLeftRight][col]) && (row < MATRIX_HEIGHT); row++) {
      if (row >= 7)
        setPixelVal(row, displayCol, colourCtrl->getAccentColour());
      else if (row >= 4)
        setPixelVal(row, displayCol, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET / 2));
      else
        setPixelVal(row, displayCol, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET - 1));
    }

    VUData->VUData_LastMax[pLeftRight][col] = VUData->VUData[pLeftRight][col];
  }
}

void clsMatrixCtrl::patVUPeak(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUPeak");

  int displayCol;

  for (int col = 0; col < (MATRIX_WIDTH / 2); col++)
  {
    // Smooth the downwards column...
    if (VUData->VUData[pLeftRight][col] < VUData->VUData_LastMax[pLeftRight][col])
      VUData->VUData[pLeftRight][col] = VUData->VUData_LastMax[pLeftRight][col] - 1;

    if (pLeftRight == LEFT)
      displayCol = col;
    else
      displayCol = (MATRIX_WIDTH - 1) - col;

    if (VUData->VUData[pLeftRight][col] >= 7)
      setPixelVal(VUData->VUData[pLeftRight][col], displayCol, colourCtrl->getAccentColour());
    else if (VUData->VUData[pLeftRight][col] >= 4)
      setPixelVal(VUData->VUData[pLeftRight][col], displayCol, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET / 2));
    else if (VUData->VUData[pLeftRight][col] > 0)
      setPixelVal(VUData->VUData[pLeftRight][col], displayCol, colourCtrl->getColour(MAX_RANDOM_COLOUR_PALLET - 1));
    else
      // setPixelVal(VUData->VUData[pLeftRight][col], displayCol, colourCtrl->getAccentColour());
      setPixelVal(VUData->VUData[pLeftRight][col], displayCol, CRGB(0, 0, 0));

    VUData->VUData_LastMax[pLeftRight][col] = VUData->VUData[pLeftRight][col];
  }
}

void clsMatrixCtrl::patVUHue(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUHue");

  int displayCol;
  CRGB displayColour;

  baseColour = colourCtrl->setBaselineColour(baseColour, Sawtooth);

  for (int col = 0; col < (MATRIX_WIDTH / 2); col++)
  {
    // Smooth the downwards column...
    if (VUData->VUData[pLeftRight][col] < VUData->VUData_LastMax[pLeftRight][col])
      VUData->VUData[pLeftRight][col] = VUData->VUData_LastMax[pLeftRight][col] - 1;

    if (pLeftRight == LEFT)
      displayCol = col;
    else
      displayCol = (MATRIX_WIDTH - 1) - col;

    displayColour = colourCtrl->getNextColour();

    for (int row = 0; (row < VUData->VUData[pLeftRight][col]) && (row < MATRIX_HEIGHT); row++) {
      setPixelVal(row, displayCol, displayColour);
    }

    VUData->VUData_LastMax[pLeftRight][col] = VUData->VUData[pLeftRight][col];
  }
}

void clsMatrixCtrl::patVUHueRev(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUHueRev");

  int displayCol;
  CRGB displayColour;

  baseColour = colourCtrl->setBaselineColour(baseColour, Sawtooth);

  for (int col = 0; col < (MATRIX_WIDTH / 2); )
  {
    if (pLeftRight == LEFT)
      displayCol = col;
    else
      displayCol = (MATRIX_WIDTH - 1) - col;

    // Add the next 2 data values together and divide by 2 to get average in 2 bars...
    int twoColVal = ((VUData->VUData[pLeftRight][col] + VUData->VUData[pLeftRight][col + 1]) / 2) + 1;

    // Smooth the downwards column...
    if (twoColVal < VUData->VUData_LastMax[pLeftRight][col]) {
      twoColVal = VUData->VUData_LastMax[pLeftRight][col] - 1;
    }

    displayColour = colourCtrl->getNextColour();

    for (int j = 0; j < 2; j++) {
      if (pLeftRight == LEFT)
        displayCol = displayCol + j;
      else
        displayCol = displayCol - j;

      for (int row = twoColVal; (row < MATRIX_HEIGHT); row++) {
        setPixelVal(row, displayCol, displayColour);
      }
    }

    VUData->VUData_LastMax[pLeftRight][col] = twoColVal;

    col += 2;
  }
}

void clsMatrixCtrl::patVUPulsar(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUPulsar");

  int displayCol;
  CRGB displayColour;

  baseColour = colourCtrl->setBaselineColour(baseColour, Sawtooth);

  for (int col = 0; col < (MATRIX_WIDTH / 2); col++)
  {
    if (pLeftRight == LEFT)
      displayCol = (MATRIX_WIDTH / 2) - col - 1;
    else
      displayCol = (MATRIX_WIDTH / 2) + col;

    displayColour = colourCtrl->getNextColour();

    // Smooth the downwards column...
    // If the current level is less than previous level we need to take one off the top of the column...
    if (VUData->VUData[pLeftRight][col] < VUData->VUData_LastMax[pLeftRight][col])
    {
      VUData->VUData[pLeftRight][col] = VUData->VUData_LastMax[pLeftRight][col] - 1;
      setPixelVal(VUData->VUData_LastMax[pLeftRight][col], displayCol, CRGB(0, 0, 0));
    }
    // Otherwise we need to add to the top of the column....
    else if (VUData->VUData[pLeftRight][col] > VUData->VUData_LastMax[pLeftRight][col])
    {
      for (int row = VUData->VUData_LastMax[pLeftRight][col]; (row < VUData->VUData[pLeftRight][col]) && (row < MATRIX_HEIGHT); row++) {
        setPixelVal(row, displayCol, displayColour);
      }
    }
    // Otherwise check if its zero, in which case blank it...
    else if (VUData->VUData[pLeftRight][col] == 0)
      setPixelVal(0, displayCol, CRGB(0, 0, 0));

    VUData->VUData_LastMax[pLeftRight][col] = VUData->VUData[pLeftRight][col];
  }
}

void clsMatrixCtrl::patVUSide(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUSide");

  CRGB displayColour;

  baseColour = colourCtrl->setBaselineColour(baseColour, Sawtooth);

  int colWidth = 0;

  for (int row = 0; row < MATRIX_HEIGHT; row++) {

    displayColour = colourCtrl->getNextColour();

    // Here we will double up the samples as the number of rows = half the number of columns.  Should do with with maths really, but for testing...
    colWidth = VUData->VUData[pLeftRight][row * 2] + VUData->VUData[pLeftRight][(row * 2) + 1];

    // Smooth the downwards row...
    if (colWidth < localPeakStore[pLeftRight][row])
      colWidth = localPeakStore[pLeftRight][row] - 1;

    // If the current level is less than previous level we need to take one off the top of the column...
    if (colWidth < localPeakStore[pLeftRight][row]) {
      if (pLeftRight == LEFT)
        setPixelVal(row, colWidth, CRGB(0, 0, 0));
      else
        setPixelVal(row, (MATRIX_WIDTH - 1 - colWidth), CRGB(0, 0, 0));
    }
    else {
      if (colWidth > localPeakStore[pLeftRight][row]) {
        for (int dispCol = 0; dispCol < colWidth; dispCol++) {
          if (pLeftRight == LEFT)
            setPixelVal(row, dispCol, displayColour);
          else
            setPixelVal(row, (MATRIX_WIDTH - 1 - dispCol), displayColour);
        }
      }
    }
    localPeakStore[pLeftRight][row] = colWidth;
  }
}

int lastDisplayTotal[2] = {0, 0};

void clsMatrixCtrl::patVUSideBar(eLeftRight pLeftRight)
{
  trace_hifreq("clsMatrixCtrl:patVUSideBar");

  CRGB pixel;

  int total = VUData->VUDataTotal[pLeftRight];

  if (VUData->VUDataTotal[pLeftRight] < VUData->VUDataTotal_LastMax[pLeftRight])
    total = VUData->VUDataTotal_LastMax[pLeftRight] - 1;

  VUData->VUDataTotal_LastMax[pLeftRight] = total;

  // Exagerate the display...
  total = total * 1.5;
  // Add pixels to show the increase....
  if (total > lastDisplayTotal[pLeftRight]) {

    // Work out which column this value lives in...
    int prevCol = lastDisplayTotal[pLeftRight] / MATRIX_HEIGHT;
    int col = total / MATRIX_HEIGHT;

    pixel = colourCtrl->getRandomColour();

    for (int j = prevCol; j <= col; j++)
      for (int i = 0; i < MATRIX_HEIGHT; i++) {
        if (pLeftRight == LEFT)
          setPixelVal(i, j, pixel);
        else
          setPixelVal(i, (MATRIX_WIDTH - 1 - j), pixel);
      }
  }
  lastDisplayTotal[pLeftRight] = total;
}

void clsMatrixCtrl::patPixBuffer(void)
{
  trace_hifreq("clsMatrixCtrl:patPixBuffer");

  for (int i = 0; i < MAX_PIXELS_BUFFERSIZE; i++) {
    if (pixelBuffer.pixels[i].pixelDead == false) {
      setPixelVal(pixelBuffer.pixels[i].pixelRow, pixelBuffer.pixels[i].pixelCol, pixelBuffer.pixels[i].colour);
    }
  }
}

inline void clsMatrixCtrl::setPixelVal(int pRow, int pCol, CRGB pPixel) {
  int idx = 0;
  idx = pRow + (pCol * MATRIX_HEIGHT);
  if (idx >= MATRIX_LEDS) {
    error("Index out of range [%d][%d]=(%d/%d)", pRow, pCol, idx, MATRIX_LEDS);
  }
  else
    leds[idx] = pPixel;
}

inline CRGB clsMatrixCtrl::getPixelVal(int pRow, int pCol) {
  int idx = 0;
  idx = pRow + (pCol * MATRIX_HEIGHT);
  if (idx >= MATRIX_LEDS) {
    error("Index out of range [%d][%d]=(%d/%d)", pRow, pCol, idx, MATRIX_LEDS);
    return leds[0];
  }
  else
    return leds[idx];
}

void clsMatrixCtrl::resetLeds() {
  for (int i = 0; i < MATRIX_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
}

void clsMatrixCtrl::fadePixels(int pFadeSpeed) {
  for (int i = 0; i < MATRIX_LEDS; i++)
    leds[i].subtractFromRGB(pFadeSpeed);
}

void clsMatrixCtrl::shiftColumns(eLeftRight pLeftRight) {
  if (pLeftRight == RIGHT) {
    for (int i = (MATRIX_WIDTH - 1); i > 0; i--) {
      for (int j = 0; j < MATRIX_HEIGHT; j++)
        setPixelVal(j, i, getPixelVal(j, i - 1));
    }
  }
  else {
    for (int i = 0; i < (MATRIX_WIDTH - 1); i++) {
      for (int j = 0; j < MATRIX_HEIGHT; j++)
        setPixelVal(j, i, getPixelVal(j, i + 1));
    }
  }
}
