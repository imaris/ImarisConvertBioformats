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


#include "bpWriterFileThumbnail.h"

#include "../meta/bpUtils.h"


#include <FreeImage.h>


bpWriterFileThumbnail::bpWriterFileThumbnail(const bpString& aFilename, tFormat aFormat)
  : mFilename(aFilename),
    mFormat(aFormat)
{
}


bpWriterFileThumbnail::~bpWriterFileThumbnail()
{
}


void bpWriterFileThumbnail::WriteThumbnail(const bpThumbnail& aThumbnail)
{
  bpSize vSizeX = aThumbnail.GetSizeX();
  bpSize vSizeY = aThumbnail.GetSizeY();

  unsigned vSize = static_cast<unsigned>(std::max(vSizeX, vSizeY));
  FIBITMAP* vImage = FreeImage_Allocate(vSize, vSize, 24);
  RGBQUAD vColor;
  vColor.rgbReserved = 1;
  bpSize vOffX = vSizeY > vSizeX ? (vSizeY - vSizeX) / 2 : 0;
  bpSize vOffY = vSizeX > vSizeY ? (vSizeX - vSizeY) / 2 : 0;
  for (bpSize vIndexY = 0; vIndexY < vSizeY; ++vIndexY) {
    for (bpSize vIndexX = 0; vIndexX < vSizeX; ++vIndexX) {
      vColor.rgbRed = aThumbnail.GetR(vIndexX, vIndexY);
      vColor.rgbGreen = aThumbnail.GetG(vIndexX, vIndexY);
      vColor.rgbBlue = aThumbnail.GetB(vIndexX, vIndexY);
      FreeImage_SetPixelColor(vImage, static_cast<unsigned>(vOffX + vIndexX), static_cast<unsigned>(vOffY + vSizeY - vIndexY - 1), &vColor);
    }
  }

  FREE_IMAGE_FORMAT vFormat = FIF_PNG;
  int vFlags = 0;
  if (mFormat == tFormat::eJPEG) {
    vFormat = FIF_JPEG;
    vFlags = JPEG_QUALITYNORMAL;
  }

#ifdef BP_UTF8_FILENAMES
  FreeImage_SaveU(vFormat, vImage, bpFromUtf8(mFilename).c_str(), vFlags);
#else
  FreeImage_Save(vFormat, vImage, mFilename.c_str(), vFlags);
#endif

  FreeImage_Unload(vImage);
}
