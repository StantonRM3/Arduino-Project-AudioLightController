//*********************************************************************************************************************************************************************************************************************************************
// clsAPXPower.h
//
// Class containing all the functionality to manage APX Power Supply, etc, etc...
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef CLSAPXPOWER_H
#define CLSAPXPOWER_H

#include <Arduino.h>

class clsAPXPower
{
  public:
    void begin(int pButtonPin, int pLEDPin, int pAPXOnlinePin);
    void checkPowerButton();
    bool APXOnline(void);
    
  private:

    // PIN's
    int buttonPin = 13;                        // the number of the pushbutton pin
    int buttonLEDPin = 12;                     // the number of the pushbutton LED pin
    int APXOnlinePin = 11;                     // the pin used to bring the APX power supply online

    // Variables will change:
    int buttonLEDState = HIGH;                 // the current state of the output pin
    int APXOnlinePinState = LOW;               // the current state of the APXOnline output pin
    int buttonState = LOW;                     // the current reading from the input pin
    int lastButtonState = LOW;                 // the previous reading from the input pin

    // the following variables are unsigned longs because the time, measured in
    // milliseconds, will quickly become a bigger number than can be stored in an int.
    unsigned long lastDebounceTime = 0;        // the last time the output pin was toggled
    const unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
};

#endif
