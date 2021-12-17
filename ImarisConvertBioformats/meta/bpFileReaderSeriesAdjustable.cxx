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


#include "bpFileReaderSeriesAdjustable.h"

#include "bpFileTools.h"

#include <limits>


bpFileReaderSeriesAdjustable::bpFileReaderSeriesAdjustable(const bpFileSeriesDelimiters& aFileSeriesDelimiters)
  : mFileSeriesDelimiters(aFileSeriesDelimiters),
    mEnableTilingXY(false),
    mIsTooManyMissingFiles(false)
{
}


bpFileReaderSeriesAdjustable::~bpFileReaderSeriesAdjustable()
{
}


void bpFileReaderSeriesAdjustable::GuessDimensionIndicesFromFileName(const bpString& aFileName)
{
  std::map<tDimension, std::vector<bpString>> vDimensionsDelimiters = { { DimensionX, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eX), ";") },
                                                                        { DimensionY, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eY), ";") },
                                                                        { DimensionZ, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eZ), ";") },
                                                                        { DimensionC, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eC), ";") },
                                                                        { DimensionT, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eT), ";") },
                                                                        { DimensionF, bpSplit(mFileSeriesDelimiters.GetDelimiters(bpFileSeriesDelimiters::tDimension::eF), ";") } };

  // split file name into substrings and indices
  std::vector<bpString> vSubStrings;
  std::vector<bpSize> vIndices;
  SplitFileName(bpFileTools::GetFileExt(aFileName), vSubStrings, vIndices);

  // reset dimensions
  mIndexDimension.clear();
  mIndexDimension.resize(vIndices.size(), DimensionNone);

  // find dimension by name
  for (bpSize vIndexCount = 0; vIndexCount < vIndices.size(); ++vIndexCount) {
    bpString vSubStringUpper = bpToUpper(vSubStrings[vIndexCount]);
    auto vDimensionDelimitersIt = vDimensionsDelimiters.begin();
    auto vDimensionDelimitersEnd = vDimensionsDelimiters.end();
    for (; vDimensionDelimitersIt != vDimensionDelimitersEnd && mIndexDimension[vIndexCount] == DimensionNone; ++vDimensionDelimitersIt) {
      const tDimension& vDim = vDimensionDelimitersIt->first;
      if ((vDim != DimensionX && vDim != DimensionY) || GetEnableTilingXY()) {
        for (const auto& vDelimiter : vDimensionDelimitersIt->second) {
          if (bpEndsWith(vSubStringUpper, vDelimiter)) {
            mIndexDimension[vIndexCount] = vDim;
            break;
          }
        }
      }
    }
  }
}


void bpFileReaderSeriesAdjustable::GuessDimensionIndicesFromSize(const std::vector<bpSize>& aIndexSize)
{
  // build vector of available, largest dimensions
  std::set<tDimension> vAssignedDimensions(mIndexDimension.begin(), mIndexDimension.end());
  std::vector<tDimension> vSmallestToLargestDimension;
  if (vAssignedDimensions.find(DimensionX) == vAssignedDimensions.end()) {
    if (GetEnableTilingXY()) {
      vSmallestToLargestDimension.push_back(DimensionX);
    }
  }
  if (vAssignedDimensions.find(DimensionY) == vAssignedDimensions.end()) {
    if (GetEnableTilingXY()) {
      vSmallestToLargestDimension.push_back(DimensionY);
    }
  }
  if (vAssignedDimensions.find(DimensionC) == vAssignedDimensions.end()) {
    vSmallestToLargestDimension.push_back(DimensionC);
  }
  if (vAssignedDimensions.find(DimensionT) == vAssignedDimensions.end()) {
    vSmallestToLargestDimension.push_back(DimensionT);
  }
  if (vAssignedDimensions.find(DimensionZ) == vAssignedDimensions.end()) {
    vSmallestToLargestDimension.push_back(DimensionZ);
  }

  // sort the sizes (ascending)
  std::vector<cIndexSize> vSortedIndices;
  bpSize vIndex = 0;
  std::vector<bpSize>::const_iterator vIndexSizeIterator = aIndexSize.begin();
  while (vIndexSizeIterator != aIndexSize.end()) {
    vSortedIndices.push_back(cIndexSize(vIndex++, *vIndexSizeIterator));
    ++vIndexSizeIterator;
  }
  std::sort(vSortedIndices.begin(), vSortedIndices.end(), cIndexSize::Less);

  // index with largest size will become dimension z, if not yet assigned
  std::vector<cIndexSize>::reverse_iterator vSortedIndexIterator = vSortedIndices.rbegin();
  while (vSortedIndexIterator != vSortedIndices.rend()) {
    vIndex = (*vSortedIndexIterator).mIndex;
    if (vIndex < mIndexDimension.size()) {
      if (mIndexDimension[vIndex] == DimensionNone) {
        if (!vSmallestToLargestDimension.empty()) {
          tDimension vDimension = vSmallestToLargestDimension.back();
          vSmallestToLargestDimension.pop_back();
          mIndexDimension[vIndex] = vDimension;
        }
      }
    }
    ++vSortedIndexIterator;
  }
}


