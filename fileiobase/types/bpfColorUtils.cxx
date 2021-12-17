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


#include "fileiobase/types/bpfColorUtils.h"


#include "fileiobase/types/bpfParameterSection.h"
#include "fileiobase/types/bpfSectionContainer.h"
#include "fileiobase/exceptions/bpfNoSuchSectionException.h"

#include <string.h>



void bpfStoreColorInParameters(bpfParameterSection* aSection, const bpfColorInfo& aColor)
{
  if (aColor.GetColorMode() == bpfColorInfo::eBaseColor) {
    aSection->SetParameter("ColorMode", "BaseColor");
    aSection->SetParameter("Color", bpfConvColorToString(aColor.GetBaseColor()));
  }
  else {
    aSection->SetParameter("ColorMode", "TableColor");
    aSection->SetParameter("ColorTable", bpfConvTableToString(aColor));
    aSection->SetParameter("ColorTableLength", bpfToString(aColor.GetTableLength()));
  }
  aSection->SetParameter("ColorRange", bpfToString(aColor.GetRangeMin()) + " " + bpfToString(aColor.GetRangeMax()));
  aSection->SetParameter("ColorOpacity", bpfToString(aColor.GetOpacity()));
  aSection->SetParameter("GammaCorrection", bpfToString(aColor.GetGammaCorrectionValue()));
}


bpfColorInfo bpfGetColorFromParameters(const bpfParameterSection* aSection)
{
  bpfColorInfo vNewColor;
  try {
    if (aSection->GetParameter("ColorMode") == "BaseColor") {
      // read base color
      bpfColor vBaseColor;
      bpfConvStringToColor(vBaseColor, aSection->GetParameter("Color"));
      vNewColor.SetBaseColor(vBaseColor);
    }
    else {
      // read color table
      bpfInt32 vLength = 0;
      bpfFromString(aSection->GetParameter("ColorTableLength"), vLength);
      bpfConvStringToTable(vNewColor, aSection->GetParameter("ColorTable"), vLength);
    }
    // read the range
    bpfFloat vRangeMin = 0;
    bpfFloat vRangeMax = 0;
    bpfString vRangeString = aSection->GetParameter("ColorRange");
    std::vector<bpfString> vStrings = bpfSplit(vRangeString, " ");
    if (vStrings.size() >= 2) {
      bpfFromString(vStrings[0], vRangeMin);
      bpfFromString(vStrings[1], vRangeMax);
    }
    vNewColor.SetRange(vRangeMin, vRangeMax);
    // read opacity
    if (aSection->HasParameter("ColorOpacity")) {
      bpfFloat vOpacity = 1.0f;
      bpfFromString(aSection->GetParameter("ColorOpacity"), vOpacity);
      vNewColor.SetOpacity(vOpacity);
    }
    // read Gamma Correction
    if (aSection->HasParameter("GammaCorrection")) {
      vNewColor.SetGammaCorrectionValue(bpfFromString<bpfFloat>(aSection->GetParameter("GammaCorrection")));
    }
  }
  catch (bpfException) {
    BP_DEBUG_MSG("bpfGetColorFromParameters: Could not get color");
  }
  return vNewColor;
}


void bpfRemoveColorFromParameters(bpfParameterSection* aSection)
{
  // Remove all parameters for the color setting
  aSection->RemoveParameter("ColorMode");
  aSection->RemoveParameter("Color");
  aSection->RemoveParameter("ColorTable");
  aSection->RemoveParameter("ColorTableLength");
  aSection->RemoveParameter("ColorRange");
  aSection->RemoveParameter("ColorOpacity");
  aSection->RemoveParameter("GammaCorrection");
}


bpfString bpfConvTableToString(const bpfColorInfo& aColorInfo)
{
  std::ostringstream vColorStringstream;
  bpfUInt32 i;
  for (i = 0; i < aColorInfo.GetTableLength(); i++) {
    vColorStringstream << bpfConvColorToString(aColorInfo[i]) << " ";
  }
  return vColorStringstream.str();
}


