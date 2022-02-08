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


#include "fileiobioformats/java/bpfJNI.h"

#include "fileiobase/application/bpfException.h"

#include <iostream>
#include <memory>
#include <vector>

//#include "boost/chrono.hpp"
#include "boost/thread.hpp"
#include <boost/dll.hpp>

#if defined(_WIN32)
#define JNI_PATH_SEPARATOR ";"
#else
#define JNI_PATH_SEPARATOR ":"
#endif

class bpfJNI::cImpl
{
public:
  static JNIEnv* GetEnv()
  {
    mMutex.lock();

    if (nullptr == mJvm) {
      static auto vImpl = std::make_shared<cImpl>(&mJvmFolder, &mJarsFolder);
      mMutex.unlock();
      return mEnv;
    }
    else {
      auto vState = mJvm->GetEnv((void**)&mEnv, JNI_VERSION_1_8);

      if (JNI_EDETACHED == vState) {
        mJvm->AttachCurrentThread((void**)&mEnv, nullptr);
      }

      mMutex.unlock();
      return mEnv;
    }
  }

  static void InitEnv(const std::string& aJVM, const std::vector<std::string>& aJars, size_t aMemLimit)
  {
    mJvmFolder = aJVM;
    mJarsFolder = aJars;
    mMemLimit = aMemLimit;
  }

  static size_t GetMemLimit() {
    return mMemLimit;
  }

  static void DetachCurrentThreadFromJVM()
  {
    mDetachMutex.lock();

    if (nullptr != mJvm) {
      mJvm->DetachCurrentThread();
    }

    mDetachMutex.unlock();
  }


  ~cImpl()
  {
    if (nullptr != mJvm) {
      //mJvm->DestroyJavaVM();
    }
  }


  cImpl(const std::string* aJVM, const std::vector<std::string>* aJars)
  {
    JavaVMInitArgs vInitArgs;
    std::vector<JavaVMOption> vOptions;
    std::string vClassPath = "";
    std::string vLibPath = "";
    if (aJars && !aJars->empty()) {
      vClassPath = GetClassPathFromJars(*aJars);
      vOptions.push_back({ const_cast<char*>(vClassPath.c_str()), nullptr });
    }

    if (aJVM && !aJVM->empty()) {
      vLibPath = GetLibraryPathFromJVM(*aJVM);
      vOptions.push_back({ const_cast<char*>(vLibPath.c_str()), nullptr });
    }

    std::ostringstream vStrMemLimit;
    vStrMemLimit << (mMemLimit / (1024 * 1024));
    std::string vMaxMem = "-Xmx" + vStrMemLimit.str() + "m";
    vOptions.push_back({ const_cast<char*>(vMaxMem.c_str()), nullptr });

    vInitArgs.version = JNI_VERSION_1_8;
    vInitArgs.nOptions = static_cast<jint>(vOptions.size());
    vInitArgs.options = vOptions.data();
    vInitArgs.ignoreUnrecognized = false;

    // load and initialize a Java VM, return a JNI interface pointer in mEnv
    //boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();

#if defined(_WIN32)
    std::string jvmDLLPath = *aJVM + "\\jvm.dll";
    HINSTANCE vJvmDll = LoadLibrary(jvmDLLPath.c_str());

    if (vJvmDll == nullptr) {
      std::cout << "Error loading JVM library from " << jvmDLLPath << std::endl;
    }
    typedef jint(JNICALL *CreateJavaVM)(JavaVM **, void **, void *);
    CreateJavaVM vCreateJavaVM = (CreateJavaVM)GetProcAddress(vJvmDll, "JNI_CreateJavaVM");

    jint rc = vCreateJavaVM(&mJvm, (void**)&mEnv, &vInitArgs);
#else
    jint rc = JNI_CreateJavaVM(&mJvm, (void**)&mEnv, &vInitArgs);
#endif

    // boost::chrono::duration<double> vSeconds = (boost::chrono::high_resolution_clock::now() - start);
    // std::cout << "Time create JavaVM: " << vSeconds.count() << std::endl;
    //========================= analyse errors if any  ==============================
    // if process interuped before error is returned, it's because jvm.dll can't be
    // found, i.e.  its directory is not in the PATH.

    // todo throw exceptions instead of print to cerr
    if (rc != JNI_OK) {
      if (rc == JNI_EVERSION) {
        std::cerr << "FATAL ERROR: JVM is oudated and doesn't meet requirements" << std::endl;
      }
      else if (rc == JNI_ENOMEM) {
        std::cerr << "FATAL ERROR: not enough memory for JVM" << std::endl;
      }
      else if (rc == JNI_EINVAL) {
        std::cerr << "FATAL ERROR: invalid ragument for launching JVM" << std::endl;
      }
      else if (rc == JNI_EEXIST) {
        std::cerr << "FATAL ERROR: the process can only launch one JVM an not more" << std::endl;
      }
      else {
        std::cerr << "FATAL ERROR:  could not create the JVM instance (error code " << rc << ")" << std::endl;
      }
      std::cin.get();
      exit(EXIT_FAILURE);
    }
  }

