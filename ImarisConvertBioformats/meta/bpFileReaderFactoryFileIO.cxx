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


#include "bpFileReaderFactoryFileIO.h"
#include "bpFileReaderImplInterfaceToImpl.h"
#include "bpFileReaderScene.h"

#include "fileiobase/application/bpfExceptionBase.h"
#include "fileiobase/application/bpfFileReaderImplFactoryBase.h"


static const bpString mImarisScene = "ImarisSceneFile";


bpFileReaderFactoryFileIO::bpFileReaderFactoryFileIO()
{
  AddFormats();

  AddFormat(mImarisScene, bpFileReaderScene::GetFormatDescription(), bpFileReaderScene::GetFileFormatExtension());
}


bpFileReaderFactoryFileIO::~bpFileReaderFactoryFileIO()
{
}


void bpFileReaderFactoryFileIO::SetPluginsPath(const bpString& aPluginsPath)
{
  if (aPluginsPath == mPluginsPath) {
    return;
  }

  CleanupFormats();

  for (const auto& vFactory : mFileReaderImplFactories) {
    vFactory->AddPluginsFormats(aPluginsPath);
  }

  mPluginsPath = aPluginsPath;

  AddFormats();
}


void bpFileReaderFactoryFileIO::AddFactoryImpl(const bpSharedPtr<bpfFileReaderImplFactoryBase>& aFactory)
{
  mFileReaderImplFactories.push_back(aFactory);
  AddFormats(aFactory);
}


bpSharedPtr<bpfFileReaderImplFactoryBase> bpFileReaderFactoryFileIO::GetFactoryImpl() const
{
  return !mFileReaderImplFactories.empty() ? mFileReaderImplFactories.front() : bpSharedPtr<bpfFileReaderImplFactoryBase>();
}


bpFileReader::tPtr bpFileReaderFactoryFileIO::CreateFileReader(const bpString& aFileName, const bpString& aFormat)
{
  if (aFormat == mImarisScene) {
    try {
      return std::make_shared<bpFileReaderScene>(aFileName);
    }
    catch (...) {
    }
  }

  bpfFileReaderImplInterface* vFileReaderImplInterface = nullptr;
  for (const auto& vFactory : mFileReaderImplFactories) {
    try {
      vFileReaderImplInterface = vFactory->CreateFileReader(aFileName, aFormat);
    }
    catch (...) {
    }

    if (vFileReaderImplInterface) {
      break;
    }
  }

  if (!vFileReaderImplInterface) {
    throw std::runtime_error(("Could not create file reader.\nFile format: " + aFormat + "\nFile name: " + aFileName).c_str());
  }

  bpSharedPtr<bpfFileReaderImplInterface> vFileReaderImplInterfacePtr(vFileReaderImplInterface);
  bpSharedPtr<bpFileReaderImpl> vFileReaderImpl = std::make_shared<bpFileReaderImplInterfaceToImpl>(vFileReaderImplInterfacePtr);
  return std::make_shared<bpFileReader>(vFileReaderImpl);
}


std::list<bpString> bpFileReaderFactoryFileIO::ReadFormats()
{
  std::list<bpString> vFormats;

  for (const auto& vFactory : mFileReaderImplFactories) {
    auto vFormatIt = vFactory->FormatBegin();
    auto vFormatItEnd = vFactory->FormatEnd();

    while (vFormatIt != vFormatItEnd) {
      vFormats.push_back(*vFormatIt);
      ++vFormatIt;
    }
  }

  return vFormats;
}


void bpFileReaderFactoryFileIO::AddFormats()
{
  for (const auto& vFactory : mFileReaderImplFactories) {
    AddFormats(vFactory);
  }

}


void bpFileReaderFactoryFileIO::AddFormats(const bpSharedPtr<bpfFileReaderImplFactoryBase>& aFactory)
{
  auto vFormatIt = aFactory->FormatBegin();
  auto vFormatItEnd = aFactory->FormatEnd();

  while (vFormatIt != vFormatItEnd) {
    auto vFormatExtensions = aFactory->GetFormatExtensions(*vFormatIt);

    AddFormat(*vFormatIt, aFactory->GetFormatDescription(*vFormatIt), vFormatExtensions);
    ++vFormatIt;
  }
}


void bpFileReaderFactoryFileIO::CleanupFormats()
{
  auto vOldFormats = ReadFormats();

  for (const auto& vFormat : vOldFormats) {
    RemoveFormat(vFormat);
  }
}
