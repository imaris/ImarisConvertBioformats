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


#include "bpFileSeriesDelimiters.h"


bpFileSeriesDelimiters::bpFileSeriesDelimiters()
{
  Reset();
}


const bpString& bpFileSeriesDelimiters::GetDelimiters(tDimension aDimension) const
{
  return mDelimiters.at(aDimension);
}


const bpFileSeriesDelimiters::tDelimiters& bpFileSeriesDelimiters::GetDelimiters() const
{
  return mDelimiters;
}


void bpFileSeriesDelimiters::SetDelimiters(const tDelimiters& aDelimiters)
{
  if (6 == aDelimiters.size()) {
    mDelimiters = aDelimiters;
  }
}


void bpFileSeriesDelimiters::Reset()
{
  mDelimiters = { { tDimension::eX, bpFileSeriesDelimitersConstant::mDefaultX },
                  { tDimension::eY, bpFileSeriesDelimitersConstant::mDefaultY },
                  { tDimension::eZ, bpFileSeriesDelimitersConstant::mDefaultZ },
                  { tDimension::eC, bpFileSeriesDelimitersConstant::mDefaultC },
                  { tDimension::eT, bpFileSeriesDelimitersConstant::mDefaultT },
                  { tDimension::eF, bpFileSeriesDelimitersConstant::mDefaultF } };
}
