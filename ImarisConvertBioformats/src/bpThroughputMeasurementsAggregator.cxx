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


#include "bpThroughputMeasurementsAggregator.h"
#include "ImarisWriter/interface/bpImageConverter.h"
#include <chrono>


class bpThroughputMeasurementsAggregator::cImpl
{
public:
  static void AddReaderMeasurement(bpDouble aBlockSize)
  {
    GetMeasurements(false).AddMeasurement(aBlockSize, GetElapsedTime());
  }


  static void AddWriterMeasurement(bpDouble aBlockSize)
  {
    GetMeasurements(true).AddMeasurement(aBlockSize, GetElapsedTime());
  }


  static bpDouble GetThroughputInWindow(bpDouble aTimeWindow)
  {
    bpDouble vTime = GetElapsedTime();
    return std::max(
      GetMeasurements(false).GetThroughputInWindow(vTime, aTimeWindow),
      GetMeasurements(true).GetThroughputInWindow(vTime, aTimeWindow));
  }

  static void RestartReferenceTime()
  {
    GetStartTime(true);
  }

private:
  using tClock = std::chrono::system_clock;

  static bpDouble GetElapsedTime()
  {
    return std::chrono::duration<bpDouble>(tClock::now() - GetStartTime()).count();
  }

  static tClock::time_point& GetStartTime(bool aRestart = false)
  {
    static tClock::time_point vStartTime = tClock::now();
    if (aRestart) {
      vStartTime = tClock::now();
    }
    return vStartTime;
  }

  static bpThroughputMeasurements& GetMeasurements(bool aWrite)
  {
    static bpThroughputMeasurements vMeasurements[2];
    return vMeasurements[aWrite ? 1 : 0];
  }
};


void bpThroughputMeasurementsAggregator::AddReaderMeasurement(bpDouble aBlockSize)
{
  cImpl::AddReaderMeasurement(aBlockSize);
}


void bpThroughputMeasurementsAggregator::AddWriterMeasurement(bpDouble aBlockSize)
{
  cImpl::AddWriterMeasurement(aBlockSize);
}


bpDouble bpThroughputMeasurementsAggregator::GetThroughputInWindow(bpDouble aTimeWindow)
{
  return cImpl::GetThroughputInWindow(aTimeWindow);
}


void bpThroughputMeasurementsAggregator::RestartReferenceTime()
{
  cImpl::RestartReferenceTime();
}
