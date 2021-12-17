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


#ifndef __BP_VECTOR4__
#define __BP_VECTOR4__


#include "fileiobase/types/bpfTypes.h"


/**
 *
 * Generic class for vectors of length 4. Arithmetic and relational
 * functions are provided. The class has no private or virtual
 * functions.  It can be used as the type argument to a bpImage.
 *
 *  @see bpfVector3
 *
 */
template <class Element>
class bpfVector4
{
public:

  /**
   * Default Constructor. The vector is set to (0,0,0,0).
   */
  inline bpfVector4();
  /**
   * Constructor to set the vector from the arguments.
   *
   * @param a0 The component with index 0 (first).
   * @param a1 The component with index 1 (second).
   * @parma a2 The component with index 2 (third).
   * @parma a3 The component with index 3 (fourth).
   */
  inline bpfVector4(Element a0,Element a1,Element a2, Element a3);
  /*
   * The default copy constructor will be used.
   *
   * No destructor is needed.
   *
   * The default assignment operator (memberwise assignment) will be used.
   *
   */

  /**
   * Access. No range checking is done.
   *
   * @return The i'th component.
   */
  inline Element& operator[](bpfSize aIndex);
  /**
    * Access for const objects.
    */
  inline const Element& operator[](bpfSize aIndex) const;

  /**
   * Equality operator. Two vectors are equal if all components are equal.
   *
   * @param other Vector to compare with.
   */
  inline bool operator==(const bpfVector4<Element>& aOther) const;
  /**
   * Non-equality operator.
   *
   * @param other Vector to compare with.
   */
  inline bool operator!=(const bpfVector4<Element>& aOther) const;

  /*
   * Arithmetic operations
   *
   * All arithmetic operations between vectors act componentwise.
   *
   */
  /**
   * @name Unary operations.
   */
  //@{
  /**
   * Addition. Calculates *this[i] += arg[i] for all components.
   *
   * @param arg The argument.
   */
  inline bpfVector4<Element>& operator+=(const bpfVector4<Element>& aArgument);
  /**
   * Subtraction. Calculates *this[i] -= arg[i] for all components.
   *
   * @param arg The argument.
   */
  inline bpfVector4<Element>& operator-=(const bpfVector4<Element>& aArgument);
  /**
   * Scalar Multiplication. Calculates *this[i] *= mult for all components.
   *
   * @param mult The multiplier.
   */
  inline bpfVector4<Element>& operator*=(Element aMultiplicator);

  /**
   * Scalar Division. Calculates *this[i] /= mult for all components.
   *
   * @param div The divisor.
   */
  inline bpfVector4<Element>& operator/=(Element aDivisor);
  //@}

  /*
   * Vector Operations
   */

  /**
   * Returns the length of the vector.
   *
   * @return The length of the vector.
   */
  inline Element Length() const;

  /**
   * Normalize the vector to unit length (if it has not zero length).
   */
  inline void Normalize();

  /**
   * Negate all components of the vector.
   */
  inline void Negate();

private:
  Element mElement[4];

};


using bpfVector4Int8 = bpfVector4<bpfInt8>;
using bpfVector4Int16 = bpfVector4<bpfInt16>;
using bpfVector4Int32 = bpfVector4<bpfInt32>;
using bpfVector4Int64 = bpfVector4<bpfInt64>;
using bpfVector4UInt8 = bpfVector4<bpfUInt8>;
using bpfVector4UInt16 = bpfVector4<bpfUInt16>;
using bpfVector4UInt32 = bpfVector4<bpfUInt32>;
using bpfVector4UInt64 = bpfVector4<bpfUInt64>;
using bpfVector4Float = bpfVector4<bpfFloat>;
using bpfVector4Double = bpfVector4<bpfDouble>;
using bpfVector4Bool = bpfVector4<bool>;
using bpfVector4Size = bpfVector4<bpfSize>;
using bpfVector4String = bpfVector4<bpfString>;
using bpfVector4NumberType = bpfVector4<bpfNumberType>;


/**
 * Output to a stream in the format (x1,x2,x3,x4).
 */
template <class Element>
inline std::ostream& operator<<(std::ostream&,const bpfVector4<Element>&);


/**
 * Convert a string to a bpfVector4
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& aString, bpfVector4<Element>& aResult);


/**
 * Convert a bpfVector4 to a bpfString.
 *
 * @param vec The vector to convert.
 * @return The string (OUT)
 */
template <class Element>
inline bpfString bpfToString(const bpfVector4<Element>& vec);

inline bpfString bpfToString(const bpfVector4<bpfFloat>& vec, bpfInt16 precision = 8);
inline bpfString bpfToString(const bpfVector4<bpfDouble>& vec, bpfInt16 precision = 8);