void bpFileReaderSeriesAdjustable::EvalFilesAndSetSeries(const bpString& aFileName, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck)
{
  mReferenceFileName = aFileName;

  mSeriesFileNames.clear();
  mSeriesFileNames.insert(aFileName);

  // check all files of directory
  bpSeriesList vFileSeriesList(aFileName, aFileReaderSeriesFileCheck);
  mSeriesFileNames = vFileSeriesList.GetSet();

  GuessDimensionIndicesFromFileName(aFileName);
  GuessDimensionIndicesFromSize(GetIndexSize());

  // set up file layout
  mDelimiterType = eWithDelimiter;
  mFileLayout = CreateLayouts(mSeriesFileNames, mIndexDimension);
  mDimensionSizes = GetDimensionSizes(mIndexDimension, mFileLayout);
  bpSize vNumberOfRequiredFiles = GetNumberOfRequiredFiles(mIndexDimension, mFileLayout);

  mIsTooManyMissingFiles = vNumberOfRequiredFiles > mSeriesFileNames.size() * 2;

  if (vNumberOfRequiredFiles > mSeriesFileNames.size()) {
    mDelimiterType = eWithoutDelimiter;
    mFileLayout = CreateLayoutsWithoutDelimiter(mSeriesFileNames, mDimensionSizes);
    mDimensionSizes = GetDimensionSizes(mIndexDimension, mFileLayout);
  }
}


bpFileReaderImpl::Dimension bpFileReaderSeriesAdjustable::GetFileReaderDimension(tDimension aDimension)
{
  switch (aDimension) {
  case DimensionX:
    return bpFileReaderImpl::X;
  case DimensionY:
    return bpFileReaderImpl::Y;
  case DimensionZ:
    return bpFileReaderImpl::Z;
  case DimensionC:
    return bpFileReaderImpl::C;
  case DimensionT:
    return bpFileReaderImpl::T;
  default:
    throw std::runtime_error("Unknown Dimension");
  }
}


std::vector<bpFileReaderSeriesAdjustable::cDimensionSize>
bpFileReaderSeriesAdjustable::GetDimensionSizes(const std::vector<tDimension>& aIndexDimension,
                                                       const bpFileLayout& aFileLayout)
{
  std::vector<cDimensionSize> vDimensionSizes;

  std::vector<tDimension>::const_iterator vIndexDimensionIterator = aIndexDimension.begin();
  while (vIndexDimensionIterator != aIndexDimension.end()) {
    const tDimension& vDimension = *vIndexDimensionIterator;
    if (vDimension != DimensionNone) {
      if (vDimension == DimensionF) {
        vDimensionSizes.push_back(cDimensionSize(vDimension, aFileLayout.GetLayouts().size()));
      }
      else {
        bpFileReaderImpl::Dimension vFileReaderDimension = GetFileReaderDimension(vDimension);
        vDimensionSizes.push_back(cDimensionSize(vDimension, aFileLayout.GetSize()[vFileReaderDimension]));
      }
    }
    ++vIndexDimensionIterator;
  }

  if (vDimensionSizes.size() < 3) {
    vDimensionSizes.resize(3);
  }

  return vDimensionSizes;
}


