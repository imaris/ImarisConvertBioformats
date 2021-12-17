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


#ifndef __BP_IMAGE_CONVERT_NEW__
#define __BP_IMAGE_CONVERT_NEW__

#include "ImarisWriter/interface/bpConverterTypes.h"
#include "../meta/bpFileReader.h"


class bpImageConvertNew
{
public:
  using tReaderPtr = bpFileReader::tPtr;

  enum tWriteMode
  {
    eWriteHDF5,
    eWriteThumbnailOnly
  };

  struct cConvertOptions
  {
    tWriteMode mWriteMode = eWriteHDF5;
    bool mCompressThumbnail = false;
  };

  static void Convert(const tReaderPtr& aReader, const bpString& aOutputFile, const cConvertOptions& aConvertOptions, const bpConverterTypes::cOptions& aWriteOptions);

private:
  class cImpl;
};

#endif
