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


#include "bpThroughputMeasurementsFetcher.h"
#include "bpThroughputMeasurementsAggregator.h"

#include <thread>
#include <atomic>
#include <iostream>


class bpThroughputMeasurementsFetcher::cImpl
{
public:
  cImpl(bpUInt32 aRepeatTimeMs)
    : mThread([this, aRepeatTimeMs] { Run(aRepeatTimeMs); })
  {
    bpThroughputMeasurementsAggregator::RestartReferenceTime();
  }

  ~cImpl()
  {
    mStop = true;
    mThread.join();
  }

private:
  void Run(bpUInt32 aRepeatTimeMs)
  {
    while (aRepeatTimeMs > 0 && !mStop) {
      for (bpSize vStopCheckIdx = 0; vStopCheckIdx < 10; ++vStopCheckIdx) {
        std::this_thread::sleep_for(std::chrono::milliseconds(aRepeatTimeMs / 10));
        if (mStop) {
          break;
        }
      }
      bpDouble vData = bpThroughputMeasurementsAggregator::GetThroughputInWindow(2);
      std::cout << "Throughput: " << vData << std::endl;
    }
  }

  std::atomic_bool mStop {false};
  std::thread mThread;
};


void bpThroughputMeasurementsFetcher::Start(bpUInt32 aRepeatTimeMs)
{
  mImpl = std::make_shared<cImpl>(aRepeatTimeMs);
}


void bpThroughputMeasurementsFetcher::Stop()
{
  mImpl.reset();
}
