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


#ifndef __BP_CONVERTER_VERSION__
#define __BP_CONVERTER_VERSION__


#define BP_COMPANY_NAME_STR    "Bitplane"
#define BP_COPYRIGHT_STR       "Copyright (c) 2021"
#define BP_COPYRIGHT_FULL_STR  "Copyright (c) 1993-2021, Bitplane AG"

#define BP_STRINGIFY(x) #x
#define BP_TO_STRING(x) BP_STRINGIFY(x)

#ifndef BP_REVISION_NUMBER
#define BP_REVISION_NUMBER     00000
#endif
#define BP_REVISION_NUMBER_STR BP_TO_STRING(BP_REVISION_NUMBER)

#if defined(_WIN64) && defined(_WIN32)
#define BP_ARCHITECTURE_STR "x64"
#elif !defined(_WIN64) && defined(_WIN32)
#define BP_ARCHITECTURE_STR "Win32"
#elif defined(__APPLE__) && defined(__ppc64__)
#define BP_ARCHITECTURE_STR "ppc64"
#elif defined(__APPLE__) && defined(__ppc__)
#define BP_ARCHITECTURE_STR "ppc"
#elif defined(__APPLE__) && defined(__i386__)
#define BP_ARCHITECTURE_STR "x86"
#elif defined(__APPLE__) && defined(__x86_64__)
#define BP_ARCHITECTURE_STR "x86_64"
#elif defined(__linux__) && defined(__i386__)
//LINUX TRANSFER DUMMY
#define BP_ARCHITECTURE_STR "linux-i386"
#elif defined(__linux__) && defined(__x86_64__)
//LINUX TRANSFER DUMMY
#define BP_ARCHITECTURE_STR "linux-amd64"
#else
#error You are working on an unknown or unsupported architecture.
#endif

////////////////////////////////////////////////////////////////////////////////
// Version number
//

#define IMARISCONVERT_APPLICATION_NAME_STR  "Imaris Convert"
//#define IMARISCONVERT_VERSION_MAJOR         10
//#define IMARISCONVERT_VERSION_MINOR         0
//#define IMARISCONVERT_VERSION_PATCH         0
//#define IMARISCONVERT_VERSION_BUILD_STR     "" // with leading space: " prealpha", " alpha 0", " beta 0", "", ...

#define IMARISCONVERT_VERSION_MAJOR_STR BP_TO_STRING(IMARISCONVERT_VERSION_MAJOR)
#define IMARISCONVERT_VERSION_MINOR_STR BP_TO_STRING(IMARISCONVERT_VERSION_MINOR)
#define IMARISCONVERT_VERSION_PATCH_STR BP_TO_STRING(IMARISCONVERT_VERSION_PATCH)
#ifndef IMARISCONVERT_VERSION_BUILD
#define IMARISCONVERT_VERSION_BUILD_STR ""
#else
#define IMARISCONVERT_VERSION_BUILD_STR BP_TO_STRING(IMARISCONVERT_VERSION_BUILD)
#endif



////////////////////////////////////////////////////////////////////////////////
// application name
//

#if defined(_WIN64)
#define IMARISCONVERT_APPLICATION_NAME IMARISCONVERT_APPLICATION_NAME_STR " x64"
#elif defined(_WIN32)
#define IMARISCONVERT_APPLICATION_NAME IMARISCONVERT_APPLICATION_NAME_STR
#else
#define IMARISCONVERT_APPLICATION_NAME IMARISCONVERT_APPLICATION_NAME_STR
#endif

#endif
