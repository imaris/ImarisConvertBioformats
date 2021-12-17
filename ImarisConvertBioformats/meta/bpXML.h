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


#ifndef __BP_XML__
#define __BP_XML__

#include "ImarisWriter/interface/bpConverterTypes.h"

/**
 * Xml formatting utilities
 *
 * \ingroup utils
 */
class bpXML
{
public:

  static bpString WriteOneLineTag(const bpString& aIndent, const bpString& aTagName, const bpString& aTagValue, bool aCDataValue = false);
  static bpString WriteOneLineTag(const bpString& aTagName, const bpString& aTagValue, bool aCDataValue = false);
  static bpString WriteOneLineTag(const bpString& aTagName, const bpString& aTagValue, const std::vector<bpString>& aAttrs, bool aCDataValue = false);
  static bpString WriteOneLineTag(const bpString& aIndent, const bpString& aTagName, const bpString& aTagValue, const std::vector<bpString>& aAttrs, bool aCDataValue = false);
  static bpString GetTagName(const bpString& str);
  static std::vector<bpString> GetTagAttrs(const bpString& str);
  static bpString GetAttrValue(const std::vector<bpString>& aAttrs, const bpString& aAttrName);
  template <typename TValueType> static void GetAttrValue(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, TValueType& aValue);
  static bpString GetTagValue(const bpString& str);
  static void AddAttrValue(const bpString& aAttrName, const bpString& aAttrValue, std::vector<bpString>& aAttrs);

  static bpString GetStartTag(const bpString& aTagName, const std::vector<bpString>& aAttrs = std::vector<bpString>());
  static bpString GetEndTag(const bpString& aTagName);

  static bpString ReadLine(FILE* file);

  static bpString ToString(const bpFloat* value, bpUInt32 size);
  static bool FromString(const bpString& str, bpFloat* value, bpUInt32 size);

  /**
  * replaces "<", ">", "&"  by "&lt;", "&gt;", "&amp;"
  */
  static bpString Encode(const bpString& aString);

  /**
  * replaces "&lt;", "&gt;", "&amp;" by "<", ">", "&"
  */
  static bpString Decode(const bpString& aString);

  static bpString Encode(const bpString& aString, bool aCData);
  static bpString EncodeCData(const bpString& aString);

  static bpString InsertSomeLineBreaks(const bpString& aXml, bpSize aApproximateLineSize);

private:
  static bpString InsertSomeLineBreaksIntoTag(const bpString& aXmlTag, bpSize aApproximateLineSize);
};

#endif // __BP_XML__
