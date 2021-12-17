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


#ifndef __BP_WRITER_FILE_THUMBNAIL__
#define __BP_WRITER_FILE_THUMBNAIL__

#include "../src/bpWriterCommonHeaders.h"
#include ICWriter_h

class bpWriterFileThumbnail : public bpWriter
{
public:
  enum class tFormat
  {
    ePNG,
    eJPEG
  };

  bpWriterFileThumbnail(const bpString& aFilename, tFormat aFormat);
  virtual ~bpWriterFileThumbnail();

  virtual void WriteHistogram(const bpHistogram& aHistogram, bpSize aIndexT, bpSize aIndexC, bpSize aIndexR) {}

  virtual void WriteMetadata(
    const bpString& aApplicationName,
    const bpString& aApplicationVersion,
    const bpConverterTypes::cImageExtent& aImageExtent,
    const bpConverterTypes::tParameters& aParameters,
    const bpConverterTypes::tTimeInfoVector& aTimeInfoPerTimePoint,
    const bpConverterTypes::tColorInfoVector& aColorInfoPerChannel) {}

  virtual void WriteThumbnail(const bpThumbnail& aThumbnail);

  virtual void WriteDataBlock(
    const void* aData, bpSize aDataSize,
    bpSize aBlockIndexX, bpSize aBlockIndexY, bpSize aBlockIndexZ,
    bpSize aIndexT, bpSize aIndexC, bpSize aIndexR) {}

private:
  bpString mFilename;
  tFormat mFormat;
};

#endif
