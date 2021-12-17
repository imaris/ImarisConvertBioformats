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


#ifndef __bpfMatrix3__
#define __bpfMatrix3__

#include "fileiobase/types/bpfVector3.h"
#include "fileiobase/types/bpfVector4.h"
#include "fileiobase/exceptions/bpfIllegalRangeException.h"
#include <iostream>

/**
 *
 * Generic class for 3x3 matrices. (This is just a minimal set of operations)
 *
 * A matrix is defined by (a00 a01 a02)
 *                        (a10 a11 a12)
 *                        (a20 a21 a22)
 * Element a(i,j) is in vRow i and column j  (normal convention)
 *
 *  @author Thomas Koller
 *
 *
 *
 *
 */


template <class Element>
class bpfMatrix3
{
public:

  /**
   * Default Constructor. The matrix is set to the identity matrix
   */
  bpfMatrix3();

  /**
   * Constructor all elements are initialized with the same value.
   */
  bpfMatrix3(Element aValue);

  /**
   * Constructor to set the vector from the arguments.
   *
   * @param elements An array of 9 elements, the matrix element (i,j) is taken
   * from the position 3*i+j - write like a text into matrix
   */
  bpfMatrix3(Element elements[9]);

  /**
   * Constructor to set the vector from the arguments.
   */
  bpfMatrix3(Element aA00, Element aA01, Element aA02,
             Element aA10, Element aA11, Element aA12,
             Element aA20, Element aA21, Element aA22);

  /**
   * Constructor where the rows are specified from the vectors. This is
   * occasionally useful when constructing an orthogonal transformation matrix
   * and the images of the coordinate vectors are known.
   *
   * @param row0 The vRow 0.
   * @param row1 The vRow 1.
   * @param row2 The vRow 2.
   */
  bpfMatrix3(const bpfVector3<Element>& row0,
             const bpfVector3<Element>& row1,
             const bpfVector3<Element>& row2);

  /**
   * The default copy constructor will be used.
   *
   * No destructor is needed.
   *
   * The default assignment operator (memberwise assignment) will be used.
   *
   */

  /**
   * String conversion
   */
  bpfString ToString() const;

  /**
   * Access.
   *
   * @param i The vRow.
   * @param j The column.
   * @return the Element in vRow i and column j.
   */
  Element& operator()(bpfSize i, bpfSize j);

  /**
   * Access for const objects.
   * @param i The vRow.
   * @param j The column.
   * @return the Element in vRow i and column j.
   */
  const Element& operator()(bpfSize i, bpfSize j) const;

  /**
   * Equality operator.
   */
  bool operator==(const bpfMatrix3<Element>&) const;

  /**
   * Non-equality operator.
   */
  bool operator!=(const bpfMatrix3<Element>&) const;

  /*
   * Arithmetic operations
   *
   */
  /**
   * @name Unary operations.
   */
  //@{
  // Scalar multiplication.
  bpfMatrix3<Element>& operator*=(Element);

  // Scalar division
  bpfMatrix3<Element>& operator/=(Element);

  /*
   * Matrix Operations
   */

  /**
   * Matrix multiplication: A = A * B
   */
  bpfMatrix3<Element>
  operator*=(const bpfMatrix3<Element>& other);

  /**
   * Matrix multiplication. C = A * B, where A is the matrix for which the
   * method is called, or the first argument when called as a binary operator.
   *
   * @param other Second matrix B.
   * @return (*this) * other, repectively A*B
   */
  bpfMatrix3<Element>
  operator*(const bpfMatrix3<Element>& other) const;

  /**
   * Matrix addition: A = A + B
   */
  bpfMatrix3<Element>
  operator+=(const bpfMatrix3<Element>& aOther);

  /**
   * Matrix addition: C = A + B
   */
  bpfMatrix3<Element>
  operator+(const bpfMatrix3<Element>& aOther) const;

  /**
   * Matrix subtraction: A = A - B
   */
  bpfMatrix3<Element>
  operator-=(const bpfMatrix3<Element>& aOther);

