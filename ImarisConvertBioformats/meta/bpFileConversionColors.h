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


#ifndef __BPFILECONVERSIONCOLORS__
#define __BPFILECONVERSIONCOLORS__


#include "bpColor.h"


namespace bpFileConversionColorsConstant
{
  static const bpColor mDefaultColor0(1.0f, 0.0f, 0.0f);
  static const bpColor mDefaultColor1(0.0f, 1.0f, 0.0f);
  static const bpColor mDefaultColor2(0.0f, 0.0f, 1.0f);
  static const bpColor mDefaultColor3(1.0f, 1.0f, 1.0f);
  static const bpSize mNumberOfUniqueChannels = 4;
}


enum class bpColorHint {
  eDefault,
  eLUT,
  eEmission
};


bpString bpToString(bpColorHint aColorHint);
void bpFromString(const bpString& aText, bpColorHint& aColorHint);


class bpFileConversionColors
{
public:
  using tDefaultColors = std::vector<bpColor>;


  explicit bpFileConversionColors(const tDefaultColors& aDefaultColors = tDefaultColors());
  ~bpFileConversionColors() = default;

  const bpColor& GetDefaultColor(bpSize aChannelId) const;
  const tDefaultColors& GetDefaultColors() const;
  void SetDefaultColors(const tDefaultColors& aDefaultColors);

private:
  void ApplyDefaultColors();

  tDefaultColors mDefaultColors;
};


#endif
