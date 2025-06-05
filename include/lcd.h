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
BigNumbers_I2C big_num(&LCD);
tm last_printed_time;

uint32_t last_printed_weather_time = 0;
uint32_t last_printed_clock_time = 0;
uint32_t timerSyncDelay_w = 60000; // 1 min
byte termometer_br[8];

bool is_backlight_on = true;

void updateBacklight() {
    // night mode from 2:00 to 5:00 am
    if (time_info.tm_hour >= 2 && time_info.tm_hour < 5) {
        if (is_backlight_on) {
            LCD.noBacklight();
            is_backlight_on = false;
        }
    } else {
        if (!is_backlight_on) {
            LCD.backlight();
            is_backlight_on = true;
        }
    }
}


void printTimeLCD(tm new_time, bool force_print = false)
{
    LCD.clear();
  
    if (((new_time.tm_sec != last_printed_time.tm_sec) && (millis() - last_printed_clock_time > 1000)) || force_print == true) {
        if ((new_time.tm_min != last_printed_time.tm_min) || (new_time.tm_hour != last_printed_time.tm_hour)) {
            LCD.clear();
        }

        LCD.setCursor(0, 0);
        LCD.print(new_time.tm_mday);
        LCD.print('/');
        if ((new_time.tm_mon + 1) < 10) LCD.print('0');
        LCD.print((new_time.tm_mon + 1));
        LCD.print('/');
        LCD.print(new_time.tm_year - 100);

        LCD.setCursor(0, 1);
        if (new_time.tm_hour < 10) LCD.print('0');
        LCD.print(new_time.tm_hour);
        LCD.print(':');
        if (new_time.tm_min < 10) LCD.print('0');
        LCD.print(new_time.tm_min);
        LCD.print(':');
        if (new_time.tm_sec < 10) LCD.print('0');
        LCD.print(new_time.tm_sec);

        last_printed_time = new_time;
        last_printed_clock_time = millis();
    }
}


void setupChars()
{
    LCD.createChar(LCD_PACMAN_CL, pacmanClosed);
    LCD.createChar(LCD_PACMAN, pacmanOpen);
    LCD.createChar(LCD_DOT, dot);
    LCD.createChar(LCD_SMALL_THERMOMETER, thermometer);
    big_num.begin();
}

ICONS getDropletIcon2x2(float humidity)
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
            return LCD_DROPLET_FULL;      // 80-100%
        default: 
            return LCD_DROPLET_EMPTY;
    };
}

void loadDropletIconSmall(float humidity, uint8_t slot = LCD_SMALL_DROPLET) {
    byte* icon;

    uint8_t sw = (int)humidity / 16;
    switch (sw) {
        case 0: icon = droplet_empty; break;
        case 1: icon = droplet_low; break;
        case 2: icon = droplet_mid1; break;
        case 3: icon = droplet_mid2; break;
        case 4: icon = droplet_mid3; break;
        case 5:
        case 6:
        default: icon = droplet_full; break;
    }

    LCD.createChar(slot, icon);
}

void loadIconToLCD(Icon2x2 &icon, 
                  uint8_t tl_slot, uint8_t tr_slot, 
                  uint8_t bl_slot, uint8_t br_slot)
{
    LCD.createChar(tl_slot, icon.tl);
    LCD.createChar(tr_slot, icon.tr);
    LCD.createChar(bl_slot, icon.bl);
    LCD.createChar(br_slot, icon.br);
}

void printTemperatureLCD(bool force_print = false, bool set_chars = false)
{
    if ((millis() - last_printed_weather_time > timerSyncDelay_w) || force_print == true)
    {
        if (set_chars) { setupChars(); }

        // ----------------------------
        // DISPLAY TEMPERATURE
        // ----------------------------
        LCD.clear();

        // loadIconToLCD(icons[LCD_THERMOMETER], LCD_TEMP_TL, LCD_TEMP_TR, LCD_TEMP_BL, LCD_TEMP_BR);

        // LCD.setCursor(0, 0);
        // LCD.write((char)LCD_TEMP_TL);
        // LCD.write((char)LCD_TEMP_TR);
 
        // LCD.setCursor(0, 1);
        // LCD.write((char)LCD_TEMP_BL);
        // LCD.write((char)LCD_TEMP_BR);

        int temp_int = (int)Weather.temp;
        int temp_dec = (int)((Weather.temp - (int)Weather.temp) * 10);

        big_num.displayLargeInt(temp_int, 4, 0, 2, false);   // 4-9
        big_num.displayLargeInt(temp_dec, 11, 0, 1, false);  // 11-13

        LCD.setCursor(10, 1);
        LCD.print((char)'.');

        LCD.setCursor(1, 1);
        LCD.write((char)LCD_SMALL_THERMOMETER);
        LCD.setCursor(14, 1);
        LCD.print((char)223); // degree symbol
        LCD.print("C");

        // reset last print time
        last_printed_weather_time = millis();
    }
}

