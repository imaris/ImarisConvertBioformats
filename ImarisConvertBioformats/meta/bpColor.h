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


#ifndef __BPCOLOR_H__
#define __BPCOLOR_H__


#include "bpUtils.h"

/**
 * Class that describes a stored color. A color is internally
 * represented by 3 floats (it does NOT contain an alpha value).
 */
class bpColor
{

public:

  enum tPredefinedColor {
    eRed,
    eGreen,
    eBlue,
    eWhite,
    eOrange,
    ePinkyRed,
    ePurple,
    eLightBlue,
    eLightGreen
  };

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
  inline bpColor();

  /**
   * Constructor for a greyvalue color.
   *
   * @param aGrayValue The greyvalue of the color as a float in the range [0..1].
   */
  inline explicit bpColor(bpFloat aGrayValue);

  /**
   * Constructor that sets the color in RGB space.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline bpColor(bpFloat aRed, bpFloat aGreen, bpFloat aBlue);

  /**
   * Constructor that sets the color using an array.
   *
   * @param aRGB   An array containing the red, green and blue component of the
   *               color.
   */
  inline bpColor(const bpFloat aRGB[3]);

  /**
   * Set the rgb components of bpcolor.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline void SetRGB(bpFloat aRed, bpFloat aGreen, bpFloat aBlue);
  inline void SetRGB(const bpString& aRRGGBB);

  /**
   * Get the rgb components of bpcolor.
   *
   * @param aRed   The red component of the color.
   * @param aGreen The green component of the color.
   * @param aBlue  The blue component of the color.
   */
  inline void GetRGB(bpFloat& aRed, bpFloat& aGreen, bpFloat& aBlue) const;
  inline void GetRGB(bpString& aRRGGBB) const;
  inline const bpFloat* GetColor3fv() const;

  inline bpFloat Red() const;
  inline bpFloat Green() const;
  inline bpFloat Blue() const;

  /**
   * Set the hsv components of bpcolor.
   *
   * @param aHue        The hue of the color.
   * @param aSaturation The saturation of the color.
   * @param aValue      The brighness value of the color.
   */
  inline void SetHSV(bpFloat aHue, bpFloat aSaturation, bpFloat aValue);

  /**
   * Get the hsv components of bpcolor.
   *
   * @param aHue        The hue of the color.
   * @param aSaturation The saturation of the color.
   * @param aValue      The brighness value of the color.
   */
  inline void GetHSV(bpFloat& aHue, bpFloat& aSaturation, bpFloat& aValue) const;

  /**
   * Set the color to a grayvalue.
   *
   * @param aGrayValue   The gray value to set.
   */
  inline void SetGrayValue(bpFloat aGrayValue);


  /**
   * Set the color to one of the predefined colors.
   *
   * @param aPredefinedColor The predefined color code.
   */
  inline void SetToPredefinedColor(tPredefinedColor aPredefinedColor);

  /**
   * Get the maximal RGB component.
   *
   * @return The value of the maximal component.
   */
  inline bpFloat GetMaxRGB() const;

  /**
   * Get the minimal RGB component.
   *
   * @return The value of the minimal component.
   */
  inline bpFloat GetMinRGB() const;

  /**
   * Get the luminance of the color.
   *
   * @return The luminance.
   */
  inline bpFloat GetLuminance() const;

  /**
   * Returns a black or white color depending on the luminance
   */
  bpColor GetContrastColor() const;

  /**
   * Get perceptual color distance.
   */
  bpFloat GetPerceptualColorDistance(const bpColor& aOtherColor) const;

  /**
   * Returns a similar random color
   *
   * @param aVariance similarity of the resulting color (0 => equal; 1 => totally different)
   * @return
   */
  bpColor GetRandomColor(bpFloat aVariance = 1.0f) const;

  /**
   * Returns a random color whose r g b values have been randomly chosen uniformly in [0,1).
   */
  static bpColor RandomColor();

  /**
   * Convert from HSV to RGB color space.
   */
  static void HSVtoRGB(bpFloat aHue, bpFloat aSaturation, bpFloat aValue,
                       bpFloat& aRed, bpFloat& aGreen, bpFloat& aBlue);