  /**
   * Matrix subtraction: C = A - B
   */
  bpfMatrix3<Element>
  operator-(const bpfMatrix3<Element>& aOther) const;


private:
  Element m[3][3];

};

typedef bpfMatrix3<bpfFloat> bpfMatrix3Float;


/**
 * Matrix Inversion.
 *
 * @param in
 * @return inverted matrix
 */

template <class Element>
inline bpfMatrix3<Element> bpfInvertMatrix(const bpfMatrix3<Element>& in);


/**
 * Multiplication of 3x3-Matrix with column vector.
 *
 * @param matrix   3x3-matrix
 * @param vector   column vector
 * @return column vector
 */

template <class Element>
inline bpfVector3<Element> bpfMultiplyMatrixWithVector(const bpfMatrix3<Element>& matrix,
                                                       const bpfVector3<Element>& vector);

/**
 * Print matrix in 2-dimensional correct shape on standard output.
 */
template <class Element>
void bpfPrintMatrix(const bpfMatrix3<Element>& in);


/**
 * Conversion from a Matrix with double to float
 */
bpfMatrix3<bpfFloat> bpfFloatMatrix(const bpfMatrix3<bpfDouble> matrix);

/**
 * Conversion from a Matrix with float to double
 */
bpfMatrix3<bpfDouble> bpfDoubleMatrix(const bpfMatrix3<bpfFloat> matrix);


//inline definitions

/**
 * Default Constructor. The matrix is set to the identity matrix
 */
template <class Element>
inline bpfMatrix3<Element>::bpfMatrix3()
{
  m[0][0] = m[1][1] = m[2][2] = 1;
  m[0][1] = m[0][2] = 0;
  m[1][0] = m[1][2] = 0;
  m[2][0] = m[2][1] = 0;
}

/**
 * Constructor all elements are initialized with the same value.
 */
template <class Element>
inline bpfMatrix3<Element>::bpfMatrix3(Element aValue)
{
  m[0][0] = m[0][1] = m[0][2] = aValue;
  m[1][0] = m[1][1] = m[1][2] = aValue;
  m[2][0] = m[2][1] = m[2][2] = aValue;
}

/**
 * Constructor to set the vector from the arguments.
 *
 * @param elements An array of 9 elements, the matrix element (i,j) is taken
 * from the position 3*i+j - write like a text into matrix
 */
template <class Element>
inline
bpfMatrix3<Element>::bpfMatrix3(Element elements[9])
{
  for (bpfSize i = 0; i < 3; i++) {
    for (bpfSize j = 0; j < 3; j++) {
      m[i][j] = elements[3*i+j];
    }
  }
}

/**
 * Constructor to set the vector from the arguments.
 */
template <class Element>
inline
bpfMatrix3<Element>::bpfMatrix3(Element aA00, Element aA01, Element aA02,
                                Element aA10, Element aA11, Element aA12,
                                Element aA20, Element aA21, Element aA22)
{
  m[0][0] = aA00;
  m[0][1] = aA01;
  m[0][2] = aA02;
  m[1][0] = aA10;
  m[1][1] = aA11;
  m[1][2] = aA12;
  m[2][0] = aA20;
  m[2][1] = aA21;
  m[2][2] = aA22;
}

/**
 * Constructor where the rows are specified from the vectors. This is
 * occasionally useful when constructing an orthogonal transformation matrix
 * and the images of the coordinate vectors are known.
 *
 * @param row0 The vRow 0.
 * @param row1 The vRow 1.
 * @param row2 The vRow 2.
 */

template <class Element>
inline
bpfMatrix3<Element>::bpfMatrix3(const bpfVector3<Element>& row0,
                                const bpfVector3<Element>& row1,
                                const bpfVector3<Element>& row2)
{
  for (bpfSize i = 0; i < 3; i++) {         // for all rows
    m[0][i] = row0[i];
    m[1][i] = row1[i];
    m[2][i] = row2[i];
  }
}

/**
 * String conversion
 * A matrix is defined by (a00 a01 a02)
 *                        (a10 a11 a12)
 *                        (a20 a21 a22)
 */
