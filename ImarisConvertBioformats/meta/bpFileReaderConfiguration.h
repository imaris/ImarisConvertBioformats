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


#ifndef __BP_FILE_READER_CONFIGURATION__
#define __BP_FILE_READER_CONFIGURATION__


#include "ImarisWriter/interface/bpConverterTypes.h"

#include "bpFileConversionColors.h"


class bpFileReaderConfiguration
{
public:
  /**
   * An enumeration type defining which color settings to use when reading the
   * file.
   */
  enum ColorHint {
    colorNoHint, colorLUTHint, colorEmissionHint, colorDefaultHint
  };

  bpFileReaderConfiguration();
  ~bpFileReaderConfiguration();

  /**
   * Sets the resample factor for all dimensions.
   */
  void SetResampleInterval(bpSize aSampleX,
                                   bpSize aSampleY,
                                   bpSize aSampleZ,
                                   bpSize aSampleC,
                                   bpSize aSampleT);

  std::vector<bpSize> GetResampleInterval();

  /**
   * Sets the cropping parameters
   */
  void SetCropLimits(bpSize aMinX, bpSize aMaxX,
                             bpSize aMinY, bpSize aMaxY,
                             bpSize aMinZ, bpSize aMaxZ,
                             bpSize aMinC, bpSize aMaxC,
                             bpSize aMinT, bpSize aMaxT);

  std::vector<bpSize> GetCropLimitsMin();
  std::vector<bpSize> GetCropLimitsMax();

  void SetForcedVoxelSize(const bpVector3Float& aVoxelSize);
  const bpVector3Float& GetForcedVoxelSize() const;

  void SetColorHint(bpColorHint aColorHint);
  bpColorHint GetColorHint() const;

  /**
  * Set all default colors to be used.  An arbitrary number of colors may be
  * specified.
  *
  * @param colors a vector containing the colors to be used.
  */
  void SetDefaultColors(const bpFileConversionColors::tDefaultColors& aDefaultColors);
  const bpFileConversionColors::tDefaultColors& GetDefaultColors();

private:
  std::vector<bpSize> mMinLimits;
  std::vector<bpSize> mMaxLimits;

  std::vector<bpSize> mSample;

  bpVector3Float mForcedVoxelSize;

  bpColorHint mColorHint;
  bpFileConversionColors mFileConversionColors;
};

#endif // __BP_FILE_READER__