bpfColor bpfConvHexStringToColor(const bpfString& aColorString)
{
  bpfString vColorString = aColorString;
  bpfSize vRGB[3];
  vRGB[0] = 0;
  vRGB[1] = 0;
  vRGB[2] = 0;
  if(aColorString[0] == '#') {
    vColorString = vColorString.substr(1);
  }
  for(bpfSize vColorIndex = 0; vColorIndex < 3; vColorIndex++) {
    for(bpfSize vLetterIndex = 0; vLetterIndex < 2; vLetterIndex++) {
      const char* vLetter = &vColorString[vColorIndex * 2 + vLetterIndex + (vColorString.size() <= 6 ? 0 : 2)];
      switch(vLetter[0]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        bpfSize vLetterValue;
        bpfFromString(bpfString(vLetter, 1), vLetterValue);
        vRGB[vColorIndex] += vLetterValue;
        break;
      case 'A':
        vRGB[vColorIndex] += 10;
        break;
      case 'B':
        vRGB[vColorIndex] += 11;
        break;
      case 'C':
        vRGB[vColorIndex] += 12;
        break;
      case 'D':
        vRGB[vColorIndex] += 13;
        break;
      case 'E':
        vRGB[vColorIndex] += 14;
        break;
      case 'F':
        vRGB[vColorIndex] += 15;
        break;
      }
      if(vLetterIndex == 0) {
        vRGB[vColorIndex] *= 16;
      }
    }
  }
  return bpfColor(bpfFloat(vRGB[0])/255.0f, bpfFloat(vRGB[1])/255.0f, bpfFloat(vRGB[2])/255.0f);
}


bpfString bpfConvColorToString(const bpfColor& aColor)
{
  return bpfToString(aColor.Red()) + " " +
      bpfToString(aColor.Green()) + " " +
      bpfToString(aColor.Blue());
}


void bpfConvStringToColor(bpfColor& aColor, const bpfString& aString)
{
  bpfColorInfo vColorInfo;
  bpfConvStringToTable(vColorInfo, aString, 1);
  aColor = vColorInfo[0];
}


void bpfConvStringToTable(bpfColorInfo& aColorInfo,
                          const bpfString& aString,
                          bpfInt32 aLength)
{
  bpfInt32 vI, vJ;
  bpfFloat vValue;
  char* vDatp, *vDate, vDattemp;
  char* vDats = new char[strlen(aString.c_str()) + 1];
  strcpy(vDats, aString.c_str());
  aColorInfo.SetTableLength(static_cast<bpfUInt32>(aLength));

  for (vI = 0; vI < aLength; vI++) {
    vValue = (float)vI / (float)aLength;
    aColorInfo[static_cast<bpfUInt32>(vI)].SetRGB(vValue, vValue, vValue);
  }

  vI = 0;
  vDatp = vDats;
  while (*vDatp == ' ') vDatp++;

  do {
    if (*vDatp != '.' &&
        (*vDatp < '0' || *vDatp > '9'))
      return;

    vDate = vDatp;

    while (*vDate == '.' ||
           (*vDate  >= '0' && *vDate <= '9'))
      vDate++;

    vDattemp = *vDate;
    *vDate = 0;
    vValue = (bpfFloat)atof(vDatp);
    *vDate = vDattemp;

    vDatp = vDate;
    while (*vDatp == ' ') vDatp++;

    vJ = vI / 3;
    if (vJ >= aLength) return;

    float vRed, vGreen, vBlue;
    aColorInfo[static_cast<bpfUInt32>(vJ)].GetRGB(vRed, vGreen, vBlue);

    switch (vI % 3) {
    case 0:
      aColorInfo[static_cast<bpfUInt32>(vJ)].SetRGB(vValue, vGreen, vBlue);
      break;
    case 1:
      aColorInfo[static_cast<bpfUInt32>(vJ)].SetRGB(vRed, vValue, vBlue);
      break;
    case 2:
      aColorInfo[static_cast<bpfUInt32>(vJ)].SetRGB(vRed, vGreen, vValue);
      break;
    default:
      break;
    }
    vI++;
  }
  while (*vDatp == '.' ||
         (*vDatp  >= '0' && *vDatp <= '9'));

  delete[] vDats;
  return;
}




