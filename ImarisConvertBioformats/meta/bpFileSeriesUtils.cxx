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


#include "bpFileSeriesUtils.h"
#include "bpUtils.h"
#include "bpFileTools.h"
#include "bpFileReaderSeriesFileCheck.h"

#include <set>
#include <map>

bpSeriesList::bpSeriesList()
{
}

bpSeriesList::bpSeriesList(const bpString& aPathName, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck, bpSize aNCounters)
  : mPath(aPathName),
    mNCounters(aNCounters),
    mMatchFileExtension(true),
    mFileReaderSeriesFileCheck(aFileReaderSeriesFileCheck)
{
  SearchFiles();
  TestSeriesFileContents();
}

bpSeriesList::bpSeriesList(const bpString& aPathName, bool aMatchFileExtension, bpFileReaderSeriesFileCheck* aFileReaderSeriesFileCheck)
  : mPath(aPathName),
    mNCounters(1),
    mMatchFileExtension(aMatchFileExtension),
    mFileReaderSeriesFileCheck(aFileReaderSeriesFileCheck)
{
  SearchFiles();
  TestSeriesFileContents();
}

bpSeriesList::~bpSeriesList()
{
}

void bpSeriesList::SetPath(const bpString& aPath)
{
  mPath = aPath;
  clear();
  SearchFiles();
}

bpString bpSeriesList::GetPath() const
{
  return mPath;
}

void bpSeriesList::SetNumberOfCounters(bpSize aNCounters)
{
  mNCounters = aNCounters;
}

bpSize bpSeriesList::GetNumberOfCounters() const
{
  return mNCounters;
}


std::set<bpString> bpSeriesList::GetSet() const
{
  std::set<bpString> vSet;
  std::list<bpString>::const_iterator vListIter = this->begin();
  for (; vListIter != this->end(); ++vListIter) {
    vSet.insert(*vListIter);
  }
  return vSet;
}


void bpSplit(const bpString& aString, std::vector<bpString>& aNonNumeric, std::vector<bpString>& aNumeric)
{
  bpString vString(aString);
  bpSize vPos = 0;

  while (!vString.empty()) {
    // split a non numeric part
    vPos = vString.find_first_of("0123456789");
    aNonNumeric.push_back(vString.substr(0, vPos));
    vString.erase(0, vPos);
    // split a numeric part
    vPos = vString.find_first_not_of("0123456789");
    aNumeric.push_back(vString.substr(0, vPos));
    vString.erase(0, vPos);
  }
}


void bpSplit(const bpString& aString, std::vector<bpString>& aNonNumeric, std::vector<bpInt32>& aNumeric)
{
  std::vector<bpString> vNumericString;
  bpSplit(aString, aNonNumeric, vNumericString);
  aNumeric.resize(vNumericString.size());
  for (bpSize vIndex = 0; vIndex < vNumericString.size(); vIndex++) {
    bpFromString(vNumericString[vIndex], aNumeric[vIndex]);
  }
}


// returns "aValue1 < aValue2" ...
bool bpSeriesList::LessThan(const std::pair<bpString, std::vector<bpInt32> >& aValue1,
                            const std::pair<bpString, std::vector<bpInt32> >& aValue2)
{
  const std::vector<bpInt32>& vValue1 = aValue1.second;
  const std::vector<bpInt32>& vValue2 = aValue2.second;
  if (vValue1.size() == vValue2.size()) {
    for (bpSize vIndex = 0; vIndex < vValue1.size(); vIndex++) {
      if (vValue1[vIndex] < vValue2[vIndex]) {
        return true;
      }
      if (vValue1[vIndex] > vValue2[vIndex]) {
        return false;
      }
    }
    return false;
  }
  else {
    return vValue1.size() < vValue2.size();
  }
}


std::pair<bpString, std::vector<bpInt32> > bpSeriesList::GetPair(const bpString& aString)
{
  return std::make_pair(aString, bpSplitNumericInt32(aString));
}


//
// Fuellt die Klasseninstanz mit der zum File gehoerenden Serie
//

