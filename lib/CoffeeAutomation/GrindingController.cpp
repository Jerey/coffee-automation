#include <Constants.hpp>
#include <GrindingController.hpp>

//----------- In/Out -----------
constexpr int relay = LED_BUILTIN;

//----------- Scale -----------
constexpr int scaleData = 4;
constexpr int scaleClk = 5;
constexpr float scaleCalibrationFactor = 418;

GrindingController::GrindingController() : mqttGrinder() {}

void GrindingController::setGrindingTime(unsigned int timeToGrind) {
  grindingTime = timeToGrind;
  mqttGrinder.publishMqttTopicAndValue(topicOutCurrentSetGrindingTime,
                                       grindingTime);
}

void GrindingController::startGrinding(const char* startTriggerOrigin,
                                       unsigned int timeToGrind) {
  auto startingTime = millis();
  mqttGrinder.publishMqttTopicAndValue(topicOutStarted, startTriggerOrigin);

  while (millis() - startingTime < timeToGrind) {
    digitalWrite(relay, HIGH);
    getCurrentWeightAndPublish();
  }
  digitalWrite(relay, LOW);
  mqttGrinder.publishMqttTopicAndValue(topicOutFinished, timeToGrind);
}

float GrindingController::getCurrentWeight() {
  return scale.get_units(1);
}

float GrindingController::getCurrentWeightAndPublish() {
  auto currentWeight = scale.get_units(1);
  if ((millis() - lastScaleUpdate) > scaleUpdateTime) {
    lastScaleUpdate = millis();
    mqttGrinder.publishMqttTopicAndValue(topicOutCurrentWeight, currentWeight);
  }
  return currentWeight;
}

unsigned int getIntFromPayload(byte* payload, unsigned int length) {
  payload[length] = '\0';
  String s = String((char*)payload);
  return s.toInt();
}

void GrindingController::callback(char* topic,
                                  byte* payload,
                                  unsigned int length) {
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
      startGrinding("callbackWithoutTime", grindingTime);
    }
  } else if (strcmp(topic, topicInTare) == 0) {
    scale.tare(5);
  } else if (strcmp(topic, topicInAutomatic) == 0) {
    if (length > 0) {
      automaticGrinding((float)getIntFromPayload(payload, length) - 0.2);
    } else {
      automaticGrinding(16.8);  // TODO: Magic number.
    }
  } else {
    Serial.print("Unhandled topic '");
    Serial.print(topic);
    Serial.println("'!");
  }
}

void GrindingController::automaticGrinding(float desiredGrams) {
  mqttGrinder.publishMqttTopicAndValue(topicOutStarted, "automaticGrinding");
  // Start with taring the scale
  scale.tare(5);

  // Approach the desired weight with a threshold (e.g. travel distance
  // between grinder and scale).
  while ((getCurrentWeightAndPublish() + thresholdTargetGrams) < desiredGrams) {
    digitalWrite(relay, HIGH);
  }

  // Stop the grinder and wait for the beans to fall down
  digitalWrite(relay, LOW);
  delay(500);

  // Now lets slowly approach the desired weight
  while (getCurrentWeightAndPublish() < desiredGrams) {
    startGrinding("automaticGrinding", 150);
  }

  mqttGrinder.publishMqttTopicAndValue(topicOutAutomaticFinished,
                                       getCurrentWeight());
}

void GrindingController::setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(
      relay, LOW);  // TODO: This can be removed when a real relay is attached.
  mqttGrinder.setup(std::bind(&GrindingController::callback, this,
                              std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3));

  scale.begin(scaleData, scaleClk);
  scale.set_scale(scaleCalibrationFactor);
  scale.tare();
}

void GrindingController::loop() {
  mqttGrinder.loop();
  getCurrentWeightAndPublish();
}
