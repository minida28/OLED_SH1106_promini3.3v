/*
  OzOLED.cpp - 0.96' I2C 128x64 OLED Driver Library
  2014 Copyright (c) OscarLiang.net  All right reserved.

  Author: Oscar Liang

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/

#include "OzOLED.h"
#include <Wire.h>
#include <avr/pgmspace.h>

#include "cp437_font.h"
#include "bigNumbers2.h"
#include "font8.h"
#include "font32.h"

// ================ by ADAFRUIT

// the memory buffer for the LCD

static uint8_t buffer[OLED_LCDHEIGHT * OLED_LCDWIDTH / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
#if (OLED_LCDHEIGHT * OLED_LCDHEIGHT > 96 * 16)
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
    0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
    0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
    0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
    0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
    0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
    0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
    0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
    0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
    0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
    0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
    0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (OLED_LCDHEIGHT == 64)
    0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
    0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
    0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
    0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
    0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
    0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
    0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
    0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
    0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
    0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
    0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
};

// ===== END OF ADAFRUIT

// 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters(glyphs)
// BasicFont is placed in code memory.

// This font be freely used without any restriction(It is placed in public domain)
const byte BasicFont[][8] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00},
    {0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, 0x00},
    {0x00, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, 0x00},
    {0x00, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00, 0x00},
    {0x00, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x00},
    {0x00, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00, 0x00},
    {0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00},
    {0x00, 0xA0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00},
    {0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00},
    {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00},
    {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00},
    {0x00, 0x62, 0x51, 0x49, 0x49, 0x46, 0x00, 0x00},
    {0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00},
    {0x00, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00},
    {0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00},
    {0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00},
    {0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00},
    {0x00, 0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0xAC, 0x6C, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00},
    {0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00},
    {0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00},
    {0x00, 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, 0x00},
    {0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x00},
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00},
    {0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00},
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x51, 0x72, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00},
    {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00},
    {0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00},
    {0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00},
    {0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
    {0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, 0x00},
    {0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00},
    {0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
    {0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00},
    {0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00},
    {0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00},
    {0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, 0x00},
    {0x00, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, 0x00},
    {0x00, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00, 0x00},
    {0x00, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00},
    {0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00},
    {0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00},
    {0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00},
    {0x00, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00},
    {0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x28, 0x00, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00},
    {0x00, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00},
    {0x00, 0x08, 0x7E, 0x09, 0x02, 0x00, 0x00, 0x00},
    {0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, 0x00, 0x00},
    {0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00},
    {0x00, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x80, 0x84, 0x7D, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, 0x00},
    {0x00, 0x7C, 0x08, 0x04, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00},
    {0x00, 0xFC, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00},
    {0x00, 0x18, 0x24, 0x24, 0xFC, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7C, 0x08, 0x04, 0x00, 0x00, 0x00},
    {0x00, 0x48, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00},
    {0x00, 0x04, 0x7F, 0x44, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, 0x00},
    {0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, 0x00},
    {0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00},
    {0x00, 0x1C, 0xA0, 0xA0, 0x7C, 0x00, 0x00, 0x00},
    {0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00},
    {0x00, 0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x02, 0x01, 0x01, 0x02, 0x01, 0x00, 0x00},
    {0x00, 0x02, 0x05, 0x05, 0x02, 0x00, 0x00, 0x00}};

// // Big numbers font, from 0 to 9 - 96 bytes each.
// const byte bigNumbers[][96] PROGMEM = {
//     {0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00}, // 0

//     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0,
//      0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
//      0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 1

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xC1, 0xC0, 0xC0, 0xC0,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
//      0x03, 0x03, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 2

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xC1, 0xC0, 0xC0, 0xC0,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x81, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
//      0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 3

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07,
//      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 4

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x81, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
//      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 5

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
//      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 6

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 7

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xE1,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
//      0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
//      0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 8

//     {0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
//      0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
//      0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07,
//      0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00}, // 9

//     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0x00,
//      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//      0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // :
// };

// ====================== LOW LEVEL =========================

/* ORIGINAL
  void OzOLED::sendCommand(byte command) {
  Wire.beginTransmission(OLED_ADDRESS); // begin transmitting
  Wire.write(OzOLED_COMMAND_MODE);//command mode 0x80
  Wire.write(command);
  Wire.endTransmission();    // stop transmitting
  }
*/

OzOLED OzOled; // Preinstantiate Objects

uint8_t mask = 0;

void OzOLED::sendCommand(byte command)
{
  // I2C
  uint8_t control = 0x80; // Co = 0, D/C = 0
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(control);
  Wire.write(command);
  Wire.endTransmission();
}

void OzOLED::sendData(byte data)
{

  Wire.beginTransmission(OLED_ADDRESS); // begin transmitting
  Wire.write(OzOLED_DATA_MODE);         //data mode 0x40
  Wire.write(data);
  Wire.endTransmission(); // stop transmitting
}

void OzOLED::printChar(char C, byte X, byte Y)
{

  if (X < 128)
    setCursorXY(X, Y);

  //Ignore unused ASCII characters. Modified the range to support multilingual characters.
  if (C < 32 || C > 127)
    C = '*'; //star - indicate characters that can't be displayed

  for (byte i = 0; i < 8; i++)
  {

    //read bytes from code memory
    sendData(pgm_read_byte(&BasicFont[C - 32][i])); //font array starts at 0, ASCII starts at 32. Hence the translation
  }
}

void OzOLED::printString(const char *String, byte X, byte Y, byte numChar)
{

  if (X < 128)
    setCursorXY(X, Y);

  byte count = 0;
  while (String[count] && count < numChar)
  {
    printChar(String[count++]);
  }
}

byte OzOLED::printNumber(long long_num, byte X, byte Y)
{

  if (X < 128)
    setCursorXY(X, Y);

  byte char_buffer[10] = "";
  byte i = 0;
  byte f = 0; // number of characters

  if (long_num < 0)
  {

    f++;
    printChar('-');
    long_num = -long_num;
  }
  else if (long_num == 0)
  {

    f++;
    printChar('0');
    return f;
  }

  while (long_num > 0)
  {

    char_buffer[i++] = long_num % 10;
    long_num /= 10;
  }

  f += i;
  for (; i > 0; i--)
  {

    printChar('0' + char_buffer[i - 1]);
  }

  return f;
}

