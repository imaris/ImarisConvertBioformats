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


#include "fileiobase/types/bpfColor.h"
#include "fileiobase/types/bpfTypes.h"
#include <algorithm>
#include <cmath>


/**
 * Returns a black or white color depending on the luminance
 */
bpfColor bpfColor::GetContrastColor() const
{
  if (GetLuminance() > 0.5f) {
    return bpfColor(0.0f);
  }
  else {
    return bpfColor(1.0f);
  }
}


/**
 * Returns a similar random color
 *
 * @param aVariance similarity of the resulting color (0 => equal; 1 => totally different)
 * @return
 */
bpfColor bpfColor::GetRandomColor(bpfFloat aVariance) const
{
  bpfFloat vHue = 0.0f;        // 0..360
  bpfFloat vSaturation = 0.0f; // 0..1
  bpfFloat vValue = 0.0f;      // 0..1
  GetHSV(vHue, vSaturation, vValue);

//  bpfFloat vVarianceHue = vSaturation*aVariance - vSaturation + 1.0f;
  bpfFloat vVarianceHue = aVariance;
  bpfFloat vVarianceSaturation = aVariance;
  bpfFloat vVarianceValue = aVariance;

  // scramble hue
  bpfFloat vRand = (bpfFloat)rand() / (bpfFloat)RAND_MAX - 0.5f;
  vHue += 360.0f * vRand * vVarianceHue;
  if (vHue > 360.0f) {
    vHue -= 360.0f;
  }
  else if (vHue < 0.0f) {
    vHue += 360.0f;
  }

  // scramble saturation
  vRand = (bpfFloat)rand() / (bpfFloat)RAND_MAX - 0.5f;
  vSaturation += vRand * vVarianceValue;
  if (vSaturation > 1.0f) {
    vSaturation = 1.0f;
  }
  else if (vSaturation < 0.0f) {
    vSaturation = 0.0f;
  }

  // scramble value
  vRand = (bpfFloat)rand() / (bpfFloat)RAND_MAX - 0.5f;
  vValue += vRand * vVarianceSaturation;
  if (vValue > 1.0f) {
    vValue = 1.0f;
  }
  else if (vValue < 0.0f) {
    vValue = 0.0f;
  }

  // compose resultin color
  bpfColor vRandomColor;
  vRandomColor.SetHSV(vHue, vSaturation, vValue);
  return vRandomColor;
}


/**
 * Convert from HSV to RGB color space.
 */
void bpfColor::HSVtoRGB(bpfFloat aHue, bpfFloat aSaturation, bpfFloat aValue,
                        bpfFloat& aRed, bpfFloat& aGreen, bpfFloat& aBlue)
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
    bpfInt32 i = (bpfInt32)floor(aHue);
    bpfFloat f = aHue - i;
    bpfFloat p = aValue * (1.0f-aSaturation);
    bpfFloat q = aValue * (1.0f-(aSaturation*f));
    bpfFloat t = aValue * (1.0f-(aSaturation*(1.0f-f)));
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
void bpfColor::RGBtoHSV(bpfFloat aRed, bpfFloat aGreen, bpfFloat aBlue,
                        bpfFloat& aHue, bpfFloat& aSaturation, bpfFloat& aValue)
{
  bpfFloat max1 = std::max(aRed, aBlue);
  max1 = std::max(max1, aGreen);

  bpfFloat min1 = std::min(aRed, aBlue);
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
    bpfFloat delta = max1-min1;
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

