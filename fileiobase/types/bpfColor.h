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


#ifndef __BP_COLOR_H__
#define __BP_COLOR_H__

#include "fileiobase/types/bpfTypes.h"
#include "fileiobase/types/bpfVector3.h"

/**
 * Class that describes a stored color.
 */
class bpfColor
{

public:

  /**
   * @name Constructors
   *
   * There are four constructors:
   * - Constructor for graystyle colors.
   * - Constructor for a RGB color with the tree channels.
   * - Constructor for a RGB array.
   */

  /**
   * Default constructor. The color is set to white.
   */
  inline bpfColor();

  /**
   * Constructor for a greyvalue color.
   *
   * @param aGrayValue The greyvalue of the color as a float in the range [0..1].
   */
  inline bpfColor(bpfFloat aGrayValue);

  /**
   * Constructor that sets the color in RGB space.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline bpfColor(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue);

  /**
   * Constructor that sets the color using an array.
   *
   * @param aRGB   An array containing the red, green and blue component of the
   *               color.
   */
  inline bpfColor(bpfFloat aRGB[3]);

  /**
   * Set the rgb components of bpfcolor.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline void SetRGB(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue);

  /**
   * Get the rgb components of bpfcolor.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline void GetRGB(bpfFloat& aRed, bpfFloat& aGreen, bpfFloat& aBlue) const;
  inline bpfVector3Float GetRGBVector() const;
  inline const bpfFloat* GetColor3fv() const;

  inline bpfFloat Red() const;
  inline bpfFloat Green() const;
  inline bpfFloat Blue() const;

  /**
   * Set the hsv components of bpfcolor.
   *
   * @param aHue        The hue of the color.
   * @param aSaturation The saturation of the color.
   * @param aValue      The brighness value of the color.
   */
  inline void SetHSV(bpfFloat aHue, bpfFloat aSaturation, bpfFloat aValue);

  /**
   * Get the hsv components of bpfcolor.
   *
   * @param aHue        The hue of the color.
   * @param aSaturation The saturation of the color.
   * @param aValue      The brighness value of the color.
   */
  inline void GetHSV(bpfFloat& aHue, bpfFloat& aSaturation, bpfFloat& aValue) const;

  /**
   * Set the color to a grayvalue.
   *
   * @param aGrayValue   The gray value to set.
   */
  inline void SetGrayValue(bpfFloat aGrayValue);

  /**
   * Get the maximal RGB component.
   *
   * @return The value of the maximal component.
   */
  inline bpfFloat GetMaxRGB() const;

  /**
   * Get the minimal RGB component.
   *
   * @return The value of the minimal component.
   */
  inline bpfFloat GetMinRGB() const;

  /**
   * Get the luminance of the color.
   *
   * @return The luminance.
   */
  inline bpfFloat GetLuminance() const;

  /**
   * Returns a black or white color depending on the luminance
   */
  bpfColor GetContrastColor() const;

  /**
   * Returns a similar random color
   *
   * @param aVariance similarity of the resulting color (0 => equal; 1 => totally different)
   * @return
   */
  bpfColor GetRandomColor(bpfFloat aVariance = 1.0f) const;

  /**
   * Convert from HSV to RGB color space.
   */
  static void HSVtoRGB(bpfFloat aHue, bpfFloat aSaturation, bpfFloat aValue,
                       bpfFloat& aRed, bpfFloat& aGreen, bpfFloat& aBlue);

  /**
   * Convert from RGB to HSV color space.
   */
  static void RGBtoHSV(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue,
                       bpfFloat& aHue, bpfFloat& aSaturation, bpfFloat& aValue);

  /**
   * Operations.
   */

  /**
   * Addition of two colors. The range is not checked.
   *
   * @param aColor The color to be added.
   * @return *this.
   */
  inline bpfColor& operator+=(const bpfColor& aColor);

  /**
   * Subtraction of two color. The range is not checked.
   *
   * @param aColor The color to be subtracted.
   * @return *this
   */
  inline bpfColor& operator-=(const bpfColor& aColor);

  /**
   * Multiplication with a bpfFloat. (Underflow/overflow is not checked).
   *
   * @param aFactor The multiplication factor.
   * @return *this
   */
  inline bpfColor& operator*=(bpfFloat aFactor);


  /**
   * Equality. Two colors are equal if all of their components are equal.
   *
   * @param aColor The color to compare to.
   */
  inline bool operator==(const bpfColor& aColor) const;

  /**
   * InEquality. Two colors are inequal if one of their components are inequal.
   *
   * @param aColor The color to compare to.
   */
  inline bool operator!=(const bpfColor& aColor) const;

