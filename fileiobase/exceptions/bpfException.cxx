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


#include "fileiobase/application/bpfException.h"

/*****************************************************************************
 *
 * Default Constructor
 *
 * public
 *
 ****************************************************************************/
bpfException::bpfException()
  :name("bpfException"), message("")
{}

/*****************************************************************************
 *
 * Constructor with argument for detail message.
 *
 * public
 *
 ****************************************************************************/
bpfException::bpfException(const bpfString& detailMessage)
  :name("bpfException"),
   message(detailMessage)
{}

/*****************************************************************************
 *
 * Constructor for subclasses
 *
 * protected
 *
 ****************************************************************************/
bpfException::bpfException(const bpfString& exceptionName,
                           const bpfString& detailMessage)
  :name(exceptionName),
   message(detailMessage)
{}

/*****************************************************************************
 *
 * Destructor
 *
 ****************************************************************************/
bpfException::~bpfException()
{}

/*****************************************************************************
 *
 * Get the name
 *
 ****************************************************************************/
const bpfString&
bpfException::GetName() const
{
  return name;
}

/*****************************************************************************
 *
 * Get the detail message. If no detail message was specified, an empty bpfString
 * is returned.
 *
 ****************************************************************************/
const bpfString&
bpfException::GetDescription() const
{
  return message;
}