void bpSeriesList::SearchFiles()
{

  //
  // split path into directory, basename, and extension
  //
  bpString vDirectory;
  bpString vFileName;
  bpString vBaseName;
  bpString vExtension;

  bpSize vPos = static_cast<bpSize>(bpFindLastSeparator(GetPath())) + 1;
  bpSize vPos2 = GetPath().size() - vPos;
  vDirectory = GetPath().substr(0, vPos);
  vFileName = GetPath().substr(vPos, vPos2);
  vPos = vFileName.find_last_of(".");
  vBaseName = vFileName.substr(0,vPos);
  vExtension = vFileName;
  vExtension.erase(0,vPos+1);



  //
  // split basename into blocks that are numeric or not
  //
  std::vector<bpString> vBlocksNonNumeric = bpSplitNonNumeric(vBaseName);
  std::vector<bpString> vBlocksNumeric = bpSplitNumeric(vBaseName);

  // Generate numeric and non-numeric blocks at once.
  // The lists have the same size. If the name consists only of a number, the non numeric entry is ""
  // bpSplit(vBaseName, vBlocksNonNumeric, vBlocksNumeric);

  bpSize vBlockNumber = vBlocksNonNumeric.size();
  bpSize vBlockNumberNumeric = vBlocksNumeric.size();

  if (vBlockNumber == 0) {
    push_back(GetPath());
    return;
  }


  //
  // get all files in directory
  //
  std::list<bpString> vDirList = bpFileTools::GetFilesOfDirectory(vDirectory);



  //
  // loop over directory
  //
  std::list<bpString>::iterator vIt = vDirList.begin();
  std::list<bpString>::iterator vEnd = vDirList.end();

  while (vIt != vEnd) {

    bpString vFile = *vIt;

    // compare filename with basename
    // all nonnumeric blocks should match!
    // all but the last aNCounter numeric blocks should match as well
    bool vMatch = true;

    bpString vFileBaseName;
    bpString vFileExtension;
    bpString vFileExtensionUpperCase;

    vPos = vFile.find_last_of(".");
    vFileBaseName = vFile.substr(0,vPos);
    vFileExtension = vFile;
    vFileExtension.erase(0,vPos+1);

    // convert into upper case ...
    vFileExtensionUpperCase = bpToUpper(vFileExtension);
    if (!mMatchFileExtension ||
        (vFileExtension == vExtension) ||
        (vFileExtensionUpperCase == vExtension)) {

      std::vector<bpString> vFileBlocks = bpSplitNonNumeric(vFileBaseName);
      std::vector<bpString> vFileBlocksNumeric = bpSplitNumeric(vFileBaseName);

      // bpSplit(vFileBaseName, vFileBlocks, vFileBlocksNumeric);
      if (vFileBlocks.size() == vBlockNumber) {

        // compare all nonnumeric blocks
        for (bpSize vCount = 0; vCount < vBlockNumber; vCount++) {
          if (vFileBlocks[vCount] != vBlocksNonNumeric[vCount] && bpToUpper(vFileBlocks[vCount]) != vBlocksNonNumeric[vCount]) {
            vMatch = false;
            break;
          }

        }


      }
      else {
        vMatch = false;
      }


      if (vFileBlocksNumeric.size() == vBlockNumberNumeric) {

        // compare all but last aNCounters numeric blocks
        if (vBlockNumberNumeric > GetNumberOfCounters() && GetNumberOfCounters() > 0) {
          for (bpSize vCount = 0; vCount < (vBlockNumberNumeric - GetNumberOfCounters()); vCount++) {
            if (vFileBlocksNumeric[vCount] != vBlocksNumeric[vCount]) {
              vMatch = false;
              break;
            }
          }
        }

      }
      else {
        vMatch = false;
      }

    }
    else {
      vMatch = false;
    }

    if (vMatch) {
      // add file to list

      // insert path:
      bpString vPath = vDirectory + *vIt;
      push_back(vPath);
    }

    ++vIt;

  } // end of directory loop

  //
  // sort numerically:
  //
  SortNumerical(*this);
}


void bpSeriesList::TestSeriesFileContents()
{
  if (!mFileReaderSeriesFileCheck || this->size() > mFileReaderSeriesFileCheck->GetMaxSeriesSizeToCheck()) {
    return;
  }

  bpSeriesList vValidFiles;
  bpSeriesList::const_iterator vListIter = this->begin();
  for (; vListIter != this->end(); ++vListIter) {
    if (mFileReaderSeriesFileCheck->SeriesFileValid(*vListIter)) {
      vValidFiles.push_back(*vListIter);
    }
  }
  this->swap(vValidFiles);
}


