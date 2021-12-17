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


#include "fileiobase/types/bpfTimeInfo.h"

#include "fileiobase/utils/bpfUtils.h"

#include <ctime>
#include <vector>
#include <cmath>
#include <iostream>

/// This macro ensures that the human readable date and time members are set,
/// useful when debugging, but in release we don't want it. Therefore a macro
#ifdef bpf_DEBUG
#define CREATE_DEBUG_INFO (ToStringNanosecond())
#else
#define CREATE_DEBUG_INFO ((void)0)
#endif

bpfTimeInfo::bpfTimeInfo()
  : mJulianDay(0),
    mNanosecondsOfDay(0),
    mLastGetTime(-1),
    mLastGetTimeNanoseconds(-1),
    mLastGetDateJulianDay(-1),
    mDate(""),
    mTime(""),
    mTimeNanoseconds("")
{
  CREATE_DEBUG_INFO;
}


bpfTimeInfo::bpfTimeInfo(const bpfString& aTimeInfo)
  : mJulianDay(0),
    mNanosecondsOfDay(0),
    mLastGetTime(-1),
    mLastGetTimeNanoseconds(-1),
    mLastGetDateJulianDay(-1),
    mDate(""),
    mTime(""),
    mTimeNanoseconds("")
{
  FromString(aTimeInfo);
  CREATE_DEBUG_INFO;
}


bpfTimeInfo::bpfTimeInfo(const bpfTimeInfo& aTimeInfo)
  : mNanosecondsOfDay(aTimeInfo.mNanosecondsOfDay),
    mJulianDay(aTimeInfo.mJulianDay),
    mLastGetTime(aTimeInfo.mLastGetTime),
    mLastGetTimeNanoseconds(aTimeInfo.mLastGetTimeNanoseconds),
    mLastGetDateJulianDay(aTimeInfo.mLastGetDateJulianDay),
    mDate(aTimeInfo.mDate),
    mTime(aTimeInfo.mTime),
    mTimeNanoseconds(aTimeInfo.mTimeNanoseconds)
{
  CREATE_DEBUG_INFO;
}


bpfTimeInfo::~bpfTimeInfo()
{
}


void bpfTimeInfo::Normalize()
{
  if (mNanosecondsOfDay >= 24LL*60*60*1000*1000*1000) {
    bpfInt64 vDays = mNanosecondsOfDay / (24LL*60*60*1000*1000*1000);
    mJulianDay += static_cast<bpfInt32>(vDays);
    mNanosecondsOfDay -= vDays * (24LL*60*60*1000*1000*1000);
  }
  if (mNanosecondsOfDay < 0) {
    bpfInt64 vDays = mNanosecondsOfDay / (24LL*60*60*1000*1000*1000) - 1;
    mJulianDay += static_cast<bpfInt32>(vDays);
    mNanosecondsOfDay -= vDays * (24LL*60*60*1000*1000*1000);
  }
  CREATE_DEBUG_INFO;
}


void bpfTimeInfo::SetTime(bpfInt64 aTimeMilliseconds)
{
  mNanosecondsOfDay = aTimeMilliseconds*1000*1000;
  Normalize();
}


void bpfTimeInfo::SetTimeMicroSeconds(bpfInt64 aTimeMicroSeconds)
{
  mNanosecondsOfDay = aTimeMicroSeconds*1000;
  Normalize();
}


void bpfTimeInfo::SetTimeNanoSeconds(bpfInt64 aTimeNanoSeconds)
{
  mNanosecondsOfDay = aTimeNanoSeconds;
  Normalize();
}


void bpfTimeInfo::FromString(const bpfString& aTimeInfo)
{
  std::vector<bpfString> vTimeInfo = bpfSplit(bpfTrim(aTimeInfo), " ");
  if (vTimeInfo.size() >= 2) {
    SetDate(vTimeInfo[0]);
    SetTime(vTimeInfo[1]);
  }
  else if (vTimeInfo.size() == 1) {
    if (vTimeInfo[0].find(":") == bpfString::npos) {
      SetDate(vTimeInfo[0]);
    }
    else {
      SetTime(vTimeInfo[0]);
    }
  }
}


bpfString bpfTimeInfo::ToString() const
{
  return GetDate() + " " + GetTime();
}


