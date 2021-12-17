/***************************************************************************
 *   Copyright (c) 2021-present Bitplane AG Zuerich                        *
 *                                                                         *
 *   ImarisConvertBioformats is free software; you can redistribute it     *
 *   and/or modify it under the terms of the GNU General Public License    *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this program.  If not, please see                  *
 *   <http://www.gnu.org/licenses/gpl-2.0.html>.                           *
 ***************************************************************************/


#include "bpTimeInfo.h"

#include "bpUtils.h"

#include <ctime>
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>

/// This macro ensures that the human readable date and time members are set,
/// useful when debugging, but in release we don't want it. Therefore a macro
//#define BP_DEBUG_TIMEINFO
#if defined(BP_DEBUG) && defined(BP_DEBUG_TIMEINFO)
#define CREATE_DEBUG_INFO (ToStringNanosecond())
#else
#define CREATE_DEBUG_INFO ((void)0)
#endif

bpTimeInfo::bpTimeInfo()
  : mJulianDay(0),
    mNanosecondsOfDay(0),
    mLastGetTime(-1),
    mLastGetTimeMicroseconds(-1),
    mLastGetTimeNanoseconds(-1),
    mLastGetDateJulianDay(-1),
    mDate(""),
    mTime(""),
    mTimeNanoseconds("")
{
  CREATE_DEBUG_INFO;
}


bpTimeInfo::bpTimeInfo(const bpString& aTimeInfo)
  : mJulianDay(0),
    mNanosecondsOfDay(0),
    mLastGetTime(-1),
    mLastGetTimeMicroseconds(-1),
    mLastGetTimeNanoseconds(-1),
    mLastGetDateJulianDay(-1),
    mDate(""),
    mTime(""),
    mTimeNanoseconds("")
{
  FromString(aTimeInfo);
  CREATE_DEBUG_INFO;
}


bpTimeInfo::bpTimeInfo(const bpTimeInfo& aTimeInfo)
  : mNanosecondsOfDay(aTimeInfo.mNanosecondsOfDay),
    mJulianDay(aTimeInfo.mJulianDay),
    mLastGetTime(aTimeInfo.mLastGetTime),
    mLastGetTimeMicroseconds(aTimeInfo.mLastGetTimeMicroseconds),
    mLastGetTimeNanoseconds(aTimeInfo.mLastGetTimeNanoseconds),
    mLastGetDateJulianDay(aTimeInfo.mLastGetDateJulianDay),
    mDate(aTimeInfo.mDate),
    mTime(aTimeInfo.mTime),
    mTimeNanoseconds(aTimeInfo.mTimeNanoseconds)
{
  CREATE_DEBUG_INFO;
}


bpTimeInfo::~bpTimeInfo()
{
}


void bpTimeInfo::SetToday()
{
  // init date with local time
  time_t vTimeT;
  time(&vTimeT);
  tm* vLocalTime = localtime(&vTimeT);

  if (nullptr != vLocalTime) {
    mJulianDay = ToJulianDay(vLocalTime->tm_year + 1900, vLocalTime->tm_mon + 1, vLocalTime->tm_mday);
  }
  else {
    mJulianDay = ToJulianDay(1900, 1, 1);
  }
  CREATE_DEBUG_INFO;
}


void bpTimeInfo::SetTodayAndNow()
{
  //Sets date and time to the actual system date and time

  std::chrono::system_clock vSystemClock;
  std::chrono::system_clock::time_point vTimePoint = vSystemClock.now();
  time_t vTime = std::chrono::system_clock::to_time_t(vTimePoint);
  tm* vLocalTime = localtime(&vTime);

  if (nullptr != vLocalTime) {
    mJulianDay = ToJulianDay(vLocalTime->tm_year + 1900, vLocalTime->tm_mon + 1, vLocalTime->tm_mday);

    bpInt32 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(vTimePoint.time_since_epoch()).count() % 1000;
    SetTime(milliseconds + (1000 * (vLocalTime->tm_sec + (60 * vLocalTime->tm_min) + (3600 * vLocalTime->tm_hour))));
  }
  else {
    mJulianDay = ToJulianDay(1900, 1, 1);
  }

  CREATE_DEBUG_INFO;
}


