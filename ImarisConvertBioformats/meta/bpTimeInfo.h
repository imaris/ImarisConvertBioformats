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


#ifndef __BPTIME_INFO__
#define __BPTIME_INFO__

#include "ImarisWriter/interface/bpConverterTypes.h"

/**
 * Provides date and time functions.
 *
 * A bpTimeInfo object contains a calendar date and a clock time. It can read
 * the current datetime from the system clock. It provides functions for
 * comparing datetimes and for manipulating a datetime by adding a number of
 * milliseconds, days or years.
 *
 * Representation of datetime object is always normalized so that day has
 * no more than 24 hours. E.g. if you add 11h to 20h it will be 7h the next
 * day. (Note: this is a change in the interface. It used to be that time was
 * NOT normalized and 31h was a valid time.)
 *
 * Internally uses a Julian calendar for days and milliseconds from midnight
 * for time.
 */
class bpTimeInfo
{

public:

  /**
   * Constructs a timeinfo starting at -4713-11-24 00:00:00.000
   *
   * @todo mg should be BCE 4713 January 01 12:00:00.0 UT Monday according to
   * Julian day definition
   *
   * Julian day and miliseconds are both set to zero.
   */
  bpTimeInfo();

  /**
   * Constructs a timeinfo object from the given string.
   *
   * String is of format "2011-11-27 15:42:37.285". It's also possible to only
   * pass the date or time part. In that case the date part will be set to
   * Julian date zero resp. time part to midnight.
   */
  explicit bpTimeInfo(const bpString& aTimeInfo);

  bpTimeInfo(const bpTimeInfo& aTimeInfo);

  virtual ~bpTimeInfo();

  /**
   * Adds years to the timeinfo object. Pos. and neg. values possible.
   */
  void AddYears(bpInt32 aYears);

  /**
   * Adds days to the timeinfo object. Pos. and neg. values possible.
   */
  void AddDays(bpInt32 aDays);

  /**
   * Adds miliseconds to the timeinfo object. Pos. and neg. values possible.
   */
  void AddMilliseconds(bpInt64 aMilliseconds);
  void AddMicroseconds(bpInt64 aMicroseconds);
  void AddNanoseconds(bpInt64 aNanoseconds);

  /**
   * Set timeinfo object to the given string.
   *
   * String is of format "2011-11-27 15:42:37.285". It's also possible to only
   * pass the date or time part. In that case the date part will be set to
   * Julian date zero resp time part to midnight.
   */
  void FromString(const bpString& aTimeInfo);

  ///returns a timestamp which may be used as part of a filename
  bpString ToFilesystemString() const;

  static bpTimeInfo ToTimeInfo(const bpString& aFilesystemString);

  /**
   * Returns a string representation of the timeinfo object
   * in the form "2011-11-27 15:42:37.285".
   */
  bpString ToString() const;
  bpString ToStringNanosecond() const;

  /**
   * Sets date from given string with format "2002-11-27". Does not change the
   * time part of the object.
   */
  void SetDate(const bpString& aDate);

  /**
   * Sets date to given Julian date. Does not change the
   * time part of the object.
   */
  void SetDate(bpInt32 aJulianDay);

  /**
   * Sets time from given string with format "15:42:37.285". The object is normalized if
   * given time crosses the 24h boundary.
   */
  void SetTime(const bpString& aTime);

  /**
   * Sets time to given milliseconds from midnight. The object is normalized if
   * given time crosses the 24h boundary.
   */
  void SetTime(bpInt64 aTimeMilliSeconds);
  void SetTimeMicroSeconds(bpInt64 aTimeMicroSeconds);
  void SetTimeNanoSeconds(bpInt64 aTimeNanoSeconds);

  /**
   * Sets date to the actual system date. Does not change the
   * time part of the object.
   */
  void SetToday();

  /**
   * Sets date and time to the actual system date and time.
   */
  void SetTodayAndNow();

  /**
   * Returns a string representation of the date part in the form "2011-11-27".
   */
  const bpString& GetDate(bpChar aDateSeparator = '-') const;

