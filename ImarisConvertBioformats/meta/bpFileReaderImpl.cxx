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


#include "bpFileReaderImpl.h"
#include "bpFileTools.h"
#include "bpUtils.h"

#include <assert.h>


bpColorHint bpFileReaderImpl::mColorHint = bpColorHint::eLUT;
bpFileConversionColors bpFileReaderImpl::mFileConversionColors;


bpFileReaderImpl::bpImageIndex::bpImageIndex()
  : mIndexVector(5)
{
}

bpFileReaderImpl::bpImageIndex::bpImageIndex(const std::vector<tDimension>& aDimensionIncrementSequence,
                                             const std::vector<bpSize>& aSize)
  : mIndexVector(5)
{
  for (bpSize vDim = 0; vDim < 5; ++vDim) {
    mIndexVector[aDimensionIncrementSequence[vDim]] = aSize[vDim];
  }
}

bpFileReaderImpl::bpImageIndex::bpImageIndex(const bpFileReaderImpl::bpImageIndex& aOther)
  : mIndexVector(aOther.mIndexVector)
{
}

bpFileReaderImpl::bpImageIndex::~bpImageIndex()
{
}

bpFileReaderImpl::bpImageIndex& bpFileReaderImpl::bpImageIndex::operator = (const bpImageIndex& aOther)
{
  mIndexVector = aOther.mIndexVector;
  return *this;
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::bpImageIndex::operator * (const bpImageIndex& aOther)
{
  bpImageIndex vIndex(*this);
  for (bpSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] * aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::bpImageIndex::operator / (const bpImageIndex& aOther)
{
  bpImageIndex vIndex(*this);
  for (bpSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] / aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::bpImageIndex::operator + (const bpImageIndex& aOther)
{
  bpImageIndex vIndex(*this);
  for (bpSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] + aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::bpImageIndex::operator - (const bpImageIndex& aOther)
{
  bpImageIndex vIndex(*this);
  for (bpSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] - aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::bpImageIndex::operator % (const bpImageIndex& aOther)
{
  bpImageIndex vIndex(*this);
  for (bpSize vDim = 0; vDim < mIndexVector.size(); ++vDim) {
    vIndex.mIndexVector[vDim] = vIndex.mIndexVector[vDim] % aOther.mIndexVector[vDim];
  }
  return vIndex;
}

bpSize& bpFileReaderImpl::bpImageIndex::operator [] (tDimension aDimension)
{
  return mIndexVector[aDimension];
}

const bpSize& bpFileReaderImpl::bpImageIndex::operator [] (tDimension aDimension) const
{
  return mIndexVector[aDimension];
}

bool bpFileReaderImpl::bpImageIndex::operator == (const bpFileReaderImpl::bpImageIndex& aOther) const
{
  return mIndexVector == aOther.mIndexVector;
}

bool bpFileReaderImpl::bpImageIndex::operator != (const bpFileReaderImpl::bpImageIndex& aOther) const
{
  return mIndexVector != aOther.mIndexVector;
}

bool bpFileReaderImpl::bpImageIndex::operator < (const bpFileReaderImpl::bpImageIndex& aOther) const
{
  return mIndexVector < aOther.mIndexVector;
}

bool bpFileReaderImpl::bpImageIndex::operator > (const bpFileReaderImpl::bpImageIndex& aOther) const
{
  return mIndexVector > aOther.mIndexVector;
}

std::vector<bpSize> bpFileReaderImpl::bpImageIndex::GetXYZCT() const
{
  return mIndexVector;
}


bpFileReaderImpl::bpFileReaderImpl(const bpString& aFileName)
  : mFileName(aFileName)
{
}


bpFileReaderImpl::~bpFileReaderImpl()
{
}

const bpString& bpFileReaderImpl::GetFileName() const
{
  return mFileName;
}


std::vector<bpString> bpFileReaderImpl::GetAllFileNamesOfDataSet(const bpString& aFileName) const
{
  std::vector<bpString> vFileList;

  if (bpFileTools::Exists(aFileName)) {
    vFileList.push_back(aFileName);
  }
  return vFileList;
}


bool bpFileReaderImpl::CanMergeImages() const
{
  return false;
}

void bpFileReaderImpl::SetMergeImages(bool aEnable)
{
}

bool bpFileReaderImpl::GetMergeImages() const
{
  return false;
}


/**
 * Returns the number of datasets (images / experiments)
 */
bpSize bpFileReaderImpl::GetNumberOfDataSets() const
{
  return 1;
}


/**
 * Activate a dataset for subsequent calls
 */
void bpFileReaderImpl::SetActiveDataSetIndex(bpSize aDataSetIndex)
{
}


/**
 * Returns the index of the currently activated dataset
 */
bpSize bpFileReaderImpl::GetActiveDataSetIndex() const
{
  return 0;
}


bool bpFileReaderImpl::SeriesConfigurable() const
{
  return false;
}

bool bpFileReaderImpl::SeriesInitialised() const
{
  return false;
}

bool bpFileReaderImpl::SceneConfigurable() const
{
  return false;
}

void bpFileReaderImpl::SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout)
{
}

std::vector<bpFileReaderImpl::tFileLayout> bpFileReaderImpl::SeriesGetFileLayouts() const
{
  return std::vector<tFileLayout>();
}

bool bpFileReaderImpl::SeriesFileValid(const bpString& aFileName) const
{
  return false;
}


bpSize bpFileReaderImpl::GetNumberOfResolutions() const
{
  return 1;
}

bpSize bpFileReaderImpl::GetActiveResolutionLevel() const
{
  return 0;
}

void bpFileReaderImpl::SetActiveResolutionLevel(bpSize aResolutionLevel)
{
}

/**
 * returns a description of the file reader (normally the static "GetDescription()")
 */
bpString bpFileReaderImpl::GetReaderDescription() const
{
  return "unknown file reader";
}


bpSize bpFileReaderImpl::GetDimension(Dimension aDimension)
{
  bpUInt32 vCount=0;
  std::vector<Dimension> vDimension = GetDimensionSequence();
  while (vDimension[vCount] != aDimension) {
    vCount++;
  }
  return vCount;
}


bpSize bpFileReaderImpl::GetDataSize(bpSize aDimensionOrder)
{
  return GetDataSize()[GetDimensionSequence()[aDimensionOrder]];
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::GetDataSize()
{
  return bpImageIndex(GetDimensionSequence(), GetDataSizeV());
}

bpSize bpFileReaderImpl::GetDataSize(Dimension aDimension)
{
  bpImageIndex vImageIndex(GetDimensionSequence(), GetDataSizeV());
  return vImageIndex[aDimension];
}

bool bpFileReaderImpl::SetDataBlockSize(bpSize aDimension0, bpSize aDimension1, bpSize aDimension2, bpSize aDimension3, bpSize aDimension4, bpSize aResolutionLevel)
{
  // per default Data Block Size cannot
  return false;
}

bpSize bpFileReaderImpl::GetDataBlockSize(bpSize aDimensionOrder)
{
  return GetDataBlockSize()[GetDimensionSequence()[aDimensionOrder]];
}

bpFileReaderImpl::bpImageIndex bpFileReaderImpl::GetDataBlockSize()
{
  return bpImageIndex(GetDimensionSequence(), GetDataBlockSizeV());
}

bpSize bpFileReaderImpl::GetDataBlockSize(Dimension aDimension)
{
  bpImageIndex vImageIndex(GetDimensionSequence(), GetDataBlockSizeV());
  return static_cast<bpSize>(vImageIndex[aDimension]);
}

bpSize bpFileReaderImpl::GetDataBlockNumberOfVoxels()
{
  bpSize vSize = 1;

  std::vector<bpSize> vBlockSize = GetDataBlockSizeV();
  for (bpSize vIndex = 0; vIndex < 5; vIndex++) {
    vSize *= vBlockSize[vIndex];
  }

  return vSize;
}


bpSize bpFileReaderImpl::GetDataBlockNumber(bpSize x, bpSize y, bpSize z,
                                            bpSize aChannel,
                                            bpSize aTimePoint)
{

  // transform start position to data block coordinates
  std::vector<bpSize> vImarisVector(5);
  std::vector<bpSize> vDataVector;

  vImarisVector[0] = x;
  vImarisVector[1] = y;
  vImarisVector[2] = z;
  vImarisVector[3] = aChannel;
  vImarisVector[4] = aTimePoint;

  vDataVector = TransformToData(vImarisVector);

  std::vector<bpSize> vDataSize = GetDataSizeV();
  std::vector<bpSize> vDataBlockSize = GetDataBlockSizeV();

  // number of blocks per unit dimension
  bpSize vBlocksPerUnit1 =                   (vDataSize[0]/vDataBlockSize[0] + (vDataSize[0]%vDataBlockSize[0] > 0 ? 1:0));
  bpSize vBlocksPerUnit2 = vBlocksPerUnit1 * (vDataSize[1]/vDataBlockSize[1] + (vDataSize[1]%vDataBlockSize[1] > 0 ? 1:0));
  bpSize vBlocksPerUnit3 = vBlocksPerUnit2 * (vDataSize[2]/vDataBlockSize[2] + (vDataSize[2]%vDataBlockSize[2] > 0 ? 1:0));
  bpSize vBlocksPerUnit4 = vBlocksPerUnit3 * (vDataSize[3]/vDataBlockSize[3] + (vDataSize[3]%vDataBlockSize[3] > 0 ? 1:0));

  bpSize vBlockCount = 0;

  // vector containing coordinates of end of block
  std::vector<bpSize> vEndOfBlock(5);
  vEndOfBlock[0] = vDataBlockSize[0]-1;
  vEndOfBlock[1] = vDataBlockSize[1]-1;
  vEndOfBlock[2] = vDataBlockSize[2]-1;
  vEndOfBlock[3] = vDataBlockSize[3]-1;
  vEndOfBlock[4] = vDataBlockSize[4]-1;

  // advance until end of block in dimension 4 passes vDataVector[4]
  if (vEndOfBlock[4] < vDataVector[4]) {
    bpSize vDiff = vDataVector[4] - vEndOfBlock[4];
    bpSize vSteps = vDiff / vDataBlockSize[4] + (vDiff % vDataBlockSize[4] == 0 ? 0 : 1);
    vEndOfBlock[4] += vSteps * vDataBlockSize[4];
    vBlockCount += vSteps * vBlocksPerUnit4;
  }

  // advance until end of block in dimension 3 passes vDataVector[3]
  if (vEndOfBlock[3] < vDataVector[3]) {
    bpSize vDiff = vDataVector[3] - vEndOfBlock[3];
    bpSize vSteps = vDiff / vDataBlockSize[3] + (vDiff % vDataBlockSize[3] == 0 ? 0 : 1);
    vEndOfBlock[3] += vSteps * vDataBlockSize[3];
    vBlockCount += vSteps * vBlocksPerUnit3;
  }

  // advance until end of block in dimension 2 passes vDataVector[2]
  if (vEndOfBlock[2] < vDataVector[2]) {
    bpSize vDiff = vDataVector[2] - vEndOfBlock[2];
    bpSize vSteps = vDiff / vDataBlockSize[2] + (vDiff % vDataBlockSize[2] == 0 ? 0 : 1);
    vEndOfBlock[2] += vSteps * vDataBlockSize[2];
    vBlockCount += vSteps * vBlocksPerUnit2;
  }

  // advance until end of block in dimension 1 passes vDataVector[1]
  if (vEndOfBlock[1] < vDataVector[1]) {
    bpSize vDiff = vDataVector[1] - vEndOfBlock[1];
    bpSize vSteps = vDiff / vDataBlockSize[1] + (vDiff % vDataBlockSize[1] == 0 ? 0 : 1);
    vEndOfBlock[1] += vSteps * vDataBlockSize[1];
    vBlockCount += vSteps * vBlocksPerUnit1;
  }

  // advance until end of block in dimension 0 passes vDataVector[0]
  if (vEndOfBlock[0] < vDataVector[0]) {
    bpSize vDiff = vDataVector[0] - vEndOfBlock[0];
    bpSize vSteps = vDiff / vDataBlockSize[0] + (vDiff % vDataBlockSize[0] == 0 ? 0 : 1);
    vEndOfBlock[0] += vSteps * vDataBlockSize[0];
    vBlockCount += vSteps;
  }

  return vBlockCount;


  /*
  // compute size of each dimension
  std::vector<bpUInt32> vDimSize(5);
  vDimSize[0] = GetDataSize(0);
  vDimSize[1] = vDimSize[0] * GetDataSize(1);
  vDimSize[2] = vDimSize[1] * GetDataSize(2);
  vDimSize[3] = vDimSize[2] * GetDataSize(3);
  vDimSize[4] = vDimSize[3] * GetDataSize(4);


  // compute position of voxel
  bpUInt32 position = 0;
  position += vDataVector[4] * vDimSize[3];
  position += vDataVector[3] * vDimSize[2];
  position += vDataVector[2] * vDimSize[1];
  position += vDataVector[1] * vDimSize[0];
  position += vDataVector[0];

  // compute size of DataBlock
  bpUInt32 i=0;
  bpUInt32 vBlockSize = 1;
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


bpSize bpFileReaderImpl::GetNumberOfDataBlocks()
{

  std::vector<bpSize> vDataSize = GetDataSizeV();
  vDataSize = TransformToXYZCT(vDataSize);

  return GetDataBlockNumber(vDataSize[0] - 1,
                            vDataSize[1] - 1,
                            vDataSize[2] - 1,
                            vDataSize[3] - 1,
                            vDataSize[4] - 1) + 1;
}


std::vector<bpSize> bpFileReaderImpl::GetStartCoordinatesOfBlock(bpUInt32 aBlockNumber)
{
  std::vector<bpSize> vPosition(5);

  bpUInt32 vBlockNumber = 0;

  bpSize vInc0 = GetDataBlockSize(0);
  bpSize vInc1 = GetDataBlockSize(1);
  bpSize vInc2 = GetDataBlockSize(2);

  bpSize vDataSize0 = GetDataSize(0);
  bpSize vDataSize1 = GetDataSize(1);
  bpSize vDataSize2 = GetDataSize(2);
  bpSize vDataSize3 = GetDataSize(3);
  bpSize vDataSize4 = GetDataSize(4);

  // loop over dimension five
  for (bpSize vIndex4 = 0; vIndex4 <  vDataSize4 ; vIndex4++) {

    // loop over dimension 4
    for (bpSize vIndex3 = 0; vIndex3 <  vDataSize3 ; vIndex3++) {

      // loop over third dimension
      for (bpSize vIndex2=0; vIndex2< vDataSize2 ; vIndex2+= vInc2) {

        // loop over second dimension
        for (bpSize vIndex1=0; vIndex1< vDataSize1 ; vIndex1+= vInc1) {

          // loop over first dimension
          for (bpSize vIndex0=0; vIndex0< vDataSize0 ; vIndex0+= vInc0) {

            if (vBlockNumber >= aBlockNumber) {

              vPosition[0] = vIndex0;
              vPosition[1] = vIndex1;
              vPosition[2] = vIndex2;
              vPosition[3] = vIndex3;
              vPosition[4] = vIndex4;

              return vPosition;
            }

            vBlockNumber++;

          }
        }
      }
    }
  }

  // should not be reached
  return vPosition;
}


std::vector<bpSize> bpFileReaderImpl::TransformToData(const std::vector<bpSize>& aVector)
{

  std::vector<bpSize> vDataVector(5);

  assert(aVector.size() == 5);

  vDataVector[GetDimension(X)] = aVector[0];
  vDataVector[GetDimension(Y)] = aVector[1];
  vDataVector[GetDimension(Z)] = aVector[2];
  vDataVector[GetDimension(C)] = aVector[3];
  vDataVector[GetDimension(T)] = aVector[4];

  return vDataVector;

}


bpVector3Float bpFileReaderImpl::GetExtentMin() const
{
  bpVector3Float vExtentMin;
  bpVector3Float vExtentMax;
  const_cast<bpFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  return vExtentMin;
}

bpVector3Float bpFileReaderImpl::GetExtentMax() const
{
  bpVector3Float vExtentMin;
  bpVector3Float vExtentMax;
  const_cast<bpFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  return vExtentMax;
}

bpVector3Float bpFileReaderImpl::GetExtentDelta() const
{
  bpVector3Float vExtentMin;
  bpVector3Float vExtentMax;
  const_cast<bpFileReaderImpl*>(this)->GetExtents(vExtentMin, vExtentMax);
  std::vector<bpSize> vDataSize = const_cast<bpFileReaderImpl*>(this)->GetDataSizeV();
  std::vector<bpSize> vDataSizeXYZCT = const_cast<bpFileReaderImpl*>(this)->TransformToXYZCT(vDataSize);
  bpVector3Float vExtentDelta;
  vExtentDelta[0] = (vExtentMax[0] - vExtentMin[0]) / vDataSizeXYZCT[0];
  vExtentDelta[1] = (vExtentMax[1] - vExtentMin[1]) / vDataSizeXYZCT[1];
  vExtentDelta[2] = (vExtentMax[2] - vExtentMin[2]) / vDataSizeXYZCT[2];
  return vExtentDelta;
}

std::vector<bpSize> bpFileReaderImpl::TransformToXYZCT(const std::vector<bpSize>& aVector)
{
  assert(aVector.size() == 5);

  std::vector<bpSize> vImarisVector(5);

  vImarisVector[0] = aVector[GetDimension(X)];
  vImarisVector[1] = aVector[GetDimension(Y)];
  vImarisVector[2] = aVector[GetDimension(Z)];
  vImarisVector[3] = aVector[GetDimension(C)];
  vImarisVector[4] = aVector[GetDimension(T)];

  return vImarisVector;

}

bool bpFileReaderImpl::ReadHistogram(bpHistogram* aHistogram, bpSize aTimePoint, bpSize aChannel)
{
  // per default no histogram are read
  return false;
}

bpSharedPtr<bpThumbnail> bpFileReaderImpl::ReadThumbnail()
{
  return bpSharedPtr<bpThumbnail>();
}

std::vector<bpTimeInfo> bpFileReaderImpl::ReadTimeInfo()
{
  std::vector<bpTimeInfo> vTimeInfo;
  bpSize vNumberOfTimePoints = GetDataSize(GetDimension(T));
  for (bpSize vTimePoint = 0; vTimePoint < vNumberOfTimePoints; vTimePoint++) {
    vTimeInfo.push_back(ReadTimeInfo(vTimePoint));
  }
  return vTimeInfo;
}


bpTimeInfo bpFileReaderImpl::ReadTimeInfo(bpSize aTimePoint)
{
  bpTimeInfo vTimeInfo;
  vTimeInfo.SetTime("00:00:" + bpToString(aTimePoint));
  return vTimeInfo;
}


std::vector<bpColorInfo> bpFileReaderImpl::ReadColorInfo()
{
  std::vector<bpColorInfo> vColors;
  bpSize vNumberOfChannels = GetDataSize(GetDimension(C));
  for (bpSize vChannelId = 0; vChannelId < vNumberOfChannels; vChannelId++) {
    vColors.push_back(ReadColorInfo(vChannelId));
  }
  return vColors;
}


bpColorInfo bpFileReaderImpl::ReadColorInfo(bpSize aChannel)
{
  bpSize vSizeC = GetDataSize(GetDimension(bpFileReaderImpl::C));

  if (vSizeC == 1) {
    return bpColorInfo(GetDefaultColor(bpFileConversionColorsConstant::mNumberOfUniqueChannels - 1));
  }
  else {
    return bpColorInfo(GetDefaultColor(aChannel));
  }
}


bool bpFileReaderImpl::ShouldColorRangeBeAdjustedToMinMax()
{
  return false;
}


void bpFileReaderImpl::SetColorHint(bpColorHint aColorHint)
{
  mColorHint = aColorHint;
}


bpColorHint bpFileReaderImpl::GetColorHint()
{
  return mColorHint;
}


const bpColor& bpFileReaderImpl::GetDefaultColor(bpSize aChannelId)
{
  return mFileConversionColors.GetDefaultColor(aChannelId);
}


const bpFileConversionColors::tDefaultColors& bpFileReaderImpl::GetDefaultColors()
{
  return mFileConversionColors.GetDefaultColors();
}


void bpFileReaderImpl::SetDefaultColors(const bpFileConversionColors::tDefaultColors& aDefaultColors)
{
  mFileConversionColors.SetDefaultColors(aDefaultColors);
}


bool bpFileReaderImpl::NaNCheckStatus() const
{
  return false;
}

bpString bpFileReaderImpl::GuessUnit(const bpString& aSomeUnit)
{
  bpString vUnit = bpToUpper(aSomeUnit);
  if ((vUnit == "NM") ||
      (vUnit.find("NANO") != bpString::npos)) {
    return "nm";
  }
  else if ((vUnit == "UM") ||
           (vUnit.find("MICRO") != bpString::npos)) {
    return "um";
  }
  else if ((vUnit == "MM") ||
           (vUnit.find("MILLI") != bpString::npos)) {
    return "mm";
  }
  else if ((vUnit == "M") ||
           (vUnit.find("METER") != bpString::npos)) {
    return "m";
  }
  else {
    return "";
  }
}
