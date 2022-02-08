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


#ifndef __BP_FILE_READER_BIOFORMATS_IMPL_FACTORY__
#define __BP_FILE_READER_BIOFORMATS_IMPL_FACTORY__

#include "fileiobase/application/bpfFileReaderImplFactoryBase.h"
#include "fileiobioformats/application/bpFileIOBioformatsDllAPI.h"

#include <map>


class BP_FILEIOBIOFORMATS_DLL_API bpfFileReaderBioformatsImplFactory : public bpfFileReaderImplFactoryBase
{
public:
  bpfFileReaderBioformatsImplFactory();
  virtual ~bpfFileReaderBioformatsImplFactory();

  virtual bpfFileReaderImplInterface* CreateFileReader(const bpfString& aFileName) override;
  virtual bpfFileReaderImplInterface* CreateFileReader(const bpfString& aFileName, const bpfString& aFormatName) override;
  virtual Iterator FormatBegin() override;
  virtual Iterator FormatEnd() override;
  virtual bpfString GetFormatDescription(const bpfString& aFormat) const override;
  virtual std::vector<bpfString> GetFormatExtensions(const bpfString& aFormat) const override;
  virtual bpfString GetFileFormat(const bpfString& aFileName) override;
  virtual void AddPluginsFormats(const bpfString& aPluginsPath) override;

  virtual bpfString GetVersion() const override;

private:

#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

  bpfString GetDefaultJVMPath();
  bpfString GetBioformatsVersion() const;
  void GetSupportedBioformatsFormats(std::vector<bpfString>&  aFormats, std::vector<std::vector<bpfString>>& aExtensions) const;

  std::list<bpfString> mFormats;
  std::map<bpfString, bpfString> mDescriptions;
  std::map<bpfString, std::vector<bpfString>> mExtensions;

#if defined (_MSC_VER)
#pragma warning(pop)
#endif
};


#endif
