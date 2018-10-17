/*
  OzOLED.h - 0.96' I2C 128x64 OLED Driver Library
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



#ifndef OzOLED_data_H
#define OzOLED_data_H

#include <Arduino.h>

#define OzOLED_Max_X          128 //128 Pixels
#define OzOLED_Max_Y          64  //64  Pixels

#define OLED_ADDRESS          0x3C
#define I2C_400KHZ            1 // 0 to use default 100Khz, 1 for 400Khz

// registers
#define OzOLED_COMMAND_MODE       0x80
#define OzOLED_DATA_MODE        0x40

// commands
#define OzOLED_CMD_DISPLAY_OFF      0xAE
#define OzOLED_CMD_DISPLAY_ON     0xAF
#define OzOLED_CMD_NORMAL_DISPLAY   0xA6
#define OzOLED_CMD_ROTATE_DISPLAY    0xA7
#define OzOLED_CMD_SET_BRIGHTNESS   0x81

#define OzOLED_RIGHT_SCROLL       0x26
#define OzOLED_LEFT_SCROLL        0x27
#define OzOLED_SET_VERTICAL_SCROLL_AREA 0xA3
#define OzOLED_VERTICAL_RIGHT_SCROLL  0x29
#define OzOLED_VERTICAL_LEFT_SCROLL   0x2A
#define OzOLED_CMD_ACTIVATE_SCROLL    0x2F
#define OzOLED_CMD_DEACTIVATE_SCROLL  0x2E

#define HORIZONTAL_ADDRESSING 0x00
#define VERTICAL_ADDRESSING 0x01
#define PAGE_ADDRESSING     0x02

#define Scroll_Left       0x00
#define Scroll_Right      0x01
#define Scroll_Up       0x02
#define Scroll_Down       0x03

#define Scroll_2Frames      0x07
#define Scroll_3Frames      0x04
#define Scroll_4Frames      0x05
#define Scroll_5Frames      0x00
#define Scroll_25Frames     0x06
#define Scroll_64Frames     0x01
#define Scroll_128Frames    0x02
#define Scroll_256Frames    0x03



//~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Some defines for the SSD1306 controller driving a 128x64 resolution OLED display
// PART     - http://www.simplelabs.co.in/content/96-blue-i2c-oled-module
// DATASHEET  - https://www.adafruit.com/datasheets/SSD1306.pdf

// The Slave Address (SLA) of the OLED controller - SSD1306 - is 0x3C 
// The LSB is supposed to be the mode. Since we are only going to WRITE to the OLED, the LSB is going to be 0
// SLA (0x3C) + WRITE_MODE (0x00) =  0x78 (0b01111000)
#define OLED_I2C_ADDRESS   0x3C

// The SSD1306 datasheet (pg.20) says that a control byte has to be sent before sending a command
// Control byte consists of 
// bit 7    : Co   : Continuation bit - If 0, then it assumes all the next bytes are data (no more control bytes).
//        :    You can send a stream of data, ie: gRAM dump - if Co=0
//        :        For Command, you'd prolly wanna set this - one at a time. Hence, Co=1 for commands
//        :    For Data stream, Co=0 :)
// bit 6      : D/C# : Data/Command Selection bit, Data=1/Command=0
// bit [5-0]  : lower 6 bits have to be 0
#define OLED_CONTROL_BYTE_CMD_SINGLE  0x80
#define OLED_CONTROL_BYTE_CMD_STREAM  0x00
#define OLED_CONTROL_BYTE_DATA_STREAM 0x40

// Fundamental commands (pg.28)
#define OLED_CMD_SET_CONTRAST     0x81  // follow with 0x7F
#define OLED_CMD_DISPLAY_RAM      0xA4
#define OLED_CMD_DISPLAY_ALLON      0xA5
#define OLED_CMD_DISPLAY_NORMAL     0xA6
#define OLED_CMD_DISPLAY_INVERTED     0xA7
#define OLED_CMD_DISPLAY_OFF      0xAE
#define OLED_CMD_DISPLAY_ON       0xAF

// Addressing Command Table (pg.30)
#define OLED_CMD_SET_MEMORY_ADDR_MODE 0x20  // follow with 0x00 = HORZ mode = Behave like a KS108 graphic LCD
#define OLED_CMD_SET_COLUMN_RANGE   0x21  // can be used only in HORZ/VERT mode - follow with 0x00 + 0x7F = COL127
#define OLED_CMD_SET_PAGE_RANGE     0x22  // can be used only in HORZ/VERT mode - follow with 0x00 + 0x07 = PAGE7

// Hardware Config (pg.31)
#define OLED_CMD_SET_DISPLAY_START_LINE 0x40
#define OLED_CMD_SET_SEGMENT_REMAP    0xA1  
#define OLED_CMD_SET_MUX_RATIO      0xA8  // follow with 0x3F = 64 MUX
#define OLED_CMD_SET_COM_SCAN_MODE    0xC8  
#define OLED_CMD_SET_DISPLAY_OFFSET   0xD3  // follow with 0x00
#define OLED_CMD_SET_COM_PIN_MAP    0xDA  // follow with 0x12
#define OLED_CMD_NOP          0xE3  // NOP

// Timing and Driving Scheme (pg.32)
#define OLED_CMD_SET_DISPLAY_CLK_DIV  0xD5  // follow with 0x80
#define OLED_CMD_SET_PRECHARGE      0xD9  // follow with 0xF1
#define OLED_CMD_SET_VCOMH_DESELCT    0xDB  // follow with 0x30

// Charge Pump (pg.62)
#define OLED_CMD_SET_CHARGE_PUMP    0x8D  // follow with 0x14

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#define LCD_SET_ADD     0x40   // plus X address (0 to 63) 
#define LCD_SET_PAGE    0xB8   // plus Y address (0 to 7)


// =================== From ADAFRUIT =======================


#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SSD1306_128_64  128x64 pixel display

    SSD1306_128_32  128x32 pixel display

    SSD1306_96_16

    -----------------------------------------------------------------------*/
