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


#ifndef __BP_FILE_READER_IMPL_INTERFACE__
#define __BP_FILE_READER_IMPL_INTERFACE__

#if defined(_WIN32)
#include <windows.h>
#endif

/*
 * Base class for file readers.
 *
 * A file reader has the following basic properties:
 *
 *  1) It can read a datablock.
 *
 *  2) It can tell us the dimensions of a datablock.
 *
 *  3) It can tell us the dimensions of the whole dataset.
 *
 *  4) It can move to any datablock within the dataset.
 *
 *
 * The datablock that is read does not have to come with
 * a prescribed sequence of dimensions X, Y, Z, C, T.
 * The order of dimensions of the dataset can be querried
 * with the member function GetDimensionSequence() that returns
 * a vector of bpfFileReaderImpl::Dimension elements.
 *
 * The size of the dataset or a datablock can be querried
 * with the GetDataSizeV() and GetDataBlockSizeV() member
 * functions.
 * These return vectors that describe the size in terms of
 * the dataset coordinates, i.e. the first element describes
 * the size of the first dimension of GetDimensionSequence(), ...
 *
 * Some Utility functions are provided:
 *
 * TransformToXYZCT transforms a vector from data coordinates
 * to xyzct coordinates.
 *
 * GetDataBlockNumber returns the number of the datablock containing
 * a given coordinate.
 *
 * @author pm
 * @version 1.0
 * @since 20.1.2002
 */

#include "fileiobase/types/bpfTypedefs.h"

#include <vector>
#include <list>
#include <map>


class bpfFileReaderImplInterface
{

public:
  bpfFileReaderImplInterface() {
  }

  virtual ~bpfFileReaderImplInterface() {
  }

public:

  enum Dimension {
    X,
    Y,
    Z,
    C,
    T,
  };

  class ImageIndex
  {
  public:

    using tDimension = Dimension;

    ImageIndex() : mIndexVector(5) {}

    ImageIndex(const std::vector<tDimension>& aDimensionIncrementSequence, const std::vector<bpfSize>& aSize) : mIndexVector(5)
    {
      for (bpfSize vDim = 0; vDim < 5; ++vDim) {
        mIndexVector[aDimensionIncrementSequence[vDim]] = aSize[vDim];
      }
    }

    ImageIndex(const ImageIndex& aOther) : mIndexVector(aOther.mIndexVector) {}

    ~ImageIndex() {}

    bpfSize& operator [] (tDimension aDimension) { return mIndexVector[aDimension]; }

    const bpfSize& operator [] (tDimension aDimension) const { return mIndexVector[aDimension]; }

  private:

#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4251)
#endif

    std::vector<bpfSize> mIndexVector;

#if defined (_MSC_VER)
#pragma warning(pop)
#endif
  };

  virtual const bpfString& GetFileName() const = 0;

  virtual const std::vector<bpfString> GetAllFileNames() const = 0;

  virtual std::vector<bpfString> GetAllFileNamesOfDataSet(const bpfString& aFileName) const = 0;

  /**
   * DataBase / multiple image Interface options
   */
  virtual bool CanMergeImages() const = 0;
  virtual void SetMergeImages(bool aEnable) = 0;
  virtual bool GetMergeImages() const = 0;

  /**
   * DataBase / multiple image Interface
   */
  virtual size_t GetNumberOfImages() const = 0;
  virtual void SetActiveImageIndex(size_t aImageIndex) = 0;
  virtual size_t GetActiveImageIndex() const = 0;


  /**
   * File Series Interface
   */
  virtual bool SeriesConfigurable() const = 0;
  virtual bool SeriesInitialised() const = 0;