  /**
   * Returns a string representation of the time part in the form "15:42:37.285".
   */
  const bpString& GetTime(bpChar aTimeSeparator = ':', bpChar aSubsecondSeparator = '.') const;
  const bpString& GetTimeMicrosecond(bpChar aTimeSeparator = ':', bpChar aSubsecondSeparator = '.') const;
  const bpString& GetTimeNanosecond(bpChar aTimeSeparator = ':', bpChar aSubsecondSeparator = '.') const;

  /**
   * Returns the day part of the timeinfo object in Julian days.
   */
  bpInt32 GetJulianDay() const;

  /**
   * Returns the time part of the timeinfo object in miliseconds.
   */
  bpInt64 GetMilliseconds() const;

  /**
   * Returns the time part of the timeinfo object in microseconds.
   */
  bpInt64 GetMicroseconds() const;

  /**
   * Returns the time part of the timeinfo object in nanoseconds.
   */
  bpInt64 GetNanoseconds() const;

  /**
   * Compares two datatime objects and returns true
   * if first object is smaller then second.
   */
  static bool IsSmaller(const bpTimeInfo& aTimeInfoA,
                        const bpTimeInfo& aTimeInfoB);

  /**
   * Compares two datatime objects and returns true
   * if first object is bigger then second.
   */
  static bool IsBigger(const bpTimeInfo& aTimeInfoA,
                       const bpTimeInfo& aTimeInfoB);

  /**
   * Compares two datatime objects and returns true
   * if objects are equal.
   */
  static bool IsEqual(const bpTimeInfo& aTimeInfoA,
                      const bpTimeInfo& aTimeInfoB);

  /**
   * Adds two datatime objects and returns a new normalized date time object.
   */
  static bpTimeInfo Add(const bpTimeInfo& aTimeInfoA,
                        const bpTimeInfo& aTimeInfoB);

  /**
   * Adds milliseconds to a datatime objects and
   * returns a new normalized date time object.
   */
  static bpTimeInfo Add(const bpTimeInfo& aTimeInfo, bpInt64 aMilliseconds);
  static bpTimeInfo AddMicroseconds(const bpTimeInfo& aTimeInfo, bpInt64 aMicroseconds);
  static bpTimeInfo AddNanoseconds(const bpTimeInfo& aTimeInfo, bpInt64 aNanoseconds);

  /**
   * Subtracts second datatime objects from first and
   * returns a new normalized date time object.
   */
  static bpTimeInfo Sub(const bpTimeInfo& aTimeInfoA,
                        const bpTimeInfo& aTimeInfoB);

  /**
   * Subtracts milliseconds from timeinfo object and
   * returns a new normalized date time object.
   */
  static bpTimeInfo Sub(const bpTimeInfo& aTimeInfo, bpInt64 aMilliseconds);
  static bpTimeInfo SubMicroseconds(const bpTimeInfo& aTimeInfo, bpInt64 aMicroseconds);
  static bpTimeInfo SubNanoseconds(const bpTimeInfo& aTimeInfo, bpInt64 aNanoseconds);

  /**
   * Mulitplies Julian days and milliseconds of object aTimeInfo with aFactor. Returned
   * object is normalized.
   */
  static bpTimeInfo Multiply(bpDouble aFactor, const bpTimeInfo& aTimeInfo);

  /**
   * Result is aTimeInfoA - aTimeInfoB all in milliseconds. May return a
   * negative value.
   */
  static bpInt64 DiffMilliseconds(const bpTimeInfo& aTimeInfoA,
                                  const bpTimeInfo& aTimeInfoB);
  static bpInt64 DiffMicroseconds(const bpTimeInfo& aTimeInfoA,
                                  const bpTimeInfo& aTimeInfoB);
  static bpInt64 DiffNanoseconds(const bpTimeInfo& aTimeInfoA,
                                 const bpTimeInfo& aTimeInfoB);

  /**
   * Calculates Julian days from aYear, aMonth and aDay. Days and months
   * start at 1 (January = 1, December = 12).
   */
  static bpInt32 ToJulianDay(bpInt32 aYear, bpInt32 aMonth, bpInt32 aDay);

  /**
   * Sets aYear, aMonth and aDay from the given aJulian day. Days and months
   * start at 1 (January = 1, December = 12).
   */
  static void FromJulianDay(bpInt32 aJulianDay,
                            bpInt32& aYear,
                            bpInt32& aMonth,
                            bpInt32& aDay);

private:

