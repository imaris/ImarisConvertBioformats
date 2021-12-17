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


#ifndef __BP_FILE_READER_IMPL_INTERFACE_TO_IMPL__
#define __BP_FILE_READER_IMPL_INTERFACE_TO_IMPL__


#include "bpFileReaderImpl.h"
#include "bpParameterSection.h"

#include "fileiobase/application/bpfFileReaderImplInterface.h"
#include "fileiobase/application/bpfException.h"

#include <list>
#include <map>
#include <exception>


class bpFileReaderImplInterfaceToImpl : public bpFileReaderImpl
{
public:
  explicit bpFileReaderImplInterfaceToImpl(const bpSharedPtr<bpfFileReaderImplInterface>& aFileReaderImplInterface)
    : mFileReaderImplInterface(aFileReaderImplInterface),
     bpFileReaderImpl(aFileReaderImplInterface->GetFileName()) {
  }

  virtual ~bpFileReaderImplInterfaceToImpl() {
  }


  const bpString& GetFileName() const {
    return mFileReaderImplInterface->GetFileName();
  }

  std::vector<bpString> GetAllFileNames() const {
    return mFileReaderImplInterface->GetAllFileNames();
  }

  virtual std::vector<bpString> GetAllFileNamesOfDataSet(const bpString& aFileName) const override {
    return mFileReaderImplInterface->GetAllFileNamesOfDataSet(aFileName);
  }

