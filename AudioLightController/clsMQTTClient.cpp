//*********************************************************************************************************************************************************************************************************************************************
// clsMQTTClient.cpp
//
// Class containing all the functionality to manage reading serial data from the NodeMCU module and loading the interesting variables...
//
//*********************************************************************************************************************************************************************************************************************************************
#define DEBUG                           TRUE
#define DEBUG_HIFREQ                    TRUE
#define TRACE                           TRUE
#define TRACE_HIFREQ                    TRUE
#define INFO                            TRUE
#define ERROR                           TRUE
#define WARNING                         TRUE
#include "Debug.h"

#include "clsMQTTClient.h"

void clsMQTTClient::begin()
{
  trace("Initialising NodeMCU");

  Serial3.begin(1200);
  // Serial3.begin(115200);
  Serial3.setTimeout(2000);
}

void clsMQTTClient::subscribe(char *pTopic)
{
  trace("'%s' - STILL TODO", pTopic);
  return;

  // TODO: Make this dynamic, just here for reference howto...
  sprintf(t_topic, pTopic);
  sprintf(t_value, "SUBSCRIBE");

  createJSON();

  // Send subscribe message over Serial to NodeMCU...
  Serial3.println(t_jsonStr);
}

bool clsMQTTClient::checkForNewMessage()
{
  bool retval_NewMsg = false;

  if (Serial3.available() > 0) {
    String input;

    // Read the incoming stream
    input = Serial3.readStringUntil('\n');

    debug("NodeMCU Message Rx: '%s'", input.c_str() );

    // Quick check on wether to just throw this message away...
    if (input.substring(0, 5) == "DEBUG")
      return false;

    // Parse the JSON and ensure it is a valid JSON...
    strncpy(t_jsonStr, input.c_str(), MAX_JSONSTR_LEN);
    parseJSON();

    debug("ARDUINO: parseJSON topic = '%s', value='%s'", t_topic, t_value);

    // Is it a valid message...
    if (strcmp(t_topic, "UNKNOWN")) {
      debug("MQTT Message received: '%s'", input.c_str());

      // Message received, now set the Public variable...
      setPublicVariable();
      retval_NewMsg = true;
    }
    else {
      warning("ARDUINO: Invalid message received: '%s'", input.c_str());
    }
  }
  else {
    debug_hifreq("Nothing for me on Serial3");
  }

  return retval_NewMsg;
}

void clsMQTTClient::createJSON(void)
{
  // Create JSON Object to send to NodeMCU...
  JSONVar myObject;
  myObject["topic"] = t_topic;
  myObject["value"] = t_value;

  String jsonString = JSON.stringify(myObject);

  jsonString.toCharArray(t_jsonStr, MAX_JSONSTR_LEN);
}

void clsMQTTClient::parseJSON(void)
{
  trace("clsMQTTClient");

  bool msgOK = true;

  debug("ARDUINO: >> Parse input JSON: '%s'", t_jsonStr);

  JSONVar myObject = JSON.parse(t_jsonStr);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
    msgOK = false;;

  if (msgOK ==  true) {
    if (myObject.hasOwnProperty("topic"))
      strncpy(t_topic, (const char*) myObject["topic"], MAX_TOPIC_LEN);
    else
      msgOK = false;

    if (msgOK == true) {
      if (myObject.hasOwnProperty("value"))
        strncpy(t_value, (const char*) myObject["value"], MAX_VALUE_LEN);
      else
        msgOK = false;
    }
  }

  if (msgOK == false) {
    // Set the values to UNKNOWN so the message interpreter will ignore them...
    strcpy(t_topic, "UNKNOWN");
    strcpy(t_value, "UNKNOWN");
  }
  return;
}

void clsMQTTClient::setPublicVariable(void)
{
  trace("clsMQTTClient");

  bool retval = false;

  // Look through the subscriptions list to see if this is a variable we are interested in...
  for (int i = 0; i < NUM_SUBSCRIPTIONS; i++)
  {
    if (!strcmp(t_topic, mqttSubscriptions[i].c_str()))
    {
      keystatus("Valid MQTT Received");
      keystatus(" Topic='%s'", t_topic);
      keystatus(" Value='%s'", t_value);

      switch (i) {
        case 0: lightsOn = convertValueToBool();       break;
        case 1:
          ledColour = t_value;
          // Now set the ENUM value...
          selectedColour = COLOUR_NOTSET;
          for (int i = 0; i < NUM_COLOUR_OPTS; i++) {
            if (!ledColour.compareTo(colourOptions[i])) {
              selectedColour = (eColours) i ;
              break;
            }
          }
          break;
        case 2: randomColour = convertValueToBool();      break;
        case 3:
          matrixPattern = t_value;
          // Now set the ENUM value...
          selectedMatrixPattern = MATRIXPATTERN_NOTSET;
          for (int i = 0; i < NUM_MATRIXPATTERN_OPTS; i++) {
            if (!matrixPattern.compareTo(matrixPatternOptions[i])) {
              selectedMatrixPattern = (eMatrixPattern) i ;
              break;
            }
          }
          break;
        case 4:
          stripPattern = t_value;
          // Now set the ENUM value...
          selectedStripPattern = STRIPPATTERN_NOTSET;
          for (int i = 0; i < NUM_STRIPPATTERN_OPTS; i++) {
            if (!stripPattern.compareTo(stripPatternOptions[i])) {
              selectedStripPattern = (eStripPattern) i ;
              break;
            }
          }
          break;
        case 5: randomPattern = convertValueToBool();         break;
        case 6: sensitivity = (100 - atoi(t_value));          break;
        case 7: cyclespeed = (100 - atoi(t_value));           break;
        case 8: demoMode = convertValueToBool();             break;
      };

      retval = true;
      break;
    }
  }

  if (!retval) {
    debug("Topic Ignored '%s'", t_topic);
  }
}

bool clsMQTTClient::convertValueToBool(void)
{
  if (!strcmp(t_value, "true"))
    return true;

  return false;
}

void clsMQTTClient::dumpSubscriptionValues()
{
  info("NEW MESSAGE RECIEVED...");

  for (int i = 0; i < DEBUG_BUFFER_SIZE; i++)
  {
    if (!(i % 10))
      Serial.print("|");
    else
      Serial.print(".");
  }
  Serial.println("<< Nothing should be longer");

  info("lightsOn = %d", lightsOn);
  info("Colour = %s [%d]", ledColour.c_str(), selectedColour);
  info("rndColour = %d", randomColour);
  info("matrix = %s [%d]", matrixPattern.c_str(), selectedMatrixPattern);
  info("strip = %s [%d]", stripPattern.c_str(), selectedStripPattern);
  info("rndPattern = %d", randomPattern);
  info("sensitivity = %d", sensitivity);
  info("cyclespeed = %d", cyclespeed);
  info("demomode = %d", demoMode);
}
