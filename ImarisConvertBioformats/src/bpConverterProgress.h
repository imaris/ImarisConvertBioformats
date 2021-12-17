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


#ifndef __BP_CONVERTER_PROGRESS__
#define __BP_CONVERTER_PROGRESS__


#include "ImarisWriter/interface/bpConverterTypes.h"

#include <chrono>


class bpConverterProgress
{
public:
  bpConverterProgress(bpSize aNumberOfIncrements, bool aIsWriteprogress);

  void Increment();

  void SetPosition(bpFloat aPercent);

private:
  using tClock = std::chrono::system_clock;

  bpSize mCurrentNumberOfIncrements;
  bpSize mTotalNumberOfIncrements;
  bool mIsWriteprogress;
  tClock::time_point mLastIncrement;
};

#endif
