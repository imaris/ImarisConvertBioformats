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


#ifndef __BPCOLORINFO_H__
#define __BPCOLORINFO_H__


#include "bpColor.h"


/**
 * Class used for storing the color information of one data channel. The color
 * can either be a base color or a color map. With the GetColor
 * method, an arbitrary value can be converted into a color.
 *
 * <p> For a base color, the brightness of the color for each data point is
 * determined by the value at that point. The base color specifies the hue, the
 * saturation and the maximal brightness.
 *
 * <p> A mapped color is determined from a color map entry. The color maps can
 * have arbitrary length. For practical purposes the length should also be
 * supported by the local openGL implementation.
 *
 * <p> The range to which the color is mapped is specified by its minimal and
 * maximal value. Values smaller than the minumal value will be mapped to black
 * in base color mode and to table entry 0 in mapped color mode. Values greater
 * than the maximal value will be set to the base color, respectively to the
 * maximal entry. The default range is 0.0 to 255.0
 *
 * <p> Besides the color an opacity value is stored. The opacity is defined
 * as 0.0 for the minimal value and 1.0 for the maximal value of the range.
 * The color and the opacity use the same range. (Added 16-Oct-2000, TK)
 *
 * @see bpColor
 *
 */
class bpColorInfo
{
public:
  /**
   * The supported color types.
   * @see bpColorInfo
   *
   */
  enum class tModeType {
    eBaseColor,
    eTableColor
  };

  /**
   * Default constructor. The default is base color mode with the default color
   * (white).
   */
  bpColorInfo();

  /**
   * Constructor for color info  with given base color. The base color is given
   * by the read, green and blue components.
   *
   * @param red The red component of the base color.
   * @param green The green component of the base color.
   * @param blue The blue component  of the base color.
   */
  bpColorInfo(bpFloat aRed, bpFloat aGreen, bpFloat aBlue, bpFloat aOpacity = 1.0);

  /**
   * Constructor for color info  with given base color. The color info is in base color
   * mode with the supplied base color.
   *
   * @param base The base color.
   */
  bpColorInfo(const bpColor& aBase, bpFloat aOpacity = 1.0);

  /**
   * Constructor for color info in map color mode. The color map
   * is supplied. The mode is set to table color.
   *
   * @param table The color map.
   */
  bpColorInfo(const std::vector<bpColor>& aTable, bpFloat aOpacity = 1.0);

  void DebugCreateRandomColorTable(bpSize aSize);

  /**
   * Get the current color mode.
   *
   * @return The color mode (BaseCOlor or TableColor).
   */
  tModeType GetColorMode() const;

  /**
   * Set the color mode. If the color mode is set to map
   * color and no color table is defined, then a new colormap
   * of size 256 is calculated.
   *
   * @param mode The new color mode.
   */
  void SetColorMode(tModeType aNewMode);

  /**
   * Set the base color.
   *
   * @param base The new base color.
   */
  void SetBaseColor(const bpColor& aBase);

  /**
   * Return the base color. The color info must be in base color mode.
   *
   * @return The base color.
   */
  bpColor GetBaseColor() const;

  /**
   * Return the base color. The color info must be in base color mode.
   *
   * @param base The color is returned in this parameter.
   */
  void GetBaseColor(bpColor& aBase) const;

  /**
   * Set the color table.
   *
   * @param table The new color table.
   */
  void SetTable(const std::vector<bpColor>& aTable);

  /**
   * Change (set) one table entry.
   *
   * @param i The index of the entry to change.
   * @param color The new color for this entry/
   * @exception bpIllegalRangeException if the index is out of range.
   */
  void SetTableEntry(bpSize aIndex, const bpColor& aColor);

  /**
   * Get the color table.
   */
  const std::vector<bpColor>& GetTable() const;

  /**
   * Get a table entry.
   *
   * @param i The index of the entry to return.
   * @return The color of the entry.
   * @exception bpIllegalRangeException if the index is out of range.
   */
  bpColor GetTableEntry(bpSize aIndex) const;

  /**
   * Get a table entry.
   *
   * @param i The index of the entry to return.
   * @param color The color of the entry.
   * @exception bpIllegalRangeException if the index is out of range.
   */
  void GetTableEntry(bpSize aIndex, bpColor& aColor) const;