template <class Element>
inline bpfString bpfMatrix3<Element>::ToString() const
{
  return "(" + bpfToString(m[0][0]) + " " + bpfToString(m[0][1]) + " " + bpfToString(m[0][2]) + ")\n" +
         "(" + bpfToString(m[1][0]) + " " + bpfToString(m[1][1]) + " " + bpfToString(m[1][2]) + ")\n" +
         "(" + bpfToString(m[2][0]) + " " + bpfToString(m[2][1]) + " " + bpfToString(m[2][2]) + ")\n";
}


template <class Element>
inline bpfString bpfToString(const bpfMatrix3<Element>& aMatrix)
{
  return aMatrix.ToString();
}


template <class Element>
inline void bpfFromString(const bpfString& aString, bpfMatrix3<Element>& aResult)
{
  std::istringstream vIn(aString);
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      vIn >> aResult(vRow, vCol);
    }
  }
}


template <class Element>
inline void bpfFromString(const bpfString& aString, std::vector<bpfMatrix3<Element> >& aResult)
{
  aResult.clear();
  std::istringstream vIn(aString);
  bpfString vLine;
  bpfSize vRow = 3;
  while (std::getline(vIn, vLine)) {
    if (vRow == 3) {
      aResult.resize(aResult.size() + 1);
      vRow = 0;
    }
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      vIn >> aResult.back()(vRow, vCol);
    }
    vRow++;
  }
}

/**
 * Access.
 *
 * @param i The vRow.
 * @param j The column.
 * @return the Element in vRow i and column j.
 */
template <class Element>
inline Element&
bpfMatrix3<Element>::operator()(bpfSize i, bpfSize j)
{
  return m[i][j];
}

/**
 * Access for const objects.
 *
 * @param i The vRow.
 * @param j The column.
 * @return the Element in vRow i and column j.
 */
template <class Element>
inline const Element&
bpfMatrix3<Element>::operator()(bpfSize i, bpfSize j) const
{
  return m[i][j];
}

/**
 * Equality operator.
 */
template <class Element>
inline bool
bpfMatrix3<Element>::operator==(const bpfMatrix3<Element>& other) const
{
  for (bpfSize i = 0; i < 3; i++) {
    for (bpfSize j = 0; j < 3; j++) {
      if (m[i][j] != other.m[i][j]) {
        return false;
      }
    }
  }
  return true;
}

/**
 * Non-equality operator.
 */
template <class Element>
inline bool
bpfMatrix3<Element>::operator!=(const bpfMatrix3<Element>& other) const
{
  return !(*this == other);
}

/*
 * Scalar multiplication: A = A * b
 */
template <class Element>
inline bpfMatrix3<Element>&
bpfMatrix3<Element>::operator*=(Element scalar)
{
  // multiply matrix elements with scalar
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      m[vRow][vCol] *= scalar;
    }
  }
  return *this;
}

/*
 * Scalar division: A = A / b
 */
template <class Element>
inline bpfMatrix3<Element>&
bpfMatrix3<Element>::operator/=(Element scalar)
{
  scalar = 1 / scalar;
  return operator*=(scalar);
}

/*
 * Matrix  multiplication: A = A * B
 */
template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator*=(const bpfMatrix3<Element>& other)
{
  bpfMatrix3<Element> old;

  // copy old matrix into working space old as it will be overwritten
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      old(vRow,vCol) = m[vRow][vCol];
      m[vRow][vCol] = 0;
    }
  }
  // multiply left side with other matrix
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      for (bpfSize  i = 0; i < 3; i++) {
        m[vRow][vCol] += old(vRow,i) * other.m[i][vCol];
      }
    }
  }
  return *this;
}

template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator*(const bpfMatrix3<Element>& other) const
{
  bpfMatrix3<Element> result;
  // multiply left side with other matrix
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      result.m[vRow][vCol] = 0; /// \todo could put this directly into initialization of result
      for (bpfSize  i = 0; i < 3; i++) {
        result.m[vRow][vCol] += m[vRow][i] * other.m[i][vCol];
      }
    }
  }
  return result;
}

