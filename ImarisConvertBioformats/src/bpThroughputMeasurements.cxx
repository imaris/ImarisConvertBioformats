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


#include "bpThroughputMeasurements.h"


bpThroughputMeasurements::bpThroughputMeasurements(bpSize aBufferSize)
  : mMeasurements(aBufferSize)
{
}


void bpThroughputMeasurements::AddMeasurement(bpDouble aBlockSize, bpDouble aTime)
{
  mMeasurements.Add(std::make_pair(aBlockSize, aTime));
}


bpDouble bpThroughputMeasurements::GetThroughputInWindow(bpDouble aCurrentTime, bpDouble aTimeWindow)
{
  if (aTimeWindow <= 0.0 || mMeasurements.Empty()) {
    return 0.0;
  }

  bpDouble vSumOfWeightedSpeeds = 0.0;

  bpSize vBufferSize = mMeasurements.Size() + 1;

  // Get index of least-recently-added item plus one (next empty slot index).
  bpSize vIndexHeadFirst = mMeasurements.GetHeadIndex() % vBufferSize;
  bpSize vTailIndex = mMeasurements.GetTailIndex() % vBufferSize;
  bpSize vIndexHeadSecond;

  bpDouble vSumOfWeights = 0;
  bpDouble vMeasuredTime = 0;
  do {
    vIndexHeadFirst = mMeasurements.GetPreviousIndex(vIndexHeadFirst) % vBufferSize;
    vIndexHeadSecond = mMeasurements.GetPreviousIndex(vIndexHeadFirst) % vBufferSize;
    auto vValuePair1 = mMeasurements.Get(vIndexHeadFirst);
    auto vValuePair2 = mMeasurements.Get(vIndexHeadSecond);
    // Calculate speed in MB/s.
    bpDouble vBlockTimeDelta = vValuePair1.second - vValuePair2.second;
    if (vBlockTimeDelta <= 0) {
      continue;
    }
    bpDouble vSpeed = vValuePair1.first / vBlockTimeDelta;
    bpDouble vBlocksTimeAverage = vBlockTimeDelta / 2.0;
    bpDouble vWeight = exp(-1.0*((vBlocksTimeAverage - aCurrentTime)*(vBlocksTimeAverage - aCurrentTime)) / (2 * aTimeWindow*aTimeWindow));
    vSumOfWeights += vWeight;
    vMeasuredTime += vBlockTimeDelta;
    vSumOfWeightedSpeeds += vSpeed * vWeight;
  } while (vIndexHeadFirst != vTailIndex);

  if (vSumOfWeights == 0) {
    return 0;
  }
  if (1.0 - vMeasuredTime > 0) {
    return (vSumOfWeightedSpeeds / vSumOfWeights) * vMeasuredTime;
  }
  return (vSumOfWeightedSpeeds / vSumOfWeights);
}
