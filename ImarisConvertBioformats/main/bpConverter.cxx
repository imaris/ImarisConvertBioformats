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


#include "bpConverter.h"
#include "bpLogger.h"
#include "bpOutput.h"

#include "../src/bpImageConvertNew.h"

#include "../meta/bpUtils.h"
#include "../meta/bpFileInfo.h"
#include "../meta/bpFileTools.h"
#include "../meta/bpDataIO.h"
#include "../meta/bpImageDescriptorXmlTree.h"
#include "../meta/bpImageDescriptorFactory.h"
#include "../meta/bpParameterSection.h"
#include "../meta/bpFileReaderFactory.h"
#include "../meta/bpFileReaderSeriesAdjustable.h"

#include <hdf5.h>

#include <sstream>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>


bpConverter::bpConverter(bpSharedPtr<bpFileReaderFactory> aFileReaderFactory)
: mInputFileName(""),
  mInputList(false),
  mInputFileFormat(""),
  mInputFileImageIndex(static_cast<bpSize>(-1)),
  mInputFileCropSizes(10,0),
  mInputSeriesLayoutFileName(""),
  mOutputFileName(""),
  mOutputFileFormat("Imaris5"),
  mAllFilesFileName(""),
  mMetaDataFileName(""),
  mMetaDataForArenaFileName(""),
  mVoxelHashFileName(""),
  mVoxelHashBlockSort("t"),
  mImageDescriptorsFileName(""),
  mThumbnailSettings(),
  mLogFile(""),
  mDoMetaDataCalculation(false),
  mDoMetaDataForArenaCalculation(false),
  mDoVoxelHashCalculation(false),
  mDoImageDescriptorsCalculation(false),
  mDoAllFileNamesCalculcation(false),
  mEnableLogProgress(false),
  mPrintSupportedFormats(false),
  mThroughputOutputInterval(0.0f),
  mFileReaderFactory(std::move(aFileReaderFactory)),
  mNumberOfThreads(8),
  mCompressionAlgorithmType(bpConverterTypes::tCompressionAlgorithmType::eCompressionAlgorithmGzipLevel2)
{
  bpLogger::SetSink(bpLogger::eSinkStdCOut);
  bpLogger::SetMinimumLogLevel(bpLogger::eLogLevelError);

  H5Eset_auto1(nullptr, nullptr);
}


const bpSharedPtr<bpFileReaderFactory>& bpConverter::GetFileReaderFactory() const
{
  return mFileReaderFactory;
}


void bpConverter::SetInputFileName(const bpString& aInputFileName, const bpString& aArgumentName)
{
  bpString vInputFileName = bpFileTools::ConvertSeparators(bpFileTools::GetAbsoluteFilePath(aInputFileName));
  SetParameterOnce(mInputFileName, vInputFileName, aArgumentName);
}


void bpConverter::SetInputList(bool aInputList)
{
  mInputList = aInputList;
}


void bpConverter::SetAdditionalInputFileNames(const std::vector<bpString>& aInputFileNames)
{
  mAdditionalInputFileNames = aInputFileNames;
}


void bpConverter::SetInputFileFormat(const bpString& aInputFileFormat, const bpString& aArgumentName)
{
  SetParameterOnce(mInputFileFormat, aInputFileFormat, aArgumentName);
}


void bpConverter::SetInputFileImageIndex(bpSize aInputFileImageIndex)
{
  mInputFileImageIndex = aInputFileImageIndex;
}


void bpConverter::SetInputFileCropSizes(const std::vector<bpSize>& aCropSizes)
{
  mInputFileCropSizes = aCropSizes;
}

void bpConverter::SetInputVoxelSize(const bpVector3Float& aInputVoxelSize)
{
  mInputVoxelSize = aInputVoxelSize;
}


void bpConverter::SetInputVoxelSizeX(bpFloat aX)
{
  mInputVoxelSize[0] = aX;
}


void bpConverter::SetInputVoxelSizeY(bpFloat aY)
{
  mInputVoxelSize[1] = aY;
}


void bpConverter::SetInputVoxelSizeZ(bpFloat aZ)
{
  mInputVoxelSize[2] = aZ;
}


