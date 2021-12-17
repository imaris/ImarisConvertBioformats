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


#ifndef __BP_TYPES__
#define __BP_TYPES__


#if ! defined (BP_DEBUG_MSG)
#define BP_DEBUG_MSG(aMessage)
#endif

#if ! defined (BP_TRACE_MSG)
#define BP_TRACE_MSG(aLevel, aMessage)
#endif


#include "fileiobase/types/bpfSmartPtr.h"
#include "fileiobase/types/bpfTypedefs.h"

#include <sstream>
#include <string.h>
#include <algorithm>
#include <limits>
#include <complex>

/*
 * Definitions of standard types by number of bits.
 * These should mainly be used for data sets.
 */
#if defined(_WIN32)

#define rint(a) ((int)(a+0.5))
#define lrint(a) rint(a)
#define lrintf(a) rint(a)

#if !defined(M_LN2)
#define M_LN2 0.6931471805599f
#endif
#if !defined(M_PI)
#define M_PI 3.14159265358979323846f
#endif
#if !defined(M_SQRT1_2)
#define M_SQRT1_2 0.70710678118654752440f
#endif
#if !defined(M_SQRT2)
#define M_SQRT2 1.41421356237309504880f
#endif
#endif


/**
 * Get the type of a number
 */
template <typename T>
inline bpfNumberType bpfGetNumberType();


/**
 * Return the number of bytes for the type indicated by the parameter.
 *
 * If the type is bpfNoType, then 0 is returned.
 *
 * @param type   The type.
 * @return The size for the type.
 */
bpfUInt32 bpfGetSizeOfType(bpfNumberType type);


/**
 * Defines the smallest and the biggest value presentable by the specified type (ANSI limits)
 * If its senseless to use this values (Example: bpfFloat and bpfDouble) both values are same (zero)
 * @param 1 the smallest value
 * @param 2 the biggest value
 */
//
// Using ANSI limits
//
inline void bpfTypeRange(bpfUInt8&  s, bpfUInt8&  b)
{
  s = 0;
  b = 255;
};
inline void bpfTypeRange(bpfUInt16& s, bpfUInt16& b)
{
  s = 0;
  b = 65535U;
};
inline void bpfTypeRange(bpfUInt32& s, bpfUInt32& b)
{
  s = 0;
  b = 4294967295UL;
};
inline void bpfTypeRange(bpfUInt64& s, bpfUInt64& b)
{
  s = 0;
  b = 18446744073709551615ULL;
};
inline void bpfTypeRange( bpfInt8&  s,  bpfInt8&  b)
{
  s = (-128);
  b = 127;
};
inline void bpfTypeRange( bpfInt16& s,  bpfInt16& b)
{
  s = (-32768);
  b = 32767;
};
inline void bpfTypeRange( bpfInt32& s,  bpfInt32& b)
{
  s = (-2147483647L - 1L);
  b = 2147483647L;
};
inline void bpfTypeRange( bpfInt64& s,  bpfInt64& b)
{
  s = (-9223372036854775807LL - 1LL);
  b = 9223372036854775807LL;
};

#if defined(__APPLE__)
inline void bpfTypeRange(bpfSize& s, bpfSize& b)
{
  bpfTypeRange((bpfUInt32&)s, (bpfUInt32&)b);
}
#endif // __APPLE__

//
// No ANSI limits for rationals
//
inline void bpfTypeRange(bpfFloat&  s, bpfFloat& b)
{
  s = 0.0f;
  b = 1.0f;
}
inline void bpfTypeRange(bpfDouble& s, bpfDouble& b)
{
  s = 0.0;
  b = 1.0;
}



/**
 * Round a numeric input value to another numeric output value
 *
 * @param input The value to be rounded.
 * @param output The result of the rounding.
 */
inline void bpfRound(bpfFloat input, bpfDouble& output);
inline void bpfRound(bpfFloat input, bpfFloat& output);
inline void bpfRound(bpfFloat input, bpfUInt64& output);
inline void bpfRound(bpfFloat input, bpfUInt32& output);
inline void bpfRound(bpfFloat input, bpfUInt16& output);
inline void bpfRound(bpfFloat input, bpfUInt8& output);
inline void bpfRound(bpfFloat input, bpfInt64& output);
inline void bpfRound(bpfFloat input, bpfInt32& output);
inline void bpfRound(bpfFloat input, bpfInt16& output);
inline void bpfRound(bpfFloat input, bpfInt8& output);

