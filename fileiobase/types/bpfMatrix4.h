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


#ifndef __BP_MATRIX4__
#define __BP_MATRIX4__

#include <iostream>
#include "fileiobase/types/bpfVector3.h"
#include "fileiobase/types/bpfMatrix3.h"

/**
 *
 * Generic class for 4x4 matrices. (This is just a minimal set of operations)
 *
 * A matrix is defined by (a00 a01 a02 a03)
 *                        (a10 a11 a12 a13)
 *                        (a20 a21 a22 a23)
 *                        (a30 a31 a32 a33)
 * Element a(i,j) is in vRow i and column j  (normal convention).
 *
 * Such a matrix is generally used for 3D homogenous transforms.
 *
 *  @author Thomas Koller
 *
 *
 *
 */


template <class Element>
class bpfMatrix4
{
public:

  /**
   * Default Constructor. The matrix is set to the identity matrix
   */
  bpfMatrix4();

  /**
   * Constructor to set the vector from the arguments.
   *
   * @param elements An array of 16 elements, the matrix element (i,j) is taken
   * from the position 3*i+j - write like a text into matrix
   */
  bpfMatrix4(Element elements[16]);

  /**
   * Constructor where the rows of the upper left 3x3 matrix are specified
   * from the vectors. The fourth vRow and column are set to zero, except a33 is
   * set to 1.
   *
   * @param row0 The vRow 0.
   * @param row1 The vRow 1.
   * @param row2 The vRow 2.
   */
  bpfMatrix4(const bpfVector3<Element>& row0,
             const bpfVector3<Element>& row1,
             const bpfVector3<Element>& row2);

  /*
   * The default copy constructor will be used.
   *
   * No destructor is needed.
   *
   * The default assignment operator (memberwise assignment) will be used.
   *
   */

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
  bool operator==(const bpfMatrix4<Element>&) const;

  /**
   * Non-equality operator.
   */
  bool operator!=(const bpfMatrix4<Element>&) const;

  /*
   * Arithmetic operations
   *
   */
  /**
   * Scalar multiplication.
   */
  bpfMatrix4<Element>& operator*=(Element);

  /*
   * Matrix Operations
   */

  /**
   * Matrix multiplication: A = A * B
   */
  bpfMatrix4<Element>
  operator*=(const bpfMatrix4<Element>& other);

  /**
   * Matrix multiplication. C = A * B, where A is the matrix for which the
   * method is called, or the first argument when called as a binary operator.
   *
   * @param other Second matrix B.
   * @return (*this) * other, repectively A*B
   */
  bpfMatrix4<Element>
  operator*(const bpfMatrix4<Element>& other) const;

  /**
   * Transpose the matrix. The elements (i,j) and (j,i) are swapped.
   */
  void Transpose();

  /**
   * Apply matrix transformation to a point.
   */
  bpfVector3<Element> Transform(const bpfVector3<Element>& aPoint, Element aTranslationFactor = 1) const;

  bpfMatrix3<Element> GetRotation() const;
  bpfVector3<Element> GetTranslation() const;

private:

  Element m[4][4];

};

typedef bpfMatrix4<bpfFloat> bpfMatrix4Float;


/**
 * Print matrix in 2-dimensional correct shape on standard output.
 */
template <class Element>
void bpfPrintMatrix(bpfMatrix4<Element> in);

/**
 * Default Constructor. The matrix is set to the identity matrix
 */
template <class Element>
inline bpfMatrix4<Element>::bpfMatrix4()
{
  m[0][0] = m[1][1] = m[2][2] = m[3][3]= 1;
  m[0][1] = m[0][2] = m[0][3] = 0;
  m[1][0] = m[1][2] = m[1][3] = 0;
  m[2][0] = m[2][1] = m[2][3] = 0;
  m[3][0] = m[3][1] = m[3][2] = 0;


}

/**
 * Constructor to set the vector from the arguments.
 *
 * @param elements An array of 9 elements, the matrix element (i,j) is taken
 * from the position 3*i+j - write like a text into matrix
 */
template <class Element>
inline
bpfMatrix4<Element>::bpfMatrix4(Element elements[16])
{
  for (bpfSize i = 0; i < 4; i++) {
    for (bpfSize j = 0; j < 4; j++) {
      m[i][j] = elements[4*i+j];
    }
  }
}

/**
 * Constructor where the rows of the upper left 3x3 matrix are specified
 * from the vectors. The fourth vRow and column are set to zero, except a33 is
 * set to 1.
 *
 * @param row0 The vRow 0.
 * @param row1 The vRow 1.
 * @param row2 The vRow 2.
 */