void bpSeriesList::SortNumerical(std::vector<bpString>& aFileVector)
{
  std::list<bpString> vFileList;
  for (bpSize vIndex = 0; vIndex < aFileVector.size(); vIndex++) {
    vFileList.push_back(aFileVector[vIndex]);
  }
  vFileList.sort();
  SortNumerical(vFileList);
  aFileVector.clear();
  std::list<bpString>::iterator vFileListPos = vFileList.begin();
  while (vFileListPos != vFileList.end()) {
    aFileVector.push_back(*vFileListPos);
    ++vFileListPos;
  }
}


void bpSeriesList::SortNumerical(std::list<bpString>& aFileList)
{
  std::vector<std::pair<bpString, std::vector<bpInt32> > > vNameVector;

  // create vector with pairs of filename and numbers
  std::list<bpString>::iterator vFileListPos = aFileList.begin();
  while (vFileListPos != aFileList.end()) {
    vNameVector.push_back(GetPair(*vFileListPos));
    ++vFileListPos;
  }

  // sort the vector
  std::sort(vNameVector.begin(), vNameVector.end(), LessThan);

  // fill back into own list
  vFileListPos = aFileList.begin();
  for (bpSize vIndex = 0; vIndex < vNameVector.size(); vIndex++) {
    *vFileListPos = vNameVector[vIndex].first;
    ++vFileListPos;
  }
}


bool bpSeriesList::GetNumber(const bpString& aString, bpSize aPos, bpSize& aNumber)
{
  if (bpIsNumber(aString[aPos])) {
    bpSize vPos = aString.find_first_not_of("0123456789", aPos);
    if (aPos < vPos) {
      bpFromString(aString.substr(aPos, vPos - aPos), aNumber);
    }
    else {
      bpFromString(aString.substr(aPos), aNumber);
    }
    return true;
  }
  return false;
}


bool bpSeriesList::GetNumber(const bpString& aString,
                             const bpString& aDelimitorA,
                             bpSize& aNumber)
{
  bpSize vPos = aString.rfind(aDelimitorA);
  if (vPos != bpString::npos) {
    bpSize vNumber = 0;
    if (GetNumber(aString, vPos + aDelimitorA.length(), vNumber)) {
      aNumber = vNumber;
      return true;
    }
  }
  return false;
}


bool bpSeriesList::GetNumber(const bpString& aString,
                             const bpString& aDelimitorA,
                             const bpString& aDelimitorB,
                             bpSize& aNumber)
{
  if (!GetNumber(aString, aDelimitorA, aNumber)) {
    return true;
  }
  if (GetNumber(aString, aDelimitorB, aNumber)) {
    return true;
  }
  return false;
}


bool bpSeriesList::CountNumber(const bpString& aString,
                               const bpString& aDelimitorA,
                               std::set<bpSize>& aCount)
{
  bpSize vPos = aString.rfind(aDelimitorA);
  if (vPos != bpString::npos) {
    bpSize vNumber = 0;
    if (GetNumber(aString, vPos + aDelimitorA.length(), vNumber)) {
      aCount.insert(vNumber);
      return true;
    }
  }
  return false;
}


bool bpSeriesList::CountNumber(const bpString& aString,
                               const bpString& aDelimitorA,
                               const bpString& aDelimitorB,
                               std::set<bpSize>& aCount)
{
  if (CountNumber(aString, aDelimitorA, aCount)) {
    return true;
  }
  if (CountNumber(aString, aDelimitorB, aCount)) {
    return true;
  }
  return false;
}


bool bpSeriesList::CountNumber(const bpString& aString,
                               const bpString& aDelimitorA,
                               const bpString& aDelimitorB,
                               const bpString& aDelimitorC,
                               std::set<bpSize>& aCount)
{
  if (CountNumber(aString, aDelimitorA, aCount)) {
    return true;
  }
  if (CountNumber(aString, aDelimitorB, aCount)) {
    return true;
  }
  if (CountNumber(aString, aDelimitorC, aCount)) {
    return true;
  }
  return false;
}