void bpTimeInfo::Normalize()
{
  if (mNanosecondsOfDay >= 24LL*60*60*1000*1000*1000) {
    bpInt64 vDays = mNanosecondsOfDay / (24LL*60*60*1000*1000*1000);
    mJulianDay += static_cast<bpInt32>(vDays);
    mNanosecondsOfDay -= vDays * (24LL*60*60*1000*1000*1000);
  }
  if (mNanosecondsOfDay < 0) {
    bpInt64 vDays = mNanosecondsOfDay / (24LL*60*60*1000*1000*1000) - 1;
    mJulianDay += static_cast<bpInt32>(vDays);
    mNanosecondsOfDay -= vDays * (24LL*60*60*1000*1000*1000);
  }
  CREATE_DEBUG_INFO;
}


void bpTimeInfo::SetTime(bpInt64 aTimeMilliseconds)
{
  mNanosecondsOfDay = aTimeMilliseconds*1000*1000;
  Normalize();
}


void bpTimeInfo::SetTimeMicroSeconds(bpInt64 aTimeMicroSeconds)
{
  mNanosecondsOfDay = aTimeMicroSeconds*1000;
  Normalize();
}


void bpTimeInfo::SetTimeNanoSeconds(bpInt64 aTimeNanoSeconds)
{
  mNanosecondsOfDay = aTimeNanoSeconds;
  Normalize();
}


void bpTimeInfo::FromString(const bpString& aTimeInfo)
{
  std::vector<bpString> vTimeInfo = bpSplit(bpTrim(aTimeInfo), " ");
  if (vTimeInfo.size() >= 2) {
    SetDate(vTimeInfo[0]);
    SetTime(vTimeInfo[1]);
  }
  else if (vTimeInfo.size() == 1) {
    if (vTimeInfo[0].find(":") == bpString::npos) {
      SetDate(vTimeInfo[0]);
    }
    else {
      SetTime(vTimeInfo[0]);
    }
  }
}


bpString bpTimeInfo::ToFilesystemString() const
{
  //returns a timestamp which may be used as part of a filename

  //ISO 8601 ( https://en.wikipedia.org/wiki/ISO_8601 ) defines a timestamp as
  //YYYY-MM-DDThh:mm:ss.sss eg 2016-06-02T11:34:13.314
  //However, we can't use this format completely as ':' is invalid for filenames
  //so instead we will replace ':' with a suitable symbol eg 2016-06-02T11-34-13.314

  return GetDate() + "T" + GetTime('-');
}


bpTimeInfo bpTimeInfo::ToTimeInfo(const bpString& aFilesystemString)
{
  size_t vFoundT = aFilesystemString.find("T");
  if (vFoundT == std::string::npos) {
    return bpTimeInfo();
  }

  bpString vTimeInfoString = aFilesystemString.substr(0, vFoundT) + " "
    + bpReplace(aFilesystemString.substr(vFoundT + 1, aFilesystemString.length()), "-", ":");
  return bpTimeInfo(vTimeInfoString);
}


bpString bpTimeInfo::ToString() const
{
  return GetDate() + " " + GetTime();
}


bpString bpTimeInfo::ToStringNanosecond() const
{
  return GetDate() + " " + GetTimeNanosecond();
}


bpString bpTimeInfo::ToString(bpInt64 aValue,
                              bpUInt16 aDecimals,
                              bool aLeadingZeros) const
{
  bpString vString = bpToString(aValue);
  bpSize vLength = vString.length();
  if (vLength < aDecimals) {
    for(bpInt32 vIndex = 0; vIndex < static_cast<bpInt32>(aDecimals - vLength); vIndex++) {
      if(aLeadingZeros) {
        vString = "0" + vString;
      }
      else {
        vString = vString + "0";
      }
    }
  }
  return vString;
}


