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


#include "bpThumbnailImageConverter.h"
#include "bpWriterFactoryFileThumbnail.h"
#include ICDeriche_h

#include <cstring>

using namespace bpConverterTypes;


template<typename TDataType>
bpThumbnailImageConverter<TDataType>::bpThumbnailImageConverter(
  tDataType aDataType, const tSize5D& aImageSize, const tSize5D& aSample,
  tDimensionSequence5D aDimensionSequence, const tSize5D& aFileBlockSize,
  const bpString& aOutputFile, const bpConverterTypes::cOptions& aWriteOptions, bool aCompressThumbnail)
  : mDimensionSequence(aDimensionSequence), mImageSize(aImageSize), mFileBlockSize(aFileBlockSize),
    mSample(aSample), mMinLimit(InitMapWithConstant(0)), mMaxLimit(aImageSize),
    mMultiresolutionImage(Div(aImageSize[X], aSample[X]), Div(aImageSize[Y], aSample[Y]), Div(aImageSize[Z], aSample[Z]), Div(aImageSize[C], aSample[C]),
    Div(aImageSize[T], aSample[T]), aDataType,
    { aFileBlockSize[X], aFileBlockSize[Y] }, { aSample[X], aSample[Y] },
    std::make_shared<bpWriterFactoryFileThumbnail>(aCompressThumbnail ? bpWriterFileThumbnail::tFormat::eJPEG : bpWriterFileThumbnail::tFormat::ePNG),
    aOutputFile, aWriteOptions.mCompressionAlgorithmType, aWriteOptions.mThumbnailSizeXY, aWriteOptions.mForceFileBlockSizeZ1, 0)
{
  mIsFlipped[0] = aWriteOptions.mFlipDimensionXYZ[0];
  mIsFlipped[1] = aWriteOptions.mFlipDimensionXYZ[1];
  mIsFlipped[2] = aWriteOptions.mFlipDimensionXYZ[2];

  // compute number of file blocks
  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    Dimension vDim = aDimensionSequence[vDimIndex];
    mNFileBlocks[vDimIndex] = Div(aImageSize[vDim], aFileBlockSize[vDim]);
  }
}


template<typename TDataType>
bpThumbnailImageConverter<TDataType>::~bpThumbnailImageConverter()
{
}


template<typename TDataType>
tSize5D bpThumbnailImageConverter<TDataType>::InitMapWithConstant(bpSize aValue)
{
  return tSize5D(X, aValue, Y, aValue, Z, aValue, C, aValue, T, aValue);
}


template<typename TDataType>
bpSize bpThumbnailImageConverter<TDataType>::Div(bpSize aNum, bpSize aDiv)
{
  return (aNum + aDiv - 1) / aDiv;
}


template<typename TDataType>
bool bpThumbnailImageConverter<TDataType>::NeedCopyBlock(const bpConverterTypes::tIndex5D& aBlockIndex) const
{
  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    bpSize vBegin;
    bpSize vEnd;
    Dimension vDim = mDimensionSequence[vDimIndex];
    GetFullRangeOfFileBlock(aBlockIndex[vDim], vDim, vBegin, vEnd);
    if (vEnd <= vBegin) {
      return false;
    }
  }
  return true;
}


template<typename TDataType>
void bpThumbnailImageConverter<TDataType>::CopyBlock(const TDataType* aFileDataBlock, const bpConverterTypes::tIndex5D& aBlockIndex)
{
  if (aFileDataBlock) {
    CopyFileBlockToImage(aBlockIndex, aFileDataBlock);
  }
}


template<typename TDataType>
void bpThumbnailImageConverter<TDataType>::Finish(
  const bpConverterTypes::cImageExtent& aImageExtent,
  const tParameters& aParameters,
  const tTimeInfoVector& aTimeInfoPerTimePoint,
  const tColorInfoVector& aColorInfoPerChannel,
  bool aAutoAdjustColorRange)
{
  mMultiresolutionImage.FinishWriteDataBlocks();
  tColorInfoVector vColorInfoPerChannel(aColorInfoPerChannel);
  if (aAutoAdjustColorRange) {
    AdjustColorRange(vColorInfoPerChannel);
  }
  mMultiresolutionImage.WriteMetadata({}, {}, aImageExtent, aParameters, aTimeInfoPerTimePoint, vColorInfoPerChannel);
}


template <typename TDataType>
bpHistogram bpThumbnailImageConverter<TDataType>::GetConversionImageHistogram(bpSize aIndexC) const
{
  return mMultiresolutionImage.GetChannelHistogram(aIndexC);
}


