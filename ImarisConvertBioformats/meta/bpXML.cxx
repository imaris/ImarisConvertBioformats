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


#include "bpXML.h"
#include "bpUtils.h"

#include <algorithm>


bpString bpXML::ToString(const bpFloat* value, bpUInt32 size)
{
  bpString str = "(";
  bpUInt32 i;
  for (i=0; i<size; i++) {
    str += bpToString(value[i]);
    if (i+1<size) {
      str += ", ";
    }
  }
  str += ")";
  return str;
}


bool bpXML::FromString(const bpString& str, bpFloat* value, bpUInt32 size)
{
  bpSize start = str.find("(", 0);
  if (start == std::string::npos) {
    return false;
  }
  bpSize end = start+1;
  for (bpUInt32 i=0; i<size; i++) {
    if (i+1<size) {
      end = str.find(",", start+1);
    }
    else {
      end = str.find(")", start+1);
    }
    if (end == std::string::npos) {
      return false;
    }
    bpFromString(str.substr(start + 1, end - start - 1), value[i]);
    start = end+1;
  }
  return true;
}


bpString bpXML::WriteOneLineTag(const bpString& aIndent, const bpString& aTagName, const bpString& aTagValue, bool aCDataValue)
{
  if (aTagValue == "") {
    return aIndent + "<" + aTagName + "/>";
  }
  else {
    return aIndent + "<" + aTagName + ">" + Encode(aTagValue, aCDataValue) + "</" + aTagName + ">";
  }
}


bpString bpXML::WriteOneLineTag(const bpString& aTagName, const bpString& aTagValue, bool aCDataValue)
{
  return WriteOneLineTag("", aTagName, aTagValue, aCDataValue);
}


bpString bpXML::WriteOneLineTag(const bpString& aTagName, const bpString& aTagValue, const std::vector<bpString>& aAttrs, bool aCDataValue)
{
  return WriteOneLineTag("", aTagName, aTagValue, aAttrs, aCDataValue);
}


bpString bpXML::WriteOneLineTag(const bpString& aIndent, const bpString& aTagName, const bpString& aTagValue, const std::vector<bpString>& aAttrs, bool aCDataValue)
{
  bpString vResult = aIndent + "<" + aTagName;
  for (bpSize vIndex = 0; vIndex < aAttrs.size()-1; vIndex += 2) {
    vResult = vResult + " " + aAttrs[vIndex] + "=\"" + Encode(aAttrs[vIndex+1]) + "\"";
  }
  if (aTagValue != "") {
    vResult = vResult + ">" + Encode(aTagValue, aCDataValue) + "</" + aTagName + ">";
  }
  else {
    vResult = vResult + "/>";
  }
  return vResult;
}

bpString bpXML::GetTagName(const bpString& str)
{
  bpSize start = str.find("<", 0);
  bpSize end = str.find(">", start);
  bpSize endname = str.find(" ",start);
  if (endname>0 && endname<end) {
    return str.substr(start + 1, endname - start - 1);
  }
  else {
    return str.substr(start + 1, end - start - 1);
  }
}


std::vector<bpString> bpXML::GetTagAttrs(const bpString& str)
{
  std::vector<bpString> attr;

  bpSize start = str.find("<", 0);
  bpSize end = str.find(">", start);
  bpSize endname = str.find(" ",start);
  bpSize endattr;

  start = str.find_first_not_of(" ",endname);
  while (str.find_first_of("=",start) != bpString::npos) {

    // left of "="
    end = str.find_first_of("=",start);
    endattr = str.find_first_of(" ",start);
    if (endattr>end) {
      endattr = end;
    }
    attr.push_back(bpString(str.substr(start,endattr-start)));

    //right of "="
    start = str.find_first_not_of(" ",end+1);
    end = str.find_first_of("/>",start);
    endattr = str.find_first_of(" /",start);
    if (endattr>end) {
      endattr = end;
    }
    attr.push_back(bpTrim(str.substr(start,endattr-start), "\""));

    start = endattr;
  }

  return attr;
}


bpString bpXML::GetAttrValue(const std::vector<bpString>& aAttrs, const bpString& aAttrName)
{
  std::vector<bpString>::const_iterator vPos;
  vPos = std::find(aAttrs.begin(), aAttrs.end(), aAttrName);
  if (vPos != aAttrs.end()) {
    ++vPos;
    if (vPos != aAttrs.end()) {
      return *vPos;
    }
  }
  return "";
}


void bpXML::AddAttrValue(const bpString& aAttrName, const bpString& aAttrValue, std::vector<bpString>& aAttrs)
{
  aAttrs.push_back(aAttrName);
  aAttrs.push_back(aAttrValue);
}


template <typename TValueType>
void bpXML::GetAttrValue(const std::vector<bpString>& aAttributes,
                         const bpString& aAttributeName,
                         TValueType& aValue)
{
  std::vector<bpString>::const_iterator vPos;
  vPos = std::find(aAttributes.begin(), aAttributes.end(), aAttributeName);
  if (vPos != aAttributes.end()) {
    ++vPos;
    if (vPos != aAttributes.end()) {
      bpFromString(*vPos, aValue);
    }
  }
}


