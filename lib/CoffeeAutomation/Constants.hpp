#pragma once

// The access point name of the ESP when it is in wifi configuration mode.
static constexpr const char* ssidAP = "AutoConnectAP";
// The default password to the access point of the ESP when it is in wifi
// configuration mode.
static constexpr const char* passwordAP = "password";
// A timeout of the wifi configuration portal in seconds.
// After this timeout, the ESP will restart and try to connect to the wifi
// again.
static constexpr unsigned int configPortalTimeout = 600;

// The mqtt broker IP address.
static constexpr const char* mqttBroker = "192.168.178.100";
// The hostname for the grinder for
// - the mqtt connection
// - firmware updates over the air (OTA)
static constexpr const char* hostName = "CoffeeGrinder";

// A threshold for the travel distance from the grinder to the scale.
static constexpr float thresholdTargetGrams = 1.0f;
// How often a scale update can happen in ms.
static constexpr unsigned int scaleUpdateTime = 500;

// The pin for the relay to control the grinder.
constexpr int relay = 15;

// Scale Configuration
constexpr int scaleData = 4;
constexpr int scaleClk = 5;
constexpr float scaleCalibrationFactor = 418;

// MQTT topics
static constexpr const char* topicInSetGrindingTime =
    "grinder/in/setGrindingTime";
static constexpr const char* topicInStart = "grinder/in/start";
static constexpr const char* topicInTare = "grinder/in/tare";
static constexpr const char* topicInAutomatic = "grinder/in/automatic";

static constexpr const char* topicOutStarted = "grinder/out/started";
static constexpr const char* topicOutCurrentSetGrindingTime =
    "grinder/out/currentSetGrindingTime";
static constexpr const char* topicOutFinished = "grinder/out/finished";
static constexpr const char* topicOutAutomaticFinished =
    "grinder/out/automaticFinished";
static constexpr const char* topicOutCurrentWeight = "grinder/out/weight";
