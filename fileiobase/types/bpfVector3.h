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


#ifndef __BP_VECTOR3__
#define __BP_VECTOR3__

#include "fileiobase/types/bpfTypes.h"
#include "fileiobase/utils/bpfUtils.h"

#include <iostream>
#include <cmath>

/**
 *
 * Generic class for vectors of length 3. Arithmetic and relational
 * functions are provided. The class has no private or virtual
 * functions.  It can be used as the type argument to a bpfImage.
 *
 *  @see bpfVector2
 *  @author Thomas Koller
 *
 */
template <class Element>
class bpfVector3
{
public:

  /**
   * Default Constructor. The vector is set to (0,0,0).
   */
  inline bpfVector3();

  /**
   * Constructor to set the vector from the arguments.
   *
   * @param x0 The component with index 0 (first).
   * @param x1 The component with index 1 (second).
   * @parma x2 The component with index 2 (third).
   */
  inline bpfVector3(Element x0,Element x1,Element x2);

  /**
   * Constructor to set the vector from the arguments.
   *
   * @param aVector pointer to an array of 3 elements.
   */
  inline bpfVector3(const Element* aVector);

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
  inline Element& operator[](bpfSize i);

  /**
   * Access for const objects.
   */
  inline const Element& operator[](bpfSize i) const;

  /**
   * Access to the 3 components
   */
  inline Element& GetX();
  inline const Element& GetX() const;
  inline Element& GetY();
  inline const Element& GetY() const;
  inline Element& GetZ();
  inline const Element& GetZ() const;

  /**
   * Equality operator. Two vectors are equal if all components are equal.
   *
   * @param other Vector to compare with.
   */
  inline bool operator==(const bpfVector3<Element>& other) const;
  /**
   * Non-equality operator.
   *
   * @param other Vector to compare with.
   */
  inline bool operator!=(const bpfVector3<Element>& other) const;

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
  inline bpfVector3<Element>& operator+=(const bpfVector3<Element>& arg);
  /**
   * Subtraction. Calculates *this[i] -= arg[i] for all components.
   *
   * @param arg The argument.
   */
  inline bpfVector3<Element>& operator-=(const bpfVector3<Element>& arg);
  /**
   * Scalar Multiplication. Calculates *this[i] *= mult for all components.
   *
   * @param mult The multiplier.
   */
  inline bpfVector3<Element>& operator*=(Element mult);

  /**
   * Scalar Division. Calculates *this[i] /= mult for all components.
   *
   * @param div The divisor.
   */
  inline bpfVector3<Element>& operator/=(Element div);
  //@}

  /**
   * access to c-array (no range check)
   */
  inline const Element* GetVec3() const;

  /*
   * Vector Operations
   */
  /**
   * Dot (inner) product. Calculates and returns Sum(*this[i] * other[i])
   *
   * @param other The argument.
   * @return The inner product.
   */
  inline Element Dot(const bpfVector3<Element>& other) const;

  /**
   * Cross (vector) product. Calculates the vector product as *this ^
   * other.
   *
   * @param other The argument.
   * @return The vector product.
   */
  inline bpfVector3<Element> Cross(const bpfVector3<Element>& other)const;

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

  /**
   * Calculate the principal component. The principal component is defined as
   * a component i for which v[i]^2 <= v[j]^2, for all j. It may not be unique.
   *
   * @return The principal component.
   */
  inline bpfInt32 GetComponentMax() const;
  inline bpfInt32 GetComponentMin() const;

private:
  Element v[3];

};


using bpfVector3Float = bpfVector3<bpfFloat>;
using bpfVector3Double = bpfVector3<bpfDouble>;
using bpfVector3String = bpfVector3<bpfString>;
using bpfVector3Int8 = bpfVector3<bpfInt8>;
using bpfVector3Int16 = bpfVector3<bpfInt16>;
using bpfVector3Int32 = bpfVector3<bpfInt32>;
using bpfVector3UInt8 = bpfVector3<bpfUInt8>;
using bpfVector3UInt16 = bpfVector3<bpfUInt16>;
using bpfVector3UInt32 = bpfVector3<bpfUInt32>;
using bpfVector3Bool = bpfVector3<bool>;


