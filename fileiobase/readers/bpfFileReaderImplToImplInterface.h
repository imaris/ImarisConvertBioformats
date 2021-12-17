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


#ifndef __BP_FILE_READER_IMPL_BRIDGE__
#define __BP_FILE_READER_IMPL_BRIDGE__


#include "fileiobase/readers/bpfFileReaderImpl.h"
#include "fileiobase/types/bpfParameterSection.h"
#include "fileiobase/types/bpfHistogram.h"
#include "fileiobase/types/bpfHistogramBlocked.h"
#include "fileiobase/exceptions/bpfFileFormatException.h"
#include "fileiobase/application/bpfFileReaderImplInterface.h"


class bpfHistogram;
class bpfPackedRGBA;
template <class Element> class bpfImageField;

class bpfFileReaderImplToImplInterface : public bpfFileReaderImplInterface
{

private:
  // must have aFileReaderImpl for construction  ==> default constructor private
  bpfFileReaderImplToImplInterface()
    : mFileReaderImpl(nullptr) {
  }
public:

  bpfFileReaderImplToImplInterface(bpfFileReaderImpl* aFileReaderImpl)
    : mFileReaderImpl(aFileReaderImpl) {
    if (aFileReaderImpl == nullptr) {
      throw bpfFileFormatException();
    }
  }

  virtual ~bpfFileReaderImplToImplInterface() {
    delete mFileReaderImpl;
  }

  virtual const bpfString& GetFileName() const override {
    return mFileReaderImpl->GetFileName();
  }

  virtual const std::vector<bpfString> GetAllFileNames() const override {
    return mFileReaderImpl->GetAllFileNames();
  }

  virtual std::vector<bpfString> GetAllFileNamesOfDataSet(const bpfString& aFileName) const override {
    return mFileReaderImpl->GetAllFileNamesOfDataSet(aFileName);
  }

  /**
   * DataBase / multiple image Interface options
   */
  virtual bool CanMergeImages() const override {
    return mFileReaderImpl->CanMergeImages();
  }

  virtual void SetMergeImages(bool aEnable) override {
    mFileReaderImpl->SetMergeImages(aEnable);
  }

  virtual bool GetMergeImages() const override {
    return mFileReaderImpl->GetMergeImages();
  }

  /**
   * Returns the number of datasets (images / experiments)
   */
  virtual bpfSize GetNumberOfImages() const override {
    return mFileReaderImpl->GetNumberOfDataSets();
  }

  virtual bpfString GetImageDescription(size_t /* aImageIndex */) const {
    return "";
  }

  /**
   * Activate a dataset for subsequent calls
   */
  virtual void SetActiveImageIndex(bpfSize aDataSetIndex) override {
    mFileReaderImpl->SetActiveDataSetIndex(aDataSetIndex);
  }

  /**
   * Returns the index of the currently activated dataset
   */
  virtual bpfSize GetActiveImageIndex() const override {
    return mFileReaderImpl->GetActiveDataSetIndex();
  }

  virtual bool SeriesConfigurable() const override {
    return mFileReaderImpl->SeriesConfigurable();
  }

  virtual bool SceneConfigurable() const override {
    return mFileReaderImpl->SceneConfigurable();
  }

  virtual bool SeriesInitialised() const override {
    return mFileReaderImpl->SeriesInitialised();
  }

