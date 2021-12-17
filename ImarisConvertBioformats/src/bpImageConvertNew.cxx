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


#include "bpImageConvertNew.h"

#include "bpWriterCommonHeaders.h"
#include "ImarisWriter/interface/bpImageConverter.h"
#include "bpConverterProgress.h"
#include "bpThroughputMeasurementsAggregator.h"
#include "bpConverterVersion.h"
#include "../thumbnailFile/bpWriterFileThumbnail.h"
#include "../thumbnailFile/bpThumbnailImageConverter.h"
#include "../meta/bpParameterSection.h"


using namespace bpConverterTypes;


int bpCalcWaveLengthColor(bpFloat wl, bpColor& lsibcolor)
{
  float fred = 0.0f;
  float fgreen = 0.0f;
  float fblue = 0.0f;

  lsibcolor.SetRGB(fred, fgreen, fblue);

  if (wl < 380.0f || wl > 780.0f) return 1;

  if (wl >= 380.0f && wl <= 440.0f) {
    fred = (440.0f - wl) / 60.0f;
    fgreen = 0.0f;
    fblue = 1.0f;
  }

  if (wl > 440.0f && wl <= 490.0f) {
    fred = 0.0f;
    fgreen = (wl - 440.0f) / 50.0f;
    fblue = 1.0f;
  }

  if (wl > 490.0f && wl <= 510.0f) {
    fred = 0.0f;
    fgreen = 1.0f;
    fblue = (510.0f - wl) / 20.0f;
  }

  if (wl > 510.0f && wl <= 580.0f) {
    fred = (wl - 510.0f) / 70.0f;
    fgreen = 1.0f;
    fblue = 0.0f;
  }

  if (wl > 580.0f && wl <= 645.0f) {
    fred = 1.0f;
    fgreen = (645.0f - wl) / 65.0f;
    fblue = 0.0f;
  }

  if (wl > 645.0f && wl <= 780.0f) {
    fred = 1.0f;
    fgreen = 0.0f;
    fblue = 0.0f;
  }

  lsibcolor.SetRGB(fred, fgreen, fblue);

  return 0;
}


class bpImageConvertNew::cImpl
{
public:
  using tReaderImplPtr = bpFileReaderImpl::tPtr;

  static void Convert(const tReaderPtr& aReader, const bpString& aDestinationFile, const cConvertOptions& aConvertOptions, const cOptions& aWriteOptions);

private:
  template<typename TDataType>
  static void ConvertT(const tReaderImplPtr& aReader, const bpString& aOutputFile, const cConvertOptions& aConvertOptions, cOptions aWriteOptions, const bpVector3Float& aForcedVoxelSize);
  static tDimensionSequence5D GetDimensionSequence(const tReaderImplPtr& aReader);
  static tSize5D GetDataSize(const tReaderImplPtr& aReader);
  static tSize5D GetDataBlockSize(const tReaderImplPtr& aReader);
  static void GetImageInfo(const tReaderImplPtr& aReader, const bpVector3Float& aForcedVoxelSize, tDataType& aDataType, cImageExtent& aImageExtent);
  static void GetImageMetadata(const tReaderImplPtr& aReader, tColorInfoVector& aColorInfoPerChannel, tTimeInfoVector& aTimeInfoPerTimePoint, tParameters& aParameters);
  static tDataType MapDataType(bpNumberType aDataType);
  static void SetColorInfo(const tReaderImplPtr& aReader, const bpSectionContainer& aParameters, tColorInfoVector& ColorInfoPerChannel);
  static void MapColor(const bpColor& aSourceColor, cColor& aTargetColor);
  static bpSharedPtr<bpThumbnail> ExtractImarisThumbnail(const tReaderPtr& aReader);
  static bpSize Div(bpSize aNum, bpSize aDiv);
};


