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


#ifndef __BP_IMAGE_DESCRIPTOR_FACTORY__
#define __BP_IMAGE_DESCRIPTOR_FACTORY__


#include "bpImageDescriptor.h"
#include "bpFileReader.h"


class bpImageDescriptorFactory
{
public:
  typedef std::pair<bpString, bpImageDescriptor> tFilenameDescriptor;
  typedef std::vector<tFilenameDescriptor> tFilesDescriptors;
  typedef std::vector<bpString> tFileNames;

  static tFilesDescriptors CreateImageDescriptors(const bpString& aFilename, const bpFileReader::tPtr& aFileReader);

private:
  using bpVector3Size = bpVec3;

  static bool IsNative(const bpFileReader::tImplPtr& aFileReader);
  static bool CanCreateMultiResolutionImageContainers(const std::vector<bpVector3Size>& aSizeXYZPerLevel);
};

#endif
