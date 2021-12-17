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


#ifndef __BP_FILE_READER_IMPL__
#define __BP_FILE_READER_IMPL__


#include "fileiobase/types/bpfTypes.h"
#include "fileiobase/types/bpfColorInfo.h"
#include "fileiobase/types/bpfTimeInfo.h"
#include "fileiobase/types/bpfSectionContainer.h"

#include "fileiobase/utils/bpfUtils.h"

class bpfHistogram;
class bpfPackedRGBA;

/**
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
 */


class bpfFileReaderImpl
{

protected:
  bpfFileReaderImpl(const bpfString& aFileName);

public:
  virtual ~bpfFileReaderImpl();

  const bpfString& GetFileName() const {
    return mFileName;
  }

  virtual std::vector<bpfString> GetAllFileNames() const = 0;

  virtual std::vector<bpfString> GetAllFileNamesOfDataSet(const bpfString& aFileName) const = 0;

  enum Dimension {
    X,
    Y,
    Z,
    C,
    T
  };

  class ImageIndex
  {
  public:

    typedef Dimension tDimension;

    ImageIndex();
    ImageIndex(const std::vector<tDimension>& aDimensionIncrementSequence,
               const std::vector<bpfSize>& aSize);
    ImageIndex(const ImageIndex& aOther);
    ~ImageIndex();
    ImageIndex& operator = (const ImageIndex& aOther);
    ImageIndex operator * (const ImageIndex& aOther);
    ImageIndex operator / (const ImageIndex& aOther);
    ImageIndex operator + (const ImageIndex& aOther);
    ImageIndex operator - (const ImageIndex& aOther);
    ImageIndex operator % (const ImageIndex& aOther);

    bpfSize& operator [] (tDimension aDimension);
    const bpfSize& operator [] (tDimension aDimension) const;

    bool operator == (const ImageIndex& aOther) const;
    bool operator != (const ImageIndex& aOther) const;

    bool operator < (const ImageIndex& aOther) const;
    bool operator > (const ImageIndex& aOther) const;

    std::vector<bpfSize> GetXYZCT() const;

  private:
    std::vector<bpfSize> mIndexVector;
  };

  /**
   * DataBase / multiple image Interface options
   */
  virtual bool CanMergeImages() const;
  virtual void SetMergeImages(bool aEnable);
  virtual bool GetMergeImages() const;

  /**
   * DataBase / multiple image Interface
   */
  virtual bpfSize GetNumberOfDataSets() const;
  virtual void SetActiveDataSetIndex(bpfSize aDataSetIndex);

  virtual bpfSize GetActiveDataSetIndex() const;

  /**
   * File Series Interface
   */
  virtual bool SeriesConfigurable() const;
  virtual bool SeriesInitialised() const;
  typedef std::map<bpfString, ImageIndex> tFileLayout;
  virtual void SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayout);
  virtual std::vector<tFileLayout> SeriesGetFileLayouts() const;
  virtual bool SeriesFileValid(const bpfString& aFileName) const;
  virtual bool SceneConfigurable() const;


  /**
   * Multiresolution Interface
   */
  virtual bpfSize GetNumberOfResolutions() const;
  virtual bpfSize GetActiveResolutionLevel() const;
  virtual void SetActiveResolutionLevel(bpfSize aResolutionLevel);

  /**
   * returns a description of the file reader (normally the static "GetDescription()")
   */
  virtual bpfString GetReaderDescription() const = 0;

  /**
   * returns a description of the file reader (normally the static "GetDescription()")
   */
  virtual std::vector<bpfString> GetReaderExtension() const = 0;

  /**
   * Get Type to which return values of ReadData should be cast.
   *
   * @return
   */
  virtual bpfNumberType GetDataType() = 0;

  /**
   *
   */
  virtual std::vector<Dimension> GetDimensionSequence() = 0;
  virtual bpfSize GetDimensionIndex(Dimension aDimension);

  virtual std::vector<bpfSize> GetDataSizeV() = 0;
  virtual bpfSize GetDataSize(Dimension aDimension);
  bpfSize GetDataSize(bpfSize aDimensionIndex) {
    return GetDataSize(GetDimensionSequence()[aDimensionIndex]);
  }
  virtual ImageIndex GetDataSize();


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
  virtual bool SetDataBlockSize(bpfSize aDimension0, bpfSize aDimension1, bpfSize aDimension2, bpfSize aDimension3, bpfSize aDimension4, bpfSize aResolutionLevel);

  /**
   * Size of datablock as vector in data coordinates
   */
  virtual std::vector<bpfSize> GetDataBlockSizeV() = 0;
  ImageIndex GetDataBlockSize();
  bpfSize GetDataBlockSize(Dimension aDimension);
  bpfSize GetDataBlockSize(bpfSize aDimensionOrder) {
    return GetDataBlockSize(GetDimensionSequence()[aDimensionOrder]);
  }

  /**
   * Total Size of datablock.
   * Returns the product of all GetDataBlockSizeV().
   */
  bpfSize GetDataBlockNumberOfVoxels();

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
  bpfSize GetDataBlockNumber(bpfSize x, bpfSize y, bpfSize z,
                             bpfSize aChnanel,
                             bpfSize aTimePoint);

  /**
   * GetNumberOfDataBlocks returns the total number datablocks
   */
  bpfSize GetNumberOfDataBlocks();

  /**
   * Returns extents of dataset in XYZCT coordinates
   */
  virtual void GetExtents(bpfVector3Float& aMin,
                          bpfVector3Float& aMax) = 0;

  /**
   * Returns extents and voxelsize of dataset in XYZ coordinates
   */
  virtual bpfVector3Float GetExtentMin() const;
  virtual bpfVector3Float GetExtentMax() const;
  virtual bpfVector3Float GetExtentDelta() const;

  /**
   * Read all the image parameters.
   *
   * @return The parameters from the file.
   */
  bpfSectionContainer ReadParameters();

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
  virtual bool ReadHistogram(bpfHistogram* aHistogram, bpfSize aTimePoint, bpfSize aChannel);


  /**
   * Reads the thumnbnail image.
   * returns false if there is no thumbnail
   */
  virtual bool ReadThumbnail(std::vector<bpfPackedRGBA>& aThumbnailPixels, bpfSize& aSizeX, bpfSize& aSizeY);

  virtual std::vector<bpfTimeInfo> ReadTimeInfo();
  virtual bpfTimeInfo ReadTimeInfo(bpfSize aTimePoint);

  virtual std::vector<bpfColorInfo> ReadColorInfo();
  virtual bpfColorInfo ReadColorInfo(bpfSize aChannel);

  virtual bool ShouldColorRangeBeAdjustedToMinMax();

  enum tColorHint {
    eColorHintDefault,
    eColorHintLUT,
    eColorHintEmission
  };

  static void SetColorHint(tColorHint aColorHint);
  static tColorHint GetColorHint();

  static void SetDefaultColor(bpfSize aChannelId, const bpfColor& aColor);
  static const bpfColor& GetDefaultColor(bpfSize aChannelId);

  static void SetOriginalFormatFileIOVersion(const bpfString& aVersion);

protected:
  static bpfString GuessUnit(const bpfString& aSomeUnit);
  virtual bpfSectionContainer ReadParametersImpl() = 0;

private:
  bpfString mFileName;
  static tColorHint mColorHint;
  static std::vector<bpfColor> mDefaultColor;

};

#endif // __BP_FILE_READER_Impl_2__

