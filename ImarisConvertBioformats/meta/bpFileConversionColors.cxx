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


#include "bpFileConversionColors.h"


bpString bpToString(bpColorHint aColorHint)
{
  bpString vText;

  switch (aColorHint) {
    case bpColorHint::eDefault:
      vText = "ColorDefaultHint";
      break;
    case bpColorHint::eLUT:
      vText = "ColorLUTHint";
      break;
    case bpColorHint::eEmission:
      vText = "ColorEmissionHint";
      break;
    default:
      break;
  }

  return vText;
}


void bpFromString(const bpString& aText, bpColorHint& aColorHint)
{
  aColorHint = bpColorHint::eLUT;

  if ("ColorDefaultHint" == aText) {
    aColorHint = bpColorHint::eDefault;
  }
  else if ("ColorLUTHint" == aText) {
    aColorHint = bpColorHint::eLUT;
  }
  else if ("ColorEmissionHint" == aText) {
    aColorHint = bpColorHint::eEmission;
  }
}


bpFileConversionColors::bpFileConversionColors(const tDefaultColors& aDefaultColors)
{
  SetDefaultColors(aDefaultColors);
}


const bpColor& bpFileConversionColors::GetDefaultColor(bpSize aChannelId) const
{
  auto vChannelId = std::min(aChannelId, mDefaultColors.size() - 1);

  return mDefaultColors[vChannelId];
}


const bpFileConversionColors::tDefaultColors& bpFileConversionColors::GetDefaultColors() const
{
  return mDefaultColors;
}


void bpFileConversionColors::SetDefaultColors(const tDefaultColors& aDefaultColors)
{
  if (bpFileConversionColorsConstant::mNumberOfUniqueChannels == aDefaultColors.size()) {
    mDefaultColors = aDefaultColors;
  }
  else {
    ApplyDefaultColors();
  }
}

void bpFileConversionColors::ApplyDefaultColors()
{
  //create red, green, blue and white channels

  mDefaultColors = { bpFileConversionColorsConstant::mDefaultColor0,
                     bpFileConversionColorsConstant::mDefaultColor1,
                     bpFileConversionColorsConstant::mDefaultColor2,
                     bpFileConversionColorsConstant::mDefaultColor3 };
}