template <class Element>
inline
bpfMatrix4<Element>::bpfMatrix4(const bpfVector3<Element>& row0,
                                const bpfVector3<Element>& row1,
                                const bpfVector3<Element>& row2)
{
  for (bpfSize i = 0; i < 3; i++) {          // for all columns
    m[0][i] = row0[i];
    m[1][i] = row1[i];
    m[2][i] = row2[i];
    m[3][i] = m[i][3] = 0;
  }
  m[3][3] = 1;
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
bpfMatrix4<Element>::operator()(bpfSize i, bpfSize j)
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
bpfMatrix4<Element>::operator()(bpfSize i, bpfSize j) const
{
  return m[i][j];
}

/**
 * Equality operator.
 */
template <class Element>
inline bool
bpfMatrix4<Element>::operator==(const bpfMatrix4<Element>& other) const
{
  for (bpfSize i = 0; i < 4; i++) {
    for (bpfSize j = 0; j < 4; j++) {
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
bpfMatrix4<Element>::operator!=(const bpfMatrix4<Element>& other) const
{
  return !(*this == other);
}

/*
 * Scalar multiplication: A = A * b
 */
template <class Element>
inline bpfMatrix4<Element>&
bpfMatrix4<Element>::operator*=(Element scalar)
{
  // multiply matrix elements with scalar
  for (bpfSize vRow = 0; vRow < 4; vRow++) {
    for (bpfSize vCol = 0; vCol < 4; vCol++) {
      m[vRow][vCol] *= scalar;
    }
  }
  return *this;
}


/*
 * Matrix  multiplication: A = A * B
 */
template <class Element>
inline bpfMatrix4<Element>
bpfMatrix4<Element>::operator*=(const bpfMatrix4<Element>& other)
{
  bpfMatrix4<Element> old;

  // copy old matrix into working space old as it will be overwritten
  for (bpfSize vRow = 0; vRow < 4; vRow++) {
    for (bpfSize vCol = 0; vCol < 4; vCol++) {
      old(vRow,vCol) = m[vRow][vCol];
      m[vRow][vCol] = 0;
    }
  }
  // multiply left side with other matrix
  for (bpfSize vRow = 0; vRow < 4; vRow++) {
    for (bpfSize vCol = 0; vCol < 4; vCol++) {
      for (bpfSize i = 0; i < 4; i++) {
        m[vRow][vCol] += old(vRow,i) * other.m[i][vCol];
      }
    }
  }
  return *this;
}

/**
 * Transpose the matrix. The elements (i,j) and (j,i) are swapped.
 */
template <class Element>
void
bpfMatrix4<Element>::Transpose()
{
  for (bpfSize i = 0; i < 4; i++) {
    for (bpfSize j = i+1; j < 4; j++) {
      std::swap(m[i][j], m[j][i]);
    }
  }
}

template <class Element>
inline bpfMatrix4<Element>
bpfMatrix4<Element>::operator*(const bpfMatrix4<Element>& other) const
{
  bpfMatrix4<Element> result;

  // multiply left side with other matrix
  for (bpfSize vRow = 0; vRow < 4; vRow++) {
    for (bpfSize vCol = 0; vCol < 4; vCol++) {
      result.m[vRow][vCol] = 0;
      for (bpfSize i = 0; i < 4; i++) {
        result.m[vRow][vCol] += m[vRow][i] * other.m[i][vCol];
      }
    }
  }
  return result;
}


template <class Element>
inline bpfVector3<Element> bpfMatrix4<Element>::Transform(const bpfVector3<Element>& aPoint, Element aTranslationFactor) const
{
  bpfVector3<Element> vResult;
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    vResult[vRow] = aTranslationFactor * m[vRow][3];
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      vResult[vRow] += m[vRow][vCol] * aPoint[vCol];
    }
  }
  return vResult;
}


template <class Element>
inline bpfMatrix3<Element> bpfMatrix4<Element>::GetRotation() const
{
  return bpfMatrix3<Element>(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
}


template <class Element>
inline bpfVector3<Element> bpfMatrix4<Element>::GetTranslation() const
{
  return bpfVector3<Element>(m[0][3], m[1][3], m[2][3]);
}


/**
 * Print matrix in 2-dimensional correct shape on standard output.
 */
template <class Element>
void bpfPrintMatrix(bpfMatrix4<Element> in)
{
  std::cout << in(0,0) << "  " << in(0,1) << "  " << in(0,2) << "  " << in(0,3) << std::endl;
  std::cout << in(1,0) << "  " << in(1,1) << "  " << in(1,2) << "  " << in(1,3) << std::endl;
  std::cout << in(2,0) << "  " << in(2,1) << "  " << in(2,2) << "  " << in(2,3) << std::endl;
  std::cout << in(3,0) << "  " << in(3,1) << "  " << in(3,2) << "  " << in(3,3) << std::endl;
}


template <class Element>
bpfMatrix4<Element> bpfCreateTranslationMatrix4(const bpfVector3<Element>& aTranslation)
{
  bpfMatrix4<Element> vMatrix;
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    vMatrix(vRow, 3) += aTranslation[vRow];
  }
  return vMatrix;
}


template <class Element>
bpfMatrix4<Element> bpfCreateRotationMatrix4(const bpfMatrix3<Element>& aRotation)
{
  bpfMatrix4<Element> vMatrix;
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    for (bpfSize vCol = 0; vCol < 3; vCol++) {
      vMatrix(vRow, vCol) = aRotation(vRow, vCol);
    }
  }
  return vMatrix;
}


template <class Element>
bpfMatrix4<Element> bpfCreateScaleMatrix4(const Element& aScaleFactor)
{
  bpfMatrix4<Element> vMatrix;
  for (bpfSize vRow = 0; vRow < 3; vRow++) {
    vMatrix(vRow, vRow) = aScaleFactor;
  }
  return vMatrix;
}


template <class Element>
void bpfApplyHomogeneousTranslation(bpfMatrix4<Element>& aMatrix, const bpfVector3<Element>& aTranslation)
{
  bpfMatrix4<Element> vMatrix = bpfCreateTranslationMatrix4(aTranslation);
  aMatrix *= vMatrix;
}


template <class Element>
void bpfApplyHomogeneousRotation(bpfMatrix4<Element>& aMatrix, const bpfMatrix3<Element>& aRotation)
{
  bpfMatrix4<Element> vMatrix = bpfCreateRotationMatrix4(aRotation);
  aMatrix *= vMatrix;
}


template <class Element>
void bpfApplyHomogeneousScale(bpfMatrix4<Element>& aMatrix, const Element& aScaleFactor)
{
  bpfMatrix4<Element> vMatrix = bpfCreateScaleMatrix4(aScaleFactor);
  aMatrix *= vMatrix;
}


#endif