  /**
   * Modify representation so that day has no more than 24 hours.
   */
  void Normalize();

  bpString GetHour() const;
  bpString GetMinute() const;
  bpString GetSecond() const;
  bpString GetMillisecond() const;
  bpString GetMicrosecond() const;
  bpString GetNanosecond() const;

  bpString ToString(bpInt64 aValue,
                    bpUInt16 aDecimals = 2,
                    bool aLeadingZeros = true) const;

  bpInt32 mJulianDay;
  bpInt64 mNanosecondsOfDay;

  /// the next members are only kept for performace reasons
  mutable bpString mTime;
  mutable bpString mTimeNanoseconds;
  mutable bpString mDate;
  mutable bpInt64 mLastGetTime;
  mutable bpInt64 mLastGetTimeMicroseconds;
  mutable bpInt64 mLastGetTimeNanoseconds;
  mutable bpInt32 mLastGetDateJulianDay;
};

/**
 * Global function for adding two bpTimeInfo objects.
 */
inline bpTimeInfo operator+ (const bpTimeInfo& aTimeInfoA,
                             const bpTimeInfo& aTimeInfoB)
{
  return bpTimeInfo::Add(aTimeInfoA, aTimeInfoB);
}

inline bpTimeInfo operator+ (const bpTimeInfo& aTimeInfo,
                             bpInt64 aMilliseconds)
{
  return bpTimeInfo::Add(aTimeInfo, aMilliseconds);
}

inline bpTimeInfo operator+ (bpInt64 aMilliseconds,
                             const bpTimeInfo& aTimeInfo)
{
  return bpTimeInfo::Add(aTimeInfo, aMilliseconds);
}

/**
 * Global function for subtracting two bpTimeInfo objects.
 */
inline bpTimeInfo operator- (const bpTimeInfo& aTimeInfoA,
                             const bpTimeInfo& aTimeInfoB)
{
  return bpTimeInfo::Sub(aTimeInfoA, aTimeInfoB);
}

inline bpTimeInfo operator- (const bpTimeInfo& aTimeInfo,
                             bpInt64 aMilliseconds)
{
  return bpTimeInfo::Sub(aTimeInfo, aMilliseconds);
}

inline bpTimeInfo operator- (bpInt64 aMilliseconds,
                             const bpTimeInfo& aTimeInfo)
{
  return bpTimeInfo::Sub(aTimeInfo, aMilliseconds);
}

/**
 * Global function for multiplying two bpTimeInfo objects.
 */

inline bpTimeInfo operator* (bpDouble aFactor,
                             const bpTimeInfo& aTimeInfo)
{
  return bpTimeInfo::Multiply(aFactor, aTimeInfo);
}

inline bpTimeInfo operator* (const bpTimeInfo& aTimeInfo,
                             bpDouble aFactor)
{
  return bpTimeInfo::Multiply(aFactor, aTimeInfo);
}

inline bpTimeInfo operator/ (const bpTimeInfo& aTimeInfo,
                             bpDouble aDivisor)
{
  return bpTimeInfo::Multiply(1.0 / aDivisor, aTimeInfo);
}

/**
 * Global function comparing two bpTimeInfo objects.
 */
inline bool operator< (const bpTimeInfo& aTimeInfoA,
                       const bpTimeInfo& aTimeInfoB)
{
  return bpTimeInfo::IsSmaller(aTimeInfoA, aTimeInfoB);
}

inline bool operator> (const bpTimeInfo& aTimeInfoA,
                       const bpTimeInfo& aTimeInfoB)
{
  return bpTimeInfo::IsBigger(aTimeInfoA, aTimeInfoB);
}

inline bool operator== (const bpTimeInfo& aTimeInfoA,
                        const bpTimeInfo& aTimeInfoB)
{
  return bpTimeInfo::IsEqual(aTimeInfoA, aTimeInfoB);
}

inline bool operator!= (const bpTimeInfo& aTimeInfoA,
                        const bpTimeInfo& aTimeInfoB)
{
  return !bpTimeInfo::IsEqual(aTimeInfoA, aTimeInfoB);
}

static inline bpString bpToString(const bpTimeInfo& aValue)
{
  return aValue.ToString();
}

#endif