void bpSeriesList::AdjustSizes(bpSize& aSizeA, bpSize& aSizeB, bpSize& aSizeC, const bpSize aSizeABC)
{
  if (aSizeA == 0) {
    if (aSizeB == 0) {
      aSizeB = 1;
    }
    if (aSizeC == 0) {
      aSizeC = 1;
    }
    aSizeA = aSizeABC / (aSizeB * aSizeC);
  }
}



void bpSeriesList::AdjustSizes(bpSize& aSizeA,
                               bpSize& aSizeB,
                               bpSize& aSizeC,
                               bpSize& aSizeD,
                               bpSize& aSizeE,
                               const bpSize aSizeABCDE)
{
  if (aSizeA == 0) {
    if (aSizeB == 0) {
      aSizeB = 1;
    }
    if (aSizeC == 0) {
      aSizeC = 1;
    }
    if (aSizeD == 0) {
      aSizeD = 1;
    }
    if (aSizeE == 0) {
      aSizeE = 1;
    }
    aSizeA = aSizeABCDE / (aSizeB * aSizeC * aSizeD * aSizeE);
  }
}



/**
 * parse file names to guess dimensions
 */
void bpSeriesList::GuessSeriesSize(const std::vector<bpString>& aFileList,
                                   bpSize& aNumberOfSlices,
                                   bpSize& aNumberOfChannels,
                                   bpSize& aNumberOfTimePoints)
{
  if (aFileList.size() == 0) {
    return;
  }

  std::set<bpSize> vInicesZ;
  std::set<bpSize> vInicesC;
  std::set<bpSize> vInicesT;

  for (bpSize vIndex = 0; vIndex < aFileList.size(); vIndex++) {

    bpString vFileName = bpToUpper(bpFileTools::GetFile(aFileList[vIndex]));

    CountNumber(vFileName, "_Z", vInicesZ);
    CountNumber(vFileName, "_C", "_CH", vInicesC);
    CountNumber(vFileName, "_T", "_TP", vInicesT);
  }

  bpSize vSizeZ = vInicesZ.size();
  bpSize vSizeC = vInicesC.size();
  bpSize vSizeT = vInicesT.size();

  AdjustSizes(vSizeZ, vSizeC, vSizeT, aFileList.size());
  AdjustSizes(vSizeT, vSizeZ, vSizeC, aFileList.size());
  AdjustSizes(vSizeC, vSizeT, vSizeZ, aFileList.size());

  if (vSizeZ * vSizeC * vSizeT == aFileList.size()) {
    aNumberOfSlices = vSizeZ;
    aNumberOfChannels = vSizeC;
    aNumberOfTimePoints = vSizeT;
  }
}


void bpSeriesList::GuessSeriesSize(const std::vector<bpString>& aFileList,
                                   bpSize& aSizeX,
                                   bpSize& aSizeY,
                                   bpSize& aSizeZ,
                                   bpSize& aSizeC,
                                   bpSize& aSizeT)
{
  if (aFileList.size() == 0) {
    return;
  }

  std::set<bpSize> vInicesY;
  std::set<bpSize> vInicesX;
  std::set<bpSize> vInicesZ;
  std::set<bpSize> vInicesC;
  std::set<bpSize> vInicesT;

  for (bpSize vIndex = 0; vIndex < aFileList.size(); vIndex++) {
    bpString vFileName = bpToUpper(bpFileTools::GetFile(aFileList[vIndex]));
    CountNumber(vFileName, "_X", vInicesX);
    CountNumber(vFileName, "_Y", vInicesY);
    CountNumber(vFileName, "_Z", vInicesZ);
    CountNumber(vFileName, "_C", "_CH", vInicesC);
    CountNumber(vFileName, "_T", "_TP", vInicesT);
  }

  bpSize vSizeX = vInicesX.size();
  bpSize vSizeY = vInicesY.size();
  bpSize vSizeZ = vInicesZ.size();
  bpSize vSizeC = vInicesC.size();
  bpSize vSizeT = vInicesT.size();

  AdjustSizes(vSizeX, vSizeY, vSizeZ, vSizeC, vSizeT, aFileList.size());
  AdjustSizes(vSizeY, vSizeX, vSizeZ, vSizeC, vSizeT, aFileList.size());
  AdjustSizes(vSizeZ, vSizeX, vSizeY, vSizeC, vSizeT, aFileList.size());
  AdjustSizes(vSizeC, vSizeX, vSizeY, vSizeZ, vSizeT, aFileList.size());
  AdjustSizes(vSizeT, vSizeX, vSizeY, vSizeZ, vSizeC, aFileList.size());

  if (vSizeX * vSizeY * vSizeZ * vSizeC * vSizeT == aFileList.size()) {
    aSizeX = vSizeX;
    aSizeY = vSizeY;
    aSizeZ = vSizeZ;
    aSizeC = vSizeC;
    aSizeT = vSizeT;
  }
}


