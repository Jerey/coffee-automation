#pragma once

#include <Arduino.h>
#include <HX711.h>
#include <MqttGrinder.hpp>

class GrindingController {
 public:
  void setup();
  void loop();

 private:
  MqttGrinder mqttGrinder;
  unsigned int grindingTime;
  HX711 scale;
  unsigned long lastScaleUpdate;

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
