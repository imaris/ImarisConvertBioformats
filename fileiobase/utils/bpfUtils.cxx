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


#include "fileiobase/utils/bpfUtils.h"

#include "fileiobase/utils/bpfSysInfo.h"
#include "fileiobase/utils/utf8.h"

#if defined(_WIN32)
#include <windows.h>
#endif


//
// Gibt die Position des Filenames innerhalb des Paths zurueck
//

int bpf_filenpos(const bpfString& fullname)
{
  // first try to find the character "\"
  bpfInt32 vResult = bpf_rfind(fullname, (char)92);

  if (vResult == -1) {
    // try to find the character "/"
    vResult = bpf_rfind(fullname, (char)47);
  }

  return vResult;
}


char bpfMonths[12][4] = {
  "Jan", "Feb", "Mar", "Apr",
  "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};


//
// Zum Zusammenstellen von Datumsstrings
//

char* bpfMonthStr(int m)
{
  return bpfMonths[m - 1];
}

char bpfWDays[7][4] = {"Mon",
                       "Tue", "Wed", "Thu",
                       "Fri", "Sat", "Sun"
                      };


//
//	Kalender - gibt Wochentag eines Datums als String zurueck
//

char* bpfWDayStr(int byear, int bmonth, int bday)
{
  if (bmonth > 2) bmonth -= 3;
  else {
    bmonth += 9;
    byear--;
  }
  return bpfWDays[
      (146097 * (byear / 100) / 4
       + 1461 * (byear % 100) / 4
       + (153 * bmonth + 2) / 5
       + bday + 1721119) % 7];
}


//
// Oft benoetigte Rueckwaertssuchfunktion (z.B zum Splitten von Pfadnamen)
//

int bpf_rfind(const bpfString& srtr, char ob)
{
  bpfSize pos = srtr.size() - 1;
  while (pos != bpfString::npos && srtr[pos] != ob) {
    pos--;
  }
  return static_cast<int>(pos);
}


int bpf_isnumber(char chj)
{
  return(chj >= '0' && chj <= '9');
}



bool bpfIsMicro(const bpfString& aString)
{
  //checks if aString is "um" or a variant

  bool vIsMicro = false;

  bpfSize vSize = aString.size();

  if ((2 == vSize) && ('m' == aString[1])) {
    //mu = U+00B5 in Unicode
    if (('u' == aString[0]) || (bpfChar(0xB5) == aString[0])) {
      vIsMicro = true;
    }
  }
  else if ((3 == vSize) && ('m' == aString[2])) {
    //mu = 0xC2 0xB5 in UTF-8
    if ((bpfChar(0xC2) == aString[0]) && (bpfChar(0xB5) == aString[1])) {
      vIsMicro = true;
    }
  }
  else if ((7 == vSize) && 'm' == aString[vSize - 1]) {
    if ((bpfString("\\u00B5") == aString.substr(0, aString.size() - 1)) ||
        (bpfString("&#181;m") == aString))
    {
      vIsMicro = true;
    }
  }
  else if ("micron" == aString) {
    vIsMicro = true;
  }

  return vIsMicro;
}


bpfString bpfTrim(const bpfString& s)
{
  if (s.length() == 0) {
    return s;
  }
  bpfSize start = s.find_first_not_of(" \t");
  bpfSize end   = s.find_last_not_of(" \t");

  if (start == bpfString::npos) {
    return "";
  }
  return s.substr(start,end-start+1);
}

bpfString bpfTrim(const bpfString& s, const bpfString& aTrimCharacters)
{
  if (s.length() == 0) {
    return s;
  }
  bpfSize start = s.find_first_not_of(aTrimCharacters);
  bpfSize end   = s.find_last_not_of(aTrimCharacters);

  if (start == bpfString::npos) {
    return "";
  }
  return s.substr(start,end-start+1);
}


std::vector<bpfString> bpfSplit(const bpfString& aString, const bpfString& aSeparatorString)
{
  return bpfSplit(aString, aSeparatorString, false);
}

std::vector<bpfString> bpfSplit(const bpfString& aString, const bpfString& aSeparatorString, const bool aSkipEmptyParts)
{
  return bpfSplit(aString, aSeparatorString, aSkipEmptyParts, true);
}

std::vector<bpfString> bpfSplit(const bpfString& aString, const bpfString& aSeparatorString, const bool aSkipEmptyParts, const bool aSkipEmptyBoundParts)
{
  std::vector<bpfString> vVector;
  if (aString.length() == 0) {
    return vVector;
  }

  bpfSize vSplitStringLength = aSeparatorString.length();
  bpfSize vCur = 0;
  bpfSize vPos = aString.find(aSeparatorString);

  while (vPos != bpfString::npos) {
    if (vPos > 0 || !aSkipEmptyBoundParts) {
      bpfString vSubString = aString.substr(vCur, vPos - vCur);
      if (vSubString.size() > 0 || !aSkipEmptyParts || (!aSkipEmptyBoundParts && vPos == 0)) {
        vVector.push_back(vSubString);
      }
    }
    vCur = vPos + vSplitStringLength;
    vPos = aString.find(aSeparatorString, vCur);
  }

  if (vCur <= aString.length()) {
    bpfString vSubString = aString.substr(vCur);
    if (vSubString.size() > 0 || !aSkipEmptyBoundParts) {
      vVector.push_back(vSubString);
    }
  }

  return vVector;
}

bpfString bpfJoin(const std::vector<bpfString>& aStrings, const bpfString& aDelimitor)
{
  return bpfJoin(aStrings.begin(), aStrings.end(), aDelimitor);
}

bpfString bpfReplace(const bpfString& aString, const bpfString& aSubStringOld, const bpfString& aSubStringNew)
{
  if (aString.empty() || aSubStringOld.empty()) {
    return aString;
  }

  std::vector<bpfSize> vReplacePositions;
  bpfSize vPos = aString.find(aSubStringOld, 0);

  while (vPos != bpfString::npos) {
    vReplacePositions.push_back(vPos);
    vPos = aString.find(aSubStringOld, vPos + 1);
  }

  bpfSize vOldStep = aSubStringOld.size();
  bpfSize vNewStep = aSubStringNew.size();
  bpfInt32 vDifference = vNewStep - vOldStep;
  bpfString vOutput;
  vOutput.resize(aString.size() + (vDifference * vReplacePositions.size()));
  bpfSize vLastPos = 0;
  auto vDestinationIt = vOutput.begin();
  auto vDestinationEnd = vOutput.end();

  for (auto vReplacePosition : vReplacePositions) {
    auto vIncrement = vReplacePosition - vLastPos;
    aString.copy(&(*vDestinationIt), vIncrement, vLastPos);
    vLastPos = vReplacePosition + vOldStep;
    vDestinationIt += vIncrement;

    if (vDestinationIt == vDestinationEnd) {
      break;
    }

    aSubStringNew.copy(&(*vDestinationIt), vNewStep);
    vDestinationIt += vNewStep;
  }

  if (vLastPos < aString.size()) {
    aString.copy(&(*vDestinationIt), aString.size() - vLastPos, vLastPos);
  }

  return vOutput;
}

bool bpfStartsWith(const bpfString & aString, const bpfString & aPrefix)
{
  if (aString.size() < aPrefix.size()) {
    return false;
  }
  return aString.compare(0, aPrefix.size(), aPrefix) == 0;
}

bool bpfEndsWith(const bpfString& aString, const bpfString& aPostfix)
{
  if (aString.size() < aPostfix.size()) {
    return false;
  }
  return aString.compare(aString.size() - aPostfix.size(), aPostfix.size(), aPostfix) == 0;
}

static bpfChar bpfToUpperCase(bpfChar aChar)
{
  return static_cast<bpfChar>(toupper(aChar));
}

bpfString bpfToUpper(const bpfString& aString)
{
  bpfString vString(aString);
  std::transform(vString.begin(), vString.end(), vString.begin(), bpfToUpperCase);
  return vString;
}


static bpfChar bpfToLowerCase(bpfChar aChar)
{
  return static_cast<bpfChar>(tolower(aChar));
}

bpfString bpfToLower(const bpfString& aString)
{
  bpfString vString(aString);
  std::transform(vString.begin(), vString.end(), vString.begin(), bpfToLowerCase);
  return vString;
}


bpfString bpfToUtf8(const std::wstring& aString)
{
  if (aString.empty()) {
    return bpfString();
  }
  // utf8 encoded chars are either 1, 2, 3 or 4 bytes long
  std::vector<char> vStringBuf(4 * aString.length());
  bpfSize vUtf8Length = 0;
  // splitted character
  wchar_t vSplitCharBuffer;
  char* vSplitChar = reinterpret_cast<char*>(&vSplitCharBuffer);

  for (std::wstring::const_iterator vIter = aString.begin(); vIter != aString.end(); ++vIter) {
    vSplitCharBuffer = *vIter;

    // check if more than one char is needed for the splitted character
    bool vIsUnicode = (bpfUInt32)(*vIter) > 0x7f;
    bpfSize vIndexMax = 0;
    for (bpfSize vIndex = 1; vIndex < sizeof(wchar_t); vIndex++) {
      if (vSplitChar[vIndex] != '\0') {
        vIndexMax = vIndex;
      }
    }

    if (!vIsUnicode) {
      vStringBuf[vUtf8Length++] = vSplitChar[0];
      continue;
    }

    // find the highest non-zero bit
    bpfUInt8 vBitMask = 0x80;
    bpfSize vBits = 8;
    while ((vBitMask & vSplitChar[vIndexMax]) == 0) {
      vBitMask >>= 1;
      vBits--;
    }

    bpfSize vNumBits = vIndexMax*8 + vBits;
    bpfSize vAdditionalBytes = 1;
    if (vNumBits > 16) {
      vAdditionalBytes = 3;
    }
    else if (vNumBits > 11) {
      vAdditionalBytes = 2;
    }

    // create the UTF-8 encoding
    bpfSize vIndex = 0;
    wchar_t vChar = *vIter;
    while (vNumBits > 6) {
#ifdef __BIG_ENDIAN__
      vStringBuf[vUtf8Length+vIndex] = 0x80 | (vChar & 0x3f);
#else
      vStringBuf[vUtf8Length+vAdditionalBytes-vIndex] = 0x80 | (vChar & 0x3f);
#endif
      vChar >>= 6;
      vNumBits -= 6;
      vIndex++;
    }

#ifdef __BIG_ENDIAN__
    bpfSize vLastIndex = vIndex;
#else
    bpfSize vLastIndex = (vAdditionalBytes-vIndex);
#endif
    switch (vIndex) {
    case 1:
      vStringBuf[vUtf8Length+vLastIndex] = 0xc0 | (vChar & 0x1f);
      break;
    case 2:
      vStringBuf[vUtf8Length+vLastIndex] = 0xe0 | (vChar & 0x0f);
      break;
    case 3:
      vStringBuf[vUtf8Length+vLastIndex] = 0xf0 | (vChar & 0x07);
      break;
    }

    vUtf8Length += (vAdditionalBytes+1);
  }

  std::string vUtf8String(&vStringBuf[0], vUtf8Length);
  return vUtf8String;
}

std::wstring bpfFromUtf8(const bpfString& aString)
{
  if (aString.empty()) {
    return std::wstring();
  }

  // init output character array
  bpfSize vMaxStrLength = aString.length() + 1;  // UTF-8 string has at least as many chars as unicode string
  std::vector<wchar_t> vWChars(vMaxStrLength, 0);

  bpfSize vIndex = 0;
  for (std::string::const_iterator vIter = aString.begin(); vIter != aString.end(); ++vIter) {
    // test for UTF-8 encoding and convert first part of unicode character
    char vChar = *vIter;
    bpfUInt8 vCount = 0;
    if ((vChar & 0xf0) == 0xf0) {      // 11110000
      vCount = 3;
      vWChars[vIndex] = (vChar & 0x7);
    }
    else if ((vChar & 0xe0) == 0xe0) { // 11100000
      vCount = 2;
      vWChars[vIndex] = (vChar & 0xf);
    }
    else if ((vChar & 0xc0) == 0xc0) { // 11000000
      vCount = 1;
      vWChars[vIndex] = (vChar & 0x1f);
    }
    else {
      vWChars[vIndex++] = vChar;
      continue;
    }

    // make space for the next 6 bits
    vWChars[vIndex] <<= 6;

    // store the characters for the case we don't have a correct UTF-8 encoded character
    bpfUInt8 vLength = vCount+1;
    std::vector<char> vBackup(vLength);
    vBackup[0] = vChar;

    // convert the rest of the character
    bool vResult = (vCount > 0);
    while (vCount && vIter != aString.end()) {
      vChar = *(++vIter);
      vBackup[vLength-vCount] = vChar;
      vResult = vResult && ((vChar & 0x80) == 0x80);
      vCount--;
      if (vResult) {
        vWChars[vIndex] |= (vChar & 0x3f);
        if (vCount > 0) {
          // make space for the next 6 bits
          // (on systems where sizeof(wchar_t) is 16 bit, part of the data might be lost!!!)
          vWChars[vIndex] <<= 6;
        }
      }
    }

    if (vResult && (vCount == 0)) {
      // conversion successful
      vIndex++;
    }
    else {
      // conversion not successful. Store the original characters
      for (bpfUInt8 i=0; i<vLength-vCount; i++) {
        vWChars[vIndex++] = vBackup[i];
      }

      if (vIter == aString.end()) {
        break;
      }
    }
  }

  std::wstring vUnicodeString(&vWChars[0], aString.length());
  return vUnicodeString;
}


bpfString bpfFromUtf16(const bpfUInt16* aString, bpfSize aNumberOfUnicodeCharacters)
{
  bpfString vString;
  utf8::utf16to8(aString, aString+aNumberOfUnicodeCharacters, back_inserter(vString));
  return vString;
}

bpfString bpfLatin1FromUtf16(const std::wstring& aString)
{
  bpfString vDest;
  for (std::wstring::const_iterator vIt = aString.begin(); vIt != aString.end(); ++vIt) {
    if (static_cast<bpfUInt32>(*vIt) < 256) {
      vDest += static_cast<bpfChar>(*vIt);
    }
  }
  return vDest;
}


bpfString bpfLatin1FromUtf8(const bpfString& aString)
{
  return bpfLatin1FromUtf16(bpfFromUtf8(aString));
}

bpfString bpfUtf8FromSystemEncoding(const bpfString& aString)
{
#if defined(_WIN32)
  wchar_t* vConverted = new wchar_t();
  // get the length of the future converted string
  bpfInt32 vLengthOfString = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, aString.c_str(), -1, vConverted, 0);
  vConverted = static_cast<wchar_t*>(malloc(vLengthOfString));
  // do the conversion
  vLengthOfString = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, aString.c_str(), -1, vConverted, vLengthOfString);
  return bpfToUtf8(vConverted);
