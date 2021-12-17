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


#ifndef __BP_FILE_INFO__
#define __BP_FILE_INFO__


#include "bpFileReader.h"
#include "bpObjectDescriptor.h"


class bpFileInfo
{
public:
  static bpString GetXML_AllFileNames(const bpString& aFileName, const bpFileReader::tPtr& aFileReader, const bpString& aIndent = "");
  static bpString GetXML_ReadMetaData(const bpString& aFileName, const bpFileReader::tPtr& aFileReader, bpSize aImageIndex, const bpString& aIndent = "");
  static bpString GetXML_ReadMetaDataForArena(const bpString& aFileName, const bpFileReader::tPtr& aFileReader, bpSize aImageIndex, const bpString& aIndent = "", const std::vector<bpObjectDescriptor>& aObjectDescriptors = {});
  static bpString GetXML_ReadVoxelHash(const bpString& aFileName, const bpFileReader::tPtr& aFileReader, bpSize aImageIndex, const bpString& aBlockSort, const bpString& aIndent = "");

private:
  static bool ReadMetaData(
    const bpString& aFileName,
    const bpFileReader::tPtr& aFileReader,
    const bpString& aIndent,
    bpString& aXML,
    bpString& aExceptionText);

  static bool ReadMetaDataForArena(
    const bpString& aFileName,
    const bpFileReader::tPtr& aFileReader,
    const bpString& aIndent,
    bpString& aXML,
    bpString& aExceptionText);

  static bool ReadVoxelHash(
    const bpString& aFileName,
    const bpFileReader::tPtr& aFileReader,
    const bpString& aBlockSort,
    bpUInt32& aVoxelHash,
    bpUInt64& aVoxelBytes,
    bpString& aExceptionText);
};

#endif // __BP_FILE_INFO__

