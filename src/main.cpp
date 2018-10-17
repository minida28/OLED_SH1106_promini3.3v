#include <Arduino.h>

#include "sholathelper.h"
#include "timehelper.h"
#include "gpshelper.h"

#include <Wire.h>
#include "OzOLED.h"
// pull in extra font characters

#include "cp437_font.h"
#include "bigNumbers2.h"
#include "resources.h"
//#include "font32.h"

#include "buzzer.h"

#include <EEPROMex.h>

// EEPROM address to store pressure & motor current settings
int mem_address_timezone;
int mem_address_latitude;
int mem_address_longitude;

// If somehow EEPROM was cleared or erased, apply safe configs
float default_timezone = configLocation.timezone;
float default_latitude = configLocation.latitude;
float default_longitude = configLocation.longitude;

void updateEEPROM()
{
  EEPROM.updateFloat(mem_address_timezone, configLocation.timezone);
  EEPROM.updateFloat(mem_address_latitude, configLocation.latitude);
  EEPROM.updateFloat(mem_address_longitude, configLocation.longitude);
}

const char pgm_timezone[] PROGMEM = "timezone";
const char pgm_latitude[] PROGMEM = "latitude";
const char pgm_longitude[] PROGMEM = "longitude";

const char *const PGM_VAR_EEPROM[] PROGMEM =
    {
        pgm_timezone,
        pgm_latitude,
        pgm_longitude,
};

void PRINT_EEPROM_SETTINGS()
{
  /*
      Serial.println(F("----------------------------------------------"));
      Serial.println(F("    Following adresses have been allocated    "));
      Serial.println(F("----------------------------------------------"));
  */

  //Serial.println(F("\r\nVARIABLE \t\t VALUE \t ADDRESS \t SIZE\r\n"));

  int varNum = 3;

  for (int8_t i = 0; i < varNum; i++)
  {
    const char *ptr = (char *)pgm_read_word(&(PGM_VAR_EEPROM[i]));
    uint8_t len = strlen_P(ptr);
    char var[len + 1];
    strcpy_P(var, ptr);

    char str[12]; // -107.123456
    uint16_t address = 0;
    uint16_t size = 0;
    if (i == 0)
    {
      dtostrf(EEPROM.readFloat(mem_address_timezone), 11, 2, str);
      address = mem_address_timezone;
      size = sizeof(float);
    }
    else if (i == 1)
    {
      dtostrf(EEPROM.readFloat(mem_address_latitude), 11, 7, str);
      address = mem_address_latitude;
      size = sizeof(float);
    }
    else if (i == 2)
    {
      dtostrf(EEPROM.readFloat(mem_address_longitude), 11, 7, str);
      address = mem_address_longitude;
      size = sizeof(float);
    }

    char buf[64];
    snprintf_P(buf, sizeof(buf), PSTR("%-20s \t %s \t %d \t\t %d\r\n"), var, str, address, size);
    Serial.print(buf);
  }
  Serial.println();
}

void EEPROMsetup()
{
  /*--- some variables used by EEPROMex library ---*/
  const int maxAllowedWrites = 10;
  const int memBase = 0;

  // start reading from position memBase (address 0) of the EEPROM. Set maximumSize to EEPROMSizeUno
  // Writes before membase or beyond EEPROMSizeUno will only give errors when _EEPROMex_DEBUG is set
  EEPROM.setMemPool(memBase, EEPROMSizeATmega328);

  EEPROM.setMaxAllowedWrites(maxAllowedWrites);

  /*--- Always get the adresses first and in the same order ---*/
  mem_address_timezone = EEPROM.getAddress(sizeof(float));
  mem_address_latitude = EEPROM.getAddress(sizeof(float));
  mem_address_longitude = EEPROM.getAddress(sizeof(float));

  // PRINT_EEPROM_SETTINGS();
  // updateEEPROM();

  configLocation.timezone = EEPROM.readFloat(mem_address_timezone);
  configLocation.latitude = EEPROM.readFloat(mem_address_latitude);
  configLocation.longitude = EEPROM.readFloat(mem_address_longitude);
}

// #define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
Encoder myEnc(3, 4);

// double times[sizeof(TimeName) / sizeof(char *)];

// void p(char *fmt, ... ) {
void DEBUG(const char *fmt, ...)
{
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, 128, fmt, args);
  va_end(args);
  Serial.print(tmp);
}

time_t prevDisplay = 0; // when the digital clock was displayed

uint8_t h, m, s;

// helper routine to display one character from the CP437 font set
void show_cp437_char(byte c)
{
  for (byte x = 0; x < 8; x++)
    OzOled.sendData(pgm_read_byte(&cp437_font[c][x]));
} // end of show_cp437_char

// byte Brightness = 0;

