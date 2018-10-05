#include <Arduino.h>

#include <NMEAGPS.h>

//======================================================================
//  Program: NMEAsimple.ino
//
//  Description:  This program shows simple usage of NeoGPS
//
//  Prerequisites:
//     1) NMEA.ino works with your device (correct TX/RX pins and baud rate)
//     2) At least one of the RMC, GGA or GLL sentences have been enabled in NMEAGPS_cfg.h.
//     3) Your device at least one of those sentences (use NMEAorder.ino to confirm).
//     4) LAST_SENTENCE_IN_INTERVAL has been set to one of those sentences in NMEAGPS_cfg.h (use NMEAorder.ino).
//     5) LOCATION and ALTITUDE have been enabled in GPSfix_cfg.h
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2014-2017, SlashDevin
//
//    This file is part of NeoGPS
//
//    NeoGPS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NeoGPS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.
//
//======================================================================

#include <GPSport.h>
#include <AltSoftSerial.h>

#include <TimeLib.h>
#include "PrayerTimes.h"


#include <Wire.h>
#include "OzOLED.h"
// pull in extra font characters

#include "cp437_font.h"
#include "bigNumbers2.h"
#include "resources.h"
//#include "font32.h"

#include <Adafruit_GFX.h>


#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

const int timeZone = 7;

AltSoftSerial altSerial;

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values


int screenW = 128;
int screenH = 64;
int clockCenterX = screenW / 2;
int clockCenterY = ((screenH - 16) / 2) + 16; // top yellow part is 16 px height
int clockRadius = 23;

