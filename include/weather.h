#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifi_conf.h"

// set location and API key
String location = "Lviv,UA";
String api_key = "8cd7834a6b2b8a74a77b47b7feb76237";

uint32_t last_sync_time_w = 0;
const uint32_t timer_sync_delay_w = 60000;

struct Weather
{
    float temp = 0.0;
    int16_t humidity = 0;
    float pressure = 0.0;
    float wind_speed = 0.0;
    int16_t wind_degree = 0;
} Weather;

void printWeatherCurrent()
{
    // print data
    Serial.printf("Temperature = %.2f°C\r\n", Weather.temp);
    Serial.printf("Humidity = %d %%\r\n", Weather.humidity);
    Serial.printf("Pressure = %.3f bar\r\n", Weather.pressure);
    Serial.printf("Wind speed = %.1f m/s\r\n", Weather.wind_speed);
    Serial.printf("Wind degree = %d°\r\n\r\n", Weather.wind_degree);
}

void setWeatherData(const JsonDocument& doc)
{
    Weather.temp = (float)(doc["main"]["temp"]) - 273.15;       // get temperature in °C
    Weather.humidity = (int16_t)doc["main"]["humidity"];        // get humidity in %
    Weather.pressure = (float)(doc["main"]["pressure"]) / 1000; // get pressure in bar
    Weather.wind_speed = doc["wind"]["speed"];                  // get wind speed in m/s
    Weather.wind_degree = doc["wind"]["deg"];                   // get wind degree in °
}

void httpGETRequest(const char *server_name)
{
    WiFiClient client;
    HTTPClient http;

    http.useHTTP10(true);
    http.begin(client, server_name);

    int http_response_code = http.GET();
    if (http_response_code > 0)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, http.getStream());
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        setWeatherData(doc);
        printWeatherCurrent();
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(http_response_code);
    }
    http.end();
}

void weatherSetup()
{
    last_sync_time_w = millis();
    String server_path = "http://api.openweathermap.org/data/2.5/weather?q=" + location + "&APPID=" + api_key;
    httpGETRequest(server_path.c_str());
}

void getWeatherLoop(uint32_t time_delay)
{
    
    if (millis() - last_sync_time_w > time_delay)
    {
        if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
        {
            String server_path = "http://api.openweathermap.org/data/2.5/weather?q=" + location + "&APPID=" + api_key;
            // http://api.openweathermap.org/data/2.5/weather?q=Lviv,UA&APPID=8cd7834a6b2b8a74a77b47b7feb76237"
            Serial.println(server_path.c_str());
            httpGETRequest(server_path.c_str());
        }
        last_sync_time_w = millis();
    }
}

#endif