/**
  * @name Operations for  bpfVector3<Element>.
  *
  * @see bpfVector3
  */
//@{
/**
 * Output to a stream in the format (x1,x2,x3).
 */
template <class Element>
inline std::ostream& operator<<(std::ostream&,const bpfVector3<Element>&);


/**
 * Convert a string to a bpfVector3
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& string, bpfVector3<Element>& result);

/**
 * Convert a string to a std::vector of bpfVector3
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& aString, std::vector<bpfVector3<Element> >& aVector);

/**
 * Convert a bpfVector3 to a bpfString.
 *
 * @param vec The vector to convert.
 * @return The string (OUT)
 */
template <class Element>
inline bpfString bpfToString(const bpfVector3<Element>& vec);

inline bpfString bpfToString(const bpfVector3Float& vec, bpfInt16 precision = 8);
inline bpfString bpfToString(const bpfVector3Double& vec, bpfInt16 precision = 8);

/*
 * Unary and Binary operations
 */

/**
 * Unary minus (returns a new vector).
 *
 * @return The negated vector.
 */
template <class Element>
inline bpfVector3<Element> operator-(const bpfVector3<Element>&);
/**
 * Vector addition. Returns the vector with components summand1[i] +
 * summand2[i].
 *
 * @param summand1 The first summand.
 * @param summand2 The second summand.
 * @return The addition of summand1 and summand2.
 */
template <class Element>
inline bpfVector3<Element> operator+(const bpfVector3<Element>& summand1,
                                     const bpfVector3<Element>& summand2);
/**
 * Vector subtraction. Returns the vector with components arg1[i] -
 * arg2[i].
 *
 * @param arg1 The first argument (from which will be subtracted).
 * @param arg2 The second argument (that will be subtracted);
 * @return The subtraction of arg2 from arg1.
 */
template <class Element>
inline bpfVector3<Element> operator-(const bpfVector3<Element>& arg1,
                                     const bpfVector3<Element>& arg2);
/**
  * Multiplication with a scalar. The vector is the first argument. Returns a
  * vector with component v[i]*mult.
  *
  * @param v The vector argument.
  * @param mult The scalar multiplier.
  * @return The multiplicated vector mult * v.
  */
template <class Element>
inline bpfVector3<Element> operator*(const bpfVector3<Element>& v,
                                     Element mult);
/**
  * Multiplication with a scalar. The vector is the second argument.Returns a
  * vector with component v[i]*mult.
  *
  * @param mult The scalar multiplier.
  * @param v The vector argument.
  * @return The multiplicated vector mult * v.
  */
template <class Element>
inline bpfVector3<Element> operator*(Element mult,const bpfVector3<Element>& v);
/**
  * Division by a scalar. Returns the vector with component v[i] / div.
  *
  * @param v The vector argument.
  * @param div The divisor.
  * @return The vector v divided by div.
  */
template <class Element>
inline bpfVector3<Element> operator/(const bpfVector3<Element>&,Element);


//inline definitions
/*
 * Constructors.
 */
template <class Element>
inline
bpfVector3<Element>::bpfVector3()
{
  v[0] = (Element)0;
  v[1] = (Element)0;
  v[2] = (Element)0;
}


template <class Element>
inline
bpfVector3<Element>::bpfVector3(
  Element x,
  Element y,
  Element z)
{
  v[0] = x;
  v[1] = y;
  v[2] = z;
}

template <class Element>
inline
bpfVector3<Element>::bpfVector3(const Element* aVector)
{
  memcpy(v, aVector, 3 * sizeof(Element));
}


/*
 * Write vector to stream in the form (v[0],v[1],v[2])
 */
template <class Element>
inline std::ostream&
operator<<(std::ostream& out, const bpfVector3<Element>& vec)
{
  out << "(" << vec[0] << "," << vec[1] << "," << vec[2] << ")";
  return out;
}

/**
 * Convert a string to a bpfVector3
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& string, bpfVector3<Element>& result)
{
  std::istringstream s_in(string);
  s_in >> result[0] >> result[1] >> result[2];
}
/**
 * Convert a string to a std::vector of bpfVector3
 *
 * @param string The string from which to get the value
 * @param result The value (OUT)
 */