#else
  return aString;
#endif
}


bpfString bpfIntToString(bpfUInt16 aValue)
{
  std::ostringstream vStr;
  vStr << (int)aValue;
  return vStr.str();
}


bpfString bpfGetTemporaryPath()
{
  return bpfSysInfo::GetTemporaryPath();
}


bpfUInt32 bpfGetProcessId()
{
  return bpfSysInfo::GetProcessId();
}


bpfTimeInfo bpfTimeInfoFromDate1900(bpfDouble aOffSetFrom1900)
{
  return bpfTimeInfoFromDate(aOffSetFrom1900, bpfTimeInfo::ToJulianDay(1899, 12, 30));
}

bool IsTimeInfoValid(const bpfTimeInfo& aTimeInfo)
{
  return !(aTimeInfo < mValidTimeInfoMin) && !(aTimeInfo > mValidTimeInfoMax);
}

bpfTimeInfo bpfTimeInfoFromDate(bpfDouble aOffSetFromJulianDay, bpfInt32 aJulianDay)
{
  bpfInt64 vOffSetFromJulianDay = static_cast<bpfInt64>(aOffSetFromJulianDay);
  bpfInt64 vTimeInMilliSeconds = static_cast<bpfInt64>((aOffSetFromJulianDay - vOffSetFromJulianDay) * 1000.0 * 24 * 3600);

  bpfTimeInfo vTimeInfo;
  vTimeInfo.SetDate(aJulianDay);
  vTimeInfo.AddDays(static_cast<bpfInt32>(vOffSetFromJulianDay));
  vTimeInfo.SetTime(vTimeInMilliSeconds);
  return vTimeInfo;
}


