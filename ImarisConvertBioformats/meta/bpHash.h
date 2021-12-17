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


#ifndef __BP_HASH__
#define __BP_HASH__

/**
 * Creates a 32-bit hash code from a sequence of samples
 */
class bpHash {

public:

  bpHash()
  : mCode(0),
    mZ(1),
    mW(2) {
  }

  inline void AddSample(bpUInt32 aSample) {
    // xor sample and random number => different sequence of the same samples
    // results in a different code
    mCode += aSample ^ Rand();
  }

  template <typename TIterator>
  inline void AddSamples(TIterator aBegin, TIterator aEnd) {
    while (aBegin != aEnd) {
      AddSample(*aBegin);
      ++aBegin;
    }
  }

  inline bpUInt32 GetCode() const {
    return mCode;
  }

  inline void Reset() {
    mCode = 0;
    mZ = 1;
    mW = 2;
  }

private:

  // simple pseudo random number generator
  inline bpUInt32 Rand() {
    mZ = 36969 * (mZ & 65535) + (mZ >> 16);
    mW = 18000 * (mW & 65535) + (mW >> 16);
    return (mZ << 16) + mW;
  }

  bpUInt32 mCode;
  bpUInt32 mZ;
  bpUInt32 mW;

};

#endif // __BP_HASH__