void bpTimeInfo::SetDate(const bpString& aDate)
{
  std::vector<bpString> vDate = bpSplit(bpTrim(aDate), "-", false, false);
  if (vDate.size() == 3) {
    bpInt32 vYear = 0;
    bpInt32 vMonth = 0;
    bpInt32 vDay = 0;
    bpFromString(vDate[0], vYear);
    bpFromString(vDate[1], vMonth);
    bpFromString(vDate[2], vDay);
    if (aDate.at(0) == '-') {
      vYear *= -1;
    }
    mJulianDay = ToJulianDay(vYear, vMonth, vDay);
  }
  CREATE_DEBUG_INFO;
}


void bpTimeInfo::SetTime(const bpString& aTime)
{
  // hour, minute, and second are zero by default
  bpUInt32 vHour = 0;
  bpUInt32 vMin = 0;
  bpDouble vSecond = 0.0;
  bpDouble vMilliSeconds = 0.0;
  // parse the time string
  std::vector<bpString> vTime = bpSplit(bpTrim(aTime), ":", false, false);
  if (vTime.size() == 4) {
    // time string format "15:55:07:123"
    bpFromString(vTime[0], vHour);
    bpFromString(vTime[1], vMin);
    bpFromString(vTime[2], vSecond);
    bpFromString(vTime[3], vMilliSeconds);
  }
  else if (vTime.size() == 3) {
    // time string format "15:55:07.123"
    bpFromString(vTime[0], vHour);
    bpFromString(vTime[1], vMin);
    bpFromString(vTime[2], vSecond);
  }
  else if (vTime.size() == 2) {
    // time string format "15:55"
    bpFromString(vTime[0], vHour);
    bpFromString(vTime[1], vMin);
  }
  // transform hour, minute, and second into nano-second
  bpInt64 vNanoSecond = static_cast<bpInt64>(1000 * 1000 * 1000 * vSecond + 1000 * 1000 * vMilliSeconds + 0.5);
  vNanoSecond += 1000LL*1000*1000*60*(vMin + (60*vHour));
  SetTimeNanoSeconds(vNanoSecond);
}


const bpString& bpTimeInfo::GetDate(bpChar aDateSeparator) const
{
  if (mLastGetDateJulianDay != mJulianDay) {
    bpInt32 vYear;
    bpInt32 vMonth;
    bpInt32 vDay;
    FromJulianDay(mJulianDay, vYear, vMonth, vDay);
    mDate = ToString(vYear) + aDateSeparator + ToString(vMonth) + aDateSeparator + ToString(vDay);
    mLastGetDateJulianDay = mJulianDay;
  }
  return mDate;
}


const bpString& bpTimeInfo::GetTime(bpChar aTimeSeparator, bpChar aSubsecondSeparator) const
{
  if (mLastGetTime != mNanosecondsOfDay / (1000 * 1000)) {
    mTime = GetHour() + aTimeSeparator + GetMinute() + aTimeSeparator + GetSecond() + aSubsecondSeparator + GetMillisecond();
    mLastGetTime = mNanosecondsOfDay / (1000 * 1000);
  }
  return mTime;
}


const bpString& bpTimeInfo::GetTimeMicrosecond(bpChar aTimeSeparator, bpChar aSubsecondSeparator) const
{
  if (mLastGetTimeMicroseconds != mNanosecondsOfDay / 1000) {
    mTime = GetHour() + aTimeSeparator + GetMinute() + aTimeSeparator + GetSecond() + aSubsecondSeparator + GetMicrosecond();
    mLastGetTimeMicroseconds = mNanosecondsOfDay / 1000;
  }
  return mTime;
}


const bpString& bpTimeInfo::GetTimeNanosecond(bpChar aTimeSeparator, bpChar aSubsecondSeparator) const
{
  if (mLastGetTimeNanoseconds != mNanosecondsOfDay) {
    mTimeNanoseconds = GetHour() + aTimeSeparator + GetMinute() + aTimeSeparator + GetSecond() + aSubsecondSeparator + GetNanosecond();
    mLastGetTimeNanoseconds = mNanosecondsOfDay;
  }
  return mTimeNanoseconds;
}


