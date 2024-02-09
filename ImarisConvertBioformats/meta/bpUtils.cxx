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


#include "bpUtils.h"
#include <limits>
#include <iomanip>


bpString bpToString(bpFloat aValue, bpInt16 aFixedPrecision)
{
  std::ostringstream vOStringStream;
  if (aFixedPrecision >= 0) {
    vOStringStream.setf(std::ios_base::fixed);
    vOStringStream << std::setprecision(aFixedPrecision) << aValue;
  }
  else {
    vOStringStream << std::setprecision(std::numeric_limits<bpFloat>::digits10) << aValue;
  }
  return vOStringStream.str();
}


bpString bpJoin(const std::vector<bpString>& aStrings, const bpString& aDelimiter)
{
  std::ostringstream vResult;
  auto vIt = aStrings.begin();
  auto vEnd = aStrings.end();
  while (vIt != vEnd) {
    vResult << *vIt;
    ++vIt;
    if (vIt != vEnd) {
      vResult << aDelimiter;
    }
  }
  return vResult.str();
}


std::vector<bpString> bpSplit(const bpString& aString, const bpString& aSeparatorString, bool aSkipEmptyParts, bool aSkipEmptyBoundParts)
{
  std::vector<bpString> vVector;
  if (aString.length() == 0) {
    return vVector;
  }

  bpSize vSplitStringLength = aSeparatorString.length();
  bpSize vCur = 0;
  bpSize vPos = aString.find(aSeparatorString);

  while (vPos != bpString::npos) {
    if (vPos > 0 || !aSkipEmptyBoundParts) {
      bpString vSubString = aString.substr(vCur, vPos - vCur);
      if (vSubString.size() > 0 || !aSkipEmptyParts || (!aSkipEmptyBoundParts && vPos == 0)) {
        vVector.push_back(vSubString);
      }
    }
    vCur = vPos + vSplitStringLength;
    vPos = aString.find(aSeparatorString, vCur);
  }

  if (vCur <= aString.length()) {
    bpString vSubString = aString.substr(vCur);
    if (vSubString.size() > 0 || !aSkipEmptyBoundParts) {
      vVector.push_back(vSubString);
    }
  }

  return vVector;
}


std::vector<bpString> bpSplitNonNumeric(const bpString& aString)
{
  bpString vString(aString);
  std::vector<bpString> vBlocksNonNumeric;

  bpSize vPos = vString.find_first_of("0123456789");
  if (vPos == 0 && vString.size()>0) { //the filename starts with a number, consider it as nonnumeric
    bpString vString2(vString);
    vPos = vString2.find_first_not_of("0123456789");
    vString2.erase(0, vPos);
    if (vString2.size()>0) {
      bpSize vPos2 = vString2.find_first_of("0123456789");
      vBlocksNonNumeric.push_back(vString.substr(0, vPos + vPos2));
      vString.erase(0, vPos + vPos2);
    }
  }

  while (vString.size()>0) {
    vPos = vString.find_first_of("0123456789");

    // push nonnumeric block onto vector
    if (vPos != 0) {
      vBlocksNonNumeric.push_back(vString.substr(0, vPos));
      vString.erase(0, vPos);
    }

    // remove numeric block
    vPos = vString.find_first_not_of("0123456789");
    vString.erase(0, vPos);
  }

  return vBlocksNonNumeric;
}


std::vector<bpString> bpSplitNumeric(const bpString& aString)
{
  bpString vString(aString);
  std::vector<bpString> vBlocksNumeric;

  bpSize vPos = vString.find_first_not_of("0123456789");
  if (vPos != 0 && vPos != bpString::npos) {
    vString.erase(0, vPos);
  }

  while (vString.size() > 0) {
    vPos = vString.find_first_not_of("0123456789");

    // push numeric block onto vector
    if (vPos != 0) {
      vBlocksNumeric.push_back(vString.substr(0, vPos));
      vString.erase(0, vPos);
    }

    // remove nonnumeric block
    vPos = vString.find_first_of("0123456789");
    vString.erase(0, vPos);
  }

  return vBlocksNumeric;
}


