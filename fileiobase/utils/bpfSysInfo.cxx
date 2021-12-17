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


#include "../utils/bpfSysInfo.h"

#if defined(_WIN32)
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#include <boost/filesystem.hpp>


bpfString bpfSysInfo::GetTemporaryPath()
{
  return boost::filesystem::temp_directory_path().generic_string();
}


bpfUInt32 bpfSysInfo::GetProcessId()
{
#if defined(_WIN32)
  return _getpid();
#else
  return getpid();
#endif
}
