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


#ifndef __BP_SECTION_CONTAINER__
#define __BP_SECTION_CONTAINER__

#include "ImarisWriter/interface/bpConverterTypes.h"


#include <map>


class bpParameterSection;


/**
 * Contains a map of string to bpParameterSections, every of
 * which holds a number of (bpString) key, (bpString) value
 * pairs. Contains method for creating sections querying, adding
 * and removing parameters and write the container to a stream.
 *
 * \ingroup applicationlogic
 */
class bpSectionContainer
{
public:
  using iterator =  std::map<bpString,bpParameterSection*>::iterator;
  using const_iterator =  std::map<bpString,bpParameterSection*>::const_iterator;

  /**
   * Constructor.
   *
   * Create a new container. Initially it contains no sections.
   */
  bpSectionContainer();

  /**
   * Copy constructor. Create a new container as copy from the old.
   *
   * @param aOther: The container from which to copy.
   */
  bpSectionContainer(const bpSectionContainer& aOther);

  /**
   * Constructor from std containers
   */
  bpSectionContainer(const std::map<bpString, std::map<bpString, bpString> >& aSectionMap);

  /**
   * Destructor.
   *
   * All contained parameter sections are deleted.
   **/
  virtual ~bpSectionContainer();

  /**
   * Assignment operator.
   *
   * @param aOther: The container from which to copy.
   */
  bpSectionContainer& operator=(const bpSectionContainer& aOther);

  /**
   * Merge two section containers. Sections that are not in the original
   * container are added, section that appear in both containers are
   * merged. New parameters (in other) overwrite old parameters.
   *
   * @param aOther: The other section to merge with this one.
   */
  void Merge(const bpSectionContainer& aOther);

  /**
   * Create a new section.
   *
   * @param aSectionName: The name of the section. If a section already exists with
   * this name, it is returned instead.
   *
   * @return A pointer to a (new) section. The section will be deleted when the
   *   container is destroyed.
   */
  bpParameterSection* CreateSection(const bpString& aSectionName);

  /**
   * Add a section. As the section is destroyed when the container is
   * destroyed a section can only be added to ONE container.
   *
   * If the section already exists, the parameters are added and the supplied
   * section is deleted! In that case the old section is returned.
   *
   * @param aSectionName: Name of the section
   * @param aSection: The section to add
   *
   * @return The supplied section if no section with the name exists, otherwise the
   *   section with that name to which the parameters have been added.
   */
  bpParameterSection* AddSection(const bpString& aSectionName, bpParameterSection* aSection);

  /**
   * Get a section value. If the section does not exist the exception
   * bpNoSuchSection is thrown.
   *
   * @param aSectoinName: The name of the section
   *
   * @return A pointer to the section.
   *
   * @exception bpNoSuchSectionException if the section does not exist
   */
  bpParameterSection* GetSection(const bpString& aSectionName) const;

  /**
   * Checks if the section exists.
   *
   * @param aSectionName: Name of the section
   *
   * @return 1 if the section exists, 0 otherwise
   */
  bool HasSection(const bpString& aSectionName) const;

  /**
   * Remove a section. If it does not exist no error occurs.
   *
   * @param aSectionName: Section to remove
   */
  void RemoveSection(const bpString& aSectionName);

  /**
   * Rename a section. No error occurs if it does not exist, but an exception
   * is thrown if the newName already exists.
   *
   * @param aOldName: The old section name.
   * @param aNewName: The new section name.
   * @exceptions bpArgumentException if the new name is already used.
   */
  void RenameSection(const bpString& aOldName, const bpString& aNewName);

  /**
   * @name Iterators
   *
   * To access all sections, iterators are used. As all iterators, Begin()
   * returns an iterator pointing to the first element, End() returns an
   * iterator pointing behind the last element.
   */
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  /**
   * Write the section to a stream.
   * The format is:
   *
   * [SectionName] \\
   * ParameterList \\
   *
   * @param aOut: the stream to write to
   *
   * @return The stream (out)
   *
   * Note: The function should be const, but it is not as there yet no const
   * iterators.
   */
  std::ostream& Write(std::ostream& aOut);

  bpString ToString() const;

  /**
   * Shortcut for getting a parameter from a specific section. If either the
   * section or the parameter does not exist an exception is thrown.
   *
   * @param aSectionName: Name of the section
   * @param parameterName: Name of the parameter in the section
   *
   * @return The value of the parameter.
   *
   * @exception bpNoSuchSectionException if the section does not exists.
   * @exception bpNoSuchParameterException of the parameter does not exists.
   */
  const bpString& GetParameter(const bpString& aSectionName, const bpString& aParameterName) const;

  /**
   * Shortcut for setting a parameter. If the section does not exist an
   * exception is thrown. If the parameter does not exist, it is created.
   *
   * @param aSectionName: The name of the section.
   * @param parameterName: The name of the parameter to set.
   * @param parameterValue: The value of the parameter.
   *
   * @exceptions bpNoSuchSectionException if the section does not exists.
   */
  void SetParameter(const bpString& aSectionName, const bpString& aParameterName, const bpString& aParameterValue);

  ///sets a default location value only if the current value is invalid
  void SetDefaultLocationParameter(const bpString& aSectionName, const bpString& aParameterName, const bpString& aParameterValue);

  /**
   * Checks if a parameter exists.
   *
   * @param aSectionName: The name of the section.
   * @param parameterName: The name of the parameter.
   *
   * @return true if it exists, false if not.
   */
  bool HasParameter(const bpString& aSectionName, const bpString& aParameterName) const;


private:

  /**
   * Delete all sections.
   */
  void Cleanup();

  std::map<bpString, bpParameterSection*> mSectionMap;

};


template<typename T>
inline bool bpTryGetParameter(const bpSectionContainer& aSectionContainer, const bpString& aSectionName, const bpString& aParameterName, T& aParameterValue)
{
  if (!aSectionContainer.HasParameter(aSectionName, aParameterName)) {
    return false;
  }

  bpFromString(aSectionContainer.GetParameter(aSectionName, aParameterName), aParameterValue);
  return true;
}


#endif