std::vector<bpSize> bpFileReaderSeriesAdjustable::GetIndexSize() const
{
  bpSize vNumberOfIndices = GetFileNameNumberOfIndices();
  std::vector<bpSize> vMinIndex(vNumberOfIndices, std::numeric_limits<bpSize>::max());
  std::vector<bpSize> vMaxIndex(vNumberOfIndices, std::numeric_limits<bpSize>::min());

  for (const auto& vFileName : mSeriesFileNames) {
    std::vector<bpString> vSubStrings;
    std::vector<bpSize> vIndices;
    SplitFileName(vFileName, vSubStrings, vIndices);
    for (bpSize vIndexCount = 0; vIndexCount < vIndices.size() && vIndexCount < vNumberOfIndices; ++vIndexCount) {
      if (vIndices[vIndexCount] < vMinIndex[vIndexCount]) {
        vMinIndex[vIndexCount] = vIndices[vIndexCount];
      }
      if (vIndices[vIndexCount] > vMaxIndex[vIndexCount]) {
        vMaxIndex[vIndexCount] = vIndices[vIndexCount];
      }
    }
  }
  std::vector<bpSize> vSize(vMinIndex);
  for (bpSize vIndexCount = 0; vIndexCount < vNumberOfIndices; ++vIndexCount) {
    vSize[vIndexCount] = vMaxIndex[vIndexCount] - vMinIndex[vIndexCount] + 1;
  }

  return vSize;
}


bpString bpFileReaderSeriesAdjustable::GetFileName() const
{
  return mReferenceFileName;
}


const std::vector<bpFileReaderSeriesAdjustable::tFileLayout>& bpFileReaderSeriesAdjustable::GetFileLayouts() const
{
  return mFileLayout.GetLayouts();
}


