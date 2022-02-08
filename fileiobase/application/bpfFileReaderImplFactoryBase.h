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


#ifndef __BPFILEREADERIMPLFACTORYBASE__
#define __BPFILEREADERIMPLFACTORYBASE__


#include "fileiobase/application/bpfFileReaderImplInterface.h"


class bpfFileReaderImplFactoryBase
{
public:
  using Iterator = std::list<bpfString>::iterator;

  virtual ~bpfFileReaderImplFactoryBase() {}

  virtual bpfFileReaderImplInterface* CreateFileReader(const bpfString& aFileName) = 0;
  virtual bpfFileReaderImplInterface* CreateFileReader(const bpfString& aFileName, const bpfString& aFormatName) = 0;
  virtual Iterator FormatBegin() = 0;
  virtual Iterator FormatEnd() = 0;
  virtual bpfString GetFormatDescription(const bpfString& aFormat) const = 0;
  virtual std::vector<bpfString> GetFormatExtensions(const bpfString& aFormat) const = 0;
  virtual bpfString GetFileFormat(const bpfString& aFileName) = 0;
  virtual void AddPluginsFormats(const bpfString& aPluginsPath) = 0;
  virtual bpfString GetVersion() const = 0;
};


#endif