  /**
   * Greater operator. A color is greater than another color if its maximal
   * component is greater than the maximal component of the other color. The
   * components must not be the same, i.e. (1.0,0,0) > (0,0.7,0)
   *
   * @param aColor The color to compare to.
   */
  inline bool operator<(const bpfColor& aColor) const;

  /**
   * Clamp all components to the range [0..1].
   */
  inline void Clamp();

private:

  /**
   * The color is locally stored as RGB array.
   */
  bpfFloat mRGB[3];
};

/**
 * Addition of two colors. No range checking is done.
 *
 * @param aColor1 The first color.
 * @param aColor2 The second color.
 * @return The sum aColor1 + aColor2 of the two colors (by components)
 */
inline bpfColor operator+(const bpfColor& aColor1, const bpfColor& aColor2);

/**
 * Subtraction of two colors. No range checking is done.
 *
 * @param aColor1 The color from which the second color is subtracted.
 * @param aColor2 The color that is subtracted from the first color.
 * @return The color aColor1 - aColor2 (by components)
 */
inline bpfColor operator-(const bpfColor& aColor1, const bpfColor& aColor2);

// Functions for the use as argument to an image field. This requires
// that a multiplication by a bpfFloat value is supported by the element (for
// interpolation).

/**
 * Multiplication with a bpfFloat value (over/underflow is not checked)
 *
 * @param aColor The color.
 * @param aFactor The multiplication factor.
 * @return The multiplied color (each component is multiplied).
 */
inline bpfColor operator*(const bpfColor& aColor, bpfFloat aFactor);

/**
 * Multiplication with a bpfFloat value (over/underflow is not checked)
 *
 * @param aFactor The multiplication factor.
 * @param aColor The color.
 * @return The multiplied color (each component is multiplied).
 */
inline bpfColor operator*(bpfFloat aFactor, const bpfColor& aColor);

/**
 * Convert a string to a bpfColor
 *
 * @param aColorString The string from which to get the value
 * @param aColor The value (OUT)
 */
inline void bpfFromString(const bpfString& aColorString, bpfColor& aColor);

/**
 * Convert a color to a bpfString.
 *
 * @param aColor The color to convert.
 * @return The string (OUT)
 */
inline bpfString bpfToString(const bpfColor& aColor, bpfInt16 aFixedPrecision = 8);

/**
 * Round a numeric input value to another numeric output value
 *
 * @param aColorIn  The value to be rounded.
 * @param aColorOut The result of the rounding.
 */
inline void bpfRound(bpfColor aColorIn, bpfColor& aColorOut);


// inline function definitions
//
/**
 * Default constructor. The color is set to white.
 */
inline bpfColor::bpfColor()
{
  mRGB[0] = 1.0f;
  mRGB[1] = 1.0f;
  mRGB[2] = 1.0f;
}


/**
 * Constructor for a greyvalue color.
 *
 * @param aGrayValue The greyvalue of the color as a float in the range [0..1].
 */
inline bpfColor::bpfColor(bpfFloat aGrayValue)
{
  mRGB[0] = aGrayValue;
  mRGB[1] = aGrayValue;
  mRGB[2] = aGrayValue;
}


/**
 * Constructor that sets the color in RGB space.
 *
 * @param aRed   The red component of the color.
 * @param aGreen The green component of the color.
 * @param aBlue  The blue component of the color.
 */
inline bpfColor::bpfColor(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue)
{
  mRGB[0] = aRed;
  mRGB[1] = aGreen;
  mRGB[2] = aBlue;
}


/**
 * Constructor that sets the color using an array.
 *
 * @param aRGB   An array containing the red, green and blue component of the
 *               color.
 */
inline bpfColor::bpfColor(bpfFloat aRGB[3])
{
  mRGB[0] = aRGB[0];
  mRGB[1] = aRGB[1];
  mRGB[2] = aRGB[2];
}


/**
 * Set the rgb components of bpfcolor.
 *
 * @param aRed   The red component of the color.
 * @param aGreen The green component of the color.
 * @param aBlue  The blue component of the color.
 */
inline void bpfColor::SetRGB(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue)
{
  mRGB[0] = aRed;
  mRGB[1] = aGreen;
  mRGB[2] = aBlue;
}


/**
 * Get the rgb components of bpfcolor.
 *
 * @param aRed   The red component of the color.
 * @param aGreen The green component of the color.
 * @param aBlue  The blue component of the color.
 */
inline void bpfColor::GetRGB(bpfFloat& aRed, bpfFloat& aGreen, bpfFloat& aBlue) const
{
  aRed   = mRGB[0];
  aGreen = mRGB[1];
  aBlue  = mRGB[2];
}

