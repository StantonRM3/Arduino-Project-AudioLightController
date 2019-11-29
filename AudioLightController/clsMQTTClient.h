//*********************************************************************************************************************************************************************************************************************************************
// clsMQTTClient.h
//
// Class containing all the functionality to manage DS1307 RTC Clock, etc, etc...
//*********************************************************************************************************************************************************************************************************************************************
// Prevent the header getting included multiple times...
#ifndef _CLSMQTTCLIENT_H
#define _CLSMQTTCLIENT_H

#include "Enums.h"
#include <SoftwareSerial.h>
#include <Arduino_JSON.h>

#define MAX_JSONSTR_LEN   96
#define MAX_TOPIC_LEN     48
#define MAX_VALUE_LEN     32

#define MIN_LOWVOLSENSITIVITY   20
#define MIN_CYCLESPEED          30

#define NUM_STRIPPATTERN_OPTS 9
const String stripPatternOptions[NUM_STRIPPATTERN_OPTS] = {
  "off",
  "ferriswheel",
  "ferriswheel_trig",
  "cyclon",
  "sparkle",
  "vu",
  "bounce",
  "swell",
  "brain"
};

#define NUM_MATRIXPATTERN_OPTS 11
const String matrixPatternOptions[NUM_MATRIXPATTERN_OPTS] = {
  "off",
  "vu",
  "vupeak",
  "huebars",
  "huebars_rev",
  "pulsar",
  "sideways",
  "sidepulse",
  "scope",
  "tartan",
  "starlight"
};

#define NUM_COLOUR_OPTS 12
const String colourOptions[NUM_COLOUR_OPTS] {
  "red",
  "blue",
  "green",
  "yellow",
  "pink",
  "purple",
  "white",
  "red-blue",
  "green-yellow",
  "blue-green",
  "rainbow",
  "pastel"
};

// I'm going to be dirty here and use the index to determine what variable to put the value in, therefor list order IS important...
#define NUM_SUBSCRIPTIONS 9
const String mqttSubscriptions[NUM_SUBSCRIPTIONS] = {
  "home/lightsOnOff",
  "home/ledColour",
  "home/randomColour",
  "home/sirenOne/matrixPattern",
  "home/sirenOne/stripPattern",
  "home/sirenOne/randomPattern",
  "home/sirenOne/sensitivity",
  "home/sirenOne/cyclespeed",
  "home/demo"
};

class clsMQTTClient
{
  public:
    void begin();
    bool checkForNewMessage(void);
    void dumpSubscriptionValues(void);

    // MQTT Values...
    eStripPattern selectedStripPattern = cyclon;
    eMatrixPattern selectedMatrixPattern = pulsar;
    eColours selectedColour = rainbow;
    bool lightsOn = true;
    bool randomColour = false;
    bool randomPattern = false;
    int sensitivity = 30;
    int cyclespeed = 40;
    bool demoMode = true;

  private:

    // Set by MQTT Message and translated into ENUM above
    String matrixPattern = matrixPatternOptions[selectedMatrixPattern]; // "UNKNOWN";
    String stripPattern = stripPatternOptions[selectedStripPattern];    // "UNKNOWN";
    String ledColour = colourOptions[selectedColour];                   // "UNKNOWN";
    
    // Single multiuse buffers...
    char t_jsonStr[MAX_JSONSTR_LEN];
    char t_topic[MAX_TOPIC_LEN],
         t_value[MAX_VALUE_LEN];
         
    void subscribe(char *pTopic);
    void createJSON(void);
    void parseJSON(void);
    void setPublicVariable(void);
    bool convertValueToBool(void);
};

#endif
