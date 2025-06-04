/*#ifndef WIFI_CONF_H
#define WIFI_CONF_H

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <WiFiClient.h>
#include "lcd.h"
#include "special_chars.h"

const char* ssid = "SoftServe-Guest"; 
const char* password = "Welcome!"; 

int retry_count = 0;


void setupWifi(LiquidCrystal_I2C *lcd_p) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");


  for (int i = 0; i < 16; i++) {
    lcd_p->setCursor(i, 1);
    lcd_p->write((char)LCD_DOT);
    Serial.print(".");
  }

  uint8_t pos = 0;
  
  while (WiFi.status() != WL_CONNECTED) {
    lcd_p->setCursor(pos, 1);
    if(pos%2 == 0){
      lcd_p->write((char)LCD_PACMAN);
    }else{
      lcd_p->write((char)LCD_PACMAN_CL);
    }

    lcd_p->setCursor(pos-1, 1);
    lcd_p->print(" ");
    
    Serial.print("+");
    pos++;
    delay(500);


        for (int i = 1; i < 16; i++) 
        {
            if(WiFi.status() == WL_CONNECTED) break;

            lcd_p->setCursor(i, 1);
            lcd_p->print("\1");
        
                for (int j = i + 1; j < 16; j++) 
                {
                    lcd_p->setCursor(j, 1);
                    lcd_p->print("\2");
                   
                }
                
                lcd_p->createChar(1, pacmanClosed);
                delay(300);
                lcd_p->createChar(1, pacmanOpen);
                delay(300);
                lcd_p->setCursor(i, 1);
                lcd_p->print(" ");
        }

  

    if (++retry_count > 1000) ESP.restart();
  }

  Serial.println();
  Serial.println("\n\nWiFi connected\n\n");
}

#endif
*/

#ifndef WIFI_CONF_H
#define WIFI_CONF_H

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <LittleFS.h>
#include <AsyncFsWebServer.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include "lcd.h"
#include "special_chars.h"

AsyncFsWebServer server(80, LittleFS, "WeatherClock");
DNSServer dnsServer;

const char* CONFIG_FILE = "/setup/wifi_conf.json";
bool isWiFiConnected = false;

int8_t retries = 0;
bool captiveModeActive = false;

bool loadCredentials(String& ssid, String& password) {
    File configFile = LittleFS.open(CONFIG_FILE, "r");
    if (!configFile) return false;

    DynamicJsonDocument doc(256);
    DeserializationError err = deserializeJson(doc, configFile);
    if (err || !doc.containsKey("ssid") || !doc.containsKey("password")) return false;

    ssid = doc["ssid"].as<String>();
    password = doc["password"].as<String>();
    return true;
}

void saveCredentials(const String& ssid, const String& password) {
    DynamicJsonDocument doc(256);
    doc["ssid"] = ssid;
    doc["password"] = password;

    File configFile = LittleFS.open(CONFIG_FILE, "w");
    if (configFile) {
        serializeJson(doc, configFile);
        configFile.close();
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

    dnsServer.start(53, "*", IPAddress(192,168,4,1));

    server.serveStatic("/", LittleFS, "/setup").setDefaultFile("index.html");

    server.on("/save_config", HTTP_POST, handleSaveConfig);
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->redirect("/");
    });

    server.begin();

    Serial.println("[WiFi] Captive portal started");
    captiveModeActive = true;

    if (!isWiFiConnected) {
        LCD.setCursor(0, 1);
        LCD.print("Open 192.168.4.1");
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

    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        isWiFiConnected = true;
        captiveModeActive = false;
    } else {
        Serial.println("[WiFi] Connection failed. Starting AP...");
        LCD.setCursor(0, 0);
        LCD.print("WiFi failed. AP...");
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
    if (captiveModeActive) {
        dnsServer.processNextRequest();
    }
}


void listFiles(const char *dirname = "/", uint8_t levels = 0) {
    Serial.printf("Listing files in: %s\n", dirname);
    Dir dir = LittleFS.openDir(dirname);

    while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        Serial.printf("FILE: %s, size: %d\n", fileName.c_str(), fileSize);
    }
}

#endif
