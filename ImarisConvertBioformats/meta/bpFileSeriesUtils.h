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


#ifndef __BP_SERIES_LIST__
#define __BP_SERIES_LIST__


#include "bpFileReaderImpl.h"

#include <list>


class bpFileReaderSeriesFileCheck;


class bpSeriesList : public std::list<bpString>
{
public:
  bpSeriesList();
  bpSeriesList(const bpString& aPathName, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck = 0, bpSize aNCounters = 0);
  bpSeriesList(const bpString& aPathName, bool aMatchFileExtension, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck = 0);
  virtual ~bpSeriesList();

  /**
   * Set a path and a "master" file name
   */
  void SetPath(const bpString& aName);
  bpString GetPath() const;

  std::set<bpString> GetSet() const;

  /**
   * Set the number of numeric counters in the sequence.
   *
   * e.g. two counters:
   *
   * file_C00_Z00.tif
   * file_C00_Z01.tif
   * file_C00_Z02.tif
   * file_C00_Z03.tif
   * file_C01_Z00.tif
   * file_C01_Z01.tif
   * file_C01_Z02.tif
   * file_C01_Z03.tif
   *
   * the correspoding one counter sequence would be
   *
   * file_C00_Z00.tif
   * file_C00_Z01.tif
   * file_C00_Z02.tif
   * file_C00_Z03.tif
   */
  void SetNumberOfCounters(bpSize aNCounters);
  bpSize GetNumberOfCounters() const;

  /**
   *  From the structure of the filenames,
   *  make an intelligent guess at the series size
   */
  static void GuessSeriesSize(const std::vector<bpString>& aFileList,
                              bpSize& aNumberOfSlices,
                              bpSize& aNumberOfChannels,
                              bpSize& aNumberOfTimePoints);

  static void GuessSeriesSize(const std::vector<bpString>& aFileList,
                              bpSize& aSizeX,
                              bpSize& aSizeY,
                              bpSize& aSizeZ,
                              bpSize& aSizeC,
                              bpSize& aSizeT);

  /**
   * From the structure of the filenames,
   * make an intelligent guess at the series sorting
   *
   * The sorted list is returned.
   * The list will be sorted according to aOrderZCT
   *
   * @parameter aOrderZCT = "ZCT", "CZT", "CTZ", "TCZ", "TZC", "ZTC"
   */
  static std::vector<bpString> GuessSeriesSorting(const std::vector<bpString>& aFileList,
                                                  bpString aOrderZCT);

  /**
   * Calculates a sorted list of files, whereas the filenames are
   * parsed by known index prefixes "_x",  "_y",  "_z",  "_c", and
   * "_t" (case insensitive). If there is no prefix, the last 5
   * numbers in a filename are interpreted as indices for "t",
   * "c", "z", "y", and "x". If there are more than 5 numbers in a
   * filename, these additional indeces are also included as
   * sorting argument. The dimension sequence defines the
   * significance of a dimension for sorting.
   *
   * Performance measurement:
   * - 4'000 files => 0.247 s
   * - 32'000 files => 1.737 s
   * - 2'000'000 files => 112 s
   */
  static std::vector<bpString> GuessSeriesSorting(const std::vector<bpString>& aFileList,
                                                  const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence);

  static std::vector<bpString> GuessSeriesSorting(const std::vector<bpString>& aFileList);

  static void SortNumerical(std::vector<bpString>& aFileVector);
  static void SortNumerical(std::list<bpString>& aFileList);

private:

  bpString mPath;
  bpSize mNCounters;
  bool mMatchFileExtension;

  bpFileReaderSeriesFileCheck* mFileReaderSeriesFileCheck;

  virtual void SearchFiles();
  virtual void TestSeriesFileContents();

  static bool LessThan(const std::pair<bpString, std::vector<bpInt32> >& aValue1,
                       const std::pair<bpString, std::vector<bpInt32> >& aValue2);
  static std::pair<bpString, std::vector<bpInt32> > GetPair(const bpString& aString);

  class cFileName
  {
  public:
    cFileName(const bpString& aFileName);
    cFileName(const cFileName& aOther);
    bool Less(const cFileName& aOther, const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence) const;
    const bpString& GetFileName() const;
  private:
    typedef std::vector<bpSize> tDimensionIndices;
    typedef std::vector<bpSize> tOtherIndices;
    bool Pick(const bpString& aPrefixA,
              const bpString& aPrefixB,
              bpSize aNumber,
              bpFileReaderImpl::Dimension aDimension);
    void MoveFirstOtherIndexToDimension(bpFileReaderImpl::Dimension aDimension);
    bpString mFileName;
    tDimensionIndices mDimensionIndices;
    tOtherIndices mOtherIndices;
  };

  class cFileLess
  {
  public:
    cFileLess(const std::vector<cFileName>& aFileNames,
              const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence);
    bool operator () (bpSize aFileNameIndexL, bpSize aFileNameIndexR) const;
  private:
    const std::vector<cFileName>& mFileNames;
    const std::vector<bpFileReaderImpl::Dimension>& mDimensionSequence;
  };

  static bool GetNumber(const bpString& aString, bpSize aPos, bpSize& aNumber);
  static bool GetNumber(const bpString& aString, const bpString& aDelimitorA, bpSize& aNumber);
  static bool GetNumber(const bpString& aString, const bpString& aDelimitorA, const bpString& aDelimitorB, bpSize& aNumber);
  static bool CountNumber(const bpString& aString, const bpString& aDelimitorA, std::set<bpSize>& aCount);
  static bool CountNumber(const bpString& aString, const bpString& aDelimitorA, const bpString& aDelimitorB, std::set<bpSize>& aCount);
  static bool CountNumber(const bpString& aString, const bpString& aDelimitorA, const bpString& aDelimitorB, const bpString& aDelimitorC, std::set<bpSize>& aCount);
  static void AdjustSizes(bpSize& aSizeA, bpSize& aSizeB, bpSize& aSizeC, const bpSize aSizeABC);
  static void AdjustSizes(bpSize& aSizeA, bpSize& aSizeB, bpSize& aSizeC, bpSize& aSizeD, bpSize& aSizeE, const bpSize aSizeABCDE);


};


#endif // __BP_SERIES_LIST__