/**
 * Matrix addition: A = A + B
 */
template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator+=(const bpfMatrix3<Element>& aOther)
{
  // add matrix elements
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      m[vRow][vCol] += aOther.m[vRow][vCol];
    }
  }
  return *this;
}

/**
 * Matrix addition: C = A + B
 */
template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator+(const bpfMatrix3<Element>& aOther) const
{
  bpfMatrix3<Element> vResult;
  // add matrix elements
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      vResult.m[vRow][vCol] = m[vRow][vCol] + aOther.m[vRow][vCol];
    }
  }
  return vResult;
}

/**
 * Matrix subtraction: A = A - B
 */
template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator-=(const bpfMatrix3<Element>& aOther)
{
  for (bpfSize  vRow = 0; vRow < 3; vRow++) {
    for (bpfSize  vCol = 0; vCol < 3; vCol++) {
      m[vRow][vCol] -= aOther.m[vRow][vCol];
    }
  }
  return *this;
}

/**
 * Matrix subtraction: C = A - B
 */
template <class Element>
inline bpfMatrix3<Element>
bpfMatrix3<Element>::operator-(const bpfMatrix3<Element>& aOther) const
{
  bpfMatrix3<Element> aResult;
  // add matrix elements
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      aResult.m[vRow][vCol] = m[vRow][vCol] - aOther.m[vRow][vCol];
    }
  }
  return aResult;
}

/**
 * Matrix Inversion.
 *
 * @param inMatrix
 * @return inverted matrix
 */

template <class Element>
inline bpfMatrix3<Element> bpfInvertMatrix(const bpfMatrix3<Element>& in)
{
  Element a,b,c,d,e,f,g,h,i;

  a = in(0,0);
  b = in(0,1);
  c = in(0,2);
  d = in(1,0);
  e = in(1,1);
  f = in(1,2);
  g = in(2,0);
  h = in(2,1);
  i = in(2,2);

  // inverted matrix
  bpfMatrix3<Element> inv;
  // determinante
  bpfDouble det = a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h;

  if (det == 0) {
    throw bpfIllegalRangeException("bpfInvertMatrix: Determinante is zero!");
  }
  inv(0,0) = (Element)((e*i - f*h) / det);
  inv(0,1) = (Element)((c*h - b*i) / det);
  inv(0,2) = (Element)((b*f - c*e) / det);
  inv(1,0) = (Element)((f*g - d*i) / det);
  inv(1,1) = (Element)((a*i - c*g) / det);
  inv(1,2) = (Element)((c*d - a*f) / det);
  inv(2,0) = (Element)((d*h - e*g) / det);
  inv(2,1) = (Element)((b*g - a*h) / det);
  inv(2,2) = (Element)((a*e - b*d) / det);

  return inv;
}

/**
 * Multiplication of 3x3-Matrix with column vector.
 *
 * @param matrix   3x3-matrix
 * @param vector   column vector
 * @return column vector
 */

template <class Element>
inline bpfVector3<Element> bpfMultiplyMatrixWithVector(const bpfMatrix3<Element>& matrix,
                                                       const bpfVector3<Element>& vector)
{
  bpfVector3<Element> out;

  for (bpfSize i = 0; i < 3; i ++) {
    out[i] = 0;
    for (bpfSize j = 0; j < 3; j ++) {
      out[i] += matrix(i,j) * vector[j];
    }
  }
  return out;
}


/**
 * Alias for the bpfVector3 version.
 *
 * @param aAxisAngle Elements 0-2 specify axis xyz, Element 3
 *                   specifies angle.
 * @return bpfMatrix3<Element>
 */
template <class Element>
inline bpfMatrix3<Element> bpfRotationMatrixFromAxisAngle(const bpfVector4<Element>& aAxisAngle)
{
  bpfVector3<Element> vAxis = bpfVector3<Element>(aAxisAngle[0], aAxisAngle[1], aAxisAngle[2]);
  return bpRotationMatrixFromAxisAngle(vAxis, aAxisAngle[3]);
}