bool bpfTimeInfoFromMetamorphFormat(const bpfString& aTime, bpfTimeInfo& aTimeInfo)
{
  // Metamorph ND stores date/time as YYYYMMDD HH:MM:SS.mss
  std::vector<bpfString> vSplittedTimeInfo = bpfSplit(bpfTrim(aTime), " ");
  if (vSplittedTimeInfo.size() < 2 || vSplittedTimeInfo[0].size() < 8 || vSplittedTimeInfo[1].size() < 9) {
    return false;
  }
  vSplittedTimeInfo[0].insert(6, "-");
  vSplittedTimeInfo[0].insert(4, "-");
  aTimeInfo.SetDate(vSplittedTimeInfo[0]);
  aTimeInfo.SetTime(vSplittedTimeInfo[1]);
  return true;
}


bool bpfTimeInfoFromAnyFormat(const bpfString& aDateTime, bpfTimeInfo& aTimeInfo)
{
  bpfString vDateTime = aDateTime;
  // if Date is of type YYYY:MM:DD HH:MM:SS as defined in the Tiff Specs...
  if (aDateTime.size() == 19) {
    vDateTime.replace(4,1,"-");
    vDateTime.replace(7,1,"-");
    aTimeInfo = bpfTimeInfo(vDateTime);
    return true;
  }
  // else if Date is of type MM-DD-YYYY as in one of the Tiff examples...
  else if (vDateTime.size() == 10) {
    std::vector<bpfString> vDateSplitted = bpfSplit(vDateTime,"-");
    // check good size
    if (vDateSplitted.size() == 3) {
      vDateTime = vDateSplitted[2] + "-" + vDateSplitted[0] + "-" + vDateSplitted[1];
      aTimeInfo.SetDate(vDateTime);
      aTimeInfo.SetTime(0);
      return true;
    }
  }
  // strangest format: 2/5/2010 1:53 PM
  else if (vDateTime.find("/") != bpfString::npos) {
    std::vector<bpfString> vDateTimeSplitted = bpfSplit(vDateTime," ");
    if (vDateTimeSplitted.size() == 3) {
      std::vector<bpfString> vDateSplitted = bpfSplit(vDateTimeSplitted[0],"/");
      // date
      if (vDateSplitted.size() == 3) {
        vDateTime = vDateSplitted[2] + "-" + vDateSplitted[1] + "-" + vDateSplitted[0];
        aTimeInfo.SetDate(vDateTime);
      }
      // time
      std::vector<bpfString> vTimeSplitted = bpfSplit(vDateTimeSplitted[1],":");
      if (vTimeSplitted.size() == 2) {
        bpfString vHour = vTimeSplitted[0];
        bpfString vMin = vTimeSplitted[1];
        if (vDateTimeSplitted[2] == "PM") {
          vHour = bpfToString(bpfFromString<bpfInt32>(vHour) + 12);
        }
        aTimeInfo.SetTime(vHour + ":" + vMin + ":00");
        return true;
      }
    }
  }
  // Metamorph ND stores date/time as YYYYMMDD HH:MM:SS.ms
  else  {
    return bpfTimeInfoFromMetamorphFormat(vDateTime, aTimeInfo);
  }
  return false;
}


