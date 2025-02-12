#pragma once

#include <Arduino.h>
#include <HX711.h>
#include <MqttGrinder.hpp>

class GrindingController {
 public:
  GrindingController();

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

  // Depending on the mill, it might take some time for the beans ..
  // .. to reach the scale. For this, the threshold can be set.
  const float thresholdTargetGrams = 1.0f;
  unsigned long lastScaleUpdate;
  unsigned int scaleUpdateTime = 500;

  /**
   * @brief A callback function when MQTT messages are received.
   *
   * @param topic The topic of the message.
   * @param payload The payload of the message.
   * @param length The length of the payload.
   */
  void callback(char* topic, byte* payload, unsigned int length);

  /**
   * @brief Weight based grinding. The grinder will grind until the desired
   * weight is reached.
   *
   * @param desiredGrams The desired weight in grams.
   */
  void automaticGrinding(float desiredGrams);

  /**
   * @brief Time based grinding. The grinder will grind for the given time.
   *
   * No MQTT messages or updates are sent.
   *
   * @param timeToGrind Time in milliseconds to grind.
   */
  void startGrinding(unsigned int timeToGrind);

  void setGrindingTime(unsigned int timeToGrind);

  void timeBasedGrinding(const char* startTriggerOrigin,
                         unsigned int timeToGrind);

  float getCurrentWeightAndPublish();

  float getCurrentWeight();
};
