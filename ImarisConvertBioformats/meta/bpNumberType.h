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


#ifndef __BP_NUMBER_TYPE__
#define __BP_NUMBER_TYPE__


#include "bpUtils.h"


enum bpNumberType {
  bpNoType,
  bpInt8Type,
  bpUInt8Type,
  bpInt16Type,
  bpUInt16Type,
  bpInt32Type,
  bpUInt32Type,
  bpFloatType,
  bpDoubleType,
  bpInt64Type,
  bpUInt64Type
};


bpUInt32 bpGetSizeOfType(bpNumberType type);


template <>
void bpFromString(const bpString& aString, bpNumberType& aNumberType);


bpString bpToString(bpNumberType value);


#endif
