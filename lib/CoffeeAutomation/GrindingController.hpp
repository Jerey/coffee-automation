#pragma once

#include <Arduino.h>
#include <HX711.h>
#include <MqttGrinder.hpp>

class GrindingController {
 public:
  GrindingController();
  void setGrindingTime(unsigned int timeToGrind);

  void startGrinding(const char* startTriggerOrigin, unsigned int timeToGrind);
  float getCurrentWeightAndPublish();

  void setup();
  void loop();

 private:
  MqttGrinder mqttGrinder;
  unsigned int grindingTime;
  unsigned long grindingStartedTime;

  //----------- Grinding Flags -----------
  bool grindingOngoing = false;
  bool automaticGrindingOngoing = false;

  HX711 scale;
  float desiredGrams = 16.8;
  // Depending on the mill, it might take some time for the beans ..
  // .. to reach the scale. For this, the threshold can be set.
  const float thresholdTargetGrams = 1.0f;
  unsigned long lastScaleUpdate;
  unsigned int scaleUpdateTime = 500;

  /**
   * @brief MQTT Callback
   */
  void callback(char* topic, byte* payload, unsigned int length);
};