std::vector<std::map<bpString, bpFileReaderSeriesAdjustable::tImageIndex> >
bpFileReaderSeriesAdjustable::CreateLayouts(const std::set<bpString>& aFileNames, const std::vector<tDimension>& aIndexDimension)
{
  // determine smallest indices along each dim
  tImageIndex vImageIndexMin;
  vImageIndexMin[bpFileReaderImpl::X] = std::numeric_limits<bpSize>::max();
  vImageIndexMin[bpFileReaderImpl::Y] = std::numeric_limits<bpSize>::max();
  vImageIndexMin[bpFileReaderImpl::Z] = std::numeric_limits<bpSize>::max();
  vImageIndexMin[bpFileReaderImpl::C] = std::numeric_limits<bpSize>::max();
  vImageIndexMin[bpFileReaderImpl::T] = std::numeric_limits<bpSize>::max();

  bpSize vNumberOfDelimiterF = 0;
  for (auto vIt = aIndexDimension.begin(); vIt != aIndexDimension.end(); ++vIt) {
    if (*vIt == DimensionF) {
      ++vNumberOfDelimiterF;
    }
  }
  std::vector<bpSize> vIndicesMinF(vNumberOfDelimiterF, std::numeric_limits<bpSize>::max());
  std::vector<bpSize> vIndicesMaxF(vNumberOfDelimiterF, 0);

  for (const auto& vFileName : aFileNames) {
    std::vector<bpString> vSubStrings;
    std::vector<bpSize> vIndices;
    bpSize vIndexCountF = 0;
    SplitFileName(vFileName, vSubStrings, vIndices);
    for (bpSize vIndexCount = 0; vIndexCount < vIndices.size(); ++vIndexCount) {
      tDimension vDimension = DimensionNone;
      if (vIndexCount < aIndexDimension.size()) {
        vDimension = aIndexDimension[vIndexCount];
      }
      switch (vDimension) {
        case DimensionNone:
          break;
        case DimensionX:
          if (vIndices[vIndexCount] < vImageIndexMin[bpFileReaderImpl::X]) {
            vImageIndexMin[bpFileReaderImpl::X] = vIndices[vIndexCount];
          }
          break;
        case DimensionY:
          if (vIndices[vIndexCount] < vImageIndexMin[bpFileReaderImpl::Y]) {
            vImageIndexMin[bpFileReaderImpl::Y] = vIndices[vIndexCount];
          }
          break;
        case DimensionZ:
          if (vIndices[vIndexCount] < vImageIndexMin[bpFileReaderImpl::Z]) {
            vImageIndexMin[bpFileReaderImpl::Z] = vIndices[vIndexCount];
          }
          break;
        case DimensionC:
          if (vIndices[vIndexCount] < vImageIndexMin[bpFileReaderImpl::C]) {
            vImageIndexMin[bpFileReaderImpl::C] = vIndices[vIndexCount];
          }
          break;
        case DimensionT:
          if (vIndices[vIndexCount] < vImageIndexMin[bpFileReaderImpl::T]) {
            vImageIndexMin[bpFileReaderImpl::T] = vIndices[vIndexCount];
          }
          break;
        case DimensionF:
          if (vIndices[vIndexCount] < vIndicesMinF[vIndexCountF]) {
            vIndicesMinF[vIndexCountF] = vIndices[vIndexCount];
          }
          if (vIndices[vIndexCount] > vIndicesMaxF[vIndexCountF]) {
            vIndicesMaxF[vIndexCountF] = vIndices[vIndexCount];
          }
          ++vIndexCountF;
          break;
      }
    }
  }

  std::vector<bpSize> vAccumulatedSizesF;
  vAccumulatedSizesF.push_back(1);
  for (bpSize vIndexCount = 0; vIndexCount < vIndicesMinF.size(); ++vIndexCount) {
    bpSize vCurrentIndexSize = 1 + vIndicesMaxF[vIndexCount] - vIndicesMinF[vIndexCount];
    vAccumulatedSizesF.push_back(vAccumulatedSizesF.back() * vCurrentIndexSize);
  }
  std::vector<std::map<bpString, tImageIndex > > vFileLayouts(vAccumulatedSizesF.back());

  for (std::set<bpString>::const_iterator vFileNameIt = aFileNames.begin(); vFileNameIt != aFileNames.end(); ++vFileNameIt) {
    const bpString& vFileName = *vFileNameIt;
    std::vector<bpString> vSubStrings;
    std::vector<bpSize> vIndices;
    SplitFileName(vFileName, vSubStrings, vIndices);
    tImageIndex vImageIndex;
    bpSize vIndexCountF = 0;
    bpSize vLayoutIndex = 0;
    for (bpSize vIndexCount = 0; vIndexCount < vIndices.size(); ++vIndexCount) {
      tDimension vDimension = aIndexDimension[vIndexCount];
      switch (vDimension) {
        case DimensionNone:
          break;
        case DimensionX:
          vImageIndex[bpFileReaderImpl::X] = vIndices[vIndexCount] - vImageIndexMin[bpFileReaderImpl::X];
          break;
        case DimensionY:
          vImageIndex[bpFileReaderImpl::Y] = vIndices[vIndexCount] - vImageIndexMin[bpFileReaderImpl::Y];
          break;
        case DimensionZ:
          vImageIndex[bpFileReaderImpl::Z] = vIndices[vIndexCount] - vImageIndexMin[bpFileReaderImpl::Z];
          break;
        case DimensionC:
          vImageIndex[bpFileReaderImpl::C] = vIndices[vIndexCount] - vImageIndexMin[bpFileReaderImpl::C];
          break;
        case DimensionT:
          vImageIndex[bpFileReaderImpl::T] = vIndices[vIndexCount] - vImageIndexMin[bpFileReaderImpl::T];
          break;
        case DimensionF:
          vLayoutIndex += vAccumulatedSizesF[vIndexCountF] * (vIndices[vIndexCount] - vIndicesMinF[vIndexCountF]);
          ++vIndexCountF;
          break;
        }
    }
    vFileLayouts[vLayoutIndex][vFileName] = vImageIndex;
  }
  return vFileLayouts;
}