byte OzOLED::printNumber(float float_num, byte prec, byte X, byte Y)
{

  if (X < 128)
    setCursorXY(X, Y);

  // prec - 6 maximum

  byte num_int = 0;
  byte num_frac = 0;
  byte num_extra = 0;

  long d = float_num;      // get the integer part
  float f = float_num - d; // get the fractional part

  if (d == 0 && f < 0.0)
  {

    printChar('-');
    num_extra++;
    printChar('0');
    num_extra++;
    f *= -1;
  }
  else if (d < 0 && f < 0.0)
  {

    num_int = printNumber(d); // count how many digits in integer part
    f *= -1;
  }
  else
  {

    num_int = printNumber(d); // count how many digits in integer part
  }

  // only when fractional part > 0, we show decimal point
  if (f > 0.0)
  {

    printChar('.');
    num_extra++;

    long f_shift = 1;

    if (num_int + prec > 8)
      prec = 8 - num_int;

    for (byte j = 0; j < prec; j++)
    {
      f_shift *= 10;
    }

    num_frac = printNumber((long)(f * f_shift)); // count how many digits in fractional part
  }

  return num_int + num_frac + num_extra;
}

//** ORIGINAL **//
// void OzOLED::printBigNumber(const char *number, byte X, byte Y, byte numChar) {
//   // big number pixels: 24 x 32

//   // Y - page
//   byte column = 0;
//   byte count = 0;

//   while (number[count] && count < numChar) {

//     setCursorXY(X, Y);

//     for (byte i = 0; i < 96; i++) {

//       // if character is not "0-9" or ':'
//       if (number[count] < 48 || number[count] > 58)
//         sendData(0);
//       else
//         sendData(pgm_read_byte(&bigNumbers[number[count] - 48][i]));

//       if (column >= 23) {
//         column = 0;
//         setCursorXY(X, ++Y);
//       }
//       else
//         column++;

//     }

//     count++;

//     X = X + 3;
//     Y = Y - 4;

//   }
// }

// void OzOLED::printBigNumber(const char *character, byte startColumn, byte startPage)
// {
//   // Please fill in font info!
//   // const byte *fontName = &bigNumbers; //font name
//   uint16_t fontWidth = 24;
//   uint16_t fontHeight = 32;
//   uint8_t startCharacter = '0'; //48, 0x30
//   uint8_t endCharacter = ':';   //58, 0x3A
//   int16_t spaceWidth = 0;

//   byte count = 0;
//   byte pageLenght = fontHeight / 8;
//   byte columnLength = fontWidth;
//   byte numChar = strlen(character);
//   byte column = 0;
//   // byte count = 0;

//   while (character[count] && count < numChar)
//   {

//     setCursorXY(startColumn, startPage);

//     for (byte i = 0; i < 96; i++)
//     {

//       // if character is not "0-9" or ':'
//       if (character[count] < startCharacter || character[count] > endCharacter)
//         sendData(0);
//       else
//         sendData(pgm_read_byte(&bigNumbers[character[count] - startCharacter][i]));

//       if (column >= fontWidth - 1)
//       {
//         column = 0;
//         setCursorXY(startColumn, ++startPage);
//       }
//       else
//         column++;
//     }

//     count++;

//     startColumn = startColumn + fontWidth + spaceWidth;
//     startPage = startPage - pageLenght;
//   }
// }

// void OzOLED::printBigNumber(const char *character, byte startColumn, byte startPage)
// {
//   // Please fill in font info!
//   // const byte *fontName = &bigNumbers; //font name
//   uint16_t fontWidth = 24;
//   uint16_t fontHeight = 32;
//   uint8_t startCharacter = '-'; //48, 0x30
//   uint8_t endCharacter = ':';   //58, 0x3A
//   int16_t spaceWidth = 0;

//   byte count = 0;
//   byte pageLength = fontHeight / 8;
//   byte columnLength = fontWidth;
//   byte numChar = strlen(character);
//   byte column = 0;
//   byte page = 0;

//   while (character[count] && count < numChar)
//   {

//     // Serial.print(startColumn);
//     // Serial.print(" ");
//     // Serial.println(startPage);

//     setCursorXY(startColumn, startPage);

//     for (byte i = 0; i < 96; i++)
//     {

//       // if character is not "0-9" or ':'
//       if (character[count] < startCharacter || character[count] > endCharacter)
//         sendData(0);
//       else
//         sendData(pgm_read_byte(&bigNumbers[character[count] - startCharacter][i]));

//       if (page >= pageLength - 1)
//       {
//         page = 0;
//         setCursorXY(++startColumn, startPage);
//       }
//       else
//         page++;
//         setCursorXY(startColumn, startPage + page);
//     }

//     count++;

//     startColumn = startColumn + spaceWidth;
//     startPage = startPage;
//   }

//   // Serial.println();
// }

void OzOLED::printBigNumber(const char *character, byte startColumn, byte startPage)
{
  // Please fill in font info!
  // const byte *fontName = &bigNumbers; //font name
  uint16_t fontWidth = 16;
  uint16_t fontHeight = 24;
  uint8_t startCharacter = '-'; //48, 0x30
  uint8_t endCharacter = ':';   //58, 0x3A
  int16_t spaceWidth = 0;

  byte count = 0;
  byte pageLenght = fontHeight / 8;
  // byte columnLength = fontWidth;
  byte numChar = strlen(character);
  byte column = 0;
  // byte count = 0;

  uint8_t data = 0;
  uint16_t color = 0;

  if (color)
    mask = ~mask;

  while (character[count] && count < numChar)
  {
    setCursorXY(startColumn, startPage);

    for (byte i = 0; i < pageLenght; i++)
    {
      byte j = i;
      // Serial.println();

      for (byte k = 0; k < fontWidth; k++)
      {
        // if character is not "0-9" or ':'
        if (character[count] < startCharacter || character[count] > endCharacter)
          data = 0;
        else
        {
          data = pgm_read_byte(&bigNumbers16x24[character[count] - startCharacter][j]);
          // sendData(hex);
          // Serial.print(hex, HEX);
          // Serial.print(" ");
        }

        data = data ^ mask;
        sendData(data);

        j = j + pageLenght;

        if (column >= fontWidth - 1)
        {
          column = 0;
          setCursorXY(startColumn, ++startPage);
        }
        else
          column++;
      }
    }

    count++;

    startColumn = startColumn + fontWidth + spaceWidth;
    startPage = startPage - pageLenght;
  }
  // Serial.println();
  // Serial.println();
}