tDimensionSequence5D bpImageConvertNew::cImpl::GetDimensionSequence(const tReaderImplPtr& aReader)
{
  std::vector<bpFileReaderImpl::Dimension> vDimensionSequence = aReader->GetDimensionSequence();

  std::vector<Dimension> vDimensionSequence5D;

  for (bpFileReaderImpl::Dimension vDimension : vDimensionSequence) {
    switch (vDimension) {
    case bpFileReaderImpl::X:
      vDimensionSequence5D.push_back(X);
      break;
    case bpFileReaderImpl::Y:
      vDimensionSequence5D.push_back(Y);
      break;
    case bpFileReaderImpl::Z:
      vDimensionSequence5D.push_back(Z);
      break;
    case bpFileReaderImpl::C:
      vDimensionSequence5D.push_back(C);
      break;
    case bpFileReaderImpl::T:
      vDimensionSequence5D.push_back(T);
      break;
    }
  }

  if (vDimensionSequence5D.size() != 5) {
    throw std::runtime_error("Unexpected size of dimensionSequence");
  }

  return tDimensionSequence5D(vDimensionSequence5D[0], vDimensionSequence5D[1], vDimensionSequence5D[2], vDimensionSequence5D[3], vDimensionSequence5D[4]);
}


tSize5D bpImageConvertNew::cImpl::GetDataSize(const tReaderImplPtr& aReader)
{
  bpFileReaderImpl::bpImageIndex vImageIndex = aReader->GetDataSize();

  return tSize5D{
    { X, vImageIndex[bpFileReaderImpl::X] },
    { Y, vImageIndex[bpFileReaderImpl::Y] },
    { C, vImageIndex[bpFileReaderImpl::C] },
    { T, vImageIndex[bpFileReaderImpl::T] },
    { Z, vImageIndex[bpFileReaderImpl::Z] }
  };
}


tSize5D bpImageConvertNew::cImpl::GetDataBlockSize(const tReaderImplPtr& aReader)
{
  bpFileReaderImpl::bpImageIndex vImageIndex = aReader->GetDataBlockSize();

  return tSize5D{
    { X, vImageIndex[bpFileReaderImpl::X] },
    { Y, vImageIndex[bpFileReaderImpl::Y] },
    { C, vImageIndex[bpFileReaderImpl::C] },
    { T, vImageIndex[bpFileReaderImpl::T] },
    { Z, vImageIndex[bpFileReaderImpl::Z] }
  };
}


void bpImageConvertNew::cImpl::GetImageInfo(const tReaderImplPtr& aReader, const bpVector3Float& aForcedVoxelSize, tDataType& aDataType, cImageExtent& aImageExtent)
{
  bpVector3Float vMin, vMax;
  aReader->GetExtents(vMin, vMax);
  tSize5D vImageSize = GetDataSize(aReader);


  aImageExtent.mExtentMinX = vMin[0];
  aImageExtent.mExtentMinY = vMin[1];
  aImageExtent.mExtentMinZ = vMin[2];
  aImageExtent.mExtentMaxX = aForcedVoxelSize[0] == 0 ? vMax[0] : vMin[0] + vImageSize[X] * aForcedVoxelSize[0];
  aImageExtent.mExtentMaxY = aForcedVoxelSize[1] == 0 ? vMax[1] : vMin[1] + vImageSize[Y] * aForcedVoxelSize[1];
  aImageExtent.mExtentMaxZ = aForcedVoxelSize[2] == 0 ? vMax[2] : vMin[2] + vImageSize[Z] * aForcedVoxelSize[2];
  aDataType = MapDataType(aReader->GetDataType());
}


