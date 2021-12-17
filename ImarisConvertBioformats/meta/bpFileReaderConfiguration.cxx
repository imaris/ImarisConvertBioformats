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


#include "bpFileReaderConfiguration.h"


bpFileReaderConfiguration::bpFileReaderConfiguration()
: mColorHint(bpColorHint::eDefault),
  mMinLimits(5, 0),
  mMaxLimits(5, 0),
  mSample(5, 1)
{
}


bpFileReaderConfiguration::~bpFileReaderConfiguration()
{
}


void bpFileReaderConfiguration::SetResampleInterval(bpSize aSampleX,
                                       bpSize aSampleY,
                                       bpSize aSampleZ,
                                       bpSize aSampleC,
                                       bpSize aSampleT)
{
  mSample[0] = aSampleX;
  mSample[1] = aSampleY;
  mSample[2] = aSampleZ;
  mSample[3] = aSampleC;
  mSample[4] = aSampleT;
}


void bpFileReaderConfiguration::SetCropLimits(bpSize aMinX, bpSize aMaxX,
                                 bpSize aMinY, bpSize aMaxY,
                                 bpSize aMinZ, bpSize aMaxZ,
                                 bpSize aMinC, bpSize aMaxC,
                                 bpSize aMinT, bpSize aMaxT)
{

  mMinLimits[0] = aMinX;
  mMinLimits[1] = aMinY;
  mMinLimits[2] = aMinZ;
  mMinLimits[3] = aMinC;
  mMinLimits[4] = aMinT;

  mMaxLimits[0] = aMaxX;
  mMaxLimits[1] = aMaxY;
  mMaxLimits[2] = aMaxZ;
  mMaxLimits[3] = aMaxC;
  mMaxLimits[4] = aMaxT;

}

std::vector<bpSize> bpFileReaderConfiguration::GetCropLimitsMin()
{
  return mMinLimits;
}


std::vector<bpSize> bpFileReaderConfiguration::GetCropLimitsMax()
{
  return mMaxLimits;
}


std::vector<bpSize> bpFileReaderConfiguration::GetResampleInterval()
{
  return mSample;
}


void bpFileReaderConfiguration::SetForcedVoxelSize(const bpVector3Float& aVoxelSize)
{
  mForcedVoxelSize = aVoxelSize;
}


const bpVector3Float& bpFileReaderConfiguration::GetForcedVoxelSize() const
{
  return mForcedVoxelSize;
}


void bpFileReaderConfiguration::SetColorHint(bpColorHint aColorHint)
{
  mColorHint = aColorHint;
}


bpColorHint bpFileReaderConfiguration::GetColorHint() const
{
  return mColorHint;
}


void bpFileReaderConfiguration::SetDefaultColors(const bpFileConversionColors::tDefaultColors& aDefaultColors)
{
  mFileConversionColors.SetDefaultColors(aDefaultColors);
}


const bpFileConversionColors::tDefaultColors& bpFileReaderConfiguration::GetDefaultColors()
{
  return mFileConversionColors.GetDefaultColors();
}