void OzOLED::printBigNumber2(const char *number, byte X, byte Y, byte numChar)
{
  // big number pixels: 24 x 32

  // Y - page
  byte column = 0;
  byte count = 0;

  while (number[count] && count < numChar)
  {

    setCursorXY(X, Y);

    for (byte i = 0; i < 150; i++)
    {

      // if character is not "0-9" or ':'
      if (number[count] < 48 || number[count] > 58)
        sendData(0);
      else
        sendData(pgm_read_byte(&bigNumbers2[number[count] - 48][i]));

      if (column >= 24)
      {
        column = 0;
        setCursorXY(X, ++Y);
      }
      else
        column++;
    }

    count++;

    X = X + 4;
    Y = Y - 6;
  }
}

void OzOLED::printBigNumber3(const char *number, byte X, byte Y, byte numChar)
{
  // big number pixels: 24 x 32

  // Y - page
  byte column = 0;
  byte count = 0;

  while (number[count] && count < numChar)
  {

    setCursorXY(X, Y);

    for (byte i = 0; i < 162; i++)
    {

      // if character is not "0-9" or ':'
      if (number[count] < 48 || number[count] > 58)
        sendData(0);
      else
        sendData(pgm_read_byte(&capsuula_48ptBitmaps[number[count] - 48][i]));

      if (column >= 26)
      {
        column = 0;
        setCursorXY(X, ++Y);
      }
      else
        column++;
    }

    count++;

    X = X + 4;
    Y = Y - 6;
  }
}

void OzOLED::printBigNumber4(const char *number, byte X, byte Y, byte startChar, byte endChar)
{
  // big number pixels: 24 x 32

  // Y - page
  byte column = 0;
  byte count = 0;

  while (count != startChar - 1)
  {
    count++;
  }

  while (number[count] && count < endChar)
  {

    setCursorXY(X, Y);

    for (byte i = 0; i < 150; i++)
    {

      // if character is not "0-9" or ':'
      if (number[count] < 48 || number[count] > 58)
        sendData(0);
      else
        sendData(pgm_read_byte(&bigNumbers2[number[count] - 48][i]));

      if (column >= 24)
      {
        column = 0;
        setCursorXY(X, ++Y);
      }
      else
        column++;
    }

    count++;

    X = X + 4;
    Y = Y - 6;
  }
}

void OzOLED::printBigNumber5(const char *number, byte X, byte Y, byte numChar, byte iStart)
{
  // big number pixels: 24 x 32

  // Y - page
  byte column = 0;
  byte count = 0;
  byte row = 0;

  while (number[count] && count < numChar)
  {

    setCursorXY(X, Y);

    for (byte i = 0; i < 162; i++)
    {

      // if character is not "0-9" or ':'
      if (number[count] < 48 || number[count] > 58)
      {
        sendData(0);
      }
      else if (i >= iStart + (27 * row))
      {
        sendData(pgm_read_byte(&capsuula_48ptBitmaps[number[count] - 48][i]));
      }
      else
      {
        sendData(0);
      }

      if (column >= 26)
      {
        column = 0;
        setCursorXY(X, ++Y);
        row++;
      }
      else
        column++;
    }

    count++;

    X = X + 4;
    Y = Y - 6;
  }
}

void OzOLED::drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height)
{

  // max width = 16
  // max height = 8

  setCursorXY(X, Y);

  byte column = 0;
  for (int i = 0; i < width * 8 * height; i++)
  {

    sendData(pgm_read_byte(&bitmaparray[i]));

    if (++column == width * 8)
    {
      column = 0;
      setCursorXY(X, ++Y);
    }
  }
}

void OzOLED::drawBitmap2(const byte *bitmaparray, byte X, byte Y, byte width, byte height)
{

  setVerticalMode(); //Set to Vertical Memory Addressing Mode
  sendCommand(0x21); //Set Column Address
  sendCommand(0);    //Column start address, range : 0-127d,(RESET=0d)
  sendCommand(26);   //Column end address, range : 0-127d,(RESET=0d)
  sendCommand(0x22); //Set Page Address
  sendCommand(0);    //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(5);    //Page end Address, range : 0-7d,(RESET = 0d)

  byte column = 0;
  for (int i = 0; i < 27; i++)
  {

    for (int j = i; j < 162; j = j + 27)
    {
      sendData(pgm_read_byte(&bitmaparray[j]));
      Serial.print(j);
      Serial.print(" ");
      if (j + 27 >= 162)
      {
        Serial.println(" ");
      }
      //delay(50);
    }
  }
}

void OzOLED::slideOutBitmap2(const byte *bitmaparray, byte X, byte Y, byte width, byte height)
{

  setVerticalMode(); //Set to Vertical Memory Addressing Mode
  sendCommand(0x21); //Set Column Address
  sendCommand(0);    //Column start address, range : 0-127d,(RESET=0d)
  sendCommand(26);   //Column end address, range : 0-127d,(RESET=0d)
  sendCommand(0x22); //Set Page Address
  sendCommand(0);    //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(5);    //Page end Address, range : 0-7d,(RESET = 0d)

  byte column = 0;
  for (int k = 0; k < 27; k++)
  {
    Serial.print("SLIDE-");
    Serial.println(k);

    sendCommand(0x21); //Set Column Address
    sendCommand(0);    //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(26);   //Column end address, range : 0-127d,(RESET=0d)
    sendCommand(0x22); //Set Page Address
    sendCommand(0);    //Page start Address, range : 0-7d,(RESET = 0d)
    sendCommand(5);    //Page end Address, range : 0-7d,(RESET = 0d)

    for (int i = k; i < 27; i++)
    {

      for (int j = i; j < 162; j = j + 27)
      {
        //delay(50);
        if (i < 27 - k)
        {
          sendData(pgm_read_byte(&bitmaparray[j]));
          Serial.print(j);
          Serial.print(" ");
          if (j + 27 >= 162)
          {
            Serial.println(" ");
          }
        }
        else if (i >= 27 - k)
        {
          sendData(pgm_read_byte(&bitmaparray[0]));
          Serial.print(j);
          Serial.print("!");
          if (j + 27 >= 162)
          {
            Serial.println("!");
          }
        }
      }
    }
  }
}

