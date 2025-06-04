#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>
#include "clock.h"
#include "weather.h"
#include "special_chars.h"

uint8_t LCD_TEMP_TL = 0;
uint8_t LCD_TEMP_TR = 1;
uint8_t LCD_TEMP_BL = 2;
uint8_t LCD_TEMP_BR = 3;
uint8_t LCD_DROPLET_TL = 4;
uint8_t LCD_DROPLET_TR = 5;
uint8_t LCD_DROPLET_BL = 6;
uint8_t LCD_DROPLET_BR = 7;

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x26, 16, 2);
BigNumbers_I2C bigNum(&LCD);
tm lastPrintedTime;

uint32_t lastPrintedWeatherTime = 0;
uint32_t lastPrintedClockTime = 0;
byte termometer_br[8];

bool isBacklightOn = true;

void updateBacklight() {
  // Night mode from 2:00 to 5:00
  if (timeinfo.tm_hour >= 2 && timeinfo.tm_hour < 5) {
    if (isBacklightOn) {
      LCD.noBacklight();
      isBacklightOn = false;
    }
  } else {
    if (!isBacklightOn) {
      LCD.backlight();
      isBacklightOn = true;
    }
  }
}


void printTimeLCD(tm newTime, bool forcePrint = false)
{
  LCD.clear();
  
  if (((newTime.tm_sec != lastPrintedTime.tm_sec) && (millis() - lastPrintedClockTime > 1000)) || forcePrint == true) {
    if ((newTime.tm_min != lastPrintedTime.tm_min) || (newTime.tm_hour != lastPrintedTime.tm_hour)) {
      LCD.clear();
    }

    LCD.setCursor(0, 0);
    LCD.print(newTime.tm_mday);
    LCD.print('/');
    if ((newTime.tm_mon + 1) < 10) LCD.print('0');
    LCD.print((newTime.tm_mon + 1));
    LCD.print('/');
    LCD.print(newTime.tm_year - 100);

    LCD.setCursor(0, 1);
    if (newTime.tm_hour < 10) LCD.print('0');
    LCD.print(newTime.tm_hour);
    LCD.print(':');
    if (newTime.tm_min < 10) LCD.print('0');
    LCD.print(newTime.tm_min);
    LCD.print(':');
    if (newTime.tm_sec < 10) LCD.print('0');
    LCD.print(newTime.tm_sec);

    lastPrintedTime = newTime;
    lastPrintedClockTime = millis();
  }
}


void setupChars()
{
    LCD.createChar(LCD_PACMAN_CL, pacmanClosed);
    LCD.createChar(LCD_PACMAN, pacmanOpen);
    LCD.createChar(LCD_DOT, dot);
    LCD.createChar(LCD_SMALL_THERMOMETER, thermometer);
    LCD.createChar(LCD_SMALL_DROPLET, droplet_empty);
    bigNum.begin();
}

ICONS getDropletIcon(float humidity)
{
    uint8_t sw = (int)humidity / 16;
    switch (sw)
    {
    case 0: return LCD_DROPLET_EMPTY; //  0-15%
    case 1: return LCD_DROPLET_LOW;   // 16-31%
    case 2: return LCD_DROPLET_MID1;  // 32-47%
    case 3: return LCD_DROPLET_MID2;  // 48-63%
    case 4: return LCD_DROPLET_MID3;  // 64-79%
    case 5:
    case 6:
      return LCD_DROPLET_FULL;  // 80-100%
    default: return LCD_DROPLET_EMPTY;
    };
}

void loadIconToLCD(Icon2x2 &icon, 
                  uint8_t tlSlot, uint8_t trSlot, 
                  uint8_t blSlot, uint8_t brSlot)
{
    LCD.createChar(tlSlot, icon.tl);
    LCD.createChar(trSlot, icon.tr);
    LCD.createChar(blSlot, icon.bl);
    LCD.createChar(brSlot, icon.br);
}

