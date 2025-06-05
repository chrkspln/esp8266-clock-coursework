#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LittleFS.h>
#include <AsyncFsWebServer.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include "lcd.h"
#include "special_chars.h"

AsyncFsWebServer web_server(80, LittleFS, "WeatherClock");
DNSServer dns_server;

const char* CONFIG_FILE = "/setup/wifi_conf.json";
bool is_wifi_connected = false;
int8 retries{};
bool captive_mode_active = false;

bool loadCredentials(String& ssid, String& password) {
    File config_file = LittleFS.open(CONFIG_FILE, "r");
    if (!config_file) return false;

    DynamicJsonDocument doc(256);
    DeserializationError err = deserializeJson(doc, config_file);
    if (err || !doc.containsKey("ssid") || !doc.containsKey("password")) return false;

    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
    return true;
}

void saveCredentials(const String& ssid, const String& password) {
    DynamicJsonDocument doc(256);
    doc["ssid"] = ssid;
    doc["password"] = password;

    File config_file = LittleFS.open(CONFIG_FILE, "w");
    if (config_file) {
        serializeJson(doc, config_file);
        config_file.close();
    }
}

void handleSaveConfig(AsyncWebServerRequest *request) {
    if (!request->hasParam("ssid", true) || !request->hasParam("password", true)) {
        request->send(400, "text/plain", "Missing ssid or password");
        return;
    }

    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
    saveCredentials(ssid, password);

    request->send(200, "text/html", "<h2>Saved! Rebooting...</h2>");
    delay(1500);
    ESP.restart();
}

void startCaptivePortal() {
    Serial.println("[WiFi] Starting AP Mode...");

    WiFi.disconnect();
    delay(100);

    WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
    WiFi.softAP("ClockAP", "setclock");
    delay(100);

    dns_server.start(53, "*", IPAddress(192,168,4,1));

    web_server.serveStatic("/", LittleFS, "/setup").setDefaultFile("index.html");

    web_server.on("/save_config", HTTP_POST, handleSaveConfig);
    web_server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    web_server.begin();

    Serial.println("[WiFi] Captive portal started");
    captive_mode_active = true;

    if (!is_wifi_connected) {
        LCD.setCursor(0, 1);
        LCD.print("Open 192.168.4.1");
    }
}

void showWifiPacmanAnim(LiquidCrystal_I2C *lcd_p, unsigned long duration_ms = 1000) {
  const unsigned long start_time = millis();
  uint8_t pos = 0;

  lcd_p->clear();
  lcd_p->setCursor(0, 0);
  lcd_p->print("WIFI Connecting");

  while (WiFi.status() != WL_CONNECTED && millis() - start_time < duration_ms) {
    // Animate pacman chomping dots
    for (int i = 0; i < 16 && WiFi.status() != WL_CONNECTED; ++i) {
      lcd_p->setCursor(i, 1);
      lcd_p->print((char)'.');
      delay(50);
    }

    // Pacman run
    for (int i = 0; i < 16 && WiFi.status() != WL_CONNECTED; ++i) {
      lcd_p->setCursor(i, 1);
      lcd_p->write((i % 2 == 0) ? (char)LCD_PACMAN : (char)LCD_PACMAN_CL);
      if (i > 0) {
        lcd_p->setCursor(i - 1, 1);
        lcd_p->print(" ");
      }

      // Recreate char to animate bite
      lcd_p->createChar(LCD_PACMAN_CL, pacmanClosed);
      delay(200);
      lcd_p->createChar(LCD_PACMAN_CL, pacmanOpen);
      delay(200);
    }
  }
}

void connectToWiFi(LiquidCrystal_I2C *lcd_p) {
    String ssid, password;
    if (!loadCredentials(ssid, password)) {
        Serial.println("[WiFi] No valid config found.");
        startCaptivePortal();
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.printf("[WiFi] Connecting to %s...\n", ssid.c_str());

    showWifiPacmanAnim(lcd_p, 10000);

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        is_wifi_connected = true;
        captive_mode_active = false;
    } else {
        Serial.println("[WiFi] Connection failed. Starting AP...");
        LCD.setCursor(0, 0);
        LCD.print("No WiFi Connect");
        startCaptivePortal();
    }
}

void setupWifi(LiquidCrystal_I2C *lcd_p) {
    if (!LittleFS.begin()) {
        Serial.println("[FS] Mount failed. Formatting...");
        LittleFS.format();
        ESP.restart();
    }

    connectToWiFi(lcd_p);
}

void handleDnsLoop() {
    if (captive_mode_active) {
        dns_server.processNextRequest();
    }
}


void listFiles(const char *dirname = "/", uint8_t levels = 0) {
    Serial.printf("Listing files in: %s\n", dirname);
    Dir dir = LittleFS.openDir(dirname);

    while (dir.next()) {
        String file_name = dir.fileName();
        size_t file_size = dir.fileSize();
        Serial.printf("FILE: %s, size: %d\n", file_name.c_str(), file_size);
    }
}