void OzOLED::drawLogo(const byte *bitmaparray, int startColumn, byte columnLength, int startPage, byte pageLenght)
{

  setVerticalMode(); //Set to Vertical Memory Addressing Mode
  sendCommand(0x21); //Set Column Address

  if (startColumn >= 0 && startColumn + columnLength <= 128)
  {
    sendCommand(startColumn);                    //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(startColumn + columnLength - 1); //Column end address, range : 0-127d,(RESET=0d)
  }
  else if (startColumn >= 0 && startColumn + columnLength > 128)
  {
    sendCommand(startColumn); //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(128 - 1);
  }
  else if (startColumn < 0)
  {
    sendCommand(0); //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(startColumn + columnLength - 1);
  }
  sendCommand(0x22);                       //Set Page Address
  sendCommand(startPage);                  //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(startPage + pageLenght - 1); //Page end Address, range : 0-7d,(RESET = 0d)

  byte column = 0;
  if (startColumn >= 0 && startColumn + columnLength <= 128)
  {
    //Serial.println("A");
    for (int i = 0; i < columnLength; i++)
    {

      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(pgm_read_byte(&bitmaparray[j]));
        //Serial.print(j);
        //Serial.print(" ");
        if (j + columnLength >= (columnLength * pageLenght))
        {
          //Serial.println(" ");
        }
        //delay(50);
      }
    }
  }
  else if (startColumn >= 0 && startColumn + columnLength > 128)
  {
    //Serial.println("B");
    for (int i = 0; i < 128 - startColumn; i++)
    {
      //Serial.println(" ");
      //for (int j = i; j < ((128 - startColumn) * pageLenght); j = j + columnLength) {
      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(pgm_read_byte(&bitmaparray[j]));
        //Serial.print(pgm_read_byte(&bitmaparray[j]), HEX);
        //Serial.print(" ");
        if (j + columnLength >= (columnLength * pageLenght))
        {
          //Serial.println(" ");
        }
        //delay(50);
      }
    }
  }
  else if (startColumn < 0)
  {
    //Serial.println("C");
    for (int i = columnLength - (startColumn + columnLength); i < columnLength; i++)
    {
      //Serial.println(" ");
      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(pgm_read_byte(&bitmaparray[j]));
        //Serial.print(pgm_read_byte(&bitmaparray[j]), HEX);
        //Serial.print(" ");
        //delay(50);
      }
    }
  }
}

void OzOLED::drawFont8(const char *character, byte startColumn, byte startPage)
{
  // Please fill in font info!

  // #define fontName font5x8      //font name
  // #define fontWidth 5
  // #define fontHeight 8
  // #define startCharacter ' '    //32, 0x20
  // #define endCharacter 0x7F     //127, unknown char (0x7F)
  // #define spaceWidth 1

#define fontName BasicFont //font name
#define fontWidth 8
#define fontHeight 8
#define startCharacter ' ' //32, 0x20
#define endCharacter 0x7F  //127, unknown char (0x7F)
#define spaceWidth 0

  byte count = 0;
  byte pageLenght = fontHeight / 8;
  byte columnLength = fontWidth;
  byte numChar = strlen(character);

  //  // set initial cursor position
  //  setVerticalMode();                                                            //Set to Vertical Memory Addressing Mode
  //  sendCommand(0x21);                                                            //Set Column Address
  //  sendCommand(startColumn);                                                     //Column start address, range : 0-127d,(RESET=0d)
  //  sendCommand(startColumn + numChar * columnLength + (numChar - 1)*spaceWidth); //Column end address, range : 0-127d,(RESET=0d)
  //  sendCommand(0x22);                                                            //Set Page Address
  //  sendCommand(startPage);                                                       //Page start Address, range : 0-7d,(RESET = 0d)
  //  sendCommand(startPage + pageLenght - 1);                                      //Page end Address, range : 0-7d,(RESET = 0d)

  uint8_t data = 0;
  uint16_t color = 0;

  if (color)
    mask = ~mask;

  setCursorXY(startColumn, startPage);

  //send data to display
  while (character[count] && count < numChar)
  {
    for (byte i = 0; i < (columnLength * pageLenght); i++)
    {
      // if character is not "0-9" or ':'
      if (character[count] < startCharacter || character[count] > endCharacter)
        data = 0;
      else
        data = pgm_read_byte(&fontName[character[count] - startCharacter][i]);

      data = data ^ mask;
      sendData(data);
    }

    count++;

    //add space between character
    data = 0;
    data = data ^ mask;
    if (spaceWidth > 0 && count < numChar)
    {
      for (byte i = 0; i < spaceWidth; i++)
      {
        for (byte j = 0; j < pageLenght; j++)
        {

          sendData(data);
        }
      }
    }
  }

  //  // reset cursor position
  //  setHorizontalMode(); //Set to Vertical Memory Addressing Mode
  //  sendCommand(0x21); //Set Column Address
  //  sendCommand(0); //Column start address, range : 0-127d,(RESET=0d)
  //  sendCommand(127); //Column end address, range : 0-127d,(RESET=0d)
  //  sendCommand(0x22); //Set Page Address
  //  sendCommand(0); //Page start Address, range : 0-7d,(RESET = 0d)
  //  sendCommand(7); //Page end Address, range : 0-7d,(RESET = 0d)
}