class cSplittedFileName
{
public:
  cSplittedFileName()
    : mName(),
      mSeparators(),
      mNumbers() {
  }
  cSplittedFileName(const cSplittedFileName& aOther)
    : mName(aOther.mName),
      mSeparators(aOther.mSeparators),
      mNumbers(aOther.mNumbers) {
  }
  inline bool operator < (const cSplittedFileName& aOther) const {
    if (mNumbers.size() == aOther.mNumbers.size()) {
      for (bpSize vIndex = 0; vIndex < mNumbers.size(); vIndex++) {
        if (mNumbers[vIndex] != aOther.mNumbers[vIndex]) {
          return mNumbers[vIndex] < aOther.mNumbers[vIndex];
        }
      }
    }
    if (mSeparators.size() == aOther.mSeparators.size()) {
      for (bpSize vIndex = 0; vIndex < mSeparators.size(); vIndex++) {
        if (mSeparators[vIndex] != aOther.mSeparators[vIndex]) {
          return mSeparators[vIndex] < aOther.mSeparators[vIndex];
        }
      }
    }
    return mName < aOther.mName;
  }
  bpString mName;
  std::vector<bpString> mSeparators;
  std::vector<bpSize> mNumbers;
};

std::vector<std::map<bpString, bpFileReaderSeriesAdjustable::tImageIndex> >
bpFileReaderSeriesAdjustable::CreateLayoutsWithoutDelimiter(const std::set<bpString>& aFileNames,
    const std::vector<cDimensionSize>& aDimensionSizes)
{
  // copy file names and sort them numerically
  std::set<cSplittedFileName> vFileSplittedNames;
  std::set<bpString>::const_iterator vFileNameIterator = aFileNames.begin();
  while (vFileNameIterator != aFileNames.end()) {
    cSplittedFileName vSplittedFileName;
    vSplittedFileName.mName = *vFileNameIterator;
    SplitFileName(vSplittedFileName.mName,
                  vSplittedFileName.mSeparators,
                  vSplittedFileName.mNumbers);
    vFileSplittedNames.insert(vSplittedFileName);
    ++vFileNameIterator;
  }

  //
  tImageIndex vImageIndex;
  vImageIndex[bpFileReaderImpl::X] = 0;
  vImageIndex[bpFileReaderImpl::Y] = 0;
  vImageIndex[bpFileReaderImpl::Z] = 0;
  vImageIndex[bpFileReaderImpl::C] = 0;
  vImageIndex[bpFileReaderImpl::T] = 0;

  bool vEndOfSequence = false;

  std::vector<bpSize> vAccumulatedSizesF;
  vAccumulatedSizesF.push_back(1);
  for (auto vIt = aDimensionSizes.begin(); vIt != aDimensionSizes.end(); ++vIt) {
    if(vIt->mDimension == DimensionF) {
      vAccumulatedSizesF.push_back(vAccumulatedSizesF.back() * vIt->mSize);
    }
  }
  std::vector<bpSize> vIndicesF(vAccumulatedSizesF.size() - 1, 0);
  std::vector<std::map<bpString, tImageIndex> > vFileLayouts(vAccumulatedSizesF.back());

  std::set<cSplittedFileName>::const_iterator vSplittedFileNameIterator = vFileSplittedNames.begin();
  while (vSplittedFileNameIterator != vFileSplittedNames.end() && !vEndOfSequence) {

    const bpString& vFileName = (*vSplittedFileNameIterator).mName;

    bpSize vLayoutIndex = 0;
    for (bpSize vIndex = 0; vIndex < vIndicesF.size(); ++vIndex) {
      vLayoutIndex += vIndicesF[vIndex] * vAccumulatedSizesF[vIndex];
    }
    vFileLayouts[vLayoutIndex][vFileName] = vImageIndex;

    bpSize vIndexCountF = vIndicesF.size() - 1;

    for (bpInt32 vCountIndex = static_cast<bpInt32>(aDimensionSizes.size()) - 1; vCountIndex >= 0; vCountIndex--) {
      bpSize vCountIndexAsSize = static_cast<bpSize>(vCountIndex);
      if (aDimensionSizes[vCountIndexAsSize].mDimension != DimensionNone) {

        bpSize& vDimensionIndex = aDimensionSizes[vCountIndexAsSize].mDimension == DimensionF ?
                                       vIndicesF[vIndexCountF] :
                                       vImageIndex[GetFileReaderDimension(aDimensionSizes[vCountIndexAsSize].mDimension)];
        if (vDimensionIndex + 1 < aDimensionSizes[vCountIndexAsSize].mSize) {
          vDimensionIndex++;
          break;
        }
        else {
          vDimensionIndex = 0;
          if (vCountIndex == 0) {
            vEndOfSequence = true;
          }
        }

        if (aDimensionSizes[vCountIndexAsSize].mDimension == DimensionF) {
          vIndexCountF--;
        }


      }
      else {
        if (vCountIndex == 0) {
          vEndOfSequence = true;
        }
      }
    }

    ++vSplittedFileNameIterator;
  }

  return vFileLayouts;
}


