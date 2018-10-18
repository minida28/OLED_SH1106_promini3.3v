#include <Arduino.h>
#include "sholat.h"
#include "sholathelper.h"
// #include "timehelper.h"
// #include "locationhelper.h"
// #include "progmemmatrix.h"
#include <time.h>

#define PRINTPORT Serial
#define DEBUGPORT Serial

#define RELEASE

#ifndef RELEASE
#define PRINT(fmt, ...)                     \
  {                                         \
    static const char pfmt[] PROGMEM = fmt; \
    printf_P(pfmt, ##__VA_ARGS__);          \
  }

#define DEBUGLOG(fmt, ...)                    \
  {                                           \
    static const char pfmt[] PROGMEM_T = fmt; \
    DEBUGPORT.printf_P(pfmt, ##__VA_ARGS__);  \
  }
#else
#define DEBUGLOG(...)
#define PRINT(...)
#endif

PrayerTimes sholat;

uint8_t HOUR;
uint8_t MINUTE;
uint8_t SECOND;
uint8_t ceilHOUR;
uint8_t ceilMINUTE;
uint8_t CURRENTTIMEID, NEXTTIMEID;

// char bufHOUR[3];
// char bufMINUTE[3];
// char bufSECOND[3];

unsigned long currentSholatTime = 0;
unsigned long nextSholatTime = 0;

NeoGPS::clock_t timestampSholatTimesYesterday[TimesCount];
NeoGPS::clock_t timestampSholatTimesToday[TimesCount];
NeoGPS::clock_t timestampSholatTimesTomorrow[TimesCount];
NeoGPS::clock_t timestampPreviousSholatTime;
NeoGPS::clock_t timestampCurrentSholatTime;
NeoGPS::clock_t timestampNextSholatTime;

// char sholatTimeYesterdayArray[TimesCount][6];
// char sholatTimeArray[TimesCount][6];
// char sholatTimeTomorrowArray[TimesCount][6];

// char bufCommonSholat[30];

char *sholatNameStr(NeoGPS::time_t _dtLocal, uint8_t id)
{
  static char buf[10];

  if (_dtLocal.date == 5 && id == Dhuhr)
  {
    char JUMUAH[] = "JUMAT";
    strcpy(buf, JUMUAH);
  }
  else
  {
    const char *ptr = (const char *)pgm_read_word(&(sholatName_P[id]));
    strcpy_P(buf, ptr);
  }

  return buf;
}

void InnerProcess(NeoGPS::clock_t _localTimeStamp, double _latitude, double _longitude, double _timezone, NeoGPS::clock_t _timestamp[])
{
  bool DEBUG = 0;

  double _timesFloat[TimesCount];

  sholat.get_prayer_times(_localTimeStamp, _latitude, _longitude, _timezone, _timesFloat);

  // for (unsigned int i = 0; i < sizeof(times) / sizeof(double); i++)
  for (unsigned int i = 0; i < TimesCount; i++)
  {
    uint8_t hr, mnt;
    sholat.get_float_time_parts(_timesFloat[i], hr, mnt);

    NeoGPS::time_t tm;
    tm = _localTimeStamp;

    tm.hours = hr;
    tm.minutes = mnt;
    tm.seconds = 0;

    int32_t offset = _timezone * NeoGPS::SECONDS_PER_HOUR;

    _timestamp[i] = tm - offset;

    //Print all results
    if (DEBUG)
    {
      char buf[32];
      snprintf_P(buf, sizeof(buf), PSTR("%d\t%02d:%02d  %lu\r\n"),
                 i,
                 //  sholatNameStr(i),
                 //dtostrf(sholat.times[i], 8, 5, tmpFloat),
                 // sholat.times[i],
                 hr,
                 mnt,
                 _timestamp[i]);
      Serial.print(buf);
      //Note:
      //snprintf specifier %f for float or double is not available in embedded device avr-gcc.
      //Therefore, float or double must be converted to string first. For this case I've used dtosrf to achive that.
    }
  }
  if (DEBUG)
  {
    Serial.println();
  }
}

void process_sholat(NeoGPS::clock_t _localTime)
{
  DEBUGLOG("\n%s\r\n", __PRETTY_FUNCTION__);

  sholat.set_calc_method(_sholatConfig.calcMethod);
  sholat.set_asr_method(_sholatConfig.asrJuristic);
  sholat.set_high_lats_adjust_method(_sholatConfig.highLatsAdjustMethod);

  if (_sholatConfig.calcMethod == Custom)
  {
    sholat.set_fajr_angle(_sholatConfig.fajrAngle);
    //sholat.set_maghrib_angle(_sholatConfig.maghribAngle);
    sholat.set_maghrib_minutes(_sholatConfig.maghribAngle);
    sholat.set_isha_angle(_sholatConfig.ishaAngle);
  }

  // apply offset to timeOffsets array
  double timeOffsets[TimesCount] = {
      //_sholatConfig.offsetImsak,
      _sholatConfig.offsetFajr,
      _sholatConfig.offsetSunrise,
      _sholatConfig.offsetDhuhr,
      _sholatConfig.offsetAsr,
      _sholatConfig.offsetSunset,
      _sholatConfig.offsetMaghrib,
      _sholatConfig.offsetIsha};

  // Tuning SHOLAT TIME
  sholat.tune(timeOffsets);

  // location settings;
  double lat = configLocation.latitude;
  double lon = configLocation.longitude;
  float tZ = configLocation.timezone;

  //CALCULATE YESTERDAY'S SHOLAT TIMES
  InnerProcess(_localTime - 86400UL, lat, lon, tZ, timestampSholatTimesYesterday);

  // CALCULATE TODAY'S SHOLAT TIMES
  InnerProcess(_localTime, lat, lon, tZ, timestampSholatTimesToday);

  // CALCULATE TOMORROW'S SHOLAT TIMES
  InnerProcess(_localTime + 86400UL, lat, lon, tZ, timestampSholatTimesTomorrow);
}

void process_sholat_2nd_stage(NeoGPS::clock_t _utcTime, NeoGPS::time_t _dtLocal)
{
  // DEBUGLOG("%s\r\n", __PRETTY_FUNCTION__);

  // char bufHOUR[3];
  // char bufMINUTE[3];
  // char bufSECOND[3];

  NeoGPS::clock_t s_tm = 0;

  // time_t t_utc;

  // time(&now);

  //int hrNextTime, mntNextTime;

  //for (unsigned int i = 0; i < sizeof(sholat.times) / sizeof(double); i++) {
  for (int i = 0; i < TimesCount; i++)
  {
    if (i != Sunset)
    {

      //First we decide, what the ID for current and next sholat time are.
      int tempCurrentID, tempPreviousID, tempNextID;

      tempCurrentID = i;
      tempPreviousID = i - 1;
      tempNextID = i + 1;

      //check NextID
      if (tempNextID == Sunset)
      {
        tempNextID = Maghrib;
      }
      if (tempCurrentID == Isha)
      {
        tempNextID = Fajr;
      }

      //check PreviousID
      if (tempPreviousID == Sunset)
      {
        tempPreviousID = Asr;
      }
      if (tempCurrentID == Fajr)
      {
        tempPreviousID = Isha;
      }

      //then
      NeoGPS::clock_t timestamp_current_today;
      NeoGPS::clock_t timestamp_next_today;
      NeoGPS::clock_t timestamp_next_tomorrow;

      timestamp_current_today = timestampSholatTimesToday[tempCurrentID];

      timestamp_next_today = timestampSholatTimesToday[tempNextID];

      timestamp_next_tomorrow = timestampSholatTimesTomorrow[tempNextID];

      DEBUGLOG("timestamp_current_today=%lu, timestamp_next_today=%lu, timestamp_next_tomorrow=%lu\r\n", timestamp_current_today, timestamp_next_today, timestamp_next_tomorrow);

      if (timestamp_current_today < timestamp_next_today)
      {
        if (_utcTime <= timestamp_current_today && _utcTime < timestamp_next_today)
        {
          CURRENTTIMEID = tempPreviousID;
          NEXTTIMEID = tempCurrentID;
          s_tm = timestamp_current_today;

          break;
        }
        else if (_utcTime > timestamp_current_today && _utcTime <= timestamp_next_today)
        {
          CURRENTTIMEID = tempCurrentID;
          NEXTTIMEID = tempNextID;
          s_tm = timestamp_next_today;

          break;
        }
      }
      else if (timestamp_current_today > timestamp_next_today)
      {
        if (_utcTime >= timestamp_current_today && _utcTime < timestamp_next_tomorrow)
        {
          CURRENTTIMEID = tempCurrentID;
          NEXTTIMEID = tempNextID;
          s_tm = timestamp_next_tomorrow;

          break;
        }
      }
    }
  } //end of for loop

  DEBUGLOG("CURRENTTIMEID=%d, NEXTTIMEID=%d\r\n", CURRENTTIMEID, NEXTTIMEID);

  NeoGPS::clock_t timestamp_current_yesterday;
  NeoGPS::clock_t timestamp_current_today;
  NeoGPS::clock_t timestamp_next_today;
  NeoGPS::clock_t timestamp_next_tomorrow;

  timestamp_current_yesterday = timestampSholatTimesYesterday[CURRENTTIMEID];
  timestamp_current_today = timestampSholatTimesToday[CURRENTTIMEID];
  timestamp_next_today = timestampSholatTimesToday[NEXTTIMEID];
  timestamp_next_tomorrow = timestampSholatTimesTomorrow[NEXTTIMEID];

  if (NEXTTIMEID > CURRENTTIMEID)
  {
    currentSholatTime = timestamp_current_today;
    nextSholatTime = timestamp_next_today;
    //PRINT("%s %lu %lu\n", "Case 2a", currentSholatTime, nextSholatTime);
    DEBUGLOG("NEXTTIMEID > CURRENTTIMEID, currentSholatTime=%lu, nextSholatTime=%lu\r\n", currentSholatTime, nextSholatTime);
  }
  else if (NEXTTIMEID < CURRENTTIMEID)
  {
    if (_dtLocal.hours >= 12) // is PM ?
    {
      currentSholatTime = timestamp_current_today;
      nextSholatTime = timestamp_next_tomorrow;
      DEBUGLOG("NEXTTIMEID < CURRENTTIMEID, currentSholatTime=%lu, nextSholatTime=%lu Hour: %d, is PM\r\n", currentSholatTime, nextSholatTime, tm_utc->tm_hour);
    }
    if (_dtLocal.hours < 12) // is AM ?
    {
      currentSholatTime = timestamp_current_yesterday;
      nextSholatTime = timestamp_next_today;
      //PRINT("%s %lu %lu\n", "Case 2c", currentSholatTime, nextSholatTime);
      DEBUGLOG("NEXTTIMEID < CURRENTTIMEID, currentSholatTime=%lu, nextSholatTime=%lu Hour: %d, is PM\r\n", currentSholatTime, nextSholatTime, tm_utc->tm_hour);
    }
  }

  NeoGPS::clock_t timeDiff = s_tm - _utcTime;
  DEBUGLOG("s_tm: %lu, now: %lu, timeDiff: %lu\r\n", s_tm, now, timeDiff);
  if (0)
  {
    char buf[64];
    snprintf_P(buf, sizeof(buf), PSTR("s_tm: %lu, utc: %lu, timeDiff: %lu\r\n"), s_tm, _utcTime, timeDiff);
    Serial.print(buf);
  }

  // uint16_t days;
  // uint8_t hr;
  // uint8_t mnt;
  // uint8_t sec;

  // // METHOD 2
  // days = elapsedDays(timeDiff);
  // HOUR = numberOfHours(timeDiff);
  // MINUTE = numberOfMinutes(timeDiff);
  // SECOND = numberOfSeconds(timeDiff);

  // METHOD 3 -> https://stackoverflow.com/questions/2419562/convert-seconds-to-days-minutes-and-seconds/17590511#17590511
  // tm *diff = gmtime(&timeDiff); // convert to broken down time
  // // DAYS = diff->tm_yday;
  // HOUR = diff->tm_hour;
  // MINUTE = diff->tm_min;
  // SECOND = diff->tm_sec;

  // METHOD 4 -> https://stackoverflow.com/questions/2419562/convert-seconds-to-days-minutes-and-seconds/2419597#2419597
  HOUR = floor(timeDiff / 3600.0);
  MINUTE = floor(fmod(timeDiff, 3600.0) / 60.0);
  SECOND = fmod(timeDiff, 60.0);

  // dtostrf(SECOND, 0, 0, bufSECOND);
  // dtostrf(MINUTE, 0, 0, bufMINUTE);
  // dtostrf(HOUR, 0, 0, bufHOUR);

  ceilMINUTE = MINUTE;
  ceilHOUR = HOUR;

  if (SECOND > 0)
  {
    ceilMINUTE++;
    if (ceilMINUTE == 60)
    {
      ceilHOUR++;
      ceilMINUTE = 0;
    }
  }

  if (0)
  {
    // if (SECOND == 0)
    if (1)
    {
      if (ceilHOUR != 0 || ceilMINUTE != 0)
      {
        if (ceilHOUR != 0)
        {
          PRINT("%d jam ", ceilHOUR);
          Serial.print(ceilHOUR);
          Serial.print(F(" jam "));
        }
        if (ceilMINUTE != 0)
        {
          PRINT("%d menit ", ceilMINUTE);
          Serial.print(ceilMINUTE);
          Serial.print(F(" menit "));
        }
        PRINT("menuju %s\r\n", sholatNameStr(NEXTTIMEID));
        Serial.print(F("menuju "));
        Serial.println(sholatNameStr(_dtLocal, NEXTTIMEID));
      }
      else if (HOUR == 0 && MINUTE == 0)
      {
        PRINT("Waktu %s telah masuk!\r\n", sholatNameStr(NEXTTIMEID));
        PRINT("Dirikanlah sholat tepat waktu.\r\n");
      }
    }
  }
}

void ProcessSholatEverySecond(NeoGPS::clock_t _utcTime, NeoGPS::clock_t _localTime, NeoGPS::time_t _dtLocal)
{
  // time_t now = time(nullptr);
  if (_utcTime >= nextSholatTime)
  {
    process_sholat(_localTime);
  }

  process_sholat_2nd_stage(_utcTime, _dtLocal);

  static int NEXTTIMEID_old = 100;

  if (NEXTTIMEID != NEXTTIMEID_old)
  {
    NEXTTIMEID_old = NEXTTIMEID;
  }
}

// float TimezoneFloat()
// {
//   time_t rawtime;
//   struct tm *timeinfo;
//   char buffer[6];

//   time(&rawtime);
//   timeinfo = localtime(&rawtime);

//   strftime(buffer, 6, "%z", timeinfo);

//   char bufTzHour[4];
//   strncpy(bufTzHour, buffer, 3);
//   int8_t hour = atoi(bufTzHour);

//   char bufTzMin[4];
//   bufTzMin[0] = buffer[0]; // sign
//   bufTzMin[1] = buffer[3];
//   bufTzMin[2] = buffer[4];
//   float min = atoi(bufTzMin) / 60.0;

//   float TZ_FLOAT = hour + min;
//   return TZ_FLOAT;
// }

// int32_t TimezoneMinutes()
// {
//   return TimezoneFloat() * 60;
// }

// int32_t TimezoneSeconds()
// {
//   return TimezoneMinutes() * 60;
// }