void OzOLED::drawFont16(const char *number, byte numChar, byte startColumn, byte columnLength, byte startPage, byte pageLenght)
{
  // number pixels: 18 x 32

  // Y - page
  byte column = 0;
  byte count = 0;
  byte row = 0;

  setVerticalMode();                       //Set to Vertical Memory Addressing Mode
  sendCommand(0x21);                       //Set Column Address
  sendCommand(startColumn);                //Column start address, range : 0-127d,(RESET=0d)
  sendCommand(startColumn + columnLength); //Column end address, range : 0-127d,(RESET=0d)
  sendCommand(0x22);                       //Set Page Address
  sendCommand(startPage);                  //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(startPage + pageLenght - 1); //Page end Address, range : 0-7d,(RESET = 0d)

  for (byte i = 0; i < (columnLength * pageLenght); i++)
  {

    // if character is not "0-9" or ':'
    if (number[count] < 48 || number[count] > 58)
      sendData(0);
    else
      sendData(pgm_read_byte(&Capsuula12x16[number[count] - 48][i]));
  }

  setHorizontalMode(); //Set to Vertical Memory Addressing Mode
  sendCommand(0x21);   //Set Column Address
  sendCommand(0);      //Column start address, range : 0-127d,(RESET=0d)
  sendCommand(127);    //Column end address, range : 0-127d,(RESET=0d)
  sendCommand(0x22);   //Set Page Address
  sendCommand(0);      //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(7);      //Page end Address, range : 0-7d,(RESET = 0d)
}

//void OzOLED::drawLogo2(const char *number, byte numChar, byte startColumn, byte columnLength, byte startPage, byte pageLenght) {
//  //void OzOLED::printBigNumber5(const char *number, byte X, byte Y, byte numChar, byte iStart) {
//  // big number pixels: 24 x 32
//
//  // Y - page
//  byte column = 0;
//  byte count = 0;
//  byte row = 0;
//
//  while (number[count] && count < numChar) {
//    Serial.print("CHAR ");
//    Serial.print(number[count]);
//    Serial.print(" ");
//    Serial.print((number[count] - 48) * 54);
//    Serial.print("/");
//    Serial.println(((number[count] - 48) * 54) + 54);
//
//    setCursorXY(startColumn, startPage);
//
//    for (int i = ((number[count] - 48) * 54); i < ((number[count] - 48) * 54) + 54; i++) {
//      //delay(50);
//
//      // if character is not "0-9" or ':'
//      if (number[count] < 48 || number[count] > 58) {
//        //sendData(0);
//        return;
//      }
//      else if (true) {
//        sendData(pgm_read_byte(&iceland_36ptBitmaps[i]));
//        Serial.print(pgm_read_byte(&iceland_36ptBitmaps[i]), HEX);
//        Serial.print(" ");
//        column++;
//      }
//      //      else {
//      //        sendData(0);
//      //      }
//
//      if (column > 17) {
//        column = 0;
//        //startPage = startPage + 1;
//        setCursorXY(startColumn, ++startPage);
//        //row++;
//        Serial.println("");
//      }
//      //      else
//      //        column++;
//
//    }
//
//    count++;
//
//    //X = X + 4;
//    //Y = Y - 6;
//
//  }
//}

void OzOLED::drawLogo2(const char *number, byte numChar, byte startColumn, byte columnLength, byte startPage, byte pageLenght)
{
  //void OzOLED::printBigNumber5(const char *number, byte X, byte Y, byte numChar, byte iStart) {
  // big number pixels: 24 x 32

  // Y - page
  byte column = 0;
  byte count = 0;
  byte row = 0;

  while (number[count] && count < numChar)
  {
    //Serial.print("CHAR ");
    //Serial.print(number[count]);
    //Serial.print(" ");
    //Serial.print(sizeof Iceland17x24 / sizeof Iceland17x24[number[count] - 48] );
    //Serial.print(" ");
    //Serial.println(sizeof Iceland17x24[number[count] - 48] /  sizeof Iceland17x24[number[count] - 48][0]);

    setCursorXY(startColumn, startPage);

    // if character is not "0-9" or ':'
    //    if (number[count] < 48 || number[count] > 58) {
    //      sendData(0);
    //      //return;
    //    }
    if (true)
    {
      for (int i = 0; i < 3; i++)
      {
        byte max;
        byte c;
        if (number[count] - 48 == 10 || number[count] == 32)
        {
          max = 9;
          c = 3;
          //Serial.println("COLON");
        }
        else
        {
          max = 51;
          c = 17;
          //Serial.println("KOLOR IJO");
        }
        for (int j = i; j < max; j = j + 3)
        {
          //delay(125);
          if (number[count] == 32)
          {
            sendData(0);
          }
          else
          {
            sendData(pgm_read_byte(&Iceland17x24[number[count] - 48][j]));
          }
          //Serial.print(pgm_read_byte(&Iceland17x24[number[count] - 48][j]), HEX);
          //Serial.print(j);
          //Serial.print(" ");
          column++;
          if (column >= c)
          {
            column = 0;
            //startPage = startPage + 1;
            setCursorXY(startColumn, ++startPage);
            //row++;
            //Serial.println("");
          }
        }
      }
      count++;
      startColumn = startColumn + 17;
      startPage = startPage - 3;
    }

    //count++;

    //X = X + 4;
    //Y = Y - 6;
  }
}

//void OzOLED::eraseLogo(byte startColumn, byte columnLength, byte startPage, byte pageLenght) {
//
//  setVerticalMode(); //Set to Vertical Memory Addressing Mode
//  sendCommand(0x21); //Set Column Address
//  sendCommand(startColumn); //Column start address, range : 0-127d,(RESET=0d)
//  sendCommand(startColumn + columnLength); //Column end address, range : 0-127d,(RESET=0d)
//  sendCommand(0x22); //Set Page Address
//  sendCommand(startPage); //Page start Address, range : 0-7d,(RESET = 0d)
//  sendCommand(startPage + pageLenght - 1); //Page end Address, range : 0-7d,(RESET = 0d)
//
//
//  byte column = 0;
//  for (int i = 0; i < columnLength; i++) {
//
//    for (int j = i; j < (columnLength * pageLenght); j = j + columnLength) {
//      sendData(0x00);
//      //Serial.print(j);
//      //Serial.print(" ");
//      if (j + columnLength >= (columnLength * pageLenght)) {
//        //Serial.println(" ");
//      }
//      //delay(50);
//    }
//  }
//}