  using tFileLayout = std::map<bpfString, ImageIndex>;
  virtual void SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout) = 0;
  virtual std::vector<tFileLayout> SeriesGetFileLayouts() const = 0;
  virtual bool SeriesFileValid(const bpfString& aFileName) const = 0;
  virtual bool SceneConfigurable() const = 0;

  /**
   * Multiresolution
   */
  virtual bpfSize GetNumberOfResolutions() const = 0;
  virtual bpfSize GetActiveResolutionLevel() const = 0;
  virtual void SetActiveResolutionLevel(bpfSize aResolutionLevel) = 0;

  /**
   * description of format
   */
  virtual bpfString GetReaderDescription() = 0;

  /**
   * file extension
   */
  virtual std::vector<bpfString> GetReaderExtension() = 0;

  /**
   * Get Type to which return values of ReadData should be cast.
   */
  virtual bpfNumberType GetDataType() = 0;

  /**
   * Dimension Sequence of data in file
   */
  virtual std::vector<Dimension> GetDimensionSequence() = 0;
  virtual bpfSize GetDimensionIndex(Dimension aDimension) = 0;

  virtual ImageIndex GetDataSize() = 0;
  virtual bpfSize GetDataSize(Dimension aDimension) = 0;

  /**
   * Try to set the data block size to aDimension0, ...,
   * aDimension4. If the filereader does not "like" the new block
   * size false is returned and nothing changes.
   *
   * @param aDimension0
   * @param aDimension1
   * @param aDimension2
   * @param aDimension3
   * @param aDimension4
   * @param aResolutionLevel
   *
   * @return bool true if success
   */
  virtual bool SetDataBlockSize(bpfSize aDimension0,
                                bpfSize aDimension1,
                                bpfSize aDimension2,
                                bpfSize aDimension3,
                                bpfSize aDimension4,
                                bpfSize aResolutionLevel) = 0;

  /**
   * Size of datablock as vector in data coordinates
   */
  virtual ImageIndex GetDataBlockSize() = 0;
  virtual bpfSize GetDataBlockSize(Dimension aDimension) = 0;

  /**
   * Total Size of datablock.
   * Returns the product of all GetDataBlockSizeV().
   */
  virtual bpfSize GetDataBlockNumberOfVoxels() = 0;

  /**
   * Reads a block of data into given memory.
   *
   * Two consecutive calls to ReadDataBlock should
   * read two consecutive blocks.
   *
   */
  virtual void ReadDataBlock(void* aDataBlocknMemory) = 0;

  /**
   * Move to some datablock
   */
  virtual void GoToDataBlock(bpfSize aBlockNumber) = 0;
  virtual void GoToNextDataBlock() = 0;

  /**
   * GetDataBlockNumber returns the number of the datablock containing
   * a given coordinate.
   *
   * The coordinate is specified as x,y,z,c,t
   */
  virtual bpfSize GetDataBlockNumber(bpfSize aIndexX, bpfSize aIndexY, bpfSize aIndexZ,
                                     bpfSize aChannelIndex,
                                     bpfSize aTimeIndex) = 0;

  /**
   * GetNumberOfDataBlocks returns the total number datablocks
   */
  virtual bpfSize GetNumberOfDataBlocks() = 0;

  /**
   * Returns extents of dataset in XYZCT coordinates
   */
  virtual void GetExtents(float& aMinX, float& aMinY, float& aMinZ,
                          float& aMaxX, float& aMaxY, float& aMaxZ) = 0;


  /**
   * Read all the image parameters.
   *
   * @return The parameters from the file.
   */
  virtual std::map<std::string, std::map<std::string, std::string> > ReadParameters() = 0;

  /**
   * Read the histogram at aTimePoint, aChannel and
   * aResolutionIndex. a true is returned if successful, false if
   * not or not available.
   *
   * @param aHistogram the histogram to fill
   * @param aTimePoint the timepoint to look for
   * @param aChannel the channel to look for
   *
   * @return bool true if succesful
   */

  virtual bool ReadHistogram(bpfSize aTimePoint,
                             bpfSize aChannel,
                             std::vector<bpfUInt64>& aBins,
                             float& aMin, float& aMax) = 0;


  /**
   * Reads the thumnbnail image. Ith there is no thumbnail stored
   * in the file, a null pointer is returned.
   */
  struct cColor {
    bpfFloat mRed;
    bpfFloat mGreen;
    bpfFloat mBlue;
    bpfFloat mAlpha;
  };

  virtual bool ReadThumbnail(std::vector<cColor>& aThumbnail, bpfSize& aSizeX, bpfSize& aSizeY) = 0;

  virtual void ReadTimeInfo(bpfSize aTimePoint,
                            bpfInt32& aJulianDay,
                            bpfInt64& aNanosecondsOfDay) = 0;

  virtual void ReadColorInfo(bpfSize aChannelIndex,
                             bool& aMode,
                             cColor& aBaseColor,
                             std::vector<cColor>& aColorTable,
                             float& aOpacity,
                             float& aRangeMin,
                             float& aRangeMax,
                             float& aGammaCorrection) = 0;

  virtual bool ShouldColorRangeBeAdjustedToMinMax() = 0;

};

#endif

