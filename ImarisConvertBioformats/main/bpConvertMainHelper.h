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

#include "bpConverterApplication.h"
#include "bpConverter.h"
#include "../meta/bpUtils.h"
#include "../src/bpConverterVersion.h"


#include <cctype>
#include <csetjmp>
#include <csignal>
#include <ctime>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

void bpHandleSignal(int aParameter)
{
  std::cerr << "Abnormal Termination! Parameter = " << aParameter << std::endl;
  exit(IMARIS_CONVERT_EXIT_ABNORMAL_TERMINATION);
}


void bpHandleAllSignals(void(*aSignalHandler)(int))
{
  //signal(SIGABRT, aSignalHandler);
  signal(SIGFPE, aSignalHandler);
  signal(SIGILL, aSignalHandler);
  signal(SIGINT, aSignalHandler);
  signal(SIGSEGV, aSignalHandler);
  signal(SIGTERM, aSignalHandler);
#if defined(_WIN32)
  signal(SIGBREAK, aSignalHandler);
#endif

#if !defined(_WIN32)
  signal(SIGCHLD, aSignalHandler);
  signal(SIGHUP, aSignalHandler);
  signal(SIGQUIT, aSignalHandler);
  signal(SIGTRAP, aSignalHandler);
  signal(SIGKILL, aSignalHandler);
  signal(SIGPIPE, SIG_IGN);
#endif
}



int Init(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories, const std::vector<bpString>& aArguments)
{
#if defined(_WIN32)
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  _putenv("TZ=");
  _tzset();
#endif

  bpConverterApplication vApplication;


  return vApplication.Execute(aFileReaderFactories, aArguments);
}


