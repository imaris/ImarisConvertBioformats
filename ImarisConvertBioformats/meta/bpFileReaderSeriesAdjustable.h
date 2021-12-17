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


#ifndef __BP_FILE_READER_SERIES_ADJUSTABLE__
#define __BP_FILE_READER_SERIES_ADJUSTABLE__


#include "bpFileSeriesUtils.h"
#include "bpFileReaderSeriesFileCheck.h"
#include "bpFileSeriesDelimiters.h"


#include <set>


class bpFileReaderSeriesAdjustable
{
public:
  typedef bpFileReaderImpl::tFileLayout tFileLayout;

  explicit bpFileReaderSeriesAdjustable(const bpFileSeriesDelimiters& aFileSeriesDelimiters);
  virtual ~bpFileReaderSeriesAdjustable();

  void EvalFilesAndSetSeries(const bpString& aFileName, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck);

  const std::vector<tFileLayout>& GetFileLayouts() const;

private:
  typedef bpFileReaderImpl::bpImageIndex tImageIndex;

  class bpFileLayout
  {
  public:
    typedef std::map<bpString, tImageIndex > tLayout;

    bpFileLayout() {
    }

    bpFileLayout(const std::vector<tLayout>& aLayouts)
      : mFileLayouts(aLayouts) {
      mSize = CalculateSize(mFileLayouts);
      mFilesByIndex = BuildFilesByIndex(mFileLayouts);
    }

    bpFileLayout(const bpFileLayout& aOther)
      : mFileLayouts(aOther.mFileLayouts) {
      mSize = CalculateSize(mFileLayouts);
      mFilesByIndex = BuildFilesByIndex(mFileLayouts);
    }

    bpFileLayout& operator = (const bpFileLayout& aOther) {
      mFileLayouts = aOther.mFileLayouts;
      mSize = CalculateSize(mFileLayouts);
      mFilesByIndex = BuildFilesByIndex(mFileLayouts);
      return *this;
    }

    bpFileLayout& operator = (const std::vector<tLayout>& aLayouts) {
      mFileLayouts = aLayouts;
      mSize = CalculateSize(mFileLayouts);
      mFilesByIndex = BuildFilesByIndex(mFileLayouts);
      return *this;
    }

    tImageIndex GetSize() const {
      return mSize;
    }

    const std::vector<tLayout>& GetLayouts() const {
      return mFileLayouts;
    }

    const std::pair<bpSize, tImageIndex> GetImageIndex(const bpString& aFileName) const {
      for (bpSize vLayoutIndex = 0; vLayoutIndex < mFileLayouts.size(); ++vLayoutIndex) {
        tLayout::const_iterator vIterator = mFileLayouts[vLayoutIndex].find(aFileName);
        if (vIterator != mFileLayouts[vLayoutIndex].end()) {
          return std::pair<bpSize, tImageIndex>(vLayoutIndex, (*vIterator).second);
        }
      }
      return std::pair<bpSize, tImageIndex>();
    }

    const bpString GetFileName(bpSize aLayoutIndex, const tImageIndex& aImageIndex) const {
      std::map<tImageIndex, bpString>::const_iterator vIterator = mFilesByIndex[aLayoutIndex].find(aImageIndex);
      if (vIterator != mFilesByIndex[aLayoutIndex].end()) {
        return (*vIterator).second;
      }
      return bpString();
    }