/*
 * Unary and Binary operations
 */

/**
 * Unary minus (returns a new vector).
 *
 * @return The negated vector.
 */
template <class Element>
inline bpfVector4<Element> operator-(const bpfVector4<Element>&);
/**
 * Vector addition. Returns the vector with components summand1[i] +
 * summand2[i].
 *
 * @param summand1 The first summand.
 * @param summand2 The second summand.
 * @return The addition of summand1 and summand2.
 */
template <class Element>
inline bpfVector4<Element> operator+(const bpfVector4<Element>& summand1,
                                     const bpfVector4<Element>& summand2);
/**
 * Vector subtraction. Returns the vector with components arg1[i] -
 * arg2[i].
 *
 * @param arg1 The first argument (from which will be subtracted).
 * @param arg2 The second argument (that will be subtracted);
 * @return The subtraction of arg2 from arg1.
 */
template <class Element>
inline bpfVector4<Element> operator-(const bpfVector4<Element>& arg1,
                                     const bpfVector4<Element>& arg2);
/**
  * Multiplication with a scalar. The vector is the first argument. Returns a
  * vector with component mElement[i]*mult.
  *
  * @param v The vector argument.
  * @param mult The scalar multiplier.
  * @return The multiplicated vector mult * v.
  */
template <class Element>
inline bpfVector4<Element> operator*(const bpfVector4<Element>& v,
                                     Element mult);
/**
  * Multiplication with a scalar. The vector is the second argument.Returns a
  * vector with component mElement[i]*mult.
  *
  * @param mult The scalar multiplier.
  * @param v The vector argument.
  * @return The multiplicated vector mult * v.
  */
template <class Element>
inline bpfVector4<Element> operator*(Element mult,const bpfVector4<Element>& v);
/**
  * Division by a scalar. Returns the vector with component mElement[i] / div.
  *
  * @param v The vector argument.
  * @param div The divisor.
  * @return The vector v divided by div.
  */
template <class Element>
inline bpfVector4<Element> operator/(const bpfVector4<Element>&,Element);

/**
 * Returns a vector that is an element wise conversion from
 * aSourceVector.
 *
 * @param aSourceVector The vector to be converted
 * @return bpfVector4<TargetType> The result of the conversion
 */
template <class SourceType, class TargetType>
bpfVector4<TargetType> ConvertVector(const bpfVector4<SourceType>& aSourceVector)
{
  bpfVector4<TargetType> vTargetVector;
  for (bpfSize vIndex = 0; vIndex < 4; vIndex++) {
    vTargetVector[vIndex] = static_cast<TargetType>(aSourceVector[vIndex]);
  }
  return vTargetVector;
}

///////////////////////////////////////////////////////////////////////////
//
// inline implementation


#include <cmath>

/*
 * Constructors.
 */
template <class Element>
inline
bpfVector4<Element>::bpfVector4()
{
  mElement[0] = (Element) 0;
  mElement[1] = (Element) 0;
  mElement[2] = (Element) 0;
  mElement[3] = (Element) 0;
}


template <class Element>
inline
bpfVector4<Element>::bpfVector4(Element a0, Element a1, Element a2, Element a3)
{
  mElement[0] = a0;
  mElement[1] = a1;
  mElement[2] = a2;
  mElement[3] = a3;
}

/*
 * Write vector to stream in the form (mElement[0],mElement[1],mElement[2])
 */
template <class Element>
inline std::ostream&
operator<<(std::ostream& out, const bpfVector4<Element>& vec)
{
  out << "(" << vec[0] << "," << vec[1] << "," << vec[2] << "," << vec[3] << ")";
  return out;
}

/**
 * Convert a string to a bpfVector4
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& string, bpfVector4<Element>& result)
{
  std::istringstream s_in(string);
  s_in >> result[0] >> result[1] >> result[2] >> result[3];
}

/**
 * Convert a bpfVector4 to a bpfString.
 *
 * @param vec The vector to convert.
 * @return The string (OUT)
 */
template <class Element>
inline bpfString bpfToString(const bpfVector4<Element>& vec)
{
  return bpfToString(vec[0]) + " " + bpfToString(vec[1]) + " "
         + bpfToString(vec[2]) + " " + bpfToString(vec[3]);
}
inline bpfString bpfToString(const bpfVector4<bpfFloat>& vec, bpfInt16 precision)
{
  return bpfToString(vec[0],precision) + " " + bpfToString(vec[1],precision) + " "
         + bpfToString(vec[2],precision) + " " + bpfToString(vec[3],precision) ;
}
inline bpfString bpfToString(const bpfVector4<bpfDouble>& vec, bpfInt16 precision)
{
  return bpfToString(vec[0],precision) + " " + bpfToString(vec[1],precision) + " "
         + bpfToString(vec[2],precision) + " " + bpfToString(vec[3],precision) ;
}

