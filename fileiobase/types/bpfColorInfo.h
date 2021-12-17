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


#ifndef __BP_COLORINFO_H__
#define __BP_COLORINFO_H__

#include "fileiobase/types/bpfColor.h"
#include "fileiobase/types/bpfPackedRGBA.h"


/**
 *
 * Class used for storing the color information of one data channel. The color
 * can either be a base color or a color map.
 *
 * <p> For a base color, the brightness of the color for each data point is
 * determined by the value at that point. The base color specifies the hue, the
 * saturation and the maximal brightness.
 *
 * <p> A mapped color is determined from a color map entry. The color maps can
 * have arbitrary length. For practical purposes the length should also be
 * supported by the local openGL implementation.
 *
 * <p> The range to which the color is mapped is specified by its minumal and
 * maximal value. Values smaller than the minimal value will be mapped to black
 * in base color mode and to table entry 0 in mapped color mode. Values greater
 * than the maximal value will be set to the base color, respectively to the
 * maximal entry. The default range is 0.0 to 255.0
 *
 * <p> Besides the color an opacity value is stored. The opacity is defined
 * as 0.0 for the minimal value and 1.0 for the maximal value of the range.
 * The color and the opacity use the same range. (Added 16-Oct-2000, TK)
 *
 * @see bpfColor
 *
 *
 *
 */
class bpfColorInfo
{
public:
  /**
   * The supported color types.
   * @see bpfColorInfo
   *
   */
  enum tModeType {
    eBaseColor,
    eTableColor
  };

  /**
   * Default constructor. The default is base color mode with the default color
   * (white).
   */
  bpfColorInfo();

  /**
   * Constructor for color info  with given base color. The base color is given
   * by the red, green and blue components.
   *
   * @param aRed The red component of the base color.
   * @param aGreen The green component of the base color.
   * @param aBlue The blue component of the base color.
   */
  bpfColorInfo(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue, bpfFloat aOpacity = 1.0);

  /**
   * Constructor for color info  with given base color. The color info is in base color
   * mode with the supplied base color.
   *
   * @param aBase The base color.
   */
  bpfColorInfo(const bpfColor& aBase, bpfFloat aOpacity = 1.0);

  /**
   * Constructor for color info in map color mode. The color map is supplied.
   *
   * @param aTable The color map.
   */
  bpfColorInfo(const std::vector<bpfColor>& aTable, bpfFloat aOpacity = 1.0);

  /**
   * Copy Constructor
   */
  bpfColorInfo(const bpfColorInfo& aOther);

  /**
   * Destructor.
   */
  ~bpfColorInfo();

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
   * @param aMode The new color mode.
   */
  void SetColorMode(tModeType aMode);

  /**
   * Set the base color.
   *
   * @param aBase The new base color.
   */
  void SetBaseColor(const bpfColor& aBase);

  /**
   * Return the base color. The color info must be in base color mode.
   *
   * @return The base color.
   */
  bpfColor GetBaseColor() const;

  /**
   * Return the base color. The color info must be in base color mode.
   *
   * @param aBase The color is returned in this parameter.
   */
  void GetBaseColor(bpfColor& aBase) const;

  /**
   * Set the color table.
   *
   * @param aTable The new color table.
   */
  void SetTable(const std::vector<bpfColor>& aTable);

  /**
   * Change (set) one table entry.
   *
   * @param aIndex The index of the entry to change.
   * @param aColor The new color for this entry/
   * @exception bpfIllegalRangeException if the index is out of range.
   */
  void SetTableEntry(bpfUInt32 aIndex, const bpfColor& aColor);

  /**
   * Get the color table.
   */
  const std::vector<bpfColor>& GetTable() const;

  /**
   * Get a table entry.
   *
   * @param aIndex The index of the entry to return.
   * @return The color of the entry.
   * @exception bpfIllegalRangeException if the index is out of range.
   */
  bpfColor GetTableEntry(bpfUInt32 aIndex) const;