bpString bpTimeInfo::GetHour() const
{
  bpInt64 vHour = mNanosecondsOfDay / (60LL*60*1000*1000*1000);
  return ToString(vHour);
}


bpString bpTimeInfo::GetMinute() const
{
  bpInt64 vMin = (mNanosecondsOfDay / (60LL*1000*1000*1000)) % 60;
  return ToString(vMin);
}


bpString bpTimeInfo::GetSecond() const
{
  bpInt64 vSec = (mNanosecondsOfDay / (1000*1000*1000)) % 60;
  return ToString(vSec);
}


bpString bpTimeInfo::GetMillisecond() const
{
  bpInt64 vMsec = (mNanosecondsOfDay/(1000*1000)) % 1000;
  return ToString(vMsec,3);
}


bpString bpTimeInfo::GetMicrosecond() const
{
  bpInt64 vUsec = (mNanosecondsOfDay/1000) % (1000*1000);
  return ToString(vUsec,6);
}


bpString bpTimeInfo::GetNanosecond() const
{
  bpInt64 vNsec = mNanosecondsOfDay % (1000*1000*1000);
  return ToString(vNsec,9);
}


bpInt32 bpTimeInfo::GetJulianDay() const
{
  return mJulianDay;
}


void bpTimeInfo::SetDate(bpInt32 aJulianDay)
{
  mJulianDay = aJulianDay;
  CREATE_DEBUG_INFO;
}


bpInt64 bpTimeInfo::GetMilliseconds() const
{
  return mNanosecondsOfDay / (1000*1000);
}


bpInt64 bpTimeInfo::GetMicroseconds() const
{
  return mNanosecondsOfDay / 1000;
}


bpInt64 bpTimeInfo::GetNanoseconds() const
{
  return mNanosecondsOfDay;
}