bpfString bpfTimeInfo::ToStringNanosecond() const
{
  return GetDate() + " " + GetTimeNanosecond();
}


bpfString bpfTimeInfo::ToString(bpfInt64 aValue,
                              bpfUInt16 aDecimals,
                              bool aLeadingZeros) const
{
  bpfString vString = bpfToString(aValue);
  bpfSize vLength = vString.length();
  if (vLength < aDecimals) {
    for(bpfInt32 vIndex = 0; vIndex < static_cast<bpfInt32>(aDecimals - vLength); vIndex++) {
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


void bpfTimeInfo::SetDate(const bpfString& aDate)
{
  std::vector<bpfString> vDate = bpfSplit(bpfTrim(aDate), "-");
  if (vDate.size() == 3) {
    bpfInt32 vYear = 0;
    bpfInt32 vMonth = 0;
    bpfInt32 vDay = 0;
    bpfFromString(vDate[0], vYear);
    bpfFromString(vDate[1], vMonth);
    bpfFromString(vDate[2], vDay);

    // swap day and month if non ISO 8601 format, i.e order YY-DD-MM
    // This is only detected if day > 12 of course, i.e. not fail proof
    if (vMonth > bpfInt32(12)) {
      std::swap(vMonth, vDay);
    }

    if (aDate.at(0) == '-') {
      vYear *= -1;
    }
    mJulianDay = ToJulianDay(vYear, vMonth, vDay);
  }
  CREATE_DEBUG_INFO;
}


void bpfTimeInfo::SetTime(const bpfString& aTime)
{
  // hour, minute, and second are zero by default
  bpfUInt32 vHour = 0;
  bpfUInt32 vMin = 0;
  bpfDouble vSecond = 0.0;
  bpfDouble vMilliSeconds = 0.0;
  // parse the time string
  std::vector<bpfString> vTime = bpfSplit(bpfTrim(aTime), ":");
  if (vTime.size() == 4) {
    // time string format "15:55:07:123"
    bpfFromString(vTime[0], vHour);
    bpfFromString(vTime[1], vMin);
    bpfFromString(vTime[2], vSecond);
    bpfFromString(vTime[3], vMilliSeconds);
  }
  else if (vTime.size() == 3) {
    // time string format "15:55:07.123"
    bpfFromString(vTime[0], vHour);
    bpfFromString(vTime[1], vMin);
    bpfFromString(vTime[2], vSecond);
  }
  else if (vTime.size() == 2) {
    // time string format "15:55"
    bpfFromString(vTime[0], vHour);
    bpfFromString(vTime[1], vMin);
  }
  // transform hour, minute, and second into nano-second
  bpfInt64 vNanoSecond = 0;
  if (24 > vHour && 60 > vMin && 60 > vSecond) {
    bpfRound(1000 * 1000 * 1000 * vSecond + 1000 * 1000 * vMilliSeconds, vNanoSecond);
    vNanoSecond += 1000LL * 1000 * 1000 * 60 * (vMin + (60 * vHour));
  }
  SetTimeNanoSeconds(vNanoSecond);
}


const bpfString& bpfTimeInfo::GetDate() const
{
  if (mLastGetDateJulianDay != mJulianDay) {
    bpfInt32 vYear;
    bpfInt32 vMonth;
    bpfInt32 vDay;
    FromJulianDay(mJulianDay, vYear, vMonth, vDay);
    mDate = ToString(vYear) + "-" +
            ToString(vMonth) + "-" +
            ToString(vDay);
    mLastGetDateJulianDay = mJulianDay;
  }
  return mDate;
}


const bpfString& bpfTimeInfo::GetTime() const
{
  if (mLastGetTime != mNanosecondsOfDay / (1000*1000)) {
    mTime = GetHour() + ":" +
            GetMinute() + ":" +
            GetSecond() + "." +
            GetMillisecond();
    mLastGetTime = mNanosecondsOfDay / (1000*1000);
  }
  return mTime;
}


const bpfString& bpfTimeInfo::GetTimeNanosecond() const
{
  if (mLastGetTimeNanoseconds != mNanosecondsOfDay) {
    mTimeNanoseconds = GetHour() + ":" +
            GetMinute() + ":" +
            GetSecond() + "." +
            GetNanosecond();
    mLastGetTimeNanoseconds = mNanosecondsOfDay;
  }
  return mTimeNanoseconds;
}


bpfString bpfTimeInfo::GetHour() const
{
  bpfInt64 vHour = mNanosecondsOfDay / (60LL*60*1000*1000*1000);
  return ToString(vHour);
}


bpfString bpfTimeInfo::GetMinute() const
{
  bpfInt64 vMin = (mNanosecondsOfDay / (60LL*1000*1000*1000)) % 60;
  return ToString(vMin);
}


bpfString bpfTimeInfo::GetSecond() const
{
  bpfInt64 vSec = (mNanosecondsOfDay / (1000*1000*1000)) % 60;
  return ToString(vSec);
}


bpfString bpfTimeInfo::GetMillisecond() const
{
  bpfInt64 vMsec = (mNanosecondsOfDay/(1000*1000)) % 1000;
  return ToString(vMsec,3);
}


bpfString bpfTimeInfo::GetNanosecond() const
{
  bpfInt64 vNsec = mNanosecondsOfDay % (1000*1000*1000);
  return ToString(vNsec,9);
}


bpfInt32 bpfTimeInfo::GetJulianDay() const
{
  return mJulianDay;
}


void bpfTimeInfo::SetDate(bpfInt32 aJulianDay)
{
  mJulianDay = aJulianDay;
  CREATE_DEBUG_INFO;
}


bpfInt64 bpfTimeInfo::GetMilliseconds() const
{
  return mNanosecondsOfDay / (1000*1000);
}


bpfInt64 bpfTimeInfo::GetMicroseconds() const
{
  return mNanosecondsOfDay / 1000;
}


bpfInt64 bpfTimeInfo::GetNanoseconds() const
{
  return mNanosecondsOfDay;
}


bpfTimeInfo bpfTimeInfo::Add(const bpfTimeInfo& aTimeInfoA,
                           const bpfTimeInfo& aTimeInfoB)
{
  bpfTimeInfo vTimeInfo;
  vTimeInfo.SetDate(aTimeInfoA.GetJulianDay() + aTimeInfoB.GetJulianDay());
  vTimeInfo.AddNanoseconds(aTimeInfoA.GetNanoseconds() + aTimeInfoB.GetNanoseconds());
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::Add(const bpfTimeInfo& aTimeInfo, bpfInt64 aMilliseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMilliseconds(aMilliseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::AddMicroseconds(const bpfTimeInfo& aTimeInfo, bpfInt64 aMicroseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMicroseconds(aMicroseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::AddNanoseconds(const bpfTimeInfo& aTimeInfo, bpfInt64 aNanoseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddNanoseconds(aNanoseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::Sub(const bpfTimeInfo& aTimeInfoA,
                           const bpfTimeInfo& aTimeInfoB)
{
  bpfTimeInfo vTimeInfo;
  vTimeInfo.SetDate(aTimeInfoA.GetJulianDay() - aTimeInfoB.GetJulianDay());
  vTimeInfo.AddNanoseconds(aTimeInfoA.GetNanoseconds() - aTimeInfoB.GetNanoseconds());
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::Sub(const bpfTimeInfo& aTimeInfo, bpfInt64 aMilliseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMilliseconds(-aMilliseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::SubMicroseconds(const bpfTimeInfo& aTimeInfo, bpfInt64 aMicroseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddMicroseconds(-aMicroseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::SubNanoseconds(const bpfTimeInfo& aTimeInfo, bpfInt64 aNanoseconds)
{
  bpfTimeInfo vTimeInfo(aTimeInfo);
  vTimeInfo.AddNanoseconds(-aNanoseconds);
  return vTimeInfo;
}


bpfTimeInfo bpfTimeInfo::Multiply(bpfDouble aFactor,
                                const bpfTimeInfo& aTimeInfo)
{
  // Don't try to convert julian day into nano-seconds! The number is too
  // large for a 64bit integer (overflow). Multiply julian day and nanoseconds
  // separately and set it individually
  bpfDouble vJulianDay = aFactor * aTimeInfo.GetJulianDay();
  bpfInt64 vNanoseconds = static_cast<bpfInt64>(aFactor * aTimeInfo.GetNanoseconds() + 0.5);
  vNanoseconds += static_cast<bpfInt64>((24LL*60*60*1000*1000*1000) * (vJulianDay - floor(vJulianDay)));
  bpfTimeInfo vTimeInfo;
  vTimeInfo.SetDate(static_cast<bpfInt32>(floor(vJulianDay)));
  vTimeInfo.AddNanoseconds(vNanoseconds);
  return vTimeInfo;
}


bpfInt64 bpfTimeInfo::DiffMilliseconds(const bpfTimeInfo& aTimeInfoA,
                                     const bpfTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay)/(1000*1000) +
         static_cast<bpfInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000);
}


bpfInt64 bpfTimeInfo::DiffMicroseconds(const bpfTimeInfo& aTimeInfoA,
                                     const bpfTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay)/(1000) +
         static_cast<bpfInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000*1000);
}


bpfInt64 bpfTimeInfo::DiffNanoseconds(const bpfTimeInfo& aTimeInfoA,
                                    const bpfTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay - aTimeInfoB.mNanosecondsOfDay) +
         static_cast<bpfInt64>(aTimeInfoA.mJulianDay - aTimeInfoB.mJulianDay) * (24LL*60*60*1000*1000*1000);
}


bool bpfTimeInfo::IsSmaller(const bpfTimeInfo& aTimeInfoA,
                           const bpfTimeInfo& aTimeInfoB)
{
  if (aTimeInfoA.mJulianDay == aTimeInfoB.mJulianDay) {
    return aTimeInfoA.mNanosecondsOfDay < aTimeInfoB.mNanosecondsOfDay;
  }
  else {
    return aTimeInfoA.mJulianDay < aTimeInfoB.mJulianDay;
  }
}


bool bpfTimeInfo::IsBigger(const bpfTimeInfo& aTimeInfoA,
                          const bpfTimeInfo& aTimeInfoB)
{
  return IsSmaller(aTimeInfoB, aTimeInfoA);
}


bool bpfTimeInfo::IsEqual(const bpfTimeInfo& aTimeInfoA,
                         const bpfTimeInfo& aTimeInfoB)
{
  return (aTimeInfoA.mNanosecondsOfDay == aTimeInfoB.mNanosecondsOfDay) &&
         (aTimeInfoA.mJulianDay == aTimeInfoB.mJulianDay);
}


void bpfTimeInfo::AddYears(bpfInt32 aYears)
{
  mJulianDay += aYears * 365;
}


void bpfTimeInfo::AddDays(bpfInt32 aDays)
{
  mJulianDay += aDays;
}


void bpfTimeInfo::AddMilliseconds(bpfInt64 aMilliseconds)
{
  mNanosecondsOfDay += 1000*1000*aMilliseconds;
  Normalize();
}


void bpfTimeInfo::AddMicroseconds(bpfInt64 aMicroseconds)
{
  mNanosecondsOfDay += 1000*aMicroseconds;
  Normalize();
}


void bpfTimeInfo::AddNanoseconds(bpfInt64 aNanoseconds)
{
  mNanosecondsOfDay += aNanoseconds;
  Normalize();
}


bpfInt32 bpfTimeInfo::ToJulianDay(bpfInt32 aYear, bpfInt32 aMonth, bpfInt32 aDay)
{
  return aDay - 32075L
         + 1461L * (aYear + 4800 + (aMonth- 14L)/12L) / 4L
         + 367L * (aMonth - 2L - (aMonth - 14L)/12L * 12L) / 12L
         - 3L * ((aYear + 4900L + (aMonth - 14L)/12L) / 100L) / 4L;
}


void bpfTimeInfo::FromJulianDay(bpfInt32 aJulianDay,
                               bpfInt32& aYear,
                               bpfInt32& aMonth,
                               bpfInt32& aDay)
{
  long t1 = aJulianDay + 68569L;
  long t2 = 4L*t1 / 146097L;
  t1 -= (146097L*t2 + 3L) / 4L;

  long y = 4000L*(t1 + 1)/1461001L;
  t1 = t1 - 1461L*y/4L + 31;

  long m = 80L *t1/2447L;
  aDay = bpfInt32(t1 - 2447L*m/80L);

  t1 = m / 11L;
  aMonth = bpfInt32(m + 2L - 12L*t1);

  aYear = bpfInt32(100L * (t2 - 49L) + y + t1);
}