  /**
   * Get a table entry (by reference) using the [] operator.
   *
   * @param i The index of the entry to return.
   * The color of the entry.
   * @exception bpIllegalRangeException if the index is out of range.
   */
  bpColor& operator[](bpSize aIndex);

  /**
   * Get a table entry using the [] operator.
   *
   * @param i The index of the entry to return.
   * The color of the entry.
   * @exception bpIllegalRangeException if the index is out of range.
   */
  const bpColor& operator[](bpSize aIndex) const;

  /**
   * Get the length of the color table. The color info must be in map color mode.
   *
   * @return The length of the color table.
   */
  bpSize GetTableLength() const;

  /**
   * Set the length of the color table. If the length is larger than the old
   * length, the new entries have the default color.
   *
   * @param aLength The new length of the table.
   */
  void SetTableLength(bpSize aLength);


  /**
   * Interpolate the table segment through linear interpolation in RGB
   * mode. Each component is interpolated separatly.
   *
   * @param from Index of first color in the interpolation.
   * @param to   Index of last color in the interpolation.
   * @exception bpIllegalRangeException If any index is out of range.
   */
  void InterpolateRGB(bpSize aFrom, bpSize aTo);

  /**
   * Interpolate the table segment through linear interpolation in HSV
   * mode. Each component is interpolated separatly, Interpolation in H is
   * always diretly between the lower and the higher value, i.e never across the
   * value 360 even if that would be the shorter path.
   *
   * @param from Index of first color in the interpolation.
   * @param to   Index of last color in the interpolation.
   * @exception bpIllegalRangeException If any index is out of range.
   */
  void InterpolateHSV(bpSize aFrom, bpSize aTo);

  /**
   * Set the range.
   *
   * @param min The minimal value of the range.
   * @param max The maximal value.
   * @exceptions bpArgumentException if min >= max
   */
  void SetRange(bpFloat aMin, bpFloat aMax);
  void SetRangeB(bpFloat aMin, bpFloat aMax);

  /**
   * Get the minimal value of the range.
   *
   * @return The minimal value of the range.
   */
  bpFloat GetRangeMin() const;
  bpFloat GetRangeMinB() const;

  /**
   * Get the maximal value of the range.
   *
   * @return The maximal value of the range.
   */
  bpFloat GetRangeMax() const;
  bpFloat GetRangeMaxB() const;

  /**
   * Set the current gammacorrection value. If gamma correction is
   * above 1 then the result will be brighter (Vout = Vin
   * power(1/gamma)). If aGamma is 0 or below 0 it will be
   * truncated to 0.1 to avoid division per 0. Maximal value is
   * 10.
   *
   * @param aGamma the gamma value.
   */
  void SetGammaCorrectionValue(bpFloat aGamma);

  /**
   * Get the current Gamma value.
   *
   * @return The current gamma value.
   */
  bpFloat GetGammaCorrectionValue() const;

  /**
   * Set the current gamma correction value using an equivalent
   * range value. Value is converted and truncated if needed to a
   * valid gamma correction (i.e. betweem 0.1 and 10).
   *
   * @param aGammaEquivalentRange the equivalent gamma value
   */
  void SetGammaCorrectionEquivalentRange(const bpFloat aGammaEquivalentRange);

  /**
   * Get the current gamma value as an equivalent range value
   * (i.e. if rangemin = 0 and rangemax = 255 and gamma = 1 then
   * the equivalent range would be 127.5).
   *
   * @return bpFloat the equivalent gamma value
   */
  bpFloat GetGammaCorrectionEquivalentRange() const;

  /**
   * Get the color for the specified value.
   *
   * @value The value for which to get the color.
   * @return The color for that value in the range
   */
  inline bpColor GetColor(bpFloat aValue) const;

  /**
   * Get the opacity for the specified value.
   *
   * @param value  The value for which to get the opacity.
   * @return The opacity for that value.
   */
  inline bpFloat GetOpacity(bpFloat aValue) const;
  inline bpFloat GetOpacity() const;
  inline void SetOpacity(bpFloat aOpacity);