void bpConverter::SetInputSeriesLayoutFileName(const bpString& aInputSeriesLayoutFileName, const bpString& aArgumentName)
{
  SetParameterOnce(mInputSeriesLayoutFileName, aInputSeriesLayoutFileName, aArgumentName);
}


void bpConverter::SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension aDimension, const bpString& aDelimiters)
{
  auto vDelimiters = mFileSeriesDelimiters.GetDelimiters();
  vDelimiters[aDimension] = aDelimiters;
  mFileSeriesDelimiters.SetDelimiters(vDelimiters);
}


void bpConverter::SetOutputFileName(const bpString& aOutputFileName, const bpString& aArgumentName)
{
  bpString vOutputFileName = bpFileTools::GetAbsoluteFilePath(aOutputFileName);
  SetParameterOnce(mOutputFileName, vOutputFileName, aArgumentName);
}


void bpConverter::AddThumbnail(const bpString& aThumbnailFileName, const bpParameterSection* aSectionDefaultParameters)
{
  if (!aThumbnailFileName.empty()) {
    cThumbnailSettings vThumbnailSettings;
    vThumbnailSettings.mFileName = bpFileTools::GetAbsoluteFilePath(aThumbnailFileName);
    vThumbnailSettings.mBackground = aSectionDefaultParameters ? aSectionDefaultParameters->GetParameter("Background") : "";
    vThumbnailSettings.mImageSize = aSectionDefaultParameters ? bpFromString<bpSize>(aSectionDefaultParameters->GetParameter("ImageSize")) : 256;
    vThumbnailSettings.mMode = aSectionDefaultParameters ? aSectionDefaultParameters->GetParameter("Mode") : "";
    vThumbnailSettings.mTimeIndex = 0;
    vThumbnailSettings.mSliceIndexZ = 0;
    vThumbnailSettings.mMaxSizeXY = 1024;
    vThumbnailSettings.mHistogramStretch = aSectionDefaultParameters ? bpFromString<bpFloat>(aSectionDefaultParameters->GetParameter("HistogramStretch")) : 1;
    vThumbnailSettings.mOutputFormat = "jpg";
    mThumbnailSettings.push_back(vThumbnailSettings);
  }
}


void bpConverter::SetThumbnailBackground(const bpString& aThumbnailBackground, const bpString& aArgumentName)
{
  GetThumbnailSettings().mBackground = aThumbnailBackground;
}


void bpConverter::SetThumbnailMode(const bpString& aThumbnailMode, const bpString& aArgumentName)
{
  GetThumbnailSettings().mMode = bpToLower(aThumbnailMode);
}


void bpConverter::SetThumbnailImageSize(bpSize aThumbnailImageSize)
{
  GetThumbnailSettings().mImageSize = aThumbnailImageSize;
}


void bpConverter::SetThumbnailTimeIndex(bpInt32 aThumbnailTimeIndex)
{
  GetThumbnailSettings().mTimeIndex = aThumbnailTimeIndex;
}


void bpConverter::SetThumbnailSliceIndexZ(bpInt32 aThumbnailSliceIndexZ)
{
  GetThumbnailSettings().mSliceIndexZ = aThumbnailSliceIndexZ;
}


void bpConverter::SetThumbnailMaxSizeXY(bpSize aThumbnailMaxSizeXY)
{
  GetThumbnailSettings().mMaxSizeXY = aThumbnailMaxSizeXY;
}

void bpConverter::SetThumbnailOutputFormat(const bpString& aOutputFormat)
{
  GetThumbnailSettings().mOutputFormat = bpToLower(aOutputFormat);
}


void bpConverter::SetThroughputOutputInterval(bpUInt32 aTimeIntervalInMs)
{
  mThroughputOutputInterval = aTimeIntervalInMs;
}


void bpConverter::SetAllFilesFileName(const bpString& aAllFilesFileName, const bpString& aArgumentName)
{
  SetParameterOnce(mAllFilesFileName, aAllFilesFileName, aArgumentName);
}


void bpConverter::SetDoAllFilesNamesCalculation(bool aFlag)
{
  mDoAllFileNamesCalculcation = aFlag;
}


void bpConverter::SetMetaDataFileName(const bpString& aMetaDataFileName, const bpString& aArgumentName)
{
  SetParameterOnce(mMetaDataFileName, aMetaDataFileName, aArgumentName);
}