template <typename TDataType>
void bpThumbnailImageConverter<TDataType>::GetRangeOfFileBlock(bpSize aFileBlockIndex, Dimension aDimension, bpSize& aBeginInBlock, bpSize& aEndInBlock) const
{
  Dimension vDim = aDimension;
  bpSize vBegin = aFileBlockIndex * mFileBlockSize[vDim];
  if (vBegin > mMinLimit[vDim]) {
    bpSize vOffset = vBegin - mMinLimit[vDim];
    bpSize vCount = (vOffset + mSample[vDim] - 1) / mSample[vDim];
    aBeginInBlock = vCount * mSample[vDim] - vOffset;
  }
  else {
    aBeginInBlock = mMinLimit[vDim] - vBegin;
  }
  bpSize vEnd = (aFileBlockIndex + 1) * mFileBlockSize[vDim];
  if (vEnd <= mMaxLimit[vDim]) {
    aEndInBlock = mFileBlockSize[vDim];
  }
  else {
    aEndInBlock = mMaxLimit[vDim] - vBegin;
  }
}


template <typename TDataType>
void bpThumbnailImageConverter<TDataType>::GetFullRangeOfFileBlock(bpSize aFileBlockIndex, Dimension aDimension, bpSize& aBegin, bpSize& aEnd) const
{
  bpSize vBeginInBlock;
  bpSize vEndInBlock;
  Dimension vDim = aDimension;
  GetRangeOfFileBlock(aFileBlockIndex, vDim, vBeginInBlock, vEndInBlock);

  bpSize vBegin = aFileBlockIndex * mFileBlockSize[vDim] + vBeginInBlock;
  aBegin = (vBegin - mMinLimit[vDim]) / mSample[vDim];
  bpSize vEnd = (aFileBlockIndex + 1) * mFileBlockSize[vDim];
  aEnd = (vEnd - mMinLimit[vDim] + mSample[vDim] - 1) / mSample[vDim];
}


