# Coffee Grinder Control

Automate your coffee grinder with the help of a D1 mini (or any similar ESP based device) and (optional) a scale.

## Motivation

I enjoy a good cup of coffee. For that, I have bought myself a coffee grinder (Graef CM 800) to start with. This grinder has a switch with three positions: Off, on and grind while pressing a small button.

Working from home during the corona pandemic made me drink more coffee at home and I started to get annoyed of the manual labour of controlling the amount of grinded beans. Inspired by [Roman Seidl's repository](https://github.com/romanseidl/MillControl), I decided to automatize my mill.

## Goal

The outcome of this project shall be able to keep the grinder on for a given time or until a target weight is reached. Further, it should not require a lot of hardware to rebuild.

## Solution

Since I had some D1 mini laying around at home, I chose it as the brain of this project. Since I have already used **mqtt** with my [weatherstation](https://github.com/jerey/weatherstation-client), I also decided to control the mill via mqtt topics. Further I had also played around with **Node-Red** and **Node-Red-Dashboard**, which made it really easy to quickly create a control panel for the grinder. See [the sample of my dashboard](#control-the-grinder).

<img src="/documentation/FinalTouches.jpg" width="400">

### MQTT Topics

The topics defined for this project are split up in two types: incoming and outgoing. All topics start with `grinder/`.

#### Incoming

These are the topics used to control the mill. All start with the topic `in/`.

- `grinder/in/start`: Starts the grinder with the time given in the payload. If no time is specified in the payload, the last set time is used.
- `grinder/in/setGrindingTime`: Sets the time, which will be used if start is called without a time specified.
- `grinder/in/tare`: Tares the scale.
- `grinder/in/automatic`: Starts the grinder with the desired bean amount given in the payload. If no amount is specified in the payload, the last set amount is used. This also **tares** the scale before starting.

#### Outgoing

In order to get updates from the grinder, the following topics are published by the D1 mini. These can be visualized by subscribing to them.

- `grinder/out/started`: Indicates, that the grinder was started. The payload holds the reason, why it was started.
- `grinder/out/currentSetGrindingTime`: Published, when the stored time was changed. The payload indicates the new time (in `ms`).
- `grinder/out/finished`: Whenever the current grinding is finished, this topic is published along with the set grinding time.
- `grinder/out/automaticFinished`: If you start an automatic grinding session, this indicates, that it was finished. The payload contains the grinded weight.
- `grinder/out/weight`: Published on a regular basis to indicate the current weight on the scale.

## Setup

If you want to setup this project at home, you will need following things:

- D1 mini (or similar ESP device)
- Relay controllable by the D1 mini and supporting the electricity of your mill
- A load cell and a load cell amplifier (I used the **HX711**)
- A mqtt broker

### Flashing

Initially, you will have to flash this software via USB, but after that, OTA is available and already configured.

### Variables in the code

- `hostName`: Defines the `MQTT` and `OTA` hostname. This is also the name defined in the `platformio.ini`-file at `upload_port`.
- `relay`: Defines the GPIO port of the relay data signal.
- `scaleData`: Defines the GPIO port of the scale data signal.
- `scaleClk`: Defines the GPIO port of the scale clock signal.
- `scaleCalibrationFactor`: Defines the scale calibration factor. See [scale calibration](#scale-calibration).
- `desiredGrams`: The default desired grams. This is the target weight you desire when automatically grinding.
- `thresholdTargetGrams`: Since the distance between the grinder/bean outlet to the scale can cause the amount being to big, one can define a threshold, to prevent this.
- `scaleUpdateTime`: The frequency of updates published for the current weight.
- `ssidAP`: The name of the access point opened up by the D1 mini, when it has no WiFi to connect to. If you connect to the D1 mini via this access point, it will allow you to provide credentials to a WiFi of your choice within reach.
- `passwordAP`: The password needed to connect to the access point of your D1 mini, when it is opened.
- `mqttBroker`: The ip address of your mqtt broker.
- `grindingTime`: The default grinding time. It is used, when start is called without any payload.

### Schematic

:warning: Do not work on the mill's electricity, if you have no clue of what you are doing. :warning:

Therefore I will just provide a simple schematic. The relay must be connected to the grinder and the load cell amplifier (here **HX711**) to the load cell.

<img src="/documentation/CoffeeGrinder.svg" width="400">

### Scale calibration

The scale must be calibrated. To retrieve the calibration factor, I used the sparkfun [calibration](https://github.com/sparkfun/HX711-Load-Cell-Amplifier/blob/master/firmware/SparkFun_HX711_Calibration/SparkFun_HX711_Calibration.ino) sample. I wanted grams, so I checked that this factor already respects that.
This retrieved calibration factor has to be set in the `program.cpp`.

### Control the grinder

Last but not least: You will need a way to publish the MQTT commands. There are several options for this:

- Command line (e.g. `mosquitto_pub -d -t grinder/in/automatic -m "15"`).
- Node Red Dash Board

<img src="/documentation/Node-RED%20Dashboard.png" width="400">

- ...