std::vector<bpString> bpSeriesList::GuessSeriesSorting(const std::vector<bpString>& aFileList,
                                                       bpString aOrderZCT)

{

  bpString vOrder = bpToUpper(aOrderZCT);
  // default "ZCT"
  if (vOrder != "ZCT"  &&
      vOrder != "CZT"  &&
      vOrder != "TCZ"  &&
      vOrder != "CTZ"  &&
      vOrder != "TZC"  &&
      vOrder != "ZTC" ) {

    vOrder = "ZCT";
  }


  if (aFileList.size() == 0) {
    return aFileList;
  }

  bpString vFileName = aFileList[0];
  bpString vBaseName = vFileName.substr(0,vFileName.find_last_of("."));

  // by default assume that
  // the last number in the file name is the slice,
  // the second to last is the channel
  // and the third to last is the time point

  bpInt32 vPosT = 1;
  bpInt32 vPosC = 2;
  bpInt32 vPosZ = 3;

  //
  // if we find specific dimension delimiters
  // revise sequence of dimensions
  //
  bpString vBaseNameUpper = bpToUpper(vBaseName);

  bpInt32 vPositionZ = static_cast<bpInt32>(vBaseNameUpper.find("_Z"));
  if (!bpIsNumber(vBaseNameUpper[static_cast<bpSize>(vPositionZ)+2])) {
    vPositionZ = -1;
  }

  bpInt32 vPositionC = static_cast<bpInt32>(vBaseNameUpper.find("_C"));
  if (!(bpIsNumber(vBaseNameUpper[static_cast<bpSize>(vPositionC)+2]) ||
        bpIsNumber(vBaseNameUpper[static_cast<bpSize>(vPositionC)+3])) ) {
    vPositionC = -1;
  }

  bpInt32 vPositionT = static_cast<bpInt32>(vBaseNameUpper.find("_T"));
  if (!bpIsNumber(vBaseNameUpper[static_cast<bpSize>(vPositionT)+2])) {
    vPositionT = -1;
  }

  if ((vPositionZ < vPositionT && vPosZ > vPosT)) {
    bpInt32 tmp = vPosT;
    vPosT = vPosZ;
    vPosZ = tmp;
  }
  if ((vPositionZ < vPositionC && vPosZ > vPosC)) {
    bpInt32 tmp = vPosC;
    vPosC = vPosZ;
    vPosZ = tmp;
  }
  if ((vPositionT < vPositionC && vPosT > vPosC)) {
    bpInt32 tmp = vPosC;
    vPosC = vPosT;
    vPosT = tmp;
  }

  vPosT = 3 - vPosT;
  vPosC = 3 - vPosC;
  vPosZ = 3 - vPosZ;


  //
  // sort file list to order given by GetDimension:
  //

  std::vector<bpString> vSortedFileList = aFileList;

  bpSize vNumberOfTimePoints = 1;
  bpSize vNumberOfChannels = 1;
  bpSize vNumberOfSlices = vSortedFileList.size();
  GuessSeriesSize(aFileList, vNumberOfSlices, vNumberOfChannels, vNumberOfTimePoints);


  bpSize vSize[3];
  vSize[vPosC] = vNumberOfChannels;
  vSize[vPosZ] = vNumberOfSlices;
  vSize[vPosT] = vNumberOfTimePoints;

  bpSize vIndex[3] = {0,0,0};
  bpSize vIndexSorted = 0;
  bpSize vIndexUnSorted = 0;
  bpSize vIndex0;
  bpSize vIndex1;
  bpSize vIndex2;

  // loop over files in unsorted list
  for (vIndex2 = 0; vIndex2 < vSize[2]; vIndex2++) {
    vIndex[2] = vIndex2;
    for (vIndex1 = 0; vIndex1 < vSize[1]; vIndex1++) {
      vIndex[1] = vIndex1;
      for (vIndex0 = 0; vIndex0 < vSize[0]; vIndex0++) {
        vIndex[0] = vIndex0;

        if (vOrder == "CZT") {

          vIndexSorted = vIndex[vPosT] * vNumberOfSlices * vNumberOfChannels +
                         vIndex[vPosZ] * vNumberOfChannels + vIndex[vPosC];

        }
        if (vOrder == "ZCT") {

          vIndexSorted = vIndex[vPosT] * vNumberOfSlices * vNumberOfChannels +
                         vIndex[vPosC] * vNumberOfSlices + vIndex[vPosZ];

        }
        if (vOrder == "CTZ") {

          vIndexSorted = vIndex[vPosZ] * vNumberOfTimePoints * vNumberOfChannels +
                         vIndex[vPosT] * vNumberOfChannels + vIndex[vPosC];

        }
        if (vOrder == "TCZ") {

          vIndexSorted = vIndex[vPosZ] * vNumberOfTimePoints * vNumberOfChannels +
                         vIndex[vPosC] * vNumberOfTimePoints + vIndex[vPosT];

        }
        if (vOrder == "ZTC") {

          vIndexSorted = vIndex[vPosC] * vNumberOfSlices * vNumberOfTimePoints +
                         vIndex[vPosT] * vNumberOfSlices + vIndex[vPosZ];

        }
        if (vOrder == "TZC") {

          vIndexSorted = vIndex[vPosC] * vNumberOfSlices * vNumberOfTimePoints +
                         vIndex[vPosZ] * vNumberOfTimePoints + vIndex[vPosT];

        }

        vSortedFileList[vIndexSorted] = aFileList[vIndexUnSorted];

        vIndexUnSorted++;
      }
    }
  }


  return vSortedFileList;
}