template <class Element>
inline void bpfFromString(const bpfString& aString, std::vector<bpfVector3<Element> >& aVector)
{
  bpfString vString = bpfTrim(aString);
  Element vValue0 = 0.0f;
  Element vValue1 = 0.0f;
  Element vValue2 = 0.0f;
  bpfSize vPos;
  aVector.clear();

  while (!vString.empty()) {
    vPos = vString.find_first_of(" ");
    bpfFromString(vString.substr(0,vPos), vValue0);
    vString.erase(0,vPos);
    vString = bpfTrim(vString);
    vPos = vString.find_first_of(" ");
    bpfFromString(vString.substr(0,vPos), vValue1);
    vString.erase(0,vPos);
    vString = bpfTrim(vString);
    vPos = vString.find_first_of(" ");
    bpfFromString(vString.substr(0,vPos), vValue2);
    vString.erase(0,vPos);
    vString = bpfTrim(vString);
    aVector.push_back(bpfVector3<Element>(vValue0,vValue1,vValue2));
  }
}

/**
 * Convert a bpfVector3 to a bpfString.
 *
 * @param vec The vector to convert.
 * @return The string (OUT)
 */
template <class Element>
inline bpfString bpfToString(const bpfVector3<Element>& vec)
{
  return bpfToString(vec[0]) + " " + bpfToString(vec[1]) + " "
         + bpfToString(vec[2]);
}
inline bpfString bpfToString(const bpfVector3Float& vec, bpfInt16 precision)
{
  return bpfToString(vec[0],precision) + " " + bpfToString(vec[1],precision) + " "
         + bpfToString(vec[2],precision);
}
inline bpfString bpfToString(const bpfVector3Double& vec, bpfInt16 precision)
{
  return bpfToString(vec[0],precision) + " " + bpfToString(vec[1],precision) + " "
         + bpfToString(vec[2],precision);
}

/*
 * Equality test
 */
template <class Element>
inline bool
bpfVector3<Element>::operator==(const bpfVector3<Element>& vec) const
{
  return(v[0] == vec[0]) && (v[1] == vec[1]) && (v[2] == vec[2]);
}

template <class Element>
inline bool
bpfVector3<Element>::operator!=(const bpfVector3<Element>& vec) const
{
  return !operator==(vec);
}

/*
 * Access
 */
template <class Element>
inline Element&
bpfVector3<Element>::operator[](bpfSize i)
{
  return v[i];
}

template <class Element>
inline const Element&
bpfVector3<Element>::operator[](bpfSize i) const
{
  return v[i];
}

template <class Element>
inline Element& bpfVector3<Element>::GetX()
{
  return v[0];
}

template <class Element>
inline const Element& bpfVector3<Element>::GetX() const
{
  return v[0];
}

template <class Element>
inline Element& bpfVector3<Element>::GetY()
{
  return v[1];
}

template <class Element>
inline const Element& bpfVector3<Element>::GetY() const
{
  return v[1];
}

template <class Element>
inline Element& bpfVector3<Element>::GetZ()
{
  return v[2];
}

template <class Element>
inline const Element& bpfVector3<Element>::GetZ() const
{
  return v[2];
}

/*
 * Vector operations
 */
template <class Element>
inline void
bpfVector3<Element>::Negate()
{
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
}

/* Arithmetic operations}
 *
 * Unary operations}
 */
template <class Element>
inline bpfVector3<Element>&
bpfVector3<Element>::operator+=(const bpfVector3<Element>& vec)
{
  v[0] += vec[0];
  v[1] += vec[1];
  v[2] += vec[2];
  return *this;
}

template <class Element>
inline bpfVector3<Element>&
bpfVector3<Element>::operator-=(const bpfVector3<Element>& vec)
{
  v[0] -= vec[0];
  v[1] -= vec[1];
  v[2] -= vec[2];
  return *this;
}

template <class Element>
inline bpfVector3<Element>&
bpfVector3<Element>::operator*=(Element value)
{
  v[0] *= value;
  v[1] *= value;
  v[2] *= value;
  return *this;
}

template <class Element>
inline bpfVector3<Element>&
bpfVector3<Element>::operator/=(Element value)
{
  v[0] /= value;
  v[1] /= value;
  v[2] /= value;
  return *this;
}

