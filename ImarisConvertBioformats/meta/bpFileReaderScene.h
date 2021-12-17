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


#ifndef __BP_FILE_READER_SCENE__
#define __BP_FILE_READER_SCENE__


#include "bpFileReader.h"


class bpFileReaderScene : public bpFileReader
{
public:
  explicit bpFileReaderScene(const bpString& aFileName);

  virtual std::vector<bpString> GetAllFileNamesOfDataSet();

  virtual bpString GetDescription() const;

  virtual bpSharedPtr<bpThumbnail> ReadThumbnail() const;

  virtual std::vector<bpString> GetFileExtension();


  static bpString GetFormatDescription();
  static std::vector<bpString> GetFileFormatExtension();

private:
  static void CheckImxScene(const bpString& aFileName);
  static void CheckImsScene(const bpString& aFileName);

  bpString mFileName;
};

#endif