inline void bpfRound(bpfDouble input, bpfDouble& output);
inline void bpfRound(bpfDouble input, bpfFloat& output);
inline void bpfRound(bpfDouble input, bpfUInt64& output);
inline void bpfRound(bpfDouble input, bpfUInt32& output);
inline void bpfRound(bpfDouble input, bpfUInt16& output);
inline void bpfRound(bpfDouble input, bpfUInt8& output);
inline void bpfRound(bpfDouble input, bpfInt64& output);
inline void bpfRound(bpfDouble input, bpfInt32& output);
inline void bpfRound(bpfDouble input, bpfInt16& output);
inline void bpfRound(bpfDouble input, bpfInt8& output);

#if defined(__APPLE__)
inline void bpfRound(bpfFloat input, bpfSize& output);
inline void bpfRound(bpfDouble input, bpfSize& output);
#endif

using bpfWString = std::wstring;

// new concatenation operator for strings...

/**
 * Append an integer to a string.
 *
 * @param s The original string.
 * @param i The integer that is appended.
 * @return A string with i appended to s
 */
bpfString operator+(const bpfString& s,bpfInt32 i);
bpfString operator+(const bpfString& s,bpfUInt32 i);
bpfString operator+(const bpfString& s,bpfInt64 i);
bpfString operator+(const bpfString& s,bpfUInt64 i);


/**
 * Convert a type to a string. This must be defined for all types used as
 * template argument in bpfImageFields.
 *
 * @param value The value to be converted to a string.
 * @return The converted string.
 */
bpfString bpfToString(bpfUInt8 value);
bpfString bpfToString(bpfInt8 value);
bpfString bpfToString(bpfUInt16 value);
bpfString bpfToString(bpfInt16 value);
bpfString bpfToString(bpfUInt32 value);
bpfString bpfToString(bpfInt32 value);
bpfString bpfToString(bpfUInt64 value);
bpfString bpfToString(bpfInt64 value);
bpfString bpfToString(bpfFloat value, bpfInt16 aFixedPrecision = 3);
bpfString bpfToString(bpfDouble value, bpfInt16 aFixedPrecision = 3);
bpfString bpfToString(bool value);
bpfString bpfToString(bpfNumberType value);
inline bpfString bpfToString(bpfString value)
{
  return value;
};

#if !defined(_WIN32)
bpfString bpfToString(bpfSize aValue);
#endif

/**
 * calculate the number of decimals required for a given precision
 * the precision is 10 ^ (-aPrecision)
 * an example call for an output with fixed precision of 1 0/oo:
 * bpfString vMyOutput = bpfToString( vMyValue, bpfPrecisionToDecimals(vMyValue) )
 **/
bpfUInt32 bpfPrecisionToDecimals(bpfFloat aValue, bpfUInt32 aPrecision = 3);
bpfUInt32 bpfPrecisionToDecimals(bpfDouble aValue, bpfUInt32 aPrecision = 3);

template <class Element>
bpfString bpfToString(Element aBeginIterator, Element aEndIterator, bpfString aDelimiter = " ");


//
// inline functions definitions
//


/**
 * Convert a string to a type.
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Type>
inline void bpfFromString(const bpfString& aString, Type& aResult)
{
  std::istringstream i(aString);
  i >> aResult;
}

template <>
inline void bpfFromString(const bpfString& aString, bpfUInt8& aResult)
{
  unsigned int vResultInt;
  std::istringstream i(aString);
  i >> vResultInt;
  aResult = (bpfUInt8) vResultInt;
}

template <>
inline void bpfFromString(const bpfString& aString, bpfInt8& aResult)
{
  int vResultInt;
  std::istringstream i(aString);
  i >> vResultInt;
  aResult = (bpfInt8) vResultInt;
}

template <>
inline void bpfFromString(const bpfString& aString, bpfNumberType& aNumberType)
{
  if (aString == "NoType") {
    aNumberType = bpfNoType;
  }
  else if (aString == "Int8") {
    aNumberType = bpfInt8Type;
  }
  else if (aString == "UInt8") {
    aNumberType = bpfUInt8Type;
  }
  else if (aString == "Int16") {
    aNumberType = bpfInt16Type;
  }
  else if (aString == "UInt16") {
    aNumberType = bpfUInt16Type;
  }
  else if (aString == "Int32") {
    aNumberType = bpfInt32Type;
  }
  else if (aString == "UInt32") {
    aNumberType = bpfUInt32Type;
  }
  else if (aString == "Int64") {
    aNumberType = bpfInt64Type;
  }
  else if (aString == "UInt64") {
    aNumberType = bpfUInt64Type;
  }
  else if (aString == "Float") {
    aNumberType = bpfFloatType;
  }
  else if (aString == "Double") {
    aNumberType = bpfDoubleType;
  }
  else if (aString == "PackedRGBA") {
    aNumberType = bpfPackedRGBAType;
  }
}


/**
 * Convert a string to a type.
 *
 * @param string The string from which to get the value
 * @return result The value
 */
