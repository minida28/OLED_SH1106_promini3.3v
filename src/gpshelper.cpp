#include "gpshelper.h"

// #include <NMEAGPS.h>
#include <GPSport.h>

NMEAGPS gps; // This parses the GPS characters
gps_fix fix; // This holds on to the latest values

NeoGPS::Location_t EighteenOfficePark(-6.299227, 106.8319975);

bool echoing;

// bool validGPSstatusFlag;
// bool validGPSsatellitesFlag;
// bool validGPStimeFlag;
// bool validGPSdateFlag;
// bool validGPSlocationFlag;
// bool validGPSaltitudeFlag;
// bool validGPSspeedFlag;
bool parseGPScompleted;

// uint32_t GPStimestamp;
// uint8_t GPSfixStatus;
// uint8_t GPSsatellites;
// double GPSlatitude;
// double GPSlongitude;
// double GPSaltitude;
// double GPSspeedKph;
double distanceKm;

// uint8_t ledPin = 13;

//--------------------------
// Set these values to the offset of your timezone from GMT

static const int32_t zone_hours = 0L;   // UTC
static const int32_t zone_minutes = 0L; // usually zero
static const NeoGPS::clock_t zone_offset =
    zone_hours * NeoGPS::SECONDS_PER_HOUR +
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

