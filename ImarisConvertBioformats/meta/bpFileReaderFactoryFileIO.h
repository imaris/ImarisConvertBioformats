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


#ifndef __BP_FILE_READER_FACTORY_FILE_IO__
#define __BP_FILE_READER_FACTORY_FILE_IO__


#include "bpFileReaderFactory.h"


class bpFileReaderFactoryFileIO : public bpFileReaderFactory
{
public:
  bpFileReaderFactoryFileIO();
  ~bpFileReaderFactoryFileIO();

  void SetPluginsPath(const bpString& aPluginsPath);
  void AddFactoryImpl(const bpSharedPtr<bpfFileReaderImplFactoryBase>& aFactory);
  bpSharedPtr<bpfFileReaderImplFactoryBase> GetFactoryImpl() const;

  bpFileReader::tPtr CreateFileReader(const bpString& aFileName, const bpString& aFormat);

private:
  void AddFormats();
  void AddFormats(const bpSharedPtr<bpfFileReaderImplFactoryBase>& aFactory);
  std::list<bpString> ReadFormats();
  void CleanupFormats();

  std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>> mFileReaderImplFactories;
  bpString mPluginsPath;
};


#endif
