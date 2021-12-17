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


#include "fileiobase/types/bpfTypes.h"
#include "fileiobase/application/bpfException.h"

#include <map>
#include <iostream>

class bpfParameterSection;

/**
 *
 * Class Section Container.
 *
 *
 */
class bpfSectionContainer
{
public:
  typedef std::map<bpfString,bpfParameterSection*>::iterator Iterator;
  typedef std::map<bpfString,bpfParameterSection*>::const_iterator ConstIterator;

  /**
   * Constructor.
   *
   * Create a new container. Initially it contains no sections.
   */
  bpfSectionContainer();

  /**
   * Copy constructor. Create a new container as copy from the old.
   *
   * @param other The container from which to copy.
   */
  bpfSectionContainer(const bpfSectionContainer& other);

  /**
   * Constructor from std containers
   *
   */
  bpfSectionContainer(const std::map<bpfString, std::map<bpfString, bpfString> >& aSectionMap);

  /**
   * Destructor.
   *
   * All contained parameter sections are deleted.
   **/
  virtual ~bpfSectionContainer();

  /**
   *
   *  Assignment operator.
   *
   * @param other The container from which to copy.
   */
  bpfSectionContainer& operator=(const bpfSectionContainer& other);

  /**
   * Merge two section containers. Sections that are not in the original
   * container are added, section that appear in both containers are
   * merged. New parameters (in other) overwrite old parameters.
   *
   * @param other The other section to merge with this one.
   */
  void Merge(const bpfSectionContainer& other);

  /**
   * Create a new section.
   *
   * @param sectionName The name of the section. If a section already exists with
   * this name, it is returned instead.
   *
   * @return A pointer to a (new) section. The section will be deleted when the
   *   container is destroyed.
   */
  bpfParameterSection* CreateSection(const bpfString& sectionName);

  /**
   * Add a section. As the section is destroyed when the container is
   * destroyed a section can only be added to ONE container.
   *
   * If the section already exists, the parameters are added and the supplied
   * section is deleted! In that case the old section is returned.
   *
   * @param sectionName  name of the section
   * @param section      the section to add
   *
   * @return  The supplied section if no section with the name exists, otherwise the
   *   section with that name to which the parameters have been added.
   */
  bpfParameterSection* AddSection(const bpfString& sectionName,
                                  bpfParameterSection* section);


  /**
   * Get a section value. If the section does not exist the exception
   * bpfNoSuchSection is thrown.
   *
   * @param sectoinName : the name of the section
   *
   * @return A pointer to the section.
   *
   * @exception bpfNoSuchSectionException if the section does not exist
   */
  bpfParameterSection* GetSection(const bpfString& sectionName) const;

  ///checks if aSectionName exists
  bool HasSection(const bpfString& aSectionName) const;

  /**
   * Remove a section. If it does not exist no error occurs.
   *
   * @param  sectionName Section to remove
   */
  void RemoveSection(const bpfString& sectionName);

  /**
   * Rename a section. No error occurs if it does not exist, but an exception
   * is thrown if the newName already exists.
   *
   * @param oldName The old section name.
   * @param newName The new section name.
   */
  void RenameSection(const bpfString& oldName,
                     const bpfString& newName);


  /**
   * @name Iterators
   *
   * To access all sections, iterators are used. As all iterators, Begin()
   * returns an iterator pointing to the first element, End() returns an
   * iterator pointing behind the last element.
   */
  Iterator Begin();
  Iterator End();
  ConstIterator Begin() const;
  ConstIterator End() const;

  /*
   * Read the section container from a stream. (Is currently in bpfParameterReader)
   *
   * @param in the stream to read from.
   * @return the input stream
   */
  // istream& Read(istream &in);

  /**
   * Write the section to a stream.
   * The format is:
   *
   * [SectionName] \\
   * ParameterList \\
   *
   * @parma the stream to write to
   *
   * @return the stream (out)
   *
   * Note: The function should be const, but it is not as there yet no const
   * iterators.
   */
  std::ostream& Write(std::ostream& out);

  bpfString ToString() const;

  /**
   * Shortcut for getting a parameter from a specific section. If either the
   * section or the parameter does not exist an exception is thrown.
   *
   *  @param  sectionName   name of the section
   *  @param  parameterName name of the parameter in the section
   *
   *  @return The value of the parameter.
   *
   *  @exception bpfNoSuchSectionException if the section does not exists.
   *  @exception bpfNoSuchParameterException of the parameter does not exists.
   */
  const bpfString& GetParameter(const bpfString& sectionName,
                                const bpfString& parameterName) const;

  /**
   * Shortcut for setting a parameter. If the section does not exist an
   * exception is thrown. If the parameter does not exist, it is created.
   *
   * @param sectionName The name of the section.
   * @param parameterName The name of the parameter to set.
   * @param value The value of the parameters.
   *
   * @exceptions bpfNoSuchSectionException if the section does not exists.
   */
  void SetParameter(const bpfString& sectionName,
                    const bpfString& parameterName,
                    const bpfString& value);

  /**
   * Checks if a parameter exists.
   *
   * @param sectionName The name of the section.
   * @param parameterName The name of the parameter.
   *
   * @return true if it exists, false if not.
   */
  bool HasParameter(const bpfString& sectionName,
                    const bpfString& parameterName) const;

private:
  /**
   * Delete all sections.
   */
  void Cleanup();

  ///
  std::map<bpfString, bpfParameterSection*> sectionMap;

};

#endif
