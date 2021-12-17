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


#include "bpParameterSection.h"


#include <sstream>


/***************************************************************************
 *
 * Constructor
 *
 * Create a new section. Initially it contains no parameters.
 *
 ***************************************************************************/
bpParameterSection::bpParameterSection()
{}

/**
 *
 * Copy Constructor.
 *
 */
bpParameterSection::bpParameterSection(const bpParameterSection& other)
  : parameterMap(other.parameterMap)
{}

/**
 * Constructor from std::map<bpString, bpString>
 *
 */
bpParameterSection::bpParameterSection(std::map<bpString, bpString> aParameterMap)
  : parameterMap(std::move(aParameterMap))
{
}

/**
 * Assignment operator.
 */
bpParameterSection& bpParameterSection::operator=(const bpParameterSection& other)
{
  if (this != &other) {
    parameterMap = other.parameterMap;
  }
  return *this;
}

/**
 * Merge this section with another section. Parameters that do not yet exist
 * are added. Parameters that exist are overwritten with new values.
 *
 * @param other The section to merge with.
   */
void
bpParameterSection::Merge(const bpParameterSection& other)
{
  if (this != &other) {                 // dont merge with ourselves

    std::map<bpString, bpString>::const_iterator pos = other.parameterMap.begin();

    while (pos != other.parameterMap.end()) {
      SetParameter((*pos).first,(*pos).second);
      ++pos;
    }
  }
}

/**************************************************************************
 *
 * Sets a parameter to the specified value. If the parameter does not yet
 * exist, it is created.
 *
 * Parameters :
 *    parameterName : parameter to set, it does not need to exist
 *    value         : value to which the parameter is set
 *
 **************************************************************************/
void
bpParameterSection::SetParameter(const bpString& parameterName,
                                 const bpString& value)
{
  parameterMap[parameterName] = value;
}

void
bpParameterSection::SetParameter(const bpString& parameterName,
                                 float value)
{
  std::ostringstream parstr;
  parstr << value;
  parameterMap[parameterName] = parstr.str();
  return;
}

/***************************************************************************
 *
 * Get a parameter value. If the parameter does not exist
 * an exception is thrown.
 *
 * Parameters:
 *  parameterName : the name of the parameter to query
 *
 * Returns:
 *   the value of the parameter
 *
 **************************************************************************/
const bpString&
bpParameterSection::GetParameter(const bpString& parameterName) const

{
  std::map<bpString,bpString>::const_iterator pos = parameterMap.find(parameterName);

  if (pos == parameterMap.end()) {
    throw std::runtime_error("bpParameterSection::GetParameter: " + parameterName);
  }

  return(*pos).second;
}

/**************************************************************************
 *
 * Checks if the parameter exists.
 *
 * Parameters:
 *   parameterName : name of the parameter to query
 *
 * Returns:
 *   1 if the parameter exists, 0 otherwise
 *
 *************************************************************************/
bool
bpParameterSection::HasParameter(const bpString& parameterName) const
{
  return parameterMap.find(parameterName) != parameterMap.end();
}

/**************************************************************************
 *
 * Remove a parameter. If it does not exist no error occurs.
 *
 * Parameters:
 *   parameterName : Parameter to remove
 *
 *************************************************************************/
void
bpParameterSection::RemoveParameter(const bpString& parameterName)
{
  std::map<bpString,bpString>::iterator pos = parameterMap.find(parameterName);
  if (pos != parameterMap.end()) {
    parameterMap.erase(pos);
  }
}


/***************************************************************************
 *
 * To access all parameters, iterators are used. As all iterators Begin()
 * returns an iterator pointing to the first element, End() returns an
 * iterator pointing behind the last element.
 *
 **************************************************************************/


bpParameterSection::iterator bpParameterSection::begin()
{
  return parameterMap.begin();
}


bpParameterSection::iterator bpParameterSection::end()
{
  return parameterMap.end();
}


bpParameterSection::const_iterator bpParameterSection::begin() const
{
  return parameterMap.begin();
}


bpParameterSection::const_iterator bpParameterSection::end() const
{
  return parameterMap.end();
}


/**************************************************************************
 *
 * Write the parameters to a stream in the format:
 *
 * ParameterName = value
 *
 * Parameters:
 *   out : the stream to write to
 *
 * Returns:
 *   the stream (out)
 *
 *************************************************************************/
std::ostream&
bpParameterSection::Write(std::ostream& out)
{
  iterator pos = begin();
  while (pos != end()) {
    out << (*pos).first.c_str() << " = " << (*pos).second.c_str() << std::endl;
//    out << (*pos).first << " = " << (*pos).second << endl;
    ++pos;
  }
  return out;
}


bpString bpParameterSection::RemoveSpecialChars(const bpString& aString) const
{
  bpString vResult = "";
  for (bpUInt32 vPos = 0; vPos < aString.length(); vPos++) {
    switch (aString[vPos]) {
    case '\n':
      vResult += "\\n";
      break;
    case '\r':
      vResult += "\\r";
      break;
    case '\t':
      vResult += "\\t";
      break;
    default:
      vResult += aString[vPos];
    }
  }
  return vResult;
}


bpString bpParameterSection::ToString() const
{
  bpString vResult = "";
  std::map<bpString, bpString>::const_iterator vPos = parameterMap.begin();
  while (vPos != parameterMap.end()) {
    bpString vLine = vPos->first + " = " + RemoveSpecialChars(vPos->second);
    vResult += vLine + "\n";
    ++vPos;
  }
  return vResult;
}

bpString bpParameterSection::ToStringLimitedLineNumberOfChars(bpSize aLineNChars) const
{
  bpString vResult = "";
  std::map<bpString, bpString>::const_iterator vPos = parameterMap.begin();
  while (vPos != parameterMap.end()) {
    bpString vLine = vPos->first + " = " + RemoveSpecialChars(vPos->second);
    if (vLine.size()> aLineNChars) {
      vResult += vLine.substr(0,aLineNChars-3) + "...\n";
    }else{
      vResult += vLine + "\n";
    }
    ++vPos;
  }
  return vResult;
}