void bpImageConvertNew::cImpl::GetImageMetadata(const tReaderImplPtr& aReader, tColorInfoVector& aColorInfoPerChannel, tTimeInfoVector& aTimeInfoPerTimePoint, tParameters& aParameters)
{
  const auto& vParams = aReader->ReadParameters();
  SetColorInfo(aReader, vParams, aColorInfoPerChannel);

  bpSectionContainer::const_iterator vIt = vParams.begin();
  bpSectionContainer::const_iterator vEnd = vParams.end();
  for (; vIt != vEnd; ++vIt) {
    auto& vParameterSection = aParameters[vIt->first];
    bpParameterSection::const_iterator vParamIt = vIt->second->begin();
    bpParameterSection::const_iterator vParamEnd = vIt->second->end();
    for (; vParamIt != vParamEnd; ++vParamIt) {
      vParameterSection.emplace_hint(vParameterSection.end(), vParamIt->first, vParamIt->second);
    }
  }

  bpSize vNumberOfTimepoints = GetDataSize(aReader)[Dimension::T];
  for (bpSize vTimeIndex = 0; vTimeIndex < vNumberOfTimepoints; vTimeIndex++) {
    bpTimeInfo vTime = aReader->ReadTimeInfo(vTimeIndex);
    cTimeInfo vTimeInfo;
    vTimeInfo.mJulianDay = vTime.GetJulianDay();
    vTimeInfo.mNanosecondsOfDay = vTime.GetNanoseconds();
    aTimeInfoPerTimePoint.push_back(vTimeInfo);
  }
}


tDataType bpImageConvertNew::cImpl::MapDataType(bpNumberType aDataType)
{
  tDataType vDataType;
  switch (aDataType) {
  case bpNumberType::bpUInt8Type:
    vDataType = bpConverterTypes::bpUInt8Type;
    break;
  case bpNumberType::bpUInt16Type:
    vDataType = bpConverterTypes::bpUInt16Type;
    break;
  case bpNumberType::bpUInt32Type:
    vDataType = bpConverterTypes::bpUInt32Type;
    break;
  case bpNumberType::bpFloatType:
    vDataType = bpConverterTypes::bpFloatType;
    break;
  default:
    vDataType = bpConverterTypes::bpNoType;
    break;
  }
  return vDataType;
}


void bpImageConvertNew::cImpl::SetColorInfo(const tReaderImplPtr& aReader, const bpSectionContainer& aParameters, tColorInfoVector& aColorInfoPerChannel)
{
  bpSize vNumberOfChannels = GetDataSize(aReader)[Dimension::C];

  for (bpSize vChannelIndex = 0; vChannelIndex < vNumberOfChannels; vChannelIndex++) {
    bpColorInfo vChannelColor = aReader->ReadColorInfo(vChannelIndex);
    auto vChannelIndexText = "Channel " + bpToString(vChannelIndex);

    if (bpColorHint::eDefault == aReader->GetColorHint()) {
      vChannelColor.SetBaseColor(aReader->GetDefaultColor(vChannelIndex));
    }
    else if ((bpColorHint::eEmission == aReader->GetColorHint()) && (aParameters.HasParameter(vChannelIndexText, "LSMEmissionWavelength"))) {
      const auto& vEmissionWavelength = aParameters.GetParameter(vChannelIndexText, "LSMEmissionWavelength");

      if (bpString::npos != vEmissionWavelength.find_first_of("0123456789")) {
        bpColor vEmissionColor;
        bpCalcWaveLengthColor(bpFromString<bpFloat>(vEmissionWavelength), vEmissionColor);
        vChannelColor.SetBaseColor(vEmissionColor);
      }
      else {
        vChannelColor.SetBaseColor(aReader->GetDefaultColor(vChannelIndex));
      }
    }

    cColorInfo vColorInfo;
    vColorInfo.mIsBaseColorMode = vChannelColor.GetColorMode() == bpColorInfo::tModeType::eBaseColor;
    vColorInfo.mOpacity = vChannelColor.GetOpacity();
    vColorInfo.mRangeMin = vChannelColor.GetRangeMin();
    vColorInfo.mRangeMax = vChannelColor.GetRangeMax();
    vColorInfo.mGammaCorrection = vChannelColor.GetGammaCorrectionValue();

    MapColor(vChannelColor.GetBaseColor(), vColorInfo.mBaseColor);
    if (vColorInfo.mBaseColor.mRed == 0 && vColorInfo.mBaseColor.mGreen == 0 && vColorInfo.mBaseColor.mBlue == 0) {
      MapColor(aReader->GetDefaultColor(vChannelIndex), vColorInfo.mBaseColor);
    }

    for (auto vColorEntry : vChannelColor.GetTable()) {
      cColor vColor;
      MapColor(vColorEntry, vColor);
      vColorInfo.mColorTable.push_back(vColor);
    }

    aColorInfoPerChannel.push_back(vColorInfo);
  }
}


