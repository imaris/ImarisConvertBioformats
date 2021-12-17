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

#include "fileiobase/types/bpfTypes.h"
#include <map>

#include "fileiobase/application/bpfException.h"

/**
 *
 * Exceptions used in bpfParameterSection :
 *
 * bpfNoSuchParameterException : thrown in bpfParameterSection::GetParameter
 *
 */
class bpfNoSuchParameterException : public bpfException
{
public:
  bpfNoSuchParameterException()
    :bpfException("bpfNoSuchParameter","") {}

  bpfNoSuchParameterException(const bpfString& detailMessage)
    :bpfException("bpfNoSuchParameter",detailMessage) {}
};

/**
 * A parameter section holds a number of <key,value> pairs (called parameters
 * and value) which are both strings.
 */
class bpfParameterSection
{
public:
  typedef std::map<bpfString, bpfString> tParameterMap;
  typedef tParameterMap::iterator Iterator;
  typedef tParameterMap::const_iterator ConstIterator;

  /***************************************************************************
   *
   * Constructor
   *
   * Create a new section. Initially it contains no parameters.
   *
   ***************************************************************************/
  bpfParameterSection();
  bpfParameterSection(tParameterMap aParameterMap);

  operator tParameterMap()&&;

  /**
   * Merge this section with another section. Parameters that do not yet exist
   * are added. Parameters that exist are overwritten with new values.
   *
   * @param other The section to merge with.
   */
  void Merge(const bpfParameterSection& other);

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
  void SetParameter(const bpfString& parameterName, const bpfString& value);
  void SetParameter(const bpfString& parameterName, float value);

  /**
   *
   * Get a parameter value. If the parameter does not exist the exception
   * bpfNoSuchParameter is thrown.
   *
   * Parameters:
   *  parameterName : the name of the parameter to query
   *
   * Returns:
   *   the value of the parameter
   *
   * Throws:
   *   bpfNoSuchParameterException if the parameter does not exist
   *
   */
  const bpfString& GetParameter(const bpfString& parameterName) const;

  /**
   *
   * Checks if the parameter exists.
   *
   * @param  parameterName : name of the parameter to query
   * @return  true if the parameter exists, 0 otherwise
   *
   */
  bool HasParameter(const bpfString& parameterName) const;

  /**
   *
   * Remove a parameter. If it does not exist no error occurs.
   *
   * @param parameterName : Parameter to remove
   *
   */
  void RemoveParameter(const bpfString& parameterName);


  /**
   *
   * To access all parameters, iterators are used. As all iterators Begin()
   * returns an iterator pointing to the first element, End() returns an
   * iterator pointing behind the last element.
   *
   */
  Iterator Begin();
  Iterator End();

  ConstIterator Begin() const;
  ConstIterator End() const;

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
  bpfString ToString() const;

private:
  bpfString RemoveSpecialChars(const bpfString& aString) const;
  tParameterMap parameterMap;
};

#endif