void bpConverter::SetMetaDataForArenaFileName(const bpString& aMetaDataFileName, const bpString& aArgumentName)
{
  SetParameterOnce(mMetaDataForArenaFileName, aMetaDataFileName, aArgumentName);
}


void bpConverter::SetDoMetaDataCalculation(bool aFlag)
{
  mDoMetaDataCalculation = aFlag;
}

void bpConverter::SetDoMetaDataForArenaCalculation(bool aFlag)
{
  mDoMetaDataForArenaCalculation = aFlag;
}

void bpConverter::SetVoxelHashFileName(const bpString& aVoxelHashFileName, const bpString& aArgumentName)
{
  SetParameterOnce(mVoxelHashFileName, aVoxelHashFileName, aArgumentName);
}


void bpConverter::SetDoVoxelHashCalculated(bool aFlag)
{
  mDoVoxelHashCalculation = aFlag;
}


void bpConverter::SetDoImageDescriptorsCalculation(bool aFlag)
{
  mDoImageDescriptorsCalculation = aFlag;
}


void bpConverter::SetLogFile(const bpString& aLogFile, const bpString& aArgumentName)
{
  SetParameterOnce(mLogFile, aLogFile, aArgumentName);

  if (mLogFile == "none") {
    bpLogger::SetSink(0);
  }
  else {
    bpLogger::SetSink(bpLogger::eSinkFile);
    bpLogger::SetLogFileName(mLogFile);
    bpLogger::SetMinimumLogLevel(bpLogger::eLogLevelInfo);
  }
}


void bpConverter::SetEnableLogProgress(bool aEnableLogProgress)
{
  mEnableLogProgress = aEnableLogProgress;
}


void bpConverter::SetPrintSupportedFormats(bool aEnable)
{
  mPrintSupportedFormats = aEnable;
}


void bpConverter::SetVoxelHashBlockSort(const bpString& aVoxelHashBlockSort, const bpString& aArgumentName)
{
  mVoxelHashBlockSort = aVoxelHashBlockSort;
}


void bpConverter::SetNumberOfThreads(bpSize aNumberOfThreads)
{
  mNumberOfThreads = aNumberOfThreads;
}


void bpConverter::SetCompressionAlgorithmType(bpConverterTypes::tCompressionAlgorithmType aCompressionAlgorithmType)
{
  mCompressionAlgorithmType = aCompressionAlgorithmType;
}


void bpConverter::SetImageDescriptorsFileName(const bpString& aImageDescriptorsFileName, const bpString& aArgumentName)
{
  mImageDescriptorsFileName = aImageDescriptorsFileName;
}


void bpConverter::SetParameterOnce(bpString& aParameter, const bpString& aValue, const bpString& aArgumentName)
{
  if (aParameter.empty()) {
    aParameter = aValue;
  }
  else {
    bpLogger::LogError("You can not set argument \"" + aArgumentName + "\" multiple times. Use --help for details");
    exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
  }
}


void bpConverter::PrintSupportedFormats()
{
  std::set<bpString> vFormats;
  auto vFormatIt = GetFileReaderFactory()->FormatBegin();
  auto vFormatEnd = GetFileReaderFactory()->FormatEnd();
  for (; vFormatIt != vFormatEnd; ++vFormatIt) {
    const auto& vExtensions = GetFileReaderFactory()->GetFormatExtensions(*vFormatIt);
    vFormats.insert(vExtensions.begin(), vExtensions.end());
  }
  for (const bpString& vExt : vFormats) {
    std::cout << vExt << " ";
  }
  std::cout << std::endl;
}


bpConverter::cThumbnailSettings& bpConverter::GetThumbnailSettings()
{
  if (!mThumbnailSettings.empty()) {
    return mThumbnailSettings.back();
  }
  else {
    bpLogger::LogError("Use option \"-t\" to specify a thumbnail, or call option \"--help\" for more details");
    exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
  }
}


void bpConverter::CheckParameters() const
{
  if (mInputFileName.empty() && !mPrintSupportedFormats) {
    bpLogger::LogError("Missing input file name. Use --help for details");
    exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
  }
}


