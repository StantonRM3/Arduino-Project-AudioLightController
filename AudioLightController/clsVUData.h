//*********************************************************************************************************************************************************************************************************************************************
// clsVUData.h
//
// Class containing all the functionality to manage and store VU data from samplers
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSVUDATA_H
#define CLSVUDATA_H

#include <Arduino.h>
#include <Wire.h>
#include "Enums.h"

#define SLAVE_NUMBER_LEFT       8
#define SLAVE_NUMBER_RIGHT      9

class clsVUDemoData
{
  public:
    void begin(void);
    void updateValues(void);

    int demoVUData[2][(MATRIX_WIDTH / 2)];
    
  private:

};

class clsVUData
{
  public:
    void begin(void);
    void I2C_loadVUData(bool pDemoMode);

    // Variables for Matrix presentation VUData=raw data; VUData_CurrentMax=current maximum value for each sample
    int VUData[2][(MATRIX_WIDTH / 2)];
    int VUData_LastMax[2][(MATRIX_WIDTH / 2)];

    // Variables for single display, i.e. strip LEDS or totals (simple sum of all the sample channels...
    int VUDataTotal[2];
    int VUDataTotal_LastMax[2];
    int bpm;

  private:
  
    clsVUDemoData VUDemoData;
  
    void I2C_begin(void);
    void I2C_requestData(int pFromSlave);
    void calculateBPM(void);
};

#endif
