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


#ifndef __BP_CONVERTER__
#define __BP_CONVERTER__


#include "../src/bpThroughputMeasurementsFetcher.h"

#include "../meta/bpFileSeriesDelimiters.h"

class bpParameterSection;
class bpFileReaderFactory;
class bpFileReader;

#define IMARIS_CONVERT_EXIT_SUCCESS              0
#define IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS    2
#define IMARIS_CONVERT_EXIT_UKNOWN_FILE          3
#define IMARIS_CONVERT_EXIT_ABNORMAL_TERMINATION 4
#define IMARIS_CONVERT_EXIT_TIMEOUT              99

/**
 * Performs the conversion of an image file into meta-data, voxel-hash,
 * thumbnails, or other image formats.
 */
class bpConverter
{
public:
  explicit bpConverter(bpSharedPtr<bpFileReaderFactory> aFileReaderFactory);

  // configuration parameters
  void SetInputFileName(const bpString& aInputFileName, const bpString& aArgumentName);
  void SetInputList(bool aInputList);
  void SetAdditionalInputFileNames(const std::vector<bpString>& aInputFileNames);
  void SetInputFileFormat(const bpString& aInputFileFormat, const bpString& aArgumentName);
  void SetInputFileImageIndex(bpSize aInputFileImageIndex);
  void SetInputFileCropSizes(const std::vector<bpSize>& aCropSizes);
  void SetInputVoxelSize(const bpVector3Float& aInputVoxelSize);
  void SetInputVoxelSizeX(bpFloat aX);
  void SetInputVoxelSizeY(bpFloat aY);
  void SetInputVoxelSizeZ(bpFloat aZ);
  void SetInputSeriesLayoutFileName(const bpString& aInputFileName, const bpString& aArgumentName);
  void SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension aDimension, const bpString& aDelimiters);
  void SetOutputFileName(const bpString& aOutputFileName, const bpString& aArgumentName);
  void AddThumbnail(const bpString& aThumbnailFileName, const bpParameterSection* aSectionDefaultParameters);
  void SetThumbnailBackground(const bpString& aThumbnailBackground, const bpString& aArgumentName);
  void SetThumbnailMode(const bpString& aThumbnailMode, const bpString& aArgumentName);
  void SetThumbnailImageSize(bpSize aThumbnailImageSize);
  void SetThumbnailTimeIndex(bpInt32 aThumbnailTimeIndex);
  void SetThumbnailSliceIndexZ(bpInt32 aSliceIndexZ);
  void SetThumbnailMaxSizeXY(bpSize aThumbnailMaxSizeXY);
  void SetThumbnailOutputFormat(const bpString& aOutputFormat);
  void SetThroughputOutputInterval(bpUInt32 aTimeIntervalInMs);
  void SetAllFilesFileName(const bpString& aMetaDataFileName, const bpString& aArgumentName);
  void SetMetaDataFileName(const bpString& aMetaDataFileName, const bpString& aArgumentName);
  void SetMetaDataForArenaFileName(const bpString& aMetaDataFileName, const bpString& aArgumentName);
  void SetVoxelHashFileName(const bpString& aVoxelHashFileName, const bpString& aArgumentName);
  void SetVoxelHashBlockSort(const bpString& aVoxelHashBlockSort, const bpString& aArgumentName);
  void SetNumberOfThreads(bpSize aNumberOfThreads);
  void SetCompressionAlgorithmType(bpConverterTypes::tCompressionAlgorithmType aCompressionAlgorithmType);
  void SetImageDescriptorsFileName(const bpString& aImageDescriptorsFileName, const bpString& aArgumentName);
  void SetLogFile(const bpString& aLogFile, const bpString& aArgumentName);
  void SetEnableLogProgress(bool aEnableLogProgress);
  void SetPrintSupportedFormats(bool aEnable);

  void SetDoMetaDataCalculation(bool aFlag);
  void SetDoMetaDataForArenaCalculation(bool aFlag);
  void SetDoVoxelHashCalculated(bool aFlag);
  void SetDoImageDescriptorsCalculation(bool aFlag);
  void SetDoAllFilesNamesCalculation(bool aFlag);

  // check consistency of parameters
  void CheckParameters() const;

  // perform the conversion
  bool Execute();

private:
  const bpSharedPtr<bpFileReaderFactory>& GetFileReaderFactory() const;
  void PrintSupportedFormats();

  class cThumbnailSettings {
  public:
    bpString mFileName;
    bpString mBackground;
    bpSize mImageSize;
    bpString mMode;
    bpInt32 mTimeIndex;
    bpInt32 mSliceIndexZ;
    bpSize mMaxSizeXY;
    bpFloat mHistogramStretch;
    bpString mOutputFormat;
  };

  // helpers
  cThumbnailSettings& GetThumbnailSettings();
  void SetParameterOnce(bpString& aParameter, const bpString& aValue, const bpString& aArgumentName);
  bpSharedPtr<bpFileReader> CreateFileReader(const bpString& aInputFileName, const bpString& aInputFileFormat, bpSize aInputFileImageIndex, bpString aXMLLayout = "") const;
  bpUInt64 GetValueFromHexString(const bpString& aValue) const;
  bpString ReadXMLLayoutFromFile() const;

  // workers
  bool ConvertFile(bpSharedPtr<bpFileReader> aFileReader);
  bool CreateThumbnails(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CreateAllFiles(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CreateMetaData(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CreateMetaDataForArena(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CreateVoxelHash(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CreateImageDescriptors(bpSharedPtr<bpFileReader> aFileReader);
  bool ConfigureSeriesReader(bpSharedPtr<bpFileReader> aFileReader) const;
  bool CheckIfVoxelSizeIsKnown(bpSharedPtr<bpFileReader> aFileReader) const;

  // members
  bpString mInputFileName;
  bool mInputList;
  std::vector<bpString> mAdditionalInputFileNames;
  std::set<bpString> mProcessedInputFileNames;
  bpString mInputFileFormat;
  bpSize mInputFileImageIndex;
  std::vector<bpSize> mInputFileCropSizes;
  bpVector3Float mInputVoxelSize;
  bpString mInputSeriesLayoutFileName;
  bpString mOutputFileName;
  bpString mOutputFileFormat;
  bpString mAllFilesFileName;
  bpString mMetaDataFileName;
  bpString mMetaDataForArenaFileName;
  bpString mVoxelHashFileName;
  bpString mVoxelHashBlockSort;
  bpString mImageDescriptorsFileName;
  std::vector<cThumbnailSettings> mThumbnailSettings;
  bpString mLogFile;
  bpSize mNumberOfThreads;
  bpConverterTypes::tCompressionAlgorithmType mCompressionAlgorithmType;

  bpThroughputMeasurementsFetcher mMeasurementFetcherThread;

  bool mDoMetaDataCalculation;
  bool mDoMetaDataForArenaCalculation;
  bool mDoVoxelHashCalculation;
  bool mDoImageDescriptorsCalculation;
  bool mDoAllFileNamesCalculcation;
  bool mEnableLogProgress;
  bool mPrintSupportedFormats;

  bpFloat mThroughputOutputInterval;

  bpSharedPtr<bpFileReaderFactory> mFileReaderFactory;

  bpFileSeriesDelimiters mFileSeriesDelimiters;
};

#endif // __BP_CONVERTER__