bpSeriesList::cFileName::cFileName(const cFileName& aOther)
  : mFileName(aOther.mFileName),
    mDimensionIndices(aOther.mDimensionIndices),
    mOtherIndices(aOther.mOtherIndices)
{
}


bpSeriesList::cFileName::cFileName(const bpString& aFileName)
  : mFileName(aFileName),
    mDimensionIndices(5)
{
  // set dimension indices to undefined value
  mDimensionIndices[0] = (bpSize)-1;
  mDimensionIndices[1] = (bpSize)-1;
  mDimensionIndices[2] = (bpSize)-1;
  mDimensionIndices[3] = (bpSize)-1;
  mDimensionIndices[4] = (bpSize)-1;

  // translate name to upper case
  bpString vFileName = bpToUpper(bpFileTools::GetFile(aFileName));

  // Make pairs with strings and numbers
  std::vector<bpString> vPrefixes;
  std::vector<bpSize> vNumbers;
  if (!vFileName.empty()) {
    bpSize vPos = 0;
    bpSize vPosBegin = 0;
    while (vPos < vFileName.size()) {
      while (!bpIsNumber(vFileName[vPos]) && (vPos < vFileName.size())) vPos++;
      vPrefixes.push_back(vFileName.substr(vPosBegin, vPos-vPosBegin));
      vPosBegin = vPos;
      while (bpIsNumber(vFileName[vPos]) && (vPos < vFileName.size())) vPos++;
      vNumbers.push_back(bpFromString<bpSize>(vFileName.substr(vPosBegin, vPos-vPosBegin)));
      vPosBegin = vPos;
    }
  }

  // assign numbers as indixes
  while (!vPrefixes.empty()) {
    // pick numbers with known prefixes
    if (!Pick("_X",  *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::X) &&
        !Pick("_Y",  *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::Y) &&
        !Pick("_Z",  *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::Z) &&
        !Pick("_C",  *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::C) &&
        !Pick("_CH", *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::C) &&
        !Pick("_T",  *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::T) &&
        !Pick("_TP", *vPrefixes.rbegin(), *vNumbers.rbegin(), bpFileReaderImpl::T)) {
      // unknown prefix
      mOtherIndices.push_back(*vNumbers.rbegin());
    }
    vPrefixes.erase(vPrefixes.begin() + vPrefixes.size() - 1);
    vNumbers.erase(vNumbers.begin() + vNumbers.size() - 1);
  }

  // try to assign an unknown index to a known dimension
  MoveFirstOtherIndexToDimension(bpFileReaderImpl::X);
  MoveFirstOtherIndexToDimension(bpFileReaderImpl::Y);
  MoveFirstOtherIndexToDimension(bpFileReaderImpl::Z);
  MoveFirstOtherIndexToDimension(bpFileReaderImpl::C);
  MoveFirstOtherIndexToDimension(bpFileReaderImpl::T);
}