inline bpfVector3Float bpfColor::GetRGBVector() const
{
  return bpfVector3Float(mRGB[0],mRGB[1],mRGB[2]);
}


inline const bpfFloat* bpfColor::GetColor3fv() const
{
  return mRGB;
}


inline bpfFloat bpfColor::Red() const
{
  return mRGB[0];
}


inline bpfFloat bpfColor::Green() const
{
  return mRGB[1];
}


inline bpfFloat bpfColor::Blue() const
{
  return mRGB[2];
}


/**
 * Set the hsv components of bpfcolor.
 *
 * @param aHue        The hue of the color.
 * @param aSaturation The saturation of the color.
 * @param aValue      The brighness value of the color.
 */
inline void bpfColor::SetHSV(bpfFloat aHue, bpfFloat aSaturation, bpfFloat aValue)
{
  HSVtoRGB(aHue, aSaturation, aValue,
           mRGB[0], mRGB[1], mRGB[2]);
}


/**
 * Get the hsv components of bpfcolor.
 *
 * @param aHue        The hue of the color.
 * @param aSaturation The saturation of the color.
 * @param aValue      The brighness value of the color.
 */
inline void bpfColor::GetHSV(bpfFloat& aHue, bpfFloat& aSaturation, bpfFloat& aValue) const
{
  RGBtoHSV(mRGB[0], mRGB[1], mRGB[2],
           aHue, aSaturation, aValue);
}


/**
 * Set the color to a grayvalue.
 *
 * @param aGrayValue   The gray value to set.
 */
inline void bpfColor::SetGrayValue(bpfFloat aGrayValue)
{
  mRGB[0] = aGrayValue;
  mRGB[1] = aGrayValue;
  mRGB[2] = aGrayValue;
}


/**
 * Get the maximal RGB component.
 *
 * @return The value of the maximal component.
 */
inline bpfFloat bpfColor::GetMaxRGB() const
{
  if (mRGB[0] > mRGB[1]) {
    if (mRGB[0] > mRGB[2]) {
      return mRGB[0];
    }
    else {
      return mRGB[2];
    }
  }
  else {
    if (mRGB[1] > mRGB[2]) {
      return mRGB[1];
    }
    else {
      return mRGB[2];
    }
  }
}


/**
 * Get the minimal RGB component.
 *
 * @return The value of the minimal component.
 */
inline bpfFloat bpfColor::GetMinRGB() const
{
  if (mRGB[0] < mRGB[1]) {
    if (mRGB[0] < mRGB[2]) {
      return mRGB[0];
    }
    else {
      return mRGB[2];
    }
  }
  else {
    if (mRGB[1] < mRGB[2]) {
      return mRGB[1];
    }
    else {
      return mRGB[2];
    }
  }
}


/**
 * Get the luminance of the color.
 *
 * @return The luminance.
 */
bpfFloat bpfColor::GetLuminance() const
{
  return 0.299f*mRGB[0] + 0.587f*mRGB[1] + 0.114f*mRGB[2];
}


/*******************************************************************************
 * Operations.
 */


/**
 * Addition of two colors. The range is not checked.
 *
 * @param aColor The color to be added.
 * @return *this.
 */
inline bpfColor& bpfColor::operator+=(const bpfColor& aColor)
{
  mRGB[0] += aColor.mRGB[0];
  mRGB[1] += aColor.mRGB[1];
  mRGB[2] += aColor.mRGB[2];

  return *this;
}


/**
 * Subtraction of two color. The range is not checked.
 *
 * @param aColor The color to be subtracted.
 * @return *this
 */
inline bpfColor& bpfColor::operator-=(const bpfColor& aColor)
{
  mRGB[0] -= aColor.mRGB[0];
  mRGB[1] -= aColor.mRGB[1];
  mRGB[2] -= aColor.mRGB[2];

  return *this;
}


/**
 * Multiplication with a bpfFloat. (Underflow/overflow is not checked).
 *
 * @param aFactor The multiplication factor.
 * @return *this
 */
inline bpfColor& bpfColor::operator*=(bpfFloat aFactor)
{
  mRGB[0] *= aFactor;
  mRGB[1] *= aFactor;
  mRGB[2] *= aFactor;

  return *this;
}


/**
 * Equality. Two colors are equal if all of their components are equal.
 *
 * @param aColor The color to compare to.
 */
inline bool  bpfColor::operator==(const bpfColor& aColor) const
{
  return
    mRGB[0] == aColor.mRGB[0] &&
    mRGB[1] == aColor.mRGB[1] &&
    mRGB[2] == aColor.mRGB[2];
}

/**
 * InEquality. Two colors are inequal if one of their components are inequal.
 *
 * @param aColor The color to compare to.
 */