template <class Element>
inline bpfMatrix3<Element> bpfRotationMatrixFromAxisAngle(const bpfVector3<Element>& aAxis,
                                                          Element aAngle)
{
  if (aAngle == 0) {
    return bpfMatrix3<Element>();
  }
  bpfVector3<Element> vAxis(aAxis);
  vAxis.Normalize();
  bpfVector3<Element> v123 = vAxis * static_cast<Element>(-sin(aAngle/2));
  Element v0 = cos(aAngle/2);
  Element v1 = v123[0];
  Element v2 = v123[1];
  Element v3 = v123[2];

  bpfMatrix3<Element> vResult(
    v0*v0 + v1*v1 - v2*v2 - v3*v3, 2*(v1*v2 + v0*v3), 2*(v1*v3 - v0*v2),
    2*(v1*v2 - v0*v3), v0*v0 - v1*v1 + v2*v2 - v3*v3, 2*(v2*v3 + v0*v1),
    2*(v1*v3 + v0*v2), 2*(v2*v3 - v0*v1), v0*v0 - v1*v1 - v2*v2 + v3*v3);
  return vResult;
}



/**
 * Returns the axis, angle representation of the rotation
 * defined by the matrix.
 *
 *
 * This algorithm is specified in
 * http://en.wikipedia.org/wiki/Rotation_representation_%28mathematics%29#Rotation_matrix_.E2.86.94_Euler_axis.2Fangle
 *
 * \todo sg the page states that it does not work for rotation
 *       angles that are multiples of PI, we should handle this
 *       case
 *
 * \todo sg this method currently can not handle the identity
 *    matrix, and probably all permutations of it
 *
 * \todo sg we could add a test if aRotationMatrix really is a
 *       rotation matrix and display a warning, this could help
 *       finding errors
 *
 * @param aRotationMatrix Matrix representing a rotation in 3
 *                        dimensions
 * @return bpfVector4<bpfDouble> axis x, y, z, angle, where the
 *         axis is normalized.
 */
template <class Element>
inline bpfVector4<Element> bpfAxisAngleFromRotationMatrix(const bpfMatrix3<Element>& aRotationMatrix)
{
  Element vA0 = aRotationMatrix(2,1) - aRotationMatrix(1,2);
  Element vA1 = aRotationMatrix(0,2) - aRotationMatrix(2,0);
  Element vA2 = aRotationMatrix(1,0) - aRotationMatrix(0,1);
  bpfDouble vCos = (aRotationMatrix(0,0) + aRotationMatrix(1,1) + aRotationMatrix(2,2) - 1) * 0.5;
  if (vCos < -1 || vCos > 1) {
    return bpfVector4<Element>(0, 0, 1, 0);
  }
  Element vAngle = static_cast<Element>(acos(vCos));
  if (vAngle == 0) {
    return bpfVector4<Element>(0, 0, 1, 0);
  }
  bpfVector3<Element> vAxis(vA0, vA1, vA2);
  vAxis.Normalize();
  bpfVector4<Element> vAxisAngle(vAxis[0], vAxis[1], vAxis[2], vAngle);
  return vAxisAngle;
}


template <class Element>
inline bpfMatrix3<Element> bpfMatrix3Transpose(const bpfMatrix3<Element>& aMatrix)
{
  bpfMatrix3<Element> vResult;
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      vResult(vRow, vCol) = aMatrix(vCol, vRow);
    }
  }
  return vResult;
}


/**
 * Print matrix in 2-dimensional correct shape on standard output.
 */
template <class Element>
void bpfPrintMatrix(const bpfMatrix3<Element>& in)
{
  std::cout << in(0,0) << "  " << in(0,1) << "  " << in(0,2) << std::endl;
  std::cout << in(1,0) << "  " << in(1,1) << "  " << in(1,2) << std::endl;
  std::cout << in(2,0) << "  " << in(2,1) << "  " << in(2,2) << std::endl;
}


#endif