template<typename TDataType>
void bpThumbnailImageConverter<TDataType>::CopyFileBlockToImage(const tSize5D& aFileBlockIndices, const TDataType* aDataBlock)
{
  bpSize vDimNrX;
  bpSize vDimNrY;

  // find position of DimX and DimY in dimensionSequence
  for (bpSize vDim = 0; vDim < 5; ++vDim) {
    if (mDimensionSequence[vDim] == X) {
      vDimNrX = vDim;
    }
    if (mDimensionSequence[vDim] == Y) {
      vDimNrY = vDim;
    }
  }

  bpSize vFileBlockSize[5];
  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    Dimension vDimension = mDimensionSequence[vDimIndex];
    bpSize vBlockSize = mFileBlockSize[vDimension];
    vFileBlockSize[vDimIndex] = vBlockSize;
  }

  bpSize vFileBlockIndices[5];
  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    Dimension vDimension = mDimensionSequence[vDimIndex];
    bpSize vBlockIndex = aFileBlockIndices[vDimension];
    vFileBlockIndices[vDimIndex] = vBlockIndex;
  }

  bpSize vDimensionMap[5];
  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    Dimension vDimension = mDimensionSequence[vDimIndex];
    switch (vDimension) {
    case X:
      vDimensionMap[vDimIndex] = 0;
      continue;
    case Y:
      vDimensionMap[vDimIndex] = 1;
      continue;
    case Z:
      vDimensionMap[vDimIndex] = 2;
      continue;
    case C:
      vDimensionMap[vDimIndex] = 3;
      continue;
    case T:
      vDimensionMap[vDimIndex] = 4;
      continue;
    }
  }

  // needs to be in order of memoryDimensionSequence
  bpSize vMemSample[5];
  vMemSample[0] = mSample[X];
  vMemSample[1] = mSample[Y];
  vMemSample[2] = mSample[Z];
  vMemSample[3] = mSample[C];
  vMemSample[4] = mSample[T];

  bpSize vMemMinLimits[5];
  vMemMinLimits[0] = mMinLimit[X];
  vMemMinLimits[1] = mMinLimit[Y];
  vMemMinLimits[2] = mMinLimit[Z];
  vMemMinLimits[3] = mMinLimit[C];
  vMemMinLimits[4] = mMinLimit[T];

  bpSize vMemMaxLimits[5];
  vMemMaxLimits[0] = mMaxLimit[X];
  vMemMaxLimits[1] = mMaxLimit[Y];
  vMemMaxLimits[2] = mMaxLimit[Z];
  vMemMaxLimits[3] = mMaxLimit[C];
  vMemMaxLimits[4] = mMaxLimit[T];


  bpSize vDimWeight[5];
  bpSize vBeginInBlock[5]; // Offset in the block to the beginning of the data for the nth dimension (min is 0 and max is blocksize - 1)
  bpSize vEndInBlock[5]; // Offset in the block to the end of the data (of the block) for the nth dimension (min is 1 and max is blocksize)
  bpSize vNonXYDim[3];
  bpSize vNonXYDimIndex = 0;

  bpSize vDimX = vDimNrX;
  bpSize vDimY = vDimNrY;
  for (bpSize vDim = 0; vDim < 5; ++vDim) {
    GetRangeOfFileBlock(vFileBlockIndices[vDim], mDimensionSequence[vDim], vBeginInBlock[vDim], vEndInBlock[vDim]);

    vDimWeight[vDim] = vDim > 0 ? vFileBlockSize[vDim - 1] * vDimWeight[vDim - 1] : 1;

    if (vDim != vDimX && vDim != vDimY) {
      vNonXYDim[vNonXYDimIndex++] = vDim;
    }
  }

  bpSize vDim2 = vNonXYDim[2];
  bpSize vD2 = vDimensionMap[vDim2];
  bpSize vDim1 = vNonXYDim[1];
  bpSize vD1 = vDimensionMap[vDim1];
  bpSize vDim0 = vNonXYDim[0];
  bpSize vD0 = vDimensionMap[vDim0];

  bpSize vBeginX;
  bpSize vEndX;
  GetFullRangeOfFileBlock(vFileBlockIndices[vDimX], mDimensionSequence[vDimX], vBeginX, vEndX);

  bpSize vBeginY;
  bpSize vEndY;
  GetFullRangeOfFileBlock(vFileBlockIndices[vDimY], mDimensionSequence[vDimY], vBeginY, vEndY);

  bpSize vDimWeightX = vDimWeight[vDimX];
  bpSize vDimWeightY = vDimWeight[vDimY];
  bpSize vStepX = vMemSample[0] * vDimWeightX;
  bpSize vStepY = vMemSample[1] * vDimWeightY;

  bpSize vSizeX = vEndX - vBeginX;
  bpSize vSizeY = vEndY - vBeginY;
  bpSize vSizeXY = vSizeX * vSizeY;

  bool vIsFlippedX = mIsFlipped[0];
  bool vIsFlippedY = mIsFlipped[1];

  bool vCanRawCopy =
    vDimX == 0 && vDimY == 1 &&
    vMemSample[0] == 1 && vMemSample[1] == 1 &&
    vBeginInBlock[vDimX] == 0 && vEndInBlock[vDimX] == vFileBlockSize[vDimX] &&
    !vIsFlippedX && !vIsFlippedY;

  bpSize vDimOffset[3];
  bpSize vImageIndex[5]; // Image global index for dimension X, Y, Z, C, T (the order is 0, 1, 2, 3, 4)
  for (bpSize vIndex2 = vBeginInBlock[vDim2]; vIndex2 < vEndInBlock[vDim2]; vIndex2 += vMemSample[vD2]) {
    bpSize vImageIndex2 = vFileBlockIndices[vDim2] * vFileBlockSize[vDim2] + vIndex2;
    vImageIndex[vD2] = (vImageIndex2 - vMemMinLimits[vD2]) / vMemSample[vD2];
    vDimOffset[2] = vDimWeight[vDim2] * vIndex2;

    for (bpSize vIndex1 = vBeginInBlock[vDim1]; vIndex1 < vEndInBlock[vDim1]; vIndex1 += vMemSample[vD1]) {
      bpSize vImageIndex1 = vFileBlockIndices[vDim1] * vFileBlockSize[vDim1] + vIndex1;
      vImageIndex[vD1] = (vImageIndex1 - vMemMinLimits[vD1]) / vMemSample[vD1];
      vDimOffset[1] = vDimWeight[vDim1] * vIndex1;

      for (bpSize vIndex0 = vBeginInBlock[vDim0]; vIndex0 < vEndInBlock[vDim0]; vIndex0 += vMemSample[vD0]) {
        bpSize vImageIndex0 = vFileBlockIndices[vDim0] * vFileBlockSize[vDim0] + vIndex0;
        vImageIndex[vD0] = (vImageIndex0 - vMemMinLimits[vD0]) / vMemSample[vD0];
        vDimOffset[0] = vDimWeight[vDim0] * vIndex0;

        bpSize vDataOffset = vDimOffset[2] + vDimOffset[1] + vDimOffset[0] + vDimWeightY * vBeginInBlock[vDimY] + vDimWeightX * vBeginInBlock[vDimX];
        const TDataType* vDataBlock = aDataBlock + vDataOffset;

        if (!vCanRawCopy) {
          // start read there but with different step size
          mTempBuffer.resize(vSizeXY);
          TDataType* vBuffer = mTempBuffer.data();
          for (bpSize vIndexY = 0; vIndexY < vSizeY; ++vIndexY) {
            const TDataType* vSource = vDataBlock + (vIndexY * vStepY);
            TDataType* vPtr = vBuffer + ((!vIsFlippedY ? vIndexY : vSizeY - vIndexY - 1) * vSizeX);
            if (!vIsFlippedX) {
              if (vStepX == 1) {
                std::memcpy(vPtr, vSource, vSizeX * sizeof(TDataType));
              }
              else {
                for (bpSize vIndexX = 0; vIndexX < vSizeX; ++vIndexX) {
                  *vPtr = *vSource;
                  ++vPtr;
                  vSource += vStepX;
                }
              }
            }
            else {
              vPtr += vSizeX - 1;
              for (bpSize vIndexX = 0; vIndexX < vSizeX; ++vIndexX) {
                *vPtr = *vSource;
                --vPtr;
                vSource += vStepX;
              }
            }
          }
          vDataBlock = vBuffer;
        }

        vImageIndex[2] = mIsFlipped[2] ? mImageSize[Z] - vImageIndex[2] - 1 : vImageIndex[2];

        // The indices here are memoryIndices for each dim, not block indices!
        // T,C,Z are only one memoryIndex (for each dim), X,Y can contain span (vBegin, vEnd) over a range of memoryIndices (for each dim)
        mMultiresolutionImage.CopyData(vImageIndex[4], vImageIndex[3], vImageIndex[2], { vFileBlockIndices[vDimX], vFileBlockIndices[vDimY] }, vDataBlock);
      }
    }
  }
}

