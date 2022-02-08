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


#include "bpFileInfo.h"

#include "bpXML.h"
#include "bpHash.h"
#include "bpXmlTree.h"
#include "bpUtils.h"
#include "bpFileTools.h"
#include "bpParameterSection.h"


#include <algorithm>


template <typename T>
bpString bpToSectionString(const std::vector<T>& aVector, const bpString& aSectionName)
{
  bpString vResult;
  bpSize vIndex = 0;
  typename std::vector<T>::const_iterator vIterator = aVector.begin();
  while (vIterator != aVector.end()) {
    vResult += "[" + aSectionName + " " + bpToString(vIndex++) + "]\n";
    vResult += bpToString(*vIterator++);
  }
  return vResult;
}


std::vector<bpSize> GetDimension(const bpString& aFileName, const bpFileReader::tPtr& aFileReader)
{
  return aFileReader->GetReaderImpl() ? aFileReader->GetReaderImpl()->GetDataSize().GetXYZCT() : std::vector<bpSize>(5);
}


bool bpFileInfo::ReadMetaData(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  const bpString& aIndent,
  bpString& aXML,
  bpString& aExceptionText)
{
  try {

    // dump the meta-data from base reader
    auto vConfig = aFileReader->GetConfig();
    if (!vConfig) {
      vConfig = std::make_shared<bpFileReaderConfiguration>();
    }

    aXML += aIndent + bpXML::WriteOneLineTag("BaseCanCrop", bpToString(aFileReader->CanCrop())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseCanResample", bpToString(aFileReader->CanResample())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseActiveDataSetIndex", bpToString(aFileReader->GetActiveDataSetIndex())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseColorHint", bpToString(vConfig->GetColorHint())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseCropLimitsMax", bpToString(vConfig->GetCropLimitsMax())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseCropLimitsMin", bpToString(vConfig->GetCropLimitsMin())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseDefaultColorHint", bpToString(vConfig->GetDefaultColors(), ";")) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseFileConverter", "ImarisConvertBioFormats") + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseDescription", aFileReader->GetDescription()) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseDimension", bpToString(GetDimension(aFileName, aFileReader))) + "\n";

    bpString vFileExtensionString = bpJoin(aFileReader->GetFileExtension(), ";");
    aXML += aIndent + bpXML::WriteOneLineTag("BaseFileExtension", vFileExtensionString) + "\n";

    aXML += aIndent + bpXML::WriteOneLineTag("BaseNumberOfDataSets", bpToString(aFileReader->GetNumberOfDataSets())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseResampleInterval", bpToString(vConfig->GetResampleInterval())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseHasSettings", bpToString(aFileReader->HasSettings())) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseIsTimeReader", bpToString(aFileReader->IsTimeReader())) + "\n";

    // dump the meta-data from reader impl
    auto vReaderImpl = aFileReader->GetReaderImpl();
    if (vReaderImpl) {
      aXML += aIndent + bpXML::WriteOneLineTag("ImplCanMergeImages", bpToString(vReaderImpl->CanMergeImages())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplActiveDataSetIndex", bpToString(vReaderImpl->GetActiveDataSetIndex())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplActiveResolutionLevel", bpToString(vReaderImpl->GetActiveResolutionLevel())) + "\n";
   // aXML += aIndent + bpXML::WriteOneLineTag("ImplColorHint", bpToString(vReaderImpl->GetColorHint())) + "\n";  ... goofy: why is this different on win and mac? it it relevant as meta-data for the image?
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataBlockNumberOfVoxels", bpToString(vReaderImpl->GetDataBlockNumberOfVoxels())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataBlockSizeV", bpToString(vReaderImpl->GetDataBlockSizeV())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataSizeV", bpToString(vReaderImpl->GetDataSizeV())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataType", bpToString(vReaderImpl->GetDataType())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDimensionSequence", bpToString(vReaderImpl->GetDimensionSequence())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplExtendMax", bpToString(vReaderImpl->GetExtentMax())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplExtendMin", bpToString(vReaderImpl->GetExtentMin())) + "\n";
   // aXML += aIndent + bpXML::WriteOneLineTag("ImplFileName", vReaderImpl->GetFileName()) + "\n"; // don't ouput (the file name is not an image property, but a file property)
      aXML += aIndent + bpXML::WriteOneLineTag("ImplMergeImages", bpToString(vReaderImpl->GetMergeImages())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfDataBlocks", bpToString(vReaderImpl->GetNumberOfDataBlocks())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfDataSets", bpToString(vReaderImpl->GetNumberOfDataSets())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfResolutions", bpToString(vReaderImpl->GetNumberOfResolutions())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplReaderDescription", vReaderImpl->GetReaderDescription()) + "\n";

      bpString vReaderExtensionString = bpJoin(vReaderImpl->GetReaderExtension(), ";");
      aXML += aIndent + bpXML::WriteOneLineTag("ImplReaderExtension", vReaderExtensionString) + "\n";

      aXML += aIndent + bpXML::WriteOneLineTag("ImplNaNCheckStatus", bpToString(vReaderImpl->NaNCheckStatus())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplColorInfo", bpToSectionString(vReaderImpl->ReadColorInfo(), "ColorInfo")) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplParameters", vReaderImpl->ReadParameters().ToString(), true) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplTimeInfo", bpToString(vReaderImpl->ReadTimeInfo(), ";")) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplSceneConfigurable", bpToString(vReaderImpl->SceneConfigurable())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplSeriesConfigurable", bpToString(vReaderImpl->SeriesConfigurable())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplSeriesFileValid", bpToString(vReaderImpl->SeriesFileValid(aFileName))) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplShouldColorRangeBeAdjustedToMinMax", bpToString(vReaderImpl->ShouldColorRangeBeAdjustedToMinMax())) + "\n";

      // potential additional info to extract:
      // - loop over multi resolution levels (read size, block layout, and voxel intensities)
    }
  }
  catch (std::exception& vException) {
    aExceptionText = vException.what();
    aXML += aIndent + bpXML::WriteOneLineTag("Exception", aExceptionText) + "\n";
    return false;
  }

  return true;
}