bool bpFileReaderSeriesAdjustable::GetEnableTilingXY() const
{
  return mEnableTilingXY;
}


void bpFileReaderSeriesAdjustable::SplitFileName(const bpString& aFileName,
                                                        std::vector<bpString>& aSubStrings,
                                                        std::vector<bpSize>& aIndices)
{
  bpString vFileName = bpFileTools::GetFileExt(aFileName);
  bpString vBaseName = bpFileTools::GetFile(aFileName);

  aSubStrings.clear();
  aIndices.clear();
  bpString vName = vBaseName;
  // split off first substring
  bpSize vBegin = vName.find_first_of("0123456789");
  aSubStrings.push_back(vName.substr(0, vBegin));
  if (vBegin != bpString::npos) {
    vName = vName.substr(vBegin);
  }
  else {
    vName = bpString();
  }
  while (!vName.empty()) {
    // find index
    bpSize vIndex = 0;
    vBegin = vName.find_first_not_of("0123456789");
    if (vBegin != bpString::npos) {
      if (vBegin > 0) {
        bpFromString(vName.substr(0, vBegin), vIndex);
        vName = vName.substr(vBegin);
      }
    }
    else {
      bpFromString(vName, vIndex);
      vName = bpString();
    }
    aIndices.push_back(vIndex);
    if (!vName.empty()) {
      vBegin = vName.find_first_of("0123456789");
      aSubStrings.push_back(vName.substr(0, vBegin));
      if (vBegin != bpString::npos) {
        vName = vName.substr(vBegin);
      }
      else {
        vName = bpString();
      }
    }
  }
}


bpSize bpFileReaderSeriesAdjustable::GetNumberOfRequiredFiles(const std::vector<tDimension>& aIndexDimension,
    const bpFileLayout& aFileLayout)
{
  std::set<tDimension> vUniqueIndexDimension;
  for (auto vIt = aIndexDimension.begin(); vIt != aIndexDimension.end(); ++vIt) {
    vUniqueIndexDimension.insert(*vIt);
  }
  bpSize vNumberOfFiles = 1;
  for (auto vIt = vUniqueIndexDimension.begin(); vIt != vUniqueIndexDimension.end(); ++vIt) {
    if (*vIt != DimensionNone) {
      if (*vIt == DimensionF) {
        vNumberOfFiles *= aFileLayout.GetLayouts().size();
      }
      else {
        bpFileReaderImpl::Dimension vFileReaderDimension = GetFileReaderDimension(*vIt);
        vNumberOfFiles *= aFileLayout.GetSize()[vFileReaderDimension];
      }
    }
  }
  return vNumberOfFiles;
}


bpSize bpFileReaderSeriesAdjustable::GetFileNameNumberOfIndices() const
{
  std::vector<bpString> vSubStrings;
  std::vector<bpSize> vIndices;
  SplitFileName(bpFileTools::GetFileExt(GetFileName()), vSubStrings, vIndices);
  return vIndices.size();
}
