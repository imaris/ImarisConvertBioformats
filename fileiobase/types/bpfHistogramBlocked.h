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


#ifndef __BP_HISTOGRAM_BLOCKED__
#define __BP_HISTOGRAM_BLOCKED__

#include "fileiobase/types/bpfHistogramBlockedParameters.h"
#include "fileiobase/types/bpfHistogram.h"

#include <cassert>
#include <string.h>


/**
 * Class for calculating, estimating a (non-)masked histogram.
 *
 * The final number of bins in the histogram is dependant of the
 * min and max of the datas which are analyzed. For example, a
 * 8-bit histogram can have a maximum of 256 bins as mValueDelta
 * is always 1 (also for 16-bit). A 16-bit histogram has a
 * maximum number of bins of 256 and a variable mValueDelta,
 * which goes from 1 to 256. A float histogram has always 256
 * bins and a variable mValueDelta.
 *
 * Usage:
 * bpfHistogramBlock<bpfUInt8> vHistogram;
 * vHistogram.Calculate(vStartIterator, vEndIterator,
 * vDataSize);
 *
 * It is possible to add histogram with the same type using the
 * Add function.
 *
 * The number of bins can be retrieved using GetNumberOfBins().
 *
 */
template<typename tElementType>
class bpfHistogramBlocked : public bpfHistogram
{
public:

  bpfHistogramBlocked()
    : mBin(0) {
  }

  bpfHistogramBlocked(const bpfHistogramBlocked& aOther)
    : bpfHistogram(aOther),
      mHistogramParameters(aOther.mHistogramParameters),
      mBin(0) {
    if (aOther.mBin != 0) {
      mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
      memcpy(mBin, aOther.mBin, mHistogramParameters.GetNumberOfBins() * sizeof(bpfUInt64));
    }
  }

  bpfHistogramBlocked(const std::vector<bpfUInt64>& aBins, tElementType aMinValue, tElementType aMaxValue)
    : mHistogramParameters(aMinValue, aMaxValue, aBins.size()) {
    mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
    for (bpfSize vBinId = 0; vBinId < mHistogramParameters.GetNumberOfBins(); ++vBinId) {
      mBin[vBinId] = aBins[vBinId];
    }
  }

  bpfHistogramBlocked& operator=(const bpfHistogramBlocked& aOther) {
    mHistogramParameters = aOther.mHistogramParameters;
    delete [] mBin;
    mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
    memcpy(mBin, aOther.mBin, mHistogramParameters.GetNumberOfBins() * sizeof(bpfUInt64));
    return (*this);
  }

  virtual ~bpfHistogramBlocked() {
    delete [] mBin;
  }

  const bpfUInt64* GetBin() const {
    return mBin;
  }

public:

  /**
  * Add the aHistogram to the existing histogram
  */
  inline void Add(const bpfHistogramBlocked& aOther) {
    if (aOther.mBin == 0) {
      return;
    }
    if (mBin == 0) {
      (*this) = aOther;
    }
    else {
      bpfHistogramParameters<tElementType> vMergedParameters(mHistogramParameters);
      vMergedParameters.Merge(aOther.mHistogramParameters);
      if (vMergedParameters.GetMin() == mHistogramParameters.GetMin() &&
          vMergedParameters.GetMax() == mHistogramParameters.GetMax()) {
        // add other histogram
        for (bpfSize vBin = 0; vBin < aOther.mHistogramParameters.GetNumberOfBins(); ++vBin) {
          bpfSize vNewBin = mHistogramParameters.GetBin((tElementType)aOther.mHistogramParameters.GetValue(vBin));
          mBin[vNewBin] += aOther.GetCount(vBin);
        }
      }
      else {
        // redistribute this histogram and add other histogram
        bpfHistogramBlocked vCopyOfThis(*this);
        mHistogramParameters.Merge(aOther.mHistogramParameters);
        delete [] mBin;
        mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
        memset(mBin, 0, mHistogramParameters.GetNumberOfBins()*sizeof(bpfUInt64));
        for (bpfSize vBin = 0; vBin < vCopyOfThis.mHistogramParameters.GetNumberOfBins(); ++vBin) {
          bpfSize vNewBin = mHistogramParameters.GetBin((tElementType)vCopyOfThis.mHistogramParameters.GetValue(vBin));
          mBin[vNewBin] += vCopyOfThis.GetCount(vBin);
        }
        for (bpfSize vBin = 0; vBin < aOther.mHistogramParameters.GetNumberOfBins(); ++vBin) {
          bpfSize vNewBin = mHistogramParameters.GetBin((tElementType)aOther.mHistogramParameters.GetValue(vBin));
          mBin[vNewBin] += aOther.GetCount(vBin);
        }
      }
    }
  }

