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


#ifndef __BP_PACKED_RGBA__
#define __BP_PACKED_RGBA__

#include "fileiobase/types/bpfColor.h"

/**
 *
 * <p>Class for a packed rgba representation. The sizes of the class MUST be
 * 32 bit. The class bpfPackedRGBA stores the colors in the sequence red, green,
 * blue and alpha. The alpha value is the opacity of the color, where full
 * alpha (255) indicates full opacity and 0 indicates a transparent value.
 *
 * <p>The classes define the operators +, = and == , as well as multiplication by
 * a float value, so they can be used in a bpfImageField. There is also a copy
 * constructor and assignment from bpfColor.
 *
 * <p>All operations now check the range of the value.
 *
 */
class bpfPackedRGBA
{
public:
  bpfUInt8 r;
  bpfUInt8 g;
  bpfUInt8 b;
  bpfUInt8 a;

  /**
   * Constructor for default color (0,0,0,255).
   */
  inline bpfPackedRGBA();

  /**
    * Constructor for default color (X,X,X,255).
    */
  inline bpfPackedRGBA(bpfUInt8 rgb);

  /**
   * Constructor with supplied color arguments.
   *
   * @param r The red component.
   * @param g The green component.
   * @param b The blue component.
   * @param a The alpha component. The default value is 255.
   */
  inline  bpfPackedRGBA(bpfUInt8 r, bpfUInt8 g, bpfUInt8 b,  bpfUInt8 a = 255);

  /**
   * Copy constructor.
   *
   * @param other Color from which the state is copied.
   */
  inline bpfPackedRGBA(const bpfPackedRGBA& other);

  /**
   * Constructor from bpfColor.
   *
   * @param color Color from which the color info is converted and copied.
   */
  inline bpfPackedRGBA(const bpfColor& color);

  /**
   * Assignment operator.
   *
   * @param other Color from which the state is copied.
   * @return self
   */
  inline bpfPackedRGBA& operator=(const bpfPackedRGBA& other);

  /**
   * Assignment operator from bpfColor.
   *
   * @param color Color from which the color info is converted and copied.
   * @return self
   */
  inline bpfPackedRGBA& operator=(const bpfColor& color);

  /**
   * Assignment operator from value.
   *
   * @param aRGB the value from which a color (X,X,X,255) is built
   * @return self
   */
  inline bpfPackedRGBA& operator=(const bpfUInt8& aRGB);

  /**
   * Equality operator. Two colors are equal if all components are equal.
   *
   * @param other The other color.
   * @return 1 if all components are equal, 0 otherwise.
   */
  inline int operator==(const bpfPackedRGBA& other);

  /**
   * Equality operator. A color is equal to a value if all its
   * component are equal to it.
   *
   * @param aOtherRGB The other color.
   * @return 1 if all components are equal, 0 otherwise.
   */
  inline int operator==(const bpfUInt8& aOtherRGB);

  /**
   * Addition. Overflown is checked.
   *
   * @param arg The argument.
   */
  inline bpfPackedRGBA& operator+=(const bpfPackedRGBA& arg);

  /**
   * Subtraction. Underflow is checked.
   *
   * @param arg The argument.
   */
  inline bpfPackedRGBA& operator-=(const bpfPackedRGBA& arg);

  /**
   * Multiplication by a float value. Components are truncated.
   *
   * @param factor Multiplication factor.
   */
  inline bpfPackedRGBA& operator*=(bpfFloat factor);

  /**
   * Comparison of color. A color is "greater" than another color if its
   * maximum color is greater.
   *
   * @param other The color to compare to
   * @return true if this color is greater
   */
  inline bool operator<(const bpfPackedRGBA& other);
  inline bool operator>(const bpfPackedRGBA& other);

  /**
   * Get the maximum of the 3 colors.
   *
   * @return The maximal value of the rgb color.
   */
  inline bpfUInt8 GetMaxRGB() const;

  /**
   * Get the luminance of the color as 8bit values.
   *
   * @return The luminance.
   */
  inline bpfUInt8 GetLuminance() const;

  /**
   * Get the luminance of the color as float value.
   */
  inline operator bpfFloat() const;
};

/**
 * Addition of two packed colors. Components are truncated!
 *
 * @param arg1 The first summand.
 * @param arg2 The second summand.
 * @return bpfPackedColorRGBA(arg1.r + arg2.r, ...)
 */
inline bpfPackedRGBA operator+(const bpfPackedRGBA& arg1, const bpfPackedRGBA& arg2);

/**
 * Multiplication ba a float value. Components are truncated.
 *
 * @param arg The color.
 * @param factor The multiplication factor.
 * @return bpfPackedRGBA(factor * arg.r, ...);
 */
