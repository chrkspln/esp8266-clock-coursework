#pragma once

enum CUSTOM_CHARS
{
    LCD_SMALL_DROPLET = 0,
    LCD_SMALL_THERMOMETER,
    LCD_PACMAN_CL,
    LCD_PACMAN
};

enum ICONS
{
    LCD_THERMOMETER = 0,
    LCD_DROPLET_EMPTY,
    LCD_DROPLET_LOW,
    LCD_DROPLET_MID1,
    LCD_DROPLET_MID2,
    LCD_DROPLET_MID3,
    LCD_DROPLET_FULL
};

struct Icon2x2
{
    byte* tl;
    byte* tr;
    byte* bl;
    byte* br;
};

uint8_t pacmanClosed[8] = {
    0b00000,
    0b00000,
    0b01110,
    0b11011,
    0b11111,
    0b01110,
    0b00000,
    0b00000
};

uint8_t pacmanOpen[] = {
    0b00000,
    0b00000,
    0b01110,
    0b11011,
    0b11100,
    0b01110,
    0b00000,
    0b00000
};

byte thermometer[8] = //icon for thermometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte thermometer_tl[8] =
{
    B00001,
    B00001,
    B00110,
    B00110,
    B00110,
    B00110,
    B00111,
    B00111
};

byte thermometer_tr[8] =
{
    B10000,
    B10000,
    B01100,
    B01100,
    B01100,
    B01100,
    B11100,
    B11100
};

byte thermometer_bl[8] =
{
    B00111,
    B00111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00111
};

byte thermometer_br[8] =
{
    B11100,
    B11100,
    B11111,
    B11111,
    B11111,
    B11111,
    B11100,
    B11100
};

byte droplet_empty[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

byte droplet_empty_tl[8] =
{
    B00001,
    B00001,
    B00111,
    B00111,
    B00110,
    B00110,
    B00110,
    B00110
};

byte droplet_empty_tr[8] =
{
    B10000,
    B10000,
    B11100,
    B11100,
    B01100,
    B01100,
    B01100,
    B01100
};

byte droplet_empty_bl[8] =
{
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B00111,
    B00111
};

byte droplet_empty_br[8] =
{
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B11100,
    B11100
};

byte droplet_low[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B11111,
    B01110,
};

byte droplet_low_tl[8] =
{
    B00001,
    B00001,
    B00111,
    B00111,
    B00110,
    B00110,
    B00110,
    B00110
};

byte droplet_low_tr[8] =
{
    B10000,
    B10000,
    B11100,
    B11100,
    B01100,
    B01100,
    B01100,
    B01100
};

byte droplet_low_bl[8] =
{
    B11000,
    B11000,
    B11000,
    B11000,
    B11111,
    B11111,
    B00111,
    B00111
};

byte droplet_low_br[8] =
{
    B00011,
    B00011,
    B00011,
    B00011,
    B11111,
    B11111,
    B11100,
    B11100
};

byte droplet_mid1[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B11111,
    B11111,
    B01110,
};

byte droplet_mid1_tl[8] =
{
    B00001,
    B00001,
    B00111,
    B00111,
    B00110,
    B00110,
    B00110,
    B00110
};

byte droplet_mid1_tr[8] =
{
    B10000,
    B10000,
    B11100,
    B11100,
    B01100,
    B01100,
    B01100,
    B01100
};

byte droplet_mid1_bl[8] =
{
    B11000,
    B11000,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00111
};

byte droplet_mid1_br[8] =
{
    B00011,
    B00011,
    B11111,
    B11111,
    B11111,
    B11111,
    B11100,
    B11100
};


byte droplet_mid2[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B11111,
    B11111,
    B11111,
    B01110,
};

byte droplet_mid2_tl[8] =
{
    B00001,
    B00001,
    B00001,
    B00011,
    B00110,
    B00110,
    B01110,
    B01110
};

byte droplet_mid2_tr[8] =
{
    B10000,
    B10000,
    B10000,
    B11000,
    B01100,
    B01100,
    B01110,
    B01110
};

byte droplet_mid2_bl[8] =
{
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00111
};

byte droplet_mid2_br[8] =
{
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11100,
    B11100
};

byte droplet_mid3[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
};

byte droplet_mid3_tl[8] =
{
    B00001,
    B00001,
    B00001,
    B00011,
    B00110,
    B00110,
    B00111,
    B00111
};

byte droplet_mid3_tr[8] =
{
    B10000,
    B10000,
    B10000,
    B11000,
    B01100,
    B01100,
    B11100,
    B11100
};

byte droplet_mid3_bl[8] =
{
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00111
};

byte droplet_mid3_br[8] =
{
    B11110,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11100,
    B11100
};

byte droplet_full[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
};

byte droplet_full_tl[8] =
{
    B00001,
    B00001,
    B00001,
    B00011,
    B00011,
    B00011,
    B00111,
    B00111
};

byte droplet_full_tr[8] =
{
    B10000,
    B10000,
    B10000,
    B11000,
    B11000,
    B11000,
    B11100,
    B11100
};

byte droplet_full_bl[8] =
{
    B01111,
    B01111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00111,
    B00111
};

byte droplet_full_br[8] =
{
    B11110,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11100,
    B11100
};

Icon2x2 icons[] =
{
    { thermometer_tl, thermometer_tr, thermometer_bl, thermometer_br },
    { droplet_empty_tl, droplet_empty_tr, droplet_empty_bl, droplet_empty_br },
    { droplet_low_tl, droplet_low_tr, droplet_low_bl, droplet_low_br },
    { droplet_mid1_tl, droplet_mid1_tr, droplet_mid1_bl, droplet_mid1_br },
    { droplet_mid2_tl, droplet_mid2_tr, droplet_mid2_bl, droplet_mid2_br },
    { droplet_mid3_tl, droplet_mid3_tr, droplet_mid3_bl, droplet_mid3_br },
    { droplet_full_tl, droplet_full_tr, droplet_full_bl, droplet_full_br }
};