  /**
   * Convert from RGB to HSV color space.
   */
  static void RGBtoHSV(bpFloat aRed, bpFloat aGreen, bpFloat aBlue,
                       bpFloat& aHue, bpFloat& aSaturation, bpFloat& aValue);

  /**
   * Convert from RGB to CIE L*CH° color space.
   */
  static void RGBtoLCH(bpDouble aRed, bpDouble aGreen, bpDouble aBlue,
                       bpDouble& aLightness, bpDouble& aChroma, bpDouble& aHue);

  /**
   * Convert from RGB to CIE L*ab color space.
   */
  static void RGBtoLAB(bpDouble aRed, bpDouble aGreen, bpDouble aBlue,
                       bpDouble& aLightness, bpDouble& aColorComponentA, bpDouble& aColorComponentB);

  /**
   * Convert from RGB to CIE XYZ color space.
   */
  static void RGBtoXYZ(bpDouble aRed, bpDouble aGreen, bpDouble aBlue,
                       bpDouble& aX, bpDouble& aY, bpDouble& aZ);

  /**
   * Convert from CIE XYZ to CIE L*ab color space.
   */
  static void XYZtoLAB(bpDouble aX, bpDouble aY, bpDouble aZ,
                       bpDouble& aLightness, bpDouble& aColorComponentA, bpDouble& aColorComponentB);

  /**
   * Operations.
   */

  /**
   * Addition of two colors. The range is not checked.
   *
   * @param aColor The color to be added.
   * @return *this.
   */
  inline bpColor& operator+=(const bpColor& aColor);

  /**
   * Subtraction of two color. The range is not checked.
   *
   * @param aColor The color to be subtracted.
   * @return *this
   */
  inline bpColor& operator-=(const bpColor& aColor);

  /**
   * Multiplication with a bpFloat. (Underflow/overflow is not checked).
   *
   * @param aFactor The multiplication fraction.
   * @return *this
   */
  inline bpColor& operator*=(bpFloat aFactor);


  /**
   * Division by a bpFloat. (Underflow/overflow is not checked).
   *
   * @param aFraction The division fraction.
   * @return *this
   */
  inline bpColor& operator/=(bpFloat aFraction);


  /**
   * Multiplication component-wise with a bpColor. (Underflow/overflow is not
   * checked).
   *
   * @param aColor The multiplication color.
   * @return *this
   */
  inline bpColor& operator*=(const bpColor& aColor);


  /**
   * Division component-wise by a bpColor. (Underflow/overflow is not checked).
   *
   * @param aColor The division color.
   * @return *this
   */
  inline bpColor& operator/=(const bpColor& aColor);


  /**
   * Equality. Two colors are equal if all of their components are equal.
   *
   * @param aColor The color to compare to.
   */
  inline bool operator==(const bpColor& aColor) const;

  /**
   * InEquality. Two colors are inequal if one of their components are inequal.
   *
   * @param aColor The color to compare to.
   */
  inline bool operator!=(const bpColor& aColor) const;

  /**
   * Clamp all components to the range [0..1].
   */
  inline void Clamp();

  inline bool IsWhite() const;
  inline bool IsBlack() const;

private:
  static bpFloat ConvertHexToFloat(const bpString& aHex);
  static bpString ConvertFloatToHex(bpFloat aFloat);

  /**
   * The color is locally stored as RGB array.
   */
  bpFloat mRGB[3];
};

/**
 * Addition of two colors. No range checking is done.
 *
 * @param aColor1 The first color.
 * @param aColor2 The second color.
 * @return The sum aColor1 + aColor2 of the two colors (by components)
 */
inline bpColor operator+(const bpColor& aColor1, const bpColor& aColor2);

/**
 * Subtraction of two colors. No range checking is done.
 *
 * @param aColor1 The color from which the second color is subtracted.
 * @param aColor2 The color that is subtracted from the first color.
 * @return The color aColor1 - aColor2 (by components)
 */
inline bpColor operator-(const bpColor& aColor1, const bpColor& aColor2);

// Functions for the use as argument to an image field. This requires
// that a multiplication by a bpFloat value is supported by the element (for
// interpolation).