std::vector<bpInt32> bpSplitNumericInt32(const bpString& aString)
{
  bpString vString(aString);
  std::vector<bpInt32> vNumbers;
  bpSize vPos = 0;
  bpInt32 vValue = 0;

  while (!vString.empty()) {
    vPos = vString.find_first_not_of("0123456789");

    // push numeric block onto vector
    if (vPos != 0) {
      bpFromString(vString.substr(0, vPos), vValue);
      vNumbers.push_back(vValue);
      vString.erase(0, vPos);
    }

    // remove nonnumeric block
    vPos = vString.find_first_of("0123456789");
    vString.erase(0, vPos);
  }

  return vNumbers;
}


bpString bpReplace(const bpString& aString, const bpString& aSubStringOld, const bpString& aSubStringNew)
{
  if (aSubStringOld.empty()) {
    return aString;
  }
  bpString vString = aString;
  bpString vResult = "";
  bpSize vPos = vString.find(aSubStringOld);
  while (vPos != bpString::npos) {
    vResult += vString.substr(0, vPos);
    vResult += aSubStringNew;
    vString.erase(0, vPos + aSubStringOld.length());
    vPos = vString.find(aSubStringOld);
  }
  vResult += vString;
  return vResult;
}


bool bpStartsWith(const bpString& aString, const bpString& aPrefix)
{
  if (aString.size() < aPrefix.size()) {
    return false;
  }
  return aString.compare(0, aPrefix.size(), aPrefix) == 0;
}


bool bpEndsWith(const bpString& aString, const bpString& aPrefix)
{
  if (aString.size() < aPrefix.size()) {
    return false;
  }
  return aString.compare(aString.size() - aPrefix.size(), aPrefix.size(), aPrefix) == 0;
}


static int bpFindLast(const bpString& aString, char aChar)
{
  int vPos = static_cast<int>(aString.size()) - 1;
  while (vPos != -1 && aString[static_cast<bpSize>(vPos)] != aChar) {
    vPos--;
  }
  return vPos;
}


bpInt32 bpFindLastSeparator(const bpString& fullname)
{
  // first try to find the character "\"
  bpInt32 vResult = bpFindLast(fullname, (char)92);

  if (vResult == -1) {
    // try to find the character "/"
    vResult = bpFindLast(fullname, (char)47);
  }

  return vResult;
}


