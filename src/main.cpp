#include <Arduino.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "wifiConf.h"
#include "led_sts.h"
#include "lcd.h"
#include "clock.h"
#include "weather.h"

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print("Starting; Compiled on ");
  Serial.println(__DATE__);
  Serial.println(__TIME__);
  Serial.println(__FILE__);  //__BASE_FILE__

  ledStsSetup();
  lcdSetupOne();
  LittleFS.begin();
  listFiles("/");
  setupWifi(&LCD);
  if (isWiFiConnected) {
    lcdSetupTwo();
    weatherSetup();
    Serial.println("Weater setup done");
    setupNTPClock();
    Serial.println("NTP setup done");
    Serial.println("LCD setup done");
    Serial.println("Setup done");
  }
}

void loop() {
  delay(1);
  handleDnsLoop();
  getWeatherLoop(timerSyncDelay_w);
  getNTPtime(timerClockDelay);
  lcdLoop();
}