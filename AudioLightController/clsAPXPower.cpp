//*********************************************************************************************************************************************************************************************************************************************
// clsAPXPower.cpp
//
// Class containing all the functionality to manage APX Power Supply and Standby Pushbutton (with LED)
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

// #include <Arduino.h>
#include "clsAPXPower.h"

void clsAPXPower::begin(int pButtonPin, int pLEDPin, int pAPXOnlinePin)
{
  trace("clsAPXPower");
  buttonPin = pButtonPin;
  buttonLEDPin = pLEDPin;
  APXOnlinePin = pAPXOnlinePin;

  pinMode(buttonPin, INPUT);
  pinMode(buttonLEDPin, OUTPUT);
  pinMode(APXOnlinePin, OUTPUT);
}

bool clsAPXPower::APXOnline(void)
{
  // TODO: Kludge
  return true;

  trace_hifreq("");
  bool retval = false;

  // If the output pin is LOW, then this has grounded the control wire and the APX is online (i.e. all the power lines are up)...
  // Could improve this by watching the actual APX control cable (grey) which comes up to 5V once the APX is fully up...
  if (APXOnlinePinState == LOW)
    retval = true;

  return retval;
}

void clsAPXPower::checkPowerButton()
{
  trace("clsAPXPower");

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        APXOnlinePinState = !APXOnlinePinState;
        buttonLEDState = !buttonLEDState;
      }
    }
  }

  // set the LED and APXOnlinePin state:
  digitalWrite(buttonLEDPin, buttonLEDState);
  digitalWrite(APXOnlinePin, APXOnlinePinState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