bool bpFileInfo::ReadMetaDataForArena(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  const bpString& aIndent,
  bpString& aXML,
  bpString& aExceptionText)
{
  try {

    // dump the meta-data from base reader
    // todo mg: right place to add this?
    aXML += aIndent + bpXML::WriteOneLineTag("BaseFileConverter", "ImarisConvertBioFormats") + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseDescription", aFileReader->GetDescription()) + "\n";
    aXML += aIndent + bpXML::WriteOneLineTag("BaseDimension", bpToString(GetDimension(aFileName, aFileReader))) + "\n";

    bpString vFileExtensionString = bpJoin(aFileReader->GetFileExtension(), ";");
    aXML += aIndent + bpXML::WriteOneLineTag("BaseFileExtension", vFileExtensionString) + "\n";

    aXML += aIndent + bpXML::WriteOneLineTag("BaseNumberOfDataSets", bpToString(aFileReader->GetNumberOfDataSets())) + "\n";

    aXML += aIndent + bpXML::WriteOneLineTag("BaseHasSettings", bpToString(aFileReader->HasSettings())) + "\n";

    auto vConfig = aFileReader->GetConfig();
    if (!vConfig) {
      vConfig = std::make_shared<bpFileReaderConfiguration>();
    }

    aXML += aIndent + bpXML::WriteOneLineTag("BaseForcedVoxelSize", bpToString(vConfig->GetForcedVoxelSize())) + "\n";

    // dump the meta-data from reader impl
    auto vReaderImpl = aFileReader->GetReaderImpl();
    if (vReaderImpl) {
      aXML += aIndent + bpXML::WriteOneLineTag("ImplSeriesConfigurable", bpToString(vReaderImpl->SeriesConfigurable())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplActiveDataSetIndex", bpToString(vReaderImpl->GetActiveDataSetIndex())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplActiveResolutionLevel", bpToString(vReaderImpl->GetActiveResolutionLevel())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataBlockNumberOfVoxels", bpToString(vReaderImpl->GetDataBlockNumberOfVoxels())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataBlockSizeV", bpToString(vReaderImpl->GetDataBlockSizeV())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataSizeV", bpToString(vReaderImpl->GetDataSizeV())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDataType", bpToString(vReaderImpl->GetDataType())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplDimensionSequence", bpToString(vReaderImpl->GetDimensionSequence())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplExtendMax", bpToString(vReaderImpl->GetExtentMax())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplExtendMin", bpToString(vReaderImpl->GetExtentMin())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplMergeImages", bpToString(vReaderImpl->GetMergeImages())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfDataBlocks", bpToString(vReaderImpl->GetNumberOfDataBlocks())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfDataSets", bpToString(vReaderImpl->GetNumberOfDataSets())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplNumberOfResolutions", bpToString(vReaderImpl->GetNumberOfResolutions())) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplReaderDescription", vReaderImpl->GetReaderDescription()) + "\n";

      bpString vReaderExtensionString = bpJoin(vReaderImpl->GetReaderExtension(), ";");
      aXML += aIndent + bpXML::WriteOneLineTag("ImplReaderExtension", vReaderExtensionString) + "\n";

      aXML += aIndent + bpXML::WriteOneLineTag("ImplNaNCheckStatus", bpToString(vReaderImpl->NaNCheckStatus())) + "\n";
      bpSectionContainer vParameters = vReaderImpl->ReadParameters();
      bpSectionContainer vParametersForArena;
      if (vParameters.HasSection("Image")) {
        bpParameterSection* vImageSectionForArena = vParametersForArena.CreateSection("Image");
        bpParameterSection* vImageSection = vParameters.GetSection("Image");
        *vImageSectionForArena = *vImageSection;
      }

      bpSize vNumberOfChannels = vReaderImpl->GetDataSize(bpFileReaderImpl::Dimension::C);
      for (bpSize vChannelIndex = 0; vChannelIndex < vNumberOfChannels; vChannelIndex++) {
        bpString vChannel = "Channel " + bpToString(vChannelIndex);
        bpString vChannelName;
        if (vParameters.HasParameter(vChannel, "Name")) {
          vChannelName = vParameters.GetParameter(vChannel, "Name");
        }
        vParametersForArena.CreateSection(vChannel);
        vParametersForArena.SetParameter(vChannel, "Name", vChannelName);
      }

      if (vParameters.HasSection("Log")) {
        bpParameterSection* vLogSectionForArena = vParametersForArena.CreateSection("Log");
        bpParameterSection* vLogSection = vParameters.GetSection("Log");
        *vLogSectionForArena = *vLogSection;
      }

      aXML += aIndent + bpXML::WriteOneLineTag("ImplParameters", vParametersForArena.ToString(), true) + "\n";
      aXML += aIndent + bpXML::WriteOneLineTag("ImplSeriesFileValid", bpToString(vReaderImpl->SeriesFileValid(aFileName))) + "\n";

      // potential additional info to extract:
      // - loop over multi resolution levels (read size, block layout, and voxel intensities)
    }
  }
  catch (std::exception& vException) {
    aExceptionText = vException.what();
    aXML += aIndent + bpXML::WriteOneLineTag("Exception", aExceptionText) + "\n";
    return false;
  }

  return true;
}


