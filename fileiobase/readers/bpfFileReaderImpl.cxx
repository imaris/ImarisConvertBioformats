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


#include "fileiobase/readers/bpfFileReaderImpl.h"

#include "fileiobase/types/bpfHistogram.h"
#include "fileiobase/types/bpfParameterSection.h"
#include "fileiobase/utils/bpfFileTools.h"

#include <algorithm>


bpfFileReaderImpl::ImageIndex::ImageIndex()
  : mIndexVector(5)
{
}

bpfFileReaderImpl::ImageIndex::ImageIndex(const std::vector<tDimension>& aDimensionIncrementSequence,
                                          const std::vector<bpfSize>& aSize)
  : mIndexVector(5)
{
  for (bpfSize vDim = 0; vDim < 5; ++vDim) {
    mIndexVector[aDimensionIncrementSequence[vDim]] = aSize[vDim];
  }
}

bpfFileReaderImpl::ImageIndex::ImageIndex(const bpfFileReaderImpl::ImageIndex& aOther)
  : mIndexVector(aOther.mIndexVector)
{
}

bpfFileReaderImpl::ImageIndex::~ImageIndex()
{
}

bpfFileReaderImpl::ImageIndex& bpfFileReaderImpl::ImageIndex::operator = (const ImageIndex& aOther)
{
  mIndexVector = aOther.mIndexVector;
  return *this;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::ImageIndex::operator * (const ImageIndex& aOther)
{
  ImageIndex vIndex(*this);
  for (bpfSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] * aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::ImageIndex::operator / (const ImageIndex& aOther)
{
  ImageIndex vIndex(*this);
  for (bpfSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] / aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::ImageIndex::operator + (const ImageIndex& aOther)
{
  ImageIndex vIndex(*this);
  for (bpfSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] + aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::ImageIndex::operator - (const ImageIndex& aOther)
{
  ImageIndex vIndex(*this);
  for (bpfSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] - aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::ImageIndex::operator % (const ImageIndex& aOther)
{
  ImageIndex vIndex(*this);
  for (bpfSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] % aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpfSize& bpfFileReaderImpl::ImageIndex::operator [] (tDimension aDimension)
{
  return mIndexVector[aDimension];
}

const bpfSize& bpfFileReaderImpl::ImageIndex::operator [] (tDimension aDimension) const
{
  return mIndexVector[aDimension];
}

bool bpfFileReaderImpl::ImageIndex::operator == (const bpfFileReaderImpl::ImageIndex& aOther) const
{
  return mIndexVector == aOther.mIndexVector;
}

bool bpfFileReaderImpl::ImageIndex::operator != (const bpfFileReaderImpl::ImageIndex& aOther) const
{
  return mIndexVector != aOther.mIndexVector;
}

bool bpfFileReaderImpl::ImageIndex::operator < (const bpfFileReaderImpl::ImageIndex& aOther) const
{
  return mIndexVector < aOther.mIndexVector;
}

bool bpfFileReaderImpl::ImageIndex::operator > (const bpfFileReaderImpl::ImageIndex& aOther) const
{
  return mIndexVector > aOther.mIndexVector;
}

std::vector<bpfSize> bpfFileReaderImpl::ImageIndex::GetXYZCT() const
{
  return mIndexVector;
}


bpfFileReaderImpl::bpfFileReaderImpl(const bpfString& aFileName)
  : mFileName(aFileName)
{
}


bpfFileReaderImpl::~bpfFileReaderImpl()
{
}


bool bpfFileReaderImpl::CanMergeImages() const
{
  return false;
}

void bpfFileReaderImpl::SetMergeImages(bool aEnable)
{
}

bool bpfFileReaderImpl::GetMergeImages() const
{
  return false;
}


/**
 * Returns the number of datasets (images / experiments)
 */
bpfSize bpfFileReaderImpl::GetNumberOfDataSets() const
{
  return 1;
}


/**
 * Activate a dataset for subsequent calls
 */
void bpfFileReaderImpl::SetActiveDataSetIndex(bpfSize aDataSetIndex)
{
}

/**
 * Returns the index of the currently activated dataset
 */
bpfSize bpfFileReaderImpl::GetActiveDataSetIndex() const
{
  return 0;
}

bool bpfFileReaderImpl::SeriesConfigurable() const
{
  return false;
}

bool bpfFileReaderImpl::SeriesInitialised() const
{
  return false;
}

bool bpfFileReaderImpl::SceneConfigurable() const
{
  return false;
}

void bpfFileReaderImpl::SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout)
{
}

std::vector<bpfFileReaderImpl::tFileLayout> bpfFileReaderImpl::SeriesGetFileLayouts() const
{
  return std::vector<bpfFileReaderImpl::tFileLayout>();
}

bool bpfFileReaderImpl::SeriesFileValid(const bpfString& aFileName) const
{
  return false;
}

bpfSize bpfFileReaderImpl::GetNumberOfResolutions() const
{
  return 1;
}

bpfSize bpfFileReaderImpl::GetActiveResolutionLevel() const
{
  return 0;
}

void bpfFileReaderImpl::SetActiveResolutionLevel(bpfSize aResolutionLevel)
{
}

/**
 * returns a description of the file reader (normally the static "GetDescription()")
 */
bpfString bpfFileReaderImpl::GetReaderDescription() const
{
  return "unknown file reader";
}


bpfSize bpfFileReaderImpl::GetDimensionIndex(Dimension aDimension)
{
  bpfSize vCount=0;
  std::vector<Dimension> vDimension = GetDimensionSequence();
  while (vDimension[vCount] != aDimension) {
    vCount++;
  }
  return vCount;
}


bpfSize bpfFileReaderImpl::GetDataSize(Dimension aDimension)
{
  return static_cast<bpfUInt32>(GetDataSize()[aDimension]);
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::GetDataSize()
{
  return ImageIndex(GetDimensionSequence(), GetDataSizeV());
}


bool bpfFileReaderImpl::SetDataBlockSize(bpfSize aDimension0, bpfSize aDimension1, bpfSize aDimension2, bpfSize aDimension3, bpfSize aDimension4, bpfSize aResolutionLevel)
{
  // per default Data Block Size cannot
  return false;
}

bpfFileReaderImpl::ImageIndex bpfFileReaderImpl::GetDataBlockSize()
{
  return ImageIndex(GetDimensionSequence(), GetDataBlockSizeV());
}

bpfSize bpfFileReaderImpl::GetDataBlockSize(Dimension aDimension)
{
  return GetDataBlockSize()[aDimension];
}

bpfSize bpfFileReaderImpl::GetDataBlockNumberOfVoxels()
{
  bpfSize vSize = 1;

  std::vector<bpfSize> vBlockSize = GetDataBlockSizeV();
  int vIndex;
  for (vIndex=0; vIndex<5; vIndex++) {
    vSize *= vBlockSize[static_cast<bpfSize>(vIndex)];
  }

  return vSize;
}


bpfSize bpfFileReaderImpl::GetDataBlockNumber(bpfSize x,
                                              bpfSize y,
                                              bpfSize z,
                                              bpfSize aChannel,
                                              bpfSize aTimePoint)
{
  // transform start position to data block coordinates
  std::vector<bpfSize> vImarisVector(5);
  std::vector<bpfSize> vDataVector(5);

  vImarisVector[0] = x;
  vImarisVector[1] = y;
  vImarisVector[2] = z;
  vImarisVector[3] = aChannel;
  vImarisVector[4] = aTimePoint;

  vDataVector[GetDimensionIndex(X)] = vImarisVector[0];
  vDataVector[GetDimensionIndex(Y)] = vImarisVector[1];
  vDataVector[GetDimensionIndex(Z)] = vImarisVector[2];
  vDataVector[GetDimensionIndex(C)] = vImarisVector[3];
  vDataVector[GetDimensionIndex(T)] = vImarisVector[4];

  std::vector<bpfSize> vDataSize = GetDataSizeV();
  std::vector<bpfSize> vDataBlockSize = GetDataBlockSizeV();

  // number of blocks per unit dimension
  bpfSize vBlocksPerUnit1 =                   (vDataSize[0]/vDataBlockSize[0] + (vDataSize[0]%vDataBlockSize[0] > 0 ? 1:0));
  bpfSize vBlocksPerUnit2 = vBlocksPerUnit1 * (vDataSize[1]/vDataBlockSize[1] + (vDataSize[1]%vDataBlockSize[1] > 0 ? 1:0));
  bpfSize vBlocksPerUnit3 = vBlocksPerUnit2 * (vDataSize[2]/vDataBlockSize[2] + (vDataSize[2]%vDataBlockSize[2] > 0 ? 1:0));
  bpfSize vBlocksPerUnit4 = vBlocksPerUnit3 * (vDataSize[3]/vDataBlockSize[3] + (vDataSize[3]%vDataBlockSize[3] > 0 ? 1:0));

  bpfSize vBlockCount = 0;

  // vector containing coordinates of end of block
  std::vector<bpfSize> vEndOfBlock(5);
  vEndOfBlock[0] = vDataBlockSize[0]-1;
  vEndOfBlock[1] = vDataBlockSize[1]-1;
  vEndOfBlock[2] = vDataBlockSize[2]-1;
  vEndOfBlock[3] = vDataBlockSize[3]-1;
  vEndOfBlock[4] = vDataBlockSize[4]-1;

  // advance until end of block in dimension 4 passes vDataVector[4]
  if (vEndOfBlock[4] < vDataVector[4]) {
    bpfSize vDiff = vDataVector[4] - vEndOfBlock[4];
    bpfSize vSteps = vDiff / vDataBlockSize[4] + (vDiff % vDataBlockSize[4] == 0 ? 0 : 1);
    vEndOfBlock[4] += vSteps * vDataBlockSize[4];
    vBlockCount += vSteps * vBlocksPerUnit4;
  }

  // advance until end of block in dimension 3 passes vDataVector[3]
  if (vEndOfBlock[3] < vDataVector[3]) {
    bpfSize vDiff = vDataVector[3] - vEndOfBlock[3];
    bpfSize vSteps = vDiff / vDataBlockSize[3] + (vDiff % vDataBlockSize[3] == 0 ? 0 : 1);
    vEndOfBlock[3] += vSteps * vDataBlockSize[3];
    vBlockCount += vSteps * vBlocksPerUnit3;
  }

  // advance until end of block in dimension 2 passes vDataVector[2]
  if (vEndOfBlock[2] < vDataVector[2]) {
    bpfSize vDiff = vDataVector[2] - vEndOfBlock[2];
    bpfSize vSteps = vDiff / vDataBlockSize[2] + (vDiff % vDataBlockSize[2] == 0 ? 0 : 1);
    vEndOfBlock[2] += vSteps * vDataBlockSize[2];
    vBlockCount += vSteps * vBlocksPerUnit2;
  }

  // advance until end of block in dimension 1 passes vDataVector[1]
  if (vEndOfBlock[1] < vDataVector[1]) {
    bpfSize vDiff = vDataVector[1] - vEndOfBlock[1];
    bpfSize vSteps = vDiff / vDataBlockSize[1] + (vDiff % vDataBlockSize[1] == 0 ? 0 : 1);
    vEndOfBlock[1] += vSteps * vDataBlockSize[1];
    vBlockCount += vSteps * vBlocksPerUnit1;
  }

  // advance until end of block in dimension 0 passes vDataVector[0]
  if (vEndOfBlock[0] < vDataVector[0]) {
    bpfSize vDiff = vDataVector[0] - vEndOfBlock[0];
    bpfSize vSteps = vDiff / vDataBlockSize[0] + (vDiff % vDataBlockSize[0] == 0 ? 0 : 1);
    vEndOfBlock[0] += vSteps * vDataBlockSize[0];
    vBlockCount += vSteps;
  }

  return vBlockCount;


  /*
  // compute size of each dimension
  std::vector<bpfUInt32> vDimSize(5);
  vDimSize[0] = GetDataSize(0);
  vDimSize[1] = vDimSize[0] * GetDataSize(1);
  vDimSize[2] = vDimSize[1] * GetDataSize(2);
  vDimSize[3] = vDimSize[2] * GetDataSize(3);
  vDimSize[4] = vDimSize[3] * GetDataSize(4);


  // compute position of voxel
  bpfUInt32 position = 0;
  position += vDataVector[4] * vDimSize[3];
  position += vDataVector[3] * vDimSize[2];
  position += vDataVector[2] * vDimSize[1];
  position += vDataVector[1] * vDimSize[0];
  position += vDataVector[0];

  // compute size of DataBlock
  bpfUInt32 i=0;
  bpfUInt32 vBlockSize = 1;
  while(i<GetDataBlockSizeV().size()){
    if (GetDataBlockSize(i) > GetDataSize(i)) {
      vBlockSize *= GetDataSize(i);
    }else{
      vBlockSize *= GetDataBlockSize(i);
    }
    i++;
  }

  return position/vBlockSize;

  */
}


bpfSize bpfFileReaderImpl::GetNumberOfDataBlocks()
{
  ImageIndex vDataSize = GetDataSize();
  return GetDataBlockNumber(vDataSize[X] - 1,
                            vDataSize[Y] - 1,
                            vDataSize[Z] - 1,
                            vDataSize[C] - 1,
                            vDataSize[T] - 1) + 1;
}


bpfVector3Float bpfFileReaderImpl::GetExtentMin() const
{
  bpfVector3Float vExtentMin;
  bpfVector3Float vExtentMax;
  const_cast<bpfFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  return vExtentMin;
}

bpfVector3Float bpfFileReaderImpl::GetExtentMax() const
{
  bpfVector3Float vExtentMin;
  bpfVector3Float vExtentMax;
  const_cast<bpfFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  return vExtentMax;
}

bpfVector3Float bpfFileReaderImpl::GetExtentDelta() const
{
  bpfVector3Float vExtentMin;
  bpfVector3Float vExtentMax;
  const_cast<bpfFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  ImageIndex vDataSize = const_cast<bpfFileReaderImpl*>(this)->GetDataSize();
  bpfVector3Float vExtentDelta;
  vExtentDelta[0] = (vExtentMax[0] - vExtentMin[0]) / vDataSize[X];
  vExtentDelta[1] = (vExtentMax[1] - vExtentMin[1]) / vDataSize[Y];
  vExtentDelta[2] = (vExtentMax[2] - vExtentMin[2]) / vDataSize[Z];
  return vExtentDelta;
}


static bpfString mOriginalFormatFileIOVersion;


void bpfFileReaderImpl::SetOriginalFormatFileIOVersion(const bpfString& aVersion)
{
  mOriginalFormatFileIOVersion = aVersion;
}


bpfSectionContainer bpfFileReaderImpl::ReadParameters()
{
  bpfSectionContainer vParameters = ReadParametersImpl();

  bpfParameterSection* vSection = vParameters.CreateSection("Image");

  if (!vSection->HasParameter("OriginalFormat") || !vSection->HasParameter("OriginalFormatFileIOVersion")) {
    vSection->SetParameter("OriginalFormat", GetReaderDescription());

    if (!mOriginalFormatFileIOVersion.empty()) {
      vSection->SetParameter("OriginalFormatFileIOVersion", mOriginalFormatFileIOVersion);
    }
  }

  return vParameters;
}

bool bpfFileReaderImpl::ReadHistogram(bpfHistogram* aHistogram, bpfSize aTimePoint, bpfSize aChannel)
{
  // per default no histogram are read
  return false;
}

bool bpfFileReaderImpl::ReadThumbnail(std::vector<bpfPackedRGBA>& aThumbnail, bpfSize& aSizeX, bpfSize& aSizeY)
{
  return false;
}

std::vector<bpfTimeInfo> bpfFileReaderImpl::ReadTimeInfo()
{
  bpfSize vTimePoint;
  std::vector<bpfTimeInfo> vTimeInfo;
  bpfSize vNumberOfTimePoints = GetDataSize(T);
  for (vTimePoint = 0; vTimePoint < vNumberOfTimePoints; vTimePoint++) {
    vTimeInfo.push_back(ReadTimeInfo(vTimePoint));
  }
  return vTimeInfo;
}


bpfTimeInfo bpfFileReaderImpl::ReadTimeInfo(bpfSize aTimePoint)
{
  bpfTimeInfo vTimeInfo;
  //vTimeInfo.SetTime("00:00:" + bpfToString(aTimePoint));
  vTimeInfo = bpfFileTools::GetFileCreationDate(mFileName);
  vTimeInfo.AddMilliseconds(1000*static_cast<bpfInt32>(aTimePoint));
  return vTimeInfo;
}


std::vector<bpfColorInfo> bpfFileReaderImpl::ReadColorInfo()
{
  bpfSize vChannelId;
  std::vector<bpfColorInfo> vColors;
  bpfSize vNumberOfChannels = GetDataSize(C);
  for (vChannelId = 0; vChannelId < vNumberOfChannels; vChannelId++) {
    vColors.push_back(ReadColorInfo(vChannelId));
  }
  return vColors;
}


bpfColorInfo bpfFileReaderImpl::ReadColorInfo(bpfSize aChannel)
{
  bpfSize vSizeC = GetDataSize(C);
  if (vSizeC == 1) {
    return bpfColorInfo(GetDefaultColor(static_cast<bpfUInt32>(mDefaultColor.size())));
  }
  else {
    return bpfColorInfo(GetDefaultColor(aChannel));
  }
}


bool bpfFileReaderImpl::ShouldColorRangeBeAdjustedToMinMax()
{
  return false;
}

bpfFileReaderImpl::tColorHint bpfFileReaderImpl::mColorHint = bpfFileReaderImpl::eColorHintLUT;

std::vector<bpfColor> bpfFileReaderImpl::mDefaultColor;

void bpfFileReaderImpl::SetColorHint(tColorHint aColorHint)
{
  mColorHint = aColorHint;
}

bpfFileReaderImpl::tColorHint bpfFileReaderImpl::GetColorHint()
{
  return mColorHint;
}

void bpfFileReaderImpl::SetDefaultColor(bpfSize aChannelId, const bpfColor& aColor)
{
  while (mDefaultColor.size() <= aChannelId) {
    mDefaultColor.push_back(bpfColor(1.0f, 1.0f, 1.0f));
  }
  mDefaultColor[aChannelId] = aColor;
}

const bpfColor& bpfFileReaderImpl::GetDefaultColor(bpfSize aChannelId)
{
  if (aChannelId < mDefaultColor.size()) {
    return mDefaultColor[aChannelId];
  }
  else if (mDefaultColor.size() > 0) {
    return mDefaultColor[mDefaultColor.size()-1];
  }
  else if (aChannelId == 0) {
    static bpfColor vDefaultColor(1.0f, 0.0f, 0.0f);
    return vDefaultColor;
  }
  else if (aChannelId == 1) {
    static bpfColor vDefaultColor(0.0f, 1.0f, 0.0f);
    return vDefaultColor;
  }
  else if (aChannelId == 2) {
    static bpfColor vDefaultColor(0.0f, 0.0f, 1.0f);
    return vDefaultColor;
  }
  else {
    static bpfColor vDefaultColor(1.0f, 1.0f, 1.0f);
    return vDefaultColor;
  }
}


bpfString bpfFileReaderImpl::GuessUnit(const bpfString& aSomeUnit)
{
  bpfString vUnit = bpfToUpper(aSomeUnit);
  if ((vUnit == "NM") ||
      (vUnit.find("NANO") != bpfString::npos)) {
    return "nm";
  }
  else if ((vUnit == "UM") ||
           (vUnit.find("MICRO") != bpfString::npos)) {
    return "um";
  }
  else if ((vUnit == "MM") ||
           (vUnit.find("MILLI") != bpfString::npos)) {
    return "mm";
  }
  else if ((vUnit == "M") ||
           (vUnit.find("METER") != bpfString::npos)) {
    return "m";
  }
  else {
    return "";
  }
}

