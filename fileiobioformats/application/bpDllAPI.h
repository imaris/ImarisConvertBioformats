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


#ifndef __BP_DLL_API_H__
#define __BP_DLL_API_H__

// DLL stuff
#ifdef _WIN32
#ifndef BP_DLL_EXPORT
#define BP_DLL_EXPORT __declspec( dllexport )
#endif
#ifndef BP_DLL_IMPORT
#define BP_DLL_IMPORT __declspec( dllimport )
#endif
#else //_WIN32
#ifndef BP_DLL_EXPORT
#define BP_DLL_EXPORT __attribute__((visibility("default")))
#endif
#ifndef BP_DLL_IMPORT
#define BP_DLL_IMPORT __attribute__((visibility("default")))
#endif
#endif


#endif // __BP_DLL_API_H__