template <class Element>
inline const Element*
bpfVector3<Element>::GetVec3() const
{
  return v;
}

template <class Element>
inline Element
bpfVector3<Element>::Dot(const bpfVector3<Element>& a)const
{
  return v[0]*a[0] + v[1]*a[1] + v[2]*a[2];
}

template <class Element>
inline bpfVector3<Element>
bpfVector3<Element>::Cross(const bpfVector3<Element>& a)const
{

  return bpfVector3<Element>(v[1]*a[2] - v[2]*a[1],
                             v[2]*a[0] - v[0]*a[2],
                             v[0]*a[1] - v[1]*a[0]);
}

/**
 * Calculate the principal component. The principal component is defined as
 * a component i for which v[i]^2 <= v[j]^2, for all j. It may not be
 * unique.
 *
 * @return The principal component.
 */
template <class Element>
inline bpfInt32
bpfVector3<Element>::GetComponentMax() const
{
  Element v0 = v[0] * v[0];
  Element v1 = v[1] * v[1];
  Element v2 = v[2] * v[2];

  if (v0 >= v1) {
    if (v0 >= v2) {
      return 0;                         // v0 >= v1 ^ v0 >= v2
    }
    else {
      return 2;                         // v2 >= v0 >= v1
    }
  }
  else {
    if (v1 >= v2) {
      return 1;                         // v1 >= v0 ^ v1 >= v2
    }
    else {
      return 2;                         // v2 >= v1 >= v0
    }
  }
}

/**
 * Calculate the principal component. The principal component is defined as
 * a component i for which v[i]^2 <= v[j]^2, for all j. It may not be
 * unique.
 *
 * @return The principal component.
 */
template <class Element>
inline bpfInt32
bpfVector3<Element>::GetComponentMin() const
{
  Element v0 = v[0] * v[0];
  Element v1 = v[1] * v[1];
  Element v2 = v[2] * v[2];

  if (v0 < v1) {
    if (v0 < v2) {
      return 0;                         // v0 < v1 ^ v0 < v2
    }
    else {
      return 2;                         // v2 <= v0 < v1
    }
  }
  else {
    if (v1 < v2) {
      return 1;                         // v1 <= v0 ^ v1 < v2
    }
    else {
      return 2;                         // v2 <= v1 <= v0
    }
  }
}

/*
 * Global operations
 *
 *
 * Unary operations}
 */
template <class Element>
inline bpfVector3<Element>
operator-(const bpfVector3<Element>& vec)
{
  return bpfVector3<Element>(-vec[0],-vec[1],-vec[2]);
}


/*
 * Binary operations}
 */
template <class Element>
inline bpfVector3<Element>
operator+(const bpfVector3<Element>& s1,const bpfVector3<Element>& s2)
{
  return bpfVector3<Element>(s1[0] + s2[0],
                             s1[1] + s2[1],
                             s1[2] + s2[2]);
}

template <class Element>
inline bpfVector3<Element>
operator-(const bpfVector3<Element>& min,const bpfVector3<Element>& sub)
{
  return bpfVector3<Element>(min[0] - sub[0],
                             min[1] - sub[1],
                             min[2] - sub[2]);
}

template <class Element>
inline bpfVector3<Element>
operator*(const bpfVector3<Element>& f1,Element f2)
{
  return bpfVector3<Element>(f1[0] * f2,
                             f1[1] * f2,
                             f1[2] * f2);
}


template <class Element>
inline bpfVector3<Element>
operator*(Element f1,const bpfVector3<Element>& f2)
{
  return bpfVector3<Element>(f1 * f2[0],
                             f1 * f2[1],
                             f1 * f2[2]);
}


template <class Element>
inline bpfVector3<Element>
operator/(const bpfVector3<Element>& dividend,Element divisor)
{

  return bpfVector3<Element>(dividend[0] / divisor,
                             dividend[1] / divisor,
                             dividend[2] / divisor);
}


/*
 * Vector Operations}
 */
template <class Element>
inline Element
bpfVector3<Element>::Length() const
{
  return static_cast<Element>(sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]));
}

template <class Element>
inline void
bpfVector3<Element>::Normalize()
{
  Element length = Length();
  if (length != 0) {
    operator/=(length);
  }
}



#endif
