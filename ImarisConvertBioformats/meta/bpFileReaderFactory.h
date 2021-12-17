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


#ifndef __BP_FILE_READER_FACTORY__
#define __BP_FILE_READER_FACTORY__

#include "bpFileReader.h"

#include <list>
#include <map>


class bpfFileReaderImplFactoryBase;


class bpFileReaderFactory
{
public:
  bpFileReaderFactory();
  virtual ~bpFileReaderFactory();

  using Iterator = std::list<bpString>::iterator;

  Iterator FormatBegin();
  Iterator FormatEnd();
  bool ExtensionMatchesFormat(const bpString& aFileName, const bpString& aFormat, bool aIsPath = true) const;
  virtual void SetPluginsPath(const bpString& aPluginsPath);

  virtual void AddFactoryImpl(const bpSharedPtr<bpfFileReaderImplFactoryBase>& aFactory) = 0;
  virtual bpSharedPtr<bpfFileReaderImplFactoryBase> GetFactoryImpl() const = 0;

protected:

  /**
   * Derived classes can add formats
   *
   * @param aFormatName
   * @param aFormatDescription
   * @param aFormatExtension
   */
  void AddFormat(const bpString& aFormatName, const bpString& aFormatDescription, const std::vector<bpString>& aFormatExtension);

public:

  /**
   * Get Description of Format
   *
   * @param aFormat
   *
   * @return bpString
   */
  bpString GetFormatDescription(const bpString& aFormat) const;

  /**
   * Get format file extensions. An empty string element means that the file reader may open the file without requiring a named extension.
   *
   * @param aFormat
   *
   * @return std::vector<bpString>
   */
  std::vector<bpString> GetFormatExtensions(const bpString& aFormat) const;

  /**
   * Removes a Format from the list of creatable formats.
   */
  void RemoveFormat(const bpString& aFormat);

  /**
   * Moves a format within the format list to the first position
   */
  void MoveFormatToFront(const bpString& aFormat);

  /**
   * Returns the file format string for the factory according to the descrition
   * of a file reader.
   */
  bpString GetFormatFromDescription(const bpString& aDescription);

  /**
   * Get Format of a File, if possible
   * throws bpNotSupportedException otherwise
   *
   * @param aFileName
   *
   * @return bpString
   */
  bpString GetFileFormat(const bpString& aFileName);

  /**
   * Create a FileReader to open aFileName
   *
   * tests all formats and returns first reader that fits
   *
   * @param aFileName
   *
   * @return bpFileReader*
   */
  bpFileReader::tPtr CreateFileReader(const bpString& aFileName);


  /**
   * Concrete Factories are to overwrite this function
   *
   * @param aFileName
   * @param aFormat
   *
   * @return bpFileReader*
   */
  virtual bpFileReader::tPtr CreateFileReader(const bpString& aFileName, const bpString& aFormat) = 0;


  /**
   *
   * deprecated
   *
   *
   */


  /**
   * Prohibits creation of time series readers
   */
  void RemoveTimeDueToMissingLicense() {
    mAllowTimeReaders = false;
  }

  /**
   * Check, if it's allowed to create time series readers
   */
  bool AllowTimeReaders() {
    return mAllowTimeReaders;
  }

  void SetTimeDefault(bool /* aDefault */) {
  }


protected:


  /**
   * The list of supported formats.
   */
  std::list<bpString> mFormats;

  bool mAllowTimeReaders;

  std::map<bpString, bpString> mDescriptions;
  std::map<bpString, std::vector<bpString> > mExtensions;
};


#endif
