#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>

#include "clock.h"
#include "weather.h"
#include "special_chars.h"
#include "switch_input.h"

#define SWITCH_DEBOUNCE_MS 100

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x26, 16, 2);
BigNumbers_I2C big_num(&LCD);
SwitchInput displaySwitch(D5, D6, D7, D0);

tm last_printed_time{};
uint32_t last_printed_weather_time{};
uint32_t last_printed_clock_time{};
bool is_backlight_on {true};

void updateBacklight()
{
    // night mode from 2:00 to 5:00 am
    if (time_info.tm_hour >= 2 && time_info.tm_hour < 5)
    {
        if (is_backlight_on)
        {
            LCD.noBacklight();
            is_backlight_on = false;
        }
    }
    else 
    {
        if (!is_backlight_on)
        {
            LCD.backlight();
            is_backlight_on = true;
        }
    }
}

void printTimeLCD(tm new_time, bool force_print = false)
{
    // only update every second or when explicitly forced
    if (((new_time.tm_sec != last_printed_time.tm_sec) 
          && (millis() - last_printed_clock_time > 1000)) 
          || force_print == true) 
    {
        // update date only if day has changed
        if (new_time.tm_mday != last_printed_time.tm_mday
            || new_time.tm_mon != last_printed_time.tm_mon
            || new_time.tm_year != last_printed_time.tm_year
            || force_print == true) 
            {
                LCD.setCursor(0, 0);
                if (new_time.tm_mday < 10) LCD.print('0');
                LCD.print(new_time.tm_mday);
                LCD.print('/');
                if ((new_time.tm_mon + 1) < 10) LCD.print('0');
                LCD.print(new_time.tm_mon + 1);
                LCD.print('/');
                LCD.print(new_time.tm_year - 100);
                LCD.print("     "); // clear remain if year shrinks
            }

        // print hours
        LCD.setCursor(0, 1);
        if (new_time.tm_hour < 10) 
        {
            LCD.print('0');
        }
        LCD.print(new_time.tm_hour);

        LCD.print(':');

        // print minutes
        if (new_time.tm_min < 10)
        {
            LCD.print('0');
        }
        LCD.print(new_time.tm_min);

        LCD.print(':');

        // print seconds
        if (new_time.tm_sec < 10)
        {
            LCD.print('0');
        }
        LCD.print(new_time.tm_sec);
        LCD.print("   "); // pad out old leftovers if time string gets shorter

        last_printed_time = new_time;
        last_printed_clock_time = millis();
    }
}

void setupChars()
{
    LCD.createChar(LCD_SMALL_DROPLET, droplet_mid1);
    LCD.createChar(LCD_SMALL_THERMOMETER, thermometer);
    LCD.createChar(LCD_PACMAN_CL, pacmanClosed);
    LCD.createChar(LCD_PACMAN, pacmanOpen);
    big_num.begin();
}

void loadDropletIconSmall(float humidity, uint8_t slot = LCD_SMALL_DROPLET)
{
    byte* icon;

    uint8_t sw = (int)humidity / 16;
    switch (sw) 
    {
        case 0: icon = droplet_empty; break;
        case 1: icon = droplet_low;   break;
        case 2: icon = droplet_mid1;  break;
        case 3: icon = droplet_mid2;  break;
        case 4: icon = droplet_mid3;  break;
        case 5:
        case 6:
        default: icon = droplet_full; break;
    }
    LCD.createChar(slot, icon);
}

void printTemperatureLCD(bool force_print = false, bool set_chars = false)
{
    if ((millis() - last_printed_weather_time > weather.get_timer_sync_delay())
         || force_print == true)
    {
        if (set_chars) 
        { 
          setupChars(); 
        }

        LCD.clear();

        int temp_int = static_cast<int>(weather.get_temp());
        int temp_dec = static_cast<int>((temp_int - static_cast<int>(weather.get_temp()) * 10));

        big_num.displayLargeInt(temp_int, 4, 0, 2, false);
        big_num.displayLargeInt(temp_dec, 11, 0, 1, false);

        LCD.setCursor(10, 1);
        LCD.print(static_cast<char>('.'));

        LCD.setCursor(0, 1);
        LCD.print("temp");
        LCD.setCursor(14, 1);
        LCD.print(static_cast<char>(223)); // degree symbol
        LCD.print("C");

        // reset last print time
        last_printed_weather_time = millis();
    }
}

void printHumidityLCD(bool force_print = false, bool set_chars = false)
{
    if ((millis() - last_printed_weather_time > weather.get_timer_sync_delay()) 
        || force_print == true)
    {
        if (set_chars) 
        {
          setupChars();
        }
        LCD.clear();
          
        int hum = static_cast<int>(weather.get_humidity());
        big_num.displayLargeInt(hum, 4, 0, 2, false);
          
        LCD.setCursor(0, 1);
        LCD.print("hum");
        //LCD.write((char)LCD_SMALL_DROPLET);
        LCD.setCursor(11, 1);
        LCD.print('%');
          
        // reset last print time
        last_printed_weather_time = millis();
    }
}

void printWeatherLCD(bool forcePrint = false, bool setChars = false)
{
    if ((millis() - last_printed_weather_time > weather.get_timer_sync_delay()) 
        || forcePrint == true)
    {
        if (setChars)
        {
            setupChars();
        }

        LCD.setCursor(9, 0);
        LCD.write(static_cast<char>(LCD_SMALL_THERMOMETER));
        LCD.print(static_cast<float>(weather.get_temp()), 1);
        LCD.print(static_cast<char>(223)); // degree symbol
        LCD.print('C');
    
        LCD.setCursor(12, 1);
        LCD.write(static_cast<char>(LCD_SMALL_DROPLET));
        LCD.print(weather.get_humidity());
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
    LCD.print("Setup done!");
    LCD.setCursor(0, 1);
    LCD.print("Starting...");
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

void lcdLoop()
{
    time_t rawtime;
    time(&rawtime);
    localtime_r(&rawtime, &time_info);
    updateBacklight();

    displaySwitch.update();

    if (displaySwitch.hasChanged()) 
    {
            LCD.clear();
            Serial.printf("[SWITCH] Raw: %d → Display Mode: %d\n", displaySwitch.get_raw_value(),
                                                                   displaySwitch.get_state());
    }
    switch (displaySwitch.get_state()) 
    {
        case SwitchInput::SHOW_TIME:
            printTimeLCD(time_info);
            break;
        case SwitchInput::SHOW_TEMP:
            printTemperatureLCD(true, true);
            break;
        case SwitchInput::SHOW_HUM:
            printHumidityLCD(true, true);
            break;
        case SwitchInput::SHOW_ALL:
            printTimeLCD(time_info, true);
            delay(1);
            printWeatherLCD(true, true);
            break;
    }
    delay(500); // avoid flicker
}