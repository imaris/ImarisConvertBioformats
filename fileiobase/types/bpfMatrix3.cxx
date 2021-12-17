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


#include "fileiobase/types/bpfMatrix3.h"
#include "fileiobase/types/bpfTypes.h"

/**
 * Conversion from a Matrix with double to float
 */
bpfMatrix3<bpfFloat> bpfFloatMatrix(const bpfMatrix3<bpfDouble> matrix)
{
  bpfMatrix3<bpfFloat> floatMatrix;

  for (bpfSize i = 0; i < 3; i++) {
    for (bpfSize j = 0; j < 3; j++) {
      floatMatrix(i,j) = (float) matrix(i,j);
    }
  }

  return floatMatrix;
}

/**
 * Conversion from a Matrix with float to double
 */
bpfMatrix3<bpfDouble> bpfDoubleMatrix(const bpfMatrix3<bpfFloat> matrix)
{
  bpfMatrix3<bpfDouble> doubleMatrix;

  for (bpfSize i = 0; i < 3; i++) {
    for (bpfSize j = 0; j < 3; j++) {
      doubleMatrix(i,j) = (double) matrix(i,j);
    }
  }

  return doubleMatrix;
}
