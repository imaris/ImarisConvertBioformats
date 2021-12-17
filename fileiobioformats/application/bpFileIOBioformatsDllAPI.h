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


#ifndef __BPFILEIOBIOFORMATSDLLAPI__
#define __BPFILEIOBIOFORMATSDLLAPI__


#include "fileiobioformats/application/bpDllAPI.h"


// For unit tests we build a static lib
#ifdef BP_UNIT_TEST
#define BP_FILEIOBIOFORMATS_DLL_API
#else

#ifdef FILEIOBIOFORMATS_DLL
#define BP_FILEIOBIOFORMATS_DLL_API BP_DLL_IMPORT
#else
#define BP_FILEIOBIOFORMATS_DLL_API BP_DLL_EXPORT
#endif

#endif

#endif