bool bpSeriesList::cFileName::Pick(const bpString& aPrefixA,
                                   const bpString& aPrefixB,
                                   bpSize aNumber,
                                   bpFileReaderImpl::Dimension aDimension)
{
  if (aPrefixA == aPrefixB) {
    if (static_cast<bpSize>(aDimension) < mDimensionIndices.size()) {
      if (mDimensionIndices[aDimension] == (bpSize)-1) {
        mDimensionIndices[aDimension] = aNumber;
        return true;
      }
    }
  }
  return false;
}


void bpSeriesList::cFileName::MoveFirstOtherIndexToDimension(bpFileReaderImpl::Dimension aDimension)
{
  if (static_cast<bpSize>(aDimension) < mDimensionIndices.size()) {
    if (mDimensionIndices[aDimension] == (bpSize)-1) {
      if (!mOtherIndices.empty()) {
        mDimensionIndices[aDimension] = *mOtherIndices.begin();
        mOtherIndices.erase(mOtherIndices.begin());
      }
    }
  }
}


bool bpSeriesList::cFileName::Less(const cFileName& aOther,
                                   const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence) const
{
  // compare indices with known dimension
  std::vector<bpFileReaderImpl::Dimension>::const_reverse_iterator vDimensionIterator = aDimensionSequence.rbegin();
  while (vDimensionIterator != aDimensionSequence.rend()) {
    bpFileReaderImpl::Dimension vDimension = *vDimensionIterator;
    if ((static_cast<bpSize>(vDimension) < mDimensionIndices.size()) &&
        (static_cast<bpSize>(vDimension) < aOther.mDimensionIndices.size())) {
      if ((mDimensionIndices[vDimension] != (bpSize)-1) &&
          (aOther.mDimensionIndices[vDimension] != (bpSize)-1)) {
        if (mDimensionIndices[vDimension] != aOther.mDimensionIndices[vDimension]) {
          return mDimensionIndices[vDimension] < aOther.mDimensionIndices[vDimension];
        }
      }
    }
    ++vDimensionIterator;
  }

  // compare size of indices with unknown dimension
  if (mOtherIndices.size() != aOther.mOtherIndices.size()) {
    return mOtherIndices.size() < aOther.mOtherIndices.size();
  }

  // compare indices with unknown dimension
  tOtherIndices::const_iterator vOtherIndexIterator = mOtherIndices.begin();
  tOtherIndices::const_iterator vOtherOtherIndexIterator = aOther.mOtherIndices.begin();
  while ((vOtherIndexIterator != mOtherIndices.end()) &&
         (vOtherOtherIndexIterator != aOther.mOtherIndices.end())) {
    if (*vOtherIndexIterator != *vOtherOtherIndexIterator) {
      return *vOtherIndexIterator < *vOtherOtherIndexIterator;
    }
    ++vOtherIndexIterator;
    ++vOtherOtherIndexIterator;
  }

  // other matches
  return false;
}


const bpString& bpSeriesList::cFileName::GetFileName() const
{
  return mFileName;
}


bpSeriesList::cFileLess::cFileLess(const std::vector<cFileName>& aFileNames,
                                   const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence)
  : mFileNames(aFileNames),
    mDimensionSequence(aDimensionSequence)
{
}


bool bpSeriesList::cFileLess::operator () (bpSize aFileNameIndexL,
                                           bpSize aFileNameIndexR) const
{
  return mFileNames[aFileNameIndexL].Less(mFileNames[aFileNameIndexR], mDimensionSequence);
}


std::vector<bpString> bpSeriesList::GuessSeriesSorting(const std::vector<bpString>& aFileList)
{
  std::vector<bpFileReaderImpl::Dimension> vDimensionSequence;
  vDimensionSequence.push_back(bpFileReaderImpl::X);
  vDimensionSequence.push_back(bpFileReaderImpl::Y);
  vDimensionSequence.push_back(bpFileReaderImpl::Z);
  vDimensionSequence.push_back(bpFileReaderImpl::C);
  vDimensionSequence.push_back(bpFileReaderImpl::T);
  return GuessSeriesSorting(aFileList, vDimensionSequence);
}