    std::vector<bpString> GetFileList(bpSize aLayoutIndex, const std::vector<bpFileReaderImpl::Dimension>& aDimensionIncrementSequence) const {
      std::vector<bpString> vFileList;
      tImageIndex vImageIndexFirst = (*mFilesByIndex[aLayoutIndex].begin()).first;
      bpFileReaderImpl::Dimension vDim4 = aDimensionIncrementSequence[4];
      tImageIndex vIndex;
      for (vIndex[vDim4] = vImageIndexFirst[vDim4]; vIndex[vDim4] < mSize[vDim4]; ++vIndex[vDim4]) {
        bpFileReaderImpl::Dimension vDim3 = aDimensionIncrementSequence[3];
        for (vIndex[vDim3] = vImageIndexFirst[vDim3]; vIndex[vDim3] < mSize[vDim3]; ++vIndex[vDim3]) {
          bpFileReaderImpl::Dimension vDim2 = aDimensionIncrementSequence[2];
          for (vIndex[vDim2] = vImageIndexFirst[vDim2]; vIndex[vDim2] < mSize[vDim2]; ++vIndex[vDim2]) {
            bpFileReaderImpl::Dimension vDim1 = aDimensionIncrementSequence[1];
            for (vIndex[vDim1] = vImageIndexFirst[vDim1]; vIndex[vDim1] < mSize[vDim1]; ++vIndex[vDim1]) {
              bpFileReaderImpl::Dimension vDim0 = aDimensionIncrementSequence[0];
              for (vIndex[vDim0] = vImageIndexFirst[vDim0]; vIndex[vDim0] < mSize[vDim0]; ++vIndex[vDim0]) {
                std::map<tImageIndex, bpString>::const_iterator vFilesByIndexIt = mFilesByIndex[aLayoutIndex].find(vIndex);
                if (vFilesByIndexIt != mFilesByIndex[aLayoutIndex].end()) {
                  bpString vFileName = (*vFilesByIndexIt).second;
                  if (!vFileName.empty()) {
                    vFileList.push_back(vFileName);
                  }
                }
              }
            }
          }
        }
      }
      return vFileList;
    }

    static tLayout FileListToLayout(const std::vector<bpFileReaderImpl::Dimension>& aDimensionIncrementSequence,
      const tImageIndex aSize,
      const std::vector<bpString>& aFileList) {
      tLayout vLayout;
      bpFileReaderImpl::Dimension vDim4 = aDimensionIncrementSequence[4];
      bpSize vFileCount = 0;
      tImageIndex vIndex;
      for (vIndex[vDim4] = 0; vIndex[vDim4] < aSize[vDim4]; ++vIndex[vDim4]) {
        bpFileReaderImpl::Dimension vDim3 = aDimensionIncrementSequence[3];
        for (vIndex[vDim3] = 0; vIndex[vDim3] < aSize[vDim3]; ++vIndex[vDim3]) {
          bpFileReaderImpl::Dimension vDim2 = aDimensionIncrementSequence[2];
          for (vIndex[vDim2] = 0; vIndex[vDim2] < aSize[vDim2]; ++vIndex[vDim2]) {
            bpFileReaderImpl::Dimension vDim1 = aDimensionIncrementSequence[1];
            for (vIndex[vDim1] = 0; vIndex[vDim1] < aSize[vDim1]; ++vIndex[vDim1]) {
              bpFileReaderImpl::Dimension vDim0 = aDimensionIncrementSequence[0];
              for (vIndex[vDim0] = 0; vIndex[vDim0] < aSize[vDim0]; ++vIndex[vDim0]) {
                if (vFileCount < aFileList.size()) {
                  const bpString& vFileName = aFileList[vFileCount];
                  vLayout[vFileName] = vIndex;
                  ++vFileCount;
                }
              }
            }
          }
        }
      }
      return vLayout;
    }


  private:
    static tImageIndex CalculateSize(const std::vector<tLayout>& aLayout) {
      tImageIndex vSize;
      for (std::map<bpString, tImageIndex >::const_iterator vFileLayout = aLayout[0].begin();
        vFileLayout != aLayout[0].end(); ++vFileLayout) {
        const tImageIndex& vFileIndex = (*vFileLayout).second;
        for (bpSize vDimension = 0; vDimension < 5; ++vDimension) {
          vSize[bpFileReaderImpl::X] = vFileIndex[bpFileReaderImpl::X] > vSize[bpFileReaderImpl::X] ?
            vFileIndex[bpFileReaderImpl::X] : vSize[bpFileReaderImpl::X];
          vSize[bpFileReaderImpl::Y] = vFileIndex[bpFileReaderImpl::Y] > vSize[bpFileReaderImpl::Y] ?
            vFileIndex[bpFileReaderImpl::Y] : vSize[bpFileReaderImpl::Y];
          vSize[bpFileReaderImpl::Z] = vFileIndex[bpFileReaderImpl::Z] > vSize[bpFileReaderImpl::Z] ?
            vFileIndex[bpFileReaderImpl::Z] : vSize[bpFileReaderImpl::Z];
          vSize[bpFileReaderImpl::C] = vFileIndex[bpFileReaderImpl::C] > vSize[bpFileReaderImpl::C] ?
            vFileIndex[bpFileReaderImpl::C] : vSize[bpFileReaderImpl::C];
          vSize[bpFileReaderImpl::T] = vFileIndex[bpFileReaderImpl::T] > vSize[bpFileReaderImpl::T] ?
            vFileIndex[bpFileReaderImpl::T] : vSize[bpFileReaderImpl::T];
        }
      }
      vSize[bpFileReaderImpl::X] += 1;
      vSize[bpFileReaderImpl::Y] += 1;
      vSize[bpFileReaderImpl::Z] += 1;
      vSize[bpFileReaderImpl::C] += 1;
      vSize[bpFileReaderImpl::T] += 1;
      return vSize;
    }