/**
 * Multiplication with a bpFloat value (over/underflow is not checked)
 *
 * @param aColor The color.
 * @param aFactor The multiplication factor.
 * @return The multiplied color (each component is multiplied).
 */
inline bpColor operator*(const bpColor& aColor, bpFloat aFactor);

/**
 * Multiplication with a bpFloat value (over/underflow is not checked)
 *
 * @param aFactor The multiplication factor.
 * @param aColor The color.
 * @return The multiplied color (each component is multiplied).
 */
inline bpColor operator*(bpFloat aFactor, const bpColor& aColor);

/**
 * Convert a string to a bpColor
 *
 * @param aColorString The string from which to get the value
 * @param aColor The value (OUT)
 * @param aDefaultColor The color value that should be returned
 *                      when the string can not be parsed.
 */
inline void bpFromString(const bpString& aColorString, bpColor& aColor, const bpColor& aDefaultColor = bpColor(0.0f, 0.0f, 0.0f));

/**
 * Convert a color to a bpString.
 *
 * @param aColor The color to convert.
 * @return The string (OUT)
 */
inline bpString bpToString(const bpColor& aColor, bpInt16 aFixedPrecision = 8);

/**
 * Round a numeric input value to another numeric output value
 *
 * @param aColorIn  The value to be rounded.
 * @param aColorOut The result of the rounding.
 */
inline void bpRound(bpColor aColorIn, bpColor& aColorOut);


// inline function definitions
//
/**
 * Default constructor. The color is set to white.
 */
inline bpColor::bpColor()
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
inline bpColor::bpColor(bpFloat aGrayValue)
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
inline bpColor::bpColor(bpFloat aRed, bpFloat aGreen, bpFloat aBlue)
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
inline bpColor::bpColor(const bpFloat aRGB[3])
{
  mRGB[0] = aRGB[0];
  mRGB[1] = aRGB[1];
  mRGB[2] = aRGB[2];
}


/**
 * Set the rgb components of bpcolor.
 *
 * @param aRed   The red component of the color.
 * @param aGreen The green component of the color.
 * @param aBlue  The blue component of the color.
 */
inline void bpColor::SetRGB(bpFloat aRed, bpFloat aGreen, bpFloat aBlue)
{
  mRGB[0] = aRed;
  mRGB[1] = aGreen;
  mRGB[2] = aBlue;
}


inline void bpColor::SetRGB(const bpString& aRRGGBB)
{
  if ((0 == aRRGGBB.find('#')) && (7 == aRRGGBB.size())) {
    //converts eg #126A3F

    auto vRedText = aRRGGBB.substr(1, 2);
    auto vGreenText = aRRGGBB.substr(3, 2);
    auto vBlueText = aRRGGBB.substr(5, 2);
    mRGB[0] = ConvertHexToFloat(vRedText);
    mRGB[1] = ConvertHexToFloat(vGreenText);
    mRGB[2] = ConvertHexToFloat(vBlueText);
  }
}


/**
 * Get the rgb components of bpcolor.
 *
 * @param aRed   The red component of the color.
 * @param aGreen The green component of the color.
 * @param aBlue  The blue component of the color.
 */
inline void bpColor::GetRGB(bpFloat& aRed, bpFloat& aGreen, bpFloat& aBlue) const
{
  aRed   = mRGB[0];
  aGreen = mRGB[1];
  aBlue  = mRGB[2];
}


inline void bpColor::GetRGB(bpString& aRRGGBB) const
{
  aRRGGBB = '#' + ConvertFloatToHex(mRGB[0]) + ConvertFloatToHex(mRGB[1]) + ConvertFloatToHex(mRGB[2]);
}


inline const bpFloat* bpColor::GetColor3fv() const
{
  return mRGB;
}


inline bpFloat bpColor::Red() const
{
  return mRGB[0];
}


inline bpFloat bpColor::Green() const
{
  return mRGB[1];
}


inline bpFloat bpColor::Blue() const
{
  return mRGB[2];
}


/**
 * Set the hsv components of bpcolor.
 *
 * @param aHue        The hue of the color.
 * @param aSaturation The saturation of the color.
 * @param aValue      The brighness value of the color.
 */