std::vector<bpString> bpSeriesList::GuessSeriesSorting(const std::vector<bpString>& aFileList,
                                                       const std::vector<bpFileReaderImpl::Dimension>& aDimensionSequence)
{
  // fill vector of file names
  std::vector<cFileName> vFileNames;
  std::vector<bpSize> vFileNameIndices;
  std::vector<bpString>::const_iterator vFileIterator = aFileList.begin();
  while (vFileIterator != aFileList.end()) {
    vFileNameIndices.push_back(vFileNameIndices.size());
    vFileNames.push_back(cFileName(*vFileIterator));
    ++vFileIterator;
  }

  // sort them
  std::sort(vFileNameIndices.begin(),
            vFileNameIndices.end(),
            cFileLess(vFileNames, aDimensionSequence));

  // fill back to vector of strings
  std::vector<bpString> vFileList;
  std::vector<bpSize>::const_iterator vFileNameIndexIterator = vFileNameIndices.begin();
  while (vFileNameIndexIterator != vFileNameIndices.end()) {
    vFileList.push_back(vFileNames[*vFileNameIndexIterator].GetFileName());
    ++vFileNameIndexIterator;
  }

  //
  return vFileList;
}

/*
  // performance measurement for "GuessSeriesSorting"

  std::vector<bpString> vFileNames;
  const bpString vBaseFileName = "c:\\program files\\bitplane\\imaris 5.7.0\\images\\retina";
  const bpSize vSizeX = 1;
  const bpSize vSizeY = 1;
  const bpSize vSizeZ = 16;
  const bpSize vSizeC = 2;
  const bpSize vSizeT = 128;
  for (bpSize vIndexT = 0; vIndexT < vSizeT; vIndexT++) {
    for (bpSize vIndexC = 0; vIndexC < vSizeC; vIndexC++) {
      for (bpSize vIndexZ = 0; vIndexZ < vSizeZ; vIndexZ++) {
        for (bpSize vIndexY = 0; vIndexY < vSizeY; vIndexY++) {
          for (bpSize vIndexX = 0; vIndexX < vSizeX; vIndexX++) {
            bpString vFileName = vBaseFileName;
            vFileName += "_t" + bpToString(vIndexT);
            vFileName += "_c" + bpToString(vIndexC);
            vFileName += "_z" + bpToString(vIndexZ);
            vFileName += "_y" + bpToString(vIndexY);
            vFileName += "_x" + bpToString(vIndexX);
            vFileName += ".ims";
            vFileNames.push_back(vFileName);
          }
        }
      }
    }
  }

  bpString vMsg;
  for (bpSize vFileNameIndex = 0; vFileNameIndex < vFileNames.size(); vFileNameIndex++) {
    vMsg += vFileNames[vFileNameIndex] + "\n";
  }

  bpEnv::WriteDebugFile("c:\\files0.txt", vMsg, false);

  bpTimer* vTimer = new bpTimer();
  vTimer->Reset();
  vTimer->Start();

  std::vector<bpFileReaderImpl::Dimension> vDimensionSequence;
  vDimensionSequence.push_back(bpFileReaderImpl::C);
  vDimensionSequence.push_back(bpFileReaderImpl::Z);
  vDimensionSequence.push_back(bpFileReaderImpl::T);
  vDimensionSequence.push_back(bpFileReaderImpl::X);
  vDimensionSequence.push_back(bpFileReaderImpl::Y);
  vFileNames = bpSeriesList::GuessSeriesSorting(vFileNames, vDimensionSequence);

  bpDouble vTime = vTimer->DiffAndStop();
  delete vTimer;

  vMsg = "Files = " + bpToString(vFileNames.size()) + "\n";
  vMsg += "Time = " + bpToString(vTime, -1) + " s\n";
  for (bpSize vFileNameIndex = 0; vFileNameIndex < vFileNames.size(); vFileNameIndex++) {
    vMsg += vFileNames[vFileNameIndex] + "\n";
  }

  bpEnv::WriteDebugFile("c:\\files1.txt", vMsg, false);
*/


