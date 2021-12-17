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


#ifndef __BP_CONVERTER_APPLICATION__
#define __BP_CONVERTER_APPLICATION__


#include "../src/bpTimeout.h"
#include "fileiobase/application/bpfFileReaderImplFactoryBase.h"

class bpFileReaderFactory;


class bpConverterApplication
{
public:
  int Execute(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories, const std::vector<bpString>& aArguments);

private:
  void PrintInputFormats(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories) const;
  void PrintOutputFormats() const;
  void PrintCmdLineHelp(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories, const bpString& aProgramName) const;
  void PrintCmdLineUsage(const bpString& aProgramName) const;
  void PrintCmdLineVersion() const;
  void PrintExitCodes() const;

  virtual void LogMessage(const bpString& aMessage) const;

  void SetColorHint(const bpString& aColorHint);
  void SetDefaultColors(const bpString& aColor0, const bpString& aColor1, const bpString& aColor2, const bpString& aColor3);
  bool IsAtomicArgument(bpSize aArgIndex, const std::vector<bpString>& aArguments);

  bpString GetVersionFullStringRevision() const;
  bpSharedPtr<bpFileReaderFactory> CreateFileReaderFactory(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories) const;

  bpTimeout mTimeout;
};


#endif