void OzOLED::eraseLogo(int startColumn, byte columnLength, int startPage, byte pageLenght)
{

  setVerticalMode(); //Set to Vertical Memory Addressing Mode
  sendCommand(0x21); //Set Column Address
  if (startColumn >= 0 && startColumn + columnLength <= 128)
  {
    sendCommand(startColumn);                    //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(startColumn + columnLength - 1); //Column end address, range : 0-127d,(RESET=0d)
  }
  else if (startColumn >= 0 && startColumn + columnLength > 128)
  {
    sendCommand(startColumn); //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(128 - 1);
  }
  else if (startColumn < 0)
  {
    sendCommand(0); //Column start address, range : 0-127d,(RESET=0d)
    sendCommand(startColumn + columnLength - 1);
  }
  sendCommand(0x22);                       //Set Page Address
  sendCommand(startPage);                  //Page start Address, range : 0-7d,(RESET = 0d)
  sendCommand(startPage + pageLenght - 1); //Page end Address, range : 0-7d,(RESET = 0d)

  byte column = 0;
  if (startColumn >= 0 && startColumn + columnLength <= 128)
  {
    for (int i = 0; i < columnLength; i++)
    {
      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(0);
      }
    }
  }
  else if (startColumn >= 0 && startColumn + columnLength > 128)
  {
    for (int i = 0; i < 128 - startColumn; i++)
    {
      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(0);
      }
    }
  }
  else if (startColumn < 0)
  {
    for (int i = columnLength - (startColumn + columnLength); i < columnLength; i++)
    {
      for (int j = i; j < (columnLength * pageLenght); j = j + columnLength)
      {
        sendData(0);
      }
    }
  }
}

// =================== from NICK GAMMON =====================

// send command to LCD display (chip 1 or 2 as in chipSelect variable)
// for example, setting page (Y) or address (X)
//void I2C_graphical_LCD_display::cmd (const byte data)
//{
//  startSend ();
//  doSend (GPIOA);                      // control port
//  doSend (LCD_RESET | LCD_ENABLE | _chipSelect);   // set enable high (D/I is low meaning instruction)
//  doSend (data);                       // (command written to GPIOB)
//  doSend (LCD_RESET | _chipSelect);    // (GPIOA again) pull enable low to toggle data
//  endSend ();
//} // end of I2C_graphical_LCD_display::cmd
void OzOLED::cmd(const byte data)
{
  // I2C
  uint8_t control = 0x00; // Co = 0, D/C = 0
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(control);
  Wire.write(data);
  Wire.endTransmission();
} // end of I2C_graphical_LCD_display::cmd

// set our "cursor" to the x/y position
// works out whether this refers to chip 1 or chip 2 and sets chipSelect appropriately

// Approx time to run: 33 ms on Arduino Uno
void OzOLED::gotoxy(byte x,
                    byte y)
{

  if (x > 127)
    x = 0;
  if (y > 63)
    y = 0;

  // remember for incrementing later
  _lcdx = x;
  _lcdy = y;

  // command LCD to the correct page and address
  cmd(LCD_SET_PAGE | (y >> 3)); // 8 pixels to a page
  cmd(LCD_SET_ADD | x);

} // end of I2C_graphical_LCD_display::gotoxy

// Approx time to run: 4 ms on Arduino Uno
void OzOLED::letter(byte c,
                    const boolean inv)
{
  if (c < 0x20 || c > 0x7F)
    c = 0x7F; // unknown glyph

  c -= 0x20; // force into range of our font table (which starts at 0x20)

  // no room for a whole character? drop down a line
  // letters are 5 wide, so once we are past 59, there isn't room before we hit 63
  if (_lcdx >= 60)
    gotoxy(0, _lcdy + 8);

  // font data is in PROGMEM memory (firmware)
  for (int x = 0; x < 5; x++)
    //writeData (pgm_read_byte (&font [c] [x]), inv);
    sendData(pgm_read_byte(&font5x8[c][x]));
  //writeData (0, inv);  // one-pixel gap between letters
  sendData(0); // one-pixel gap between letters

} // end of I2C_graphical_LCD_display::letter

// write an entire null-terminated string to the LCD: inverted or normal
void OzOLED::string(const char *s,
                    const boolean inv)
{
  char c;
  while (c = *s++)
    letter(c, inv);
} // end of I2C_graphical_LCD_display::string

// =================== High Level ===========================