bpTimeInfo bpTimeInfo::Add(const bpTimeInfo& aTimeInfoA,
                           const bpTimeInfo& aTimeInfoB)
{
  bpTimeInfo vTimeInfo;
  vTimeInfo.SetDate(aTimeInfoA.GetJulianDay() + aTimeInfoB.GetJulianDay());
  vTimeInfo.AddNanoseconds(aTimeInfoA.GetNanoseconds() + aTimeInfoB.GetNanoseconds());
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::Add(const bpTimeInfo& aTimeInfo, bpInt64 aMilliseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMilliseconds(aMilliseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::AddMicroseconds(const bpTimeInfo& aTimeInfo, bpInt64 aMicroseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMicroseconds(aMicroseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::AddNanoseconds(const bpTimeInfo& aTimeInfo, bpInt64 aNanoseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddNanoseconds(aNanoseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::Sub(const bpTimeInfo& aTimeInfoA,
                           const bpTimeInfo& aTimeInfoB)
{
  bpTimeInfo vTimeInfo;
  vTimeInfo.SetDate(aTimeInfoA.GetJulianDay() - aTimeInfoB.GetJulianDay());
  vTimeInfo.AddNanoseconds(aTimeInfoA.GetNanoseconds() - aTimeInfoB.GetNanoseconds());
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::Sub(const bpTimeInfo& aTimeInfo, bpInt64 aMilliseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMilliseconds(-aMilliseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::SubMicroseconds(const bpTimeInfo& aTimeInfo, bpInt64 aMicroseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMicroseconds(-aMicroseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::SubNanoseconds(const bpTimeInfo& aTimeInfo, bpInt64 aNanoseconds)
{
  bpTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddNanoseconds(-aNanoseconds);
  return vTimeInfo;
}


bpTimeInfo bpTimeInfo::Multiply(bpDouble aFactor,
                                const bpTimeInfo& aTimeInfo)
{
  // Don't try to convert julian day into nano-seconds! The number is too
  // large for a 64bit integer (overflow). Multiply julian day and nanoseconds
  // separately and set it individually
  bpDouble vJulianDay = aFactor * aTimeInfo.GetJulianDay();
  bpInt64 vNanoseconds = static_cast<bpInt64>(aFactor * aTimeInfo.GetNanoseconds() + 0.5);
  vNanoseconds += static_cast<bpInt64>((24LL*60*60*1000*1000*1000) * (vJulianDay - floor(vJulianDay)));
  bpTimeInfo vTimeInfo;
  vTimeInfo.SetDate(static_cast<bpInt32>(floor(vJulianDay)));
  vTimeInfo.AddNanoseconds(vNanoseconds);
  return vTimeInfo;
}


bpInt64 bpTimeInfo::DiffMilliseconds(const bpTimeInfo& aTimeInfoA,
                                     const bpTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay)/(1000*1000) +
         static_cast<bpInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000);
}


bpInt64 bpTimeInfo::DiffMicroseconds(const bpTimeInfo& aTimeInfoA,
                                     const bpTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay)/(1000) +
         static_cast<bpInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000*1000);
}


bpInt64 bpTimeInfo::DiffNanoseconds(const bpTimeInfo& aTimeInfoA,
                                    const bpTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay) +
         static_cast<bpInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000*1000*1000);
}


bool bpTimeInfo::IsSmaller(const bpTimeInfo& aTimeInfoA,
                           const bpTimeInfo& aTimeInfoB)
{
  if (aTimeInfoA.mJulianDay == aTimeInfoB.mJulianDay) {
    return aTimeInfoA.mNanosecondsOfDay < aTimeInfoB.mNanosecondsOfDay;
  }
  else {
    return aTimeInfoA.mJulianDay < aTimeInfoB.mJulianDay;
  }
}


bool bpTimeInfo::IsBigger(const bpTimeInfo& aTimeInfoA,
                          const bpTimeInfo& aTimeInfoB)
{
  return IsSmaller(aTimeInfoB, aTimeInfoA);
}


bool bpTimeInfo::IsEqual(const bpTimeInfo& aTimeInfoA,
                         const bpTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay == aTimeInfoB.mNanosecondsOfDay) &&
         (aTimeInfoA.mJulianDay == aTimeInfoB.mJulianDay);
}


void bpTimeInfo::AddYears(bpInt32 aYears)
{
  mJulianDay += aYears * 365;
}


void bpTimeInfo::AddDays(bpInt32 aDays)
{
  mJulianDay += aDays;
}


void bpTimeInfo::AddMilliseconds(bpInt64 aMilliseconds)
{
  mNanosecondsOfDay += 1000*1000*aMilliseconds;
  Normalize();
}


void bpTimeInfo::AddMicroseconds(bpInt64 aMicroseconds)
{
  mNanosecondsOfDay += 1000*aMicroseconds;
  Normalize();
}


void bpTimeInfo::AddNanoseconds(bpInt64 aNanoseconds)
{
  mNanosecondsOfDay += aNanoseconds;
  Normalize();
}


bpInt32 bpTimeInfo::ToJulianDay(bpInt32 aYear, bpInt32 aMonth, bpInt32 aDay)
{
  return aDay - 32075L
         + 1461L * (aYear + 4800 + (aMonth- 14L)/12L) / 4L
         + 367L * (aMonth - 2L - (aMonth - 14L)/12L * 12L) / 12L
         - 3L * ((aYear + 4900L + (aMonth - 14L)/12L) / 100L) / 4L;
}


void bpTimeInfo::FromJulianDay(bpInt32 aJulianDay,
                               bpInt32& aYear,
                               bpInt32& aMonth,
                               bpInt32& aDay)
{
  long t1 = aJulianDay + 68569L;
  long t2 = 4L*t1 / 146097L;
  t1 -= (146097L*t2 + 3L) / 4L;

  long y = 4000L*(t1 + 1)/1461001L;
  t1 = t1 - 1461L*y/4L + 31;

  long m = 80L *t1/2447L;
  aDay = bpInt32(t1 - 2447L*m/80L);

  t1 = m / 11L;
  aMonth = bpInt32(m + 2L - 12L*t1);

  aYear = bpInt32(100L * (t2 - 49L) + y + t1);
}
