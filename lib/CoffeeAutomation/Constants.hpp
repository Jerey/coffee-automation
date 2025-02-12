#pragma once

static constexpr const char* ssidAP = "AutoConnectAP";
static constexpr const char* passwordAP = "password";
static constexpr unsigned int configPortalTimeout = 600;

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

static constexpr const char* mqttBroker = "192.168.178.100";
static constexpr const char* hostName = "CoffeeGrinder";

static constexpr float thresholdTargetGrams = 1.0f;
static constexpr unsigned int scaleUpdateTime = 500;
