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


#include "bpColor.h"

#include <algorithm>
#include <cmath>
#include <iomanip>


/**
 * Returns a black or white color depending on the luminance
 */
bpColor bpColor::GetContrastColor() const
{
  if (GetLuminance() > 0.5f) {
    return bpColor(0.0f);
  }
  else {
    return bpColor(1.0f);
  }
}

bpColor bpColor::RandomColor()
{
  bpFloat vRed = (bpFloat)rand() / (bpFloat)(RAND_MAX+1.0f);
  bpFloat vGreen = (bpFloat)rand() / (bpFloat)(RAND_MAX+1.0f);
  bpFloat vBlue = (bpFloat)rand() / (bpFloat)(RAND_MAX+1.0f);
  return bpColor(vRed, vGreen, vBlue);
}

/**
 * Get perceptual color distance.
 */
bpFloat bpColor::GetPerceptualColorDistance(const bpColor& aOtherColor) const
{
  // Convert this and other color to CIE L*ab color space
  bpDouble vThisL, vThisA, vThisB, vOtherL, vOtherA, vOtherB;
  RGBtoLAB(mRGB[0], mRGB[1], mRGB[2], vThisL, vThisA, vThisB);
  RGBtoLAB(aOtherColor.Red(), aOtherColor.Green(), aOtherColor.Blue(), vOtherL, vOtherA, vOtherB);

  // Euclidian distance in L*ab space
  bpDouble vDiffL = vThisL - vOtherL;
  bpDouble vDiffA = vThisA - vOtherA;
  bpDouble vDiffB = vThisB - vOtherB;
  return static_cast<bpFloat>(std::sqrt(vDiffL*vDiffL + vDiffA*vDiffA + vDiffB*vDiffB));

}

bpColor bpColor::GetRandomColor(bpFloat aVariance) const
{
  bpFloat vHue = 0.0f;        // 0..360
  bpFloat vSaturation = 0.0f; // 0..1
  bpFloat vValue = 0.0f;      // 0..1
  GetHSV(vHue, vSaturation, vValue);

//  bpFloat vVarianceHue = vSaturation*aVariance - vSaturation + 1.0f;
  bpFloat vVarianceHue = aVariance;
  bpFloat vVarianceSaturation = aVariance;
  bpFloat vVarianceValue = aVariance;

  // scramble hue
  bpFloat vRand = (bpFloat)rand() / (bpFloat)RAND_MAX - 0.5f;
  vHue += 360.0f * vRand * vVarianceHue;
  if (vHue > 360.0f) {
    vHue -= 360.0f;
  }
  else if (vHue < 0.0f) {
    vHue += 360.0f;
  }

  // scramble saturation
  vRand = (bpFloat)rand() / (bpFloat)RAND_MAX - 0.5f;
  vSaturation += vRand * vVarianceValue;
  if (vSaturation > 1.0f) {
    vSaturation = 1.0f;
  }
  else if (vSaturation < 0.0f) {
    vSaturation = 0.0f;
  }

  // scramble value
  vRand = (bpFloat)rand() / (bpFloat)RAND_MAX - 0.5f;
  vValue += vRand * vVarianceSaturation;
  if (vValue > 1.0f) {
    vValue = 1.0f;
  }
  else if (vValue < 0.0f) {
    vValue = 0.0f;
  }

  // compose resultin color
  bpColor vRandomColor;
  vRandomColor.SetHSV(vHue, vSaturation, vValue);
  return vRandomColor;
}


/**
 * Convert from HSV to RGB color space.
 */
void bpColor::HSVtoRGB(bpFloat aHue, bpFloat aSaturation, bpFloat aValue,
                       bpFloat& aRed, bpFloat& aGreen, bpFloat& aBlue)
{
  if (aSaturation == 0.0f) {
    aRed = aValue;
    aGreen = aValue;
    aBlue = aValue;
  }
  else {
    if (aHue == 360.0f) {
      aHue = 0.0f;
    }
    aHue = aHue / 60.0f;
    bpInt32 i = (bpInt32)floor(aHue);
    bpFloat f = aHue - i;
    bpFloat p = aValue * (1.0f-aSaturation);
    bpFloat q = aValue * (1.0f-(aSaturation*f));
    bpFloat t = aValue * (1.0f-(aSaturation*(1.0f-f)));
    switch (i) {
    case 0:
      aRed = aValue;
      aGreen = t;
      aBlue = p;
      break;
    case 1:
      aRed = q;
      aGreen = aValue;
      aBlue = p;
      break;
    case 2:
      aRed = p;
      aGreen = aValue;
      aBlue = t;
      break;
    case 3:
      aRed = p;
      aGreen = q;
      aBlue = aValue;
      break;
    case 4:
      aRed = t;
      aGreen = p;
      aBlue = aValue;
      break;
    case 5:
      aRed = aValue;
      aGreen = p;
      aBlue = q;
      break;
    }
  }
}


