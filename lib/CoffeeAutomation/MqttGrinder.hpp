#pragma once

#include <PubSubClient.h>
#include <WiFiManager.h>

/**
 * @class MqttGrinder
 * @brief A class to manage MQTT communication for a coffee grinder.
 */
class MqttGrinder {
 public:
  /**
   * @brief Constructor for MqttGrinder.
   */
  MqttGrinder();

  /**
   * @brief Sets up the MQTT client with a callback function.
   * @param callback A function to handle incoming MQTT messages.
   */
  void setup(std::function<void(char*, uint8_t*, unsigned int)> callback);

  /**
   * @brief Handles the MQTT client loop, ensuring it stays connected and
   * processes messages.
   */
  void loop();

  /**
   * @brief Publishes a float value to a specified MQTT topic.
   * @param topic The MQTT topic to publish to.
   * @param value The float value to publish.
   */
  void publishMqttTopicAndValue(const char* topic, float value);

  /**
   * @brief Publishes a string value to a specified MQTT topic.
   * @param topic The MQTT topic to publish to.
   * @param value The string value to publish.
   */
  void publishMqttTopicAndValue(const char* topic, const char* value);

  /**
   * @brief Publishes an unsigned int value to a specified MQTT topic.
   * @param topic The MQTT topic to publish to.
   * @param value The unsigned int value to publish.
   */
  void publishMqttTopicAndValue(const char* topic, unsigned int value);

 private:
  WiFiClient espClient;     ///< WiFi client for network communication.
  PubSubClient mqttClient;  ///< MQTT client for handling MQTT communication.

  /**
   * @brief Subscribes to necessary MQTT topics.
   */
  void subscribeToMqttTopics();

  /**
   * @brief Reconnects to the MQTT broker if the connection is lost.
   */
  void reconnectToMqttBroker();

  /**
   * @brief Publishes a payload to a specified MQTT topic.
   * @param topic The MQTT topic to publish to.
   * @param payload The payload to publish.
   */
  void publishMqtt(const char* topic, const char* payload);

  /**
   * @brief Prints the topic and value to the serial monitor.
   * @tparam valueType The type of the value to print.
   * @param topic The MQTT topic.
   * @param value The value to print.
   */
  template <typename valueType>
  void printTopicAndValue(const char* topic, valueType value) {
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(value);
  }
};