inline bpfPackedRGBA operator*(const bpfPackedRGBA& arg, float factor);

/**
 * Multiplication ba a float value. Components are truncated.
 *
 * @param arg The color.
 * @param factor The multiplication factor.
 * @return bpfPackedRGBA(factor * arg.r, ...);
 */
inline bpfPackedRGBA operator*(float factor, const bpfPackedRGBA& arg);

/**
 * Convert a type to a string. This must be defined for all types used as
 * template argument in bpfImageFields.
 *
 * @param value The value to be converted to a string.
 * @return The converted string.
 */
bpfString bpfToString(const bpfPackedRGBA& value);

/**
 * Round a numeric input value to another numeric output value
 *
 * @param input The value to be rounded.
 * @param output The result of the rounding.
 */
inline void bpfRound(bpfPackedRGBA input, bpfPackedRGBA& output);

//
// inline function definitions
//

/**
 * Constructor for default color (0,0,0,255).
  */
inline
bpfPackedRGBA::bpfPackedRGBA()
  :r(0),g(0),b(0),a(255)
{}

inline
bpfPackedRGBA::bpfPackedRGBA(bpfUInt8 rgb)
  :r(rgb),g(rgb),b(rgb),a(255)
{}

/**
 * Constructor with supplied color arguments.
 *
 * @param r The red component.
 * @param g The green component.
 * @param b The blue component.
 * @param a The alpha component. The default value is 0.
 */
inline
bpfPackedRGBA::bpfPackedRGBA(bpfUInt8 red, bpfUInt8 green, bpfUInt8 blue,  bpfUInt8 alpha)
  :r(red),g(green),b(blue),a(alpha)
{}

/**
 * Constructor from bpfColor.
 *
 * @param color Color from which the color info is converted and copied.
 */
inline
bpfPackedRGBA::bpfPackedRGBA(const bpfColor& color)
  :r((bpfUInt8) (color.Red()*255)),
   g((bpfUInt8) (color.Green()*255)),
   b((bpfUInt8) (color.Blue()*255)),
   a(255)
{
#ifdef BP_DEBUG
  if (color.Red() > 1.0 || color.Green() > 1.0 || color.Blue() > 1.0) {
    BP_DEBUG_MSG("Initializing of bpfPackedRGBA with illegal color");
  }
#endif
}

/**
 * Copy constructor.
 *
 * @param other Color from which the state is copied.
 */
inline bpfPackedRGBA:: bpfPackedRGBA(const bpfPackedRGBA& other)
  :r(other.r),g(other.g),b(other.b),a(other.a)
{}

/**
 * Assignment operator.
 *
 * @param other Color from which the state is copied.
 * @return self
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator=(const bpfPackedRGBA& other)
{
  if (&other == this) {
    // assignment to self
    return *this;
  }
  r = other.r;
  g = other.g;
  b = other.b;
  a = other.a;

  return *this;
}

/**
 * Assignment operator from bpfColor.
 *
 * @param color Color from which the color info is converted and copied.
 * @return self
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator=(const bpfColor& color)
{
#ifdef BP_DEBUG
  if (color.Red() > 1.0 || color.Green() > 1.0 || color.Blue() > 1.0) {
    BP_DEBUG_MSG("Assignment of bpfPackedRGBA with illegal color");
  }
#endif
  r = (bpfUInt8) (color.Red()*255);
  g = (bpfUInt8) (color.Green()*255);
  b = (bpfUInt8) (color.Blue()*255);
  a = 255;


  return *this;
}

/**
 * Assignment operator from value.
 *
 * @param aRGB the value from which a color (X,X,X,255) is built
 * @return self
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator=(const bpfUInt8& aRGB)
{
  r = aRGB;
  g = aRGB;
  b = aRGB;
  a = 255;

  return *this;
}

/**
 * Equality operator. Two colors are equal if all components are equal.
 *
 * @param other The other color.
 * @return 1 if all components are equal, 0 otherwise.
 */
inline int
bpfPackedRGBA::operator==(const bpfPackedRGBA& other)
{
  return(other.r == r) && (other.g == g) && (other.b == b) && (other.a == a);
}

/**
 * Equality operator. A color is equal to a value if all its
 * component are equal to it.
 *
 * @param aOtherRGB The other color.
 * @return 1 if all components are equal, 0 otherwise.
 */
inline int
bpfPackedRGBA::operator==(const bpfUInt8& aOtherRGB)
{
  return (r == aOtherRGB && g == aOtherRGB && b == aOtherRGB && a == 255);
}