  /**
   * Equality. Two color infos are equal if all of their components are equal.
   *
   * @param aOtherParameter The color info to compare to.
   */
  inline bool operator==(const bpColorInfo& aOtherParameter) const {
    if (mMode == aOtherParameter.mMode &&
        mBaseColor == aOtherParameter.mBaseColor &&
        mTableColor == aOtherParameter.mTableColor &&
        mOpacity == aOtherParameter.mOpacity &&
        mMinRange == aOtherParameter.mMinRange &&
        mMaxRange == aOtherParameter.mMaxRange &&
        mDeltaRange == aOtherParameter.mDeltaRange &&
        mMinRangeB == aOtherParameter.mMinRangeB &&
        mMaxRangeB == aOtherParameter.mMaxRangeB &&
        mDeltaRangeB == aOtherParameter.mDeltaRangeB &&
        mGammaCorrection == aOtherParameter.mGammaCorrection) {
      return true;
    }
    return false;
  }

  /**
   * InEquality. Two color infos are inequal if one of their components are inequal.
   *
   * @param aOtherParameter The color info to compare to.
   */
  inline bool operator!=(const bpColorInfo& aOtherParameter) const {
    return !this->operator ==(aOtherParameter);
  }

private:

  /// The mode (BaseColor or TableColor).
  tModeType mMode;

  /// The value of the base color.
  bpColor mBaseColor;

  /// The colors of the table.
  std::vector<bpColor> mTableColor;

  // the alpha (opacity) value for blending
  bpFloat mOpacity;

  // The range for the lower threshold
  bpFloat mMinRange;
  bpFloat mMaxRange;
  bpFloat mDeltaRange;

  // The range for the upper threshold
  bpFloat mMinRangeB;
  bpFloat mMaxRangeB;
  bpFloat mDeltaRangeB;

  // gamma
  bpFloat mGammaCorrection;
};

//inline function definitions
/**
 * Get the color for the specified value.
 *
 * @value The value for which to get the color.
 * @return The color for that value in the range
 */
inline bpColor
bpColorInfo::GetColor(bpFloat aValue) const
{
  if (mMode == tModeType::eBaseColor) {
    if (aValue <= mMinRange) {
      return bpColor(0,0,0);
    }
    else if (aValue >= mMaxRange) {
      return mBaseColor;
    }
    else {
      bpFloat t = (aValue-mMinRange)/ mDeltaRange;
      if (mGammaCorrection != 1.0f) {
        t = powf(t, 1.0f/mGammaCorrection);
      }
      return bpColor(t*mBaseColor.Red(),
                     t*mBaseColor.Green(),
                     t*mBaseColor.Blue());
    }
  }
  else {
    if (aValue <= mMinRange) {
      return mTableColor[0];
    }
    else if (aValue >= mMaxRange) {
      return mTableColor[mTableColor.size()-1];
    }
    else {
      bpFloat t = (aValue-mMinRange)/ mDeltaRange;
      if (mGammaCorrection != 1.0f) {
        t = powf(t, 1.0f/mGammaCorrection);
      }
      return mTableColor[(bpSize)(t*(mTableColor.size()-1))];
    }
  }
}

/**
  * Get the opacity for the specified value.
  *
  * @param aValue  The value for which to get the opacity.
  * @return The opacity for that value.
  */
inline bpFloat
bpColorInfo::GetOpacity(bpFloat aValue) const
{
  if (aValue <= mMinRange) {
    return 0.0f;
  }
  else if (aValue >= mMaxRange) {
    return mOpacity;
  }
  else {
    return mOpacity * (aValue - mMinRange) / mDeltaRange;
  }
}

inline bpFloat
bpColorInfo::GetOpacity() const
{
  return mOpacity;
}

inline void
bpColorInfo::SetOpacity(bpFloat aOpacity)
{
  mOpacity = aOpacity;
}

bpString bpToString(const bpColorInfo& aColorInfo);
bpString bpToString(bpColorInfo::tModeType aMode);
void bpFromString(const bpString& aText, bpColorInfo::tModeType& aMode);

// 0 <= opacity <= 1, 0 <= aBlendValue <= aMaxBlendValue
bpUInt32 bpConvertOpacityToBlendValue(bpFloat aOpacity, bpUInt32 aMaxBlendValue);
bpFloat bpConvertBlendValueToOpacity(bpUInt32 aBlendValue, bpUInt32 aMaxBlendValue);

#endif