template <typename T>
inline T bpfFromString(const bpfString& aString)
{
  T vResult;
  bpfFromString(aString, vResult);
  return vResult;
}


/**
 * Convert a string to a type. (serialization for bool)
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
inline void bpfFromString(const bpfString& string,bool& result)
{
  if ((string == "true") || (string == "1")) {
    result = true;
  }
  else if ((string == "false") || (string == "0")) {
    result = false;
  }
  else {
    result = false;
  }
}

/**
 * Convert a string to a type. (serialization for bpfString)
 * This routine is provided for compatibility reasons
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 *
 */
inline void bpfFromString(const bpfString& aString,bpfString& aResult)
{
  aResult = aString;
}

/**
 * Round a numeric input value to another numeric output value
 *
 * @param input The value to be rounded.
 * @param output The result of the rounding.
 */
inline void bpfRound(bpfFloat input, bpfDouble& output)
{
  output = (bpfDouble) input;
}

inline void bpfRound(bpfFloat input, bpfFloat& output)
{
  output = input;
}

inline void bpfRound(bpfFloat input, bpfUInt32& output)
{
  output = (bpfUInt32) (input + 0.5f);
}

inline void bpfRound(bpfFloat input, bpfUInt64& output)
{
  output = (bpfUInt64) (input + 0.5f);
}

inline void bpfRound(bpfFloat input, bpfUInt16& output)
{
  output = (bpfUInt16) (input + 0.5f);
}

inline void bpfRound(bpfFloat input, bpfUInt8& output)
{
  output = (bpfUInt8) (input + 0.5f);
}

inline void bpfRound(bpfFloat input, bpfInt32& output)
{
  output = input >= 0.0f ? (bpfInt32) (input + 0.5f) : (bpfInt32) (input - 0.5f);
}

inline void bpfRound(bpfFloat input, bpfInt64& output)
{
  output = input >= 0.0f ? (bpfInt64) (input + 0.5f) : (bpfInt64) (input - 0.5f);
}

inline void bpfRound(bpfFloat input, bpfInt16& output)
{
  output = input >= 0.0f ? (bpfInt16) (input + 0.5f) : (bpfInt16) (input - 0.5f);
}

inline void bpfRound(bpfFloat input, bpfInt8& output)
{
  output = input >= 0.0f ? (bpfInt8) (input + 0.5f) : (bpfInt8) (input - 0.5f);
}


inline void bpfRound(bpfDouble input, bpfDouble& output)
{
  output = input;
}

inline void bpfRound(bpfDouble input, bpfFloat& output)
{
  output = (bpfFloat) input;
}

inline void bpfRound(bpfDouble input, bpfUInt32& output)
{
  output = (bpfUInt32) (input + 0.5);
}

inline void bpfRound(bpfDouble input, bpfUInt64& output)
{
  output = (bpfUInt64) (input + 0.5);
}

inline void bpfRound(bpfDouble input, bpfUInt16& output)
{
  output = (bpfUInt16) (input + 0.5);
}

inline void bpfRound(bpfDouble input, bpfUInt8& output)
{
  output = (bpfUInt8) (input + 0.5);
}

inline void bpfRound(bpfDouble input, bpfInt32& output)
{
  output = input >= 0.0 ? (bpfInt32) (input + 0.5) : (bpfInt32) (input - 0.5);
}

