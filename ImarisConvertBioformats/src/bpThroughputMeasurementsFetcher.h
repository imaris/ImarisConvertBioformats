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


#ifndef __BP_THROUGHPUT_MEASUREMENTS_FETCHER_H__
#define __BP_THROUGHPUT_MEASUREMENTS_FETCHER_H__


#include "ImarisWriter/interface/bpConverterTypes.h"


class bpThroughputMeasurementsFetcher
{
public:
  void Start(bpUInt32 aRepeatTimeMs);
  void Stop();

private:
  class cImpl;
  bpSharedPtr<cImpl> mImpl;
};


#endif // __BP_THROUGHPUT_MEASUREMENTS_FETCHER_H__
