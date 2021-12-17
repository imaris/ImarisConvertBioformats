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


#include "bpWriterFactoryFileThumbnail.h"


bpWriterFactoryFileThumbnail::bpWriterFactoryFileThumbnail(bpWriterFileThumbnail::tFormat aFormat)
  : mFormat(aFormat)
{
}

bpWriterFactoryFileThumbnail::~bpWriterFactoryFileThumbnail()
{
}

bpSharedPtr<bpWriter> bpWriterFactoryFileThumbnail::CreateWriter(const bpString& aFilename, const bpImsLayout& aImageLayout, bpConverterTypes::tCompressionAlgorithmType aCompressionAlgorithmType)
{
  return std::make_shared<bpWriterFileThumbnail>(aFilename, mFormat);
}