template <typename TDataType>
void bpThumbnailImageConverter<TDataType>::AdjustColorRange(std::vector<cColorInfo>& aColorInfo) const
{
  for (bpSize vIndexC = 0; vIndexC < aColorInfo.size(); vIndexC++) {
    const bpHistogram& vHistogram = GetConversionImageHistogram(vIndexC);
    bpSize vSize = vHistogram.GetNumberOfBins();
    std::vector<bpFloat> vBins = GetFilteredBins(vHistogram, 5.0f * vSize / 256);
    bpFloat vPrevious = -1.0f;
    bpSize vFirstModeBinId = 0;
    for (bpSize vBinId = 0; vBinId < vSize - 1; ++vBinId) {
      if (vBins[vBinId] > vPrevious && vBins[vBinId] > vBins[vBinId + 1]) {
        vFirstModeBinId = vBinId;
        break;
      }
      vPrevious = vBins[vBinId];
    }

    bpSize vHighPercentileBinId = vSize - 1;
    bpUInt64 vTotalVolumeSum = 0;
    for (bpSize vBinId = 0; vBinId < vSize; vBinId++) {
      vTotalVolumeSum += vBins[vBinId];
    }
    bpUInt64 vVolumeSum = 0;
    for (bpSize vBinId = 0; vBinId < vSize; vBinId++) {
      vVolumeSum += vBins[vBinId];
      if (vVolumeSum / (bpFloat)vTotalVolumeSum > 0.998f) {
        vHighPercentileBinId = vBinId;
        break;
      }
    }


    bpFloat vMin = vHistogram.GetBinValue(vFirstModeBinId);
    bpFloat vMax = vHistogram.GetBinValue(vHighPercentileBinId);
    vMax = vMax + (vMax - vMin) * 0.2f;
    vMax = static_cast<bpInt32>(std::min(vMax, vHistogram.GetMax()));

    aColorInfo[vIndexC].mRangeMin = vMin;
    aColorInfo[vIndexC].mRangeMax = vMax;
  }
}

template <typename TDataType>
std::vector<bpFloat> bpThumbnailImageConverter<TDataType>::GetFilteredBins(const bpHistogram& aHistogram, bpFloat aFilterWidth)
{
  bpSize vSize = aHistogram.GetNumberOfBins();

  std::vector<bpFloat> vOriginal(vSize);
  for (bpSize vBinId = 0; vBinId < vSize; vBinId++) {
    vOriginal[vBinId] = static_cast<bpFloat>(aHistogram.GetCount(vBinId));
  }
  std::vector<bpFloat> vFiltered(vSize);
  bpDeriche::FilterGauss(vOriginal.data(), vFiltered.data(), vSize, aFilterWidth);
  return vFiltered;
}


template class bpThumbnailImageConverter<bpUInt8>;
template class bpThumbnailImageConverter<bpUInt16>;
template class bpThumbnailImageConverter<bpUInt32>;
template class bpThumbnailImageConverter<bpFloat>;