/**
 * Addition. Overflown is checked.
 *
 * @param arg The argument.
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator+=(const bpfPackedRGBA& arg)
{
  int result;

  // red
  result = r + arg.r;
  if (result > 255) r = 255;
  else              r = static_cast<bpfUInt8>(result);

  //green
  result = g + arg.g;
  if (result > 255) g = 255;
  else              g = static_cast<bpfUInt8>(result);

  //blue
  result = b + arg.b;
  if (result > 255) b = 255;
  else              b = static_cast<bpfUInt8>(result);

  // alpha
  result = a + arg.a;
  if (result > 255) a = 255;
  else              a = static_cast<bpfUInt8>(result);

  return *this;
}

/**
 * Subtraction. Underflow is checked.
 *
 * @param arg The argument.
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator-=(const bpfPackedRGBA& arg)
{
  // red
  if (r > arg.r) r -= arg.r;
  else           r = 0;

  // green
  if (g > arg.g) g -= arg.g;
  else           g = 0;

  // blue
  if (b > arg.b) b -= arg.b;
  else           b = 0;

  if (a > arg.a) a -= arg.a;
  else           a = 0;

  return *this;
}

/**
 * Addition of two packed colors. Components are truncated!
 *
 * @param arg1 The first summand.
 * @param arg2 The second summand.
 * @return bpfPackedColorRGBA(arg1.r + arg2.r, ...)
 */
inline bpfPackedRGBA operator+(const bpfPackedRGBA& arg1,
                               const bpfPackedRGBA& arg2)
{
  int red = arg1.r + arg2.r;
  if (red > 255) red = 255;

  int green = arg1.g + arg2.g;
  if (green > 255) green = 255;

  int blue = arg1.b + arg2.b;
  if (blue > 255) blue = 255;

  int alpha = arg1.a + arg2.a;
  if (alpha > 255) alpha = 255;

  return bpfPackedRGBA(static_cast<bpfUInt8>(red),
                      static_cast<bpfUInt8>(green),
                      static_cast<bpfUInt8>(blue),
                      static_cast<bpfUInt8>(alpha));
}

/**
 * Subtraction
 */
inline bpfPackedRGBA operator-(const bpfPackedRGBA& aColor1, const bpfPackedRGBA& aColor2)
{
  bpfUInt8 vR = aColor1.r > aColor2.r ? aColor1.r - aColor2.r : 0;
  bpfUInt8 vG = aColor1.g > aColor2.g ? aColor1.g - aColor2.g : 0;
  bpfUInt8 vB = aColor1.b > aColor2.b ? aColor1.b - aColor2.b : 0;
  bpfUInt8 vA = aColor1.a > aColor2.a ? aColor1.a - aColor2.a : 0;
  return bpfPackedRGBA(vR, vG, vB, vA);
}

inline bpfPackedRGBA operator-(const bpfPackedRGBA& aColor1, bpfFloat aValue)
{
  bpfUInt8 vValue = aValue > 0.0f ? (bpfUInt8)(aValue + 0.5f) : 0;
  bpfUInt8 vR = aColor1.r > vValue ? aColor1.r - vValue : 0;
  bpfUInt8 vG = aColor1.g > vValue ? aColor1.g - vValue : 0;
  bpfUInt8 vB = aColor1.b > vValue ? aColor1.b - vValue : 0;
  bpfUInt8 vA = aColor1.a > vValue ? aColor1.a - vValue : 0;
  return bpfPackedRGBA(vR, vG, vB, vA);
}

/**
 * Division
 */
inline bpfFloat operator/(const bpfPackedRGBA& aColor1, const bpfPackedRGBA& aColor2)
{
  bpfFloat vLuminance1 = 0.299f*aColor1.r + 0.587f*aColor1.g + 0.114f*aColor1.b;
  bpfFloat vLuminance2 = 0.299f*aColor2.r + 0.587f*aColor2.g + 0.114f*aColor2.b;
  if (vLuminance2 != 0.0f) {
    return vLuminance1 / vLuminance2;
  }
  else {
    return 0.0f;
  }
}

/**
 * Get the luminance of the color as float value.
 */
inline bpfPackedRGBA::operator bpfFloat() const
{
  return 0.299f*r + 0.587f*g + 0.114f*b;
};

/**
 * Multiplication by a float value. Components are truncated.
 *
 * @param factor Multiplication factor.
 */
