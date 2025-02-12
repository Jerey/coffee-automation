#include "MqttGrinder.hpp"
#include <Constants.hpp>

MqttGrinder::MqttGrinder() : espClient(WiFiClient()), mqttClient(espClient) {}

void MqttGrinder::subscribeToMqttTopics() {
  mqttClient.subscribe(topicInSetGrindingTime);
  mqttClient.subscribe(topicInStart);
  mqttClient.subscribe(topicInTare);
  mqttClient.subscribe(topicInAutomatic);
}

void MqttGrinder::reconnectToMqttBroker() {
  while (not mqttClient.connected()) {
    Serial.print("Reconnecting...");
    if (not mqttClient.connect(hostName)) {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
    subscribeToMqttTopics();
  }
}
void MqttGrinder::publishMqttTopicAndValue(const char* topic, float value) {
  char mqttBuffer[16];
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%.1f", value);  // One decimal place
  publishMqtt(topic, mqttBuffer);
}

void MqttGrinder::publishMqttTopicAndValue(const char* topic,
                                           const char* value) {
  publishMqtt(topic, value);
}

void MqttGrinder::publishMqttTopicAndValue(const char* topic,
                                           unsigned int value) {
  char mqttBuffer[12];
  snprintf(mqttBuffer, sizeof(mqttBuffer), "%u", value);
  publishMqtt(topic, mqttBuffer);
}
void MqttGrinder::publishMqtt(const char* topic, const char* payload) {
  reconnectToMqttBroker();
  mqttClient.loop();

  printTopicAndValue(topic, payload);

  if (not mqttClient.publish(topic, payload)) {
    Serial.print("Failed to publish the topic '");
    Serial.print(topic);
    Serial.println("'");
  }
}

void MqttGrinder::setup(
    std::function<void(char*, uint8_t*, unsigned int)> callback) {
  mqttClient.setServer(mqttBroker, 1883);
  mqttClient.setCallback(callback);
  reconnectToMqttBroker();
}

void MqttGrinder::loop() {
  mqttClient.loop();
  reconnectToMqttBroker();
}
