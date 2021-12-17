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



#include "fileiobase/types/bpfSectionContainer.h"

#include "fileiobase/exceptions/bpfNoSuchSectionException.h"

#include "fileiobase/types/bpfParameterSection.h"

/**************************************************************************
 *
 * Constructor
 *
 * Create a new container. Initially it contains no sections.
 *
 *************************************************************************/
bpfSectionContainer::bpfSectionContainer()
{}

/**************************************************************************
 *
 * Destructor
 *
 * All contained parameter sections are deleted.
 *
 *************************************************************************/
bpfSectionContainer::~bpfSectionContainer()
{
  Cleanup();
}


/**
 * Copy constructor. Create a new container as copy from the old.
 *
 * @param other The container from which
 */
bpfSectionContainer::bpfSectionContainer(const bpfSectionContainer& other)
{
  std::map<bpfString,bpfParameterSection*>::const_iterator pos = other.sectionMap.begin();

  while (pos != other.sectionMap.end()) {

    // create a new section as copy of the other
    bpfParameterSection* section = new bpfParameterSection(*((*pos).second));

    // add it
    AddSection((*pos).first,section);
    ++pos;

  }
}

/**
 * Constructor from std containers
 *
 */
bpfSectionContainer::bpfSectionContainer(const std::map<bpfString, std::map<bpfString, bpfString> >& aSectionMap)
{
  std::map<bpfString, std::map<bpfString, bpfString> >::const_iterator pos = aSectionMap.begin();

  while (pos != aSectionMap.end()) {

    // create a new section as copy of the other
    bpfParameterSection* section = new bpfParameterSection(((*pos).second));

    // add it
    AddSection((*pos).first,section);
    ++pos;

  }

}


/**
 * Assigment operator.
 */
