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


#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#pragma GCC system_header

#elif defined _MSC_VER
#pragma warning(push, 1)

#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang system_header

#endif

#ifndef Q_MOC_RUN
#include "boost/filesystem.hpp"
#endif

#if defined _MSC_VER
#pragma warning(pop)
#endif
