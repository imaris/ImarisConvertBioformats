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


#include "bpThumbnailReader.h"

#include <FreeImage.h>


static bpSharedPtr<bpThumbnail> MakeThumbnailFromImage(FIBITMAP* aImage)
{
  if (!aImage) {
    return{};
  }

  bpSize vSizeX = static_cast<bpSize>(FreeImage_GetWidth(aImage));
  bpSize vSizeY = static_cast<bpSize>(FreeImage_GetHeight(aImage));

  std::vector<bpUInt8> vRGBA(vSizeX * vSizeY * 4);

  RGBQUAD vColor;
  for (bpSize vIndexY = 0; vIndexY < vSizeY; ++vIndexY) {
    for (bpSize vIndexX = 0; vIndexX < vSizeX; ++vIndexX) {
      FreeImage_GetPixelColor(aImage, static_cast<unsigned>(vIndexX), static_cast<unsigned>(vIndexY), &vColor);
      bpSize vOff = (vIndexX + vIndexY * vSizeX) * 4;
      vRGBA[vOff + 0] = vColor.rgbRed;
      vRGBA[vOff + 1] = vColor.rgbGreen;
      vRGBA[vOff + 2] = vColor.rgbBlue;
      vRGBA[vOff + 3] = 255;
    }
  }

  FreeImage_Unload(aImage);

  return std::make_shared<bpThumbnail>(vSizeX, vSizeY, vRGBA);
}


bpSharedPtr<bpThumbnail> bpThumbnailReader::ReadThumbnailFromFile(const bpString& aFilename)
{
  FREE_IMAGE_FORMAT vFormat = FreeImage_GetFileType(aFilename.c_str());
  if (vFormat == FIF_UNKNOWN) {
    return{};
  }

  FIBITMAP* vImage = FreeImage_Load(vFormat, aFilename.c_str());

  return MakeThumbnailFromImage(vImage);
}


bpSharedPtr<bpThumbnail> bpThumbnailReader::ReadThumbnailFromData(const bpString& aBinaryData)
{
  FIMEMORY* vMemory = FreeImage_OpenMemory(reinterpret_cast<BYTE*>(const_cast<char*>(aBinaryData.data())), aBinaryData.size());

  FREE_IMAGE_FORMAT vFormat = FreeImage_GetFileTypeFromMemory(vMemory);
  if (vFormat == FIF_UNKNOWN) {
    FreeImage_CloseMemory(vMemory);
    return{};
  }

  FIBITMAP* vImage = FreeImage_LoadFromMemory(vFormat, vMemory);
  FreeImage_CloseMemory(vMemory);

  return MakeThumbnailFromImage(vImage);
}
