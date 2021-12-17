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


#ifndef __BP_FILE_EXCEPTION__
#define __BP_FILE_EXCEPTION__

/*****************************************************************************
 *
 * Base class for all exception that occur in the fileIO classes.
 *
 * Base classes          : bpfException
 * Known derived classes : bpfFileIOException, bpf FileFormatException
 *
 *
 ****************************************************************************/
#include "fileiobase/application/bpfException.h"

class bpfFileException : public bpfException
{
public:
  /**************************************************************************
   *
   * Construct an exception with no detail message.
   *
   **************************************************************************/
  bpfFileException()
    :bpfException("bpfFileException","") {}

  /**************************************************************************
   *
   * Construct an exception with the given detail message.
   *
   **************************************************************************/
  bpfFileException(const bpfString& detailMessage)
    :bpfException("bpfFileException",detailMessage) {}

protected:
  /**************************************************************************
   *
   * Construct an exception with the given name and detail message. Used
   * (only) for subclasses.
   *
   *************************************************************************/
  bpfFileException(const bpfString& exceptionName, const bpfString& detailMessage)
    :bpfException(exceptionName,detailMessage) {}
};

#endif