void localTime(NeoGPS::time_t &dt)
{
    NeoGPS::clock_t seconds = dt; // convert date/time structure to seconds

#ifdef CALCULATE_DST
    //  Calculate DST changeover times once per reset and year!
    static NeoGPS::time_t changeover;
    static NeoGPS::clock_t springForward, fallBack;

    if ((springForward == 0) || (changeover.year != dt.year))
    {

        //  Calculate the spring changeover time (seconds)
        changeover.year = dt.year;
        changeover.month = springMonth;
        changeover.date = springDate;
        changeover.hours = springHour;
        changeover.minutes = 0;
        changeover.seconds = 0;
        changeover.set_day();
        // Step back to a Sunday, if day != SUNDAY
        changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
        springForward = (NeoGPS::clock_t)changeover;

        //  Calculate the fall changeover time (seconds)
        changeover.month = fallMonth;
        changeover.date = fallDate;
        changeover.hours = fallHour - 1; // to account for the "apparent" DST +1
        changeover.set_day();
        // Step back to a Sunday, if day != SUNDAY
        changeover.date -= (changeover.day - NeoGPS::time_t::SUNDAY);
        fallBack = (NeoGPS::clock_t)changeover;
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

void changeBaud(const char *textCommand, unsigned long baud)
{
    gps.send_P(&gpsPort, (const __FlashStringHelper *)textCommand);
    gpsPort.flush();
    gpsPort.end();

    char buf[30];
    sprintf_P(buf, PSTR("Baudrate changed to %lu\r\n"), baud);
    DEBUG_PORT.print(buf);
    // delay(500);
    gpsPort.begin(baud);

} // changeBaud

void sendUBX(const unsigned char *progmemBytes, size_t len)
{
    gpsPort.write(0xB5); // SYNC1
    gpsPort.write(0x62); // SYNC2

    uint8_t a = 0, b = 0;
    while (len-- > 0)
    {
        uint8_t c = pgm_read_byte(progmemBytes++);
        a += c;
        b += a;
        gpsPort.write(c);
    }

    gpsPort.write(a); // CHECKSUM A
    gpsPort.write(b); // CHECKSUM B

} // sendUBX

void GPSSetup()
{
    // pinMode(ledPin, OUTPUT);
    // digitalWrite(ledPin, HIGH);

    // set initial LED state
    // digitalWrite(ledPin, ledState);

    gpsPort.begin(9600);
    // delay(500);
    // changeBaud(baud9600, 9600UL);
    // delay(500);

    // while (gps.available(gpsPort))
    //     ;
    // sendUBX(ubxRate2Hz, sizeof(ubxRate2Hz));

    // while (gps.available(gpsPort))
    //     ;
    sendUBX(ubxDisableGSV, sizeof(ubxDisableGSV));
    // while (gps.available(gpsPort))
    //     ;
    sendUBX(ubxDisableGSA, sizeof(ubxDisableGSA));
    // while (gps.available(gpsPort))
    //     ;
    sendUBX(ubxDisableVTG, sizeof(ubxDisableVTG));
    // while (gps.available(gpsPort))
    //     ;
    sendUBX(ubxDisableGLL, sizeof(ubxDisableGLL));
    // while (gps.available(gpsPort))
    //     ;
    sendUBX(ubxRate1Hz, sizeof(ubxRate1Hz));
#ifdef NMEAGPS_INTERRUPT_PROCESSING
    gpsPort.attachInterrupt(GPSisr);
#endif
}

void processGPS()
{
    parseGPScompleted = true;

    // validGPSstatusFlag = fix.valid.status;
    // validGPSdateFlag = fix.valid.date;
    // validGPStimeFlag = fix.valid.time;
    // validGPSlocationFlag = fix.valid.location;
    // validGPSaltitudeFlag = fix.valid.altitude;
    // validGPSspeedFlag = fix.valid.speed;
    // validGPSsatellitesFlag = fix.valid.satellites;

    // if (fix.valid.status)
    // {
    //     GPSfixStatus = fix.status;
    // }

    // if (fix.valid.satellites)
    // {
    //     GPSsatellites = fix.satellites;
    // }

    // if (fix.valid.date && fix.valid.time)
    // {

    //     GPStimestamp = fix.dateTime;
    // }

    // if (fix.valid.location)
    // {
    //     GPSlatitude = fix.latitude();
    //     GPSlongitude = fix.longitude();

    //     // distanceKm = haversine(fix.latitude(), fix.longitude(), EighteenOfficePark.latF(), EighteenOfficePark.lonF()) / 1000UL;
        

    //     // distance = fix.location.DistanceKm(EighteenOfficePark);
    //     // or distance = NeoGPS::Location_t::DistanceKm( fix.location, EighteenOfficePark );
    //     // Serial.print( dist );
    //     // Serial.println( F(" km") );

    //     // NeoGPS::Location_t madrid(-6.2759328, 106.8679153);
    //     // distance = haversine(fix.latitude(), fix.longitude(), -6.299227, 106.8319975)/1000UL;
    // }

    // if (fix.valid.altitude)
    // {
    //     GPSaltitude = fix.altitude();
    // }

    // if (fix.valid.speed)
    // {
    //     GPSspeedKph = fix.speed_kph();
    // }
}

void GPSLoop()
{
    // validGPSstatusFlag = false;
    // validGPSdateFlag = false;
    // validGPStimeFlag = false;
    // validGPSlocationFlag = false;
    //   validGPSaltitudeFlag = false;
    // validGPSsatellitesFlag = false;

    static bool displayingHex = false;

    echoing = 0;

    if (echoing)
    {
        // Use advanced character-oriented methods to echo received characters to
        //    the Serial Monitor window.
        if (gpsPort.available())
        {

            char c = gpsPort.read();

            if (((' ' <= c) && (c <= '~')) || (c == '\r') || (c == '\n'))
            {
                DEBUG_PORT.write(c);
                displayingHex = false;
            }
            else
            {
                if (!displayingHex)
                {
                    displayingHex = true;
                    DEBUG_PORT.print(F("0x"));
                }
                if (c < 0x10)
                    DEBUG_PORT.write('0');
                DEBUG_PORT.print((uint8_t)c, HEX);
                DEBUG_PORT.write(' ');
            }

            gps.handle(c);
            if (gps.available())
            {
                fix = gps.read();

                processGPS();

                if (displayingHex)
                    displayingHex = false;
                DEBUG_PORT.println();
                // doSomeWork();
            }
        }
    }
    else
    {

        // Use the normal fix-oriented methods to display fixes
        if (gps.available(gpsPort))
        {
            // fix = gps.read();
            // // doSomeWork();

            fix = gps.read();

            processGPS();

            displayingHex = false;
        }
    }
}