void printHumidityLCD(bool force_print = false, bool set_chars = false)
{
    if ((millis() - last_printed_weather_time > timerSyncDelay_w) || force_print == true)
    {
        if (set_chars) { setupChars(); }
    // ----------------------------
    // DISPLAY HUMIDITY
    // ----------------------------
    LCD.clear();
    
    // ICONS humidity_icon = getDropletIcon2x2(Weather.humidity);
    // loadIconToLCD(icons[humidity_icon], LCD_DROPLET_TL, LCD_DROPLET_TR, LCD_DROPLET_BL, LCD_DROPLET_BR);

    // LCD.setCursor(0, 0);
    // LCD.write((char)LCD_DROPLET_TL);
    // LCD.write((char)LCD_DROPLET_TR);
    // LCD.setCursor(0, 1);
    // LCD.write((char)LCD_DROPLET_BL);
    // LCD.write((char)LCD_DROPLET_BR);

    loadDropletIconSmall(Weather.humidity, LCD_SMALL_DROPLET);

    int hum = (int)Weather.humidity;
    big_num.displayLargeInt(hum, 4, 0, 2, false);

    LCD.setCursor(1, 1);
    LCD.write((char)LCD_SMALL_DROPLET);
    LCD.setCursor(11, 1);
    LCD.print('%');

    // reset last print time
    last_printed_weather_time = millis();
    }
}

void printWeatherLCD(bool forcePrint = false, bool setChars = false)
{
    if ((millis() - last_printed_weather_time > timerSyncDelay_w) || forcePrint == true)
    {
        if (setChars)
        {
            setupChars();
        }

        loadDropletIconSmall(Weather.humidity, LCD_SMALL_DROPLET);

        LCD.setCursor(9, 0);
        LCD.write((char)LCD_THERMOMETER);
        LCD.print((float)Weather.temp, 1);
        LCD.print((char)223);
        LCD.print('C');
    
        LCD.setCursor(12, 1);
        LCD.write((char)LCD_SMALL_DROPLET);
        LCD.print(Weather.humidity);
        LCD.print('%');
    }
}

void lcdSetupOne()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    LCD.init();
    LCD.backlight();
    big_num.begin();
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
    last_printed_clock_time = millis();
    printTimeLCD(time_info, true);
    last_printed_weather_time = millis();
    printTemperatureLCD(true, true);
    printHumidityLCD(true, true);
    digitalWrite(LED_BUILTIN, HIGH);
}

enum DisplayState {
    SHOW_TIME,
    SHOW_TEMP,
    SHOW_HUM,
    SHOW_ALL
};

DisplayState current_display = SHOW_TIME;
uint32_t display_start{};

void lcdLoop()
{
    time_t rawtime;
    time(&rawtime);
    localtime_r(&rawtime, &time_info);
    updateBacklight();
    uint32_t now_millis = millis();
    if (now_millis - display_start >= 10000) {
        display_start = now_millis;

        switch (current_display) {
            case SHOW_TIME:
                printTimeLCD(time_info, true);
                current_display = SHOW_TEMP;
                break;
            case SHOW_TEMP:
                printTemperatureLCD(true, true);
                current_display = SHOW_HUM;
                break;
            case SHOW_HUM:
                printHumidityLCD(true, true);
                current_display = SHOW_ALL;
                break;
            case SHOW_ALL:
                printTimeLCD(time_info);
                printWeatherLCD();
                current_display = SHOW_TIME;
                break;
        }
    }

    delay(1000); // avoid flicker
}


#endif