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


#include "fileiobase/types/bpfColorInfo.h"

#include "fileiobase/application/bpfException.h"


const bpfFloat mGammaCorrectionMax = 5.0f;
const bpfFloat mGammaCorrectionMin = 0.1f;



bpfColorInfo::bpfColorInfo()
  : mMode(eBaseColor),
    mBaseColor(1.0),
    mOpacity(1.0),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    mGammaCorrection(1.0f)
{
}


bpfColorInfo::bpfColorInfo(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue, bpfFloat aOpacity)
  : mMode(eBaseColor),
    mBaseColor(aRed, aGreen, aBlue),
    mOpacity(aOpacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    mGammaCorrection(1.0f)
{
}


bpfColorInfo::bpfColorInfo(const bpfColor& aBase, bpfFloat aOpacity)
  : mMode(eBaseColor),
    mBaseColor(aBase),
    mOpacity(aOpacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    mGammaCorrection(1.0f)
{
}


bpfColorInfo::bpfColorInfo(const std::vector<bpfColor>& aTable, bpfFloat aOpacity)
  : mMode(eTableColor),
    mTableColor(aTable),
    mOpacity(aOpacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    mGammaCorrection(1.0f)
{
}


bpfColorInfo::bpfColorInfo(const bpfColorInfo& aOther)
  : mMode(aOther.mMode),
    mBaseColor(aOther.mBaseColor),
    mTableColor(aOther.mTableColor),
    mOpacity(aOther.mOpacity),
    mMinRange(aOther.mMinRange),
    mMaxRange(aOther.mMaxRange),
    mDeltaRange(aOther.mDeltaRange),
    mGammaCorrection(aOther.mGammaCorrection)
{
}


bpfColorInfo::~bpfColorInfo()
{
}


bpfColorInfo::tModeType bpfColorInfo::GetColorMode() const
{
  return mMode;
}


void
bpfColorInfo::SetColorMode(tModeType aMode)
{
  // making sure we have a valid color table
  if(aMode == eTableColor && GetTableLength() == 0) {
    bpfColor vOldBase = mBaseColor;
    SetTableLength(256);
    (*this)[0].SetRGB(0, 0, 0);
    (*this)[GetTableLength() - 1] = vOldBase;
    InterpolateRGB(0, GetTableLength() - 1);
  }
  mMode = aMode;
}


void bpfColorInfo::SetBaseColor(const bpfColor& aBase)
{
  mBaseColor = aBase;
  mMode = eBaseColor;
}


bpfColor bpfColorInfo::GetBaseColor() const
{
  return mBaseColor;
}


void bpfColorInfo::GetBaseColor(bpfColor& aBase) const
{
  aBase = mBaseColor;
}


void bpfColorInfo::SetTable(const std::vector<bpfColor>& aTable)
{
  mTableColor = aTable;
  mMode = eTableColor;
}


const std::vector<bpfColor>& bpfColorInfo::GetTable() const
{
  return mTableColor;
}


void bpfColorInfo::SetTableEntry(bpfUInt32 aIndex, const bpfColor& aColor)
{
  mTableColor[aIndex] = aColor;
}


bpfColor bpfColorInfo::GetTableEntry(bpfUInt32 aIndex) const
{
  return mTableColor[aIndex];
}


void bpfColorInfo::GetTableEntry(bpfUInt32 aIndex, bpfColor& aColor) const
{
  aColor = mTableColor[aIndex];
}


bpfUInt32 bpfColorInfo::GetTableLength() const
{
  return static_cast<bpfUInt32>(mTableColor.size());
}


void bpfColorInfo::SetTableLength(bpfUInt32 aLength)
{
  mMode = eTableColor;
  if (mTableColor.size() > aLength)
    while (mTableColor.size() != aLength)
      mTableColor.pop_back();
  else {
    bpfColor vColor(1.0);

    while (mTableColor.size() != aLength)
      mTableColor.push_back(vColor);
  }
}


void bpfColorInfo::InterpolateRGB(bpfUInt32 aFrom, bpfUInt32 aTo)
{
  bpfFloat vR1, vG1, vB1, vR2, vG2, vB2;
  bpfFloat vRFactor = 0.0, vGFactor = 0.0, vBFactor = 0.0;

  if (aFrom > aTo) {
    std::swap(aFrom, aTo);
  }
  bpfUInt32 vStep = aTo - aFrom;

  mTableColor[aFrom].GetRGB(vR1, vG1, vB1);
  mTableColor[aTo].GetRGB(vR2, vG2, vB2);

  if (vR1 < vR2) {
    vRFactor = (vR2 - vR1) / vStep;
  }
  else if (vR1 > vR2) {
    vRFactor = -((vR1 - vR2) / vStep);
  }

  if (vB1 < vB2) {
    vBFactor = (vB2 - vB1) / vStep;
  }
  else if (vB1 > vB2) {
    vBFactor = -((vB1 - vB2) / vStep);
  }

  if (vG1 < vG2) {
    vGFactor = (vG2 - vG1) / vStep;
  }
  else if (vG1 > vG2) {
    vGFactor = -((vG1 - vG2) / vStep);
  }

  bpfUInt32 vIndex = 1;
  while (vIndex < vStep) {
    mTableColor[static_cast<bpfSize>(aFrom + vIndex)].SetRGB(vR1 + (vRFactor * vIndex),
                                                             vG1 + (vGFactor * vIndex),
                                                             vB1 + (vBFactor * vIndex));
    ++vIndex;
  }
}


void bpfColorInfo::InterpolateHSV(bpfUInt32 aFrom, bpfUInt32 aTo)
{
  bpfFloat vH1, vS1, vV1, vH2, vS2, vV2;
  bpfFloat vHFactor = 0.0, vSFactor = 0.0, vVFactor = 0.0;

  if (aFrom > aTo) {
    std::swap(aFrom, aTo);
  }
  bpfUInt32 vStep = aTo - aFrom;

  mTableColor[aFrom].GetHSV(vH1, vS1, vV1);
  mTableColor[aTo].GetHSV(vH2, vS2, vV2);

  if (vH1 < vH2) {
    vHFactor = (vH2 - vH1) / vStep;
  }
  else if (vH1 > vH2) {
    vHFactor = -((vH1 - vH2) / vStep);
  }

  if (vS1 < vS2) {
    vSFactor = (vS2 - vS1) / vStep;
  }
  else if (vS1 > vS2) {
    vSFactor = -((vS1 - vS2) / vStep);
  }

  if (vV1 < vV2) {
    vVFactor = (vV2 - vV1) / vStep;
  }
  else if (vV1 > vV2) {
    vVFactor = -((vV1 - vV2) / vStep);
  }

  bpfUInt32 vIndex = 1;
  while (vIndex < vStep) {
    mTableColor[static_cast<bpfSize>(aFrom + vIndex)].SetHSV(vH1 + (vHFactor * vIndex),
                                                             vS1 + (vSFactor * vIndex),
                                                             vV1 + (vVFactor * vIndex));
    ++vIndex;
  }

}


bpfColor& bpfColorInfo::operator[](bpfUInt32 aIndex)
{
  return mTableColor[aIndex];
}


const bpfColor& bpfColorInfo::operator[](bpfUInt32 aIndex) const
{
  return mTableColor[aIndex];
}


void bpfColorInfo::SetRange(bpfFloat aMin, bpfFloat aMax)
{
  if (aMin < aMax) {
    mMinRange = aMin;
    mMaxRange = aMax;
  }
  else if (aMin > aMax) {
    mMinRange = aMax;
    mMaxRange = aMin;
  }
  else {
    bpfFloat vDelta = 0.0005f + 0.0005f * static_cast<bpfFloat>(fabs(aMin));
    mMinRange = aMin - vDelta;
    mMaxRange = aMin + vDelta;
  }
  mDeltaRange = mMaxRange - mMinRange;
}


void bpfColorInfo::SetRangeFromType(bpfNumberType aType)
{
  switch (aType) {
  case bpfUInt8Type :
    mMinRange = 0.0F;
    mMaxRange = 255.0F;
    break;
  case bpfUInt16Type :
    mMinRange = 0.0F;
    mMaxRange = 65535.0F;
    break;
  case bpfUInt32Type :
    mMinRange = 0.0F;
    mMaxRange = 4294967295.0F;
    break;
  case bpfFloatType :
    mMinRange = 0.0F;
    mMaxRange = 1.0F;
    break;
  default:
    throw bpfException("bpfColorInfo::SetRangeFromType(" + bpfToString(aType) + ") - Number type not supported!");
    break;
  }
  mDeltaRange = mMaxRange; // - minRange;
}


bpfFloat bpfColorInfo::GetRangeMin() const
{
  return mMinRange;
}


bpfFloat bpfColorInfo::GetRangeMax() const
{
  return mMaxRange;
}


void bpfColorInfo::SetGammaCorrectionValue(bpfFloat aGamma)
{
  if (aGamma <= mGammaCorrectionMin) {
    mGammaCorrection = mGammaCorrectionMin;
  }
  else if (aGamma >= mGammaCorrectionMax) {
    mGammaCorrection = mGammaCorrectionMax;
  }
  else {
    mGammaCorrection = aGamma;
  }
}


bpfFloat bpfColorInfo::GetGammaCorrectionValue() const
{
  return mGammaCorrection;
}


void bpfColorInfo::SetGammaCorrectionEquivalentRange(const bpfFloat aGammaEquivalentRange)
{
  if (mMaxRange != mMinRange &&
      aGammaEquivalentRange > mMinRange) {
    bpfFloat vX = (aGammaEquivalentRange - mMinRange) / (mMaxRange - mMinRange);
    SetGammaCorrectionValue(- log10f(vX) / 0.30102999566f);	// gamma = -log(x)/log(2)
  }
}


bpfFloat bpfColorInfo::GetGammaCorrectionEquivalentRange() const
{
  // we follow the point in x so that pow(x, 1/gamma) = 0.5
  // if gamma = 1, then x = 0.5 and equivalent range = minRange + x*(maxRange - minRange)
  bpfFloat vX = powf(0.5f, mGammaCorrection);
  return (mMinRange + vX * (mMaxRange - mMinRange));
}
