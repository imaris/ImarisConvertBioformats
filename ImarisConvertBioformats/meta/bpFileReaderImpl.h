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

#include "../src/bpWriterCommonHeaders.h"
#include "bpSectionContainer.h"

#include ICHistogram_h
#include ICThumbnail_h

#include "bpColorInfo.h"
#include "bpTimeInfo.h"
#include "bpFileConversionColors.h"
#include "bpNumberType.h"


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
 * a vector of bpFileReaderImpl::Dimension elements.
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
 * GetStartCoordinatesOfBlock returns the first coordinate of
 * the given block.
 *
 *
 * \ingroup persistence
 */
class bpFileReaderImpl
{

protected:
  bpFileReaderImpl(const bpString& aFileName);

public:
  using tPtr = bpSharedPtr<bpFileReaderImpl>;

  virtual ~bpFileReaderImpl();

  enum Dimension {
    X,
    Y,
    Z,
    C,
    T
  };



  class bpImageIndex
  {
  public:

    typedef Dimension tDimension;

    bpImageIndex();
    bpImageIndex(const std::vector<tDimension>& aDimensionIncrementSequence,
                 const std::vector<bpSize>& aSize);
    bpImageIndex(const bpImageIndex& aOther);
    ~bpImageIndex();
    bpImageIndex& operator = (const bpImageIndex& aOther);
    bpImageIndex operator * (const bpImageIndex& aOther);
    bpImageIndex operator / (const bpImageIndex& aOther);
    bpImageIndex operator + (const bpImageIndex& aOther);
    bpImageIndex operator - (const bpImageIndex& aOther);
    bpImageIndex operator % (const bpImageIndex& aOther);

    bpSize& operator [] (tDimension aDimension);
    const bpSize& operator [] (tDimension aDimension) const;

    bool operator == (const bpImageIndex& aOther) const;
    bool operator != (const bpImageIndex& aOther) const;

    bool operator < (const bpImageIndex& aOther) const;
    bool operator > (const bpImageIndex& aOther) const;

    std::vector<bpSize> GetXYZCT() const;

  private:
    std::vector<bpSize> mIndexVector;
  };

  const bpString& GetFileName() const;
  /**
   * Returns a vector containing names of files that will be read
   * when ReadFile is called with aFileName.
   *
   * @param aFileName
   * @return
   */
  virtual std::vector<bpString> GetAllFileNamesOfDataSet(const bpString& aFileName) const;

  /**
   * DataBase / multiple image Interface options
   */
  virtual bool CanMergeImages() const;
  virtual void SetMergeImages(bool aEnable);
  virtual bool GetMergeImages() const;

  /**
   * DataBase / multiple image Interface
   */
  virtual bpSize GetNumberOfDataSets() const;
  virtual void SetActiveDataSetIndex(bpSize aDataSetIndex);
  virtual bpSize GetActiveDataSetIndex() const;

  /**
   * File Series Interface
   */
  virtual bool SeriesConfigurable() const;
  virtual bool SeriesInitialised() const;
  typedef std::map<bpString, bpImageIndex> tFileLayout;
  virtual void SeriesSetFileLayouts(const std::vector<tFileLayout>& aLayouts);
  virtual std::vector<tFileLayout> SeriesGetFileLayouts() const;
  virtual bool SeriesFileValid(const bpString& aFileName) const;
  virtual bool SceneConfigurable() const;


  /**
   * Multiresolution Interface
   */
  virtual bpSize GetNumberOfResolutions() const;
  virtual bpSize GetActiveResolutionLevel() const;
  virtual void SetActiveResolutionLevel(bpSize aResolutionLevel);

  /**
   * returns a description of the file reader (normally the static "GetDescription()")
   */
  virtual bpString GetReaderDescription() const;

  /**
   * returns a description of the file reader (normally the static "GetDescription()")
   */
  virtual std::vector<bpString> GetReaderExtension() const = 0;

  /**
   * Get Type to which return values of ReadData should be cast.
   *
   * @return
   */
  virtual bpNumberType GetDataType() = 0;

  /**
   *
   */
  virtual std::vector<Dimension> GetDimensionSequence() = 0;
  virtual bpSize GetDimension(Dimension aDimension);