bool bpConverter::Execute()
{
  if (mPrintSupportedFormats) {
    PrintSupportedFormats();
    return true;
  }

  bool vSuccess = true;
  /*
  // force a "random" crash (for testing)
  if (mInputFileName.length() % 3 == 0) {
    bpUInt32* vValue = 0;
    *vValue = 0;
  }
  */
  bpString vXMLLayout;
  if (!mInputSeriesLayoutFileName.empty()) {
    vXMLLayout = ReadXMLLayoutFromFile();
    vSuccess &= !vXMLLayout.empty();
  }

  if (mInputList) {
    mInputList = false;
    std::ifstream vInput(mInputFileName);
    if (!std::getline(vInput, mInputFileName) || mInputFileName.empty()) {
      return false;
    }
    bpString vLine;
    while (std::getline(vInput, vLine)) {
      if (!vLine.empty()) {
        mAdditionalInputFileNames.push_back(vLine);
      }
    }
  }

  bool vTriedCreateFileReader = false;
  bpSharedPtr<bpFileReader> vFileReader;
  auto vCreateFileReader = [&vTriedCreateFileReader, &vFileReader, this, &vXMLLayout] {
    if (!vTriedCreateFileReader) {
      vFileReader = CreateFileReader(mInputFileName, mInputFileFormat, mInputFileImageIndex, vXMLLayout);
      vTriedCreateFileReader = true;
    }
    return !!vFileReader;
  };

  // image descriptors output?
  if (mDoImageDescriptorsCalculation || !mImageDescriptorsFileName.empty()) {
    boost::filesystem::path vInputPath(mInputFileName);
    bool vIsFound = false;

    for (const auto& vProcessedFileName : mProcessedInputFileNames) {
      boost::filesystem::path vProcessedPath(vProcessedFileName);
      boost::system::error_code vErrorCode;

      if (boost::filesystem::equivalent(vInputPath, vProcessedPath, vErrorCode)) {
        vIsFound = true;
        break;
      }
    }

    if (!vIsFound) {
      vCreateFileReader(); // empty descriptor is a success
    }

    vSuccess &= CreateImageDescriptors(vFileReader);
  }

  // list of all attached files output?
  if (mDoAllFileNamesCalculcation || !mAllFilesFileName.empty()) {
    vSuccess &= vCreateFileReader() && CreateAllFiles(vFileReader);
  }

  // meta info output?
  if (mDoMetaDataCalculation || !mMetaDataFileName.empty()) {
    vSuccess &= vCreateFileReader() && CreateMetaData(vFileReader);
  }

  // meta info output?
  if (mDoMetaDataForArenaCalculation || !mMetaDataForArenaFileName.empty()) {
    vSuccess &= vCreateFileReader() && CreateMetaDataForArena(vFileReader);
  }

  // voxel hash code output?
  if (mDoVoxelHashCalculation || !mVoxelHashFileName.empty()) {
    vSuccess &= vCreateFileReader() && CreateVoxelHash(vFileReader);
  }

  // Did the user request writing an output file?
  if (!mOutputFileName.empty() && !mOutputFileFormat.empty()) {
    vSuccess &= vCreateFileReader() && CheckIfVoxelSizeIsKnown(vFileReader) && ConvertFile(vFileReader);
  }

  // Did the user request writing thumbnails?
  if (!mThumbnailSettings.empty()) {
    vSuccess &= vCreateFileReader() && CreateThumbnails(vFileReader);
  }

  if (!mAdditionalInputFileNames.empty()) {
    std::vector<bpString> vInput;
    vInput.swap(mAdditionalInputFileNames);
    for (auto& vInputFile : vInput) {
      mInputFileName = std::move(vInputFile);
      vSuccess &= Execute();
    }
  }

  return vSuccess;
}


