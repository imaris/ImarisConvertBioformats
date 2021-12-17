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


#include "bpSectionContainer.h"

#include "bpParameterSection.h"
#include "bpFileTools.h"

#include <sstream>


bpSectionContainer::bpSectionContainer()
{
}


bpSectionContainer::~bpSectionContainer()
{
  Cleanup();
}


bpSectionContainer::bpSectionContainer(const bpSectionContainer& aOther)
{
  std::map<bpString,bpParameterSection*>::const_iterator vPos = aOther.mSectionMap.begin();

  while (vPos != aOther.mSectionMap.end()) {
    // create a new section as copy of the other
    bpParameterSection* vSection = new bpParameterSection(*((*vPos).second));
    // add it
    AddSection((*vPos).first,vSection);
    ++vPos;
  }
}


bpSectionContainer::bpSectionContainer(const std::map<bpString, std::map<bpString, bpString> >& aSectionMap)
{
  std::map<bpString, std::map<bpString, bpString> >::const_iterator vPos = aSectionMap.begin();

  while (vPos != aSectionMap.end()) {
    // create a new section as copy of the other
    bpParameterSection* vSection = new bpParameterSection(((*vPos).second));
    // add it
    AddSection((*vPos).first,vSection);
    ++vPos;
  }
}


bpSectionContainer& bpSectionContainer::operator=(const bpSectionContainer& aOther)
{
  if (this != &aOther) {
    Cleanup();
    std::map<bpString,bpParameterSection*>::const_iterator vPos = aOther.mSectionMap.begin();
    while (vPos != aOther.mSectionMap.end()) {
      // create a new section as copy of the other
      bpParameterSection* vSection = new bpParameterSection(*((*vPos).second));
      // add it
      AddSection((*vPos).first,vSection);
      ++vPos;
    }
  }
  return *this;
}


void bpSectionContainer::Merge(const bpSectionContainer& aOther)
{
  if (this != &aOther) {
    // we dont need to merge with ourselves
    std::map<bpString,bpParameterSection*>::const_iterator vPos = aOther.mSectionMap.begin();

    // look at all sections
    while (vPos != aOther.mSectionMap.end()) {
      if (HasSection((*vPos).first)) {
        // section exists, merge it
        bpParameterSection* vSection = GetSection((*vPos).first);
        const bpParameterSection* vOtherSection = ((*vPos).second);
        vSection->Merge(*vOtherSection);
      }
      else {
        // section does not exist, add it
        bpParameterSection* vSection = new bpParameterSection(*((*vPos).second));
        // add it
        AddSection((*vPos).first,vSection);
      }
      ++vPos;
    }
  }
}


bpParameterSection* bpSectionContainer::CreateSection(const bpString& aSectionName)
{
  // find a section with that name
  iterator vPos = mSectionMap.find(aSectionName);
  if (vPos == mSectionMap.end()) {
    // create a new section
    bpParameterSection* vSection = new bpParameterSection;
    mSectionMap[aSectionName] = vSection;
    return vSection;
  }
  else {
    // return the old section
    return(*vPos).second;
  }
}


bpParameterSection* bpSectionContainer::AddSection(const bpString& aSectionName, bpParameterSection* aSection)
{
  // find a section with that name
  iterator vPos = mSectionMap.find(aSectionName);
  if (vPos == mSectionMap.end()) {
    // insert the section
    mSectionMap[aSectionName] = aSection;
    return aSection;
  }
  else {
    // add parameters to the old section
    bpParameterSection* vNewSection = (*vPos).second;
    bpParameterSection::iterator vParameterIt = aSection->begin();
    while (vParameterIt != aSection->end()) {
      vNewSection->SetParameter((*vParameterIt).first,(*vParameterIt).second);
      ++vParameterIt;
    }
    // delete old section
    delete aSection;

    // Issue a warning message here to see where this happens (the called might
    // continue to use the supplied section)
    return vNewSection;
  }
}


bpParameterSection* bpSectionContainer::GetSection(const bpString& aSectionName) const
{
  std::map<bpString,bpParameterSection*>::const_iterator vPos = mSectionMap.find(aSectionName);
  if (vPos == mSectionMap.end()) {
    throw std::runtime_error("bpSectionContainer::GetSection: " + aSectionName);
  }
  return(*vPos).second;
}