int GetAlternateColorMap(bpfSectionContainer* aPreferences,
                         bpfInt32 aChannel,
                         bpfInt32 aColorMapNumber,
                         bpfColorInfo& aColorInfo)
{
  std::ostringstream vMap;
  std::ostringstream vMapCount;
  std::ostringstream vChannel;
  vMap << "AlternateColorMapEntries" << aColorMapNumber;
  vMapCount << "AlternateColorMapLength" << aColorMapNumber;
  vChannel << "Channel " << aChannel;
  bpfString vLengthString;
  bpfString vMapString;

  try {
    vLengthString = aPreferences->GetParameter(vChannel.str(), vMapCount.str());
    vMapString = aPreferences->GetParameter(vChannel.str(), vMap.str());
  }
  catch (bpfNoSuchParameterException) {
    return 0;
  }
  catch (bpfNoSuchSectionException) {
    return 0;
  }

  bpfInt32 vLength = (bpfInt32)atoi(vLengthString.c_str());
  if (vLength < 1 || strlen(vMapString.c_str()) == 0) {
    return 0;
  }
  bpfConvStringToTable(aColorInfo, vMapString.c_str(), vLength);
  return 1;
}


int GetAlternateColor(bpfSectionContainer* aPreferences,
                      bpfInt32 aChannel,
                      bpfInt32 aColorNumber,
                      bpfColor& aColor)
{
  // read the color
  bpfString vName = bpfString("AlternateColor") + bpfToString(aColorNumber);
  bpfString vChannelName = bpfString("Channel ") + bpfToString(aChannel);
  bpfString vColorString;
  try {
    vColorString = aPreferences->GetParameter(vChannelName, vName);
  }
  catch (bpfException) {
    BP_DEBUG_MSG("GetAlternateColor: could not get color");
    return 0;
  }
  if (vColorString.empty()) {
    return 0;
  }
  bpfConvStringToColor(aColor, vColorString);

  // read the range
  try {
    vName = bpfString("ColorMin") + bpfToString(aColorNumber);
    bpfString vMinString = aPreferences->GetParameter(vChannelName, vName);
    bpfFloat vMin = 0.0F;
    bpfFromString(vMinString, vMin);

    vName = bpfString("ColorMax") + bpfToString(aColorNumber);
    bpfString vMaxString = aPreferences->GetParameter(vChannelName, vName);
    bpfFloat vMax = 0.0F;
    bpfFromString(vMaxString, vMax);
  }
  catch (bpfException) {
    BP_DEBUG_MSG("GetAlternateColor: could not get range info");
    return 0;
  }
  return 1;
}


void SetAlternateColor(bpfSectionContainer* aPreferences,
                       bpfInt32 aChannel,
                       bpfInt32 aColorNumber,
                       const bpfColor& aColor)
{
  // Write the color into the channel section, the parameters written are
  // AlternateColori, ColorMini, ColorMaxi, where i is the color number
  bpfString vSectionName = bpfString("Channel ") + bpfToString(aChannel);
  bpfParameterSection* vSection = aPreferences->GetSection(vSectionName);

  // save the color and the range
  bpfString vColorString = bpfToString(aColorNumber);
  bpfString vName = bpfString("AlternateColor") + vColorString;
  vSection->SetParameter(vName, bpfConvColorToString(aColor));
}