    static std::vector<std::map<tImageIndex, bpString> > BuildFilesByIndex(const std::vector<tLayout>& aLayout) {
      std::vector<std::map<tImageIndex, bpString> > vFilesByIndex;
      for (auto vLayoutIt = aLayout.begin(); vLayoutIt != aLayout.end(); ++vLayoutIt) {
        vFilesByIndex.push_back(std::map<tImageIndex, bpString>());
        for (std::map<bpString, tImageIndex >::const_iterator vFileLayout = vLayoutIt->begin();
          vFileLayout != vLayoutIt->end(); ++vFileLayout) {
          const bpString& vFileName = (*vFileLayout).first;
          const tImageIndex& vFileIndex = (*vFileLayout).second;
          vFilesByIndex.back()[vFileIndex] = vFileName;
        }
      }
      return vFilesByIndex;
    }
  private:
    std::vector<tLayout> mFileLayouts;
    std::vector<std::map<tImageIndex, bpString> > mFilesByIndex;
    tImageIndex mSize;
  };

  bool GetEnableTilingXY() const;
  bpString GetFileName() const;

  enum tDimension {
    DimensionNone,
    DimensionX,
    DimensionY,
    DimensionZ,
    DimensionC,
    DimensionT,
    DimensionF
  };

  class cIndexSize
  {
  public:
    cIndexSize(bpSize aIndex, bpSize aSize)
      : mIndex(aIndex),
        mSize(aSize) {
    }
    static bool Less(const cIndexSize& aIndexSize1, const cIndexSize& aIndexSize2) {
      if (aIndexSize1.mSize != aIndexSize2.mSize) {
        return aIndexSize1.mSize < aIndexSize2.mSize;
      }
      else {
        return aIndexSize1.mIndex < aIndexSize2.mIndex;
      }
    }
    bpSize mIndex;
    bpSize mSize;
  };

  bpFileSeriesDelimiters mFileSeriesDelimiters;

  bool mIsTooManyMissingFiles;

  bool mEnableTilingXY;

  bpString mReferenceFileName;
  std::set<bpString> mSeriesFileNames;
  bpFileLayout mFileLayout;
  std::vector<tDimension> mIndexDimension;

  class cDimensionSize
  {
  public:
    cDimensionSize()
      : mDimension(DimensionNone),
        mSize(0) {
    }
    cDimensionSize(tDimension aDimension, bpSize aSize)
      : mDimension(aDimension),
        mSize(aSize) {
    }
    tDimension mDimension;
    bpSize mSize;
  };

  std::vector<cDimensionSize> mDimensionSizes;

  enum tDelimiterType {
    eWithDelimiter,
    eWithoutDelimiter
  };

  tDelimiterType mDelimiterType;

  static void SplitFileName(const bpString& aFileName,
                            std::vector<bpString>& aSubStrings,
                            std::vector<bpSize>& aIndices);

  static std::vector<std::map<bpString, tImageIndex> > CreateLayouts(const std::set<bpString>& aFileNames,
                                                      const std::vector<tDimension>& aIndexDimension);

  static std::vector<std::map<bpString, tImageIndex> > CreateLayoutsWithoutDelimiter(const std::set<bpString>& aFileNames,
      const std::vector<cDimensionSize>& aDimensionSizes);

  void GuessDimensionIndicesFromFileName(const bpString& aFileName);
  void GuessDimensionIndicesFromSize(const std::vector<bpSize>& aIndexSize);

  static bpFileReaderImpl::Dimension GetFileReaderDimension(tDimension aDimension);
  static std::vector<cDimensionSize> GetDimensionSizes(const std::vector<tDimension>& aIndexDimension,
                                                       const bpFileLayout& aFileLayout);

  static bpSize GetNumberOfRequiredFiles(const std::vector<tDimension>& aIndexDimension,
                                         const bpFileLayout& aFileLayout);

  bpSize GetFileNameNumberOfIndices() const;
  std::vector<bpSize> GetIndexSize() const;
};

#endif