bpSharedPtr<bpFileReader> bpConverter::CreateFileReader(const bpString& aInputFileName,
                                                         const bpString& aInputFileFormat,
                                                         bpSize aInputFileImageIndex,
                                                         bpString aXMLLayout) const
{
  bpFileReader::tPtr vFileReader;

  try {
    if (!aInputFileFormat.empty()) {
      vFileReader = GetFileReaderFactory()->CreateFileReader(aInputFileName, aInputFileFormat);
    }
    else {
      vFileReader = GetFileReaderFactory()->CreateFileReader(aInputFileName);
    }

    auto vReaderImpl = vFileReader->GetReaderImpl();
    if (vReaderImpl) {
      if (!aXMLLayout.empty()) {
        vReaderImpl->SeriesSetFileLayouts({ bpFileReader::FileLayoutFromXMLString(aXMLLayout) });
      }
      else if (vReaderImpl->SeriesConfigurable() && !vReaderImpl->SeriesInitialised()) {
        bpFileReaderSeriesFileCheck vSeriesFileCheck(vReaderImpl.get());
        bpFileReaderSeriesAdjustable vSeriesAdjustableDialog(mFileSeriesDelimiters);
        vSeriesAdjustableDialog.EvalFilesAndSetSeries(vReaderImpl->GetFileName(), &vSeriesFileCheck);
        vReaderImpl->SeriesSetFileLayouts(vSeriesAdjustableDialog.GetFileLayouts());
      }

      if (aInputFileImageIndex != static_cast<bpSize>(-1)) {
        if (aInputFileImageIndex < vFileReader->GetNumberOfDataSets()) {
          vFileReader->SetActiveDataSetIndex(aInputFileImageIndex);
        }
        else {
          throw std::runtime_error("ImarisConvert: Can't set image index " + bpToString(aInputFileImageIndex) + ". File has only " + bpToString(vFileReader->GetNumberOfDataSets()) + " images.");
        }
      }
    }

    auto vConfig = vFileReader->GetConfig();
    if (vConfig) {
      if (mInputFileCropSizes.size() != 10) {
        throw std::runtime_error("Wrong Crop Limits.");
      }
      vConfig->SetCropLimits(mInputFileCropSizes[0], mInputFileCropSizes[1],
        mInputFileCropSizes[2], mInputFileCropSizes[3],
        mInputFileCropSizes[4], mInputFileCropSizes[5],
        mInputFileCropSizes[6], mInputFileCropSizes[7],
        mInputFileCropSizes[8], mInputFileCropSizes[9]);
      vConfig->SetForcedVoxelSize(mInputVoxelSize);
    }

    return vFileReader;
  }
  catch (const std::exception& vException) {
    bpLogger::LogError("Unable to create reader for \"" + aInputFileName + "\"");
    bpLogger::LogError(vException.what());
  }

  return vFileReader;
}