inline bpfPackedRGBA&
bpfPackedRGBA::operator*=(bpfFloat factor)
{
#ifdef BP_DEBUG
  if (factor < 0) {
    BP_DEBUG_MSG(bpfString("bpfPackedRGBA::operator*: Negative multiplication factor") +
                 bpfToString(factor));
    return *this;
  }
#endif

  int red = (int)(factor * r);
  if (red > 255) r = 255;
  else           r = static_cast<bpfUInt8>(red);

  int green = (int)(factor * g);
  if (green > 255) g = 255;
  else             g = static_cast<bpfUInt8>(green);

  int blue = (int)(factor * b);
  if (blue > 255) b = 255;
  else            b = static_cast<bpfUInt8>(blue);

  int alpha = (int)(factor * a);
  if (alpha > 255) a = 255;
  else             a = static_cast<bpfUInt8>(alpha);

  return *this;
}

/**
 * Multiplication by a float value. Components are truncated.
 *
 * @param arg The color.
 * @param factor The multiplication factor.
 * @return bpfPackedRGBA(factor * arg.r, ...);
 */
inline bpfPackedRGBA operator*(const bpfPackedRGBA& arg, float factor)
{
#ifdef BP_DEBUG
  if (factor < 0) {
    BP_DEBUG_MSG("bpfPackedRGBA::operator*: Negative multiplication factor");
    return bpfPackedRGBA();
  }
#endif
  int red = (int)(factor * arg.r);
  if (red > 255) red = 255;

  int green = (int)(factor * arg.g);
  if (green > 255) green = 255;

  int blue = (int)(factor * arg.b);
  if (blue > 255) blue = 255;

  int alpha = (int)(factor * arg.a);
  if (alpha > 255) alpha = 255;

  return bpfPackedRGBA(static_cast<bpfUInt8>(red),
                      static_cast<bpfUInt8>(green),
                      static_cast<bpfUInt8>(blue),
                      static_cast<bpfUInt8>(alpha));
}

/**
 * Multiplication ba a float value. Components are truncated.
 *
 * @param arg The color.
 * @param factor The multiplication factor.
 * @return bpfPackedRGBA(factor * arg.r, ...);
 */
inline bpfPackedRGBA operator*(float factor, const bpfPackedRGBA& arg)
{
  return arg*factor;
}

/**
 * Comparison of color. A color is "greater" than another color if its
 * maximum color is greater.
 *
 * @param other The color to compare to
 * @return true if this color is greater
 */
inline bool
bpfPackedRGBA::operator<(const bpfPackedRGBA& other)
{
  return GetLuminance() < other.GetLuminance();
}

inline bool
bpfPackedRGBA::operator>(const bpfPackedRGBA& other)
{
  return GetLuminance() > other.GetLuminance();
}


/**
 * Get the maximum of the 3 colors.
 *
 * @return The maximal value of the rgb color.
 */
inline bpfUInt8
bpfPackedRGBA::GetMaxRGB() const
{
  if (r > g) {
    if (r > b) {
      return r;
    }
    else {
      return b;
    }
  }
  else {
    if (g > b) {
      return g;
    }
    else {
      return b;
    }
  }
}



/**
 * Convert a type to a string. This must be defined for all types used as
 * template argument in bpfImageFields.
 *
 * @param value The value to be converted to a string.
 * @return The converted string.
 */
inline bpfString bpfToString(const bpfPackedRGBA& value)
{
  std::ostringstream vOut;
  vOut.width(3);
  vOut << "(" << (int)value.r << " " <<
       (int)value.g << " " << (int)value.b << " " << (int)value.a << " )";
  return vOut.str();
}

/**
 * Round a numeric input value to another numeric output value
 *
 * @param input The value to be rounded.
 * @param output The result of the rounding.
 */
inline void bpfRound(bpfPackedRGBA input, bpfPackedRGBA& output)
{
  output = input;
}

/**
 * Get the luminance of the color as 8bit values.
 *
 * @return The luminance.
 */
inline bpfUInt8
bpfPackedRGBA::GetLuminance() const
{
  return(bpfUInt8)(0.299f*r + 0.587f*g + 0.114f*b);
}

inline void bpfTypeRange(bpfPackedRGBA& aMin, bpfPackedRGBA& aMax)
{
  aMin.r = 0;
  aMin.g = 0;
  aMin.b = 0;
  aMin.a = 0;
  aMax.r = 255;
  aMax.g = 255;
  aMax.b = 255;
  aMax.a = 255;
}

/*inline bpfNumberType bpfGetNumberType(bpfPackedRGBA vValue)
{
  return bpfPackedRGBAType;
};*/

template <>
inline bpfNumberType bpfGetNumberType<bpfPackedRGBA>()
{
  return bpfPackedRGBAType;
}

#endif
