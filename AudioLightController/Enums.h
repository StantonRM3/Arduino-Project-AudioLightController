#ifndef _ENUMS_H
#define _ENUMS_H

#include <Arduino.h>

#define MATRIX_HEIGHT       8
#define MATRIX_WIDTH       32

#define STRIP_LEDS        110

// Pixel actions (Strip)
const byte PIX_STOP     = 0b00000001;      // Stop pixel at the end... (Either end)
const byte PIX_LOOP     = 0b00000010;      // Go back to start/end loop
const byte PIX_BOUNCE   = 0b00000100;      // Reverse direction
const byte PIX_RANDOM   = 0b00001000;      // Randomly determine wether to bounce, loop (or stop)
const byte PIX_SLOWDOWN = 0b00010000;      // Slow down
const byte PIX_FADE     = 0b00100000;      // Fade down
const byte PIX_SHRINK   = 0b01000000;      // Shrink
const byte PIX_BRIGHTEN = 0b10000000;      // Increase intensity...

// Pixel die conditions...
// const byte PIX_AUTOKILL   = 0b11110000;    // Kill it when its speed = 0 OR its width shrinks to 0 or its fades to 0 or it brightens to 255
const byte PIX_LIFETIME   = 0b00000001;    // Kill the pixel when it reaches the end of its lifetime (set later)
const byte PIX_TBD1       = 0b00000010;    // Kill it when ???
const byte PIX_TBD2       = 0b00000100;    // Kill it when ???
const byte PIX_TBD3       = 0b00001000;    // Kill it when ???
const byte PIX_STOPS      = 0b00010000;    // Kill it when its speed = 0
const byte PIX_TBD4       = 0b00100000;    // Kill it when its fades to 0 [NOTE WE HAVE TO KILL IT AS THERES NOTHING TO DISPLAY...]
const byte PIX_DISAPPEARS = 0b01000000;    // Kill it when its width shrinks to 0
const byte PIX_BURNOUT    = 0b10000000;    // Kill it when it brightens to 255

// These values are set by NodeRed Controller and come in MQTTClient
typedef enum {
  STRIPPATTERN_NOTSET = -1,
  ledstrip_off,
  ferriswheel,
  ferriswheel_t,
  cyclon,
  sparkle,
  strip_vu,           // ENUM doesn't like same name used in multiple enum definitions...
  bounce,
  swell
} eStripPattern;

typedef enum {
  MATRIXPATTERN_NOTSET = -1,
  ledmatrix_off,
  matrix_vu,
  vupeak,
  huebars,
  huebars_r,
  pulsar,
  sideways,
  sidepulse,
  scope,
  tartan,
  starlight  
} eMatrixPattern;

typedef enum {
  COLOUR_NOTSET = -1,
  red,
  blue,
  green,
  yellow,
  pink,
  purple,
  white,
  red_blue,
  green_yellow,
  blue_green,
  rainbow,
  pastel
} eColours;

typedef enum {
  ON = 0,
  OFF
} eOnOff;

typedef enum {
  LEFT = 0,
  RIGHT
} eLeftRight;

typedef enum {
  UP = 1,
  DOWN = -1
} eUpDown;

typedef enum {
  Up=0,
  Down,
  Left,
  Right
} eDirection;

typedef enum {
  Triangle,
  Sawtooth,
  Sinewave,
  Randomise
} eWavePattern;

#endif
