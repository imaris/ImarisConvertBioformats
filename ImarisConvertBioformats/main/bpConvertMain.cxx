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



#include "bpConvertMainHelper.h"
#include "fileiobioformats/application/bpfFileReaderBioformatsImplFactory.h"
#include "fileiobase/types/bpfSmartPtr.h"


int main(int argc, char* argv[])
{
  std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>> vFileReaderImplFactories;
  vFileReaderImplFactories.push_back(bpfMakeSharedPtr<bpfFileReaderBioformatsImplFactory>());
  
  #ifdef _WIN32
  int vArgc = 0;
  LPWSTR* vArgs = CommandLineToArgvW(GetCommandLineW(), &vArgc);
  std::vector<bpString> vArguments;
  for (int vIndex = 0; vIndex < vArgc; ++vIndex) {
    vArguments.push_back(bpToUtf8(vArgs[vIndex]));
  }
#else
  std::vector<bpString> vArguments(argv, argv + argc);
#endif

  return Init(vFileReaderImplFactories, vArguments);
}

#ifndef BP_UNIT_TEST

// shared code from ImarisWriter, maybe should be moved in a shared lib
#include "ImarisWriter/writer/bpHistogram.cxx"
#include "ImarisWriter/writer/bpImsImage3D.cxx"
#include "ImarisWriter/writer/bpImsImage5D.cxx"
#include "ImarisWriter/writer/bpImsLayout.cxx"
#include "ImarisWriter/writer/bpImsLayout3D.cxx"
#include "ImarisWriter/writer/bpImsImageBlock.cxx"
#include "ImarisWriter/writer/bpMemoryManager.cxx"
#include "ImarisWriter/writer/bpMultiresolutionImsImage.cxx"
#include "ImarisWriter/writer/bpOptimalBlockLayout.cxx"
#include "ImarisWriter/writer/bpThreadPool.cxx"
#include "ImarisWriter/writer/bpThumbnailBuilder.cxx"


#endif