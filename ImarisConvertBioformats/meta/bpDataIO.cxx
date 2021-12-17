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


#include "bpDataIO.h"
#include "bpXmlTree.h"
#include "bpUtils.h"

#include <sstream>


static bool groupExists(hid_t parent, const char* aDirectoryName)
{
  return H5Lexists(parent, aDirectoryName, H5P_LINK_ACCESS_DEFAULT) > 0;
}


static bpString ReadVariableStringDataSet(hid_t aDataSet)
{
  hid_t vType = H5Dget_type(aDataSet);
  hid_t vSpace = H5Dget_space(aDataSet);
  hid_t vScalar = H5Screate(H5S_SCALAR);
  hsize_t vSize = 0;
  H5Sget_simple_extent_dims(vSpace, &vSize, nullptr);
  bpString vResult;
  for (hsize_t vIndex = 0; vIndex < vSize; vIndex++) {
    H5Sselect_elements(vSpace, H5S_SELECT_SET, 1, &vIndex);

    hvl_t vData;
    herr_t vStatus = H5Dread(aDataSet, vType, vScalar, vSpace, H5P_DEFAULT, &vData);
    if (vStatus >= 0) {
      vResult.append(static_cast<const char*>(vData.p), vData.len);
      H5Dvlen_reclaim(vType, vScalar, H5P_DEFAULT, &vData);
    }
  }
  H5Sclose(vScalar);
  H5Sclose(vSpace);
  H5Tclose(vType);
  return vResult;
}


static bpString ReadStringAttribute(hid_t aAttr)
{
  hsize_t size = H5Aget_storage_size(aAttr);
  std::vector<char> v(size + 1, '\0');
  hid_t type = H5Aget_type(aAttr);
  H5Aread(aAttr, type, v.data());
  H5Tclose(type);
  return bpString(v.data());
}


static bpUInt64 readAttrUI(hid_t group, const char* attrName)
{
  if (!H5Aexists(group, attrName)) {
    return 0;
  }
  auto attr = H5Aopen(group, attrName, H5P_DEFAULT);
  bpUInt64 v = 0;
  H5Aread(attr, H5T_NATIVE_UINT64, &v);
  H5Aclose(attr);
  return v;
}


static bpString readAttr(hid_t group, const char* attrName)
{
  if (!H5Aexists(group, attrName)) {
    return {};
  }
  auto attr = H5Aopen(group, attrName, H5P_DEFAULT);
  bpString vResult = ReadStringAttribute(attr);
  H5Aclose(attr);
  return vResult;
}


static bool IsVersionSmallerThan(const bpString& aVersion, bpInt32 aMajor, bpInt32 aMinor)
{
  if (aVersion.empty()) {
    return false;
  }

  std::vector<bpString> vNumbers = bpSplit(aVersion, ".");
  if (vNumbers.empty()) {
    return false;
  }

  bpInt32 vMajor = bpFromString<bpInt32>(bpSplit(vNumbers[0], " ").back());
  if (vMajor <= 0 || vMajor > aMajor) {
    return false;
  }

  if (vMajor < aMajor || vNumbers.size() < 2) {
    return true;
  }

  bpInt32 vMinor = bpFromString<bpInt32>(bpSplit(vNumbers[1], " ").front());
  return vMinor < aMinor;
}


static bpString GetPreferredSceneName(hid_t aFile)
{
  bpString vScene = "Scene";
  bpString vScene8 = "Scene8";
  if (!groupExists(aFile, vScene8.c_str())) {
    return vScene;
  }
  if (!groupExists(aFile, vScene.c_str())) {
    return vScene8;
  }
  hid_t vGroup = H5Gopen(aFile, vScene.c_str(), H5P_DEFAULT);
  bool vPreferScene = false;
  hid_t vData = H5Dopen(vGroup, "Data", H5P_DEFAULT);
  if (vData != H5I_INVALID_HID) {
    bpString vXmlData = ReadVariableStringDataSet(vData);
    if (!vXmlData.empty()) {
      bpXmlTree vXml(vXmlData, false, false);
      if (IsVersionSmallerThan(vXml.GetAttr("Version"), 7, 7)) {
        vPreferScene = true;
      }
    }
    H5Dclose(vData);
  }
  H5Gclose(vGroup);
  return vPreferScene ? vScene : vScene8;
}


void bpDataIO::ReadObjectsInfo(const bpString& aFileName, std::vector<bpObjectDescriptor>& aObjectsDescriptors)
{
  hid_t vFile = H5Fopen(aFileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  if (vFile == H5I_INVALID_HID) {
    return;
  }

  bpString vLoadSceneName = GetPreferredSceneName(vFile);
  if (groupExists(vFile, vLoadSceneName.c_str())) {
    hid_t vScene = H5Gopen(vFile, vLoadSceneName.c_str(), H5P_DEFAULT);
    if (groupExists(vScene, "Content")) {
      hid_t vContent = H5Gopen(vScene, "Content", H5P_DEFAULT);
      ReadObjectInfo(vContent, aObjectsDescriptors, "Points", bpObjectDescriptor::eTypeSpots);
      ReadObjectInfo(vContent, aObjectsDescriptors, "Surfaces", bpObjectDescriptor::eTypeSurfaces);
      ReadObjectInfo(vContent, aObjectsDescriptors, "MegaSurfaces", bpObjectDescriptor::eTypeMegaSurfaces);
      ReadObjectInfo(vContent, aObjectsDescriptors, "Filaments", bpObjectDescriptor::eTypeFilaments);
      ReadObjectInfo(vContent, aObjectsDescriptors, "Cells", bpObjectDescriptor::eTypeCells);
      H5Gclose(vContent);
    }
    H5Gclose(vScene);
  }
  H5Fclose(vFile);
}


void bpDataIO::ReadObjectInfo(hid_t aContentGroup, std::vector<bpObjectDescriptor>& aObjectsDescriptors, const bpString& aName, bpObjectDescriptor::tType aType)
{
  bpUInt64 vSize = readAttrUI(aContentGroup, ("NumberOf" + aName).c_str());
  for (bpUInt64 vIndex = 0; vIndex < vSize; vIndex++) {
    std::ostringstream vGroupName;
    vGroupName << aName;
    vGroupName << vIndex;
    hid_t vObjects = H5Gopen(aContentGroup, vGroupName.str().c_str(), H5P_DEFAULT);
    bpUInt64 vId = readAttrUI(vObjects, "Id");
    bpString vName = readAttr(vObjects, "Name");
    bpString vCreatorName = readAttr(vObjects, "CreatorName");
    bpString vCreationParemters = readAttr(vObjects, "CreationParameters");
    aObjectsDescriptors.emplace_back(vName, aType, vId, vCreatorName, vCreationParemters);
    H5Gclose(vObjects);
  }
}