void bpImageConvertNew::cImpl::MapColor(const bpColor& aSourceColor, cColor& aTargetColor)
{
  aSourceColor.GetRGB(aTargetColor.mRed, aTargetColor.mGreen, aTargetColor.mBlue);
  aTargetColor.mAlpha = 1;
}


bpSharedPtr<bpThumbnail> bpImageConvertNew::cImpl::ExtractImarisThumbnail(const tReaderPtr& aReader)
{
  bool vIsImarisFormat = aReader->GetDescription().find("Imaris") != bpString::npos;
  if (!vIsImarisFormat) {
    return {};
  }

  bpSharedPtr<bpThumbnail> vThumbnail = aReader->ReadThumbnail();
  if (!vThumbnail || vThumbnail->GetSizeX() < 128) {
    return {};
  }

  return vThumbnail;
}


bpSize bpImageConvertNew::cImpl::Div(bpSize aNum, bpSize aDiv)
{
  return (aNum + aDiv - 1) / aDiv;
}


template<typename TDataType>
void bpImageConvertNew::cImpl::ConvertT(const tReaderImplPtr& aReader, const bpString& aOutputFile, const cConvertOptions& aConvertOptions, cOptions aWriteOptions, const bpVector3Float& aForcedVoxelSize)
{
  bpSize vThumbnailSize = aWriteOptions.mThumbnailSizeXY;

  tDataType vDataType;
  cImageExtent vImageExtent;
  GetImageInfo(aReader, aForcedVoxelSize, vDataType, vImageExtent);

  aWriteOptions.mFlipDimensionXYZ[0] = vImageExtent.mExtentMinX > vImageExtent.mExtentMaxX;
  aWriteOptions.mFlipDimensionXYZ[1] = vImageExtent.mExtentMinY > vImageExtent.mExtentMaxY;
  aWriteOptions.mFlipDimensionXYZ[2] = vImageExtent.mExtentMinZ > vImageExtent.mExtentMaxZ;

  tSize5D vSample(X, 1, Y, 1, Z, 1, C, 1, T, 1);
  tSize5D vImageSize = GetDataSize(aReader);

  if (aConvertOptions.mWriteMode == eWriteThumbnailOnly) {
    // Limit resolution (set downsampling factors). If the image is larger than the
    // desired size, the loaded image shouldn't be much larger after resampling. Later
    // the loaded image will be down-sampled to the desired maximum limit. If the image
    // is smaller than the desired size, to loaded image should keep its size after loading.
    bpSize vDesiredSizeX = vThumbnailSize;
    bpSize vDesiredSizeY = vThumbnailSize;
    bpSize vDesiredSizeZ = 64;

    bpSize vSizeR = aReader->GetNumberOfResolutions();
    for (bpSize vIndex = 1; vIndex < vSizeR; ++vIndex) {
      aReader->SetActiveResolutionLevel(vIndex);
      tSize5D vImageSizeR = GetDataSize(aReader);
      bool vGoodX = vImageSizeR[X] >= vImageSize[X] || vImageSizeR[X] >= vDesiredSizeX;
      bool vGoodY = vImageSizeR[Z] >= vImageSize[Y] || vImageSizeR[Y] >= vDesiredSizeY;
      bool vGoodZ = vImageSizeR[Z] >= vImageSize[Z] || vImageSizeR[Z] >= vDesiredSizeZ;
      if (vGoodX && vGoodY && vGoodZ) {
        vImageSize = vImageSizeR;
      }
      else {
        aReader->SetActiveResolutionLevel(vIndex - 1);
        break;
      }
    }

    bpSize vResampleX = vImageSize[X] / vDesiredSizeX > 1 ? vImageSize[X] / vDesiredSizeX : 1;
    bpSize vResampleY = vImageSize[Y] / vDesiredSizeY > 1 ? vImageSize[Y] / vDesiredSizeY : 1;
    bpSize vResampleZ = vImageSize[Z] / vDesiredSizeZ > 1 ? vImageSize[Z] / vDesiredSizeZ : 1;
    bpSize vResampleXY = vResampleX > vResampleY ? vResampleX : vResampleY;

    vSample[X] = vResampleXY;
    vSample[Y] = vResampleXY;
    vSample[Z] = vResampleZ;
    vSample[T] = vImageSize[T];
  }

  tDimensionSequence5D vDimensionSequence = GetDimensionSequence(aReader);
  tSize5D vBlockSize = GetDataBlockSize(aReader);

  bpUniquePtr<bpImageConverterInterface<TDataType>> vImageConverter;
  if (aConvertOptions.mWriteMode == eWriteHDF5) {
    const bpString vApplicationName = IMARISCONVERT_APPLICATION_NAME_STR;
    const bpString vApplicationVersion = IMARISCONVERT_VERSION_MAJOR_STR "." IMARISCONVERT_VERSION_MINOR_STR "." IMARISCONVERT_VERSION_PATCH_STR IMARISCONVERT_VERSION_BUILD_STR;
    bpConverterTypes::tProgressCallback vProgressCallback;
    if (aWriteOptions.mEnableLogProgress) {
      auto vConverterProgress = std::make_shared<bpConverterProgress>(1, true);
      auto vTotalBytesWritten = std::make_shared<bpUInt64>(0);
      vProgressCallback = [vConverterProgress, vTotalBytesWritten](bpFloat aProgress, bpUInt64 aTotalBytesWritten) {
        vConverterProgress->SetPosition(aProgress);
        bpUInt64 vBytesWritten = aTotalBytesWritten - *vTotalBytesWritten;
        bpThroughputMeasurementsAggregator::AddWriterMeasurement(vBytesWritten / (1024.0 * 1024));
        *vTotalBytesWritten = aTotalBytesWritten;
      };
    }
    vImageConverter.reset(new bpImageConverter<TDataType>(vDataType, vImageSize, vSample, vDimensionSequence, vBlockSize, aOutputFile, aWriteOptions, vApplicationName, vApplicationVersion, vProgressCallback));
  }
  else if (aConvertOptions.mWriteMode == eWriteThumbnailOnly) {
    vImageConverter.reset(new bpThumbnailImageConverter<TDataType>(vDataType, vImageSize, vSample, vDimensionSequence, vBlockSize, aOutputFile, aWriteOptions, aConvertOptions.mCompressThumbnail));
  }

  bpSize vNumberOfBlocks = aReader->GetNumberOfDataBlocks();
  bpSize vBufferSize = aReader->GetDataBlockNumberOfVoxels();

  std::unique_ptr<TDataType[]> vBufferPtr(new TDataType[vBufferSize]);
  TDataType* vBuffer = vBufferPtr.get();

  tSize5D vDataBlockIndex(X, 0, Y, 0, Z, 0, C, 0, T, 0);
  tSize5D vBlocksPerDimension(X, 0, Y, 0, Z, 0, C, 0, T, 0);
  bpConverterProgress vProgress(vNumberOfBlocks, false);

  for (bpSize vDimIndex = 0; vDimIndex < 5; ++vDimIndex) {
    Dimension vDim = vDimensionSequence[vDimIndex];
    vBlocksPerDimension[vDim] = Div(vImageSize[vDim], vBlockSize[vDim]);
  }

  for (bpSize vIndex = 0; vIndex < vNumberOfBlocks; vIndex++) {
    if (vImageConverter->NeedCopyBlock(vDataBlockIndex)) {
      aReader->GoToDataBlock(vIndex);
      bool vError = false;
      try {
        aReader->ReadDataBlock(vBuffer);
      }
      catch (bpException&) {
        vError = true;
      }
      if (!vError) {
        bpThroughputMeasurementsAggregator::AddReaderMeasurement(vBufferSize * sizeof(TDataType) / (1024.0 * 1024));
      }
      vImageConverter->CopyBlock(vBuffer, vDataBlockIndex);
    }
    if (aWriteOptions.mEnableLogProgress) {
      vProgress.Increment();
    }

    for (bpSize vDimIndex = 0; vDimIndex < 5; vDimIndex++) {
      Dimension vDim = vDimensionSequence[vDimIndex];
      bpSize& vBlockIndex = vDataBlockIndex[vDim];
      ++vBlockIndex;
      if (vBlockIndex == vBlocksPerDimension[vDim]) {
        vBlockIndex = 0;
      }
      else {
        break;
      }
    }
  }

  tColorInfoVector vColorInfoPerChannel;
  tTimeInfoVector vTimeInfoPerTimePoint;
  tParameters vParameters;
  GetImageMetadata(aReader, vColorInfoPerChannel, vTimeInfoPerTimePoint, vParameters);

  vImageConverter->Finish(vImageExtent, vParameters, vTimeInfoPerTimePoint, vColorInfoPerChannel, aReader->ShouldColorRangeBeAdjustedToMinMax());
}