bpString bpToUtf8(const std::wstring& aString)
{
  if (aString.empty()) {
    return bpString();
  }
  // utf8 encoded chars are either 1, 2, 3 or 4 bytes long
  std::vector<char> vStringBuf(4 * aString.length());
  bpSize vUtf8Length = 0;
  // splitted character
  wchar_t vSplitCharBuffer;
  char* vSplitChar = reinterpret_cast<char*>(&vSplitCharBuffer);

  for (std::wstring::const_iterator vIter = aString.begin(); vIter != aString.end(); ++vIter) {
    vSplitCharBuffer = *vIter;

    // check if more than one char is needed for the splitted character
    bool vIsUnicode = (bpUInt32)(*vIter) > 0x7f;
    bpSize vIndexMax = 0;
    for (bpSize vIndex = 1; vIndex < sizeof(wchar_t); vIndex++) {
      if (vSplitChar[vIndex] != '\0') {
        vIndexMax = vIndex;
      }
    }

    if (!vIsUnicode) {
      vStringBuf[vUtf8Length++] = vSplitChar[0];
      continue;
    }

    // find the highest non-zero bit
    bpUInt8 vBitMask = 0x80;
    bpSize vBits = 8;
    while ((vBitMask & vSplitChar[vIndexMax]) == 0) {
      vBitMask >>= 1;
      vBits--;
    }

    bpSize vNumBits = vIndexMax * 8 + vBits;
    bpSize vAdditionalBytes = 1;
    if (vNumBits > 16) {
      vAdditionalBytes = 3;
    }
    else if (vNumBits > 11) {
      vAdditionalBytes = 2;
    }

    // create the UTF-8 encoding
    bpSize vIndex = 0;
    wchar_t vChar = *vIter;
    while (vNumBits > 6) {
#ifdef __BIG_ENDIAN__
      vStringBuf[vUtf8Length + vIndex] = 0x80 | (vChar & 0x3f);
#else
      vStringBuf[vUtf8Length + vAdditionalBytes - vIndex] = 0x80 | (vChar & 0x3f);
#endif
      vChar >>= 6;
      vNumBits -= 6;
      vIndex++;
    }

#ifdef __BIG_ENDIAN__
    bpSize vLastIndex = vIndex;
#else
    bpSize vLastIndex = (vAdditionalBytes - vIndex);
#endif
    switch (vIndex) {
    case 1:
      vStringBuf[vUtf8Length + vLastIndex] = 0xc0 | (vChar & 0x1f);
      break;
    case 2:
      vStringBuf[vUtf8Length + vLastIndex] = 0xe0 | (vChar & 0x0f);
      break;
    case 3:
      vStringBuf[vUtf8Length + vLastIndex] = 0xf0 | (vChar & 0x07);
      break;
    }

    vUtf8Length += (vAdditionalBytes + 1);
  }

  std::string vUtf8String(&vStringBuf[0]/*, vUtf8Length*/);
  return vUtf8String;
}


std::wstring bpFromUtf8(const bpString& aString)
{
  if (aString.empty()) {
    return std::wstring();
  }

  // init output character array
  bpSize vMaxStrLength = aString.length() + 1;  // UTF-8 string has at least as many chars as unicode string
  std::vector<wchar_t> vWChars(vMaxStrLength, 0);

  bpSize vIndex = 0;
  for (std::string::const_iterator vIter = aString.begin(); vIter != aString.end(); ++vIter) {
    // test for UTF-8 encoding and convert first part of unicode character
    char vChar = *vIter;
    bpUInt8 vCount = 0;
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
    bpUInt8 vLength = vCount + 1;
    std::vector<char> vBackup(vLength);
    vBackup[0] = vChar;

    // convert the rest of the character
    bool vResult = (vCount > 0);
    while (vCount && vIter != aString.end()) {
      vChar = *(++vIter);
      vBackup[vLength - vCount] = vChar;
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
      for (bpUInt8 i = 0; i<vLength - vCount; i++) {
        vWChars[vIndex++] = vBackup[i];
      }

      if (vIter == aString.end()) {
        break;
      }
    }
  }

  std::wstring vUnicodeString(&vWChars[0]/*, aString.length()*/);
  return vUnicodeString;
}


static bpChar bpToUpperCase(bpChar aChar)
{
  return static_cast<bpChar>(toupper(aChar));
}

bpString bpToUpper(const bpString& aString)
{
  bpString vString(aString);
  std::transform(vString.begin(), vString.end(), vString.begin(), bpToUpperCase);
  return vString;
}


static bpChar bpToLowerCase(bpChar aChar)
{
  return static_cast<bpChar>(tolower(aChar));
}

bpString bpToLower(const bpString& aString)
{
  bpString vString(aString);
  std::transform(vString.begin(), vString.end(), vString.begin(), bpToLowerCase);
  return vString;
}


bool CheckForExtension(const std::vector<bpString>& aExtensions, const bpString& aExtensionName)
{
  for (const bpString& aExtension : aExtensions) {
    if (aExtension == aExtensionName) {
      return true;
    }
  }
  return false;
}


bool CheckForExtensionIms(const std::vector<bpString>& aExtensions)
{
  return CheckForExtension(aExtensions, "ims");
}
