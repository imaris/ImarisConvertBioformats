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


/**
 *
 *
 * Color Utilities.
 *
 * (moved from bpfImageUtils.h)
 */


#include "fileiobase/types/bpfTypes.h"
#include "fileiobase/types/bpfColorInfo.h"

class bpfParameterSection;
class bpfSectionContainer;

/**
 * When the color info is stored in a parameter section it used the following
 * format :
 * ColorMode  = [BaseColor | Table Color]
 * ColorRange = min max
 * Color      =  R G B
 * ColorTable = ....
 * ColorTableLength = length
 */

/**
 * Write the colorinfo to the specified parameter section.
 *
 * @param aSection The section into which to write the color info.
 * @param aColor   The color.
 */
void bpfStoreColorInParameters(bpfParameterSection* aSection, const bpfColorInfo& aColor);


/**
 * Get a color from the parameter section. If no color is found an exception is
 * thrown.
 *
 * @param aSection The section from which to get the color info.
 * @return The color.
 */
bpfColorInfo bpfGetColorFromParameters(const bpfParameterSection* aSection);


/**
 * Remove the color settings from the specified parameter section.
 *
 * @param aSection The section from which to remove the color info.
 */
void bpfRemoveColorFromParameters(bpfParameterSection* aSection);


bpfString bpfConvColorToString(const bpfColor& aColor);
bpfString bpfConvTableToString(const bpfColorInfo&aColorInfo);


/**
 * Converts a string of the format RRGGBB in which the two corresponding
 * letters represent the hexadecimal value of their color into bpfColor
 *
 * Accepted formats are: RRGGBB
 *                       AARRGGBB
 *                       #RRGGBB
 *                       #AARRGGBB
 *
 * The '#' and the AA value are ignored
 */
bpfColor bpfConvHexStringToColor(const bpfString& aColorString);

int bpfCalcWaveLengthColor(bpfFloat aWavelength, bpfColor& aColor);

void bpfConvStringToColor(bpfColor& aColor, const bpfString& aString);
void bpfConvStringToTable(bpfColorInfo& aColorInfo,
                          const bpfString& aString,
                          bpfInt32 aLength);

int GetAlternateColor(bpfSectionContainer* aPreferences,
                      bpfInt32 aChannel,
                      bpfInt32 aColorNumber,
                      bpfColor&aColor);

int GetAlternateColorMap(bpfSectionContainer* aPreferences,
                         bpfInt32 aChannel,
                         bpfInt32 aColorMapNumber,
                         bpfColorInfo& aColorInfo);

void SetAlternateColor(bpfSectionContainer* aPreferences,
                       bpfInt32 aChannel,
                       bpfInt32 aColorNumber,
                       const bpfColor& aColor);

void SetAlternateColorMap(bpfSectionContainer* aPreferences,
                          bpfInt32 aChannel,
                          bpfInt32 aColorMapNumber,
                          const bpfColorInfo& aColorInfo);

void AddAlternateColor(bpfSectionContainer* aPreferences,
                       bpfInt32 aChannel,
                       const bpfColor& aColor);

void AddAlternateColorMap(bpfSectionContainer* aPreferences,
                          bpfInt32 aChannel,
                          const bpfColorInfo&aColorInfo);
