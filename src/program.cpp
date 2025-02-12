#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Constants.hpp>
#include <GrindingController.hpp>
#include <MqttGrinder.hpp>

GrindingController grinder;

/**
 * @brief Connect to WiFi using WiFiManager
 *
 * Typically, it will automatically reconnect to the previously configured wifi.
 * In case it can't connect, it will start an access point with the given
 * ssidAP. The user can then connect to this access point and configure the
 * wifi.
 *
 * If the user doesn't connect to the access point within the given time, the
 * ESP will restart. Possibly, the wifi was not available at the moment.
 *
 */
void connectToWifi() {
  WiFiManager wifiManager;
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setConfigPortalTimeout(configPortalTimeout);
  wifiManager.setConfigPortalTimeoutCallback([] { ESP.restart(); });
  wifiManager.autoConnect(ssidAP, passwordAP);
}

/**
 * @brief Setup OTA
 *
 * Setup OTA with the given hostname. The hostname is used to identify the ESP
 * in the network.
 */
void setupOTA() {
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
}

void setup() {
  Serial.begin(115200);

  connectToWifi();

  setupOTA();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  grinder.setup();
}

void loop() {
  grinder.loop();
  // Since the grinder will be blocking, while grinding, OTA cannot happen while
  // the grinder is grinding.
  ArduinoOTA.handle();
}
