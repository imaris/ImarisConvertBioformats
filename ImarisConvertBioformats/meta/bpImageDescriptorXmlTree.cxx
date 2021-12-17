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


bpXmlTree::tPtr bpImageDescriptorXmlTree::ToXml(const bpString& aImagePath, const bpImageDescriptor& aImageDescriptor, bool aMakeRelativePaths)
{
  auto vTree = std::make_shared<bpXmlTree>("<Image/>");

  vTree->AddAttr("path", aMakeRelativePaths ? bpFileTools::GetFileExt(aImagePath) : aImagePath);
  vTree->AddAttr("baseFileName", aMakeRelativePaths ? bpFileTools::GetFileExt(aImageDescriptor.mBaseFileName) : aImageDescriptor.mBaseFileName);
  vTree->AddAttr("dataSetName", aImageDescriptor.mDatasetName);
  vTree->AddAttr("imageIndex", bpToString(aImageDescriptor.mImageIndex));
  vTree->AddAttr("isNative", bpToString(aImageDescriptor.mIsNative));

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
