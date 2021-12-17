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


#include "bpColorInfo.h"
#include <limits>

const bpFloat mGammaCorrectionMax = 5.0f;
const bpFloat mGammaCorrectionMin = 0.1f;


bpColorInfo::bpColorInfo()
  : mMode(tModeType::eBaseColor),
    mBaseColor(1.0),
    mOpacity(1.0),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    //mMinRangeB(0),
    //mMaxRangeB(255),
    mMinRangeB(std::numeric_limits<bpFloat>::max()),
    mMaxRangeB(std::numeric_limits<bpFloat>::max()),
    mDeltaRangeB(mMaxRangeB - mMinRangeB),
    mGammaCorrection(1.0f)
{
}


bpColorInfo::bpColorInfo(bpFloat red, bpFloat green, bpFloat blue, bpFloat opacity)
  : mMode(tModeType::eBaseColor),
    mBaseColor(red, green, blue),
    mOpacity(opacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    //mMinRangeB(0),
    //mMaxRangeB(255),
    mMinRangeB(std::numeric_limits<bpFloat>::max()),
    mMaxRangeB(std::numeric_limits<bpFloat>::max()),
    mDeltaRangeB(mMaxRangeB - mMinRangeB),
    mGammaCorrection(1.0f)
{
}


bpColorInfo::bpColorInfo(const bpColor& base, bpFloat opacity)
  : mMode(tModeType::eBaseColor),
    mBaseColor(base),
    mOpacity(opacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    //mMinRangeB(0),
    //mMaxRangeB(255),
    mMinRangeB(std::numeric_limits<bpFloat>::max()),
    mMaxRangeB(std::numeric_limits<bpFloat>::max()),
    mDeltaRangeB(mMaxRangeB - mMinRangeB),
    mGammaCorrection(1.0f)
{
}


bpColorInfo::bpColorInfo(const std::vector<bpColor>& table, bpFloat opacity)
  : mMode(tModeType::eTableColor),
    mTableColor(table),
    mOpacity(opacity),
    mMinRange(0),
    mMaxRange(255),
    mDeltaRange(mMaxRange - mMinRange),
    //mMinRangeB(0),
    //mMaxRangeB(255),
    mMinRangeB(std::numeric_limits<bpFloat>::max()),
    mMaxRangeB(std::numeric_limits<bpFloat>::max()),
    mDeltaRangeB(mMaxRangeB - mMinRangeB),
    mGammaCorrection(1.0f)
{
}


void bpColorInfo::DebugCreateRandomColorTable(bpSize aSize)
{
  if (aSize <  1) {
    mMode = tModeType::eBaseColor;
    mBaseColor = bpColor::RandomColor();

  }
  else {
    mMode = tModeType::eTableColor;
  }
  mOpacity = 1.0f;
  mMinRange = 0;
  mMaxRange = static_cast<bpFloat>(aSize);
  mDeltaRange = mMaxRange - mMinRange;
  mGammaCorrection = 1.0f;

  mTableColor.clear();
  bpSize vIndex = 0;
  while(vIndex < aSize) {
    mTableColor.push_back(bpColor::RandomColor());
    ++vIndex;
  }
}


bpString bpToString(const std::vector<bpColor>& aColors, const bpString& aSeparator, bpInt16 aPrecision)
{
  bpString vString;
  std::vector<bpColor>::const_iterator vColorIterator = aColors.begin();
  while (vColorIterator != aColors.end()) {
    vString += bpToString(*vColorIterator++, aPrecision);
    if (vColorIterator != aColors.end()) vString += aSeparator;
  }
  return vString;
}


bpString bpToString(const bpColorInfo& aColorInfo)
{
  bpString vString;
  vString += "Type = " + (aColorInfo.GetColorMode() == bpColorInfo::tModeType::eBaseColor ? bpString("BaseColor") : bpString("TableColor")) + "\n";
  vString += "Range = " + bpToString(aColorInfo.GetRangeMin()) + " " + bpToString(aColorInfo.GetRangeMax()) + "\n";
  vString += "Opacity = " + bpToString(aColorInfo.GetOpacity()) + "\n";
  vString += "BaseColor = " + bpToString(aColorInfo.GetBaseColor()) + "\n";
  vString += "Table = " + bpToString(aColorInfo.GetTable(), ";", 4) + "\n";
  return vString;
}


bpString bpToString(bpColorInfo::tModeType aMode)
{
  bpString vText;

  switch (aMode) {
    case bpColorInfo::tModeType::eBaseColor:
      vText = "BaseColor";
      break;
    case bpColorInfo::tModeType::eTableColor:
      vText = "TableColor";
      break;
    default:
      break;
  }

  return vText;
}


void bpFromString(const bpString& aText, bpColorInfo::tModeType& aMode)
{
  if (bpToString(bpColorInfo::tModeType::eBaseColor) == aText) {
    aMode = bpColorInfo::tModeType::eBaseColor;
  }
  else if (bpToString(bpColorInfo::tModeType::eTableColor) == aText) {
    aMode = bpColorInfo::tModeType::eTableColor;
  }
}


bpUInt32 bpConvertOpacityToBlendValue(bpFloat aOpacity, bpUInt32 aMaxBlendValue)
{
  bpFloat vBlendValue = std::min(std::max(std::pow(aOpacity, 1.0f / 3.0f), 0.0f), 1.0f);
  return static_cast<bpUInt32>(vBlendValue * aMaxBlendValue + 0.5f);
}


bpFloat bpConvertBlendValueToOpacity(bpUInt32 aBlendValue, bpUInt32 aMaxBlendValue)
{
  bpFloat vOpacity = static_cast<bpFloat>(aBlendValue) / static_cast<bpFloat>(aMaxBlendValue);
  return std::min(std::pow(vOpacity, 3.0f), 1.0f);
}

/*****************************************************************************
 *
 *
 *****************************************************************************/

bpColorInfo::tModeType bpColorInfo::GetColorMode() const
{
  return mMode;
}


/**
 * Set the color mode. If the color mode is set to map color and
 * no color table is defined, then a new colormap of size 256 is
 * calculated.
 *
 * @param newMode The new color mode.
 */
void
bpColorInfo::SetColorMode(tModeType newMode)
{
  // making sure we have a valid color table
  if(newMode == tModeType::eTableColor && GetTableLength() == 0) {
    bpColor oldBase = mBaseColor;
    SetTableLength(256);
    (*this)[0].SetRGB(0,0,0);
    (*this)[GetTableLength()-1] = oldBase;
    InterpolateRGB(0, GetTableLength() - 1);
  }
  mMode = newMode;
}

void bpColorInfo::SetBaseColor(const bpColor& base)
{
  mBaseColor = base;
  mMode = tModeType::eBaseColor;
}

bpColor bpColorInfo::GetBaseColor() const
{
  return mBaseColor;
}

void bpColorInfo::GetBaseColor(bpColor& base) const
{
  base = mBaseColor;
}

void bpColorInfo::SetTable(const std::vector<bpColor>& table)
{
  mTableColor = table;
  mMode = tModeType::eTableColor;
}

const std::vector<bpColor>& bpColorInfo::GetTable() const
{
  return mTableColor;
}

void bpColorInfo::SetTableEntry(bpSize i, const bpColor& color)
{
  if (mTableColor.size() <= i)
    throw std::runtime_error("bpColorInfo::SetTableEntry : illegal index");
  mTableColor[i] = color;
}

bpColor bpColorInfo::GetTableEntry(bpSize i) const
{
  if (mTableColor.size() <= i)
    throw std::runtime_error("bpColorInfo::GetTableEntry : illegal index");
  return mTableColor[i];
}


void bpColorInfo::GetTableEntry(bpSize i, bpColor& color) const
{
  if (mTableColor.size() <= i)
    throw std::runtime_error("bpColorInfo::GetTableEntry : illegal index");
  color = mTableColor[i];
}


bpSize bpColorInfo::GetTableLength() const
{
  return mTableColor.size();
}


void bpColorInfo::SetTableLength(bpSize lng)
{
  mMode = tModeType::eTableColor;
  if (mTableColor.size() > lng)
    while (mTableColor.size() != lng)
      mTableColor.pop_back();
  else {
    bpColor col(1.0);

    while (mTableColor.size() != lng)
      mTableColor.push_back(col);
  }
}


void bpColorInfo::InterpolateRGB(bpSize from, bpSize to)
{
  if ((mTableColor.size() < from) || (mTableColor.size() < to))
    throw std::runtime_error("bpColorInfo::InterpolateRGB : illegal index");

  bpSize step;
  bpFloat r1,g1,b1,r2,g2,b2;
  bpFloat rfactor = 0.0,gfactor = 0.0,bfactor= 0.0;

  if (from > to) {
    bpSize tmp = from;
    from = to;
    to = tmp;
  }

  step = to - from;

  mTableColor[from].GetRGB(r1,g1,b1);
  mTableColor[to].GetRGB(r2,g2,b2);

  if (r1 < r2) {
    rfactor = (r2-r1)/step;
  }
  else if (r1 > r2) {
    rfactor = -((r1-r2)/step);
  }

  if (b1 < b2) {
    bfactor = (b2-b1)/step;
  }
  else if (b1 > b2) {
    bfactor = -((b1-b2)/step);
  }

  if (g1 < g2) {
    gfactor = (g2-g1)/step;
  }
  else if (g1 > g2) {
    gfactor = -((g1-g2)/step);
  }

  bpSize i = 1;
  while (i < step) {
    bpFloat vIFloat = static_cast<bpFloat>(i);
    mTableColor[from+i].SetRGB(r1+(rfactor*vIFloat),
                               g1+(gfactor*vIFloat),
                               b1+(bfactor*vIFloat));
    ++i;
  }
}


void bpColorInfo::InterpolateHSV(bpSize from, bpSize to)
{
  if ((mTableColor.size() < from) || (mTableColor.size() < to))
    throw std::runtime_error("bpColorInfo::InterpolateHSV : illegal index");

  bpFloat h1,s1,v1,h2,s2,v2;
  bpFloat hfactor = 0.0,sfactor = 0.0,vfactor= 0.0;

  bpSize step;

  if (from > to) {
    bpSize tmp = from;
    from = to;
    to = tmp;
  }

  step = to - from;

  mTableColor[from].GetHSV(h1,s1,v1);
  mTableColor[to].GetHSV(h2,s2,v2);

  if (h1 < h2) {
    hfactor = (h2-h1)/step;
  }
  else if (h1 > h2) {
    hfactor = -((h1-h2)/step);
  }

  if (s1 < s2) {
    sfactor = (s2-s1)/step;
  }
  else if (s1 > s2) {
    sfactor = -((s1-s2)/step);
  }

  if (v1 < v2) {
    vfactor = (v2-v1)/step;
  }
  else if (v1 > v2) {
    vfactor = -((v1-v2)/step);
  }

  bpSize i = 1;
  while (i < step) {
    bpFloat vIFloat = static_cast<bpFloat>(i);
    mTableColor[from+i].SetHSV(h1+(hfactor*vIFloat),
                               s1+(sfactor*vIFloat),
                               v1+(vfactor*vIFloat));
    ++i;
  }

}


bpColor& bpColorInfo::operator[](bpSize i)
{
  if (mTableColor.size() < i)
    throw std::runtime_error("bpColorInfo::operator[] : illegal index");
  return mTableColor[i];
}


const bpColor& bpColorInfo::operator[](bpSize i) const
{
  if (mTableColor.size() < i)
    throw std::runtime_error("bpColorInfo::operator[] : illegal index");
  return mTableColor[i];
}

/**
 * Set the range.
 *
 * @param min The minimal value of the range.
 * @param max The maximal value.
 * @exceptions bpArgumentException if min >= max
 */
void
bpColorInfo::SetRange(bpFloat min, bpFloat max)
{
  if (min < max) {
    mMinRange = min;
    mMaxRange = max;
  }
  else if (min > max) {
    mMinRange = max;
    mMaxRange = min;
  }
  else {
    bpFloat vDelta = 0.0005f + 0.0005f * fabsf(min);
    mMinRange = min - vDelta;
    mMaxRange = min + vDelta;
  }
  mDeltaRange = mMaxRange - mMinRange;
}

void
bpColorInfo::SetRangeB(bpFloat min, bpFloat max)
{
  if (min < max) {
    mMinRangeB = min;
    mMaxRangeB = max;
  }
  else if (min > max) {
    mMinRangeB = max;
    mMaxRangeB = min;
  }
  else {
    bpFloat vDelta = 0.0005f + 0.0005f * fabsf(min);
    mMinRangeB = min - vDelta;
    mMaxRangeB = min + vDelta;
  }
  mDeltaRangeB = mMaxRangeB - mMinRangeB;
}


/**
 * Get the minimal value of the range.
 *
 * @return The minimal value of the range.
 */
bpFloat
bpColorInfo::GetRangeMin() const
{
  return mMinRange;
}

bpFloat bpColorInfo::GetRangeMinB() const
{
  return mMinRangeB;
}

/**
 * Get the maximal value of the range.
 *
 * @return The maximal value of the range.
 */
bpFloat
bpColorInfo::GetRangeMax() const
{
  return mMaxRange;
}

bpFloat bpColorInfo::GetRangeMaxB() const
{
  return mMaxRangeB;
}

void bpColorInfo::SetGammaCorrectionValue(bpFloat aGamma)
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
bpFloat bpColorInfo::GetGammaCorrectionValue() const
{
  return mGammaCorrection;
}

void bpColorInfo::SetGammaCorrectionEquivalentRange(const bpFloat aGammaEquivalentRange)
{
  if (mMaxRange != mMinRange &&
      aGammaEquivalentRange > mMinRange) {
    bpFloat vX = (aGammaEquivalentRange - mMinRange) / (mMaxRange - mMinRange);
    SetGammaCorrectionValue(- log10f(vX) / 0.30102999566f);	// gamma = -log(x)/log(2)
  }
}

bpFloat bpColorInfo::GetGammaCorrectionEquivalentRange() const
{
  // we follow the point in x so that pow(x, 1/gamma) = 0.5
  // if gamma = 1, then x = 0.5 and equivalent range = mMinRange + x*(mMaxRange - mMinRange)
  bpFloat vX = powf(0.5f, mGammaCorrection);
  return (mMinRange + vX * (mMaxRange - mMinRange));
}