  virtual std::vector<bpSize> GetDataSizeV() = 0;
  bpImageIndex GetDataSize();
  bpSize GetDataSize(Dimension aDimension);
  bpSize GetDataSize(bpSize aDimensionOrder);


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
  virtual bool SetDataBlockSize(bpSize aDimension0, bpSize aDimension1, bpSize aDimension2, bpSize aDimension3, bpSize aDimension4, bpSize aResolutionLevel);

  /**
   * Size of datablock as vector in data coordinates
   */
  virtual std::vector<bpSize> GetDataBlockSizeV() = 0;
  bpImageIndex GetDataBlockSize();
  bpSize GetDataBlockSize(Dimension aDimension);
  bpSize GetDataBlockSize(bpSize aDimension);

  /**
   * Total Size of datablock.
   * Returns the product of all GetDataBlockSizeV().
   */
  bpSize GetDataBlockNumberOfVoxels();


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
  virtual void GoToDataBlock(bpSize aBlockNumber) = 0;
  virtual void GoToNextDataBlock() = 0;

  /**
   * GetDataBlockNumber returns the number of the datablock containing
   * a given coordinate.
   *
   * The coordinate is specified as x,y,z,c,t
   */
  bpSize GetDataBlockNumber(bpSize x, bpSize y, bpSize z,
                            bpSize aChnanel,
                            bpSize aTimePoint);

  /**
   * GetNumberOfDataBlocks returns the total number datablocks
   */
  bpSize GetNumberOfDataBlocks();

  /**
   * GetStartCoordinatesOfBlock returns the first coordinate of
   * the given block.
   *
   * The return value is a vector in dataset coordinates.
   */
  std::vector<bpSize> GetStartCoordinatesOfBlock(bpUInt32 aBlockNumber);

  /**
   *  Transform vector from data coordinates to xyzct coordinates
   */
  std::vector<bpSize> TransformToXYZCT(const std::vector<bpSize>& aVector);

  /**
   *  Transform vector from xyzct coordinates to data coordinates
   */
  std::vector<bpSize> TransformToData(const std::vector<bpSize>& aVector);

  /**
   * Returns extents of dataset in XYZCT coordinates
   */
  virtual void GetExtents(bpVector3Float& aMin,
                          bpVector3Float& aMax) = 0;

  /**
   * Returns extents and voxelsize of dataset in XYZ coordinates
   */
  virtual bpVector3Float GetExtentMin() const;
  virtual bpVector3Float GetExtentMax() const;
  virtual bpVector3Float GetExtentDelta() const;

  /**
   * Read all the image parameters.
   *
   * @return The parameters from the file.
   */
  virtual bpSectionContainer ReadParameters() = 0;

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
  virtual bool ReadHistogram(bpHistogram* aHistogram, bpSize aTimePoint, bpSize aChannel);

  /**
   * Reads the thumnbnail image. Ith there is no thumbnail stored
   * in the file, a null pointer is returned.
   */
  virtual bpSharedPtr<bpThumbnail> ReadThumbnail();

  virtual std::vector<bpTimeInfo> ReadTimeInfo();
  virtual bpTimeInfo ReadTimeInfo(bpSize aTimePoint);

  virtual std::vector<bpColorInfo> ReadColorInfo();
  virtual bpColorInfo ReadColorInfo(bpSize aChannel);

  virtual bool ShouldColorRangeBeAdjustedToMinMax();

  static void SetColorHint(bpColorHint aColorHint);
  static bpColorHint GetColorHint();

  static const bpColor& GetDefaultColor(bpSize aChannelId);
  static const bpFileConversionColors::tDefaultColors& GetDefaultColors();
  static void SetDefaultColors(const bpFileConversionColors::tDefaultColors& aDefaultColors);

  virtual bool NaNCheckStatus() const;

protected:
  static bpString GuessUnit(const bpString& aSomeUnit);

private:
  const bpString mFileName;
  static bpColorHint mColorHint;
  static bpFileConversionColors mFileConversionColors;

};

#endif // __BP_FILE_READER_Impl_2__