template void bpXML::GetAttrValue<bpUInt8>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpUInt8& aValue);
template void bpXML::GetAttrValue<bpUInt16>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpUInt16& aValue);
template void bpXML::GetAttrValue<bpUInt32>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpUInt32& aValue);
template void bpXML::GetAttrValue<bpUInt64>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpUInt64& aValue);
template void bpXML::GetAttrValue<bpInt8>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpInt8& aValue);
template void bpXML::GetAttrValue<bpInt16>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpInt16& aValue);
template void bpXML::GetAttrValue<bpInt32>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpInt32& aValue);
template void bpXML::GetAttrValue<bpInt64>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpInt64& aValue);
template void bpXML::GetAttrValue<bpFloat>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpFloat& aValue);
template void bpXML::GetAttrValue<bpDouble>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpDouble& aValue);
template void bpXML::GetAttrValue<bpString>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpString& aValue);
template void bpXML::GetAttrValue<bool>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bool& aValue);
#ifndef __linux__
template void bpXML::GetAttrValue<bpSize>(const std::vector<bpString>& aAttributes, const bpString& aAttributeName, bpSize& aValue);
#endif

bpString bpXML::GetTagValue(const bpString& str)
{
  bpSize start = str.find(">", 0);
  bpSize end = str.find("<", start);
  return str.substr(start + 1, end - start - 1);
}


bpString bpXML::ReadLine(FILE* file)
{
  bpString str = "";
  char ch = static_cast<char>(fgetc(file));
  while (!feof(file) && (ch != '\n') && (ch != '\r')) {
    str += ch;
    ch = static_cast<char>(fgetc(file));
  }
  return str;
}


bpString bpXML::GetStartTag(const bpString& aTagName, const std::vector<bpString>& aAttrs)
{
  bpString vStartTag = "";
  vStartTag += "<" + aTagName;
  for (bpUInt32 vAttrIndex = 0; 2*vAttrIndex+1 < aAttrs.size(); vAttrIndex++) {
    vStartTag += " " + aAttrs[2*vAttrIndex] + "=\"" + Encode(aAttrs[2*vAttrIndex+1]) + "\"";
  }
  vStartTag += ">";
  return vStartTag;
}


bpString bpXML::GetEndTag(const bpString& aTagName)
{
  return "</" + aTagName + ">";
}


bpString bpXML::Encode(const bpString& aString)
{
  bpString vString(aString);
  vString = bpReplace(vString, "&", "&amp;"); // must be first
  vString = bpReplace(vString, "<", "&lt;");
  vString = bpReplace(vString, ">", "&gt;");
  vString = bpReplace(vString, "\"", "&quot;");
  vString = bpReplace(vString, "'", "&apos;");
  return vString;
}


bpString bpXML::Decode(const bpString& aString)
{
  bpString vString(aString);
  vString = bpReplace(vString, "&lt;", "<");
  vString = bpReplace(vString, "&gt;", ">");
  vString = bpReplace(vString, "&quot;", "\"");
  vString = bpReplace(vString, "&apos;", "'");
  vString = bpReplace(vString, "&amp;", "&"); // must be last
  return vString;
}


bpString bpXML::EncodeCData(const bpString& aString)
{
  return "<![CDATA[" + aString + "]]>\n";
}


bpString bpXML::Encode(const bpString& aString, bool aCData)
{
  return aCData ? EncodeCData(aString) : Encode(aString);
}



bpString bpXML::InsertSomeLineBreaks(const bpString& aXml, bpSize aApproximateLineSize)
{
  if (aXml.size() < aApproximateLineSize) {
    return aXml;
  }
  bpString vResult;
  bpSize vLineSize = 0;
  std::vector<bpString> vTags = bpSplit(aXml.substr(1, aXml.size() - 2), "><");
  for (bpSize vIndex = 0; vIndex < vTags.size(); vIndex++) {
    bpString vTag = InsertSomeLineBreaksIntoTag("<" + vTags[vIndex] + ">", aApproximateLineSize);
    vResult += vTag;
    vLineSize += vTag.size();
    if (vLineSize >= aApproximateLineSize && vIndex + 1 < vTags.size()) {
      vResult += "\n";
      vLineSize = 0;
    }
  }
  return vResult;
}


bpString bpXML::InsertSomeLineBreaksIntoTag(const bpString& aXmlTag, bpSize aApproximateLineSize)
{
  if (aXmlTag.size() < aApproximateLineSize) {
    return aXmlTag;
  }
  bpString vResult;
  vResult.reserve(aXmlTag.size() + (aXmlTag.size() / aApproximateLineSize) + 1);
  bpSize vLineSize = 0;
  bool vOpenQuote = false;
  for (bpSize vIndex = 0; vIndex < aXmlTag.size(); vIndex++) {
    char vChar = aXmlTag[vIndex];
    vResult += vChar;
    vLineSize++;
    if (vChar == '\"') {
      if (vOpenQuote && vLineSize >= aApproximateLineSize) {
        vResult += "\n";
        vLineSize = 0;
      }
      vOpenQuote = !vOpenQuote;
    }
  }
  return vResult;
}
