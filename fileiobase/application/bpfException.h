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


#ifndef __BP_EXCEPTION__
#define __BP_EXCEPTION__

#if defined(_WIN32)
#include <windows.h>
#endif

/*****************************************************************************
 *
 * Base class for all exception.
 *
 * All objects for which exceptions are thrown should be derived from this
 * class. An exception contains its name and may contain a more specific
 * message, which is set when it is constructed.
 *
 * Classes derived from exception should always have two public constructors:\\
 *  - a default constructor, and\\
 *  - a constructor with a bpfString argument \\
 * and a protected contructor (for subclassing) that initializes the name and
 * the message by calling the parents protected constructor. If the class is
 * {\bf not} to be used as a base class, this constructor may be ommitted.
 *
 * The name of all exceptions should be their class name.
 *
 ****************************************************************************/
#include "fileiobase/application/bpfExceptionBase.h"


class bpfException : public bpfExceptionBase
{
public:
  /**************************************************************************
   *
   * Construct an exception with no detail message.
   *
   **************************************************************************/
  bpfException();



  /**************************************************************************
   *
   * Construct an exception with the given detail message.
   *
   **************************************************************************/
  bpfException(const bpfString& detailMessage);

  /**************************************************************************
   *
   * Destructor
   *
   **************************************************************************/
  virtual ~bpfException();

  /**************************************************************************
   *
   * Get the name and the detail message. If there is no detail message, an
   * empty bpfString is returned.
   *
   *************************************************************************/
  const bpfString& GetName() const;
  const bpfString& GetDescription() const;

protected:
  /**************************************************************************
   *
   * Construct an exception with the given name and detail message. Used
   * (only) for subclasses.
   *
   *************************************************************************/
  bpfException(const bpfString& exceptionName, const bpfString& detailMessage);

private:
  bpfString name;
  bpfString message;
};

#endif
