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


#ifndef __BPTYPEDEFS__
#define __BPTYPEDEFS__


#if defined(__linux__)
#include <stdint.h>
#include <stdio.h>
#endif

#include <string>

using bpfString = std::string;
using bpfFloat = float;
using bpfDouble = double;
using bpfSize = size_t;

#if defined(_WIN32) || defined(__linux__) 
using bpfChar = char;
using bpfInt8 = signed char;
using bpfInt16 = short int;
using bpfInt32 = int;
using bpfInt64 = long long;
using bpfUChar = unsigned char;
using bpfUInt8 = unsigned char;
using bpfUInt16 = unsigned short int;
using bpfUInt32 = unsigned int;
using bpfUInt64 = unsigned long long;
#endif

#if defined(__APPLE__) 
using bpfChar = char;
using bpfInt8 = int8_t;
using bpfInt16 = int16_t;
using bpfInt32 = int32_t;
using bpfInt64 = int64_t;
using bpfUChar = unsigned char;
using bpfUInt8 = uint8_t;
using bpfUInt16 = uint16_t;
using bpfUInt32 = uint32_t;
using bpfUInt64 = uint64_t;
#endif

enum bpfNumberType {
  bpfNoType,
  bpfInt8Type,
  bpfUInt8Type,
  bpfInt16Type,
  bpfUInt16Type,
  bpfInt32Type,
  bpfUInt32Type,
  bpfFloatType,
  bpfDoubleType,
  bpfPackedRGBAType,
  bpfInt64Type,
  bpfUInt64Type
};


#endif
