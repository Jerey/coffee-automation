#include <Arduino.h>
#include <ArduinoOTA.h>
#include <HX711.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

//----------- Hostname -----------
constexpr const char* hostName = "CoffeeGrinder";

//----------- In/Out -----------
constexpr int relay = 15;

//----------- Scale -----------
constexpr int scaleData = 4;
constexpr int scaleClk = 5;
HX711 scale;
constexpr float scaleCalibrationFactor = 418;
float desiredGrams = 16.8;
unsigned long lastScaleUpdate;
unsigned int scaleUpdateTime = 500;

//----------- AP -----------
constexpr const char* ssidAP = "AutoConnectAP";
constexpr const char* passwordAP = "password";

//----------- MQTT -----------
constexpr const char* mqttBroker = "192.168.178.100";
WiFiClient espClient;
PubSubClient mqttClient(espClient);
char mqttBuffer[64];
unsigned int grindingTime = 0 * 1000;
unsigned long grindingStartedTime;

constexpr const char* topicInSetGrindingTime = "grinder/in/setGrindingTime";
constexpr const char* topicInStart = "grinder/in/start";
constexpr const char* topicInTare = "grinder/in/tare";
constexpr const char* topicInAutomatic = "grinder/in/automatic";

constexpr const char* topicOutStarted = "grinder/out/started";
constexpr const char* topicOutCurrentSetGrindingTime =
    "grinder/out/currentSetGrindingTime";
constexpr const char* topicOutFinished = "grinder/out/finished";
constexpr const char* topicOutAutomaticFinished =
    "grinder/out/automaticFinished";
constexpr const char* topicOutCurrentWeight = "grinder/out/weight";

//----------- Grinding Flags -----------
bool grindingOngoing = false;
bool automaticGrindingOngoing = false;

/**
 * Function to connect to a wifi. It waits until a connection is established.
 */
void connectToWifi() {
  WiFiManager wifiManager;
  wifiManager.setBreakAfterConfig(true);
  wifiManager.autoConnect(ssidAP, passwordAP);
}

void subscribeToMqttTopics() {
  mqttClient.subscribe(topicInSetGrindingTime);
  mqttClient.subscribe(topicInStart);
  mqttClient.subscribe(topicInTare);
  mqttClient.subscribe(topicInAutomatic);
}

template <typename valueType>
void printTopicAndValue(const char* topic, valueType value) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(value);
}

void reconnectToMqttBroker() {
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

template <typename valueType>
void publishMqttTopicAndValue(const char* topic, valueType value) {
  reconnectToMqttBroker();
  mqttClient.loop();
  printTopicAndValue(topic, value);
  auto resultOfPublish = mqttClient.publish(topic, mqttBuffer);
  if (not resultOfPublish) {
    Serial.print("Failed to publish the topic '");
    Serial.print(topic);
    Serial.print("'");
  }
}

float getCurrentWeightAndPublish() {
  auto currentWeight = scale.get_units(3);
  if ((millis() - lastScaleUpdate) > scaleUpdateTime) {
    lastScaleUpdate = millis();
    snprintf(mqttBuffer, sizeof mqttBuffer, "%.1f", currentWeight);
    publishMqttTopicAndValue(topicOutCurrentWeight, currentWeight);
  }
  return currentWeight;
}

unsigned int getIntFromPayload(byte* payload, unsigned int length) {
  payload[length] = '\0';
  String s = String((char*)payload);
  return s.toInt();
}

void setGrindingTime(unsigned int timeToGrind) {
  grindingTime = timeToGrind;
  snprintf(mqttBuffer, sizeof mqttBuffer, "%d", grindingTime);
  publishMqttTopicAndValue(topicOutCurrentSetGrindingTime, grindingTime);
}

void startGrinding(const char* startTriggerOrigin,
                   unsigned int timeToGrind = grindingTime) {
  grindingOngoing = true;
  grindingStartedTime = millis();
  setGrindingTime(timeToGrind);
  strcpy(mqttBuffer, startTriggerOrigin);
  publishMqttTopicAndValue(topicOutStarted, startTriggerOrigin);
  digitalWrite(relay, HIGH);
}

/**
 * @brief MQTT Callback
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message [");
  Serial.print(topic);
  Serial.println("] ");

  if (strcmp(topic, topicInSetGrindingTime) == 0) {
    setGrindingTime(getIntFromPayload(payload, length));
    Serial.print("-> ");
    Serial.println(grindingTime);
  } else if (strcmp(topic, topicInStart) == 0) {
    if (length > 0) {
      startGrinding("callbackWithTime", getIntFromPayload(payload, length));
    } else {
      startGrinding("callbackWithoutTime");
    }
  } else if (strcmp(topic, topicInTare) == 0) {
    scale.tare(5);
  } else if (strcmp(topic, topicInAutomatic) == 0) {
    scale.tare(5);
    if (length > 0) {
      desiredGrams = ((float)getIntFromPayload(payload, length) - 0.2);
    } else {
      desiredGrams = 16.8;
    }
    getCurrentWeightAndPublish();
    automaticGrindingOngoing = true;
    startGrinding("automaticGrinding", 6000);
  } else {
    Serial.print("Unhandled topic '");
    Serial.print(topic);
    Serial.println("'!");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  connectToWifi();
  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(mqttBroker, 1883);
  mqttClient.setCallback(callback);
  reconnectToMqttBroker();

  scale.begin(scaleData, scaleClk);
  scale.set_scale(scaleCalibrationFactor);
  scale.tare();
}

void loop() {
  mqttClient.loop();
  reconnectToMqttBroker();

  getCurrentWeightAndPublish();
  if (grindingOngoing) {
    if ((millis() - grindingStartedTime) > grindingTime) {
      grindingOngoing = false;
      digitalWrite(relay, LOW);
      snprintf(mqttBuffer, sizeof mqttBuffer, "%d", grindingTime);
      publishMqttTopicAndValue(topicOutFinished, grindingTime);
    }
    if (not grindingOngoing && automaticGrindingOngoing) {
      delay(500);
      auto currentWeight = getCurrentWeightAndPublish();
      if (desiredGrams <= currentWeight) {
        automaticGrindingOngoing = false;
        snprintf(mqttBuffer, sizeof mqttBuffer, "%.1f", currentWeight);
        publishMqttTopicAndValue(topicOutAutomaticFinished, currentWeight);
      } else {
        startGrinding("automaticGrinding", 150);
      }
    }
  } else {
    ArduinoOTA.handle();
  }
}