bool bpSectionContainer::HasSection(const bpString& aSectionName) const
{
  return mSectionMap.find(aSectionName) != mSectionMap.end();
}


void bpSectionContainer::RemoveSection(const bpString& aSectionName)
{
  iterator vPos = mSectionMap.find(aSectionName);
  if (vPos != mSectionMap.end()) {
    bpParameterSection* vSection = (*vPos).second;
    // delete entry
    mSectionMap.erase(vPos);
    // delete section
    delete vSection;
  }
}


void bpSectionContainer::RenameSection(const bpString& aOldName, const bpString& aNewName)
{
  if (HasSection(aNewName)) {
    throw std::runtime_error("bpSectionContainer::RenameSection: cannot rename, section exist");
  }
  // find the old section
  iterator vPos = mSectionMap.find(aOldName);
  if (vPos == mSectionMap.end()) {
    // does not exist
    return;
  }
  bpParameterSection* vSection = (*vPos).second;
  // erase the old entry and add new
  mSectionMap.erase(vPos);
  AddSection(aNewName,vSection);
}


bpSectionContainer::iterator bpSectionContainer::begin()
{
  return mSectionMap.begin();
}


bpSectionContainer::iterator bpSectionContainer::end()
{
  return mSectionMap.end();
}


bpSectionContainer::const_iterator bpSectionContainer::begin() const
{
  return mSectionMap.begin();
}


bpSectionContainer::const_iterator bpSectionContainer::end() const
{
  return mSectionMap.end();
}


std::ostream& bpSectionContainer::Write(std::ostream& aOut)
{
  iterator vPos = begin();
  while (vPos != end()) {
    aOut << "[" << (*vPos).first.c_str()  << "]" << std::endl;
    ((*vPos).second)->Write(aOut);
    ++vPos;
  }
  return aOut;
}


bpString bpSectionContainer::ToString() const
{
  bpString vResult = "";
  std::map<bpString, bpParameterSection*>::const_iterator vPos = mSectionMap.begin();
  while (vPos != mSectionMap.end()) {
    vResult += "[" + vPos->first + "]\n";
    vResult += vPos->second->ToString();
    ++vPos;
  }
  return vResult;
}


const bpString& bpSectionContainer::GetParameter(const bpString& aSectionName, const bpString& aParameterName) const
{
  return GetSection(aSectionName)->GetParameter(aParameterName);
}


void bpSectionContainer::SetParameter(const bpString& aSectionName, const bpString& aParameterName, const bpString& aParameterValue)
{
  GetSection(aSectionName)->SetParameter(aParameterName,aParameterValue);
}


void bpSectionContainer::SetDefaultLocationParameter(const bpString& aSectionName, const bpString& aParameterName, const bpString& aParameterValue)
{
  //sets a default location value only if the current value is invalid

  if (!HasSection(aSectionName)) {
    CreateSection(aSectionName);
  }

  bool vIsValidLocation = true;

  if (!HasParameter(aSectionName, aParameterName)) {
    vIsValidLocation = false;
  }
  else {
    try {
      vIsValidLocation = bpFileTools::Exists(GetParameter(aSectionName, aParameterName));
    }
    catch (...) {
      vIsValidLocation = false;
    }
  }

  if (!vIsValidLocation) {
    SetParameter(aSectionName, aParameterName, aParameterValue);
  }
}


bool bpSectionContainer::HasParameter(const bpString& aSectionName, const bpString& aParameterName) const
{
  if (HasSection(aSectionName)) {
    return GetSection(aSectionName)->HasParameter(aParameterName);
  }
  else {
    return false;
  }
}


void bpSectionContainer::Cleanup()
{
  iterator vPos = begin();
  while (vPos != end()) {
    bpParameterSection* vSection = (*vPos).second;
    if (!vSection) {
      //BP_DEBUG_MSG("bpSectionContainer::Cleanup: Illegal section entry");
    }
    // delete entry
    mSectionMap.erase(vPos);
    // delete section
    delete vSection;
    vPos = begin();
  }
}
