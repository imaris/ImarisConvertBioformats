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


#ifndef __BP_FILE_IO_EXCEPTION__
#define __BP_FILE_IO_EXCEPTION__

/*****************************************************************************
 *
 * Class for FileIO Exceptions.
 *
 *\begin{tabular}{ll}
 * Base classes          & bpfFileException \\
 * Known derived classes & should not be used as base class \\
 *\end{tabular}
 *
 ****************************************************************************/
#include "fileiobase/exceptions/bpfFileException.h"

class bpfFileIOException : public bpfFileException
{
public:
  /**************************************************************************
   *
   * Construct an exception with no detail message.
   *
   **************************************************************************/
  bpfFileIOException()
    :bpfFileException("bpfFileIOException","") {}

  /**************************************************************************
   *
   * Construct an exception with the given detail message.
   *
   **************************************************************************/
  bpfFileIOException(const bpfString& detailMessage)
    :bpfFileException("bpfFileIOException",detailMessage) {}
};

#endif