void SetAlternateColorMap(bpfSectionContainer* aPreferences,
                          bpfInt32 aChannel,
                          bpfInt32 aColorMapNumber,
                          const bpfColorInfo& aColorInfo)
{
  // Write the color into the channel section, the parameters written are
  // AlternateColorMapEntriesi, AlternateColorMapLengthi, ColorMini, ColorMaxi, where i is the color number
  bpfString vSectionName = bpfString("Channel ") + bpfToString(aChannel);
  bpfParameterSection* vSection = aPreferences->GetSection(vSectionName);

  // cannot take the method below as we have a section container instead of a
  // data set (this should be changed)
  //
  // bpfParameterSection *section = dataSet->GetSection(dataSet->GetChannelSectionName(channel));
  bpfString vColorMapNumberString = bpfToString(aColorMapNumber);
  bpfString vName = bpfString("AlternateColorMapLength") + vColorMapNumberString;

  vSection->SetParameter(vName, bpfToString(aColorInfo.GetTableLength()));
  vName = bpfString("AlternateColorMapEntries") + vColorMapNumberString;
  vSection->SetParameter(vName, bpfConvTableToString(aColorInfo));

  vName = bpfString("ColorMin") + vColorMapNumberString;
  vSection->SetParameter(vName, bpfToString(aColorInfo.GetRangeMin()));

  vName = bpfString("ColorMax") + vColorMapNumberString;
  vSection->SetParameter(vName, bpfToString(aColorInfo.GetRangeMax()));
}


void AddAlternateColor(bpfSectionContainer* aPreferences,
                       bpfInt32 aChannel,
                       const bpfColor& aColor)
{
  bpfInt32 vIndex = 1;
  bpfColor vColorInfo;
  while (GetAlternateColor(aPreferences, aChannel, vIndex, vColorInfo)) {
    vIndex++;
  };
  SetAlternateColor(aPreferences, aChannel, vIndex, aColor);
  return;
}


void AddAlternateColorMap(bpfSectionContainer* aPreferences,
                          bpfInt32 aChannel,
                          const bpfColorInfo& aColorInfo)
{
  bpfInt32 vIndex = 1;
  bpfColorInfo vColorInfo;
  while (GetAlternateColorMap(aPreferences, aChannel, vIndex, vColorInfo)) {
    vIndex++;
  };
  SetAlternateColorMap(aPreferences, aChannel, vIndex, aColorInfo);
  return;
}


int bpfCalcWaveLengthColor(bpfFloat aWavelength, bpfColor& aColor)
{
  float vRed = 0.0f;
  float vGreen = 0.0f;
  float vBlue = 0.0f;

  aColor.SetRGB(vRed, vGreen, vBlue);

  if (aWavelength < 380.0f || aWavelength > 780.0f) {
    return 1;
  }

  if (aWavelength >= 380.0f && aWavelength <= 440.0f) {
    vRed = (440.0f - aWavelength) / 60.0f;
    vGreen = 0.0f;
    vBlue = 1.0f;
  }

  if (aWavelength > 440.0f && aWavelength <= 490.0f) {
    vRed = 0.0f;
    vGreen = (aWavelength - 440.0f) / 50.0f;
    vBlue = 1.0f;
  }

  if (aWavelength > 490.0f && aWavelength <= 510.0f) {
    vRed = 0.0f;
    vGreen = 1.0f;
    vBlue = (510.0f - aWavelength) / 20.0f;
  }

  if (aWavelength > 510.0f && aWavelength <= 580.0f) {
    vRed = (aWavelength - 510.0f) / 70.0f;
    vGreen = 1.0f;
    vBlue = 0.0f;
  }

  if (aWavelength > 580.0f && aWavelength <= 645.0f) {
    vRed = 1.0f;
    vGreen = (645.0f - aWavelength) / 65.0f;
    vBlue = 0.0f;
  }

  if (aWavelength > 645.0f && aWavelength <= 780.0f) {
    vRed = 1.0f;
    vGreen = 0.0f;
    vBlue = 0.0f;
  }

  aColor.SetRGB(vRed, vGreen, vBlue);
  return 0;
}