inline void bpColor::SetHSV(bpFloat aHue, bpFloat aSaturation, bpFloat aValue)
{
  HSVtoRGB(aHue, aSaturation, aValue,
           mRGB[0], mRGB[1], mRGB[2]);
}


/**
 * Get the hsv components of bpcolor.
 *
 * @param aHue        The hue of the color.
 * @param aSaturation The saturation of the color.
 * @param aValue      The brighness value of the color.
 */
inline void bpColor::GetHSV(bpFloat& aHue, bpFloat& aSaturation, bpFloat& aValue) const
{
  RGBtoHSV(mRGB[0], mRGB[1], mRGB[2],
           aHue, aSaturation, aValue);
}


/**
 * Set the color to a grayvalue.
 *
 * @param aGrayValue   The gray value to set.
 */
inline void bpColor::SetGrayValue(bpFloat aGrayValue)
{
  mRGB[0] = aGrayValue;
  mRGB[1] = aGrayValue;
  mRGB[2] = aGrayValue;
}


/**
  * Set the color to one of the predefined colors.
  *
  * @param aPredefinedColor The predefined color code.
  */
void bpColor::SetToPredefinedColor(bpColor::tPredefinedColor aPredefinedColor)
{
  switch (aPredefinedColor) {
    case eRed:
      SetRGB(1.0f, 0.0f, 0.0f);
      break;
    case eGreen:
      SetRGB(0.0f, 1.0f, 0.0f);
      break;
    case eBlue:
      SetRGB(0.0f, 0.0f, 1.0f);
      break;
    case eWhite:
      SetRGB(1.0f, 1.0f, 1.0f);
      break;
    case eOrange:
      SetRGB(1.0f, 0.6f, 0.0f);
      break;
    case ePinkyRed:
      SetRGB(1.0f, 0.0f, 0.4f);
      break;
    case ePurple:
      SetRGB(0.6f, 0.0f, 1.0f);
      break;
    case eLightBlue:
      SetRGB(0.0f, 0.6f, 1.0f);
      break;
    case eLightGreen:
      SetRGB(0.6f, 1.0f, 0.0f);
      break;
  }
}


/**
 * Get the maximal RGB component.
 *
 * @return The value of the maximal component.
 */
inline bpFloat bpColor::GetMaxRGB() const
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
inline bpFloat bpColor::GetMinRGB() const
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
bpFloat bpColor::GetLuminance() const
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
inline bpColor& bpColor::operator+=(const bpColor& aColor)
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
inline bpColor& bpColor::operator-=(const bpColor& aColor)
{
  mRGB[0] -= aColor.mRGB[0];
  mRGB[1] -= aColor.mRGB[1];
  mRGB[2] -= aColor.mRGB[2];

  return *this;
}


/**
 * Multiplication with a bpFloat. (Underflow/overflow is not checked).
 *
 * @param aFactor The multiplication factor.
 * @return *this
 */
inline bpColor& bpColor::operator*=(bpFloat aFactor)
{
  mRGB[0] *= aFactor;
  mRGB[1] *= aFactor;
  mRGB[2] *= aFactor;

  return *this;
}


/**
 * Division by a bpFloat. (Underflow/overflow is not checked).
 *
 * @param aFactor The division fraction.
 * @return *this
 */
inline bpColor& bpColor::operator/=(bpFloat aFraction)
{
  mRGB[0] /= aFraction;
  mRGB[1] /= aFraction;
  mRGB[2] /= aFraction;

  return *this;
}


/**
 * Multiplication component-wise with a bpColor. (Underflow/overflow is not
 * checked).
 *
 * @param aColor The multiplication color.
 * @return *this
 */
inline bpColor& bpColor::operator*=(const bpColor& aColor)
{
  mRGB[0] *= aColor.Red();
  mRGB[1] *= aColor.Green();
  mRGB[2] *= aColor.Blue();

  return *this;
}


/**
 * Division component-wise by a bpColor. (Underflow/overflow is not checked).
 *
 * @param aColor The division color.
 * @return *this
 */