bpString bpFileInfo::GetXML_AllFileNames(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  const bpString& aIndent)
{
  // main container for xml
  bpString vXML;
  vXML += aIndent + bpXML::GetStartTag("AllFileNames") + "\n";;
  bpString vIndent = aIndent + "  ";

  // first dump the related file names
  vXML += vIndent + bpXML::WriteOneLineTag("BaseDescription", aFileReader->GetDescription()) + "\n";
  vXML += vIndent + bpXML::WriteOneLineTag("FileName", bpFileTools::ConvertSeparatorsToUnix(aFileName)) + "\n";
  vXML += vIndent + bpXML::GetStartTag("AllFileNamesOfDataSet") + "\n";;
  bpSize vFileIndex = 0;
  const std::vector<bpString>& vFileNamesOfDataSet = aFileReader->GetAllFileNamesOfDataSet();
  for (const bpString& vFileName : vFileNamesOfDataSet) {
    std::vector<bpString> vAttrs;
    vAttrs.push_back("mIndex");
    vAttrs.push_back(bpToString(vFileIndex));
    vAttrs.push_back("mName");
    vAttrs.push_back(bpFileTools::ConvertSeparatorsToUnix(vFileName));
    vXML += vIndent + "  " + bpXML::WriteOneLineTag("File", "", vAttrs) + "\n";
    vFileIndex++;
  }
  vXML += vIndent + bpXML::GetEndTag("AllFileNamesOfDataSet") + "\n";;
  vXML += vIndent + bpXML::WriteOneLineTag("NumberOfImages", bpToString(aFileReader->GetNumberOfDataSets())) + "\n";

  // done
  vXML += aIndent + bpXML::GetEndTag("AllFileNames") + "\n";;
  return vXML;
}