void bpImageConvertNew::cImpl::Convert(const tReaderPtr& aReader, const bpString& aOutputFile, const cConvertOptions& aConvertOptions, const cOptions& aWriteOptions)
{
  if (aConvertOptions.mWriteMode == eWriteThumbnailOnly) {
    bpSharedPtr<bpThumbnail> vThumbnail = ExtractImarisThumbnail(aReader);
    if (vThumbnail) {
      bpWriterFileThumbnail vWriter(aOutputFile, aConvertOptions.mCompressThumbnail ? bpWriterFileThumbnail::tFormat::eJPEG : bpWriterFileThumbnail::tFormat::ePNG);
      vWriter.WriteThumbnail(*vThumbnail);
      return;
    }
  }

  if (!aReader->GetReaderImpl()) {
    throw std::runtime_error("Invalid reader for conversion");
  }

  bpVector3Float vForcedVoxelSize;
  if (aReader->GetConfig()) {
    vForcedVoxelSize = aReader->GetConfig()->GetForcedVoxelSize();
  }

  const auto& vReaderImpl = aReader->GetReaderImpl();
  auto vType = vReaderImpl->GetDataType();
  switch (vType) {
  case bpConverterTypes::bpUInt8Type:
    ConvertT<bpUInt8>(vReaderImpl, aOutputFile, aConvertOptions, aWriteOptions, vForcedVoxelSize);
    break;
  case bpConverterTypes::bpUInt16Type:
    ConvertT<bpUInt16>(vReaderImpl, aOutputFile, aConvertOptions, aWriteOptions, vForcedVoxelSize);
    break;
  case bpConverterTypes::bpUInt32Type:
    ConvertT<bpUInt32>(vReaderImpl, aOutputFile, aConvertOptions, aWriteOptions, vForcedVoxelSize);
    break;
  case bpConverterTypes::bpFloatType:
    ConvertT<bpFloat>(vReaderImpl, aOutputFile, aConvertOptions, aWriteOptions, vForcedVoxelSize);
    break;
  default:
    throw std::runtime_error("Invalid reader type");
    break;
  }
}

void bpImageConvertNew::Convert(const tReaderPtr& aReader, const bpString& aOutputFile, const cConvertOptions& aConvertOptions, const cOptions& aWriteOptions)
{
  cImpl::Convert(aReader, aOutputFile, aConvertOptions, aWriteOptions);
}