  /**
   * Get a table entry.
   *
   * @param aIndex The index of the entry to return.
   * @param aColor The color of the entry.
   * @exception bpfIllegalRangeException if the index is out of range.
   */
  void GetTableEntry(bpfUInt32 aIndex, bpfColor& aColor) const;

  /**
   * Get a table entry (by reference) using the [] operator.
   *
   * @param aIndex The index of the entry to return.
   * The color of the entry.
   * @exception bpfIllegalRangeException if the index is out of range.
   */
  bpfColor& operator[](bpfUInt32 aIndex);

  /**
   * Get a table entry using the [] operator.
   *
   * @param aIndex The index of the entry to return.
   * The color of the entry.
   * @exception bpfIllegalRangeException if the index is out of range.
   */
  const bpfColor& operator[](bpfUInt32 aIndex) const;

  /**
   * Get the length of the color table. The color info must be in map color mode.
   *
   * @return The length of the color table.
   */
  bpfUInt32 GetTableLength() const;

  /**
   * Set the length of the color table. If the length is larger than the old
   * length, the new entries have the default color.
   *
   * @param aLength The new length of the table.
   */
  void SetTableLength(bpfUInt32 aLength);


  /**
   * Interpolate the table segment through linear interpolation in RGB
   * mode. Each component is interpolated separatly.
   *
   * @param aFrom Index of first color in the interpolation.
   * @param aTo   Index of last color in the interpolation.
   * @exception bpfIllegalRangeException If any index is out of range.
   */
  void InterpolateRGB(bpfUInt32 aFrom, bpfUInt32 aTo);

  /**
   * Interpolate the table segment through linear interpolation in HSV
   * mode. Each component is interpolated separatly, Interpolation in H is
   * always diretly between the lower and the higher value, i.e never across the
   * value 360 even if that would be the shorter path.
   *
   * @param aFrom Index of first color in the interpolation.
   * @param aTo   Index of last color in the interpolation.
   * @exception bpfIllegalRangeException If any index is out of range.
   */
  void InterpolateHSV(bpfUInt32 aFrom, bpfUInt32 aTo);

  /**
   * Set the range.
   *
   * @param aMin The minimal value of the range.
   * @param aMax The maximal value.
   */
  void SetRange(bpfFloat aMin, bpfFloat aMax);

  /**
   * Set a default range for this type. The ranges are
   *
   * bpfUInt8  : 0 255
   * bpfUInt16 : 0 65535
   * bpfFloat  : 0 1
   *
   * the other ranges are not yet supported.
   *
   * @param aType The type for which to set the range.
   * @exceptions bpfNotSupportedException is thrown if the type is not
   * supported.
   */
  void SetRangeFromType(bpfNumberType aType);

  /**
   * Get the minimal value of the range.
   *
   * @return The minimal value of the range.
   */
  bpfFloat GetRangeMin() const;

  /**
   * Get the maximal value of the range.
   *
   * @return The maximal value of the range.
   */
  bpfFloat GetRangeMax() const;

  /**
   * Set the current gammacorrection value. If gamma correction is
   * above 1 then the result will be brighter (Vout = Vin
   * power(1/gamma)). If aGamma is 0 or below 0 it will be
   * truncated to 0.1 to avoid division per 0. Maximal value is
   * 10.
   *
   * @param aGamma the gamma value.
   */
  void SetGammaCorrectionValue(bpfFloat aGamma);

  /**
   * Get the current Gamma value.
   *
   * @return The current gamma value.
   */
  bpfFloat GetGammaCorrectionValue() const;

  /**
   * Set the current gamma correction value using an equivalent
   * range value. Value is converted and truncated if needed to a
   * valid gamma correction (i.e. betweem 0.1 and 10).
   *
   * @param aGammaEquivalentRange the equivalent gamma value
   */
  void SetGammaCorrectionEquivalentRange(const bpfFloat aGammaEquivalentRange);

