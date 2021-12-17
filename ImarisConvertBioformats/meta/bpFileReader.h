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


#ifndef __BP_FILE_READER__
#define __BP_FILE_READER__


#include "bpFileReaderImpl.h"
#include "bpFileReaderConfiguration.h"

#include <sstream>

class bpFileReader
{
public:
  using tPtr = bpSharedPtr<bpFileReader>;
  using tImplPtr = bpSharedPtr<bpFileReaderImpl>;
  using tConfigPtr = bpSharedPtr<bpFileReaderConfiguration>;
  using tFileLayout = bpFileReaderImpl::tFileLayout;

  explicit bpFileReader(tImplPtr aImpl)
    : mImpl(std::move(aImpl))
  {
    if (mImpl) {
      mConfig = std::make_shared<bpFileReaderConfiguration>();
    }
  }

  virtual ~bpFileReader()
  {
  }

  const tImplPtr& GetReaderImpl() const
  {
    return mImpl;
  }

  const tConfigPtr& GetConfig() const
  {
    return mConfig;
  }

  virtual std::vector<bpString> GetAllFileNamesOfDataSet()
  {
    return mImpl ? mImpl->GetAllFileNamesOfDataSet(mImpl->GetFileName()) : std::vector<bpString>();
  }

  virtual bpString GetDescription() const
  {
    return mImpl ? mImpl->GetReaderDescription() : "";
  }

  virtual bpSharedPtr<bpThumbnail> ReadThumbnail() const
  {
    return mImpl ? mImpl->ReadThumbnail() : bpSharedPtr<bpThumbnail>();
  }

  virtual std::vector<bpString> GetFileExtension()
  {
    return mImpl ? mImpl->GetReaderExtension() : std::vector<bpString>();
  }

  virtual bpSize GetNumberOfDataSets() const
  {
    return mImpl ? mImpl->GetNumberOfDataSets() : 0;
  }

  virtual void SetActiveDataSetIndex(bpSize aIndex)
  {
    if (mImpl) {
      mImpl->SetActiveDataSetIndex(aIndex);
    }
  }

  virtual bpSize GetActiveDataSetIndex() const
  {
    return mImpl ? mImpl->GetActiveDataSetIndex() : 0;
  }

  bool HasSettings() const
  {
    return mImpl && (mImpl->SeriesConfigurable() || mImpl->GetNumberOfDataSets() > 1 || mImpl->SceneConfigurable());
  }

  bool IsTimeReader() const
  {
    return true;
  }

  bool CanCrop() const
  {
    return !!mImpl;
  }

  bool CanResample() const
  {
    return !!mImpl;
  }

  bool NaNCheckStatus() const
  {
    return false;
  }

  void SetFileSeriesLayoutXmls(const std::vector<bpString>& aFileSeriesLayoutXml);
  std::vector<bpString> GetFileSeriesLayoutXmls();

  static bpString FileLayoutToXMLString(const tFileLayout& aLayout);
  static tFileLayout FileLayoutFromXMLString(const bpString& aLayoutString);

private:
  tImplPtr mImpl;
  tConfigPtr mConfig;
};



#endif // __BP_IMAGEDESCRIPTOR_
