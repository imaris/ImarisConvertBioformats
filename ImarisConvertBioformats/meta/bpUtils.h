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


#ifndef __BP_UTILS__
#define __BP_UTILS__


#include "ImarisWriter/interface/bpConverterTypes.h"


#include <sstream>
#include <algorithm>


template<typename T>
inline void bpFromString(const bpString& aString, T& aValue)
{
  std::istringstream vStream(aString);
  vStream >> aValue;
}


template <>
inline void bpFromString(const bpString& aString, bpUInt8& aResult)
{
  unsigned int vResultInt;
  std::istringstream i(aString);
  i >> vResultInt;
  aResult = (bpUInt8)vResultInt;
}

template <>
inline void bpFromString(const bpString& aString, bpInt8& aResult)
{
  int vResultInt;
  std::istringstream i(aString);
  i >> vResultInt;
  aResult = (bpInt8)vResultInt;
}


template <>
inline void bpFromString(const bpString& aString, bool& aResult)
{
  if (aString == "true" || aString == "1") {
    aResult = true;
  }
}


template<typename T, size_t N>
inline void bpFromString(const bpString& aString, std::array<T, N>& aValue)
{
  std::istringstream vStream(aString);
  for (T& vValue : aValue) {
    vStream >> vValue;
  }
}


inline void bpFromString(const bpString& aString, bpVector3Float& aValue)
{
  bpFloatVec3& vValue = aValue;
  bpFromString(aString, vValue);
}


template<typename T>
inline T bpFromString(const bpString& aString)
{
  T vValue;
  bpFromString(aString, vValue);
  return vValue;
}


std::vector<bpString> bpSplit(const bpString& aString, const bpString& aSeparatorString, bool aSkipEmptyParts = false, bool aSkipEmptyBoundParts = true);


template <class Type>
void bpFromString(const bpString& aString, std::vector<Type>& aResult, bpString aDelimiter)
{
  aResult.clear();
  std::vector<bpString> vStringVector = bpSplit(aString, aDelimiter);
  aResult.reserve(vStringVector.size());
  std::vector<bpString>::iterator vIt = vStringVector.begin();
  while (vIt != vStringVector.end()) {
    Type vValue;
    bpFromString(*vIt, vValue);
    aResult.push_back(vValue);
    ++vIt;
  }
}


template<typename T>
inline bpString bpToString(const T& aValue)
{
  std::ostringstream vStream;
  vStream << aValue;
  return vStream.str();
}


inline bpString bpToString(bpString aValue)
{
  return aValue;
}


inline bpString bpToString(bool aValue)
{
  return aValue ? "true" : "false";
}


bpString bpToString(bpFloat aValue, bpInt16 aFixedPrecision = 3);


template<typename T, size_t N>
inline bpString bpToString(const std::array<T, N>& aValue, bpInt16 aFixedPrecision = 3)
{
  std::ostringstream vStream;
  if (N > 0) {
    vStream << bpToString(aValue[0], aFixedPrecision);
  }
  for (bpSize vIndex = 1; vIndex < aValue.size(); ++vIndex) {
    vStream << " " << bpToString(aValue[vIndex], aFixedPrecision);
  }
  return vStream.str();
}


inline bpString bpToString(const bpVector3Float& aValue)
{
  const bpFloatVec3& vValue = aValue;
  return bpToString(vValue);
}


inline bpString bpTrim(const bpString& s, const bpString& aTrimCharacters = " \t\n\r")
{
  bpSize start = s.find_first_not_of(aTrimCharacters);
  if (start == bpString::npos) {
    return "";
  }
  bpSize end = s.find_last_not_of(aTrimCharacters);
  return s.substr(start, end - start + 1);
}


bpString bpJoin(const std::vector<bpString>& aStrings, const bpString& aDelimiter);


std::vector<bpString> bpSplitNonNumeric(const bpString& aString);


std::vector<bpString> bpSplitNumeric(const bpString& aString);


std::vector<bpInt32> bpSplitNumericInt32(const bpString& aString);


bpString bpReplace(const bpString& aString, const bpString& aSubStringOld, const bpString& aSubStringNew);


bool bpStartsWith(const bpString& aString, const bpString& aPrefix);


bool bpEndsWith(const bpString& aString, const bpString& aPrefix);


bpInt32 bpFindLastSeparator(const bpString& fullname);


bpString bpToUtf8(const std::wstring& aString);


std::wstring bpFromUtf8(const bpString& aString);


bpString bpToUpper(const bpString& aString);


bpString bpToLower(const bpString& aString);


inline bool bpIsNumber(bpChar aChar)
{
  return aChar >= '0' && aChar <= '9';
}


bool CheckForExtension(const std::vector<bpString>& aExtensions, const bpString& aExtensionName);


bool CheckForExtensionIms(const std::vector<bpString>& aExtensions);


#endif // __BP_UTILS__