void FixPadding(bpfChar* aData, const bpfUInt64 (&aSrcDim)[3], const bpfUInt64 (&aDestDim)[3], bpfUInt32 aSizeOfType)
{
  if (aSrcDim[0] == aDestDim[0] && aSrcDim[1] == aDestDim[1] && aSrcDim[2] == aDestDim[2]) {
    return;
  }
  else if (aSrcDim[0] > aDestDim[0] || aSrcDim[1] > aDestDim[1] || aSrcDim[2] > aDestDim[2]) {
    // This would be the right place to throw an exception, but exceptions thrown in ReadDataBlock are silently suppressed.
    // Anyway, it should be clear that this code position should never be reached, negative padding should never occur.
    return;
  }

  bpfUInt64 vSrcBlockSizeBytesX = aSizeOfType * aSrcDim[0];
  bpfUInt64 vDestBlockSizeBytesX = aSizeOfType * aDestDim[0];

  // starts from the end so that padding does not override data that still has to be copied in a more advanced position
  const bpfChar* vSrc = aData + vSrcBlockSizeBytesX * aSrcDim[1] * aSrcDim[2];
  bpfChar* vDest = aData + vDestBlockSizeBytesX * aDestDim[1] * aDestDim[2];

  bpfUInt64 vPadBytesXYZ = (aDestDim[2] - aSrcDim[2]) * aDestDim[1] * vDestBlockSizeBytesX;
  bpfUInt64 vPadBytesXY = (aDestDim[1] - aSrcDim[1]) * vDestBlockSizeBytesX;
  bpfUInt64 vPadBytesX = vDestBlockSizeBytesX - vSrcBlockSizeBytesX;

  vDest -= vPadBytesXYZ;
  memset(vDest, 0, (bpfSize)vPadBytesXYZ);
  for (bpfSize vIndexZ = 0; vIndexZ < aSrcDim[2]; vIndexZ++) {
    vDest -= vPadBytesXY;
    memset(vDest, 0, (bpfSize)vPadBytesXY);
    for (bpfSize vIndexY = 0; vIndexY < aSrcDim[1]; vIndexY++) {
      vDest -= vPadBytesX;
      memset(vDest, 0, (bpfSize)vPadBytesX);
      vSrc -= vSrcBlockSizeBytesX;
      vDest -= vSrcBlockSizeBytesX;
      memcpy(vDest, vSrc, (bpfSize)vSrcBlockSizeBytesX);
    }
  }

}
