#pragma once

#include <PubSubClient.h>
#include <WiFiManager.h>

class MqttGrinder {
 public:
  MqttGrinder();
  void subscribeToMqttTopics();
  void reconnectToMqttBroker();

  void setup(std::function<void(char*, uint8_t*, unsigned int)> callback);
  void loop();

  template <typename valueType>
  void printTopicAndValue(const char* topic, valueType value) {
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(value);
  }
  void publishMqttTopicAndValue(const char* topic, float value);
  void publishMqttTopicAndValue(const char* topic, const char* value);
  void publishMqttTopicAndValue(const char* topic, unsigned int value);

 private:
  WiFiClient espClient;
  PubSubClient mqttClient;

  void publishMqtt(const char* topic, const char* payload);
};
