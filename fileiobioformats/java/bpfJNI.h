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


#ifndef BP_JNI
#define BP_JNI


#include <jni.h>
#include <string>
#include <vector>


class bpfJNI
{
public:
  static void Init(const std::string& aJVM, const std::vector<std::string>& aJars, size_t aMemLimit);

  static JNIEnv* GetEnv();
  static std::string GetVersion();
  static std::string GetConfiguration();
  static void DetachCurrentThreadFromJVM();
  static size_t GetMemLimit();
private:
  class cImpl;
};


void bpfJNISanityCheck(JNIEnv* aEnv = bpfJNI::GetEnv());
void bpfJNISanityCheck(void* aPointer, const std::string& aPointerName, JNIEnv* aEnv = bpfJNI::GetEnv());


#endif