inline void bpfRound(bpfDouble input, bpfInt64& output)
{
  output = input >= 0.0 ? (bpfInt64) (input + 0.5) : (bpfInt64) (input - 0.5);
}

inline void bpfRound(bpfDouble input, bpfInt16& output)
{
  output = input >= 0.0 ? (bpfInt16) (input + 0.5) : (bpfInt16) (input - 0.5);
}

inline void bpfRound(bpfDouble input, bpfInt8& output)
{
  output = input >= 0.0 ? (bpfInt8) (input + 0.5) : (bpfInt8) (input - 0.5);
}

#if defined(__APPLE__)
inline void bpfRound(bpfFloat input, bpfSize& output)
{
  output = input >= 0.0 ? (bpfSize) (input + 0.5) : (bpfSize) (input - 0.5);
}

inline void bpfRound(bpfDouble input, bpfSize& output)
{
  output = input >= 0.0 ? (bpfSize) (input + 0.5) : (bpfSize) (input - 0.5);
}
#endif // __APPLE__
/**
 * Get the type of a number
 */

/*inline bpfNumberType bpfGetNumberType(bpfInt8 vValue)
{
  return bpfInt8Type;
}

inline bpfNumberType bpfGetNumberType(bpfUInt8 vValue)
{
  return bpfUInt8Type;
}

inline bpfNumberType bpfGetNumberType(bpfInt16 vValue)
{
  return bpfInt16Type;
}

inline bpfNumberType bpfGetNumberType(bpfUInt16 vValue)
{
  return bpfUInt16Type;
}

inline bpfNumberType bpfGetNumberType(bpfInt32 vValue)
{
  return bpfInt32Type;
}

inline bpfNumberType bpfGetNumberType(bpfUInt32 vValue)
{
  return bpfUInt32Type;
}

inline bpfNumberType bpfGetNumberType(bpfInt64 vValue)
{
  return bpfInt64Type;
}

inline bpfNumberType bpfGetNumberType(bpfUInt64 vValue)
{
  return bpfUInt64Type;
}

inline bpfNumberType bpfGetNumberType(bpfFloat vValue)
{
  return bpfFloatType;
}

inline bpfNumberType bpfGetNumberType(bpfDouble vValue)
{
  return bpfDoubleType;
}*/

template <>
inline bpfNumberType bpfGetNumberType<bpfInt8>()
{
  return bpfInt8Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfUInt8>()
{
  return bpfUInt8Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfInt16>()
{
  return bpfInt16Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfUInt16>()
{
  return bpfUInt16Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfInt32>()
{
  return bpfInt32Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfUInt32>()
{
  return bpfUInt32Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfInt64>()
{
  return bpfInt64Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfUInt64>()
{
  return bpfUInt64Type;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfFloat>()
{
  return bpfFloatType;
}

template <>
inline bpfNumberType bpfGetNumberType<bpfDouble>()
{
  return bpfDoubleType;
}

/**
 * Converts a bpfSize type to a bpfUInt32 type.
 *
 * bpfSize is an unsigned int that is 4 or 8 bytes wide depending
 * on the environment.
 *
 */
// Disable warnings temporarily
//
// 4244 = 'variable' : conversion from 'type' to 'type', possible loss of data
//        => You assigned a value of type __int64 to a variable of type unsigned int.
//           A possible loss of data may have occurred.
//     Doing a cast like below eliminates this waring. Therefore it is not necessary
//     to disable this one
//
// 4311 = 'variable' : pointer truncation from 'type' to 'type'
//        => A 64-bit pointer was truncated to a 32-bit int or 32-bit long.
//     Doing a cast like below eliminates this waring. Therefore it is not necessary
//     to disable this one
//
// 4267 = 'argument'     : conversion from 'type' to 'type', possible loss of data
//        'return'       : conversion from 'type' to 'type', possible loss of data
//        '='            : conversion from 'type' to 'type', possible loss of data
//        'initializing' : conversion from 'type' to 'type', possible loss of data
//     Doing a cast like below eliminates this waring. Therefore it is not necessary
//     to disable this one

//#pragma warning( disable : 4244 4311 4267 )
inline bpfUInt32 bpfToUInt32(bpfSize aSize)
{
  return (bpfUInt32) aSize;
};
//#pragma warning( default : 4244 4311 4267 )

#endif // __BP_TYPES__
