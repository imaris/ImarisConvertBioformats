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


#ifndef __BP_PARAMETER_SECTION__
#define __BP_PARAMETER_SECTION__


/*****************************************************************************
 *
 * Class Parameter Section
 *
 ****************************************************************************/


#include "ImarisWriter/interface/bpConverterTypes.h"


#include <map>


/**
 * A parameter section holds a number of <key,value> pairs (called parameters
 * and value) which are both strings.
 */
class bpParameterSection
{
public:
  // This is actually a map iterator which is of type "pair". This should be
  // replaced by an Iterator for which *Iterator is of type bpString
  using iterator = std::map<bpString, bpString>::iterator;
  using const_iterator =  std::map<bpString, bpString>::const_iterator;

  /***************************************************************************
   *
   * Constructor
   *
   * Create a new section. Initially it contains no parameters.
   *
   ***************************************************************************/
  bpParameterSection();

  /**
   *
   * Copy Constructor.
   *
   */
  bpParameterSection(const bpParameterSection& other);

  /**
   * Constructor from std::map<bpString, bpString>
   *
   */
  bpParameterSection(std::map<bpString, bpString> aParameterMap);

  /**
   * Assignment operator.
   */
  bpParameterSection& operator=(const bpParameterSection& other);

  /**
   * Merge this section with another section. Parameters that do not yet exist
   * are added. Parameters that exist are overwritten with new values.
   *
   * @param other The section to merge with.
   */
  void Merge(const bpParameterSection& other);

  /**
   *
   * Sets a parameter to the specified value. If the parameter does not yet
   * exist, it is created.
   *
   *
   * @param parameterName  parameter to set, it does not need to exist
   * @param value          value to which the parameter is set
   *
   */
  void SetParameter(const bpString& parameterName, const bpString& value);
  void SetParameter(const bpString& parameterName, float value);

  /**
   *
   * Get a parameter value. If the parameter does not exist the exception
   * bpNoSuchParameter is thrown.
   *
   * Parameters:
   *  parameterName : the name of the parameter to query
   *
   * Returns:
   *   the value of the parameter
   *
   * Throws:
   *   bpNoSuchParameterException if the parameter does not exist
   *
   */
  const bpString& GetParameter(const bpString& parameterName) const;

  /**
   *
   * Checks if the parameter exists.
   *
   * @param  parameterName : name of the parameter to query
   * @return  true if the parameter exists, 0 otherwise
   *
   */
  bool HasParameter(const bpString& parameterName) const;

  /**
   *
   * Remove a parameter. If it does not exist no error occurs.
   *
   * @param parameterName : Parameter to remove
   *
   */
  void RemoveParameter(const bpString& parameterName);


  /**
   *
   * To access all parameters, iterators are used. As all iterators Begin()
   * returns an iterator pointing to the first element, End() returns an
   * iterator pointing behind the last element.
   *
   */
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  /**
   *
   * Write the parameters to a stream in the format:
   *
   * ParameterName = value
   *
   * @param  out : the stream to write to
   *
   * @return  the stream (out)
   *
   * Note: The function should be const, but it is not as there yet no const
   * iterators.
   *
   */
  std::ostream& Write(std::ostream& out);
  bpString ToString() const;
  bpString ToStringLimitedLineNumberOfChars(bpSize aLineWidth) const;

private:
  bpString RemoveSpecialChars(const bpString& aString) const;
  std::map<bpString, bpString> parameterMap;
};

#endif
