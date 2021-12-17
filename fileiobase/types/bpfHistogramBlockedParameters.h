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


#ifndef __BP_HISTOGRAM_BLOCKED_PARAMETERS__
#define __BP_HISTOGRAM_BLOCKED_PARAMETERS__

template <typename tElement>
class bpfHistogramParameters
{
private:
  tElement mMin;
  tElement mMax;
  bpfSize mNumberOfBins;
public:
  bpfHistogramParameters()
    : mNumberOfBins(0),
      mMin(0),
      mMax(0) {
  }
  bpfHistogramParameters(const bpfHistogramParameters& aOther)
    : mNumberOfBins(aOther.mNumberOfBins),
      mMax(aOther.mMax),
      mMin(aOther.mMin) {
  }
  bpfHistogramParameters(tElement aMin, tElement aMax, bpfSize aNumberOfBins)
    : mNumberOfBins(aNumberOfBins),
      mMax(aMax),
      mMin(aMin) {
  }
  bpfHistogramParameters& operator=(const bpfHistogramParameters& aOther) {
    mMin = aOther.mMin;
    mMax = aOther.mMax;
    mNumberOfBins = aOther.mNumberOfBins;
    return *this;
  }

  void SetParameters(const bpfSize& aNumberOfBins, const tElement& aMin, const tElement& aMax) {
    mNumberOfBins = aNumberOfBins;
    mMin = aMin;
    mMax = aMax;
  }

  template <typename tIterator>
  void ComputeParameters(tIterator aStart, tIterator aEnd) {
    if (aStart != aEnd) {
      mMin = *aStart;
      mMax = *aStart;
    }
    tIterator vIt(aStart);
    while (vIt != aEnd) {
      if (*vIt < mMin) {
        mMin = *vIt;
      }
      if (*vIt > mMax) {
        mMax = *vIt;
      }
      ++vIt;
    }
    mNumberOfBins = 1024;
  }

  void Merge(const bpfHistogramParameters& aOther) {
    if (mNumberOfBins == 0) {
      (*this)=aOther;
    }
    if (aOther.mNumberOfBins == 0) {
      return;
    }
    mMin = (mMin < aOther.mMin ? mMin : aOther.mMin);
    mMax = (mMax > aOther.mMax ? mMax : aOther.mMax);
  }

  tElement GetMin() const {
    return mMin;
  }
  tElement GetMax() const {
    return mMax;
  }
  bpfSize GetNumberOfBins() const {
    return mNumberOfBins;
  }
  bpfSize GetBin(tElement aValue) const {
    return aValue < mMax ? (bpfSize)(((bpfFloat)(aValue-mMin))/(mMax-mMin)*mNumberOfBins) : (mNumberOfBins-1);
  }
  bpfFloat GetValue(bpfSize aBin) const {
    return aBin*((bpfFloat)(mMax-mMin))/mNumberOfBins+mMin;
  }

};

template <>
class bpfHistogramParameters<bpfUInt8>
{
public:
  bpfHistogramParameters() {
  }
  bpfHistogramParameters(bpfUInt8 /* aMin */, bpfUInt8 /* aMax */, bpfSize /* aNumberOfBins */) {
  }
  bpfHistogramParameters(const bpfHistogramParameters& ) {
  }
  bpfHistogramParameters& operator=(const bpfHistogramParameters& ) {
    return *this;
  }

  void SetParameters(const bpfSize&, const bpfUInt8&, const bpfUInt8&) {
  }

  template <typename tIterator>
  void ComputeParameters(tIterator /* aStart */, tIterator /* aEnd */) {
  }

  void Merge(const bpfHistogramParameters<bpfUInt8>&) {
  }

  bpfUInt8 GetMin() const {
    return 0;
  }
  bpfUInt8 GetMax() const {
    return 255;
  }
  bpfSize GetNumberOfBins() const {
    return 256;
  }
  bpfSize GetBin(bpfUInt8 aValue) const {
    return aValue;
  }
  bpfFloat GetValue(bpfSize aBin) const {
    return (bpfFloat)aBin;
  }
};


#endif