  static const std::string& GetJVMFolderString()
  {
    return mJvmFolder;
  }

  static std::string GetJarsFolderString()
  {
    std::string vJars = "";
    for (auto& vJar : mJarsFolder) {
      vJars += vJar + JNI_PATH_SEPARATOR;
    }
    return vJars;
  }

private:
  static std::string GetClassPathFromJars(const std::vector<std::string>& aJars);
  static std::string GetLibraryPathFromJVM(const std::string& aJVM);
  static JNIEnv* mEnv; // Pointer to native interface
  static JavaVM* mJvm; // Pointer to the JVM (Java Virtual Machine)
  static std::string mJvmFolder;
  static std::vector<std::string> mJarsFolder;
  static size_t mMemLimit;
  static boost::recursive_mutex mMutex;
  static boost::recursive_mutex mDetachMutex;
};


std::string bpfJNI::cImpl::GetClassPathFromJars(const std::vector<std::string>& aJars)
{
  std::string vClassPath;
  std::string vExecutableParentPath = boost::dll::program_location().parent_path().generic_string();

  std::string vPrefix = "-Djava.class.path=";
  if (!aJars.empty()) {
    vClassPath = vExecutableParentPath;

    for (const auto& vJar : aJars) {
      vClassPath += JNI_PATH_SEPARATOR + vExecutableParentPath + "/" + vJar;
    }
#if defined(_WIN32)
    vClassPath = "\"" + vClassPath + "\"";
#endif
    vClassPath = vPrefix + vClassPath;
  }
  return vClassPath;
}

std::string bpfJNI::cImpl::GetLibraryPathFromJVM(const std::string& aJVM)
{
  std::string vLibraryPath = "";
  std::string vPrefix = "-Djava.library.path=";

  if (!aJVM.empty()) {
    vLibraryPath = aJVM;
#if defined(_WIN32)
    vLibraryPath = "\"" + vLibraryPath + "\"";
#endif
    vLibraryPath = vPrefix + vLibraryPath;
  }
  return vLibraryPath;
}

void bpfJNI::Init(const std::string& aJVM, const std::vector<std::string>& aJars, size_t aMemLimit)
{
  cImpl::InitEnv(aJVM, aJars, aMemLimit);
}

size_t bpfJNI::GetMemLimit()
{
  return cImpl::GetMemLimit();
}

JNIEnv* bpfJNI::GetEnv()
{
  return cImpl::GetEnv();
}


std::string bpfJNI::GetVersion()
{
  jint vVer = GetEnv()->GetVersion();
  std::string vVersionString = "JVM Version: " + std::to_string((vVer >> 16) & 0x0f) + "." + std::to_string(vVer & 0x0f);
  return vVersionString;
}

std::string bpfJNI::GetConfiguration()
{
  std::string vVersion = "JVM Version: " + GetVersion();
  std::string vJVMString = "JAVAVM: " + cImpl::GetJVMFolderString();
  std::string vJARString = "JARS: " + cImpl::GetJarsFolderString();
  return vVersion + "\n" + vJVMString + "\n" + vJARString;
}


void bpfJNI::DetachCurrentThreadFromJVM()
{
  cImpl::DetachCurrentThreadFromJVM();
}


void bpfJNISanityCheck(JNIEnv* aEnv)
{
  if (aEnv->ExceptionCheck()) {
    aEnv->ExceptionDescribe();
    throw bpfException("JNI exception.");
  }
}


void bpfJNISanityCheck(void* aPointer, const std::string& aPointerName, JNIEnv* aEnv)
{
  bpfJNISanityCheck(aEnv);
  if (!aPointer) {
    throw bpfException(aPointerName + " is null.");
  }
}

JNIEnv* bpfJNI::cImpl::mEnv = nullptr;
JavaVM* bpfJNI::cImpl::mJvm = nullptr;
std::string bpfJNI::cImpl::mJvmFolder;
std::vector<std::string> bpfJNI::cImpl::mJarsFolder;
size_t bpfJNI::cImpl::mMemLimit;
boost::recursive_mutex bpfJNI::cImpl::mMutex;
boost::recursive_mutex bpfJNI::cImpl::mDetachMutex;