bpString bpFileInfo::GetXML_ReadMetaData(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  bpSize aImageIndex,
  const bpString& aIndent)
{
  // main container for xml
  bpString vXML;
  vXML += aIndent + bpXML::GetStartTag("MetaData") + "\n";;
  bpString vIndent = aIndent + "  ";
  bool vHasExceptions = false;

  // dump the individual image data
  bpSize vImageIndex = aImageIndex;
  bpSize vImageIndexEnd = aImageIndex + 1;
  if (aImageIndex == static_cast<bpSize>(-1)) {
    vImageIndex = 0;
    vImageIndexEnd = aFileReader->GetNumberOfDataSets();
  }
  vXML += vIndent + bpXML::WriteOneLineTag("NumberOfImages", bpToString(aFileReader->GetNumberOfDataSets())) + "\n";
  while (vImageIndex != vImageIndexEnd) {
    aFileReader->SetActiveDataSetIndex(vImageIndex);
    std::vector<bpString> vAttrs;
    vAttrs.push_back("mIndex");
    vAttrs.push_back(bpToString(vImageIndex));
    vXML += vIndent + bpXML::GetStartTag("Image", vAttrs) + "\n";
    bpString vExceptionText = "";
    ReadMetaData(aFileName, aFileReader, vIndent + "  ", vXML, vExceptionText);
    if (vExceptionText != "") vHasExceptions = true;
    vXML += vIndent + bpXML::GetEndTag("Image") + "\n";
    ++vImageIndex;
  }

  // add a crash-tag to indicate exceptions
  vXML += vHasExceptions ? (vIndent + "<Crash/>\n") : "";

  // done
  vXML += aIndent + bpXML::GetEndTag("MetaData") + "\n";;
  return vXML;
}


