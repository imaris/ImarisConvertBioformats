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


#ifndef __BP_HISTOGRAM__
#define __BP_HISTOGRAM__
#include "fileiobase/types/bpfTypes.h"

/**
 * Interface for bpfHistogram.
 * To create an instance of bpfHistogram, one should use
 * bpfHistogramBlocked<tElementType, tIterator>.
 *
 *
 */
class bpfHistogram
{
public:
  /**
   * Destructor.
   */
  virtual ~bpfHistogram() {}
  /**
   * Returns the real number of bins used by the histogram.
   *
   * @return const bpfSize&
   */
  virtual bpfSize GetNumberOfBins() const = 0;

  /**
   * Returns the minimal bin value in the histogram.
   *
   * @return bpfFloat
   */
  virtual bpfFloat GetValueMinF() const = 0;

  /**
   * Returns the maximal bin value used in the histogram.
   *
   * @return bpfFloat
   */
  virtual bpfFloat GetValueMaxF() const = 0;

  /**
   * Returns the distance between two bins in the histogram. Note:
   * For 8-bit and 16-bit histograms the value is always 1.
   *
   * @return const bpfFloat&
   */
  virtual bpfFloat GetValueDelta() const = 0;

  /**
   * Returns the minimal bin value in the histogram.
   *
   * @return bpfFloat
   */
  virtual bpfFloat GetValueMinNonZeroF() const = 0;

  /**
   * Returns the maximal bin value used in the histogram.
   *
   * @return bpfFloat
   */
  virtual bpfFloat GetValueMaxNonZeroF() const = 0;

  /**
   * Returns the percentage of data above a certain aBinId. Note:
   * aBinId must be within range of the histogram.
   *
   * @param aBinId the bin above which the calculation takes
   *               place.
   *
   * @return bpfFloat the result in %
   */
  virtual bpfFloat GetDataPercentage(bpfSize aBinId) const = 0;

  /**
   * Returns the bin value at aBinId. Note: aBinId must be within
   * the range of the histogram.
   *
   * @param aBinId
   *
   * @return bpfFloat
   */
  virtual bpfFloat GetValueF(bpfSize aBinId) const = 0;

  /**
   * Returns the binId corresponding to aValue. Note: aValue must
   * be within range of the histogram.
   *
   * @param aValue
   *
   * @return bpfSize
   */
  virtual bpfSize GetBinIdF(bpfFloat aValue) const = 0;

  /**
   * Returns the number of count in aBinId. Note: aBinId must be
   * within range of the histogram.
   *
   * @param aBinId
   *
   * @return const bpfSize&
   */
  virtual bpfUInt64 GetCount(bpfSize aBinId) const = 0;
};



#endif // __BP_HISTOGRAM__

