// #define DEBUG                           TRUE
// #define DEBUG_HIFREQ                    TRUE
// #define TRACE                           TRUE
// #define TRACE_HIFREQ                    TRUE
// #define INFO                            TRUE
#define ERROR                           TRUE
#define WARNING                         TRUE
#include "Debug.h"

#include "clsVUData.h"
#include "clsColourCtrl.h"
#include "clsMatrixCtrl.h"
#include "clsStripCtrl.h"
#include "clsMQTTClient.h"
#include "clsAPXPower.h"

clsColourCtrl colourCtrl;
clsMatrixCtrl matrixCtrl;
clsStripCtrl stripCtrl;
clsVUData VUData;
clsMQTTClient myMQTTClient;
clsAPXPower APXPower;

#define STANDBY_PB            13
#define STANDBY_LED           12
#define STANDBY_POWERLINE     11

char debug_stringBuf[DEBUG_BUFFER_SIZE];

void setup() {

  Serial.begin(SERIAL_BAUD);
  Serial.println("sirenOne starting up...");

  trace("");

  randomSeed(analogRead(0));

  I2C_Setup();

  APXPower.begin(STANDBY_PB, STANDBY_LED, STANDBY_POWERLINE);
  VUData.begin();
  colourCtrl.begin();

  stripCtrl.begin(&VUData, &colourCtrl);
  matrixCtrl.begin(&VUData, &colourCtrl);

  myMQTTClient.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN LOOP()...
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned long prevTime = 0;
unsigned long prevMQTTCheckTime = 0;
bool prevAPXState = false;

void loop() {
  trace_hifreq("loop()");

  unsigned long currTime = millis();
  int peakLevel = 0;

  // Only if the APX is online will we do anything with the VU Meters and Matrix/Strip displays...
  //  Otherwise we will just watch the WiFi for an MQTT message to bring us back online...
  //  Plus we'll go into a slow mode...
  if (APXPower.APXOnline() == true) {

    if (prevAPXState == false) {
      // Lets force the I2C bus back into action...
      I2C_Setup();
      prevAPXState = true;
    }
    debug_hifreq("APX is online...");

    // Only go to the VU meters if the lights are on...
    if (myMQTTClient.lightsOn == true)
    {
      debug_hifreq("Lights are on... cur=%ld, prev=%ld, diff=%ld", currTime, prevTime, (currTime - prevTime));

      if ((int)(currTime - prevTime) >= myMQTTClient.cyclespeed)  {
        debug_hifreq("Read from VU and display...");

        prevTime = currTime;

        VUData.I2C_loadVUData(myMQTTClient.demoMode);
        peakLevel = VUData.VUDataTotal[LEFT] + VUData.VUDataTotal[RIGHT];

        debug_hifreq("Peak: %d - Threshold: %d", peakLevel, myMQTTClient.sensitivity);

        // Filter low volume sounds...
        if (peakLevel < myMQTTClient.sensitivity) {
          peakLevel = 0;
        }

        // Manage displaying the matrix and strip and adding items to the pixBufferes if peakLevel > threshold...
        matrixCtrl.displaySample(myMQTTClient.selectedMatrixPattern, peakLevel);
        stripCtrl.displaySample(myMQTTClient.selectedStripPattern, peakLevel);
      }
    } // Lights aren't on, so we can sleep for a bit...
    else {
      matrixCtrl.turnOffTheLights();
      stripCtrl.turnOffTheLights();
      debug("Lights are off so relax...");
      delay(1000);
    }
  }  // APX is online...
  else {
    debug_hifreq("APX is offline...");
    prevAPXState = false;
    delay(1000);
  }

  // Only check for new messages every 1 second or so...
  if ((int)(currTime - prevMQTTCheckTime) >= 1000) {
    if (myMQTTClient.checkForNewMessage() == true) {
      myMQTTClient.dumpSubscriptionValues();

      // Check if the colour has changed...
      // colourCtrl.setColourPallet(myMQTTClient.lightsOn, myMQTTClient.selectedColour, myMQTTClient.getSelectedColour());
      colourCtrl.setColourPallet(myMQTTClient.lightsOn, myMQTTClient.selectedColour);
    }

    APXPower.checkPowerButton();
    prevMQTTCheckTime = currTime;
  }
}

/**
   I2C_ClearBus
   (http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html)
   (c)2014 Forward Computing and Control Pty. Ltd.
   NSW Australia, www.forward.com.au
   This code may be freely used for both private and commerical use
*/
/**
   This routine turns off the I2C bus and clears it
   on return SCA and SCL pins are tri-state inputs.
   You need to call Wire.begin() after this to re-enable I2C
   This routine does NOT use the Wire library at all.

   returns 0 if bus cleared
           1 if SCL held low.
           2 if SDA held low by slave clock stretch for > 2sec
           3 if SDA held low after 20 clocks.
*/
int I2C_ClearBus() {
  trace("");

#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN));                             //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP);                       // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power

  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW);      // Check is SCL is Low.
  if (SCL_LOW)                                      //If it is held low Arduno cannot become the I2C master.
    return 1;                                       //I2C bus error. Could not clear SCL clock line held low

  boolean SDA_LOW = (digitalRead(SDA) == LOW);      // vi. Check SDA input.
  int clockCount = 20;                              // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) {             //  vii. If SDA is Low,
    clockCount--;

    // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT);                            // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT);                           // then clock SCL Low
    delayMicroseconds(10);                          //  for >5uS
    pinMode(SCL, INPUT);                            // release SCL LOW
    pinMode(SCL, INPUT_PULLUP);                     // turn on pullup resistors again

    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10);                          //  for >5uS

    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW);            // Check if SCL is Low.

    int counter = 20;
    while (SCL_LOW && (counter > 0)) {              //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }

    if (SCL_LOW)                                    // still low after 2 sec error
      return 2;                                     // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec

    SDA_LOW = (digitalRead(SDA) == LOW);            //   and check SDA input again and loop
  }

  if (SDA_LOW)                                      // still low
    return 3;                                       // I2C bus error. Could not clear. SDA data line held low

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT);                              // remove pullup.
  pinMode(SDA, OUTPUT);                             // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT);                              // remove output low
  pinMode(SDA, INPUT_PULLUP);                       // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10);                            // x. wait >5uS
  pinMode(SDA, INPUT);                              // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);

  return 0;                                         // all ok
}

void I2C_Setup() {
  trace("");

  int rtn = I2C_ClearBus();                         // clear the I2C bus first before calling Wire.begin()

  if (rtn != 0) {
    error("I2C bus error. Could not clear");

    if (rtn == 1) {
      error("SCL clock line held low");
    }
    else if (rtn == 2) {
      error("SCL clock line held low by slave clock stretch");
    }
    else if (rtn == 3) {
      error("SDA data line held low");
    }
  }
  else {
    // bus clear
    // re-enable Wire
    // now can start Wire Arduino master
    Wire.begin();
  }
}