  /**
   * Get the current gamma value as an equivalent range value
   * (i.e. if rangemin = 0 and rangemax = 255 and gamma = 1 then
   * the equivalent range would be 127.5).
   *
   * @return bpfFloat the equivalent gamma value
   */
  bpfFloat GetGammaCorrectionEquivalentRange() const;

  /**
   * Get the color for the specified value.
   *
   * @value The aValue for which to get the color.
   * @return The color for that value in the range
   */
  inline bpfColor GetColor(bpfFloat aValue) const;

  /**
   * Get the opacity for the specified value.
   *
   * @param aValue  The value for which to get the opacity.
   * @return The opacity for that value.
   */
  inline bpfFloat GetOpacity(bpfFloat aValue) const;
  inline bpfFloat GetOpacity() const;
  inline void SetOpacity(bpfFloat aOpacity);

  /**
   * Get the rgba color and opacity for the specified value.
   *
   * @param aValue  The value for which to obtain the color and opacity.
   * @return The rgba color and opacity for this value.
   */
  inline bpfPackedRGBA GetRGBA32(bpfFloat aValue) const;

  //*********** Muss noch definiert werden !! *************************
  int operator<(const bpfColorInfo&)const {
    return 1;
  };
  int operator==(const bpfColorInfo&)const {
    return 0;
  };
  //*********** Muss noch definiert werden !! *************************


private:

  /// The mode (BaseColor or TableColor).
  tModeType mMode;

  /// The value of the base color.
  bpfColor mBaseColor;

  /// The colors of the table.
  std::vector<bpfColor> mTableColor;

  // the alpha (opacity) value for blending
  bpfFloat mOpacity;

  // The range
  bpfFloat mMinRange;
  bpfFloat mMaxRange;
  bpfFloat mDeltaRange;

  // gamma
  bpfFloat mGammaCorrection;
};

//inline function definitions
/**
 * Get the color for the specified value.
 *
 * @value The value for which to get the color.
 * @return The color for that value in the range
 */
inline bpfColor bpfColorInfo::GetColor(bpfFloat aValue) const
{
  if (mMode == eBaseColor) {
    if (aValue <= mMinRange) {
      return bpfColor(0,0,0);
    }
    else if (aValue >= mMaxRange) {
      return mBaseColor;
    }
    else {
      bpfFloat vT = (aValue-mMinRange)/ mDeltaRange;
      if (mGammaCorrection != 1.0f) {
        vT = powf(vT, 1.0f/mGammaCorrection);
      }
      return bpfColor(vT*mBaseColor.Red(),
                      vT*mBaseColor.Green(),
                      vT*mBaseColor.Blue());
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
      bpfFloat vT = (aValue-mMinRange)/ mDeltaRange;
      if (mGammaCorrection != 1.0f) {
        vT = powf(vT, 1.0f/mGammaCorrection);
      }
      return mTableColor[(bpfUInt32)(vT*(mTableColor.size()-1))];
    }
  }
}

/**
  * Get the opacity for the specified value.
  *
  * @param value  The value for which to get the opacity.
  * @return The opacity for that value.
  */
inline bpfFloat bpfColorInfo::GetOpacity(bpfFloat aValue) const
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

inline bpfFloat bpfColorInfo::GetOpacity() const
{
  return mOpacity;
}

inline void bpfColorInfo::SetOpacity(bpfFloat aOpacity)
{
  mOpacity = aOpacity;
}

/**
 * Get the rgba color and opacity for the specified value.
 *
 * @param value  The value for which to obtain the color and opacity.
 * @return The rgba color and opacity for this value.
 */
inline bpfPackedRGBA bpfColorInfo::GetRGBA32(bpfFloat aValue) const
{
  bpfPackedRGBA vOutColor = GetColor(aValue);
  vOutColor.a = (bpfUInt8)(255.0f * GetOpacity(aValue));
  return vOutColor;
}

#endif