void OzOLED::init(uint8_t vccstate, uint8_t i2caddr, bool reset)
{
  Wire.begin();

  // upgrade to 400KHz! (only use when your other i2c device support this speed)
  if (I2C_400KHZ)
  {
    // save I2C bitrate (default 100Khz)
    // byte twbrbackup = TWBR;
    // TWBR = 12;
    TWBR = ((F_CPU / 400000L) - 16) / 2;
    //TWBR = twbrbackup;
    //Serial.println(TWBR, DEC);
    //Serial.println(TWSR & 0x3, DEC);
  }

  // Init sequence
  sendCommand(SSD1306_DISPLAYOFF);         // 0xAE *** 0xAE
  sendCommand(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5 *** Command no.15 on SH1106 datasheet, 0xD5
  sendCommand(0x80);                       // the suggested ratio 0x80

  sendCommand(SSD1306_SETMULTIPLEX); // 0xA8 *** Command no.9 on SH1106 datasheet, 0xA8
  sendCommand(OLED_LCDHEIGHT - 1);

  sendCommand(SSD1306_SETDISPLAYOFFSET);   // 0xD3 *** Command no.14 on SH1106 datasheet, 0xD3
  sendCommand(0x0);                        // no offset
  sendCommand(SSD1306_SETSTARTLINE | 0x0); // line #0 *** Command no.4 on SH1106 datasheet, 0x40

#if defined SH1106_132_64
  //CHARGE PUMP SETTINGS, default to using Built-in DC-DC, Command no.10 on SH1106 datasheet, 0x8B
#else
  sendCommand(SSD1306_CHARGEPUMP); // 0x8D
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    sendCommand(0x10);
  }
  else
  {
    sendCommand(0x14);
  }
#endif

#if defined SH1106_132_64
  // do nothing, Not availabgle for SH1106
#else
  sendCommand(SSD1306_MEMORYMODE); // 0x20
  sendCommand(0x00);               // 0x0 act like ks0108
#endif

  sendCommand(SSD1306_SEGREMAP | 0x1); // 0xA0 *** Command no.6 on SH1106 datasheet, (A0H - A1H)
  sendCommand(SSD1306_COMSCANDEC);     // 0xC8 *** Command no.13 on SH1106 datasheet, (C0H - C8H)

#if defined SSD1306_128_32
  sendCommand(SSD1306_SETCOMPINS); // 0xDA
  sendCommand(0x02);
  sendCommand(SSD1306_SETCONTRAST); // 0x81
  sendCommand(0x8F);

#elif defined SSD1306_128_64
  sendCommand(SSD1306_SETCOMPINS); // 0xDA
  sendCommand(0x12);
  sendCommand(SSD1306_SETCONTRAST); // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    sendCommand(0x9F);
  }
  else
  {
    sendCommand(0xCF);
  }

#elif defined SSD1306_96_16
  sendCommand(SSD1306_SETCOMPINS);  // 0xDA
  sendCommand(0x2);                 //ada x12
  sendCommand(SSD1306_SETCONTRAST); // 0x81
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    sendCommand(0x10);
  }
  else
  {
    sendCommand(0xAF);
  }

#elif defined SH1106_132_64
  sendCommand(SSD1306_SETCOMPINS);  // 0xDA,  *** Command no.17 on SH1106 datasheet,  (DAH)
  sendCommand(0x12);                // *** Command no.17 on SH1106 datasheet, Sequential/Alternative Mode Set: (02H - 12H)
  sendCommand(SSD1306_SETCONTRAST); // 0x81 *** Command no.5 on SH1106 datasheet,   (81H)
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    sendCommand(0x9F);
  }
  else
  {
    sendCommand(0xCF);
  }

#endif

  sendCommand(SSD1306_SETPRECHARGE); // 0xd9 *** Command no.16 on SH1106 datasheet,  (D9H)
  if (vccstate == SSD1306_EXTERNALVCC)
  {
    sendCommand(0x22);
  }
  else
  {
    sendCommand(0xF1);
    //sendCommand(0x22); //test
  }
  sendCommand(SSD1306_SETVCOMDETECT); // 0xDB *** Command no.18 on SH1106 datasheet,  (DBH)

  //sendCommand(0x40);

  //sendCommand(0x00); //test

  sendCommand(SSD1306_DISPLAYALLON_RESUME); // 0xA4 *** Command no.7 on SH1106 datasheet, (A4H)
  sendCommand(SSD1306_NORMALDISPLAY);       // 0xA6 *** Command no.8 on SH1106 datasheet, (A6H)

#if defined SH1106_132_64
  //do nothing, scrolling command is not availabe for SH1106
#else
  sendCommand(SSD1306_DEACTIVATE_SCROLL);
#endif

  sendCommand(SSD1306_DISPLAYON); //--turn on oled panel   // 0xAE *** Command no.8 on SH1106 datasheet, (AEH)
}

//void OzOLED::setCursorXY(byte X, byte Y) {
//  // Y - 1 unit = 1 page (8 pixel rows)
//  // X - 1 unit = 8 pixel columns
//
//  sendCommand(0x02 + (8 * X & 0x0F));   //set column lower address
//  sendCommand(0x10 + ((8 * X >> 4) & 0x0F)); //set column higher address
//  sendCommand(0xB0 + Y);          //set page address
//
//}

void OzOLED::setCursorXY(byte X, byte Y)
{
  // Y - 1 unit = 1 page (8 pixel rows)
  // X - 1 unit = 8 pixel columns

  X = X + 2;
  byte lowerColumn = X;
  lowerColumn = 0x00 + (lowerColumn & 0x0F);
  //byte higherColumn = 0x10 + ((X >> 4) & 0x0F);
  byte higherColumn = ((X >> 4) & 0x0F);

  //Serial.print(lowerColumn);
  //Serial.print(" ");
  //Serial.println(higherColumn);

  higherColumn = 0x10 + higherColumn;

  //higherColumn = 0x10 + ((X >> 4) & 0x0F);

  sendCommand(lowerColumn);  //set column lower address
  sendCommand(higherColumn); //set column higher address
  sendCommand(0xB0 + Y);     //set page address
}

void OzOLED::clearDisplay()
{
  uint8_t data = 0;
  uint16_t color = 0;

  if (color)
    mask = ~mask;

  data = data ^ mask;

  for (byte page = 0; page < 8; page++)
  {

    setCursorXY(0, page);
    for (byte column = 0; column < 132; column++)
    { //clear all columns
      sendData(data);
    }
  }

  setCursorXY(0, 0);
}

//void OzOLED::clearDisplay() {
//
//
//  for (uint16_t i = 0; i < 1024; i++) {
//    Wire.beginTransmission(OLED_I2C_ADDRESS);
//    Wire.write(OLED_CONTROL_BYTE_DATA_STREAM);
//    for (uint8_t x = 0; x < 16; x++) {
//      Wire.write(0);
//      i++;
//    }
//    i--;
//    Wire.endTransmission();
//  }
//
//}

//void OzOLED::clearDisplay() {
//
//  for (uint16_t i = 0; i < 1056; i++) {
//    Wire.beginTransmission(OLED_I2C_ADDRESS);
//    Wire.write(OLED_CONTROL_BYTE_DATA_STREAM);
//    for (uint8_t x = 0; x < 16; x++) {
//      Wire.write(0);
//      i++;
//    }
//    i--;
//    Wire.endTransmission();
//  }
//
//}

