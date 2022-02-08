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


#include "bpImageDescriptorXmlTree.h"
#include "bpFileReader.h"
#include "bpUtils.h"


inline void bpFromString(const bpString& aString, std::vector<bpFileReaderImpl::Dimension>& aResult)
{
  aResult.clear();
  auto vStringVector = bpSplit(aString, " ");
  aResult.reserve(vStringVector.size());
  for (const auto& vString : vStringVector) {
    unsigned int vValue = bpFromString<unsigned int>(vString);
    aResult.push_back(static_cast<bpFileReaderImpl::Dimension>(vValue));
  }
}


bpXmlTree::tPtr bpImageDescriptorXmlTree::ToXml(const bpString& aImagePath, const bpImageDescriptor& aImageDescriptor, bool aMakeRelativePaths)
{
  auto vTree = std::make_shared<bpXmlTree>("<Image/>");

  vTree->AddAttr("path", aMakeRelativePaths ? bpFileTools::GetFileExt(aImagePath) : aImagePath);
  vTree->AddAttr("baseFileName", aMakeRelativePaths ? bpFileTools::GetFileExt(aImageDescriptor.mBaseFileName) : aImageDescriptor.mBaseFileName);
  vTree->AddAttr("dataSetName", aImageDescriptor.mDatasetName);
  vTree->AddAttr("imageIndex", bpToString(aImageDescriptor.mImageIndex));
  vTree->AddAttr("isNative", bpToString(aImageDescriptor.mIsNative));
  vTree->AddAttr("isSeriesConfigurable", bpToString(aImageDescriptor.mIsSeriesConfigurable));
  vTree->AddAttr("isSceneConfigurable", bpToString(aImageDescriptor.mIsSceneConfigurable));
  vTree->AddAttr("DimensionSequence", bpToString(aImageDescriptor.mDimensionSequence));
  vTree->AddAttr("DataSizeV", bpToString(aImageDescriptor.mDataSizeV));
  vTree->AddAttr("ExtentMin", bpToString(aImageDescriptor.mExtentMin));
  vTree->AddAttr("ExtentMax", bpToString(aImageDescriptor.mExtentMax));
  vTree->AddAttr("ForcedVoxelSize", bpToString(aImageDescriptor.mForcedVoxelSize));
  vTree->AddAttr("fileconverter", bpToString(aImageDescriptor.mConverterInfo.mExecutable));
  vTree->AddAttr("fileformat", bpToString(aImageDescriptor.mConverterInfo.mFileFormat));

  bpString vLayoutXml = bpFileReader::FileLayoutToXMLString(aImageDescriptor.mFileSeriesLayout);
  if (!vLayoutXml.empty()) {
    bpXmlTree vLayout(vLayoutXml);
    vTree->AddTag(vLayout);
  }

  if (!aImageDescriptor.mImageDependencies.empty()) {
    bpXmlTree::tPtr vDependenciesTag = vTree->AddTag("FilesDependencies");
    for (const auto& vDependency : aImageDescriptor.mImageDependencies) {
      bpXmlTree::tPtr vDependencyTag = vDependenciesTag->AddTag("Dependency");
      vDependencyTag->AddAttr("value", aMakeRelativePaths ? bpFileTools::GetRelativeFileName(aImagePath, vDependency) : vDependency);
    }
  }
  return vTree;
}


bpXmlTree::tPtr bpImageDescriptorXmlTree::ToXml(const std::vector<std::pair<bpString, bpImageDescriptor>>& aImageDescriptors, bool aMakeRelativePaths)
{
  auto vTree = std::make_shared<bpXmlTree>("<Images/>");
  for (const auto& vImageDescriptor : aImageDescriptors) {
    auto vSubTree = ToXml(vImageDescriptor.first, vImageDescriptor.second);
    vTree->AddTag(*vSubTree);
  }
  return vTree;
}


bool bpImageDescriptorXmlTree::FromXml(const bpXmlTree::tPtr& aXml, bpString& aImagePath, bpImageDescriptor& aImageDescriptor)
{
  if (aXml->GetName() != "Image") {
    return false;
  }

  aImagePath = aXml->GetAttr("path");
  aImageDescriptor = bpImageDescriptor(aXml->GetAttr("baseFileName"), aXml->GetAttr("dataSetName"));
  bpFromString(aXml->GetAttr("imageIndex"), aImageDescriptor.mImageIndex);
  bpFromString(aXml->GetAttr("isNative"), aImageDescriptor.mIsNative);
  bpFromString(aXml->GetAttr("isSeriesConfigurable"), aImageDescriptor.mIsSeriesConfigurable);
  bpFromString(aXml->GetAttr("isSceneConfigurable"), aImageDescriptor.mIsSceneConfigurable);
  bpFromString(aXml->GetAttr("DimensionSequence"), aImageDescriptor.mDimensionSequence);
  bpFromString(aXml->GetAttr("DataSizeV"), aImageDescriptor.mDataSizeV, " ");
  bpFromString(aXml->GetAttr("ExtentMin"), aImageDescriptor.mExtentMin);
  bpFromString(aXml->GetAttr("ExtentMax"), aImageDescriptor.mExtentMax);
  bpFromString(aXml->GetAttr("ForcedVoxelSize"), aImageDescriptor.mForcedVoxelSize);

  bpFromString(aXml->GetAttr("fileconverter"), aImageDescriptor.mConverterInfo.mExecutable);
  bpFromString(aXml->GetAttr("fileformat"), aImageDescriptor.mConverterInfo.mFileFormat);
  aImageDescriptor.mFileSeriesLayout = bpFileReader::FileLayoutFromXMLString(aXml->GetTag("FileSeriesLayout")->ToXml());
  if (aXml->GetTag("FilesDependencies")) {
    std::vector<bpXmlTree::tPtr> vDependencies = aXml->GetTag("FilesDependencies")->GetAllTags();
    for (const auto& vDependency : vDependencies) {
      aImageDescriptor.mImageDependencies.push_back(vDependency->GetAttr("value"));
    }
  }
  return true;
}


bool bpImageDescriptorXmlTree::FromXml(const bpXmlTree::tPtr& aXml, std::vector<std::pair<bpString, bpImageDescriptor>>& aImageDescriptors)
{
  aImageDescriptors.clear();

  if (aXml->GetName() != "Images") {
    return false;
  }

  std::vector<bpXmlTree::tPtr> vImageDescriptors = aXml->GetTags("Image");
  aImageDescriptors.resize(vImageDescriptors.size());
  for (bpSize vIndex = 0; vIndex < vImageDescriptors.size(); ++vIndex) {
    FromXml(vImageDescriptors[vIndex], aImageDescriptors[vIndex].first, aImageDescriptors[vIndex].second);
  }

  return true;
}
