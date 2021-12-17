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


#include "bpNumberType.h"


bpUInt32 bpGetSizeOfType(bpNumberType type)
{
  switch (type) {
  case bpInt8Type:
    return sizeof(bpInt8);
  case bpUInt8Type:
    return sizeof(bpUInt8);
  case bpInt16Type:
    return sizeof(bpInt16);
  case bpUInt16Type:
    return sizeof(bpUInt16);
  case bpInt32Type:
    return sizeof(bpInt32);
  case bpUInt32Type:
    return sizeof(bpUInt32);
  case bpInt64Type:
    return sizeof(bpInt64);
  case bpUInt64Type:
    return sizeof(bpUInt64);
  case bpFloatType:
    return sizeof(bpFloat);
  case bpDoubleType:
    return sizeof(bpDouble);
  default:
    return 0;
  }
}


template <>
void bpFromString(const bpString& aString, bpNumberType& aNumberType)
{
  if (aString == "NoType") {
    aNumberType = bpNoType;
  }
  else if (aString == "Int8") {
    aNumberType = bpInt8Type;
  }
  else if (aString == "UInt8") {
    aNumberType = bpUInt8Type;
  }
  else if (aString == "Int16") {
    aNumberType = bpInt16Type;
  }
  else if (aString == "UInt16") {
    aNumberType = bpUInt16Type;
  }
  else if (aString == "Int32") {
    aNumberType = bpInt32Type;
  }
  else if (aString == "UInt32") {
    aNumberType = bpUInt32Type;
  }
  else if (aString == "Int64") {
    aNumberType = bpInt64Type;
  }
  else if (aString == "UInt64") {
    aNumberType = bpUInt64Type;
  }
  else if (aString == "Float") {
    aNumberType = bpFloatType;
  }
  else if (aString == "Double") {
    aNumberType = bpDoubleType;
  }
}


bpString bpToString(bpNumberType aNumberType)
{
  switch (aNumberType) {
  case bpNoType:
    return "NoType";
  case bpInt8Type:
    return "Int8";
  case bpUInt8Type:
    return "UInt8";
  case bpInt16Type:
    return "Int16";
  case bpUInt16Type:
    return "UInt16";
  case bpInt32Type:
    return "Int32";
  case bpUInt32Type:
    return "UInt32";
  case bpInt64Type:
    return "Int64";
  case bpUInt64Type:
    return "UInt64";
  case bpFloatType:
    return "Float";
  case bpDoubleType:
    return "Double";
  default:
    return "";
  }
}