bpfSectionContainer&
bpfSectionContainer::operator=(const bpfSectionContainer& other)
{
  if (this != &other) {
    Cleanup();
    std::map<bpfString,bpfParameterSection*>::const_iterator pos = other.sectionMap.begin();
    while (pos != other.sectionMap.end()) {
      // create a new section as copy of the other
      bpfParameterSection* section = new bpfParameterSection(*((*pos).second));
      // add it
      AddSection((*pos).first,section);
      ++pos;
    }
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
bpfSectionContainer::Merge(const bpfSectionContainer& other)
{
  if (this != &other) {                 // we dont need to merge with ourselves
    std::map<bpfString,bpfParameterSection*>::const_iterator pos =
      other.sectionMap.begin();

    // look at all sections
    while (pos != other.sectionMap.end()) {
      if (HasSection((*pos).first)) {

        // section exists, merge it
        bpfParameterSection* section = GetSection((*pos).first);
        const bpfParameterSection* otherSection = ((*pos).second);
        section->Merge(*otherSection);
      }

      else {
        // section does not exist, add it
        bpfParameterSection* section = new bpfParameterSection(*((*pos).second));
        // add it
        AddSection((*pos).first,section);
      }
      ++pos;
    }
  }
}

/**************************************************************************
 *
 * Create a new section.
 *
 * Parameters:
 *   sectionName : The name of the section. If a section already exists with
 * this name, it is returned instead.
 *
 * Returns:
 *   A pointer to a (new) section. The section will be deleted when the
 *   container is destroyed.
 *
 **************************************************************************/
bpfParameterSection*
bpfSectionContainer::CreateSection(const bpfString& sectionName)
{
  // find a section with that name
  Iterator pos = sectionMap.find(sectionName);

  if (pos == sectionMap.end()) {
    // create a new section
    bpfParameterSection* section = new bpfParameterSection;
    sectionMap[sectionName] = section;
    return section;
  }
  else {
    // return the old section
    return(*pos).second;
  }
}

/***************************************************************************
 *
 * Add a section. As the section is destroyed when the container is
 * destroyed a section can only be added to ONE container.
 *
 * If the section already exists, the parameters are added and the supplied
 * section is deleted! In that case the old section is returned.
 *
 * Parameters:
 *   sectionName : name of the section
 *   section     : the section to add
 *
 * Returns:
 *   The supplied section if no section with the name exists, otherwise the
 *   section with that name to which the parameters have been added.
 *
 *************************************************************************/
bpfParameterSection*
bpfSectionContainer::AddSection(const bpfString& sectionName,
                                bpfParameterSection* section)
{
  // find a section with that name
  Iterator pos = sectionMap.find(sectionName);

  if (pos == sectionMap.end()) {
    // insert the section
    sectionMap[sectionName] = section;
    return section;
  }
  else {
    // add parameters to the old section
    bpfParameterSection* newSection = (*pos).second;

    bpfParameterSection::Iterator parameter = section->Begin();
    while (parameter != section->End()) {
      newSection->SetParameter((*parameter).first,(*parameter).second);
      ++parameter;
    }
    // delete old section
    delete section;

    //
    // Issue a warning message here to see where this happens (the called might
    // continue to use the supplied section)
    //
    BP_DEBUG_MSG("bpfSectionContainer::AddSection: Section was already present");
    return newSection;
  }
}


/***************************************************************************
 *
 * Get a section . If the section does not exist the exception
 * bpfNoSuchSection is thrown.
 *
 * Parameters:
 *  sectionName : the name of the sectopm
 *
 * Returns:
 *  A pointer to the section.
 *
 * Throws:
 *   bpfNoSuchSectionException if the section does not exist
 *
 **************************************************************************/
bpfParameterSection*
bpfSectionContainer::GetSection(const bpfString& sectionName) const

{
  std::map<bpfString,bpfParameterSection*>::const_iterator pos = sectionMap.find(sectionName);
  if (pos == sectionMap.end()) {
    throw bpfNoSuchSectionException(bpfString("bpfSectionContainer::GetSection: ")
                                    + sectionName);
  }
  return(*pos).second;
}


bool bpfSectionContainer::HasSection(const bpfString& aSectionName) const
{
  //checks if the section exists

  return sectionMap.find(aSectionName) != sectionMap.end();
}


/**************************************************************************
 *
 * Remove a section. If it does not exist no error occurs.
 *
 * Parameters:
 *   sectionName :Section to remove
 *
 *************************************************************************/
void
bpfSectionContainer::RemoveSection(const bpfString& sectionName)
{
  Iterator pos = sectionMap.find(sectionName);
  if (pos != sectionMap.end()) {
    bpfParameterSection* section = (*pos).second;
    // delete entry
    sectionMap.erase(pos);
    // delete section
    delete section;
  }
}

/**
 * Rename a section. No error occurs if it does not exist, but an exception
 * is thrown if the newName already exists.
 *
 * @param oldName The old section name.
 * @param newName The new section name.
 */
void
bpfSectionContainer::RenameSection(const bpfString& oldName,
                                   const bpfString& newName)

{
  // find the old section
  Iterator pos = sectionMap.find(oldName);
  if (pos == sectionMap.end()) {
    // does not exist
    return;
  }
  bpfParameterSection* section = (*pos).second;
  // erase the old entry and add new
  sectionMap.erase(pos);
  AddSection(newName,section);
}


/***************************************************************************
 *
 * To access all sections, iterators are used. As all iterators, Begin()
 * returns an iterator pointing to the first element, End() returns an
 * iterator pointing behind the last element.
 *
 **************************************************************************/
bpfSectionContainer::Iterator
bpfSectionContainer::Begin()
{
  return sectionMap.begin();
}

bpfSectionContainer::Iterator
bpfSectionContainer::End()
{
  return sectionMap.end();
}

bpfSectionContainer::ConstIterator
bpfSectionContainer::Begin() const
{
  return sectionMap.begin();
}

bpfSectionContainer::ConstIterator
bpfSectionContainer::End() const
{
  return sectionMap.end();
}

/**************************************************************************
 *
 * Write the section to a stream in the format:
 *
 * [SectionName]
 * ParameterList
 *
 * Parameters:
 *   out : the stream to write to
 *
 * Returns:
 *   the stream (out)
 *
 * Note: The function should be const, but it is not as there have no const
 * iterators yet.
 *
 *************************************************************************/
std::ostream&
bpfSectionContainer::Write(std::ostream& out)
{
  Iterator pos = Begin();
  while (pos != End()) {
    out << "[" << (*pos).first.c_str()  << "]" << std::endl;
    ((*pos).second)->Write(out);
    ++pos;
  }
  return out;
}


bpfString bpfSectionContainer::ToString() const
{
  bpfString vResult = "";
  std::map<bpfString, bpfParameterSection*>::const_iterator vPos = sectionMap.begin();
  while (vPos != sectionMap.end()) {
    vResult += "[" + vPos->first + "]\n";
    vResult += vPos->second->ToString();
    ++vPos;
  }
  return vResult;
}


/***************************************************************************
 *
 * Shortcut for getting a parameter from a specific section. If either the
 * section or the parameter does not exist an exception is thrown.
 *
 * Parameters:
 *   sectionName : name of the section
 *   parameterName : name of the parameter in the section
 *
 * Returns:
 *   The value of the parameter.
 *
 * Throws:
 *   bpfNoSuchSectionException if the section does not exists.
 *   bpfNoSuchParameterException of the parameter does not exists.
 *
 *************************************************************************/
const bpfString&
bpfSectionContainer::GetParameter(const bpfString& sectionName,
                                  const bpfString& parameterName) const

{
  return GetSection(sectionName)->GetParameter(parameterName);
}

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
void bpfSectionContainer::SetParameter(const bpfString& sectionName,
                                       const bpfString& parameterName,
                                       const bpfString& value)
{
  GetSection(sectionName)->SetParameter(parameterName,value);
}

bool bpfSectionContainer::HasParameter(const bpfString& sectionName,
                                       const bpfString& parameterName) const
{
  if (HasSection(sectionName)) {
    return GetSection(sectionName)->HasParameter(parameterName);
  }
  else {
    return false;
  }
}

/**
 * Cleanup.
 */
void
bpfSectionContainer::Cleanup()
{
  Iterator pos = Begin();
  while (pos != End()) {

    bpfParameterSection* section = (*pos).second;
    if (!section) {
      BP_DEBUG_MSG("bpfSectionContainer::Cleanup: Illegal section entry");
    }
    // delete entry
    sectionMap.erase(pos);
    // delete section
    delete section;

    pos = Begin();
  }
}