//   #define SSD1306_128_64
//   #define SSD1306_128_32
//   #define SSD1306_96_16
#define SH1106_132_64
/*=========================================================================*/


#if defined SSD1306_128_64 && defined SSD1306_128_32
  #error "Only one SSD1306 display can be specified at once in SSD1306.h"
#endif
#if !defined SSD1306_128_64 && !defined SSD1306_128_32 && !defined SSD1306_96_16 && !defined SH1106_132_64
  #error "At least one OLED display must be specified in SSD1306.h"
#endif

#if defined SSD1306_128_64
  #define OLED_LCDWIDTH                  128
  #define OLED_LCDHEIGHT                 64
#endif
#if defined SSD1306_128_32
  #define OLED_LCDWIDTH                  128
  #define OLED_LCDHEIGHT                 32
#endif
#if defined SSD1306_96_16
  #define OLED_LCDWIDTH                  96
  #define OLED_LCDHEIGHT                 16
#endif
#if defined SH1106_132_64
  #define OLED_LCDWIDTH                  132
  #define OLED_LCDHEIGHT                 64
#endif

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

// =================== END OF From ADAFRUIT =======================


class OzOLED {

public:

  byte addressingMode;

  void sendCommand(byte command);
  void sendData(byte Data);

  void printChar(char c, byte X=255, byte Y=255);
  void printString(const char *String, byte X=255, byte Y=255, byte numChar=255);
  byte printNumber(long n, byte X=255, byte Y=255);
  byte printNumber(float float_num, byte prec=6, byte Y=255, byte numChar=255);  
  // void printBigNumber(const char *number, byte column=0, byte page=0, byte numChar=255);
  void printBigNumber(const char *character, byte startColumn, byte startPage);
  void printBigNumber2(const char *number, byte column=0, byte page=0, byte numChar=255); 
  void printBigNumber3(const char *number, byte column=0, byte page=0, byte numChar=255);
  void printBigNumber4(const char *number, byte column=0, byte page=0, byte startChar=255, byte endChar=255);
  void printBigNumber5(const char *number, byte column=0, byte page=0, byte numChar=255, byte iStart=255);
  void drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height);
  void drawBitmap2(const byte *bitmaparray, byte X, byte Y, byte width, byte height);
  void slideOutBitmap2(const byte *bitmaparray, byte X, byte Y, byte width, byte height);
  void drawLogo (const byte *bitmaparray, int startColumn, byte columnLength, int startPage, byte pageLenght);
  void drawFont8 (const char *number, byte startColumn, byte startPage);
  void drawFont16 (const char *number, byte numChar, byte startColumn, byte columnLength, byte startPage, byte pageLenght);
  void drawLogo2 (const char *number, byte numChar, byte startColumn, byte columnLength, byte startPage, byte pageLenght);
  void eraseLogo (int startColumn, byte columnLength, int startPage, byte pageLenght);

  void init();
  void init(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = SSD1306_I2C_ADDRESS, bool reset=true);
  
  void setCursorXY(byte Column, byte Row);
  void clearDisplay();
  //void clearPage(byte page);
  
  void setNormalDisplay();
  void setRotateDisplay();
  void setPowerOff();
  void setPowerOn();
  void setPageMode();
  void setVerticalMode();
  void setHorizontalMode();
  void setBrightness(byte Brightness);
  
  void scrollRight(byte start, byte end, byte speed);
  void scrollLeft(byte start, byte end, byte speed);
  void scrollDiagRight();
  void scrollDiagLeft();
  void setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed);
  void setDeactivateScroll();

  void setInverseDisplayColor(bool i);



  void cmd (const byte data);
  void gotoxy (byte x, byte y);
  void letter (byte c, const boolean inv);
  void letter (byte c) {letter(c, _invmode);}
  void string (const char * s, const boolean inv);
  void string (const char * s) {string(s, _invmode);}
  byte _lcdx;        // current x position (0 - 127)
  byte _lcdy;        // current y position (0 - 63)
  boolean _invmode;


  // ===== ADAFRUIT

  void startscrollright(uint8_t start, uint8_t stop, uint8_t speed);
  void clearDisplayAdafruit(void);

};

extern OzOLED OzOled;  // OzOLED object 

//// Font data for Capsuula 32pt
//extern const byte capsuula_32ptBitmaps[];
//extern const FONT_INFO capsuula_32ptFontInfo;
//extern const FONT_CHAR_INFO capsuula_32ptDescriptors[];



#endif