// utility function for digital clock display: prints leading 0
String twoDigits(int digits) {
  if (digits < 10) {
    String i = '0' + String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}


int HOUR;
int MINUTE;
String CURRENTTIMENAME;
String NEXTTIMENAME;
int frameCount = 2;

int overlaysCount = 1;

double times[sizeof(TimeName) / sizeof(char*)];

//void p(char *fmt, ... ) {
void p(const char *fmt, ... ) {
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  Serial.print(tmp);
}

time_t prevDisplay = 0; // when the digital clock was displayed

byte h, m, s, old_s;
unsigned long millisSecond;
boolean flip;


// helper routine to display one character from the CP437 font set
void show_cp437_char (byte c)
{
  for (byte x = 0; x < 8; x++)
    OzOled.sendData (pgm_read_byte (&cp437_font [c] [x]));
} // end of show_cp437_char



byte Brightness = 0;

void borderLine (char number) {
  Serial.println("Start border");

  //OzOled.printBigNumber2(number, 5, 1, 1);

  //Minimun instruction for Page Addressing Mode:
  //Set the page start address of the target display location by command B0h to B7h.
  //Set the lower start column address of pointer by command 00h~0Fh.
  //Set the upper start column address of pointer by command 10h~1Fh.
  //OzOled.sendCommand(0xB0); //set page
  //OzOled.sendCommand(0x40); //set lower start column address
  //OzOled.sendCommand(0xB1);

  //OzOled.sendCommand(0x80); //set lower start column address
  //OzOled.sendCommand(0x10); //set upper start column
  //OzOled.setCursorXY(0, 1);
  //    OzOled.sendCommand(0x00 + (8 * 6 & 0x0F));   //set column lower address
  //  OzOled.sendCommand(0x10 + ((8 * 6 >> 4) & 0x0F)); //set column higher address

  //OzOLED.setCursorXY(1, 1);
  //OzOled.setCursorXY(1, 0);

  //  OzOled.sendData(B00000001);
  //  OzOled.sendData(B10000000);
  //  OzOled.sendData(B00000001);

  for (byte i = 0; i < 128; i++) {
    if (i == 0 || i == 127) {
      OzOled.sendData(0xFF);
      //OzOled.sendData(0xFF);
    }
    else {
      OzOled.sendData(B00000001);
      //OzOled.sendData(B10000000);
    }
    Serial.print(i);
    Serial.print(" ");
    //delay(15);
    //i++;
  }
  Serial.println("");

  //  for (byte i = 0; i < 64; i++) {
  //    OzOled.sendCommand(0x40 + i);
  //    delay(30);
  //  }


  OzOled.setCursorXY(0, 7);
  for (byte i = 0; i < 128; i++) {
    if (i == 0 || i == 127) {
      OzOled.sendData(0xFF);
      //OzOled.sendData(0xFF);
    }
    else {
      //OzOled.sendData(B00000001);
      OzOled.sendData(B10000000);
    }
    //delay(15);
    //i++;
  }

  //OzOled.setCursorXY(0, 1);
  for (byte i = 1; i < 7; i++) {
    delay(15);
    OzOled.setCursorXY(0, i);
    OzOled.sendData(B01010101);
  }
  for (byte i = 1; i < 7; i++) {
    delay(15);
    OzOled.setCursorXY(127, i);
    OzOled.sendData(B01010101);
  }



  //Data
  //  for (byte i = 0; i < 3; i++) {
  //    OzOled.sendCommand(0xB0); //set page
  //    OzOled.sendData(B10000001);
  //    //OzOled.sendData(B00000000);
  //    //OzOled.sendData(B00000001);
  //    //OzOled.sendData(B00000000);
  ////OzOled.sendData(0xFF);
  ////OzOled.sendData(0xFF);
  //
  //    OzOled.sendCommand(0xB7); //set page
  ////    OzOled.sendData(B10000000);
  ////    OzOled.sendData(B10000000);
  ////    OzOled.sendData(B10000000);
  ////    OzOled.sendData(B10000000);
  //  }

  //  OzOled.sendCommand(0xB7); //set page
  //  for (byte i = 0; 0 < 128; i++) {
  //    OzOled.sendData(B10000000);
  //  }


  //Minimun instruction for Page Addressing Mode:
  //Set the column start and end address of the target display location by command 21h.
  //Set the page start and end address of the target display location by command 22h.

  //OzOled.setHorizontalMode(); //set page,NOT AVAILABLE FOR SH1106 !

  //  OzOled.sendCommand(0x21);
  //  OzOled.sendCommand(0);
  //  OzOled.sendCommand(0);
  //  OzOled.sendCommand(0x22);
  //  OzOled.sendCommand(0);
  //  OzOled.sendCommand(7);

  //OzOled.sendCommand(0xB0);

  //Data
  //  for (byte i = 0; i < 1; i++) {
  //    OzOled.sendData(0xFF);
  //  }

  //  OzOled.sendCommand(0x21);
  //  //OzOled.sendCommand(127);
  //  //OzOled.sendCommand(127);
  //  OzOled.sendCommand(131);
  //  OzOled.sendCommand(131);
  //  OzOled.sendCommand(0x22);
  //  OzOled.sendCommand(0);
  //  OzOled.sendCommand(7);
  //  for (byte i = 0; i < 8; i++) {
  //    OzOled.sendData(0xFF);
  //  }

}


// set pin numbers:
const int buttonPin = 11;    // the number of the pushbutton pin
const int enterPin = 12;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin


// Variables will change:
boolean ledState = HIGH;         // the current state of the output pin
boolean drawState = HIGH;
boolean buttonState;             // the current reading from the input pin
boolean enterPinState;
boolean lastButtonState = LOW;   // the previous reading from the input pin
boolean lastEnterPinState = LOW;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimeEnterPin = 0;
unsigned long debounceDelay = 25;    // the debounce time; increase if the output flickers



int count;


#ifdef NMEAGPS_INTERRUPT_PROCESSING
static void GPSisr( uint8_t c )
{
  gps.handle( c );
}
#endif

void setup() {

  //Serial.begin(57600);
  Serial.begin(57600);
  while (!Serial)
    ;
  Serial.print( F("NMEAsimple.INO: started\n") );

  gpsPort.begin(9600);

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  gpsPort.attachInterrupt( GPSisr );
#endif

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(enterPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  //set initial LED state
  digitalWrite(ledPin, ledState);

  //setTime(1501442890 + timeZone * SECS_PER_HOUR);

  //setSyncProvider(timezoneTime);  //set function to call when sync required
  //setSyncInterval(10);  // set the number of seconds between re-sync
  Serial.println("Waiting for sync message");

  //OzOled.init();  //initialze Oscar OLED display
  OzOled.init(SSD1306_SWITCHCAPVCC, 0x3C);  //Adafruit init

  OzOled.clearDisplay();               // clear the screen and set start position to top left corner
  //OzOled.setPageMode();                // Set display addressing to page mode. (has to use this, can't use horizontal mode)
  //OzOled.setHorizontalMode();      //Set addressing mode to Horizontal Mode

  //Minimun instruction for Page Addressing Mode:
  //Set the page start address of the target display location by command B0h to B7h.
  //Set the lower start column address of pointer by command 00h~0Fh.
  //Set the upper start column address of pointer by command 10h~1Fh.


  //B0~B7, Set GDDRAM Page Start Address (PAGE0~PAGE7) for Page Addressing Mode

  //OzOled.sendCommand(0xA7);

  if (0) {
    borderLine('5');


    delay(1000);

    OzOled.drawFont8("               ", 12, 0);
    OzOled.drawFont8("TESTING", 40, 0);
    OzOled.drawLogo2("01", 2, 64, 3, 2, 3);

    delay(2000);

    //change MULTIPLEX
    OzOled.sendCommand(0xA8);
    OzOled.sendCommand(0x3F - 32);

    delay(2000);

    //change OFFSET
    OzOled.sendCommand(0xD3);
    OzOled.sendCommand(0x00 + 48);

    delay(2000);

    byte STARTLINE = (0x40 + 48);
    OzOled.sendCommand(STARTLINE);

    while (true);



    delay(1000);

    //change STARTLINE
    OzOled.drawFont8("VARY START LINE", 12, 0);
    OzOled.drawLogo2("92", 2, 64, 3, 2, 3);
    delay(1000);

    Serial.println("Vary Start Line");

    //Varying Display Start Line: (40H - 7FH)
    //Command no.4
    //When this command changes the line address, the smooth scrolling or page change takes place.
    //Default = 40H
    for (int i = 0; i < 64; i++) {
      delay(25);
      byte STARTLINE = (0x40 + i);
      OzOled.sendCommand(STARTLINE);
      Serial.println(STARTLINE, HEX);
    }
    Serial.println("********");
    delay(2000);
    OzOled.drawLogo2("88", 2, 64, 3, 2, 3);
    delay(2000);
    for (int i = 0; i < 64; i++) {
      delay(25);
      byte STARTLINE = (0x7F - i);
      OzOled.sendCommand(STARTLINE);
      Serial.println(STARTLINE, HEX);
    }
    Serial.println("********");


    delay(1000);
    OzOled.drawFont8("               ", 12, 0);
    OzOled.drawFont8("VARY MULTIPLEX", 16, 0);
    OzOled.drawLogo2("57", 2, 64, 3, 2, 3);
    delay(1000);

    //Varying Multiplex Ration: (Double Bytes Command)
    //Command no.9
    //Multiplex Ration Mode Set: (A8H)
    //Multiplex Ration Data Set: (00H - 3FH)
    //Default: 0x3F
    for (int i = 0; i < 64; i++) {
      delay(25);
      OzOled.sendCommand(0xA8);
      OzOled.sendCommand(0x3F - i);
      Serial.println(i);
    }
    for (int i = 0; i < 64; i++) {
      delay(25);
      OzOled.sendCommand(0xA8);
      OzOled.sendCommand(0x00 + i);
      Serial.println(i);
    }


    delay(1000);
    OzOled.drawFont8("               ", 12, 0);
    OzOled.drawFont8("VARY OFFSET", 26, 0);
    OzOled.drawLogo2("39", 2, 64, 3, 2, 3);
    delay(1000);

    //Varying Display Offset: (Double Bytes Command)
    //Command no. 14
    //Display Offset Mode Set: (D3H)
    //Display Offset Data Set: (00H~3FH)
    //Default: 0x00
    for (int i = 0; i < 64; i++) {
      delay(25);
      OzOled.sendCommand(0xD3);
      OzOled.sendCommand(0x00 + i);
      Serial.println(i);
    }
    for (int i = 0; i < 64; i++) {
      delay(25);
      OzOled.sendCommand(0xD3);
      OzOled.sendCommand(0x3F - i);
      Serial.println(i);
    }

    //OzOled.drawBitmap(OscarLogo, 0, 0, 16, 8);
    //OzOled.sendCommand(0x71);
    //OzOled.drawBitmap(menu_torch, 6, 2, 4, 4);
    //OzOled.drawBitmap(menu_settings, 11, 2, 4, 4);

    //OzOled.setPageMode();
    //  //OzOled.setHorizontalMode();
    //  OzOled.setCursorXY(64,0);
    //borderLine('5');
    //  OzOled.setCursorXY(64,1);
    //  borderLine("5");
    //OzOled.drawLogo(locationBMP, 48, 32, 2,4);
    //OzOled.setHorizontalMode();

    //delay(1000);

    while (true);
  }

}

long value = 977;
long value_min = 977;
long value_max = 999;
int x = 800;

boolean t = false;
long a;

unsigned long blinkTimer;
boolean
boolShow,
boolShowOld,
colonON,
colonON_old;

char h_old[1];
char m_old[1];
char h_digit_1_old[1], h_digit_2_old[1], m_digit_1_old[1], m_digit_2_old[1], s_digit_1_old[1], s_digit_2_old[1];


unsigned long previousMillis;
unsigned long timer_500ms;
unsigned long timer_500ms_old;
unsigned long timer_1s;
unsigned long timer_1s_old;

const long interval = 500;


byte columnStart;
byte rowStart = 0xB7;

byte increment;

byte mux = 16; //normal value is 63
boolean muxHIGH = LOW;

byte offset = 47; //normal value is 0
boolean offsetHIGH = HIGH;

byte startline = 0x6F; //min value is 0x40(0); max 0x7F(63)
boolean startlineHIGH = HIGH;



byte PAGE;
byte PAGE_old = 100;
byte MENU;
boolean UPDATE = true;
boolean UPDATE_TIME = true;
boolean UPDATE_SHOLAT = true;
boolean FORCE_UPDATE_TIME;
boolean FORCE_UPDATE_SHOLAT;
int CURRENTTIMEID_old = 100;
int NEXTTIMEID_old = 100;
int HOUR_old = 100;
int MINUTE_old = 100;


boolean GPS = true;
boolean OLED = true;



void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}



//--------------------------
// Set these values to the offset of your timezone from GMT

static const int32_t          zone_hours   = 7L; // EST
static const int32_t          zone_minutes =  0L; // usually zero
static const NeoGPS::clock_t  zone_offset  =
  zone_hours   * NeoGPS::SECONDS_PER_HOUR +
  zone_minutes * NeoGPS::SECONDS_PER_MINUTE;

// Uncomment one DST changeover rule, or define your own:
//#define USA_DST
//#define EU_DST

//#if defined(USA_DST)
//static const uint8_t springMonth =  3;
//static const uint8_t springDate  = 14; // latest 2nd Sunday
//static const uint8_t springHour  =  2;
//static const uint8_t fallMonth   = 11;
//static const uint8_t fallDate    =  7; // latest 1st Sunday
//static const uint8_t fallHour    =  2;
//#define CALCULATE_DST
//
//#elif defined(EU_DST)
//static const uint8_t springMonth =  3;
//static const uint8_t springDate  = 31; // latest last Sunday
//static const uint8_t springHour  =  1;
//static const uint8_t fallMonth   = 10;
//static const uint8_t fallDate    = 31; // latest last Sunday
//static const uint8_t fallHour    =  1;
//#define CALCULATE_DST
//#endif

void localTime( NeoGPS::time_t & dt )
{
  NeoGPS::clock_t seconds = dt; // convert date/time structure to seconds

#ifdef CALCULATE_DST
  //  Calculate DST changeover times once per reset and year!
  static NeoGPS::time_t  changeover;
  static NeoGPS::clock_t springForward, fallBack;

  if ((springForward == 0) || (changeover.year != dt.year)) {

    //  Calculate the spring changeover time (seconds)
    changeover.year    = dt.year;
    changeover.month   = springMonth;
    changeover.date    = springDate;
    changeover.hours   = springHour;
    changeover.minutes = 0;
    changeover.seconds = 0;
    changeover.set_day();
    // Step back to a Sunday, if day != SUNDAY
    changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
    springForward = (NeoGPS::clock_t) changeover;

    //  Calculate the fall changeover time (seconds)
    changeover.month   = fallMonth;
    changeover.date    = fallDate;
    changeover.hours   = fallHour - 1; // to account for the "apparent" DST +1
    changeover.set_day();
    // Step back to a Sunday, if day != SUNDAY
    changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
    fallBack = (NeoGPS::clock_t) changeover;
  }
#endif

  //  First, offset from UTC to the local timezone
  seconds += zone_offset;

#ifdef CALCULATE_DST
  //  Then add an hour if DST is in effect
  if ((springForward <= seconds) && (seconds < fallBack))
    seconds += NeoGPS::SECONDS_PER_HOUR;
#endif

  dt = seconds; // convert seconds back to a date/time structure

} // adjustTime


void requestSync()
{
  //DEBUG_PORT.println(F( "Sync time with GPS..." ));

  while (gps.available(gpsPort)) {
    fix = gps.read();

    DEBUG_PORT.print( F("Status: ") );
    if (fix.valid.status) {
      DEBUG_PORT.print( fix.status );
    }

    DEBUG_PORT.print( F(", Location: ") );
    if (fix.valid.location) {
      DEBUG_PORT.print( fix.latitude(), 6 );
      DEBUG_PORT.print( ',' );
      DEBUG_PORT.print( fix.longitude(), 6 );
    }

    DEBUG_PORT.print( F(", Altitude: ") );
    if (fix.valid.altitude)
      DEBUG_PORT.print( fix.altitude() );

    if (fix.valid.time && fix.valid.date) {
      localTime( fix.dateTime );

      DEBUG_PORT.print( F(", DateTime: ") );
      DEBUG_PORT << fix.dateTime;

      setTime(
        fix.dateTime.hours,
        fix.dateTime.minutes,
        fix.dateTime.seconds,
        fix.dateTime.date,
        fix.dateTime.month,
        fix.dateTime.year
      );
    }

    DEBUG_PORT.println();
  }
}





//--------------------------

void loop() {

  //  if (timeStatus() == timeNotSet) {
  //    requestSync();
  //  }

  //************************************//
  //                GPS                 //
  //************************************//
  //Serial.print(GPS);

  if (GPS) {

    requestSync();

    /*
      while (gps.available(gpsPort)) {
      fix = gps.read();

      DEBUG_PORT.print( F("Status: ") );
      if (fix.valid.status) {
        DEBUG_PORT.print( fix.status );
      }

      DEBUG_PORT.print( F(", Location: ") );
      if (fix.valid.location) {
        DEBUG_PORT.print( fix.latitude(), 6 );
        DEBUG_PORT.print( ',' );
        DEBUG_PORT.print( fix.longitude(), 6 );
      }

      DEBUG_PORT.print( F(", Altitude: ") );
      if (fix.valid.altitude)
        DEBUG_PORT.print( fix.altitude() );

      DEBUG_PORT.print( F(", Time: ") );
      if (fix.valid.time) {
        DEBUG_PORT.print( fix.dateTime.hours );
        DEBUG_PORT.print( ':' );
        DEBUG_PORT.print( fix.dateTime.minutes );
        DEBUG_PORT.print( ':' );
        DEBUG_PORT.print( fix.dateTime.seconds );
      }

      DEBUG_PORT.println();
      }

      if (fix.valid.time) {
      setTime(
        fix.dateTime.hours,
        fix.dateTime.minutes,
        fix.dateTime.seconds,
        fix.dateTime.day,
        fix.dateTime.month,
        fix.dateTime.year
      );
      adjustTime(timeZone * SECS_PER_HOUR);

      GPS = false;
      }
    */

    GPS = false;

  }

  //************************************//
  //              SHOLAT                //
  //************************************//

  else {

    GPS = true;

    //************************************//
    //          READING PIN 1             //
    //************************************//

    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:

      // if the button state has changed:
      if (reading != buttonState) {
        buttonState = reading;

        // only toggle the LED if the new button state is HIGH
        if (buttonState == LOW) {
          ledState = !ledState;
          drawState = !drawState;
          MENU++;
          if (MENU > 6) {
            MENU = 0;
          }
          UPDATE = true;
        }
      }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;


    //************************************//
    //        READING ENTER PIN           //
    //************************************//

    int readingEnterPin = digitalRead(enterPin);

    if (readingEnterPin != lastEnterPinState) {
      lastDebounceTimeEnterPin = millis();
    }

    if ((millis() - lastDebounceTimeEnterPin) > debounceDelay) {
      if (readingEnterPin != enterPinState) {
        enterPinState = readingEnterPin;

        // only toggle the LED if the new button state is HIGH
        if (enterPinState == LOW) {
          ledState = !ledState;
          //drawState = !drawState;
          PAGE++;
          OzOled.clearDisplay();
          if (PAGE > 1) {
            PAGE = 0;
          }
          UPDATE = true;

          if (PAGE == 0) {
            FORCE_UPDATE_TIME = true;
            FORCE_UPDATE_SHOLAT = true;
          }
        }
      }
    }

    lastEnterPinState = readingEnterPin;

    // set the LED:
    //digitalWrite(ledPin, ledState);


    //************************************//
    //          READING SERIAL            //
    //************************************//

    //    //if (Serial.available()) {
    //    while (Serial.available() > 0) {
    //      //Serial.println("Serial available!");
    //      processSyncMessage();
    //    }
    //    if (timeStatus() != timeNotSet) {
    //      //digitalClockDisplay();
    //    }
    //    if (timeStatus() == timeSet) {
    //      //digitalWrite(13, HIGH); // LED on if synced
    //    } else {
    //      //digitalWrite(13, LOW);  // LED off if needs refresh
    //    }
    //delay(1000);


    boolean SHOW = false;
    unsigned long millisNow = millis();

    //************************************//
    //              TEST                  //
    //************************************//



    //  OzOled.drawLogo(menu_settings, increment, 32, 0, 4);
    //  OzOled.eraseLogo(increment, 32, 0, 4);
    //  increment = increment + 32;
    //  if (increment > 127) {
    //    increment = 0;
    //  }


    //Menu sequence:
    //Alarm, Flashlight, Stopwatch, Games, Settings, Diagnostics, Exit
    if (PAGE == 1 && MENU == 0 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.eraseLogo(0, 32, 2, 4);
      OzOled.drawBitmap(menu_alarm, 6, 2, 4, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_torch, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Alarm     ", 0, 7);

      UPDATE = false;
    }
    if (PAGE == 1 && MENU == 1 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_alarm, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_torch, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_stopwatch, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Flashlight", 0, 7);

      UPDATE = false;
    }

    if (PAGE == 1 && MENU == 2 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_torch, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_stopwatch, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_games, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Stopwatch ", 0, 7);

      UPDATE = false;
    }

    if (PAGE == 1 && MENU == 3 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_stopwatch, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_games, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_settings, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Games     ", 0, 7);

      UPDATE = false;
    }

    if (PAGE == 1 && MENU == 4 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_games, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_settings, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_diagnostic, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Settings", 0, 7);

      UPDATE = false;
    }

    if (PAGE == 1 && MENU == 5 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_settings, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_diagnostic, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      OzOled.drawLogo(menu_exit, 95, 32, 2, 4);
      //OzOled.eraseLogo(112, 32, 0, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Diagnostic", 0, 7);

      UPDATE = false;
    }

    if (PAGE == 1 && MENU == 6 && UPDATE) {

      //title
      OzOled.drawFont8("< MAIN MENU >", 17, 0);

      OzOled.drawLogo(menu_diagnostic, 0, 32, 2, 4);
      //OzOled.eraseLogo(-16, 32, 0, 4);
      OzOled.drawLogo(menu_exit, 48, 32, 2, 4);
      //OzOled.eraseLogo(48, 32, 0, 4);
      //OzOled.drawLogo(menu_exit, 95, 32, 2, 4);
      OzOled.eraseLogo(95, 32, 2, 4);
      OzOled.drawLogo(selectbar_top, 46, 36, 1, 1);
      OzOled.drawLogo(selectbar_bottom, 46, 36, 6, 1);

      //menu text
      OzOled.drawFont8("Exit      ", 0, 7);

      UPDATE = false;
    }

    //OzOled.drawFont8("Mantap lah boss..!?", 0, 5, 5, 1);
    //OzOled.drawFont8("885", 0, 6);
    //OzOled.drawFont8(">Back jqQ", 0, 5, 7, 1);
    //char buf[2];
    //dtostrf(s * 14, 4, 0, buf);
    //OzOled.drawFont8(buf, 0, 7);



    //************************************//
    //              COLON                 //
    //************************************//

    if (PAGE == 0 && UPDATE) {

      //blink colon symbol every 0.5 second)
      if (millisNow >= timer_500ms_old + 500) {

        // reset timer
        timer_500ms_old = millisNow;

        //colonON = !colonON;

      }

      if (colonON != colonON_old && colonON_old == false) {
        //OzOled.drawLogo2(":", 1, 45, 4, 3, 2);
        OzOled.drawLogo2(":", 1, 41, 3, 2, 3);
      }
      else if (colonON != colonON_old && colonON_old == true) {
        //OzOled.drawLogo2("A", 1, 45, 4, 3, 2);
        OzOled.drawLogo2(" ", 1, 41, 3, 2, 3);
      }
      // update the status

      colonON_old = colonON;
    }

    //************************************
    //     TIME
    //************************************

    s = second();

    if (s != old_s) {
      old_s = s;
      UPDATE_TIME = true;
      UPDATE_SHOLAT = true;

    }

    if (PAGE == 0 && UPDATE_TIME) {

      h = hour();
      m = minute();


      char buf[2];
      char h_digit_1[2], h_digit_2[2], m_digit_1[2], m_digit_2[2], s_digit_1[2], s_digit_2[2];

      //display SECOND buffer
      if (s < 10) {
        //1st digit
        s_digit_1[0] = '0';

        //2nd digit
        dtostrf(s, 1, 0, buf);
        s_digit_2[0] = buf[0];
      }
      else {
        dtostrf(s, 2, 0, buf);
        //1st digit
        s_digit_1[0] = buf[0];

        //2nd digit
        s_digit_2[0] = buf[1];
      }

      if (s_digit_2[0] != s_digit_2_old[0] || FORCE_UPDATE_TIME) {

        //      OzOled.drawLogo2("0", 1, 0, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 21, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 42, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 63, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 84, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 105, 19, 2, 4);
        s_digit_2_old[0] = s_digit_2[0];
        //OzOled.drawFont16(s_digit_2, 1, 111, 12, 4, 2);
        OzOled.drawLogo2(s_digit_2, 1, 111, 17, 2, 3);
        timer_500ms_old = millisNow;
        colonON = true;
        colonON_old = !colonON;
      }
      if (s_digit_1[0] != s_digit_1_old[0] || FORCE_UPDATE_TIME) {
        s_digit_1_old[0] = s_digit_1[0];
        //OzOled.drawFont16(s_digit_1, 1, 98, 12, 4, 2);
        OzOled.drawLogo2(s_digit_1, 1, 91, 17, 2, 3);
        //colonON = false;
      }

      //formatting the MINUTE buffer
      if (m < 10) {
        //1st digit
        m_digit_1[0] = '0';

        //2nd digit
        dtostrf(m, 1, 0, buf);
        m_digit_2[0] = buf[0];
      }
      else {
        dtostrf(m, 2, 0, buf);
        //1st digit
        m_digit_1[0] = buf[0];

        //2nd digit
        m_digit_2[0] = buf[1];
      }

      if (m_digit_2[0] != m_digit_2_old[0] || FORCE_UPDATE_TIME) {

        //      OzOled.drawLogo2("0", 1, 0, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 21, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 42, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 63, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 84, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 105, 19, 2, 4);
        m_digit_2_old[0] = m_digit_2[0];
        //OzOled.drawLogo2(m_digit_2, 1, 73, 21, 2, 4);
        OzOled.drawLogo2(m_digit_2, 1, 68, 17, 2, 3);
      }
      if (m_digit_1[0] != m_digit_1_old[0] || FORCE_UPDATE_TIME) {

        m_digit_1_old[0] = m_digit_1[0];
        //OzOled.drawLogo2(m_digit_1, 1, 52, 21, 2, 4);
        OzOled.drawLogo2(m_digit_1, 1, 48, 17, 2, 3);
      }

      //formatting the HOUR buffer
      if (h < 10) {
        //1st digit
        h_digit_1[0] = '0';

        //2nd digit
        dtostrf(h, 1, 0, buf);
        h_digit_2[0] = buf[0];
      }
      else {
        dtostrf(h, 2, 0, buf);
        //1st digit
        h_digit_1[0] = buf[0];

        //2nd digit
        h_digit_2[0] = buf[1];
      }

      if (h_digit_2[0] != h_digit_2_old[0] || FORCE_UPDATE_TIME) {

        //      OzOled.drawLogo2("0", 1, 0, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 21, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 42, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 63, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 84, 19, 2, 4);
        //      OzOled.drawLogo2("0", 1, 105, 19, 2, 4);
        h_digit_2_old[0] = h_digit_2[0];
        //OzOled.drawLogo2(h_digit_2, 1, 21, 19, 2, 4);
        //OzOled.drawLogo2(h_digit_2, 1, 21, 21, 2, 4);
        OzOled.drawLogo2(h_digit_2, 1, 20, 17, 2, 3);
      }
      if (h_digit_1[0] != h_digit_1_old[0] || FORCE_UPDATE_TIME) {

        h_digit_1_old[0] = h_digit_1[0];
        //OzOled.drawLogo2(h_digit_1, 1, 0, 19, 2, 4);
        //OzOled.drawLogo2(h_digit_1, 1, 0, 21, 2, 4);
        //OzOled.drawLogo2(h_digit_1, 1, 0, 21, 2, 4);
        OzOled.drawLogo2(h_digit_1, 1, 0, 17, 2, 3);
      }

      UPDATE_TIME = false;
      FORCE_UPDATE_TIME = false;

    }


    //************************************
    //     SHOLAT MENU
    //************************************

    if (PAGE == 0 && PAGE != PAGE_old) {
      PAGE_old = PAGE;
      OzOled.drawFont8("hr", 80, 0);
      OzOled.drawFont8("min", 111, 0);
    }

    //************************************
    //     CALCULATE SHOLAT TIME
    //************************************


    if (UPDATE_SHOLAT || FORCE_UPDATE_SHOLAT) {

      int timezone = 7;
      int CURRENTTIMEID, NEXTTIMEID;
      int hoursNext, minutesNext;

      set_calc_method(Custom);
      set_asr_method(Shafii);
      set_high_lats_adjust_method(AngleBased);
      set_fajr_angle(20);
      set_isha_angle(18);

      //BEKASI
      float latitude = -6.26526191;
      float longitude = 106.97298512;
      //get prayer time on the current year, month and day
      get_prayer_times(year(), month(), day(), latitude, longitude, timezone, times);

      for (int i = 0; i < sizeof(times) / sizeof(double); i++) {
        char tmp[10];
        int hours, minutes;
        get_float_time_parts(times[i], hours, minutes);
        p("%d \t %10s %s \t %02d:%02d \n\r", i, TimeName[i], dtostrf(times[i], 2, 2, tmp), hours, minutes);
      }

      for (int i = 0; i < sizeof(times) / sizeof(double); i++) {


        float timeNow = hour() / 1.0 + minute() / 60.0;
        float timeA, timeB;
        int hours, minutes;
        get_float_time_parts(times[i], hours, minutes);
        timeA = times[i];
        //timeA = times[i] + 2.0 / 60;
        CURRENTTIMEID = i;
        if (CURRENTTIMEID == 4) {
          CURRENTTIMEID = 3;
        }
        if (i != 6) {
          get_float_time_parts(times[i + 1], hoursNext, minutesNext);
          timeB = times[i + 1];
          //timeB = times[i + 1]  +  2.0 / 60;
          NEXTTIMEID = i + 1;
          if (NEXTTIMEID == 4) {
            NEXTTIMEID = 5;
          }
        }
        else if (i == 6) {
          get_float_time_parts(times[0], hoursNext, minutesNext);
          timeB = times[0];
          //timeB = times[0] + 2.0 / 60;
          NEXTTIMEID = 0;
        }

        if (timeB > timeA) {
          //Serial.println("case A");
          if (timeA < timeNow && timeNow < timeB) {
            //Serial.println("case A1");
            CURRENTTIMENAME = TimeName[CURRENTTIMEID];
            NEXTTIMENAME = TimeName[NEXTTIMEID];

            //extract hour and minute from time difference
            get_float_time_parts(time_diff(timeNow, timeB), HOUR, MINUTE);

            break;
          }
        }

        else if (timeB < timeA) {
          //Serial.println("case B");
          if ( (timeA < timeNow && timeNow < 24) || (0 <= timeNow && timeNow < timeB) ) {
            //Serial.println("case B1");
            CURRENTTIMENAME = TimeName[CURRENTTIMEID];
            NEXTTIMENAME = TimeName[NEXTTIMEID];

            //extract hour and minute from time difference
            get_float_time_parts(time_diff(timeNow, timeB), HOUR, MINUTE);

            break;
          }
        }
      }//end of for loop

      // print the current time
      digitalClockDisplay();


      Serial.print(F("NEXTTIMENAME "));
      Serial.println(NEXTTIMENAME);
      Serial.print(F("String length "));
      Serial.println(NEXTTIMENAME.length());

      if (NEXTTIMEID != NEXTTIMEID_old) {
        NEXTTIMEID_old = NEXTTIMEID;
        int len = NEXTTIMENAME.length() + 1;
        char buf[len];
        NEXTTIMENAME.toCharArray(buf, len);
        OzOled.drawFont8("       ", 12, 0);
        OzOled.drawFont8(buf, 0, 0);
      }

      //int hoursNext, minutesNext;



      if (HOUR != HOUR_old) {
        HOUR_old = HOUR;

        char bufHOUR[2];
        dtostrf(HOUR, 2, 0, bufHOUR);

        OzOled.drawFont8(bufHOUR, 66, 0);

      }

      if (MINUTE != MINUTE_old) {
        MINUTE_old = MINUTE;

        char bufMINUTE[2];
        dtostrf(MINUTE, 2, 0, bufMINUTE);

        OzOled.drawFont8(bufMINUTE, 97, 0);

      }

      time_t t = now();
      isAM(t);


      UPDATE_SHOLAT = false;
      FORCE_UPDATE_SHOLAT = false;

    }



    //
    //
    //
    //
    //
    //  //Set Display Offset (D3h)
    //  //Set vertical shift by COM from 0d~63d. The value is reset to 00h after RESET.
    //  byte offsetON = 0;
    //
    //  if (offsetON) {
    //    if (offset == 47) {
    //      offsetHIGH = HIGH;
    //      mux = 16;
    //      muxHIGH = LOW;
    //      startline = 0x6F;
    //      startlineHIGH = HIGH;
    //    }
    //    if (offset == 0) {
    //      offsetHIGH = LOW;
    //      mux = 63;
    //      muxHIGH = HIGH;
    //      startline = 0x40;
    //      startlineHIGH = LOW;
    //    }
    //    byte offsetIncrement = 1;
    //    if (offsetHIGH == HIGH) {
    //      offset = offset - offsetIncrement;
    //    }
    //    else {
    //      offset = offset + offsetIncrement;
    //    }
    //
    //
    //    OzOled.sendCommand(0xD3);
    //    OzOled.sendCommand(offset);
    //  }
    //  else {
    ////    offset = 0;
    ////    OzOled.sendCommand(0xD3);
    ////    OzOled.sendCommand(offset);
    //  }
    //
    //  //Set MUX ratio(A8h)
    //  //This command switches the default 63 multiplex mode to any multiplex ratio
    //  //ranging from 16 to 63. The output pads COM0~COM63 will be switched
    //  //to the corresponding COM signal.
    //  //Set MUX ratio to N+1 MUX
    //  //N=A[5:0] : from 16MUX to 64MUX, RESET=111111b (i.e. 63d, 64MUX)
    //  //A[5:0] from 0 to 14 are invalid entry.
    //
    //  byte muxON = 0;
    //
    //  if (muxON == 1) {
    //    if (mux == 63) {
    //      muxHIGH = HIGH;
    //    }
    //    if (mux == 16) {
    //      muxHIGH = LOW;
    //    }
    //    byte muxIncrement = 1;
    //    if (muxHIGH == HIGH) {
    //      mux = mux - muxIncrement;
    //    }
    //    else {
    //      mux = mux + muxIncrement;
    //    }
    //
    //    // Mux Command
    //    OzOled.sendCommand(0xA8);
    //    OzOled.sendCommand(mux);
    //  }
    //  else {
    //    // Mux Command
    //    mux = 63;
    ////    OzOled.sendCommand(0xA8);
    ////    OzOled.sendCommand(mux);
    //  }
    //
    //
    //
    //  //Set Display start line (40h - 7Fh) or 64d - 127d
    //  //Display start line register is reset to 000000b during RESET.
    //  byte startlineON = 0;
    //
    //  if (startlineON) {
    //    if (startline == 0x78) {
    //      startlineHIGH = HIGH;
    //    }
    //    if (startline == 0x40) {
    //      startlineHIGH = LOW;
    //    }
    //    byte startlineIncrement = 1;
    //    if (startlineHIGH == HIGH) {
    //      startline = startline - startlineIncrement;
    //    }
    //    else {
    //      startline = startline + startlineIncrement;
    //    }
    //
    //    OzOled.sendCommand(startline);
    //  }
    //  else {
    ////    startline = 0x40;
    ////    OzOled.sendCommand(startline);
    //  }
    //
    //
    //  /*
    //    Serial.print("mux=");
    //    Serial.print(mux);
    //    Serial.print(" ");
    //    Serial.print("offset=");
    //    Serial.print(offset);
    //    Serial.print(" ");
    //    Serial.print("startline=");
    //    Serial.print(" ");
    //    Serial.println(startline);
    //  */
    //
    //
    //  if (offset == 0) {
    //
    //    //borderLine("2");
    //    //delay(500);
    //    //offset = 47;
    //    //mux = 16;
    //    //startline = 0x6F;
    //
    //    //OzOled.sendCommand(0xD3);
    //    //OzOled.sendCommand(0);
    //    //while (true);
    //  }
    //  else if (offset == 56) {
    //    //borderLine("3");
    //  }
    //
    //
    //
    //  //if (drawState) {
    //  if (false) {
    //    count++;
    //    if (count % 2) {
    //      OzOled.drawLogo(locationBMP, 8, 32, 2, 4);
    //      OzOled.drawLogo(homeBMP, 48, 32, 2, 4);
    //      OzOled.eraseLogo(88, 32, 2, 4);
    //    }
    //    else {
    //      OzOled.eraseLogo(8, 32, 2, 4);
    //      OzOled.drawLogo(locationBMP, 48, 32, 2, 4);
    //      OzOled.drawLogo(homeBMP, 88, 32, 2, 4);
    //    }
    //    //delay(100);
    //    //OzOled.drawLogo(locationBMP, 8, 32, 2, 4);
    //    //delay(50);
    //    //OzOled.eraseLogo(48, 32, 2,4);
    //    //OzOled.drawLogo(homeBMP, 48, 32, 2, 4);
    //    //delay(50);
    //    //OzOled.eraseLogo(88, 32, 2,4);
    //    drawState = LOW;
    //  }
    //
    //
    //  OzOled.setBrightness(120);
  }


}








