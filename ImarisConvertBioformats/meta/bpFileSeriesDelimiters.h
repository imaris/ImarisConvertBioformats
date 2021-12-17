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


#ifndef __BPFILESERIESDELIMITERS__
#define __BPFILESERIESDELIMITERS__


#include "ImarisWriter/interface/bpConverterTypes.h"


#include <map>


namespace bpFileSeriesDelimitersConstant
{
  static const bpString mDefaultX = "_X;-X";
  static const bpString mDefaultY = "_Y;-Y";
  static const bpString mDefaultZ = "_Z;_S;-Z";
  static const bpString mDefaultC = "_C;_CH;_W;-C";
  static const bpString mDefaultT = "_T;_TP;-T";
  static const bpString mDefaultF;
}


class bpFileSeriesDelimiters
{
public:
  enum class tDimension {
    eX,
    eY,
    eZ,
    eC,
    eT,
    eF
  };


  using tDelimiters = std::map<tDimension, bpString>;


  bpFileSeriesDelimiters();
  ~bpFileSeriesDelimiters() = default;

  const bpString& GetDelimiters(tDimension aDimension) const;
  const tDelimiters& GetDelimiters() const;
  void SetDelimiters(const tDelimiters& aDelimiters);

private:
  void Reset();

  tDelimiters mDelimiters;
};


#endif
