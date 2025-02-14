---
title: Coffee Automation
theme: solarized
---
<!-- https://jerey.at/coffee-automation -->

# Coffee Automation

<div style="display: table; width: 100%;">
  <div style="display: table-row">
    <div style="display: table-cell; vertical-align: middle;">
      <img src="CoffeeGrinderAutomation.JPG" height="350">
    </div>
    <div style="display: table-cell; vertical-align: middle;">
      <ul>
        <li>Automation of a coffee grinder</li>
        <li>C++ / ESP8266 Wi-Fi module</li>
        <li>Created in 2020</li>
        <li><a href="https://github.com/Jerey/coffee-automation">Github</a></li>
      </ul>
    </div>
  </div>
</div>

----

## Automation Options

- **Time-Based Grinding** - Runs for a set duration.
- **Weight-Based Grinding** - Stops when target weight is reached.

<img src="FinalTouches.jpg" width="500">

----

## Node RED Dashboard

<img src="Node-RED Dashboard.png" width="700">

---

## Introduction

- I like coffee
- Beans weighed by hand
- Grinders often offer time based grinding
- Sometimes even weight based
- ... mine offered neither

----

### What is this project?

- Automates the grinding of coffee beans
- Can be added to any coffee grinder*
- Provides precise grinding control (time-based & weight-based).
- Flexible UI due to mqtt

> \* with an on/off switch

----

### Why does it matter?

- Same amount improves repeatability
- Reduces manual effort
- Almost "removes" the grinding step
- Can be integrated in IoT

---

## System Overview

![](./CoffeeGrinder.svg)

> Kept minimal for experts only -> power outlet electricity

----

### Hardware Components

- **D1 Mini (ESP8266)** - Controller of the scale and relay
- **Relay Module** - Controls the power of the grinder
- **Load Cell + HX711 (Optional)** - Measures the ground coffee

----

### Software & Communication

- **ESP8266 Firmware (C++)** - Controls the automation logic
- **MQTT Protocol**
  - Common IoT protocol -> Great number of available integrations
  - Quality of Service levels
  - Publish and subscribe model
  - Broker is required
  <!-- - **Node-RED Dashboard (Optional)** - Web-based user interface
  - **CLI Tool Alternative (Optional)** - `mosquitto_pub -t grinder/in/start -n`
  - **...** -->

---

## Key Parts of the Codebase

<!-- TODO: Add an UML diagram here? -->
----

### MQTT Communication

- Separates the user interface from the controller
- Subscribed topics end up here:

```cpp[|4,5]
// byte and uint used for parameter communication
void callback(char* topic, byte*, unsigned int) {
    /* .. */
    else if (strcmp(topic, "grinder/in/tare") == 0) {
        scale.tare(5);
    }
    /* .. */
}
```

----

### Grinder Control

- Uses GPIO pins to control a relay
- Relay turning the grinder on and off

```cpp[|5-8|9]
void GrindingController::startGrinding(
                  unsigned int timeToGrind) {
  auto startingTime = millis();

  while (millis() - startingTime < timeToGrind) {
    digitalWrite(relay, HIGH);
    getCurrentWeightAndPublish();
  }
  digitalWrite(relay, LOW);
}
```

----

### Load Cell Integration

- Reads weight from HX711 sensor
- Sensor placed under the outlet

```cpp[|2]
float GrindingController::getCurrentWeight() {
  return scale.get_units(1);
}
```

----

### Automation Logic

- Weight based
  - Tares scale before grinding
  - Stops when weight target is met
- Time based
  - Grinds for a given time

----

### Automation Logic

```cpp[|3,4|6-9|11,12|14-16|19]
void GrindingController::automaticGrinding(
        float desiredGrams) {
  mqttGrinder.publishMqttTopicAndValue(/* started */);
  scale.tare(5);

  while ((getCurrentWeight() + thresholdTargetGrams) 
          < desiredGrams) {
    digitalWrite(relay, HIGH);
  }

  digitalWrite(relay, LOW);
  delay(500);

  while (getCurrentWeight() < desiredGrams) {
    startGrinding(150);
    delay(500);
  }

  mqttGrinder.publishMqttTopicAndValue(/* finished */);
}
```

---

## Challenges

- **Time to Scale**
  - Ground beans travel time to scale
  - Retention of the grinder
- **MQTT Topics**
  - Many of them
  - Proper naming is hard
- **Testing**
  - PlatformIO (Development environment)
  - Mostly manual

---

## Future Enhancements

- **Time to Scale** - Remember the really required threshold
- **Simple Controls** - Add physical controls
- **Energy Efficiency** - Make use of deep sleep
  - OTA challenges
  - Wake up challenges
- **Further Refactoring**

---

## Q & A
