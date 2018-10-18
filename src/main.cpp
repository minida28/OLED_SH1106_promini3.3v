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
    const char *ptr = (char *).pgm_read_word(&(PGM_VAR_EEPROM[i]));
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

bool processSholatFlag = false;
bool processSholat2ndStageFlag = false;

bool tick500ms = LOW;
bool ledState = LOW;
volatile bool switchPinFlag = 0;
volatile bool leftPinFlag = 0;
volatile bool rightPinFlag = 0;
volatile unsigned long switchPinMillis = 0;

volatile bool switchPinToggled = 0;
volatile bool switchPinHold = 0;

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

void ProcessTimestamp(unsigned long _utc)
{
  dtUtc = RtcDateTime(_utc);
  localTime = _utc + TimezoneSeconds();
  dtLocal = RtcDateTime(localTime);

  s = dtLocal.Second();
  h = dtLocal.Hour();
  m = dtLocal.Minute();
}

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
    while (!fix.valid.date && !fix.valid.time)
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
      utcTime = fix.dateTime;

      ProcessTimestamp(utcTime);

      if (fix.valid.location)
      {
        // configLocation.latitude = fix.latitude();
        // configLocation.longitude = fix.longitude();
      }

      process_sholat();
      process_sholat_2nd_stage();
    }
  }

  // utcTime = 592953120 - 5;
  // utcTime = 592953120 - 305;
  // Serial.println(F("Setup completed"));
}

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

  // char mon[4];
  // strcpy(mon, monthShortStr(dtLocal.Month()));
  // snprintf_P(buf, sizeof(buf), PSTR("%s,%d-%s SAT%2d"),
  //            dayShortStr(dtLocal.DayOfWeek()),
  //            dtLocal.Day(),
  //            mon,
  //            fix.satellites);

  // snprintf_P(buf, sizeof(buf), PSTR("%d-%d-%d SAT%2d"),
  //            dtLocal.Day(),
  //            dtLocal.Month(),
  //            dtLocal.Year(),
  //            fix.satellites);

  // OzOled.drawFont8(buf, 0, 0);

  snprintf_P(buf, sizeof(buf), PSTR("%2d:%02d:%02d"), h, m, s);
  OzOled.printBigNumber(buf, 0, 2);

  snprintf_P(buf, sizeof(buf), PSTR("%-7s   %2dh%2dm"), sholatNameStr(NEXTTIMEID), ceilHOUR, ceilMINUTE);
  OzOled.drawFont8(buf, 0, 7);
}

