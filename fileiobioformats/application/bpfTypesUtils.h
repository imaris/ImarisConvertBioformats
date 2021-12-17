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


#ifndef __BP_TYPEUTILS__
#define __BP_TYPEUTILS__

#include "fileiobase/types/bpfTypes.h"
#include <string.h>


// Working with the type size_t in the functions prinft, scanf and similar functions
// Windows: size_t s = 1; printf("%Iu", s);
// MAC    : size_t s = 1; printf("%zu", s);
#if defined(_WIN32)
#define BP_SIZET_FORMAT "I"
#else
#define BP_SIZET_FORMAT "z"
#endif

#if defined(_WIN32)
#define BP_BIGENDIAN 1
#define BP_LITTLEENDIAN 0
#define BPOPEN (O_BINARY | O_RDONLY)
#define BPIOSOPEN (0200)
#include <io.h>

#elif defined(__GNUC__)
#ifdef __BIG_ENDIAN__   // this is defined by gcc
#define BP_BIGENDIAN 0
#define BP_LITTLEENDIAN 1
#else // __BIG_ENDIAN__
#define BP_BIGENDIAN 1
#define BP_LITTLEENDIAN 0
#endif // __BIG_ENDIAN__

#define BPOPEN (O_RDONLY)
#define BPIOSOPEN (ios::nocreate)

#else
#ifndef __INCLUDE_SEX_H__
/*
 * Byte sex constants
 */
#define BP_BIGENDIAN     0
#define BP_LITTLEENDIAN  1
#define BP_UNKNOWNENDIAN 2
#endif

#include <sstream>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#define BPOPEN (O_RDONLY)
#define BPIOSOPEN (ios::nocreate)
#endif

#define BPMIN(a,b) (((a)<(b))?(a):(b))
#define BPMAX(a,b) (((a)>(b))?(a):(b))

template <class T>
void bpfSwapVal(T* dat)
{
  std::reverse(reinterpret_cast<bpfUChar*>(dat),
               reinterpret_cast<bpfUChar*>(dat) + sizeof(T));
}


template <class T>
T bpfSwapLittleEndian(T* val)
{
#if (BP_BIGENDIAN != 0)
  bpfSwapVal(val);
#endif
  return *val;
}


template <class T>
T bpfSwapBigEndian(T* val)
{
#if (BP_LITTLEENDIAN != 0)
  bpfSwapVal(val);
#endif
  return *val;
}


template <class T>
T bpfSwapEndian(T* val, bool islittle)
{
  if (islittle) return bpfSwapBigEndian(val);
  else return bpfSwapLittleEndian(val);
}


template <class T>
T bpfReadLittleEndian(T val)
{
  T dummy = val;
#if BP_BIGENDIAN != 0
  bpfSwapVal(&dummy);
#endif
  return dummy;
}


template <class T>
T bpfReadBigEndian(T val)
{
  T dummy = val;
#if BP_LITTLEENDIAN != 0
  bpfSwapVal(&dummy);
#endif
  return dummy;
}


template <class T>
void bpfStrtoVal(const bpfString& pstr, T* val)
{
  if (pstr.empty()) return;
  std::istringstream strdat(pstr);
  while (strdat >> *val) {
    ;
  }
  return;
}


void bpfStrtoVal(const bpfString&, char*);


template <class T>
void bpfBinToStruct(T* strucd, char** bind)
{
  memcpy((void*)strucd, (void*)(*bind), sizeof(T));
  *bind += sizeof(T);
  return;
}


template <class T>
void bpfStrctCnvrtBig(T* strucd, char** bind)
{
  memcpy((void*)strucd, (void*)(*bind), sizeof(T));
  *bind += sizeof(T);
  bpfSwapBigEndian(strucd);
  return;
}


template <class T>
void bpfStrctCnvrtLittle(T* strucd, char** bind)
{
  memcpy((void*)strucd, (void*)(*bind), sizeof(T));
  *bind += sizeof(T);
  bpfSwapLittleEndian(strucd);
  return;
}


template <class T>
void bpfStrctCnvrt(T* strucd, char** bind, bool islittle)
{
  if (islittle) bpfStrctCnvrtBig(strucd, bind);
  else bpfStrctCnvrtLittle(strucd, bind);
  return;
}


template <class T>
void bpfSourceCnvrtBig(T* strucd, char** bind)
{
  T conv = *strucd;
  bpfSwapBigEndian(&conv);
  memcpy((void*)(*bind), (void*)(&conv), sizeof(T));
  *bind += sizeof(T);
  return;
}


template <class T>
void bpfSourceCnvrtLittle(T* strucd, char** bind)
{
  T conv = *strucd;
  bpfSwapBigEndian(&conv);
  memcpy((void*)(*bind), (void*)(&conv), sizeof(T));
  *bind += sizeof(T);
  return;
}


template <class T>
void bpfSourceCnvrt(T* strucd, char** bind, bool islittle)
{
  if (islittle) bpfSourceCnvrtBig(strucd, bind);
  else bpfSourceCnvrtLittle(strucd, bind);
  return;
}

#endif