/*
  void OzOLED::clearPage(byte page) {
  // clear page and set cursor at beginning of that page

  setCursorXY(0, page);
  for(byte column=0; column<128; column++){  //clear all columns
    sendData(0x00);
  }

  }
*/

void OzOLED::setRotateDisplay()
{

  sendCommand(OzOLED_CMD_ROTATE_DISPLAY);
}

void OzOLED::setNormalDisplay()
{

  sendCommand(OzOLED_CMD_NORMAL_DISPLAY);
}

void OzOLED::setPowerOff()
{

  sendCommand(OzOLED_CMD_DISPLAY_OFF);
}

void OzOLED::setPowerOn()
{

  sendCommand(OzOLED_CMD_DISPLAY_ON);
}

void OzOLED::setBrightness(byte Brightness)
{

  sendCommand(OzOLED_CMD_SET_BRIGHTNESS);
  sendCommand(Brightness);
}

void OzOLED::setPageMode()
{
  addressingMode = PAGE_ADDRESSING;
  sendCommand(0x20);            //set addressing mode
  sendCommand(PAGE_ADDRESSING); //set page addressing mode
}

void OzOLED::setHorizontalMode()
{
  addressingMode = HORIZONTAL_ADDRESSING;
  sendCommand(0x20);                  //set addressing mode
  sendCommand(HORIZONTAL_ADDRESSING); //set Horizontal addressing mode
}

void OzOLED::setVerticalMode()
{
  addressingMode = VERTICAL_ADDRESSING;
  sendCommand(0x20);                //set addressing mode
  sendCommand(VERTICAL_ADDRESSING); //set Vdrtical addressing mode
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// scrollRight(0x00, 0x0F)  - start - stop
void OzOLED::scrollRight(byte start, byte end, byte speed)
{

  sendCommand(OzOLED_RIGHT_SCROLL); //Horizontal Scroll Setup
  sendCommand(0x00);                // dummy byte
  sendCommand(start);               // start page address
  sendCommand(speed);               // set time interval between each scroll
  sendCommand(end);                 // end page address

  sendCommand(0x01);
  sendCommand(0xFF);

  sendCommand(0x2f); //active scrolling
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)   - start - stop
void OzOLED::scrollLeft(byte start, byte end, byte speed)
{

  sendCommand(OzOLED_LEFT_SCROLL); //Horizontal Scroll Setup
  sendCommand(0x00);               // dummy byte
  sendCommand(start);              // start page address
  sendCommand(speed);              // set time interval between each scroll
  sendCommand(end);                // end page address

  sendCommand(0x01);
  sendCommand(0xFF);

  sendCommand(0x2f); //active scrolling
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OzOLED::scrollDiagRight()
{

  sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);
  sendCommand(0X00);
  sendCommand(OzOLED_Max_Y);
  sendCommand(OzOLED_VERTICAL_RIGHT_SCROLL); //Vertical and Horizontal Scroll Setup
  sendCommand(0X00);                         //dummy byte
  sendCommand(0x00);                         //define page0 as startpage address
  sendCommand(0X00);                         //set time interval between each scroll ste as 6 frames
  sendCommand(0x07);                         //define page7 as endpage address
  sendCommand(0X01);                         //set vertical scrolling offset as 1 row
  sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);   //active scrolling
}

void OzOLED::scrollDiagLeft()
{

  sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);
  sendCommand(0X00);
  sendCommand(OzOLED_Max_Y);
  sendCommand(OzOLED_VERTICAL_LEFT_SCROLL); //Vertical and Horizontal Scroll Setup
  sendCommand(0X00);                        //dummy byte
  sendCommand(0x00);                        //define page0 as startpage address
  sendCommand(0X00);                        //set time interval between each scroll ste as 6 frames
  sendCommand(0x07);                        //define page7 as endpage address
  sendCommand(0X01);                        //set vertical scrolling offset as 1 row
  sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);  //active scrolling
}

void OzOLED::setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed)
{

  /*
    This function is still not complete, we need more testing also.
    Use the following defines for 'direction' :

    Scroll_Left
    Scroll_Right

    For Scroll_vericle, still need to debug more...

    Use the following defines for 'scrollSpeed' :

    Scroll_2Frames
    Scroll_3Frames
    Scroll_4Frames
    Scroll_5Frames
    Scroll_25Frames
    Scroll_64Frames
    Scroll_128Frames
    Scroll_256Frames

  */

  if (direction == Scroll_Right)
  {

    //Scroll Right
    sendCommand(0x26);
  }
  else
  {

    //Scroll Left
    sendCommand(0x27);
  }
  /*
    else if (direction == Scroll_Up ){

    //Scroll Up
    sendCommand(0x29);

    }
    else{

    //Scroll Down
    sendCommand(0x2A);

    }
  */
  sendCommand(0x00); //dummy byte
  sendCommand(startPage);
  sendCommand(scrollSpeed);
  sendCommand(endPage); // for verticle scrolling, use 0x29 as command, endPage should = start page = 0

  /*
    if(direction == Scroll_Up) {

    sendCommand(0x01);

    }
  */

  sendCommand(0x01);
  sendCommand(0xFF);

  sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);
}

void OzOLED::setDeactivateScroll()
{

  sendCommand(OzOLED_CMD_DEACTIVATE_SCROLL);
}

// ==== ADAFRUIT

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OzOLED::startscrollright(uint8_t start, uint8_t stop, uint8_t speed)
{
  sendCommand(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  sendCommand(0X00);
  sendCommand(start);
  sendCommand(speed);
  sendCommand(stop);
  sendCommand(0X00);
  sendCommand(0XFF);
  sendCommand(SSD1306_ACTIVATE_SCROLL);
}

// clear everything
void OzOLED::clearDisplayAdafruit(void)
{
  memset(buffer, 0, (OLED_LCDWIDTH * OLED_LCDHEIGHT / 8));
}

// void OzOLED::inverseDisplayColor(boolean i)
// {
//   if (i) {
//     mask = ~mask;
//   }
// }

void OzOLED::setInverseDisplayColor(bool i = false)
{
    mask = 0;
    if (i) {
      mask = ~mask;
    }
}
