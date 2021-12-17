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


#include "bpImageDescriptorFactory.h"

#include "src/bpWriterCommonHeaders.h"
#include  ICOptimalBlockLayout_h


bpImageDescriptorFactory::tFilesDescriptors bpImageDescriptorFactory::CreateImageDescriptors(const bpString& aFilename, const bpFileReader::tPtr& aFileReader)
{
  tFilesDescriptors vFilesToOpen;
  auto vFileReader = aFileReader->GetReaderImpl();
  if (!vFileReader) {
    return vFilesToOpen;
  }

  bpString vFilename = aFilename; //todo ib: bpNetTools::GetUNCPathFromMappedDrive(aFilename);

  // ims with cell has 2 datasets or 3 if ims5.5 and ims8 in one
  bool vIsImsWithCellComponent = CheckForExtensionIms(aFileReader->GetFileExtension()) && vFileReader->GetNumberOfDataSets() > 1;

  std::vector<bpString> vAllFileNames = aFileReader->GetAllFileNamesOfDataSet();
  for (auto& vFileName : vAllFileNames) {
    vFileName = bpFileTools::ConvertSeparators(vFileName);
  }
  bool vIsMultiImage = !vAllFileNames.empty()
                    && vFileReader->GetNumberOfDataSets() > 1
                    && !vIsImsWithCellComponent;

  bpSize vSize = vIsMultiImage ? vFileReader->GetNumberOfDataSets() : 1;
  for (bpSize vCount = 0; vCount < vSize; ++vCount) {

    bpImageDescriptor vImageDescriptor;
    vImageDescriptor.mBaseFileName = vFilename;
    vImageDescriptor.mDatasetName = bpFileTools::GetFileExt(vFilename);
    vImageDescriptor.mImageIndex = vIsMultiImage ? vCount : -1;

    if (!vAllFileNames.empty()) {
      vImageDescriptor.mImageDependencies = vAllFileNames;
    }
    // multiimage
    if (vIsMultiImage) {
      vFileReader->SetActiveDataSetIndex(vImageDescriptor.mImageIndex);
      // sub image name
      bpString vDatasetName = "";
      bpSectionContainer vParameters(vFileReader->ReadParameters());
      if (vParameters.HasParameter("Image", "Name")) {
        bpString vImageName = vParameters.GetParameter("Image", "Name");
        if ((vImageName.find("not specified") == bpString::npos) &&
          (vImageName.find("info available") == bpString::npos)) {
          vDatasetName = vImageName;
        }
      }
      vImageDescriptor.mDatasetName = vDatasetName + " Image " + bpToString(vImageDescriptor.mImageIndex + 1);
    }

    // series
    if (vFileReader->SeriesConfigurable()) {
      std::vector<bpFileReaderImpl::tFileLayout> vLayouts = vFileReader->SeriesGetFileLayouts();
      if (vLayouts.size() > 0 && vImageDescriptor.mImageIndex == -1) {
        if (!vLayouts[0].empty()) {
          vImageDescriptor.mFileSeriesLayout = vLayouts[0];
        }
      }
      else if (vLayouts.size() > static_cast<bpSize>(vImageDescriptor.mImageIndex) && !vLayouts[vImageDescriptor.mImageIndex].empty()) {
        vImageDescriptor.mFileSeriesLayout = vLayouts[vImageDescriptor.mImageIndex];
      }
    }

    // iQ single file
    else if (bpFileTools::GetExt(vFilename) == "kinetic") {
      bpSectionContainer vParameters(vFileReader->ReadParameters());
      if (vParameters.HasParameter("Image", "Name")) {
        vImageDescriptor.mDatasetName = vParameters.GetParameter("Image", "Name");
      }
    }

    vImageDescriptor.mIsNative = IsNative(vFileReader);

    vFilesToOpen.push_back(tFilenameDescriptor(vFilename, vImageDescriptor));
  }
  return vFilesToOpen;
}


