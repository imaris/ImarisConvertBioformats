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


#include "bpConverterProgress.h"

#include <iostream>
#include <iomanip>


bpConverterProgress::bpConverterProgress(bpSize aNumberOfIncrements, bool aIsWriteprogress)
  : mCurrentNumberOfIncrements(0),
    mTotalNumberOfIncrements(aNumberOfIncrements),
    mIsWriteprogress(aIsWriteprogress),
    mLastIncrement(tClock::now())
{
}

void bpConverterProgress::Increment()
{
  ++mCurrentNumberOfIncrements;
  auto vNow = tClock::now();
  if (vNow - mLastIncrement < std::chrono::milliseconds(250) && mCurrentNumberOfIncrements < mTotalNumberOfIncrements) {
    return;
  }
  mLastIncrement = vNow;

  bpFloat vProgress = static_cast<bpFloat>(mCurrentNumberOfIncrements) / mTotalNumberOfIncrements;
  std::cout << (mIsWriteprogress ? "Write" : "Read") << "Progress: " << std::fixed << std::setprecision(3) << vProgress << std::endl;
}

void bpConverterProgress::SetPosition(bpFloat aCurrentPercent)
{
  auto vNow = tClock::now();
  if (vNow - mLastIncrement < std::chrono::milliseconds(250) && aCurrentPercent < 1.0f) {
    return;
  }
  mLastIncrement = vNow;

  std::cout << (mIsWriteprogress ? "Write" : "Read") << "Progress: " << std::fixed << std::setprecision(3) << aCurrentPercent << std::endl;
}
