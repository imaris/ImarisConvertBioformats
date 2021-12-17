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


#include "bpTimeout.h"


#include <thread>
#include <cstdlib>


void bpTimeout::Start(bpFloat aTimeoutSeconds, int aExitCode)
{
  std::thread vThread([aTimeoutSeconds, aExitCode] {
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<bpUInt64>(aTimeoutSeconds * 1000)));
    std::exit(aExitCode);
  });
  vThread.detach();
}