bool bpConverter::ConvertFile(bpSharedPtr<bpFileReader> aFileReader)
{
  mMeasurementFetcherThread.Start(mThroughputOutputInterval);

  bool vSuccess = true;
  try {
    bpLogger::LogInfo("Converting \"" + mInputFileName + "\" into \"" + mOutputFileName + "\"");

    bpConverterTypes::cOptions vOptions;
    vOptions.mEnableLogProgress = mEnableLogProgress;
    vOptions.mNumberOfThreads = mNumberOfThreads;
    vOptions.mCompressionAlgorithmType = mCompressionAlgorithmType;
    bpImageConvertNew::Convert(aFileReader, mOutputFileName, {}, vOptions);
  }
  catch (std::exception& vException) {
    bpLogger::LogError("Error during conversion : '" + bpString(vException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  catch (...) {
    bpLogger::LogError("Unknown error during conversion on " + mInputFileName);
    vSuccess = false;
  }

  mMeasurementFetcherThread.Stop();
  return vSuccess;
}


bpUInt64 bpConverter::GetValueFromHexString(const bpString& aValue) const
{
  bpUInt64 vResult = 0;
  bpUInt64 vFactor = 1;
  bpString vValue = bpToUpper(aValue);
  static bpString vHexDigits("0123456789ABCDEF");
  for (bpSize vIndex = vValue.length(); vIndex > 0; vIndex--) {
    bpSize vPos = vHexDigits.find(vValue[vIndex-1]);
    if (vPos != bpString::npos) {
      vResult += vFactor * vPos;
      vFactor *= 16;
    }
  }
  return vResult;
}

bpString bpConverter::ReadXMLLayoutFromFile() const
{
#if defined(_WIN32)
#define FILE_SCHEME "file:///"
#else
#define FILE_SCHEME "file://"
#endif

  bpString vInputSeriesLayoutFileName = bpReplace(mInputSeriesLayoutFileName, FILE_SCHEME, "");

  // read file
#ifdef BP_UTF8_FILENAMES
  std::ifstream vInputFile(bpFileTools::FromUtf8Path(vInputSeriesLayoutFileName), std::ifstream::binary);
#else
  std::ifstream vInputFile(vInputSeriesLayoutFileName.c_str());
#endif

  if (vInputFile) {
    vInputFile.seekg(0, vInputFile.end);
    std::streamoff vLength = vInputFile.tellg();
    vInputFile.seekg(0, vInputFile.beg);
    char* vBuffer = new char[vLength];
    vInputFile.read(vBuffer, vLength);

    vInputFile.close();
    bpString vXMLLayoutFromFile(vBuffer, static_cast<bpSize>(vLength));
    delete[] vBuffer;
    return vXMLLayoutFromFile;
  }
  return bpString();
}


//void bpConverter::GetColorRGBA(const bpString& aColorName, bpPackedRGBA& aColorRGBA) const
//{
//  if (aColorName.find('#') == 0) {
//    if (aColorName.length() == 7) {
//      aColorRGBA.r = (bpUInt8)GetValueFromHexString(aColorName.substr(1, 2));
//      aColorRGBA.g = (bpUInt8)GetValueFromHexString(aColorName.substr(3, 2));
//      aColorRGBA.b = (bpUInt8)GetValueFromHexString(aColorName.substr(5, 2));
//    }
//    else if (aColorName.length() == 9) {
//      aColorRGBA.r = (bpUInt8)GetValueFromHexString(aColorName.substr(1, 2));
//      aColorRGBA.g = (bpUInt8)GetValueFromHexString(aColorName.substr(3, 2));
//      aColorRGBA.b = (bpUInt8)GetValueFromHexString(aColorName.substr(5, 2));
//      aColorRGBA.a = (bpUInt8)GetValueFromHexString(aColorName.substr(7, 2));
//    }
//  }
//}


bool bpConverter::CreateThumbnails(bpSharedPtr<bpFileReader> aFileReader) const
{
  bool vSuccess = true;
  // loop over all thumbnails
  for (const cThumbnailSettings& vThumbnailSettings : mThumbnailSettings) {
    // keep the user informed
    bpLogger::LogInfo("Generate Thumbnail ("
      + bpToString(vThumbnailSettings.mSliceIndexZ) + " "
      + bpToString(vThumbnailSettings.mTimeIndex) + " "
      + vThumbnailSettings.mMode + " "
      + bpToString(vThumbnailSettings.mImageSize) + " "
      + vThumbnailSettings.mBackground + " "
      + vThumbnailSettings.mOutputFormat + ") on "
      + mInputFileName);

    // skip, if there is no name
    if (vThumbnailSettings.mFileName.empty()) {
      continue;
    }

    // keep the user informed
    bpLogger::LogInfo("Save as " + vThumbnailSettings.mFileName + " of " + mInputFileName);

    bpImageConvertNew::cConvertOptions vConvertOptions;
    vConvertOptions.mWriteMode = bpImageConvertNew::eWriteThumbnailOnly;
    vConvertOptions.mCompressThumbnail = vThumbnailSettings.mOutputFormat == "jpg" || vThumbnailSettings.mOutputFormat == "jpeg";

    bpConverterTypes::cOptions vOptions;
    vOptions.mEnableLogProgress = mEnableLogProgress;

    try {
      bpImageConvertNew::Convert(aFileReader, vThumbnailSettings.mFileName, vConvertOptions, vOptions);
    }
    catch (const std::exception& vException) {
      bpLogger::LogError("Unable to convert \"" + vThumbnailSettings.mFileName + "\"");
      bpLogger::LogError(vException.what());
      vSuccess = false;
    }

    vSuccess ? bpLogger::LogInfo("Thumbnail Saved of " + mInputFileName) : bpLogger::LogError("Thumbnail Failed of " + mInputFileName);
  }

  return vSuccess;
}


bool bpConverter::CreateAllFiles(bpSharedPtr<bpFileReader> aFileReader) const
{
  bool vSuccess = true;

  bpOutput vOutput;
  bpString vLogMessage = "Extracting List of attached Files of \"" + mInputFileName + "\" into ";
  if (mAllFilesFileName.empty()) {
    vOutput.SetSink(bpOutput::eSinkStdCOut);
    vLogMessage += "stdout";
  }
  else {
    vOutput.SetSink(bpOutput::eSinkFile);
    vOutput.SetOutputFileName(mAllFilesFileName);
    vLogMessage += mAllFilesFileName;
  }

  bpLogger::LogInfo(vLogMessage);

  try {
    // get the xml information
    // bpFileInfo vFileInfo(mInputFileName, aFileReader);
    // bpString vXML = vFileInfo.GetXML();
    bpString vXML = bpFileInfo::GetXML_AllFileNames(mInputFileName, aFileReader);

    // finally write out
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    // finished
    bpLogger::LogInfo("Finished writing attached files of " + mInputFileName);
  }
  catch (std::exception& aException) {
    bpLogger::LogError(bpString(aException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  return vSuccess;
}


bool bpConverter::CreateMetaData(bpSharedPtr<bpFileReader> aFileReader) const
{
  bool vSuccess = true;

  bpOutput vOutput;
  bpString vLogMessage = "Extracting File Meta Info of \"" + mInputFileName + "\" into ";
  if (mMetaDataFileName.empty()) {
    vOutput.SetSink(bpOutput::eSinkStdCOut);
    vLogMessage += "stdout";
  }
  else {
    vOutput.SetSink(bpOutput::eSinkFile);
    vOutput.SetOutputFileName(mMetaDataFileName);
    vLogMessage += mMetaDataFileName;
  }

  bpLogger::LogInfo(vLogMessage);

  try {
    // get the xml information
    // bpFileInfo vFileInfo(mInputFileName, aFileReader);
    // bpString vXML = vFileInfo.GetXML();
    bpString vXML = bpFileInfo::GetXML_ReadMetaData(mInputFileName, aFileReader, mInputFileImageIndex);

    // finally write out
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    // finished
    bpLogger::LogInfo("Finished writing metadata of " + mInputFileName);
  }
  catch (std::exception& aException) {
    bpLogger::LogError(bpString(aException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  return vSuccess;
}


bool bpConverter::CreateMetaDataForArena(bpSharedPtr<bpFileReader> aFileReader) const
{
  bool vSuccess = true;

  bpOutput vOutput;
  bpString vLogMessage = "Extracting File Meta Info of \"" + mInputFileName + "\" into ";
  if (mMetaDataForArenaFileName.empty()) {
    vOutput.SetSink(bpOutput::eSinkStdCOut);
    vLogMessage += "stdout";
  }
  else {
    vOutput.SetSink(bpOutput::eSinkFile);
    vOutput.SetOutputFileName(mMetaDataForArenaFileName);
    vLogMessage += mMetaDataForArenaFileName;
  }

  bpLogger::LogInfo(vLogMessage);

  try {
    std::vector<bpObjectDescriptor> vObjectsDescriptors;
    if (CheckForExtensionIms(aFileReader->GetFileExtension())) {
      bpDataIO::ReadObjectsInfo(mInputFileName, vObjectsDescriptors);
    }

    bpString vXML = bpFileInfo::GetXML_ReadMetaDataForArena(mInputFileName, aFileReader, mInputFileImageIndex, "", vObjectsDescriptors);

    // finally write out
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    // finished
    bpLogger::LogInfo("Finished writing metadata of " + mInputFileName);
  }
  catch (std::exception& aException) {
    bpLogger::LogError(bpString(aException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  return vSuccess;
}


bool bpConverter::CreateVoxelHash(bpSharedPtr<bpFileReader> aFileReader) const
{
  bool vSuccess = true;

  bpOutput vOutput;
  bpString vLogMessage = "Extracting Voxel Intensity Hash of \"" + mInputFileName + "\" into ";
  if (mVoxelHashFileName.empty()) {
    vOutput.SetSink(bpOutput::eSinkStdCOut);
    vLogMessage += "stdout";
  }
  else {
    vOutput.SetSink(bpOutput::eSinkFile);
    vOutput.SetOutputFileName(mVoxelHashFileName);
    vLogMessage += mVoxelHashFileName;
  }

  bpLogger::LogInfo(vLogMessage);

  try {
    // get the xml information
    bpString vXML = bpFileInfo::GetXML_ReadVoxelHash(mInputFileName, aFileReader, mInputFileImageIndex, mVoxelHashBlockSort);

    // finally write out
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    // finished
    bpLogger::LogInfo("Finished writing voxelhash of " + mInputFileName);
  }
  catch (std::exception& aException) {
    bpLogger::LogError(bpString(aException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  return vSuccess;
}


bool bpConverter::CreateImageDescriptors(bpSharedPtr<bpFileReader> aFileReader)
{
  bool vSuccess = true;

  bpOutput vOutput;
  bpString vLogMessage = "Extracting Image Descriptors of \"" + mInputFileName + "\" into ";
  if (mImageDescriptorsFileName.empty()) {
    vOutput.SetSink(bpOutput::eSinkStdCOut);
    vLogMessage += "stdout";
  }
  else {
    vOutput.SetSink(bpOutput::eSinkFile);
    vOutput.SetOutputFileName(mImageDescriptorsFileName);
    vLogMessage += mImageDescriptorsFileName;
  }

  bpLogger::LogInfo(vLogMessage);

  try {
    bpImageDescriptorFactory::tFilesDescriptors vFileDescriptors;
    if (aFileReader && mProcessedInputFileNames.find(mInputFileName) == mProcessedInputFileNames.end()) {
      vFileDescriptors = bpImageDescriptorFactory::CreateImageDescriptors(mInputFileName, aFileReader);
      for (const auto& vImageDescriptors : vFileDescriptors) {
        mProcessedInputFileNames.insert(vImageDescriptors.first);
        mProcessedInputFileNames.insert(vImageDescriptors.second.mImageDependencies.begin(), vImageDescriptors.second.mImageDependencies.end());
      }
    }

    auto vXmlTree = bpImageDescriptorXmlTree::ToXml(vFileDescriptors);

    auto vFileTag = vXmlTree->AddTag("File");
    vFileTag->AddAttr("path", mInputFileName);

    // get the xml information
    bpString vXML = vXmlTree->ToXml();

    // finally write out
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    // finished
    bpLogger::LogInfo("Finished writing Image Descriptors of " + mInputFileName);
  }
  catch (std::exception& aException) {
    auto vTree = std::make_shared<bpXmlTree>("<Images/>");
    auto vFileTag = vTree->AddTag("File");
    vFileTag->AddAttr("path", mInputFileName);
    bpString vXML = vTree->ToXml();
    vOutput.Write("<?xml version = \"1.0\" encoding = \"UTF-8\"?>\n");
    vOutput.Write(vXML);

    bpLogger::LogError(bpString(aException.what()) + " on " + mInputFileName);
    vSuccess = false;
  }
  return vSuccess;
}


bool bpConverter::ConfigureSeriesReader(bpSharedPtr<bpFileReader> aFileReader) const
{
  bpString vXMLLayoutFromFile = ReadXMLLayoutFromFile();
  if (!vXMLLayoutFromFile.empty()) {
    // configure
    bpFileReader::tFileLayout vSeriesLayout = bpFileReader::FileLayoutFromXMLString(vXMLLayoutFromFile);
    aFileReader->GetReaderImpl()->SeriesSetFileLayouts({ vSeriesLayout });
    return true;
  }
  return false;
}


bool bpConverter::CheckIfVoxelSizeIsKnown(bpSharedPtr<bpFileReader> aFileReader) const
{
   auto vVoxelSizeFromFile = aFileReader->GetReaderImpl()->GetExtentDelta();
   bpVector3Float vCombinedVoxelSize {
    mInputVoxelSize[0] != 0 ? mInputVoxelSize[0] : vVoxelSizeFromFile[0],
    mInputVoxelSize[1] != 0 ? mInputVoxelSize[1] : vVoxelSizeFromFile[1],
    mInputVoxelSize[2] != 0 ? mInputVoxelSize[2] : vVoxelSizeFromFile[2]
  };
  auto vAllSizesKnown = vCombinedVoxelSize[0] != 0 && vCombinedVoxelSize[1] != 0 && vCombinedVoxelSize[2] != 0;
  if (!vAllSizesKnown) {
    std::ostringstream vError;
    vError << "Unknown voxel size: "
           << vCombinedVoxelSize[0] << ","
           << vCombinedVoxelSize[1] << ","
           << vCombinedVoxelSize[2] << "."
           << " Please use the option -vs to specify the voxel size."
           << " To set the voxel size along a particular axis use the options -vsx, -vsy, and/or -vsz.";
    bpLogger::LogError(vError.str());
    exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
  }
  else {
    return true;
  }
}
