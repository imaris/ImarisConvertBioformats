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


#ifndef __BP_THUMBNAIL_IMAGE_CONVERTER__
#define __BP_THUMBNAIL_IMAGE_CONVERTER__

#include "src/bpWriterCommonHeaders.h"
#include "ImarisWriter/interface/bpImageConverterInterface.h"
#include  ICMultiresolutionImsImage_h


template<typename TDataType>
class bpThumbnailImageConverter : public bpImageConverterInterface<TDataType>
{
public:
  bpThumbnailImageConverter(
    bpConverterTypes::tDataType aDataType, const bpConverterTypes::tSize5D& aImageSize, const bpConverterTypes::tSize5D& aSample,
    bpConverterTypes::tDimensionSequence5D aDimensionSequence, const bpConverterTypes::tSize5D& aFileBlockSize,
    const bpString& aOutputFile, const bpConverterTypes::cOptions& aWriteOptions, bool aCompressThumbnail);
  ~bpThumbnailImageConverter();

  bool NeedCopyBlock(const bpConverterTypes::tIndex5D& aBlockIndex) const override;

  void CopyBlock(const TDataType* aFileDataBlock, const bpConverterTypes::tIndex5D& aBlockIndex) override;

  void Finish(
    const bpConverterTypes::cImageExtent& aImageExtent,
    const bpConverterTypes::tParameters& aParameters,
    const bpConverterTypes::tTimeInfoVector& aTimeInfoPerTimePoint,
    const bpConverterTypes::tColorInfoVector& aColorInfoPerChannel,
    bool aAutoAdjustColorRange) override;

private:
  using tSize5D = bpConverterTypes::tSize5D;

  static tSize5D InitMapWithConstant(bpSize aValue);
  static bpSize Div(bpSize aNum, bpSize aDiv);

  void GetRangeOfFileBlock(bpSize aFileBlockIndex, bpConverterTypes::Dimension aDimension, bpSize& aBeginInBlock, bpSize& aEndInBlock) const;
  void GetFullRangeOfFileBlock(bpSize aFileBlockIndex, bpConverterTypes::Dimension aDimension, bpSize& aBegin, bpSize& aEnd) const;
  void CopyFileBlockToImage(const tSize5D& aFileBlockIndices, const TDataType* aDataBlock);
  bpHistogram GetConversionImageHistogram(bpSize aIndexC) const;
  void AdjustColorRange(std::vector<bpConverterTypes::cColorInfo>& aColorInfo) const;
  static std::vector<bpFloat> GetFilteredBins(const bpHistogram& aHistogram, bpFloat aFilterWidth);

  bpConverterTypes::tDimensionSequence5D mDimensionSequence;   // file dimension sequence
  tSize5D mImageSize;
  tSize5D mFileBlockSize;
  std::array<bpSize, 5> mNFileBlocks = { 0 };  // in file dimension sequence

  tSize5D mSample;
  tSize5D mMinLimit;
  tSize5D mMaxLimit;
  bool mIsFlipped[3];  // for X,Y,Z dim

  bpMultiresolutionImsImage<TDataType> mMultiresolutionImage;

  std::vector<TDataType> mTempBuffer;
};
#endif // __BP_THUMBNAIL_IMAGE_CONVERTER__