bpString bpFileInfo::GetXML_ReadMetaDataForArena(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  bpSize aImageIndex,
  const bpString& aIndent,
  const std::vector<bpObjectDescriptor>& aObjectDescriptors)
{
  // main container for xml
  bpString vXML;
  vXML += aIndent + bpXML::GetStartTag("MetaData") + "\n";;
  bpString vIndent = aIndent + "  ";
  bool vHasExceptions = false;
  bpSize vNumberOfDataSets = aFileReader->GetNumberOfDataSets();

  // dump the individual image data
  bpSize vImageIndex = aImageIndex;
  bpSize vImageIndexEnd = aImageIndex + (vNumberOfDataSets > 0 ? 1 : 0);
  if (aImageIndex == static_cast<bpSize>(-1)) {
    vImageIndex = 0;
    vImageIndexEnd = vNumberOfDataSets;
  }
  vXML += vIndent + bpXML::WriteOneLineTag("NumberOfImages", bpToString(vNumberOfDataSets)) + "\n";
  while (vImageIndex != vImageIndexEnd) {
    aFileReader->SetActiveDataSetIndex(vImageIndex);
    std::vector<bpString> vAttrs;
    vAttrs.push_back("mIndex");
    vAttrs.push_back(bpToString(vImageIndex));
    vXML += vIndent + bpXML::GetStartTag("Image", vAttrs) + "\n";
    bpString vExceptionText = "";
    ReadMetaDataForArena(aFileName, aFileReader, vIndent + "  ", vXML, vExceptionText);
    if (vExceptionText != "") vHasExceptions = true;
    vXML += vIndent + bpXML::GetEndTag("Image") + "\n";
    ++vImageIndex;
  }

  if (vNumberOfDataSets == 0) {
    vXML += vIndent + bpXML::GetStartTag("Scene") + "\n";
    vXML += vIndent + "  " + bpXML::WriteOneLineTag("ReaderDescription", aFileReader->GetDescription()) + "\n";
    vXML += vIndent + bpXML::GetEndTag("Scene") + "\n";
  }

  // add a crash-tag to indicate exceptions
  vXML += vHasExceptions ? (vIndent + "<Crash/>\n") : "";

  vIndent = aIndent + "  ";
  if (!aObjectDescriptors.empty()) {
    //vXML += vIndent + bpXML::WriteOneLineTag("NumberOfObjects", bpToString(aObjectDescriptors.size())) + "\n";
    vXML += vIndent + bpXML::GetStartTag("Objects") + "\n";
    for (const auto& vObjectDescriptor : aObjectDescriptors) {
      std::vector<bpString> vAttrs;
      vAttrs.push_back("Name");
      vAttrs.push_back(vObjectDescriptor.GetName());
      vXML += vIndent + bpXML::GetStartTag("Object", vAttrs) + "\n";
      bpString vExceptionText = "";
      vXML += vIndent + "  " + bpXML::WriteOneLineTag("Type", vObjectDescriptor.GetTypeString()) + "\n";
      vXML += vIndent + "  " + bpXML::WriteOneLineTag("Id", bpToString(vObjectDescriptor.GetId())) + "\n";
      vXML += vIndent + "  " + bpXML::WriteOneLineTag("CreatorName", vObjectDescriptor.GetCreatorName()) + "\n";

      vXML += vIndent + "  " + bpXML::WriteOneLineTag("CreationParamters", vObjectDescriptor.GetCreationParameters()) + "\n";

      vXML += vIndent + bpXML::GetEndTag("Object") + "\n";

    }
    vXML += vIndent + bpXML::GetEndTag("Objects") + "\n";
  }

  //// add a crash-tag to indicate exceptions
  //vXML += vHasExceptions ? (vIndent + "<Crash/>\n") : "";

  // done
  vXML += aIndent + bpXML::GetEndTag("MetaData") + "\n";;
  return vXML;
}



std::vector<bpSize> GetIndexSequence(bpSize aSize)
{
  std::vector<bpSize> vIndices(aSize);
  for (bpSize vIndex = 0; vIndex < aSize; vIndex++) vIndices[vIndex] = vIndex;
  return vIndices;
}


std::vector<bpSize> GetIndexSequenceRandom(bpSize aSize)
{
  std::vector<bpSize> vIndices(aSize);
  for (bpSize vIndex = 0; vIndex < aSize; vIndex++) vIndices[vIndex] = vIndex;
  std::random_shuffle(vIndices.begin(), vIndices.end());
  return vIndices;
}


class bpComparator
{
public:

  bpComparator(const std::vector<bpSize>& aDimensionSizes,
               bpSize aDimension)
  : mDimensionSizes(aDimensionSizes),
    mDimension(aDimension) {
  }

  inline bool operator () (bpSize vIndexA, bpSize vIndexB) const {
    return GetBlockIndex(vIndexA) < GetBlockIndex(vIndexB);
  }

private:

  bpSize GetBlockIndex(bpSize aIndex) const {
    bpSize vBlocks = 1;
    for (bpSize vDimension = 0; vDimension < mDimensionSizes.size(); vDimension++) {
      if (vDimension == mDimension) {
        return (aIndex / vBlocks) % mDimensionSizes[vDimension];
      }
      vBlocks *= mDimensionSizes[vDimension];
    }
    return 0;
  }

