//*********************************************************************************************************************************************************************************************************************************************
// clsVUData.cpp
//
// Class containing all the functionality to manage getting and manipulating VU Data fro  samplers...
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

#include "clsVUData.h"

void clsVUDemoData::begin(void)
{
  trace("clsVUDemoData");
  int maxVal = 0;

  // Seed the demoVUData with random values...
  for ( int i = 0; i < 2; i++) {
    for (int j = 0; j < (MATRIX_WIDTH / 2); j++) {
      // Trying to give this data a sort of shape (downward triangle), from 8 down to max 4...
      maxVal = MATRIX_HEIGHT - (j / 4);
      demoVUData[i][j] = random(0, maxVal);
    }
  }
}

void clsVUDemoData::updateValues(void)
{
  trace_hifreq("updateValues");
  int maxVal = 0;
  int change = 0;

  // Seed the demoVUData with random values...
  for ( int i = 0; i < 2; i++) {
    for (int j = 0; j < (MATRIX_WIDTH / 2); j++) {

      // Trying to give this data a sort of shape (downward triangle), from 8 down to max 4...
      maxVal = MATRIX_HEIGHT - (j / 4);
      change = random(0, 3);

      if (random(0, 10) < 7) {  // VU value goes down...
        if (demoVUData[i][j] >= change)
          demoVUData[i][j] -= change;
        else
          demoVUData[i][j] = 0;
      }
      else {
        if (demoVUData[i][j] < (maxVal - change))
          demoVUData[i][j] += change;
        else
          demoVUData[i][j] = maxVal;
      }
    }
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// I2C - COMMS (REQUEST DATA FROM SAMPLERS)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void clsVUData::begin(void)
{
  trace("clsVUData");

  // Blank all values down to zero (just to be sure to be sure)..
  for (int i = LEFT; i <= RIGHT; i++) {
    for (int j = 0; j < (MATRIX_WIDTH / 2); j++) {
      VUData[i][j] = 0;
      VUData_LastMax[i][j] = 0;
    }
    VUDataTotal[i] = 0;
    VUDataTotal_LastMax[i] = 0;
  }

  VUDemoData.begin();

  clsVUData::I2C_begin();
}

void clsVUData::I2C_begin(void)
{
  trace("");
  // Wire.begin();      // join i2c bus (address optional for master)
}

void clsVUData::I2C_loadVUData(bool pDemoMode)
{
  trace_hifreq("I2C_loadVUData");

  if (pDemoMode == false) {
    I2C_requestData(SLAVE_NUMBER_LEFT);
    I2C_requestData(SLAVE_NUMBER_RIGHT);
  }
  else {    // Load up the arrays with demo data...
    VUDemoData.updateValues();

    VUDataTotal[LEFT] = 0;
    VUDataTotal[RIGHT] = 0;

    for (int i = 0; i < (MATRIX_WIDTH / 2); i++) {
      VUData[LEFT][i] = VUDemoData.demoVUData[LEFT][i];
      VUDataTotal[LEFT] += VUDemoData.demoVUData[LEFT][i];

      VUData[RIGHT][i] = VUDemoData.demoVUData[RIGHT][i];
      VUDataTotal[RIGHT] += VUDemoData.demoVUData[RIGHT][i];
    }
  }
}

void clsVUData::I2C_requestData(int pFromSlave) {
  trace_hifreq("I2C_requestData");
  Wire.requestFrom(pFromSlave, (MATRIX_WIDTH / 2));  // request 16 bytes from slave device #8

  int idx = 0;

  if (pFromSlave == SLAVE_NUMBER_LEFT) {
    VUDataTotal[LEFT] = 0;        // Calculate the total for this channel while we're at it...
  }
  else {
    VUDataTotal[RIGHT] = 0;
  }

  while (Wire.available()) {          // slave may send less than requested
    char c = Wire.read();             // receive a byte as character

    if ( idx < (MATRIX_WIDTH / 2)) {  // Ensure we limit the number of values to (MATRIX_WIDTH / 2) = 16, otherwise throw it away as it will go out of bounds...

      if (pFromSlave == SLAVE_NUMBER_LEFT) {
        VUData[LEFT][idx] = c - '0';
        VUDataTotal[LEFT] += VUData[LEFT][idx];
      }
      else {
        VUData[RIGHT][idx] = c - '0';
        VUDataTotal[RIGHT] += VUData[RIGHT][idx];
      }
#ifdef DEBUG
      if (pFromSlave == SLAVE_NUMBER_LEFT)
        debug_hifreq(">L:RD:IN-'%s'", VUData[pFromSlave][idx]);         // print the character
      else
        debug_hifreq(">R:RD:IN-'%s'", VUData[pFromSlave][idx]);         // print the character
#endif
    }
    else {
      error("Index out of bounds...");
    }

    ++idx;
  }
}
