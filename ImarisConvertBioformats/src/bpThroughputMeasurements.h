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


#ifndef __BP_THROUGHPUT_MEASUREMENTS_H__
#define __BP_THROUGHPUT_MEASUREMENTS_H__

#include "bpWriterCommonHeaders.h"
#include ICCircularBuffer_h


class bpThroughputMeasurements
{
public:

  bpThroughputMeasurements(bpSize aBufferSize = 1000);
  ~bpThroughputMeasurements() = default;

  void AddMeasurement(bpDouble aBlockSize, bpDouble aTime);

  bpDouble GetThroughputInWindow(bpDouble aCurrentTime, bpDouble aTimeWindow);

private:
  bpCircularBuffer<std::pair<bpDouble, bpDouble>> mMeasurements;
};


#endif // __BP_THROUGHPUT_MEASUREMENTS_H__