  /**
   * DataBase / multiple image Interface options
   */
  virtual bool CanMergeImages() const override {
    try {
      return mFileReaderImplInterface->CanMergeImages();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual void SetMergeImages(bool aEnable) override {
    try {
      mFileReaderImplInterface->SetMergeImages(aEnable);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bool GetMergeImages() const override {
    try {
      return mFileReaderImplInterface->GetMergeImages();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Returns the number of datasets (images / experiments)
   */
  virtual bpSize GetNumberOfDataSets() const override {
    try {
      return mFileReaderImplInterface->GetNumberOfImages();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }


  /**
   * Activate a dataset for subsequent calls
   */
  virtual void SetActiveDataSetIndex(bpSize aDataSetIndex) override {
    try {
      mFileReaderImplInterface->SetActiveImageIndex(aDataSetIndex);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Returns the index of the currently activated dataset
   */
  virtual bpSize GetActiveDataSetIndex() const override {
    try {
      return mFileReaderImplInterface->GetActiveImageIndex();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }


  /**
   * File Series Interface
   */
  virtual bool SeriesConfigurable() const override {
    try {
      return mFileReaderImplInterface->SeriesConfigurable();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bool SeriesInitialised() const override {
    try {
      return mFileReaderImplInterface->SeriesInitialised();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bool SceneConfigurable() const override {
    try {
      return mFileReaderImplInterface->SceneConfigurable();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual void SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout) override {
    try {
      std::vector<bpfFileReaderImplInterface::tFileLayout> vFileLayouts;

      for (const auto& vLayout : aLayout) {
        vFileLayouts.push_back(bpfFileReaderImplInterface::tFileLayout());

        for (const auto& vPair : vLayout) {
          bpString vFileName = vPair.first;
          bpImageIndex vFileIndex = vPair.second;
          bpfFileReaderImplInterface::ImageIndex vImageIndex;
          vImageIndex[bpfFileReaderImplInterface::X] = vFileIndex[bpFileReaderImpl::X];
          vImageIndex[bpfFileReaderImplInterface::Y] = vFileIndex[bpFileReaderImpl::Y];
          vImageIndex[bpfFileReaderImplInterface::Z] = vFileIndex[bpFileReaderImpl::Z];
          vImageIndex[bpfFileReaderImplInterface::C] = vFileIndex[bpFileReaderImpl::C];
          vImageIndex[bpfFileReaderImplInterface::T] = vFileIndex[bpFileReaderImpl::T];
          vFileLayouts.back()[vFileName] = vImageIndex;
        }
      }
      mFileReaderImplInterface->SeriesSetFileLayouts(vFileLayouts);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }


  virtual std::vector<tFileLayout> SeriesGetFileLayouts() const override {
    try {
      std::vector<bpfFileReaderImplInterface::tFileLayout> vFileLayoutsInterface = mFileReaderImplInterface->SeriesGetFileLayouts();
      std::vector<tFileLayout> vFileLayouts;

      for (const auto& vFileLayoutInterface : vFileLayoutsInterface) {
        tFileLayout vFileLayout;

        for (const auto& vPair : vFileLayoutInterface) {
          bpfString vFileName = vPair.first;
          bpfFileReaderImplInterface::ImageIndex vFileIndex = vPair.second;
          bpImageIndex vImageIndex;
          vImageIndex[bpFileReaderImpl::X] = vFileIndex[bpfFileReaderImplInterface::X];
          vImageIndex[bpFileReaderImpl::Y] = vFileIndex[bpfFileReaderImplInterface::Y];
          vImageIndex[bpFileReaderImpl::Z] = vFileIndex[bpfFileReaderImplInterface::Z];
          vImageIndex[bpFileReaderImpl::C] = vFileIndex[bpfFileReaderImplInterface::C];
          vImageIndex[bpFileReaderImpl::T] = vFileIndex[bpfFileReaderImplInterface::T];
          vFileLayout[vFileName] = vImageIndex;
        }
        vFileLayouts.push_back(vFileLayout);
      }
      return vFileLayouts;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bool SeriesFileValid(const bpString& aFileName) const override {
    try {
      return mFileReaderImplInterface->SeriesFileValid(aFileName);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpSize GetNumberOfResolutions() const override {
    try {
      return mFileReaderImplInterface->GetNumberOfResolutions();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpSize GetActiveResolutionLevel() const override {
    try {
      return mFileReaderImplInterface->GetActiveResolutionLevel();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }


  virtual void SetActiveResolutionLevel(bpSize aResolutionLevel) override {
    try {
      mFileReaderImplInterface->SetActiveResolutionLevel(aResolutionLevel);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * returns a description of the file reader (normally the static "GetDescription()")
   */
  virtual bpString GetReaderDescription() const override {
    try {
      return mFileReaderImplInterface->GetReaderDescription();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual std::vector<bpString> GetReaderExtension() const override {
    try {
      return mFileReaderImplInterface->GetReaderExtension();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Get Type to which return values of ReadData should be cast.
   *
   * @return
   */
  virtual bpNumberType GetDataType() override {
    try {
      switch (mFileReaderImplInterface->GetDataType()) {
      default:
      case bpfUInt8Type:
        return bpUInt8Type;
      case bpfUInt16Type:
        return bpUInt16Type;
      case bpfUInt32Type:
        return bpUInt32Type;
      case bpfInt8Type:
        return bpInt8Type;
      case bpfInt16Type:
        return bpInt16Type;
      case bpfInt32Type:
        return bpInt32Type;
      case bpfFloatType:
        return bpFloatType;
      }
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   *
   */
  virtual std::vector<Dimension> GetDimensionSequence() override {
    try {
      std::vector<Dimension> vDimension(5);
      std::vector<bpfFileReaderImplInterface::Dimension> vDimensionImplInterface = mFileReaderImplInterface->GetDimensionSequence();
      for (bpSize vIndex = 0; vIndex < vDimensionImplInterface.size(); ++vIndex) {
        switch (vDimensionImplInterface[vIndex]) {
        case bpfFileReaderImplInterface::X:
          vDimension[vIndex] = X;
          break;
        case bpfFileReaderImplInterface::Y:
          vDimension[vIndex] = Y;
          break;
        case bpfFileReaderImplInterface::Z:
          vDimension[vIndex] = Z;
          break;
        case bpfFileReaderImplInterface::C:
          vDimension[vIndex] = C;
          break;
        case bpfFileReaderImplInterface::T:
          vDimension[vIndex] = T;
          break;
        }
      }
      return vDimension;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpSize GetDimensionIndex(Dimension aDimension) {
    try {
      switch (aDimension) {
      case X:
        return mFileReaderImplInterface->GetDimensionIndex(bpfFileReaderImplInterface::X);
      case Y:
        return mFileReaderImplInterface->GetDimensionIndex(bpfFileReaderImplInterface::Y);
      case Z:
        return mFileReaderImplInterface->GetDimensionIndex(bpfFileReaderImplInterface::Z);
      case C:
        return mFileReaderImplInterface->GetDimensionIndex(bpfFileReaderImplInterface::C);
      case T:
        return mFileReaderImplInterface->GetDimensionIndex(bpfFileReaderImplInterface::T);
      default:
        throw std::runtime_error("invalid dimension index");
      }
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual std::vector<bpSize> GetDataSizeV() override {
    try {
      bpImageIndex vImageSize(GetDataSize());
      std::vector<Dimension> vDimensionSequence = GetDimensionSequence();
      std::vector<bpSize> vDataSizeV;
      for(std::vector<Dimension>::const_iterator vDimensionIt = vDimensionSequence.begin();
          vDimensionIt != vDimensionSequence.end(); ++vDimensionIt) {
        vDataSizeV.push_back(vImageSize[*vDimensionIt]);
      }
      return vDataSizeV;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpImageIndex GetDataSize() {
    bpfFileReaderImplInterface::ImageIndex vImageIndex = mFileReaderImplInterface->GetDataSize();
    bpImageIndex vInterfaceImageIndex;
    vInterfaceImageIndex[X] = vImageIndex[bpfFileReaderImplInterface::X];
    vInterfaceImageIndex[Y] = vImageIndex[bpfFileReaderImplInterface::Y];
    vInterfaceImageIndex[Z] = vImageIndex[bpfFileReaderImplInterface::Z];
    vInterfaceImageIndex[C] = vImageIndex[bpfFileReaderImplInterface::C];
    vInterfaceImageIndex[T] = vImageIndex[bpfFileReaderImplInterface::T];
    return vInterfaceImageIndex;
  }

  virtual bpSize GetDataSize(bpSize aDimensionOrder) {
    try {
      return GetDataSize()[GetDimensionSequence()[aDimensionOrder]];
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
  * Try to set the data block size to aDimension0, ...,
  * aDimension4. If the filereader does not "like" the new block
  * size false is returned and nothing changes.
  *
  * @param aDimension0
   * @param aDimension1
   * @param aDimension2
   * @param aDimension3
  * @param aDimension4
  * @param aResolutionLevel
   *
  * @return bool true if success
   */
  virtual bool SetDataBlockSize(bpSize aDimension0, bpSize aDimension1, bpSize aDimension2, bpSize aDimension3, bpSize aDimension4, bpSize aResolutionLevel) override {
    try {
      return mFileReaderImplInterface->SetDataBlockSize(aDimension0, aDimension1, aDimension2, aDimension3, aDimension4, aResolutionLevel);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Size of datablock as vector in data coordinates
   */
  virtual std::vector<bpSize> GetDataBlockSizeV() override {
    try {
      bpImageIndex vBlockSize(GetDataBlockSize());
      std::vector<Dimension> vDimensionSequence = GetDimensionSequence();
      std::vector<bpSize> vDataBlockSizeV;
      for(std::vector<Dimension>::const_iterator vDimensionIt = vDimensionSequence.begin();
          vDimensionIt != vDimensionSequence.end(); ++vDimensionIt) {
        vDataBlockSizeV.push_back(vBlockSize[*vDimensionIt]);
      }
      return vDataBlockSizeV;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Size of datablock as vector in data coordinates
   */
  virtual bpImageIndex GetDataBlockSize() {
    bpfFileReaderImplInterface::ImageIndex vImageIndex = mFileReaderImplInterface->GetDataBlockSize();
    bpImageIndex vInterfaceImageIndex;
    vInterfaceImageIndex[X] = vImageIndex[bpfFileReaderImplInterface::X];
    vInterfaceImageIndex[Y] = vImageIndex[bpfFileReaderImplInterface::Y];
    vInterfaceImageIndex[Z] = vImageIndex[bpfFileReaderImplInterface::Z];
    vInterfaceImageIndex[C] = vImageIndex[bpfFileReaderImplInterface::C];
    vInterfaceImageIndex[T] = vImageIndex[bpfFileReaderImplInterface::T];
    return vInterfaceImageIndex;
  }

  /**
   * Size of a given dimension of a datablock
   */
  bpSize GetDataBlockSize(bpSize aDimensionOrder) {
    try {
      return GetDataBlockSize()[GetDimensionSequence()[aDimensionOrder]];
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Total Size of datablock.
   * Returns the product of all GetDataBlockSizeV().
   */
  bpSize GetDataBlockNumberOfVoxels() {
    try {
      return mFileReaderImplInterface->GetDataBlockNumberOfVoxels();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Reads a block of data into given memory.
   *
   * Two consecutive calls to ReadDataBlock should
   * read two consecutive blocks.
   *
   */
  virtual void ReadDataBlock(void* aDataBlocknMemory) override {
    try {
      mFileReaderImplInterface->ReadDataBlock(aDataBlocknMemory);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Move to some datablock
   */
  virtual void GoToDataBlock(bpSize aBlockNumber) override {
    try {
      mFileReaderImplInterface->GoToDataBlock(aBlockNumber);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual void GoToNextDataBlock() override {
    try {
      mFileReaderImplInterface->GoToNextDataBlock();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * GetDataBlockNumber returns the number of the datablock containing
   * a given coordinate.
   *
   * The coordinate is specified as x,y,z,c,t
   */
  bpSize GetDataBlockNumber(bpSize x, bpSize y, bpSize z,
                            bpSize aChnanel,
                            bpSize aTimePoint) {
    try {
      return mFileReaderImplInterface->GetDataBlockNumber(x,y,z,aChnanel, aTimePoint);
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * GetNumberOfDataBlocks returns the total number datablocks
   */
  bpSize GetNumberOfDataBlocks() {
    try {
      return mFileReaderImplInterface->GetNumberOfDataBlocks();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Returns extents of dataset in XYZCT coordinates
   */
  virtual void GetExtents(bpVector3Float& aMin,
                          bpVector3Float& aMax) override {
    try {
      bpFloat vMinX;
      bpFloat vMinY;
      bpFloat vMinZ;
      bpFloat vMaxX;
      bpFloat vMaxY;
      bpFloat vMaxZ;
      mFileReaderImplInterface->GetExtents(vMinX, vMinY, vMinZ,
                                           vMaxX, vMaxY, vMaxZ);
      aMin[0] = vMinX;
      aMin[1] = vMinY;
      aMin[2] = vMinZ;
      aMax[0] = vMaxX;
      aMax[1] = vMaxY;
      aMax[2] = vMaxZ;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Returns extents and voxelsize of dataset in XYZ coordinates
   */
  virtual bpVector3Float GetExtentMin() const override {
    try {
      bpFloat vMinX;
      bpFloat vMinY;
      bpFloat vMinZ;
      bpFloat vMaxX;
      bpFloat vMaxY;
      bpFloat vMaxZ;
      mFileReaderImplInterface->GetExtents(vMinX, vMinY, vMinZ,
                                           vMaxX, vMaxY, vMaxZ);
      return{ vMinX, vMinY, vMinZ };
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpVector3Float GetExtentMax() const override {
    try {
      bpFloat vMinX;
      bpFloat vMinY;
      bpFloat vMinZ;
      bpFloat vMaxX;
      bpFloat vMaxY;
      bpFloat vMaxZ;
      mFileReaderImplInterface->GetExtents(vMinX, vMinY, vMinZ,
                                           vMaxX, vMaxY, vMaxZ);
      return{ vMaxX, vMaxY, vMaxZ };
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpVector3Float GetExtentDelta() const override {
    try {
      bpFloat vMinX;
      bpFloat vMinY;
      bpFloat vMinZ;
      bpFloat vMaxX;
      bpFloat vMaxY;
      bpFloat vMaxZ;
      bpSize vSizeX;
      bpSize vSizeY;
      bpSize vSizeZ;
      mFileReaderImplInterface->GetExtents(vMinX, vMinY, vMinZ,
                                           vMaxX, vMaxY, vMaxZ);
      bpFileReaderImplInterfaceToImpl* vReaderImpl = const_cast<bpFileReaderImplInterfaceToImpl*>(this);
      vSizeX = vReaderImpl->GetDataSize(vReaderImpl->GetDimension(X));
      vSizeY = vReaderImpl->GetDataSize(vReaderImpl->GetDimension(Y));
      vSizeZ = vReaderImpl->GetDataSize(vReaderImpl->GetDimension(Z));
      bpVector3Float vExtentDelta{ (vMaxX - vMinX) / vSizeX,
                                      (vMaxY - vMinY) / vSizeY,
                                      (vMaxZ - vMinZ) / vSizeZ };
      return vExtentDelta;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Read all the image parameters.
   *
   * @return The parameters from the file.
   */
  virtual bpSectionContainer ReadParameters() override {
    try {
      // read
      std::map<std::string, std::map<std::string, std::string> > vParameterContainer = mFileReaderImplInterface->ReadParameters();

      //convert
      bpSectionContainer vSectionContainer;
      for (auto& vParameter : vParameterContainer) {
        const bpString& vSectionName = vParameter.first;
        bpParameterSection* vParameterSection = new bpParameterSection(std::move(vParameter.second));
        vSectionContainer.AddSection(vSectionName, vParameterSection);
      }
      return vSectionContainer;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
  * Read the histogram at aTimePoint, aChannel and
  * aResolutionIndex. a true is returned if successful, false if
  * not or not available.
  *
  * @param aHistogram the histogram to fill
  * @param aTimePoint the timepoint to look for
  * @param aChannel the channel to look for
   *
  * @return bool true if succesful
   */
  virtual bool ReadHistogram(bpHistogram* aHistogram, bpSize aTimePoint, bpSize aChannel) override {
    try {
      bpFloat vMin;
      bpFloat vMax;
      std::vector<bpUInt64> vBins;
      bool vHistOK = mFileReaderImplInterface->ReadHistogram(aTimePoint, aChannel, vBins, vMin, vMax);
      *aHistogram = bpHistogram(vMin, vMax, vBins);
      return vHistOK;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  /**
   * Reads the thumnbnail image. Ith there is no thumbnail stored
   * in the file, a null pointer is returned.
   */
  virtual bpSharedPtr<bpThumbnail> ReadThumbnail() override {
    try {
      bpSharedPtr<bpThumbnail> vThumbnail;
      std::vector<bpfFileReaderImplInterface::cColor> vThumbnailPixels;
      bpSize vSizeX = 0;
      bpSize vSizeY = 0;
      bool vHasThumbnail = mFileReaderImplInterface->ReadThumbnail(vThumbnailPixels, vSizeX, vSizeY);
      if (vHasThumbnail && vSizeX > 0 && vSizeY > 0) {
        bpSize vCount = vSizeX * vSizeY;
        std::vector<bpUInt8> vBuffer(vCount * 4);
        for (bpSize vIndex = 0; vIndex < vCount; ++vIndex) {
          const bpfFileReaderImplInterface::cColor& vPixel = vThumbnailPixels[vIndex];
          vBuffer[vIndex * 4 + 0] = static_cast<bpUInt8>(vPixel.mRed * 255);
          vBuffer[vIndex * 4 + 1] = static_cast<bpUInt8>(vPixel.mGreen * 255);
          vBuffer[vIndex * 4 + 2] = static_cast<bpUInt8>(vPixel.mBlue * 255);
          vBuffer[vIndex * 4 + 3] = static_cast<bpUInt8>(vPixel.mAlpha * 255);
        }
        vThumbnail = std::make_shared<bpThumbnail>(vSizeX, vSizeY, std::move(vBuffer));
      }
      return vThumbnail;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual std::vector<bpTimeInfo> ReadTimeInfo() override {
    try {
      std::vector<bpTimeInfo> vTimeInfo;
      bpSize vSizeT = GetDataSize(GetDimension(T));
      for (bpUInt32 vTimeIndex = 0; vTimeIndex < vSizeT; ++vTimeIndex) {
        vTimeInfo.push_back(ReadTimeInfo(vTimeIndex));
      }
      return vTimeInfo;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpTimeInfo ReadTimeInfo(bpSize aTimePoint) override {
    try {
      bpInt32 vJulianDay;
      bpInt64 vNanosecondsOfDay;
      try {
        mFileReaderImplInterface->ReadTimeInfo(aTimePoint, vJulianDay, vNanosecondsOfDay);
      }
      catch (bpfException& vException) {
        throw Error(vException);
      }
      bpTimeInfo vTimeInfo;
      vTimeInfo.SetDate(vJulianDay);
      vTimeInfo.SetTimeNanoSeconds(vNanosecondsOfDay);
      return vTimeInfo;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual std::vector<bpColorInfo> ReadColorInfo() override {
    try {
      std::vector<bpColorInfo> vColorInfo;
      for (bpUInt32 vChannelIndex = 0; vChannelIndex < GetDataSize(GetDimension(C)); ++vChannelIndex) {
        vColorInfo.push_back(ReadColorInfo(vChannelIndex));
      }
      return vColorInfo;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bpColorInfo ReadColorInfo(bpSize aChannelIndex) override {
    try {
      bool vMode;
      bpfFileReaderImplInterface::cColor vBaseColorI;
      std::vector<bpfFileReaderImplInterface::cColor> vColorTableI;
      float vOpacity;
      float vRangeMin;
      float vRangeMax;
      float vGamma;
      mFileReaderImplInterface->ReadColorInfo(aChannelIndex,
                                              vMode,
                                              vBaseColorI,
                                              vColorTableI,
                                              vOpacity,
                                              vRangeMin,
                                              vRangeMax,
                                              vGamma);

      // create bpColorInfo
      std::vector<bpColor> vColorTable;
      for (const auto& vColor : vColorTableI) {
        vColorTable.emplace_back(vColor.mRed, vColor.mGreen, vColor.mBlue);
      }
      bpColorInfo vColorInfo;
      if (vMode) {
        vColorInfo.SetTable(vColorTable);
        vColorInfo.SetBaseColor(bpColor(vBaseColorI.mRed, vBaseColorI.mGreen, vBaseColorI.mBlue));
      }
      else {
        vColorInfo.SetBaseColor(bpColor(vBaseColorI.mRed, vBaseColorI.mGreen, vBaseColorI.mBlue));
        vColorInfo.SetTable(vColorTable);
      }
      vColorInfo.SetRange(vRangeMin, vRangeMax);
      vColorInfo.SetGammaCorrectionEquivalentRange(vGamma);
      vColorInfo.SetOpacity(vOpacity);
      return vColorInfo;
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

  virtual bool ShouldColorRangeBeAdjustedToMinMax() override {
    try {
      return mFileReaderImplInterface->ShouldColorRangeBeAdjustedToMinMax();
    }
    catch (bpfException& vException) {
      throw Error(vException);
    }
  }

private:
  static std::runtime_error Error(bpfException& aException) {
    return std::runtime_error(("bpFileIOException - " + aException.GetDescription()).c_str());
  }

  bpSharedPtr<bpfFileReaderImplInterface> mFileReaderImplInterface;
};


#endif