bool bpImageDescriptorFactory::IsNative(const bpFileReader::tImplPtr& aFileReader)
{
  if (aFileReader->GetReaderDescription() != "Bitplane: Imaris 5.5" && aFileReader->GetReaderDescription() != "Big Data Viewer") {
    return false;
  }

  bpFileReaderImpl::Dimension vFileDimSeq[] = {
    bpFileReaderImpl::X, bpFileReaderImpl::Y, bpFileReaderImpl::Z, bpFileReaderImpl::C, bpFileReaderImpl::T
  };
  std::vector<bpFileReaderImpl::Dimension> vMemDimSeq = aFileReader->GetDimensionSequence();
  if (!std::equal(vMemDimSeq.begin(), vMemDimSeq.end(), vFileDimSeq)) {
    return false; // not the right dimension sequence (in truth only non-singleton block dims order need to match)
  }

  bpSize vSizeR = aFileReader->GetNumberOfResolutions();
  bpSize vSizeT = 1;
  std::vector<bpVector3Size> vPyramidSize(vSizeR);
  for (bpSize vIndexR = 0; vIndexR < vSizeR; ++vIndexR) {
    aFileReader->SetActiveResolutionLevel(vIndexR);
    std::vector<bpSize> vFileImageSize = aFileReader->GetDataSize().GetXYZCT();
    vPyramidSize[vIndexR] = { vFileImageSize[0], vFileImageSize[1], vFileImageSize[2] };
    vSizeT = vFileImageSize[4];
  }

  if (!CanCreateMultiResolutionImageContainers(vPyramidSize)) {
    return false;
  }

  bpSize vImageBlockSize = 1024 * 1024 / bpGetSizeOfType(aFileReader->GetDataType());
  std::vector<bpVector3Size> vPyramidBlockSize = GetOptimalBlockSizes(vImageBlockSize, vPyramidSize, vSizeT);

  for (bpSize vIndexR = 0; vIndexR < vSizeR; ++vIndexR) {
    const bpVector3Size& vBlockSize = vPyramidBlockSize[vIndexR];
    bpSize vIdealBlockSize[] = {
      vBlockSize[0],
      vBlockSize[1],
      vBlockSize[2],
      1,
      vImageBlockSize / (vBlockSize[0] * vBlockSize[1] * vBlockSize[2])
    };
    aFileReader->SetDataBlockSize(
      vIdealBlockSize[0],
      vIdealBlockSize[1],
      vIdealBlockSize[2],
      vIdealBlockSize[3],
      vIdealBlockSize[4],
      vIndexR);
    aFileReader->SetActiveResolutionLevel(vIndexR);
    std::vector<bpSize> vFileBlockSize = aFileReader->GetDataBlockSize().GetXYZCT();
    if (!std::equal(vFileBlockSize.begin(), vFileBlockSize.end(), vIdealBlockSize)) {
      return false;
    }
  }
  return true;
}


bool bpImageDescriptorFactory::CanCreateMultiResolutionImageContainers(const std::vector<bpVector3Size>& aSizeXYZPerLevel)
{
  if (aSizeXYZPerLevel.empty()) {
    return false;
  }

  bpSize vLimit = 16 * 1024 * 1024;

  bpSize vLowestLevel = aSizeXYZPerLevel.size() - 1;
  for (bpSize vResolutionLevel = 0; vResolutionLevel < vLowestLevel; ++vResolutionLevel) {
    const bpVector3Size& vDimensionsHigherLevel = aSizeXYZPerLevel[vResolutionLevel];
    const bpVector3Size& vDimensionsLowerLevel = aSizeXYZPerLevel[vResolutionLevel + 1];

    if (vDimensionsHigherLevel[0] == 0 || vDimensionsHigherLevel[1] == 0 || vDimensionsHigherLevel[2] == 0) {
      return false;
    }

    bool vCorrectXScaling = (vDimensionsHigherLevel[0] / 2) == vDimensionsLowerLevel[0] || vDimensionsHigherLevel[0] == vDimensionsLowerLevel[0];
    bool vCorrectYScaling = (vDimensionsHigherLevel[1] / 2) == vDimensionsLowerLevel[1] || vDimensionsHigherLevel[1] == vDimensionsLowerLevel[1];
    bool vCorrectZScaling = (vDimensionsHigherLevel[2] / 2) == vDimensionsLowerLevel[2] || vDimensionsHigherLevel[2] == vDimensionsLowerLevel[2];

    if (!(vCorrectXScaling && vCorrectYScaling && vCorrectZScaling)) {
      return vDimensionsHigherLevel[0] * vDimensionsHigherLevel[1] * vDimensionsHigherLevel[2] <= vLimit;
    }
  }

  const bpVector3Size& vLowestLevelResolution = aSizeXYZPerLevel[vLowestLevel];
  return vLowestLevelResolution[0] * vLowestLevelResolution[1] * vLowestLevelResolution[2] <= vLimit;
}
