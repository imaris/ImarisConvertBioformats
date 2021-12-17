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


#include "bpObjectDescriptor.h"

#include "bpUtils.h"


static std::map<bpObjectDescriptor::tType, bpString>& GetToStringMap()
{
  static std::map<bpObjectDescriptor::tType, bpString> vMap{
    { bpObjectDescriptor::eTypeUnknown, "Unknwon"},
    { bpObjectDescriptor::eTypeSpots, "Spots"},
    { bpObjectDescriptor::eTypeSurfaces, "Surfaces"},
    { bpObjectDescriptor::eTypeMegaSurfaces, "MegaSurfaces"},
    { bpObjectDescriptor::eTypeCells, "Cells"},
    { bpObjectDescriptor::eTypeFilaments, "Filaments"}
  };
  return vMap;
}


static std::map<bpString, bpObjectDescriptor::tType>& GetFromStringMap()
{
  static std::map<bpString, bpObjectDescriptor::tType> vMap{
    { "", bpObjectDescriptor::eTypeUnknown },
    { "Unknwon", bpObjectDescriptor::eTypeUnknown },
    { "Spots", bpObjectDescriptor::eTypeSpots },
    { "Surfaces", bpObjectDescriptor::eTypeSurfaces },
    { "MegaSurfaces", bpObjectDescriptor::eTypeMegaSurfaces },
    { "Cells", bpObjectDescriptor::eTypeCells },
    { "Filaments", bpObjectDescriptor::eTypeFilaments }
  };
  return vMap;
}


bpObjectDescriptor::bpObjectDescriptor()
  : mName(),
    mType(eTypeUnknown),
    mId(0),
    mHasLabelsDescription(false),
    mCreatorName(""),
    mCreationParameters("")
{
}


bpObjectDescriptor::bpObjectDescriptor(const bpObjectDescriptor& aOther)
  : mName(aOther.mName),
    mType(aOther.mType),
    mId(aOther.mId),
    mLabels(aOther.mLabels),
    mHasLabelsDescription(aOther.mHasLabelsDescription),
    mCreatorName(aOther.mCreatorName),
    mCreationParameters(aOther.mCreationParameters)
{
}


bpObjectDescriptor::bpObjectDescriptor(const bpString& aName, tType aType, bpId aId, const bpString& aCreatorName, const bpString& aCreationParameters)
  : mName(aName),
    mType(aType),
    mId(aId),
    mHasLabelsDescription(false),
    mCreatorName(aCreatorName),
    mCreationParameters(aCreationParameters)
{
}


bpObjectDescriptor::bpObjectDescriptor(const bpString& aName, tType aType, bpId aId, const LabelsDescription& aLabels, const bpString& aCreatorName, const bpString& aCreationParameters)
  : mName(aName),
    mType(aType),
    mId(aId),
    mLabels(aLabels),
    mHasLabelsDescription(true),
    mCreatorName(aCreatorName),
    mCreationParameters(aCreationParameters)
{
}


bpObjectDescriptor::~bpObjectDescriptor()
{
}

bool bpObjectDescriptor::IsValid() const
{
  return !(mName == "" && mType == eTypeUnknown && mId == 0);
}

bool bpObjectDescriptor::HasLabelsDescription() const
{
  return mHasLabelsDescription;
}


const bpString& bpObjectDescriptor::GetName() const
{
  return mName;
}


bpObjectDescriptor::tType bpObjectDescriptor::GetType() const
{
  return mType;
}

bpString bpObjectDescriptor::GetTypeString() const
{
  return GetToStringMap()[mType];
}

bpId bpObjectDescriptor::GetId() const
{
  return mId;
}

bpObjectDescriptor::LabelsDescription bpObjectDescriptor::GetLabels() const
{
  return mLabels;
}

bpString bpObjectDescriptor::ToString() const
{
  return "[mName, mType, mId] = [" + mName + ", " + GetToStringMap()[mType] + ", " + bpToString(mId) + "]";
}

const bpString& bpObjectDescriptor::GetCreatorName() const
{
  return mCreatorName;
}

const bpString& bpObjectDescriptor::GetCreationParameters() const
{
  return mCreationParameters;
}

bpObjectDescriptor::tType bpObjectDescriptor::GetTypeFromString(const bpString& aStringType)
{
  return GetFromStringMap()[aStringType];
}