/**
 * Convert from RGB to HSV color space.
 */
void bpColor::RGBtoHSV(bpFloat aRed, bpFloat aGreen, bpFloat aBlue,
                       bpFloat& aHue, bpFloat& aSaturation, bpFloat& aValue)
{
  bpFloat max1 = std::max(aRed, aBlue);
  max1 = std::max(max1, aGreen);

  bpFloat min1 = std::min(aRed, aBlue);
  min1 = std::min(min1, aGreen);

  aValue = max1;

  if (max1 != 0.0f) {
    aSaturation = (max1-min1)/max1;
  }
  else {
    aSaturation = 0.0f;
  }

  if (aSaturation == 0.0f) {
    // undefined
    aHue = 0.0f;
  }
  else {
    bpFloat delta = max1-min1;
    if (aRed == max1) {
      aHue = (aGreen-aBlue)/delta;
    }
    else if (aGreen == max1) {
      aHue = 2.0f + (aBlue-aRed)/delta;
    }
    else if (aBlue == max1) {
      aHue = 4.0f + (aRed-aGreen)/delta;
    }
    aHue *= 60.0f;
    if (aHue < 0.0f) {
      aHue += 360.0f;
    }
  }
}

/**
 * Convert from RGB to CIE L*ab color space.
 */
void bpColor::RGBtoLAB(bpDouble aRed, bpDouble aGreen, bpDouble aBlue,
                       bpDouble& aLightness, bpDouble& aColorComponentA, bpDouble& aColorComponentB)
{
  bpDouble vX, vY, vZ;
  RGBtoXYZ(aRed, aGreen, aBlue, vX, vY, vZ);
  XYZtoLAB(vX, vY, vZ, aLightness, aColorComponentA, aColorComponentB);
}

/**
 * Convert from RGB to CIE XYZ color space.
 * Source: www.easyrgb.com
 */
void bpColor::RGBtoXYZ(bpDouble aRed, bpDouble aGreen, bpDouble aBlue,
                       bpDouble& aX, bpDouble& aY, bpDouble& aZ)
{
  if (aRed > 0.04045) {
    aRed = std::pow((aRed + 0.055) / 1.055, 2.4);
  }
  else {
    aRed = aRed / 12.92;
  }
  if (aGreen > 0.04045) {
    aGreen = std::pow((aGreen + 0.055) / 1.055, 2.4);
  }
  else {
    aGreen = aGreen / 12.92;
  }
  if (aBlue > 0.04045) {
    aBlue = std::pow((aBlue + 0.055) / 1.055, 2.4);
  }
  else {
    aBlue = aBlue / 12.92;
  }

  aRed *= 100.0;
  aGreen *= 100.0;
  aBlue *= 100.0;

  // Observer = 2°, Illuminant = D65
  aX = aRed * 0.4124 + aGreen * 0.3576 + aBlue * 0.1805;
  aY = aRed * 0.2126 + aGreen * 0.7152 + aBlue * 0.0722;
  aZ = aRed * 0.0193 + aGreen * 0.1192 + aBlue * 0.9505;
}

/**
 * Convert from CIE XYZ to CIE L*ab color space.
 * Source: www.easyrgb.com
 */
void bpColor::XYZtoLAB(bpDouble aX, bpDouble aY, bpDouble aZ,
                       bpDouble& aLightness, bpDouble& aColorComponentA, bpDouble& aColorComponentB)
{
  // normalize by reference white point (Observer= 2°, Illuminant= D65)
  aX /= 95.047;
  aY /= 100.000;
  aZ /= 108.883;

  if (aX > 0.008856) {
    aX = std::pow(aX, 1.0/3.0);
  }
  else {
    aX = (7.787 * aX) + (16.0/116.0);
  }
  if (aY > 0.008856) {
    aY = std::pow(aY, 1.0/3.0);
  }
  else {
    aY = (7.787 * aY) + (16.0/116.0);
  }
  if (aZ > 0.008856) {
    aZ = std::pow(aZ, 1.0/3.0);
  }
  else {
    aZ = (7.787 * aZ) + (16.0/116.0);
  }

  aLightness = (116.0 * aY) - 16.0;
  aColorComponentA = 500.0 * (aX - aY);
  aColorComponentB = 200.0 * (aY - aZ);
}

bpFloat bpColor::ConvertHexToFloat(const bpString& aHex)
{
  std::stringstream vStringstream;
  bpUInt32 vTemp;
  vStringstream << std::hex << aHex;
  vStringstream >> vTemp;

  return vTemp / 255.0f;
}


bpString bpColor::ConvertFloatToHex(bpFloat aFloat)
{
  bpUInt32 vTemp = std::min(std::max(static_cast<bpUInt32>(aFloat * 255.0f), bpUInt32(0)), bpUInt32(255));

  std::stringstream vStringstream;
  vStringstream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << vTemp;

  return vStringstream.str();
}