inline bpColor& bpColor::operator/=(const bpColor& aColor)
{
  mRGB[0] /= aColor.Red();
  mRGB[1] /= aColor.Green();
  mRGB[2] /= aColor.Blue();

  return *this;
}


/**
 * Equality. Two colors are equal if all of their components are equal.
 *
 * @param aColor The color to compare to.
 */
inline bool  bpColor::operator==(const bpColor& aColor) const
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
inline bool bpColor::operator!=(const bpColor& aColor) const
{
  return
    mRGB[0] != aColor.mRGB[0] ||
    mRGB[1] != aColor.mRGB[1] ||
    mRGB[2] != aColor.mRGB[2];
}

inline bool bpColor::IsWhite() const
{
  return ((mRGB[0] == 1) && (mRGB[1] == 1) && (mRGB[2] == 1));
}

inline bool bpColor::IsBlack() const
{
  return ((mRGB[0] == 0) && (mRGB[1] == 0) && (mRGB[2] == 0));
}


/**
 * Clamp all components to the range [0..1].
 */
inline void bpColor::Clamp()
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
inline bpColor operator+(const bpColor& aColor1, const bpColor& aColor2)
{
  return bpColor(aColor1.Red()   + aColor2.Red(),
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
inline bpColor operator-(const bpColor& aColor1, const bpColor& aColor2)
{
  return bpColor(aColor1.Red()   - aColor2.Red(),
                 aColor1.Green() - aColor2.Green(),
                 aColor1.Blue()  - aColor2.Blue());
}


// Functions for the use as argument to an image field. This requires
// that a multiplication by a bpFloat value is supported by the element (for
// interpolation).

/**
 * Multiplication with a bpFloat value (over/underflow is not checked)
 *
 * @param aColor The color.
 * @param aFactor The multiplication factor.
 * @return The multiplied color (each component is multiplied).
 */
inline bpColor operator*(const bpColor& aColor, bpFloat aFactor)
{
  return bpColor(aColor.Red()   * aFactor,
                 aColor.Green() * aFactor,
                 aColor.Blue()  * aFactor);
}


/**
 * Multiplication with a bpFloat value (over/underflow is not checked)
 *
 * @param aFactor The multiplication factor.
 * @param aColor The color.
 * @return The multiplied color (each component is multiplied).
 */
inline bpColor operator*(bpFloat aFactor, const bpColor& aColor)
{
  return bpColor(aColor.Red()   * aFactor,
                 aColor.Green() * aFactor,
                 aColor.Blue()  * aFactor);
}


/**
 * Convert a string to a bpColor. If it cannot parse the string,
 * the resulting color will be 0,0,0.
 *
 * @param aColorString The string from which to get the value
 * @param aColor The value (OUT)
 */
inline void bpFromString(const bpString& aColorString, bpColor& aColor, const bpColor& aDefaultColor)
{
  auto vRed = aDefaultColor.Red();
  auto vGreen = aDefaultColor.Green();
  auto vBlue = aDefaultColor.Blue();

  if ((0 == aColorString.find('#')) && (7 == aColorString.size())) {
    //converts eg #126A3F

    aColor.SetRGB(aColorString);
  }
  else {
    //converts eg "0.3 0.6 1.0"

    sscanf(aColorString.c_str(), "%f%f%f", &vRed, &vGreen, &vBlue);

    aColor.SetRGB(vRed, vGreen, vBlue);
  }
}


/**
 * Convert a color to a bpString.
 *
 * @param aColor The color to convert.
 * @return The string (OUT)
 */
inline bpString bpToString(const bpColor& aColor, bpInt16 aFixedPrecision)
{
  return bpToString(aColor.Red(), aFixedPrecision) + " " +
         bpToString(aColor.Green(), aFixedPrecision) + " " +
         bpToString(aColor.Blue(), aFixedPrecision);
}


/**
 * Round a numeric input value to another numeric output value
 *
 * @param aColorIn  The value to be rounded.
 * @param aColorOut The result of the rounding.
 */
inline void bpRound(bpColor aColorIn, bpColor& aColorOut)
{
  aColorOut = aColorIn;
}

#endif // __BPCOLOR_H__