void ConstructSpeedPage()
{
  char buf[17];

  if (fix.valid.status)
    snprintf_P(buf, sizeof(buf), PSTR("SAT%2d"), fix.satellites);
  else
    snprintf_P(buf, sizeof(buf), PSTR("NOSIG"));

  OzOled.drawFont8(buf, 90, 0);

  if (fix.valid.speed)
  {
    // snprintf_P(buf, sizeof(buf), PSTR("%03d"), (int)GPSspeedKph);
    dtostrf(fix.speed_kph(), 6, 1, buf);
  }
  else
  {
    snprintf_P(buf, sizeof(buf), PSTR("----.-"));
  }
  OzOled.printBigNumber(buf, 0, 2);

  if (fix.valid.location)
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

  snprintf_P(buf, sizeof(buf), PSTR("BASE LOC."));
  OzOled.drawFont8(buf, 0, 0);
  OzOled.drawFont8(dtostrf(configLocation.latitude, 11, 7, buf), 0, 1);
  OzOled.drawFont8(dtostrf(configLocation.longitude, 11, 6, buf), 0, 2);

  snprintf_P(buf, sizeof(buf), PSTR("CURRENT LOC."));
  OzOled.drawFont8(buf, 0, 4);
  OzOled.drawFont8(dtostrf(fix.latitude(), 11, 7, buf), 0, 5);
  OzOled.drawFont8(dtostrf(fix.longitude(), 11, 6, buf), 0, 6);
  OzOled.drawFont8(dtostrf(distanceToBaseKm, 11, 3, buf), 0, 7);

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

      unsigned long tempTimestamp = timestampSholatTimesToday[i] + TimezoneSeconds();

      RtcDateTime tm;
      tm = RtcDateTime(tempTimestamp);

      hr = tm.Hour();
      mnt = tm.Minute();

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

  if (1)
  {
    // uptime strings
    unsigned long uptime = millis() / 1000;

    uint16_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;

    days = elapsedDays(uptime);
    hours = numberOfHours(uptime);
    minutes = numberOfMinutes(uptime);
    seconds = numberOfSeconds(uptime);

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

  if (switchPinToggled)
  {
    // DEBUG("pin toggled\r\n");
    switchPinToggled = false;

    if (page == Timezone || page == BaseLocation)
    {
      mode++;
      if (mode == modeCount)
        mode = pageMode;
      // DEBUG("mode: %d\r\n", mode);

      if (page == BaseLocation)
      {
        if (fix.valid.location)
        {
          configLocation.latitude = fix.latitude();
          configLocation.longitude = fix.longitude();
        }
      }

      Tone1(buzzerPin, 512);

      updateEEPROM();

      processSholatFlag = true;
    }
  }

  if (switchPinHold)
  {
    // Tone0(buzzerPin, 4000, 20, true);

    DEBUG("pin hold\r\n");
    switchPinHold = false;
  }

  if (leftPinFlag || rightPinFlag)
  {
    Tone0(buzzerPin, 4000, 20, true);

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

  //************************************//
  //                GPS                 //
  //************************************//

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
      if (fix.valid.location)
      {
        distanceToBaseKm = haversine(fix.latitude(), fix.longitude(), configLocation.latitude, configLocation.longitude) / 1000UL;

        // if ((int)distanceToBaseKm >= 10)
        // {
        //   configLocation.latitude = fix.latitude();
        //   configLocation.longitude = fix.longitude();

        //   updateEEPROM();

        //   process_sholat();
        // }
      }
    }
    if (parseGPScompleted && fix.valid.time && fix.valid.date)
    {
      utcTime = fix.dateTime;

      ProcessTimestamp(utcTime);

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

        ProcessTimestamp(utcTime);

        tick1000ms = true;
      }
    }
  }

  digitalWrite(ledPin, LOW);

  // process praytime
  static uint8_t monthDay_old = 254;
  uint8_t monthDay = dtLocal.Day();
  if (monthDay != monthDay_old)
  {
    monthDay_old = monthDay;
    processSholatFlag = true;
  }

  if (processSholatFlag)
  {
    processSholatFlag = false;
    process_sholat();
    process_sholat_2nd_stage();
  }

  if (tick1000ms)
  {
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
    char buf[17];
    snprintf_P(buf, sizeof(buf), PSTR("%d-%d-%d SAT%2d"),
               dtLocal.Day(),
               dtLocal.Month(),
               dtLocal.Year(),
               fix.satellites);

    static int x = 127;
    OzOled.drawFont8(buf, x, 0);

    static uint8_t count = 0;
    count++;
    if (count == 5)
    {
      x--;
      count = 0;
      if (x == -128)
      {
        x = 127;
      }
    }

    // snprintf_P(buf, sizeof(buf), PSTR("SAT%2d"), fix.satellites);
    // OzOled.drawFont8(buf, 88, 0);

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
        uint8_t x = (127 - len) / 2;
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

  // buzzer
  if (tick1000ms)
  {
    static bool alarmPreSholat = false;
    if (utcTime == (nextSholatTime - 10 * 60))
    {
      alarmPreSholat = true;
    }

    if (utcTime == (nextSholatTime - 5 * 60))
    {
      alarmPreSholat = true;
    }

    if (alarmPreSholat)
    {

      static uint8_t count = 0;

      count++;
      if (count == 3)
      {
        count = 0;
        alarmPreSholat = false;
      }

      buzzer(buzzerPin, 500);
    }

    static bool alarmSholat = false;
    if (utcTime == nextSholatTime)
    {
      alarmSholat = true;
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

      Tone0(buzzerPin, 2375, dur, false);
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

  if (0)
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
}