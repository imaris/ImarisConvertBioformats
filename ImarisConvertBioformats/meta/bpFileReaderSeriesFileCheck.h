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


#ifndef __BP_FILE_READER_SERIES_ADJUSTABLE_FILE_CHECK__
#define __BP_FILE_READER_SERIES_ADJUSTABLE_FILE_CHECK__


#include "bpFileReaderImpl.h"


class bpFileReaderSeriesFileCheck
{
public:
  explicit bpFileReaderSeriesFileCheck(bpFileReaderImpl* aFileReaderImpl)
    : mFileReaderImpl(aFileReaderImpl)
  {
  }

  virtual ~bpFileReaderSeriesFileCheck()
  {
  }

  /**
   * Checks, if the file matches to the other files of a series.
   */
  virtual bool SeriesFileValid(bpString aFileName)
  {
    return mFileReaderImpl->SeriesFileValid(aFileName);
  }

  bpSize GetMaxSeriesSizeToCheck() const
  {
    return 10;
  }

private:
  bpFileReaderImpl* mFileReaderImpl;
};

#endif // __BP_FILE_READER_SERIES_ADJUSTABLE_FILE_CHECK__

