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


#include "bpFileReaderFactory.h"

#include "bpFileTools.h"
#include "bpUtils.h"


bpFileReaderFactory::bpFileReaderFactory()
  : mAllowTimeReaders(true)
{
}


bpFileReaderFactory::~bpFileReaderFactory()
{
}


bpFileReaderFactory::Iterator bpFileReaderFactory::FormatBegin()
{
  return mFormats.begin();
}


bpFileReaderFactory::Iterator bpFileReaderFactory::FormatEnd()
{
  return mFormats.end();
}


void bpFileReaderFactory::SetPluginsPath(const bpString& aPluginsPath)
{
}


void bpFileReaderFactory::AddFormat(const bpString& aFormatName,
                                    const bpString& aFormatDescription,
                                    const std::vector<bpString>& aFormatExtension)
{
  mFormats.push_back(aFormatName);
  mDescriptions[aFormatName] = aFormatDescription;
  mExtensions[aFormatName] = aFormatExtension;
}


void bpFileReaderFactory::MoveFormatToFront(const bpString& aFormat)
{
  std::list<bpString>::iterator vFormatPos = std::find(mFormats.begin(), mFormats.end(), aFormat);
  if (vFormatPos != mFormats.end()) {
    mFormats.erase(vFormatPos);
    mFormats.push_front(aFormat);
  }
}


void bpFileReaderFactory::RemoveFormat(const bpString& aFormat)
{
  std::list<bpString>::iterator vFormatPos = std::find(mFormats.begin(), mFormats.end(), aFormat);
  if (vFormatPos != mFormats.end()) {
    mFormats.erase(vFormatPos);
    mDescriptions.erase(aFormat);
    mExtensions.erase(aFormat);
  }
}


/**
 * Returns the file format string for the factory according to the descrition
 * of a file reader.
 */
bpString bpFileReaderFactory::GetFormatFromDescription(const bpString& aDescription)
{
  std::map<bpString, bpString>::const_iterator vIt = mDescriptions.begin();
  while (vIt != mDescriptions.end()) {
    const bpString& vDescription = (*vIt).second;
    if (vDescription == aDescription) {
      const bpString& vFormat = (*vIt).first;
      return vFormat;
    }
    ++vIt;
  }
  return "";
}


/**
 * Get Format of a File, if possible
 * throws bpNotSupportedException otherwise
 *
 * @param aFileName
 *
 * @return bpString
 */
bpString bpFileReaderFactory::GetFileFormat(const bpString& aFileName)
{
  std::list<bpString> vUnusedFormats;

  // try formats with correct extension first
  Iterator vFormatIt = mFormats.begin();
  while (vFormatIt != mFormats.end()) {
    if (ExtensionMatchesFormat(aFileName, *vFormatIt)) {
      try {
        bpFileReader::tPtr vFileReader = CreateFileReader(aFileName, *vFormatIt);
        return *vFormatIt;
      }
      catch (...) {
      }
    } else {
      vUnusedFormats.push_back(*vFormatIt);
    }
    ++vFormatIt;
  }
  // then try all formats that we have not yet used
  vFormatIt = vUnusedFormats.begin();
  while (vFormatIt != vUnusedFormats.end()) {
    try {
      bpFileReader::tPtr vFileReader = CreateFileReader(aFileName, *vFormatIt);
      return *vFormatIt;
    }
    catch (...) {
    }
    ++vFormatIt;
  }
  throw std::runtime_error("Format not supported");
}


bpFileReader::tPtr bpFileReaderFactory::CreateFileReader(const bpString& aFileName)
{
  std::list<bpString> vUnusedFormats;
  // try formats with correct extension first
  Iterator vFormatIt = mFormats.begin();
  while (vFormatIt != mFormats.end()) {
    if (ExtensionMatchesFormat(aFileName, *vFormatIt)) {
      try {
        bpFileReader::tPtr vFileReader = CreateFileReader(aFileName, *vFormatIt);
        return vFileReader;
      }
      catch (...) {
      }
    } else {
      vUnusedFormats.push_back(*vFormatIt);
    }
    ++vFormatIt;
  }
  // then try all formats
  vFormatIt = vUnusedFormats.begin();
  while (vFormatIt != vUnusedFormats.end()) {
    try {
      bpFileReader::tPtr vFileReader = CreateFileReader(aFileName, *vFormatIt);
      return vFileReader;
    }
    catch (...) {
    }
    ++vFormatIt;
  }
  throw std::runtime_error("Unknown File Format.");
}


bpString bpFileReaderFactory::GetFormatDescription(const bpString& aFormat) const
{
  std::map<bpString, bpString>::const_iterator vDescriptit = mDescriptions.find(aFormat);
  if (vDescriptit != mDescriptions.end()) {
    return(*vDescriptit).second;
  }
  return "";
}


std::vector<bpString> bpFileReaderFactory::GetFormatExtensions(const bpString& aFormat) const
{
  std::vector<bpString> vFormatExtensions;

  auto vIterator = mExtensions.find(aFormat);

  if (mExtensions.end() != vIterator) {
    vFormatExtensions = vIterator->second;
    if (vFormatExtensions.begin() != vFormatExtensions.end()) {
      for (bpSize vIndex = 0; vIndex < vFormatExtensions.size(); vIndex++) {
        vFormatExtensions[vIndex] = bpToUpper(vFormatExtensions[vIndex]);
      }
    }
  }

  return vFormatExtensions;
}


bool bpFileReaderFactory::ExtensionMatchesFormat(const bpString& aFileName, const bpString& aFormat, bool aIsPath) const
{
  auto vFileExtension = aIsPath ? bpToUpper(bpFileTools::GetExt(aFileName)) : aFileName;
  auto vFormatExtensions = GetFormatExtensions(aFormat);
  auto vIterator = std::find(vFormatExtensions.begin(), vFormatExtensions.end(), vFileExtension);

  return (vFormatExtensions.end() != vIterator);
}
