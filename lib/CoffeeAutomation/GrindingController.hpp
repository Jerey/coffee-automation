#pragma once

#include <Arduino.h>
#include <HX711.h>
#include <MqttGrinder.hpp>

/**
 * @brief The GrindingController class is responsible for controlling the
 * grinder.
 *
 * It listens to MQTT messages and controls the grinder accordingly.
 * Since it is only controllable via MQTT, only the setup and loop functions are
 * exposed.
 */
class GrindingController {
 public:
  void setup();
  void loop();

 private:
  MqttGrinder mqttGrinder;
  unsigned int grindingTime = 0;
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

  /**
   * @brief Set the grinding time.
   *
   * This will then be used, if the grinder is started without a time.
   *
   * @param timeToGrind Time in milliseconds to grind.
   */
  void setGrindingTime(unsigned int timeToGrind);

  /**
   * @brief Time based grinding. The grinder will grind for the given time.
   *
   * MQTT messages are sent for the start and finish.
   *
   * @param startTriggerOrigin Who triggered the start - part of a MQTT message.
   * @param timeToGrind Time in milliseconds to grind.
   */
  void timeBasedGrinding(const char* startTriggerOrigin,
                         unsigned int timeToGrind);

  /**
   * @brief Get the current weight from the scale and publish it.
   *
   * @return The current weight in grams.
   */
  float getCurrentWeightAndPublish();

  /**
   * @brief Get the current weight from the scale.
   *
   * @return The current weight in grams.
   */
  float getCurrentWeight();
};