  /**
   * Copy the information from a bin defined between
   * aStartIterator and aEndIterator to the histogram.
   *
   * @param aStartIterator new bin array begin
   * @param aEndIterator new bin array end
   * @param aMin the minimum value in the bin
   * @param aMax the maximum value in the bin
   */
  template<typename tBinIterator>
  inline void CopyTo(const tBinIterator& aStartIterator, const tBinIterator& aEndIterator, const tElementType& aMin, const tElementType& aMax) {
    mHistogramParameters.SetParameters(static_cast<bpfSize>(aEndIterator - aStartIterator), aMin, aMax);

    delete [] mBin;
    mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
    memset(mBin, 0, mHistogramParameters.GetNumberOfBins()*sizeof(bpfUInt64));

    // copy the bin
    tBinIterator vIt(aStartIterator);
    bpfUInt64* vBinIterator = mBin;
    while (vIt != aEndIterator) {
      *vBinIterator = *vIt;
      ++vBinIterator;
      ++vIt;
    }
  }

  /**
   * Calculate the histogram for ALL the data defined between
   * aStartIterator and aEndIterator.
   *
   * @param aStartIterator
   * @param aEndIterator
   */
  template<typename tIterator>
  inline void Calculate(const tIterator& aStartIterator, const tIterator& aEndIterator) {
    mHistogramParameters.ComputeParameters(aStartIterator,aEndIterator);

    delete [] mBin;
    mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
    memset(mBin, 0, mHistogramParameters.GetNumberOfBins()*sizeof(bpfUInt64));

    tIterator vIt(aStartIterator);
    while (vIt != aEndIterator) {
      bpfSize vBinId = mHistogramParameters.GetBin(*vIt);
      ++mBin[vBinId];
      ++vIt;
    }
  }

  /**
   * Calculate the histogram for ALL the data defined between
   * aStartIterator and aEndIterator. The data will only be taken
   * in account when the corresponding masked data is above
   * aMaskThreshold.
   *
   * @param aStartIterator
   * @param aEndIterator
   * @param aMaskStartIterator
   * @param aMaskEndIterator
   * @param aMaskThreshold
   */
  template<typename tIterator>
  inline void CalculateMasked(const tIterator& aStartIterator, const tIterator& aEndIterator, const tIterator& aMaskStartIterator, tElementType aMaskThreshold) {
    mHistogramParameters.ComputeParameters(aStartIterator,aEndIterator);

    delete [] mBin;
    mBin = new bpfUInt64[mHistogramParameters.GetNumberOfBins()];
    memset(mBin, 0, mHistogramParameters.GetNumberOfBins()*sizeof(bpfUInt64));

    tIterator vMaskIt(aMaskStartIterator);
    tIterator vIt(aStartIterator);
    while (vIt != aEndIterator) {
      if (!((*vMaskIt) < aMaskThreshold)) {
        bpfSize vBinId = mHistogramParameters.GetBin(*vIt);
        mBin[vBinId]++;
      }
      ++vIt;
      ++vMaskIt;
    }
  }

  /**
   * Returns the number of bins
   */
  inline virtual bpfSize GetNumberOfBins() const {
    return mHistogramParameters.GetNumberOfBins();
  }

  /**
   * Returns the minimum data value
   */
  inline tElementType GetValueMin() const {
    return mHistogramParameters.GetMin();
  }

  /**
   * Returns the maximum data value
   */
  inline tElementType GetValueMax() const {
    return mHistogramParameters.GetMax();
  }

