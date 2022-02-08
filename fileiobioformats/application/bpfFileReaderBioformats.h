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


#ifndef __BP_FILE_READER_BIOFORMATS__
#define __BP_FILE_READER_BIOFORMATS__

#include "fileiobioformats/application/bpFileIOBioformatsDllAPI.h"
#include "fileiobase/readers/bpfFileReaderImpl.h"
#include "fileiobase/types/bpfParameterSection.h"
#include "fileiobase/types/bpfSmartPtr.h"
#include "fileiobase/utils/bpfFileTools.h"

#include "fileiobioformats/java/bpfJNI.h"

#include <string>
#include <vector>
#include <map>


class BP_FILEIOBIOFORMATS_DLL_API bpfFileReaderBioformats : public bpfFileReaderImpl
{

public:

  bpfFileReaderBioformats(const bpfString& aFilename);
  ~bpfFileReaderBioformats();


  // methods from interface

  std::vector<bpfString> GetAllFileNames() const override;
  std::vector<bpfString> GetAllFileNamesOfDataSet(const bpfString& aFileName) const override;

  bpfSize GetNumberOfDataSets() const override;
  void SetActiveDataSetIndex(bpfSize aDataSetIndex) override;
  bpfSize GetActiveDataSetIndex() const override;

  bpfSize GetNumberOfResolutions() const override;
  bpfSize GetActiveResolutionLevel() const override;
  void SetActiveResolutionLevel(bpfSize aResolutionLevel) override;

  bpfString GetReaderDescription() const override;
  std::vector<bpfString> GetReaderExtension() const override;

  bpfNumberType GetDataType() override;

  std::vector<Dimension> GetDimensionSequence() override;

  std::vector<bpfSize> GetDataSizeV() override;
  std::vector<bpfSize> GetDataBlockSizeV() override;

  void ReadDataBlock(void* aDataBlockMemory) override;
  void GoToDataBlock(bpfSize aBlockNumber) override;
  void GoToNextDataBlock() override;

  void GetExtents(bpfVector3Float& aMin, bpfVector3Float& aMax) override;

  bool ReadThumbnail(std::vector<bpfPackedRGBA>& aThumbnailPixels, bpfSize& aSizeX, bpfSize& aSizeY) override;

  bpfColorInfo ReadColorInfo(bpfSize aChannel) override;

  bool ShouldColorRangeBeAdjustedToMinMax() override;


protected:
  bpfSectionContainer ReadParametersImpl() override;


private:
  void InitializeFileReader();
  void TryOpenAsSeries(JNIEnv* aEnv);
  void HandleMetadataOptions(JNIEnv* aEnv);

  bpfString ConvertString(jstring aString, JNIEnv* aEnv) const;
  std::vector<bpfString> ConvertArrayOfStrings(jobjectArray aArrayOfStrings, JNIEnv* aEnv) const;

  bpfFloat GetFloatFromLength(jobject aLength, JNIEnv* aEnv);
  
  bpfSize GetSizeOfDimension(const bpfString& aMethodName, JNIEnv* aEnv);
  bpfSize GetEffectiveSizeC(JNIEnv* aEnv);

  std::vector<bpfFloat> GetPixelScales(JNIEnv* aEnv);
  bpfFloat GetPixelsPhysicalSize(const bpfString& aMethodName, JNIEnv* aEnv);
  bpfString GetPixelsPhysicalSizeString(const bpfString& aMethodName, JNIEnv* aEnv);

  bpfNumberType ConvertPixelType(bpfString aPixelType, JNIEnv* aEnv);
  bpfString GetPixelTypeString(JNIEnv* aEnv);

  using tOptionalColor = bpfUniquePtr<bpfColor>;
  tOptionalColor GetChannelColor(bpfSize aChannel, JNIEnv* aEnv);
  bpfColor ConvertColor(jobject aJColor, JNIEnv* aEnv);
  std::vector<bpfColor> GetColorTable8Bit(JNIEnv* aEnv);
  std::vector<bpfColor> GetColorTable16Bit(JNIEnv* aEnv);
  bpfSize GetRGBChannelCount(JNIEnv* aEnv);
  bool IsInterleaved(JNIEnv* aEnv);

  void MapCoreMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv);
  void MapOriginalMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv);
  void MapSeriesMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv);
  void MapMetadataParameters(jobject aMetadata, bpfParameterSection* aParameterSection, JNIEnv* aEnv);

  void LockImageReaderObject(JNIEnv* aEnv) const;
  void UnlockImageReaderObject(JNIEnv* aEnv) const;
  void LockMetadataObject(JNIEnv* aEnv) const;
  void UnlockMetadataObject(JNIEnv* aEnv) const;


  bpfString mFileName;
  bpfSize mBlockNumber;
  
  jclass mImageReaderClass;
  jobject mImageReaderObject;
  jclass mMetadataClass;
  jobject mMetadataObject;
};

#endif