  const std::vector<bpSize>& mDimensionSizes;
  bpSize mDimension;
};


std::vector<bpSize> GetIndexSequenceByDimension(bpSize aSize,
                                                const std::vector<bpSize>& aDimensionSizes,
                                                bpSize aDimension)
{
  std::vector<bpSize> vIndices(aSize);
  for (bpSize vIndex = 0; vIndex < aSize; vIndex++) vIndices[vIndex] = vIndex;
  bpComparator vComparator(aDimensionSizes, aDimension);
  std::stable_sort(vIndices.begin(), vIndices.end(), vComparator);
  return vIndices;
}


std::vector<bpSize> GetNumberOfBlocksV(const std::vector<bpSize>& aSizes,
                                       const std::vector<bpSize>& aBlockSizes)
{
  std::vector<bpSize> vNumberOfBlocksV;
  if (aSizes.size() == aBlockSizes.size()) {
    vNumberOfBlocksV.resize(aSizes.size(), 0);
    for (bpSize vIndex = 0; vIndex < aSizes.size(); vIndex++) {
      if (aBlockSizes[vIndex] != 0) {
        vNumberOfBlocksV[vIndex] = aSizes[vIndex] / aBlockSizes[vIndex];
        if (vNumberOfBlocksV[vIndex] * aBlockSizes[vIndex] < aSizes[vIndex]) {
          vNumberOfBlocksV[vIndex]++;
        }
      }
    }
  }
  return vNumberOfBlocksV;
}


bool bpFileInfo::ReadVoxelHash(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  const bpString& aBlockSort,
  bpUInt32& aVoxelHash,
  bpUInt64& aVoxelBytes,
  bpString& aExceptionText)
{
  try {
    // need reader impl to access the voxel intensities
    auto vReaderImpl = aFileReader->GetReaderImpl();
    if (vReaderImpl) {
      bpUInt32* vBuffer = 0;

      // allocate buffer for data block
      bpUInt64 vBlockVoxels = vReaderImpl->GetDataBlockNumberOfVoxels();
      bpUInt64 vTypeSize = bpGetSizeOfType(vReaderImpl->GetDataType());
      bpUInt64 vBufferSize = (vBlockVoxels * vTypeSize) / sizeof(bpUInt32) + 1;
      const bpUInt64 vBufferExtraSize = 10;
      vBuffer = new bpUInt32[static_cast<bpSize>(vBufferSize + vBufferExtraSize)];
      memset(vBuffer, 0xAA, static_cast<bpSize>((vBufferSize + vBufferExtraSize) * sizeof(bpInt32)));
      bpHash vVoxelHash;

      // set up block sequence
      bpSize vNumberOfDataBlocks = vReaderImpl->GetNumberOfDataBlocks();
      std::vector<bpFileReaderImpl::Dimension> vBlockSort;
      for (char vChar : aBlockSort) {
        switch (vChar) {
          case 'x': vBlockSort.push_back(bpFileReaderImpl::X); break;
          case 'y': vBlockSort.push_back(bpFileReaderImpl::Y); break;
          case 'z': vBlockSort.push_back(bpFileReaderImpl::Z); break;
          case 'c': vBlockSort.push_back(bpFileReaderImpl::C); break;
          case 't': vBlockSort.push_back(bpFileReaderImpl::T); break;
          default: break;
        }
      }
      std::vector<bpSize> vBlockIndices;
      if (vBlockSort.size() == 0) {
        vBlockIndices = GetIndexSequence(vNumberOfDataBlocks);
      }
      else if (vBlockSort.size() == 1) {
        vBlockIndices = GetIndexSequenceByDimension(vNumberOfDataBlocks,
                                                    GetNumberOfBlocksV(vReaderImpl->GetDataSizeV(), vReaderImpl->GetDataBlockSizeV()),
                                                    vReaderImpl->GetDimension(vBlockSort[0]));
      }
      else {
        delete [] vBuffer;
        return false;
      }

      // read all the data blocks
      for (bpSize vBlockNumber = 0; vBlockNumber < vNumberOfDataBlocks; vBlockNumber++) {
        memset(vBuffer, 0, static_cast<bpSize>(vBufferSize * sizeof(bpInt32)));
        vReaderImpl->GoToDataBlock(vBlockIndices[vBlockNumber]);
        vReaderImpl->ReadDataBlock(vBuffer);
        vVoxelHash.AddSamples(vBuffer, vBuffer + vBufferSize);
      }

      // check, if extra buffer is still intact
      for (bpUInt64 vIndex = vBufferSize; vIndex < vBufferSize + vBufferExtraSize; vIndex++) {
        bpUInt32 vValue = vBuffer[vIndex];
        if (vValue != 0xAAAAAAAA) {
          throw std::runtime_error("Nirvana Memory Violation!");
        }
      }

      // get hash & clean-up
      aVoxelHash = vVoxelHash.GetCode();
      aVoxelBytes = vNumberOfDataBlocks * vBlockVoxels * vTypeSize;
      delete [] vBuffer;
      return true;
    }
    else {
      return false;
    }
  }
  catch (std::exception& vException) {
    aExceptionText = vException.what();
    return false;
  }
}


