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


#include "bpFileReader.h"
#include "bpXmlTree.h"


void bpFileReader::SetFileSeriesLayoutXmls(const std::vector<bpString>& aFileSeriesLayoutXml)
{
  if (!mImpl) {
    return;
  }

  auto vIt = aFileSeriesLayoutXml.begin();
  auto vEnd = aFileSeriesLayoutXml.end();
  std::vector<tFileLayout> vLayouts;
  for (; vIt != vEnd; ++vIt) {
    vLayouts.push_back(FileLayoutFromXMLString(*vIt));
  }
  mImpl->SeriesSetFileLayouts(vLayouts);
}


std::vector<bpString> bpFileReader::GetFileSeriesLayoutXmls()
{
  std::vector<bpString> vXmls;
  if (!mImpl) {
    return vXmls;
  }

  std::vector<tFileLayout> vFileLayouts = mImpl->SeriesGetFileLayouts();
  for (auto vFileLayoutsIt = vFileLayouts.begin(); vFileLayoutsIt != vFileLayouts.end(); ++vFileLayoutsIt) {
    vXmls.push_back(FileLayoutToXMLString(*vFileLayoutsIt));
  }
  return vXmls;
}


bpString bpFileReader::FileLayoutToXMLString(const tFileLayout& aLayout)
{
  std::ostringstream vLayoutXML;
  vLayoutXML << "<FileSeriesLayout>";
  for (const auto& vLayout : aLayout) {
    const auto& vXYZCT = vLayout.second;
    vLayoutXML << "\n<ImageIndex name=\"" << vLayout.first <<
      "\" x=\"" << vXYZCT[bpFileReaderImpl::X] <<
      "\" y=\"" << vXYZCT[bpFileReaderImpl::Y] <<
      "\" z=\"" << vXYZCT[bpFileReaderImpl::Z] <<
      "\" c=\"" << vXYZCT[bpFileReaderImpl::C] <<
      "\" t=\"" << vXYZCT[bpFileReaderImpl::T] << "\"/>";
  }
  vLayoutXML << "</FileSeriesLayout>";
  return vLayoutXML.str();
}


/*
<FileSeriesLayout>
<ImageIndex name="myName_T0_C0.tif" x="0" y="0" z="0" t="0" c="0"/>
<ImageIndex name="myName_T1_C0.tif" x="0" y="0" z="0" t="1" c="0"/>
<ImageIndex name="myName_T2_C0.tif" x="0" y="0" z="0" t="2" c="0"/>
<ImageIndex name="myName_T0_C1.tif" x="0" y="0" z="0" t="0" c="1"/>
<ImageIndex name="myName_T1_C1.tif" x="0" y="0" z="0" t="1" c="1"/>
<ImageIndex name="myName_T2_C1.tif" x="0" y="0" z="0" t="2" c="1"/>
</FileSeriesLayout>
*/
bpFileReader::tFileLayout bpFileReader::FileLayoutFromXMLString(const bpString& aLayoutString)
{
  bpFileReader::tFileLayout vLayout;
  if (aLayoutString.empty()) {
    return vLayout;
  }

  bpXmlTree vLayoutParser(aLayoutString, false, false);
  if (vLayoutParser.GetName() != "FileSeriesLayout") {
    return vLayout;
  }

  auto vTags = vLayoutParser.GetTags("ImageIndex");
  for (const auto& vTag : vTags) {
    bpFileReaderImpl::bpImageIndex vImageIndex;
    vImageIndex[bpFileReaderImpl::X] = bpFromString<bpSize>(vTag->GetAttr("x"));
    vImageIndex[bpFileReaderImpl::Y] = bpFromString<bpSize>(vTag->GetAttr("y"));
    vImageIndex[bpFileReaderImpl::Z] = bpFromString<bpSize>(vTag->GetAttr("z"));
    vImageIndex[bpFileReaderImpl::C] = bpFromString<bpSize>(vTag->GetAttr("c"));
    vImageIndex[bpFileReaderImpl::T] = bpFromString<bpSize>(vTag->GetAttr("t"));
    vLayout[vTag->GetAttr("name")] = vImageIndex;

  }
  return vLayout;
}