void borderLine(char number)
{
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

  for (byte i = 0; i < 128; i++)
  {
    if (i == 0 || i == 127)
    {
      OzOled.sendData(0xFF);
      //OzOled.sendData(0xFF);
    }
    else
    {
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
  for (byte i = 0; i < 128; i++)
  {
    if (i == 0 || i == 127)
    {
      OzOled.sendData(0xFF);
      //OzOled.sendData(0xFF);
    }
    else
    {
      //OzOled.sendData(B00000001);
      OzOled.sendData(B10000000);
    }
    //delay(15);
    //i++;
  }

  //OzOled.setCursorXY(0, 1);
  for (byte i = 1; i < 7; i++)
  {
    delay(15);
    OzOled.setCursorXY(0, i);
    OzOled.sendData(B01010101);
  }
  for (byte i = 1; i < 7; i++)
  {
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

int I2C_ClearBus()
{
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  // Wait 2.5 secs, i.e. delay(2500). This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.
  // Serial.println(F("Delay 2.5 secs to allow DS3231 module to initialize properly"));
  // DEBUGLOG("Delay 2.5 secs to allow DS3231 module to initialize properly");
  delay(2500);
  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW)
  {           //If it is held low Arduno cannot become the I2C master.
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW); // vi. Check SDA input.
  int clockCount = 20;                         // > 2x9 clock

  while (SDA_LOW && (clockCount > 0))
  { //  vii. If SDA is Low,
    clockCount--;
    // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT);        // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT);       // then clock SCL Low
    delayMicroseconds(10);      //  for >5uS
    pinMode(SCL, INPUT);        // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0))
    { //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW)
    {           // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW)
  {           // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT);  // remove pullup.
  pinMode(SDA, OUTPUT); // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10);      // wait >5uS
  pinMode(SDA, INPUT);        // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10);      // x. wait >5uS
  pinMode(SDA, INPUT);        // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}

// set pin numbers:
// const byte leftPin = 2;
// const byte rightPin = 3;
const byte switchPin = 2;  // the number of the pushbutton pin
const byte ledPin = 13;    // the number of the LED pin
const byte buzzerPin = 10; // the number of the pushbutton pin

// // Variables will change:
// // boolean ledState = HIGH; // the current state of the output pin
// boolean drawState = HIGH;
// boolean buttonState; // the current reading from the input pin
// boolean enterPinState;
// boolean lastButtonState = LOW;   // the previous reading from the input pin
// boolean lastEnterPinState = LOW; // the previous reading from the input pin

// // the following variables are unsigned long's because the time, measured in miliseconds,
// // will quickly become a bigger number than can be stored in an int.
// unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
// unsigned long lastDebounceTimeEnterPin = 0;
// unsigned long debounceDelay = 25; // the debounce time; increase if the output flickers

int count;

#ifdef NMEAGPS_INTERRUPT_PROCESSING
static void GPSisr(uint8_t c)
{
  gps.handle(c);
}
#endif

void TestTone()
{

  // // notes in the melody:
  // int melody[] = {
  //     NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

  // // note durations: 4 = quarter note, 8 = eighth note, etc.:
  // int noteDurations[] = {
  //     4, 8, 8, 4, 4, 4, 4, 4};

  int melody[] = {
      4000, 0, 4000, 0, 4000};

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {
      80, 75, 50, 25, 50};

  for (uint8_t thisNote = 0; thisNote < sizeof(melody) / sizeof(int); thisNote++)
  {
    uint8_t pin = buzzerPin;

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = noteDurations[thisNote];
    tone(pin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(pin);
  }
}

bool tick500ms = LOW;
bool ledState = LOW;
volatile bool switchPinFlag = 0;
volatile bool leftPinFlag = 0;
volatile bool rightPinFlag = 0;
volatile unsigned long switchPinMillis = 0;

volatile bool switchPinToggled = 0;
volatile bool switchPinHold = 0;

struct tm *dtLocal;

void SwitchPinISR(void)
{
  switchPinFlag = !switchPinFlag;

  if (switchPinFlag)
  {
    switchPinMillis = millis();
  }
  else
  {
    if (millis() - switchPinMillis > 1000)
    {
      switchPinHold = true;
    }
    else if (millis() - switchPinMillis > 40)
    {
      switchPinToggled = true;
    }
  }
}

void LeftPinPressed(void)
{
  leftPinFlag = true;
}

void RightPinPressed(void)
{
  rightPinFlag = true;
}

/******************/

double distanceToBaseKm = 0.0;

// https://community.particle.io/t/tinygps-using-distancebetween/28233/3
// Returns the great-circle distance (in meters) between two points on a sphere
// lat1, lat2, lon1, lon2 must be provided in Degrees.  (Radians = Degrees * PI / 180, Degrees = Radians / PI * 180)
double haversine(double lat1, double lon1, double lat2, double lon2)
{
  const double rEarth = 6371000.0; // in meters
  double x = pow(sin(((lat2 - lat1) * M_PI / 180.0) / 2.0), 2.0);
  double y = cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0);
  double z = pow(sin(((lon2 - lon1) * M_PI / 180.0) / 2.0), 2.0);
  double a = x + y * z;
  double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
  double d = rEarth * c;
  // Serial.printlnf("%12.9f, %12.9f, %12.9f, %12.9f, %12.9f, %12.9f", x, y, z, a, c, d);
  return d; // in meters
}

// void Tone0(uint8_t _buzzerPin, uint16_t freq, uint16_t _duration, bool _duration_as_delay = false)
// {
//   if (_duration_as_delay)
//   {
//     tone(_buzzerPin, freq);
//     delay(_duration);
//     noTone(_buzzerPin);
//   }
//   else
//   {
//     tone(_buzzerPin, freq, _duration);
//   }
// }

void setup()
{
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  //set initial pin state
  digitalWrite(ledPin, LOW); // off

  attachInterrupt(digitalPinToInterrupt(switchPin), SwitchPinISR, CHANGE);

  Serial.begin(57600);
  while (!Serial)
    ;

  // TestTone();

  EEPROMsetup();

  // // -------------------------------------------------------------------
  // // Setup I2C stuffs
  // // -------------------------------------------------------------------
  // // DEBUGLOGLN("Clearing I2C Bus"); //http://www.forward.com.au/pfod/ArduinoProgramming/I2C_ClearBus/index.html
  // int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
  // if (rtn != 0)
  // {
  //   // DEBUGLOG("I2C bus error. Could not clear");
  //   if (rtn == 1)
  //   {
  //     // DEBUGLOG("SCL clock line held low");
  //   }
  //   else if (rtn == 2)
  //   {
  //     // DEBUGLOG("SCL clock line held low by slave clock stretch");
  //   }
  //   else if (rtn == 3)
  //   {
  //     // DEBUGLOG("SDA data line held low");
  //   }
  // }
  // else
  // {
  //   // DEBUGLOG("bus clear, re-enable Wire.begin();");
  //   // Wire.begin(SDA, SCL);
  //   Wire.begin();
  // }

  //OzOled.init();  //initialze Oscar OLED display
  OzOled.init(SSD1306_SWITCHCAPVCC, 0x3C); //Adafruit init

  OzOled.setInverseDisplayColor(LOW);

  OzOled.clearDisplay(); // clear the screen and set start position to top left corner
                         //OzOled.setPageMode();                // Set display addressing to page mode. (has to use this, can't use horizontal mode)
                         //OzOled.setHorizontalMode();      //Set addressing mode to Horizontal Mode

  //Minimun instruction for Page Addressing Mode:
  //Set the page start address of the target display location by command B0h to B7h.
  //Set the lower start column address of pointer by command 00h~0Fh.
  //Set the upper start column address of pointer by command 10h~1Fh.

  //B0~B7, Set GDDRAM Page Start Address (PAGE0~PAGE7) for Page Addressing Mode

  //OzOled.sendCommand(0xA7);

  // GPSSetup();

  if (1)
  {
    char buf[17];
    snprintf_P(buf, sizeof(buf), PSTR("Waiting GPS"));
    OzOled.drawFont8(buf, 0, 0);
    unsigned long start;
    while (!validGPSdateFlag && !validGPStimeFlag)
    {
      if (millis() - start >= 1000)
      {
        GPSSetup();
        start = millis();

        static int8_t x = 80;

        if (x == 80)
        {
          x = x + 8;
          OzOled.drawFont8("   ", x, 0);
        }
        else if (x < 112)
        {
          OzOled.drawFont8(".", x, 0);
          x = x + 8;
        }
        else
        {
          x = 88;
          OzOled.drawFont8("   ", x, 0);
        }
      }
      GPSLoop();
    }

    if (1)
    {
      utcTime = GPStimestamp;
      struct tm *timeinfo;
      timeinfo = gmtime(&utcTime);

      mktime(timeinfo);
      localTime = utcTime + TimezoneSeconds();

      dtLocal = localtime(&localTime);

      s = dtLocal->tm_sec;
      h = dtLocal->tm_hour;
      m = dtLocal->tm_min;

      // if (validGPSlocationFlag)
      // {
      //   configLocation.latitude = GPSlatitude;
      //   configLocation.longitude = GPSlongitude;
      // }

      // process_sholat();
      // process_sholat_2nd_stage();
    }
  }

  // utcTime = 592953110;

  if (0)
  {
    char buf[17];
    int len;
    int8_t x;

    borderLine('5');

    delay(1000);

    snprintf_P(buf, sizeof(buf), PSTR("               "));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    snprintf_P(buf, sizeof(buf), PSTR("TESTING"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);
    OzOled.drawLogo2("01", 2, 64, 3, 2, 3);

    delay(2000);

    //change MULTIPLEX
    snprintf_P(buf, sizeof(buf), PSTR("MULTIPLEX"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    OzOled.sendCommand(0xA8);
    OzOled.sendCommand(0x3F + 5);

    delay(2000);

    OzOled.sendCommand(0xA8);
    OzOled.sendCommand(0x3F);

    delay(2000);

    //change OFFSET
    snprintf_P(buf, sizeof(buf), PSTR("               "));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);
    snprintf_P(buf, sizeof(buf), PSTR("OFFSET"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    OzOled.sendCommand(0xD3);
    OzOled.sendCommand(0x00 + 48);

    delay(2000);

    OzOled.sendCommand(0xD3);
    OzOled.sendCommand(0x00);

    delay(2000);

    snprintf_P(buf, sizeof(buf), PSTR("START LINE"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    byte STARTLINE = (0x40 + 48);
    OzOled.sendCommand(STARTLINE);

    // while (true)
    //   ;

    delay(2000);

    STARTLINE = (0x40);
    OzOled.sendCommand(STARTLINE);

    delay(2000);

    //change STARTLINE
    snprintf_P(buf, sizeof(buf), PSTR("VARY START LINE"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);
    OzOled.drawLogo2("92", 2, 64, 3, 2, 3);
    delay(1000);

    Serial.println("Vary Start Line");

    //Varying Display Start Line: (40H - 7FH)
    //Command no.4
    //When this command changes the line address, the smooth scrolling or page change takes place.
    //Default = 40H
    for (int i = 0; i < 64; i++)
    {
      delay(25);
      byte STARTLINE = (0x40 + i);
      OzOled.sendCommand(STARTLINE);
      Serial.println(STARTLINE, HEX);
    }
    Serial.println("********");
    delay(2000);
    OzOled.drawLogo2("88", 2, 64, 3, 2, 3);
    delay(2000);
    for (int i = 0; i < 64; i++)
    {
      delay(25);
      byte STARTLINE = (0x7F - i);
      OzOled.sendCommand(STARTLINE);
      Serial.println(STARTLINE, HEX);
    }
    Serial.println("********");

    delay(2000);

    STARTLINE = (0x40);
    OzOled.sendCommand(STARTLINE);

    delay(2000);

    snprintf_P(buf, sizeof(buf), PSTR("               "));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    snprintf_P(buf, sizeof(buf), PSTR("VARY MULTIPLEX"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);
    OzOled.drawLogo2("57", 2, 64, 3, 2, 3);
    delay(1000);

    //Varying Multiplex Ration: (Double Bytes Command)
    //Command no.9
    //Multiplex Ration Mode Set: (A8H)
    //Multiplex Ration Data Set: (00H - 3FH)
    //Default: 0x3F
    for (int i = 0; i < 64; i++)
    {
      delay(25);
      OzOled.sendCommand(0xA8);
      OzOled.sendCommand(0x3F - i);
      Serial.println(i);
    }
    for (int i = 0; i < 64; i++)
    {
      delay(25);
      OzOled.sendCommand(0xA8);
      OzOled.sendCommand(0x00 + i);
      Serial.println(i);
    }

    delay(2000);

    OzOled.sendCommand(0xA8);
    OzOled.sendCommand(0x3F);

    delay(2000);

    snprintf_P(buf, sizeof(buf), PSTR("               "));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);

    snprintf_P(buf, sizeof(buf), PSTR("VARY OFFSET"));
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 0);
    OzOled.drawLogo2("39", 2, 64, 3, 2, 3);
    delay(1000);

    //Varying Display Offset: (Double Bytes Command)
    //Command no. 14
    //Display Offset Mode Set: (D3H)
    //Display Offset Data Set: (00H~3FH)
    //Default: 0x00
    for (int i = 0; i < 64; i++)
    {
      delay(25);
      OzOled.sendCommand(0xD3);
      OzOled.sendCommand(0x00 + i);
      Serial.println(i);
    }
    for (int i = 0; i < 64; i++)
    {
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

    while (1)
      ;

    // bool monitor = 0;
    // while (monitor == 0)
    // {
    //   monitor = switchPinToggled;
    // };
  }

  // Serial.println(F("Setup completed"));
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

// byte PAGE;
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

enum enumMode
{
  pageMode,
  editMode,
  modeCount
};
// enumMode MODE = pageMode;

enum enumPage
{
  Clock,
  Speed,
  Timezone,
  BaseLocation,
  SholatTime,
  Uptime,
  PageCount
};
// enumPage page = Clock;

void ConstructClockPage()
{
  char buf[17]; // MAGHRIB 18hr 36min
  // char day[3];
  // strcpy(day, dayShortStr(dtLocal->tm_wday));
  char mon[4];
  strcpy(mon, monthShortStr(dtLocal->tm_mon));
  snprintf_P(buf, sizeof(buf), PSTR("%s,%d-%s SAT%2d"),
             dayShortStr(dtLocal->tm_wday),
             dtLocal->tm_mday,
             mon,
             GPSsatellites);
  OzOled.drawFont8(buf, 0, 0);
  // snprintf_P(buf, sizeof(buf), PSTR("SAT%2d"), GPSsatellites);
  // OzOled.drawFont8(buf, 90, 0);

  // OzOled.printBigNumber(dtostrf(h, 0, 0, buf), 0, 2);
  // OzOled.printBigNumber(dtostrf(m, 0, 0, buf), 75, 2);

  snprintf_P(buf, sizeof(buf), PSTR("%2d:%02d:%02d"), h, m, s);
  OzOled.printBigNumber(buf, 0, 2);

  snprintf_P(buf, sizeof(buf), PSTR("%-7s   %2dh%2dm"), sholatNameStr(NEXTTIMEID), ceilHOUR, ceilMINUTE);
  OzOled.drawFont8(buf, 0, 7);
}

void ConstructSpeedPage()
{
  char buf[17];

  if (GPSfixStatus)
    snprintf_P(buf, sizeof(buf), PSTR("SAT%2d"), GPSsatellites);
  else
    snprintf_P(buf, sizeof(buf), PSTR("NOSIG"));

  OzOled.drawFont8(buf, 90, 0);

  if (validGPSspeedFlag)
  {
    // snprintf_P(buf, sizeof(buf), PSTR("%03d"), (int)GPSspeedKph);
    dtostrf(GPSspeedKph, 6, 1, buf);
  }
  else
  {
    snprintf_P(buf, sizeof(buf), PSTR("----.-"));
  }
  OzOled.printBigNumber(buf, 0, 2);

  if (validGPSspeedFlag)
  {
    dtostrf(distanceToBaseKm, 6, 1, buf);
  }
  else
  {
    snprintf_P(buf, sizeof(buf), PSTR("----.-"));
  }
  OzOled.printBigNumber(buf, 0, 5);

  // OzOled.drawFont8(dtostrf(GPSlatitude, 0, 7, buf), 0, 6);
  // OzOled.drawFont8(dtostrf(GPSlongitude, 0, 6, buf), 0, 7);
}

void ConstructBaseLocationPage()
{
  //title
  char buf[17];

  // snprintf_P(buf, sizeof(buf), PSTR("BASE LOCATION"));
  // int len = strlen(buf);
  // len = len * 8;
  // uint8_t x = (127 - len) / 2;

  // OzOled.drawFont8(buf, x, 0);

  snprintf_P(buf, sizeof(buf), PSTR("BASE"));
  OzOled.drawFont8(buf, 0, 0);
  OzOled.drawFont8(dtostrf(configLocation.latitude, 0, 7, buf), 0, 1);
  OzOled.drawFont8(dtostrf(configLocation.longitude, 0, 6, buf), 0, 2);

  snprintf_P(buf, sizeof(buf), PSTR("CURRENT"));
  OzOled.drawFont8(buf, 0, 4);
  OzOled.drawFont8(dtostrf(GPSlatitude, 0, 7, buf), 0, 5);
  OzOled.drawFont8(dtostrf(GPSlongitude, 0, 6, buf), 0, 6);
  OzOled.drawFont8(dtostrf(distanceToBaseKm, 0, 3, buf), 0, 7);

  //menu text
  // OzOled.drawFont8("Alarm     ", 0, 7);
}

void ConstructTimezonePage()
{
  //title
  const char *buf = "TIMEZONE";
  int len = strlen(buf);
  len = len * 8;
  uint8_t x = (128 - len) / 2;

  OzOled.drawFont8(buf, x, 0);

  char temp[7];
  dtostrf(configLocation.timezone, 3, 0, temp);
  len = strlen(temp);
  len = 3 * 8 * len;
  x = (128 - len) / 2;
  OzOled.printBigNumber(temp, x, 2);

  //menu text
  // OzOled.drawFont8("Alarm     ", 0, 7);
}

void ConstructSholatTimePage()
{
  //title
  const char *buf = "SHOLAT TIME";
  int len = strlen(buf);
  len = len * 8;
  uint8_t x = (127 - len) / 2;

  OzOled.drawFont8(buf, x, 0);

  char temp[17];

  for (uint8_t i = 0; i < TimesCount; i++)
  {
    if (i != Sunset)
    {
      uint8_t hr, mnt;

      time_t tempTimestamp = timestampSholatTimesToday[i] + TimezoneSeconds();
      struct tm *tm;
      tm = gmtime(&tempTimestamp);

      hr = tm->tm_hour;
      mnt = tm->tm_min;

      snprintf_P(temp, sizeof(temp), PSTR(" %-7s: %02d:%02d"), sholatNameStr(i), hr, mnt);

      int x = i;
      if (i > 4)
        x--;
      OzOled.drawFont8(temp, 0, 2 + x);
    }
  }

  //menu text
  // OzOled.drawFont8("Alarm     ", 0, 7);
}

void ConstructUptimePage()
{
  //title
  char buf[17];
  snprintf_P(buf, sizeof(buf), PSTR("UPTIME"));

  int len = strlen(buf);
  len = len * 8;
  uint8_t x = (128 - len) / 2;
  OzOled.drawFont8(buf, x, 0);

  if (0)
  {
    // uptime strings
    time_t uptime = millis() / 1000;
    uint16_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    struct tm *tm = gmtime(&uptime); // convert to broken down time
    days = tm->tm_yday;
    hours = tm->tm_hour;
    minutes = tm->tm_min;
    seconds = tm->tm_sec;

    snprintf_P(buf, sizeof(buf), PSTR("%u days"), days);
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 2);

    snprintf_P(buf, sizeof(buf), PSTR("%02d:%02d:%02d"), hours, minutes, seconds);
    len = strlen(buf);
    len = len * 8;
    x = (128 - len) / 2;
    OzOled.drawFont8(buf, x, 4);

    //menu text
    // OzOled.drawFont8("Alarm     ", 0, 7);
  }
}

//--------------------------

void loop()
{

  parseGPScompleted = false;
  bool tick1000ms = 0;
  static byte page = 0;
  static byte mode = 0;

  // ENCODER
  long encPos = myEnc.read();
  static long encPos_old = encPos;

  if (encPos != encPos_old)
  {
    if (encPos % 4 == 0)
    {
      if (encPos > encPos_old)
      {
        rightPinFlag = true;
      }
      else
      {
        leftPinFlag = true;
      }
      long encPos = myEnc.read();
      static long encPos_old = encPos;

      if (encPos != encPos_old)
      {
        if (encPos % 4 == 0)
        {
          if (encPos > encPos_old)
          {
            rightPinFlag = true;
          }
          else
          {
            leftPinFlag = true;
          }
        }

        encPos_old = encPos;
      }
    }

    encPos_old = encPos;
  }

  //************************************//
  //                GPS                 //
  //************************************//
  //Serial.print(GPS);

  // validGPSstatusFlag = false;
  // validGPSdateFlag = false;
  // validGPStimeFlag = false;
  // validGPSlocationFlag = false;
  // validGPSaltitudeFlag = false;
  // validGPSsatellitesFlag = false;

  if (!switchPinFlag || rightPinFlag || leftPinFlag)
    // noInterrupts();
    GPSLoop();
  // interrupts();

  digitalWrite(ledPin, parseGPScompleted);

  if (1)
  {
    unsigned long currentMillis = millis();
    static unsigned long prevMillis = 0;

    if (parseGPScompleted)
    {
      if (validGPSlocationFlag)
      {
        distanceToBaseKm = haversine(GPSlatitude, GPSlongitude, configLocation.latitude, configLocation.longitude) / 1000UL;
      }
    }
    if (validGPStimeFlag && validGPSdateFlag)
    {
      utcTime = GPStimestamp;

      struct tm *timeinfo;

      // time_t rawtime;

      // time(&utcTime);
      timeinfo = gmtime(&utcTime);

      mktime(timeinfo);
      localTime = utcTime + TimezoneSeconds();

      dtLocal = localtime(&localTime);

      static uint32_t utcTime_old = 0;
      if (utcTime != utcTime_old)
      {
        utcTime_old = utcTime;
        prevMillis = currentMillis;
        tick1000ms = true;
      }
    }
    else
    {
      if (currentMillis - prevMillis >= 999)
      {
        prevMillis = currentMillis;

        utcTime++;

        struct tm *timeinfo;

        // time_t rawtime;

        // time(&utcTime);
        timeinfo = gmtime(&utcTime);

        mktime(timeinfo);
        localTime = utcTime + TimezoneSeconds();

        dtLocal = localtime(&localTime);

        tick1000ms = true;
      }
    }
  }

  if (tick1000ms)
  {
    s = dtLocal->tm_sec;
    h = dtLocal->tm_hour;
    m = dtLocal->tm_min;

    // if (validGPSlocationFlag)
    // {
    //   configLocation.latitude = GPSlatitude;
    //   configLocation.longitude = GPSlongitude;
    // }
  }

  if (0)
  {
    if (parseGPScompleted)
    {
      utcTime = GPStimestamp;

      struct tm *timeinfo;

      // time_t rawtime;

      // time(&utcTime);
      timeinfo = gmtime(&utcTime);

      mktime(timeinfo);
      localTime = utcTime + TimezoneSeconds();

      dtLocal = localtime(&localTime);
    }

    if (parseGPScompleted)
    {
      static uint32_t utcTime_old = 0;
      if (utcTime != utcTime_old)
      {
        utcTime_old = utcTime;
        tick1000ms = true;
      }
    }

    if (parseGPScompleted)
    {
      s = dtLocal->tm_sec;
      h = dtLocal->tm_hour;
      m = dtLocal->tm_min;

      if (validGPSlocationFlag)
      {
        if ((int)distanceToBaseKm >= 10)
        {
          configLocation.latitude = GPSlatitude;
          configLocation.longitude = GPSlongitude;

          updateEEPROM();

          process_sholat();
        }
      }
    }
  }

  digitalWrite(ledPin, LOW);

  if (switchPinToggled)
  {
    DEBUG("pin toggled\r\n");
    switchPinToggled = false;

    mode++;
    if (mode == modeCount)
      mode = pageMode;
    DEBUG("mode: %d\r\n", mode);
    // alarmState = HIGH;
    // tone10 = HIGH;
  }

  if (switchPinHold)
  {
    // Tone0(buzzerPin, 4000, 20, true);

    DEBUG("pin hold\r\n");
    switchPinHold = false;
  }

  if (leftPinFlag || rightPinFlag)
  {
    // Tone0(buzzerPin, 4000, 20, true);

    if (mode == pageMode)
    {
      if (rightPinFlag)
      {
        page++;
        if (page == PageCount)
        {
          page = Clock;
        }
      }
      if (leftPinFlag)
      {
        if (page == Clock)
        {
          page = PageCount;
        }
        page--;
      }
    }
    else if (mode == editMode)
    {
      if (page == Timezone)
      {
        if (rightPinFlag)
        {
          configLocation.timezone++;
          if (configLocation.timezone > 14)
            configLocation.timezone = 14;
        }

        if (leftPinFlag)
        {
          configLocation.timezone--;
          if (configLocation.timezone < -12)
            configLocation.timezone = -12;
        }
      }
    }
  }

  // Process Sholat
  if (tick1000ms)
  {
    // tone(buzzerPin, 4000, 750);
    // tone(buzzerPin, 4000, 750);
    // TestTone();
    // DEBUG("%lu\r\n", utcTime);

    static bool alarmSholat = false;

    if (utcTime == (nextSholatTime - 10 * 60))
    {
    }
    else if (utcTime == (nextSholatTime - 5 * 60))
    {
      // Tone1(buzzerPin, 500);
    }
    else if (utcTime == nextSholatTime)
    {
      alarmSholat = true;
      
      // Tone0(buzzerPin, 2200, 500, false);

      // tone(buzzerPin, 2200);
      // delay(500);
      // noTone(buzzerPin);
    }

    if (alarmSholat)
    {
      static uint8_t count = 0;
      int dur = 500;
      
      count++;
      if (count == 5)
      {
        dur = 2000;
        count = 0;
        alarmSholat = false;
      }
      Tone0(buzzerPin, 2200, dur, false);
    }

    // process praytime
    bool processSholatFlag = false;
    static uint8_t monthDay_old = 254;
    uint8_t monthDay = dtLocal->tm_mday;
    if (monthDay != monthDay_old)
    {
      monthDay_old = monthDay;
      processSholatFlag = true;
    }

    if (processSholatFlag)
    {
      process_sholat();
    }

    process_sholat_2nd_stage();
  }

  static uint8_t page_old = PageCount;
  if (page != page_old)
  {
    page_old = page;

    OzOled.clearDisplay();

    if (page == Clock)
    {
      ConstructClockPage();
    }

    if (page == Speed)
    {
      ConstructSpeedPage();
    }

    if (page == BaseLocation)
    {
      ConstructBaseLocationPage();
    }

    if (page == Timezone)
    {
      ConstructTimezonePage();
    }

    if (page == SholatTime)
    {
      ConstructSholatTimePage();
    }

    if (page == Uptime)
    {
      ConstructUptimePage();
    }

    if (page == Speed)
      sendUBX(ubxRate4Hz, sizeof(ubxRate4Hz));
    else
      sendUBX(ubxRate1Hz, sizeof(ubxRate1Hz));
  }

  if (page == Clock)
  {
    if (tick1000ms)
    {
      ConstructClockPage();
    }
  }
  else if (page == Speed)
  {
    if (parseGPScompleted)
    {
      ConstructSpeedPage();
    }
  }
  else if (page == BaseLocation)
  {
    if (parseGPScompleted)
    {
      ConstructBaseLocationPage();
    }
  }
  else if (page == Timezone)
  {
    if (mode == editMode)
    {
      if (leftPinFlag || rightPinFlag)
      {
        int len;
        char temp[7];
        dtostrf(configLocation.timezone, 3, 0, temp);
        len = strlen(temp);
        len = 3 * 8 * len;
        x = (128 - len) / 2;
        OzOled.printBigNumber(temp, x, 2);
      }
    }
  }
  else if (page == Uptime)
  {
    if (tick1000ms)
    {
      ConstructUptimePage();
    }
  }

  if (0)
  {
    static unsigned long timer500ms = 0;

    if (tick1000ms)
    {
      timer500ms = millis();
      tick500ms = true;
    }

    if (millis() - timer500ms >= 500 && tick500ms == true)
    {
      timer500ms = millis();
      tick500ms = false;
    }

    static bool tick500ms_old = 0;
    if (tick500ms != tick500ms_old)
    {
      tick500ms_old = tick500ms;

      if (tick500ms)
      {
        // OzOled.printBigNumber(":", 52, 2);
        // OzOled.drawBitmap(circle, 60, 3, 1, 4);
        // OzOled.drawBitmap(hole, 60, 3, 1, 4);
      }
      else
      {
        if (page == 0)
        {
          // OzOled.printBigNumber(" ", 50, 2);
          // OzOled.printBigNumber(" ", 42, 2);
          // OzOled.printBigNumber(" ", 32, 2);
          // OzOled.drawBitmap(circleblank, 60, 3, 1, 4);
        }
      }
    }
  }

  if (1)
  {
    if (leftPinFlag)
    {
      Serial.print(F("Left Pin pressed, encPos: "));
      Serial.println(encPos);
    }

    if (rightPinFlag)
    {
      Serial.print(F("Right Pin pressed, encPos: "));
      Serial.println(encPos);
    }

    // if (switchPinFlag)
    // {
    //   Serial.println(F("Switch Pin pressed"));
    // }
  }

  // if (switchPinFlag)
  // {
  //   switchPinFlag = false;
  // }

  if (leftPinFlag)
  {
    leftPinFlag = false;
  }

  if (rightPinFlag)
  {
    rightPinFlag = false;
  }

  // Tone0(pinBuzzer, 50);
  // tone(pinBuzzer, 1000, 500);
  // buzzer(buzzerPin, 50);
  // Tone0(pinBuzzer, 50);
  // Tone1(buzzerPin, 50);
  // Tone10(pinBuzzer, 50);

#ifdef DEBUG
  if (0)
  {
    if (GPS)
    {
      requestSync();

      if (validGPSlocationFlag)
      {
        configLocation.latitude = GPSlatitude;
        configLocation.longitude = GPSlongitude;
      }
      GPS = false;
    }

    //************************************//
    //              SHOLAT                //
    //************************************//

    else
    {
      GPS = true;

      // //************************************//
      // //          READING PIN 1             //
      // //************************************//

      // // read the state of the switch into a local variable:
      // int reading = digitalRead(buttonPin);

      // // check to see if you just pressed the button
      // // (i.e. the input went from LOW to HIGH),  and you've waited
      // // long enough since the last press to ignore any noise:

      // // If the switch changed, due to noise or pressing:
      // if (reading != lastButtonState)
      // {
      //   // reset the debouncing timer
      //   lastDebounceTime = millis();
      // }

      // if ((millis() - lastDebounceTime) > debounceDelay)
      // {
      //   // whatever the reading is at, it's been there for longer
      //   // than the debounce delay, so take it as the actual current state:

      //   // if the button state has changed:
      //   if (reading != buttonState)
      //   {
      //     buttonState = reading;

      //     // only toggle the LED if the new button state is HIGH
      //     if (buttonState == LOW)
      //     {
      //       // ledState = !ledState;
      //       drawState = !drawState;
      //       MENU++;
      //       if (MENU > 6)
      //       {
      //         MENU = 0;
      //       }
      //       // Serial.println(MENU);
      //       UPDATE = true;
      //     }
      //   }
      // }

      // // save the reading.  Next time through the loop,
      // // it'll be the lastButtonState:
      // lastButtonState = reading;

      // //************************************//
      // //        READING ENTER PIN           //
      // //************************************//

      // int readingEnterPin = digitalRead(enterPin);

      // if (readingEnterPin != lastEnterPinState)
      // {
      //   lastDebounceTimeEnterPin = millis();
      // }

      // if ((millis() - lastDebounceTimeEnterPin) > debounceDelay)
      // {
      //   if (readingEnterPin != enterPinState)
      //   {
      //     enterPinState = readingEnterPin;

      //     // only toggle the LED if the new button state is HIGH
      //     if (enterPinState == LOW)
      //     {
      //       // ledState = !ledState;
      //       //drawState = !drawState;
      //       PAGE++;
      //       OzOled.clearDisplay();
      //       if (PAGE > 1)
      //       {
      //         PAGE = 0;
      //       }
      //       UPDATE = true;

      //       if (PAGE == 0)
      //       {
      //         FORCE_UPDATE_TIME = true;
      //         FORCE_UPDATE_SHOLAT = true;
      //       }
      //     }
      //   }
      // }

      // lastEnterPinState = readingEnterPin;

      // set the LED:
      // digitalWrite(ledPin, ledState);

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

      // bool SHOW = false;
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
      if (PAGE == 1 && MENU == 0 && UPDATE)
      {

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
      if (PAGE == 1 && MENU == 1 && UPDATE)
      {

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

      if (PAGE == 1 && MENU == 2 && UPDATE)
      {

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

      if (PAGE == 1 && MENU == 3 && UPDATE)
      {

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

      if (PAGE == 1 && MENU == 4 && UPDATE)
      {

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

      if (PAGE == 1 && MENU == 5 && UPDATE)
      {

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

      if (PAGE == 1 && MENU == 6 && UPDATE)
      {

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

      if (PAGE == 0 && UPDATE)
      {

        //blink colon symbol every 0.5 second)
        if (millisNow >= timer_500ms_old + 500)
        {

          // reset timer
          timer_500ms_old = millisNow;

          //colonON = !colonON;
        }

        if (colonON != colonON_old && colonON_old == false)
        {
          //OzOled.drawLogo2(":", 1, 45, 4, 3, 2);
          // OzOled.drawLogo2(":", 1, 41, 3, 2, 3);
        }
        else if (colonON != colonON_old && colonON_old == true)
        {
          //OzOled.drawLogo2("A", 1, 45, 4, 3, 2);
          // OzOled.drawLogo2(" ", 1, 41, 3, 2, 3);
        }
        // update the status

        colonON_old = colonON;
      }

      //************************************
      //     TIME
      //************************************

      s = dtLocal->tm_sec;

      static uint8_t s_old = 61;

      if (s != s_old)
      {
        s_old = s;
        UPDATE_TIME = true;
        UPDATE_SHOLAT = true;
      }

      if (PAGE == 0 && UPDATE_TIME)
      {

        h = dtLocal->tm_hour;
        m = dtLocal->tm_min;

        UPDATE_TIME = false;
        FORCE_UPDATE_TIME = false;
      }

      //************************************
      //     SHOLAT MENU
      //************************************

      if (PAGE == 0 && PAGE != PAGE_old)
      {
        PAGE_old = PAGE;

        // OzOled.drawFont8("hr", 80, 0);
        // OzOled.drawFont8("min", 111, 0);
      }

      //************************************
      //     CALCULATE SHOLAT TIME
      //************************************

      if (UPDATE_SHOLAT || FORCE_UPDATE_SHOLAT)
      {
        process_sholat();
        process_sholat_2nd_stage();

        // char buf[10];

        // sprintf_P(buf, PSTR("%-7s"), sholatNameStr(NEXTTIMEID));
        // OzOled.drawFont8(buf, 0, 0);
        // sprintf_P(buf, PSTR("%2d"), ceilHOUR);
        // OzOled.drawFont8(buf, 66, 0);
        // sprintf_P(buf, PSTR("%2d"), ceilMINUTE);
        // OzOled.drawFont8(buf, 97, 0);

        UPDATE_SHOLAT = false;
        FORCE_UPDATE_SHOLAT = false;

        /*
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

      for (int i = 0; i < sizeof(times) / sizeof(double); i++)
      {
        char tmp[10];
        int hours, minutes;
        get_float_time_parts(times[i], hours, minutes);
        p("%d \t %10s %s \t %02d:%02d \n\r", i, TimeName[i], dtostrf(times[i], 2, 2, tmp), hours, minutes);
      }

      for (int i = 0; i < sizeof(times) / sizeof(double); i++)
      {

        float timeNow = hour() / 1.0 + minute() / 60.0;
        float timeA, timeB;
        int hours, minutes;
        get_float_time_parts(times[i], hours, minutes);
        timeA = times[i];
        //timeA = times[i] + 2.0 / 60;
        CURRENTTIMEID = i;
        if (CURRENTTIMEID == 4)
        {
          CURRENTTIMEID = 3;
        }
        if (i != 6)
        {
          get_float_time_parts(times[i + 1], hoursNext, minutesNext);
          timeB = times[i + 1];
          //timeB = times[i + 1]  +  2.0 / 60;
          NEXTTIMEID = i + 1;
          if (NEXTTIMEID == 4)
          {
            NEXTTIMEID = 5;
          }
        }
        else if (i == 6)
        {
          get_float_time_parts(times[0], hoursNext, minutesNext);
          timeB = times[0];
          //timeB = times[0] + 2.0 / 60;
          NEXTTIMEID = 0;
        }

        if (timeB > timeA)
        {
          //Serial.println("case A");
          if (timeA < timeNow && timeNow < timeB)
          {
            //Serial.println("case A1");
            CURRENTTIMENAME = TimeName[CURRENTTIMEID];
            NEXTTIMENAME = TimeName[NEXTTIMEID];

            //extract hour and minute from time difference
            get_float_time_parts(time_diff(timeNow, timeB), HOUR, MINUTE);

            break;
          }
        }

        else if (timeB < timeA)
        {
          //Serial.println("case B");
          if ((timeA < timeNow && timeNow < 24) || (0 <= timeNow && timeNow < timeB))
          {
            //Serial.println("case B1");
            CURRENTTIMENAME = TimeName[CURRENTTIMEID];
            NEXTTIMENAME = TimeName[NEXTTIMEID];

            //extract hour and minute from time difference
            get_float_time_parts(time_diff(timeNow, timeB), HOUR, MINUTE);

            break;
          }
        }
      } //end of for loop

      // print the current time
      digitalClockDisplay();

      Serial.print(F("NEXTTIMENAME "));
      Serial.println(NEXTTIMENAME);
      Serial.print(F("String length "));
      Serial.println(NEXTTIMENAME.length());

      if (NEXTTIMEID != NEXTTIMEID_old)
      {
        NEXTTIMEID_old = NEXTTIMEID;
        int len = NEXTTIMENAME.length() + 1;
        char buf[len];
        NEXTTIMENAME.toCharArray(buf, len);
        OzOled.drawFont8("       ", 12, 0);
        OzOled.drawFont8(buf, 0, 0);
      }

      //int hoursNext, minutesNext;

      if (HOUR != HOUR_old)
      {
        HOUR_old = HOUR;

        char bufHOUR[2];
        dtostrf(HOUR, 2, 0, bufHOUR);

        OzOled.drawFont8(bufHOUR, 66, 0);
      }

      if (MINUTE != MINUTE_old)
      {
        MINUTE_old = MINUTE;

        char bufMINUTE[2];
        dtostrf(MINUTE, 2, 0, bufMINUTE);

        OzOled.drawFont8(bufMINUTE, 97, 0);
      }

      time_t t = now();
      isAM(t);

      UPDATE_SHOLAT = false;
      FORCE_UPDATE_SHOLAT = false;
      */
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

#endif
}