  virtual void SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout) override {
    std::vector<bpfFileReaderImpl::tFileLayout> vFileLayouts;

    auto vLayoutIt = aLayout.begin();
    auto vLayoutEnd = aLayout.end();
    for (; vLayoutIt != vLayoutEnd; ++vLayoutIt) {
      vFileLayouts.push_back(bpfFileReaderImpl::tFileLayout());
      for (tFileLayout::const_iterator vFileLayoutIt = vLayoutIt->begin();
      vFileLayoutIt != vLayoutIt->end(); ++vFileLayoutIt) {
        bpfString vFileName = (*vFileLayoutIt).first;
        ImageIndex vFileIndex = (*vFileLayoutIt).second;
        bpfFileReaderImpl::ImageIndex vImageIndex;
        vImageIndex[bpfFileReaderImpl::X] = vFileIndex[bpfFileReaderImplInterface::X];
        vImageIndex[bpfFileReaderImpl::Y] = vFileIndex[bpfFileReaderImplInterface::Y];
        vImageIndex[bpfFileReaderImpl::Z] = vFileIndex[bpfFileReaderImplInterface::Z];
        vImageIndex[bpfFileReaderImpl::C] = vFileIndex[bpfFileReaderImplInterface::C];
        vImageIndex[bpfFileReaderImpl::T] = vFileIndex[bpfFileReaderImplInterface::T];
        //vImageIndex[bpfFileReaderImpl::F] = vFileIndex[bpfFileReaderImplInterface::F];
        vFileLayouts.back()[vFileName] = vImageIndex;
      }
    }
    mFileReaderImpl->SeriesSetFileLayouts(vFileLayouts);
  }

  virtual std::vector<tFileLayout> SeriesGetFileLayouts() const override {
    std::vector<bpfFileReaderImpl::tFileLayout> vFileLayouts = mFileReaderImpl->SeriesGetFileLayouts();
    std::vector<tFileLayout> vInterfaceFileLayouts;
    for (auto vFileLayoutsIt = vFileLayouts.begin(); vFileLayoutsIt != vFileLayouts.end(); ++vFileLayoutsIt) {
      tFileLayout vInterfaceFileLayout;
      for (bpfFileReaderImpl::tFileLayout::const_iterator vFileLayoutIt = vFileLayoutsIt->begin();
      vFileLayoutIt != vFileLayoutsIt->end(); ++vFileLayoutIt) {
        bpfString vFileName = (*vFileLayoutIt).first;
        bpfFileReaderImpl::ImageIndex vFileIndex = (*vFileLayoutIt).second;
        ImageIndex vImageIndex;
        vImageIndex[bpfFileReaderImplInterface::X] = vFileIndex[bpfFileReaderImpl::X];
        vImageIndex[bpfFileReaderImplInterface::Y] = vFileIndex[bpfFileReaderImpl::Y];
        vImageIndex[bpfFileReaderImplInterface::Z] = vFileIndex[bpfFileReaderImpl::Z];
        vImageIndex[bpfFileReaderImplInterface::C] = vFileIndex[bpfFileReaderImpl::C];
        vImageIndex[bpfFileReaderImplInterface::T] = vFileIndex[bpfFileReaderImpl::T];
        vInterfaceFileLayout[vFileName] = vImageIndex;
      }
      vInterfaceFileLayouts.push_back(vInterfaceFileLayout);
    }
    return vInterfaceFileLayouts;
  }

  virtual bool SeriesFileValid(const bpfString& aFileName) const override {
    return mFileReaderImpl->SeriesFileValid(aFileName);
  }

  virtual bpfSize GetNumberOfResolutions() const override {
    return mFileReaderImpl->GetNumberOfResolutions();
  }

  virtual bpfSize GetActiveResolutionLevel() const override {
    return mFileReaderImpl->GetActiveResolutionLevel();
  }

  virtual void SetActiveResolutionLevel(bpfSize aResolutionLevel) override {
    return mFileReaderImpl->SetActiveResolutionLevel(aResolutionLevel);
  }

  virtual bpfString GetReaderDescription() override {
    if (mFileReaderImpl != nullptr) {
      return mFileReaderImpl->GetReaderDescription();
    }
    return "";
  }

  virtual std::vector<bpfString> GetReaderExtension() override {
    if (mFileReaderImpl != nullptr) {
      return mFileReaderImpl->GetReaderExtension();
    }
    return {};
  }

  virtual bpfNumberType GetDataType() override {
    return mFileReaderImpl->GetDataType();
  }


  virtual std::vector<Dimension> GetDimensionSequence() override {
    std::vector<Dimension> vDimension(5);
    std::vector<bpfFileReaderImpl::Dimension> vDimensionImpl = mFileReaderImpl->GetDimensionSequence();
    for (bpfSize vIndex = 0; vIndex < vDimensionImpl.size(); ++vIndex) {
      switch(vDimensionImpl[vIndex]) {
      case bpfFileReaderImpl::X:
        vDimension[vIndex] = X;
        break;
      case bpfFileReaderImpl::Y:
        vDimension[vIndex] = Y;
        break;
      case bpfFileReaderImpl::Z:
        vDimension[vIndex] = Z;
        break;
      case bpfFileReaderImpl::C:
        vDimension[vIndex] = C;
        break;
      case bpfFileReaderImpl::T:
        vDimension[vIndex] = T;
        break;
      }
    }
    return vDimension;
  }

  virtual bpfSize GetDimensionIndex(Dimension aDimension) override {
    switch (aDimension) {
    case X:
      return mFileReaderImpl->GetDimensionIndex(bpfFileReaderImpl::X);
    case Y:
      return mFileReaderImpl->GetDimensionIndex(bpfFileReaderImpl::Y);
    case Z:
      return mFileReaderImpl->GetDimensionIndex(bpfFileReaderImpl::Z);
    case C:
      return mFileReaderImpl->GetDimensionIndex(bpfFileReaderImpl::C);
    case T:
      return mFileReaderImpl->GetDimensionIndex(bpfFileReaderImpl::T);
    }
    return static_cast<bpfSize>(-1);
  }


  virtual ImageIndex GetDataSize() override {
    bpfFileReaderImpl::ImageIndex vImageIndex = mFileReaderImpl->GetDataSize();
    ImageIndex vInterfaceImageIndex;
    vInterfaceImageIndex[X] = vImageIndex[bpfFileReaderImpl::X];
    vInterfaceImageIndex[Y] = vImageIndex[bpfFileReaderImpl::Y];
    vInterfaceImageIndex[Z] = vImageIndex[bpfFileReaderImpl::Z];
    vInterfaceImageIndex[C] = vImageIndex[bpfFileReaderImpl::C];
    vInterfaceImageIndex[T] = vImageIndex[bpfFileReaderImpl::T];
    return vInterfaceImageIndex;
  }

  virtual bpfSize GetDataSize(Dimension aDimension) override {
    return mFileReaderImpl->GetDataSize(aDimension);
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
  virtual bool SetDataBlockSize(bpfSize aDimension0,
                                bpfSize aDimension1,
                                bpfSize aDimension2,
                                bpfSize aDimension3,
                                bpfSize aDimension4,
                                bpfSize aResolutionLevel) override {
    return mFileReaderImpl->SetDataBlockSize(aDimension0,aDimension1,aDimension2,aDimension3,aDimension4,aResolutionLevel);
  }



  virtual ImageIndex GetDataBlockSize() override {
    bpfFileReaderImpl::ImageIndex vImageIndex = mFileReaderImpl->GetDataBlockSize();
    ImageIndex vInterfaceImageIndex;
    vInterfaceImageIndex[X] = vImageIndex[bpfFileReaderImpl::X];
    vInterfaceImageIndex[Y] = vImageIndex[bpfFileReaderImpl::Y];
    vInterfaceImageIndex[Z] = vImageIndex[bpfFileReaderImpl::Z];
    vInterfaceImageIndex[C] = vImageIndex[bpfFileReaderImpl::C];
    vInterfaceImageIndex[T] = vImageIndex[bpfFileReaderImpl::T];
    return vInterfaceImageIndex;
  }

  /**
   * Size of a given dimension of a datablock
   */
  virtual bpfSize GetDataBlockSize(Dimension aDimension) override {
    return mFileReaderImpl->GetDataBlockSize(aDimension);
  }


  /**
   * Total Size of datablock.
   * Returns the product of all GetDataBlockSize().
   */
  virtual bpfSize GetDataBlockNumberOfVoxels() override {
    return mFileReaderImpl->GetDataBlockNumberOfVoxels();
  }


  /**
   * Reads a block of data into given memory.
   *
   * Two consecutive calls to ReadDataBlock should
   * read two consecutive blocks.
   *
   */
  virtual void ReadDataBlock(void* aDataBlocknMemory) override {
    mFileReaderImpl->ReadDataBlock(aDataBlocknMemory);
  }


  /**
   * Move to some datablock
   */
  virtual void GoToDataBlock(bpfSize aBlockNumber) override {
    mFileReaderImpl->GoToDataBlock(aBlockNumber);
  }

  virtual void GoToNextDataBlock() override {
    mFileReaderImpl->GoToNextDataBlock();
  }


  /**
   * GetDataBlockNumber returns the number of the datablock containing
   * a given coordinate.
   *
   * The coordinate is specified as x,y,z,c,t
   */
  virtual bpfSize GetDataBlockNumber(bpfSize x, bpfSize y, bpfSize z,
                                     bpfSize aChnanel,
                                     bpfSize aTimePoint) override {
    return mFileReaderImpl->GetDataBlockNumber(x,y,z,aChnanel,aTimePoint);
  }


  /**
   * GetNumberOfDataBlocks returns the total number datablocks
   */
  virtual bpfSize GetNumberOfDataBlocks() override {
    return mFileReaderImpl->GetNumberOfDataBlocks();
  }


  /**
   * Returns extents of dataset in XYZCT coordinates
   */
  virtual void GetExtents(bpfFloat& aMinX, bpfFloat& aMinY, bpfFloat& aMinZ,
                          bpfFloat& aMaxX, bpfFloat& aMaxY, bpfFloat& aMaxZ) override {
    bpfVector3Float vMin;
    bpfVector3Float vMax;
    mFileReaderImpl->GetExtents(vMin, vMax);
    aMinX = vMin[0];
    aMinY = vMin[1];
    aMinZ = vMin[2];
    aMaxX = vMax[0];
    aMaxY = vMax[1];
    aMaxZ = vMax[2];
  }



  /**
   * Read all the image parameters.
   *
   * @return The parameters from the file.
   */
  virtual std::map<std::string, std::map<std::string, std::string> > ReadParameters() override {
    std::map<std::string, std::map<std::string, std::string>> vParameterContainer;
    bpfSectionContainer vParameters = mFileReaderImpl->ReadParameters();
    bpfSectionContainer::Iterator vIt = vParameters.Begin();
    bpfSectionContainer::Iterator vEnd = vParameters.End();
    for (; vIt != vEnd; ++vIt) {
      vParameterContainer.emplace_hint(vParameterContainer.end(), vIt->first, std::move(*vIt->second));
    }
    return vParameterContainer;
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
  virtual bool ReadHistogram(bpfSize aTimePoint, bpfSize aChannel,
                             std::vector<bpfUInt64>& aBins,
                             float& aMin, float& aMax) override {
    bpfSharedPtr<bpfHistogram> vHistogram;
    switch (mFileReaderImpl->GetDataType()) {
    case bpfUInt8Type:
      vHistogram = bpfMakeSharedPtr<bpfHistogramBlocked<bpfUInt8> >();
      break;
    case bpfUInt16Type:
      vHistogram = bpfMakeSharedPtr<bpfHistogramBlocked<bpfUInt16> >();
      break;
    case bpfUInt32Type:
      vHistogram = bpfMakeSharedPtr<bpfHistogramBlocked<bpfUInt32> >();
      break;
    case bpfFloatType:
      vHistogram = bpfMakeSharedPtr<bpfHistogramBlocked<bpfFloat> >();
      break;
    default:
      // throw bpfException("bpfFileReaderImplToImplInterface::ReadHistogram(...) - Data type \"" + bpfToString(mFileReaderImpl->GetDataType()) + "\" not supportet");
      break;
    }
    if (!vHistogram || !mFileReaderImpl->ReadHistogram(vHistogram.get(), aTimePoint, aChannel)) {
      return false;
    }
    aMin = vHistogram->GetValueMinF();
    aMax = vHistogram->GetValueMaxF();
    bpfSize vCount = vHistogram->GetNumberOfBins();
    aBins.resize(vCount);
    for (bpfSize vBinIndex = 0; vBinIndex < vCount; ++vBinIndex) {
      aBins[vBinIndex] = vHistogram->GetCount(vBinIndex);
    }
    return true;
  }



  /**
   * Reads the thumnbnail image. Ith there is no thumbnail stored
   * in the file, a null pointer is returned.
   */
  virtual bool ReadThumbnail(std::vector<cColor>& aThumbnail, bpfSize& aSizeX, bpfSize& aSizeY) override {
    std::vector<bpfPackedRGBA> vThumbnail;
    bool vHasThumbnail = mFileReaderImpl->ReadThumbnail(vThumbnail, aSizeX, aSizeY);
    if (vHasThumbnail) {
      aThumbnail.resize(vThumbnail.size());
      for (bpfSize vIndex = 0; vIndex < vThumbnail.size(); ++vIndex) {
        bpfPackedRGBA& vPackedRGBA = vThumbnail[vIndex];
        cColor& vColor = aThumbnail[vIndex];
        vColor.mRed = vPackedRGBA.r / 255.0f;
        vColor.mGreen = vPackedRGBA.g / 255.0f;
        vColor.mBlue = vPackedRGBA.b / 255.0f;
        vColor.mAlpha = vPackedRGBA.a / 255.0f;
      }
      return true;
    }
    return false;
  }


  virtual void ReadTimeInfo(bpfSize aTimePoint,
                            bpfInt32& aJulianDay,
                            bpfInt64& aNanosecondsOfDay) override {
    bpfTimeInfo vTimeInfo = mFileReaderImpl->ReadTimeInfo(aTimePoint);
    aJulianDay = vTimeInfo.GetJulianDay();
    aNanosecondsOfDay = vTimeInfo.GetNanoseconds();
  }


  virtual void ReadColorInfo(bpfSize aChannelIndex,
                             bool& aMode,
                             cColor& aBaseColor,
                             std::vector<cColor>& aColorTable,
                             float& aOpacity,
                             float& aRangeMin,
                             float& aRangeMax,
                             float& aGammaCorrection) override {
    bpfColorInfo vColorInfo = mFileReaderImpl->ReadColorInfo(aChannelIndex);

    aMode = vColorInfo.GetColorMode() == bpfColorInfo::eBaseColor;
    bpfColor vBaseColor = vColorInfo.GetBaseColor();
    aBaseColor.mRed = vBaseColor.Red();
    aBaseColor.mGreen = vBaseColor.Green();
    aBaseColor.mBlue = vBaseColor.Blue();
    aOpacity = vColorInfo.GetOpacity();
    aRangeMin = vColorInfo.GetRangeMin();
    aRangeMax = vColorInfo.GetRangeMax();
    aGammaCorrection = vColorInfo.GetGammaCorrectionEquivalentRange();
    aColorTable.resize(vColorInfo.GetTableLength());
    for (bpfSize vIndex = 0; vIndex < vColorInfo.GetTableLength(); ++vIndex) {
      bpfColor vColor = vColorInfo.GetTableEntry(static_cast<bpfUInt32>(vIndex));
      cColor vColorI;
      vColorI.mRed = vColor.Red();
      vColorI.mGreen = vColor.Green();
      vColorI.mBlue = vColor.Blue();
      vColorI.mAlpha = 1.0;
      aColorTable[vIndex] = vColorI;
    }
  }


  virtual bool ShouldColorRangeBeAdjustedToMinMax() override {
    return mFileReaderImpl->ShouldColorRangeBeAdjustedToMinMax();
  }


private:

  bpfFileReaderImpl* mFileReaderImpl;
};

#endif

