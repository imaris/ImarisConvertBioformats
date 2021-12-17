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


#ifndef __BPOBJECTDESCRIPTOR__
#define __BPOBJECTDESCRIPTOR__

#include "ImarisWriter/interface/bpConverterTypes.h"


class bpObjectDescriptor
{
public:

  typedef std::pair<bpString, bpString> LabelDescription;
  typedef std::vector<LabelDescription> LabelsDescription;

  enum tType
  {
    eTypeUnknown,
    eTypeSpots,
    eTypeSurfaces,
    eTypeCells,
    eTypeFilaments,
    eTypeMegaSurfaces
  };

  bpObjectDescriptor();
  bpObjectDescriptor(const bpObjectDescriptor& aOther);
  bpObjectDescriptor(const bpString& aName, tType aType, bpId aId, const bpString& aCreatorName, const bpString& aCreationParameters);
  bpObjectDescriptor(const bpString& aName, tType aType, bpId aId, const LabelsDescription& aLabels, const bpString& aCreatorName, const bpString& aCreationParameters);
  ~bpObjectDescriptor();

  const bpString& GetName() const;
  tType GetType() const;
  bpString GetTypeString() const;
  bpId GetId() const;
  LabelsDescription GetLabels() const;
  const bpString& GetCreatorName() const;
  const bpString& GetCreationParameters() const;

  bpString ToString() const;

  bool IsValid() const;
  // mLabels could be empty because SurpassObject has no label, or because labels hasn't been set
  // This function allows to verify if labels have been set
  bool HasLabelsDescription() const;
  static tType GetTypeFromString(const bpString& aStringType);

private:
  bpString mName;
  bpString mCreatorName;
  tType mType;
  bpId mId;
  LabelsDescription mLabels;
  bool mHasLabelsDescription;
  bpString mCreationParameters;
};
#endif