/*
 * Equality test
 */
template <class Element>
inline bool
bpfVector4<Element>::operator==(const bpfVector4<Element>& vec) const
{
  return(mElement[0] == vec[0]) &&
        (mElement[1] == vec[1]) &&
        (mElement[2] == vec[2]) &&
        (mElement[3] == vec[3]);
}

template <class Element>
inline bool
bpfVector4<Element>::operator!=(const bpfVector4<Element>& vec) const
{
  return !operator==(vec);
}

/*
 * Access
 */
template <class Element>
inline Element&
bpfVector4<Element>::operator[](bpfSize i)
{
  return mElement[i];
}

template <class Element>
inline const Element&
bpfVector4<Element>::operator[](bpfSize i) const
{
  return mElement[i];
}
/*
 * Vector operations
 */
template <class Element>
inline void
bpfVector4<Element>::Negate()
{
  mElement[0] = -mElement[0];
  mElement[1] = -mElement[1];
  mElement[2] = -mElement[2];
  mElement[3] = -mElement[3];
}

/* Arithmetic operations}
 *
 * Unary operations}
 */
template <class Element>
inline bpfVector4<Element>&
bpfVector4<Element>::operator+=(const bpfVector4<Element>& vec)
{
  mElement[0] += vec[0];
  mElement[1] += vec[1];
  mElement[2] += vec[2];
  mElement[3] += vec[3];
  return *this;
}

template <class Element>
inline bpfVector4<Element>&
bpfVector4<Element>::operator-=(const bpfVector4<Element>& vec)
{
  mElement[0] -= vec[0];
  mElement[1] -= vec[1];
  mElement[2] -= vec[2];
  mElement[3] -= vec[3];
  return *this;
}

template <class Element>
inline bpfVector4<Element>&
bpfVector4<Element>::operator*=(Element value)
{
  mElement[0] *= value;
  mElement[1] *= value;
  mElement[2] *= value;
  mElement[3] *= value;
  return *this;
}

template <class Element>
inline bpfVector4<Element>&
bpfVector4<Element>::operator/=(Element value)
{
  mElement[0] /= value;
  mElement[1] /= value;
  mElement[2] /= value;
  mElement[3] /= value;
  return *this;
}


/*
 * Global operations
 *
 *
 * Unary operations}
 */
template <class Element>
inline bpfVector4<Element>
operator-(const bpfVector4<Element>& vec)
{
  return bpfVector4<Element>(-vec[0],-vec[1],-vec[2],-vec[3]);
}


/*
 * Binary operations}
 */
template <class Element>
inline bpfVector4<Element>
operator+(const bpfVector4<Element>& s1,const bpfVector4<Element>& s2)
{
  return bpfVector4<Element>(s1[0] + s2[0],
                             s1[1] + s2[1],
                             s1[2] + s2[2],
                             s1[3] + s2[3]);
}

template <class Element>
inline bpfVector4<Element>
operator-(const bpfVector4<Element>& min,const bpfVector4<Element>& sub)
{
  return bpfVector4<Element>(min[0] - sub[0],
                             min[1] - sub[1],
                             min[2] - sub[2],
                             min[3] - sub[3]);
}

template <class Element>
inline bpfVector4<Element>
operator*(const bpfVector4<Element>& f1,Element f2)
{
  return bpfVector4<Element>(f1[0] * f2,
                             f1[1] * f2,
                             f1[2] * f2,
                             f1[3] * f2);
}


template <class Element>
inline bpfVector4<Element>
operator*(Element f1,const bpfVector4<Element>& f2)
{
  return bpfVector4<Element>(f1 * f2[0],
                             f1 * f2[1],
                             f1 * f2[2],
                             f1 * f2[3]);
}


template <class Element>
inline bpfVector4<Element>
operator/(const bpfVector4<Element>& dividend,Element divisor)
{

  return bpfVector4<Element>(dividend[0] / divisor,
                             dividend[1] / divisor,
                             dividend[2] / divisor,
                             dividend[3] / divisor);
}


/*
 * Vector Operations}
 */
template <class Element>
inline Element
bpfVector4<Element>::Length() const
{
  return static_cast<Element>(sqrt(mElement[0]*mElement[0] + mElement[1]*mElement[1] +
                                   mElement[2]*mElement[2] + mElement[3]*mElement[3]));
}

template <class Element>
inline void
bpfVector4<Element>::Normalize()
{
  Element length = Length();
  if (length != 0) {
    operator/=(length);
  }
}



#endif