bpString bpFileInfo::GetXML_ReadVoxelHash(
  const bpString& aFileName,
  const bpFileReader::tPtr& aFileReader,
  bpSize aImageIndex,
  const bpString& aBlockSort,
  const bpString& aIndent)
{
  // main container for xml
  bpString vXML;
  vXML += aIndent + bpXML::GetStartTag("VoxelHash") + "\n";;
  bpString vIndent = aIndent + "  ";

  // summary number of file and images
  vXML += vIndent + bpXML::WriteOneLineTag("NumberOfImages", bpToString(aFileReader->GetNumberOfDataSets())) + "\n";

  // check the selected image index
  bpSize vImageIndex = aImageIndex;
  bpSize vImageIndexEnd = aImageIndex + 1;
  if (aImageIndex == static_cast<bpSize>(-1)) {
    vImageIndex = 0;
    vImageIndexEnd = aFileReader->GetNumberOfDataSets();
  }

  // loop over images
  bool vHasExceptions = false;
  bpUInt64 vTotalVoxelBytes = 0;
  while (vImageIndex != vImageIndexEnd) {
    aFileReader->SetActiveDataSetIndex(vImageIndex);
    bpString vDummyXML = "";
    bpString vDummyExceptionText = "";
    ReadMetaData(aFileName, aFileReader, "", vDummyXML, vDummyExceptionText);
    std::vector<bpString> vAttrs;
    vAttrs.push_back("mIndex");
    vAttrs.push_back(bpToString(vImageIndex));
    bpUInt32 vVoxelHash = 0;
    bpUInt64 vVoxelBytes = 0;
    bpString vExceptionText = "";
    if (ReadVoxelHash(aFileName, aFileReader, aBlockSort, vVoxelHash, vVoxelBytes, vExceptionText)) {
      vTotalVoxelBytes += vVoxelBytes;
      vAttrs.push_back("mVoxelHash");
      vAttrs.push_back(bpToString(vVoxelHash));
      vAttrs.push_back("mVoxelBytes");
      vAttrs.push_back(bpToString(vVoxelBytes));
    }
    if (vExceptionText != "") {
      vAttrs.push_back("mException");
      vAttrs.push_back(vExceptionText);
      vHasExceptions = true;
    }
    vXML += vIndent + bpXML::WriteOneLineTag("Image", "", vAttrs) + "\n";
    ++vImageIndex;
  }

  // sum of bytes of all images
  vXML += vIndent + bpXML::WriteOneLineTag("TotalVoxelBytes", bpToString(vTotalVoxelBytes)) + "\n";

  // add a crash-tag to indicate exceptions
  vXML += vHasExceptions ? (vIndent + "<Crash/>\n") : "";

  // done
  vXML += aIndent + bpXML::GetEndTag("VoxelHash") + "\n";;
  return vXML;
}