void printTemperatureLCD(bool forcePrint = false, bool setChars = false)
{
    if ((millis() - lastPrintedWeatherTime > timerSyncDelay_w) || forcePrint == true)
    {
        if (setChars) { setupChars(); }

        // ----------------------------
        // DISPLAY TEMPERATURE
        // ----------------------------
        LCD.clear();

        loadIconToLCD(icons[LCD_THERMOMETER], LCD_TEMP_TL, LCD_TEMP_TR, LCD_TEMP_BL, LCD_TEMP_BR);

        LCD.setCursor(0, 0);
        LCD.write((char)LCD_TEMP_TL);
        LCD.write((char)LCD_TEMP_TR);

        LCD.setCursor(0, 1);
        LCD.write((char)LCD_TEMP_BL);
        LCD.write((char)LCD_TEMP_BR);

        int temp_int = (int)Weather.temp;
        int temp_dec = (int)((Weather.temp - (int)Weather.temp) * 10);

        bigNum.displayLargeInt(temp_int, 4, 0, 2, false);   // 4-9
        bigNum.displayLargeInt(temp_dec, 11, 0, 1, false);  // 11-13

        LCD.setCursor(10, 1);
        LCD.print((char)'.');

        LCD.setCursor(14, 1);
        LCD.print((char)223); // degree symbol
        LCD.print("C");

        // reset last print time
        lastPrintedWeatherTime = millis();
    }
}

void printHumidityLCD(bool forcePrint = false, bool setChars = false)
{
    if ((millis() - lastPrintedWeatherTime > timerSyncDelay_w) || forcePrint == true)
    {
        if (setChars) { setupChars(); }
    // ----------------------------
    // DISPLAY HUMIDITY
    // ----------------------------
    LCD.clear();
    
    ICONS humidityIcon = getDropletIcon(Weather.humidity);
    loadIconToLCD(icons[humidityIcon], LCD_DROPLET_TL, LCD_DROPLET_TR, LCD_DROPLET_BL, LCD_DROPLET_BR);

    LCD.setCursor(0, 0);
    LCD.write((char)LCD_DROPLET_TL);
    LCD.write((char)LCD_DROPLET_TR);
    LCD.setCursor(0, 1);
    LCD.write((char)LCD_DROPLET_BL);
    LCD.write((char)LCD_DROPLET_BR);

    int hum = (int)Weather.humidity;
    bigNum.displayLargeInt(hum, 4, 0, 2, false);

    LCD.setCursor(11, 1);
    LCD.print('%');

    // reset last print time
    lastPrintedWeatherTime = millis();
  }
}


void lcdSetupOne()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  LCD.init();
  LCD.backlight();
  bigNum.begin();
  setupChars();

  LCD.setCursor(0, 0);
  LCD.print("WIFI Connecting");
}

void lcdSetupTwo()
{
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Setup done");
  LCD.setCursor(0, 1);
  LCD.print("Starting");
  delay(3000);
  LCD.clear();
  LCD.setCursor(0, 0);
  lastPrintedClockTime = millis();
  printTimeLCD(timeinfo, true);
  lastPrintedWeatherTime = millis();
  printTemperatureLCD(true, true);
  printHumidityLCD(true, true);
  digitalWrite(LED_BUILTIN, HIGH);
}

enum DisplayState {
  SHOW_TIME,
  SHOW_TEMP,
  SHOW_HUM
};

DisplayState currentDisplay = SHOW_TIME;
uint32_t displayStart = 0;

void lcdLoop()
{
  // Always keep backlight updated
  updateBacklight();
  uint32_t now = millis();

  if (now - displayStart >= 10000) {
    displayStart = now;

    // UPDATE timeinfo before using it
    time_t rawtime;
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);

    switch (currentDisplay) {
      case SHOW_TIME:
        printTimeLCD(timeinfo, true);
        currentDisplay = SHOW_TEMP;
        break;
      case SHOW_TEMP:
        printTemperatureLCD(true, true);
        currentDisplay = SHOW_HUM;
        break;
      case SHOW_HUM:
        printHumidityLCD(true, true);
        currentDisplay = SHOW_TIME;
        break;
    }
  }

  delay(1000); // avoid flicker
}


#endif