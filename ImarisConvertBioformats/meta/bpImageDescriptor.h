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


#ifndef __BP_IMAGEDESCRIPTOR_
#define __BP_IMAGEDESCRIPTOR_

#include "bpFileReaderImpl.h"
#include "bpFileTools.h"
#include "bpUtils.h"


#define BPIMAGEINDEXINDICATOR "#"


class bpConverterInfo
{
public:
  bpString mFileFormat;
  bpString mExecutable;
};


class bpImageDescriptor
{
public:
  typedef std::vector<bpString> tFileNames;

  bpImageDescriptor()
  {
  }

  /**
    * aFileName is of format /Volumes/development/data/FileFormats/LeicaLIF2/APDnew.lif#0
   */
  explicit bpImageDescriptor(const bpString& aFilename)
    : mDatasetName(aFilename),
      mBaseFileName(aFilename),
      mConverterInfo()
  {
  }

  bpImageDescriptor(const bpString& aFilename, const bpString& aImageName, const tFileNames& aDependencies = tFileNames())
    : mImageDependencies(aDependencies),
      mDatasetName(aImageName),
      mBaseFileName(aFilename),
      mConverterInfo()
  {
  }


  bpImageDescriptor RewriteFilenames(const bpString& aNewFilename) const
  {
    bpString vFilenameOnlyOld = bpFileTools::GetFileExt(mBaseFileName);
    bpString vFilePathNew = bpFileTools::GetPath(aNewFilename);
    if (vFilePathNew == bpFileTools::GetPath(mBaseFileName)) {
      return bpImageDescriptor(*this);
    }
    if (!vFilePathNew.empty()) {
      vFilePathNew = bpFileTools::AppendSeparator(vFilePathNew);
    }

    bpImageDescriptor vNewDescriptor;
    vNewDescriptor.mBaseFileName = vFilePathNew + vFilenameOnlyOld;
    vNewDescriptor.mImageIndex = mImageIndex;
    tFileNames::const_iterator vImageDependenciesIter = mImageDependencies.begin();
    for (; vImageDependenciesIter != mImageDependencies.end(); ++vImageDependenciesIter) {
      bpString vImageDepOnlyOld = bpFileTools::GetFileExt((*vImageDependenciesIter));
      bpString vRelativeSubPath = bpFileTools::GetPath(bpFileTools::GetRelativeFileName(mBaseFileName, *vImageDependenciesIter));
      if (!vRelativeSubPath.empty()) {
        vRelativeSubPath = bpFileTools::AppendSeparator(vRelativeSubPath);
      }
      vNewDescriptor.mImageDependencies.push_back(vFilePathNew + vRelativeSubPath + vImageDepOnlyOld);

    }

    std::map<bpString, bpFileReaderImpl::bpImageIndex>::const_iterator vSeriesIter = mFileSeriesLayout.begin();
    for (; vSeriesIter != mFileSeriesLayout.end(); ++vSeriesIter) {
      bpString vImageSeriesOnlyOld = bpFileTools::GetFileExt(vSeriesIter->first);
      vNewDescriptor.mFileSeriesLayout[vFilePathNew + vImageSeriesOnlyOld] = vSeriesIter->second;
    }
    return vNewDescriptor;
  }

  bpString ToString() const
  {
    bpString vString = "mBaseFileName=[" + mBaseFileName + "]";
    vString += ", mDatasetName=[" + mDatasetName + "]";
    vString += ", mImageIndex=[" + bpToString(mImageIndex) + "]";
    vString += ", mImageDependencies.size=[" + bpToString(mImageDependencies.size()) + "]";
    return vString;
  }

  bpString mBaseFileName; // => bpLocalFilename
  bpString mDatasetName;
  bpInt32 mImageIndex = -1;
  tFileNames mImageDependencies;
  std::map<bpString, bpFileReaderImpl::bpImageIndex> mFileSeriesLayout;
  bool mIsNative = false;
  bpConverterInfo mConverterInfo;

  // needed for the file converter in order to speed up adding files...
  bool mIsSeriesConfigurable = false;
  bool mIsSceneConfigurable = false;
  std::vector<bpFileReaderImpl::Dimension> mDimensionSequence;
  std::vector<bpSize> mDataSizeV;
  bpVector3Float mExtentMin;
  bpVector3Float mExtentMax;
  bpVector3Float mForcedVoxelSize;
};



#endif // __BP_IMAGEDESCRIPTOR_