inline bool bpfColor::operator!=(const bpfColor& aColor) const
{
  return
    mRGB[0] != aColor.mRGB[0] ||
    mRGB[1] != aColor.mRGB[1] ||
    mRGB[2] != aColor.mRGB[2];
}

/**
 * Greater operator. A color is greater than another color if its maximal
 * component is greater than the maximal component of the other color. The
 * components must not be the same, i.e. (1.0,0,0) > (0,0.7,0)
 *
 * @param aColor The color to compare to.
 */
inline bool bpfColor::operator<(const bpfColor& aColor) const
{
  return GetMaxRGB() < aColor.GetMaxRGB();
}


/**
 * Clamp all components to the range [0..1].
 */
inline void bpfColor::Clamp()
{
  // red
  if (mRGB[0] > 1.0) {
    mRGB[0] = 1.0;
  }
  else if (mRGB[0] < 0.0) {
    mRGB[0] = 0.0;
  }

  // green
  if (mRGB[1] > 1.0) {
    mRGB[1] = 1.0;
  }
  else if (mRGB[1] < 0.0) {
    mRGB[1] = 0.0;
  }

  // blue
  if (mRGB[2] > 1.0) {
    mRGB[2] = 1.0;
  }
  else if (mRGB[2] < 0.0) {
    mRGB[2] = 0.0;
  }
}


/**
 * Addition of two colors. No range checking is done.
 *
 * @param aColor1 The first color.
 * @param aColor2 The second color.
 * @return The sum aColor1 + aColor2 of the two colors (by components)
 */
inline bpfColor operator+(const bpfColor& aColor1, const bpfColor& aColor2)
{
  return bpfColor(aColor1.Red()   + aColor2.Red(),
                  aColor1.Green() + aColor2.Green(),
                  aColor1.Blue()  + aColor2.Blue());
}


/**
 * Subtraction of two colors. No range checking is done.
 *
 * @param aColor1 The color from which the second color is subtracted.
 * @param aColor2 The color that is subtracted from the first color.
 * @return The color aColor1 - aColor2 (by components)
 */
inline bpfColor operator-(const bpfColor& aColor1, const bpfColor& aColor2)
{
  return bpfColor(aColor1.Red()   - aColor2.Red(),
                  aColor1.Green() - aColor2.Green(),
                  aColor1.Blue()  - aColor2.Blue());
}


// Functions for the use as argument to an image field. This requires
// that a multiplication by a bpfFloat value is supported by the element (for
// interpolation).

/**
 * Multiplication with a bpfFloat value (over/underflow is not checked)
 *
 * @param aColor The color.
 * @param aFactor The multiplication factor.
 * @return The multiplied color (each component is multiplied).
 */
inline bpfColor operator*(const bpfColor& aColor, bpfFloat aFactor)
{
  return bpfColor(aColor.Red()   * aFactor,
                  aColor.Green() * aFactor,
                  aColor.Blue()  * aFactor);
}


/**
 * Multiplication with a bpfFloat value (over/underflow is not checked)
 *
 * @param aFactor The multiplication factor.
 * @param aColor The color.
 * @return The multiplied color (each component is multiplied).
 */
inline bpfColor operator*(bpfFloat aFactor, const bpfColor& aColor)
{
  return bpfColor(aColor.Red()   * aFactor,
                  aColor.Green() * aFactor,
                  aColor.Blue()  * aFactor);
}


/**
 * Convert a string to a bpfColor
 *
 * @param aColorString The string from which to get the value
 * @param aColor The value (OUT)
 */
inline void bpfFromString(const bpfString& aColorString, bpfColor& aColor)
{
  bpfFloat vRed = 0.0f;
  bpfFloat vGreen = 0.0f;
  bpfFloat vBlue = 0.0f;
  sscanf(aColorString.c_str(),"%f%f%f", &vRed, &vGreen, &vBlue);
  aColor.SetRGB(vRed, vGreen, vBlue);
}


/**
 * Convert a color to a bpfString.
 *
 * @param aColor The color to convert.
 * @return The string (OUT)
 */
inline bpfString bpfToString(const bpfColor& aColor, bpfInt16 aFixedPrecision)
{
  return bpfToString(aColor.Red(), aFixedPrecision) + " " +
         bpfToString(aColor.Green(), aFixedPrecision) + " " +
         bpfToString(aColor.Blue(), aFixedPrecision);
}


/**
 * Round a numeric input value to another numeric output value
 *
 * @param aColorIn  The value to be rounded.
 * @param aColorOut The result of the rounding.
 */
inline void bpfRound(bpfColor aColorIn, bpfColor& aColorOut)
{
  aColorOut = aColorIn;
}


#endif // __BP_COLOR_H__