  /**
   * Returns the minimum data value
   */
  inline tElementType GetValueMinNonZero() const {
    if (mBin != 0 && GetCount(0) > 0) {
      return (tElementType)mHistogramParameters.GetValue(0);
    }
    for (bpfSize vBin = 0; vBin < mHistogramParameters.GetNumberOfBins(); ++vBin) {
      if (GetCount(vBin) > 0) {
        return (tElementType)mHistogramParameters.GetValue(vBin);
      }
    }
    return 0;
  }

  /**
   * Returns the maximum data value
   */
  inline tElementType GetValueMaxNonZero() const {
    if (mBin != 0 && GetCount(mHistogramParameters.GetNumberOfBins()-1) > 0) {
      return (tElementType)mHistogramParameters.GetValue(mHistogramParameters.GetNumberOfBins()-1);
    }
    for (bpfSize vBin = (mHistogramParameters.GetNumberOfBins()-1); vBin > 0 ; --vBin) {
      if (GetCount(vBin) > 0) {
        return (tElementType)mHistogramParameters.GetValue(vBin);
      }
    }
    return 0;
  }

  /**
   * Returns the distance between two bins
   */
  virtual bpfFloat GetValueDelta() const {
    return ((bpfFloat)(mHistogramParameters.GetMax()-mHistogramParameters.GetMin()))/mHistogramParameters.GetNumberOfBins();
  }

  /**
   * Returns the index of the bin for "aValue".
   *
   * @param aValue Must be within the value range defined by
   *               GetValueMin and GetValueMax.
   */
  inline bpfSize GetBinId(tElementType aValue) const {
    if (aValue < mHistogramParameters.GetMin()) {
      return 0;
    }
    if (aValue > mHistogramParameters.GetMax()) {
      return mHistogramParameters.GetNumberOfBins();
    }
    return mHistogramParameters.GetBin(aValue);
  }

  virtual bpfUInt64 GetCount(bpfSize aBinId) const {
    assert(mBin != 0);
    assert(aBinId < GetNumberOfBins());
    return mBin[aBinId];
  }

  virtual void SetCount(bpfSize aBinId, bpfUInt64 aCount) {
    if(mBin != 0 && aBinId < GetNumberOfBins()) {
      mBin[aBinId] = aCount;
    }
  }

  /**
   * Returns the percentage of the data above the specified bin
   *
   * @param aBinId Must be within the range
   */
  virtual bpfFloat GetDataPercentage(bpfSize aBinId) const {
    assert(mBin != 0);
    assert(aBinId < GetNumberOfBins());

    bpfUInt64 vVolumeSum = 0;
    bpfUInt64 vVolume = 0;
    for (bpfSize vBinId = 0; vBinId < mHistogramParameters.GetNumberOfBins(); vBinId++) {
      vVolumeSum += mBin[vBinId];
      if (!(vBinId < aBinId)) {
        vVolume += mBin[vBinId];
      }
    }
    if (vVolumeSum != 0) {
      return 100.0f * (bpfFloat)vVolume / (bpfFloat)vVolumeSum;
    }
    else {
      return 0.0f;
    }
  }

private:

  // THE bin
  bpfUInt64* mBin;

  bpfHistogramParameters<tElementType> mHistogramParameters;


public:

  virtual bpfFloat GetValueMinF() const {
    return (bpfFloat)GetValueMin();
  }
  virtual bpfFloat GetValueMaxF() const {
    return (bpfFloat)GetValueMax();
  }
  virtual bpfFloat GetValueMinNonZeroF() const {
    return (bpfFloat)GetValueMinNonZero();
  }
  virtual bpfFloat GetValueMaxNonZeroF() const {
    return (bpfFloat)GetValueMaxNonZero();
  }

  virtual bpfSize GetBinIdF(bpfFloat aValue) const {
    return mHistogramParameters.GetBin((tElementType)(aValue));
  }

  virtual bpfFloat GetValueF(bpfSize aBindId) const {
    return (bpfFloat)mHistogramParameters.GetValue(aBindId);
  }
};


#endif // __BP_HISTOGRAM_BLOCKED__

