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



#include "fileiobioformats/application/bpfFileReaderBioformatsImplFactory.h"

#include "fileiobioformats/application/bpfFileReaderBioformats.h"
#include "fileiobase/readers/bpfFileReaderImplToImplInterface.h"

#include "fileiobioformats/java/bpfJNI.h"

#include <iostream>


bpfString ConvertString(jstring aString, JNIEnv* aEnv)
{
  const char *vChars = aEnv->GetStringUTFChars(aString, 0);
  bpfJNISanityCheck();
  bpfString vString = vChars;
  aEnv->ReleaseStringUTFChars(aString, vChars);
  bpfJNISanityCheck();

  return vString;
}

std::vector<bpfString> ConvertArrayOfStrings(jobjectArray aArrayOfStrings, JNIEnv* aEnv)
{
  std::vector<bpfString> vStringVector;

  size_t vArrayLength = aEnv->GetArrayLength(aArrayOfStrings);
  bpfJNISanityCheck();

  for (size_t i = 0; i < vArrayLength; i++) {
    jstring vJavaString = (jstring)(aEnv->GetObjectArrayElement(aArrayOfStrings, i));
    bpfString vString = ConvertString(vJavaString, aEnv);
    vStringVector.push_back(vString);
    aEnv->DeleteLocalRef(vJavaString);
    bpfJNISanityCheck();
  }

  return vStringVector;
}


bpfFileReaderBioformatsImplFactory::bpfFileReaderBioformatsImplFactory()
{

  // initialize JNI to read bioformats jar files
  std::string vJVMFolder = GetDefaultJVMPath();
  
  std::string vJarPath = "";
#if defined(_WIN32)
  vJarPath = "bioformats\\bioformats_package.jar";
#elif defined(__APPLE__)
  vJarPath = "../Frameworks/bioformats/bioformats_package.jar";
#else
  vJarPath = "bioformats/bioformats_package.jar";
#endif
  std::vector<std::string> vJars = { vJarPath };

  size_t vMemoryLimitMB = 8000;
  size_t vMemoryLimitJava = (vMemoryLimitMB * 1024 * 1024) * 0.8;
  bpfJNI::Init(vJVMFolder, vJars, vMemoryLimitJava);
}


bpfFileReaderBioformatsImplFactory::~bpfFileReaderBioformatsImplFactory()
{
}


bpfString bpfFileReaderBioformatsImplFactory::GetBioformatsVersion() const
{
  // get version number of the bioformats.jar file used here
  auto vEnv = bpfJNI::GetEnv();
  bpfJNISanityCheck();
  jclass vFormatTools(vEnv->FindClass("loci/formats/FormatTools"));
  bpfJNISanityCheck(vFormatTools, "vFormatTools");
  jfieldID vGetVersionNumber = vEnv->GetStaticFieldID(vFormatTools, "VERSION", "Ljava/lang/String;");
  bpfJNISanityCheck(vGetVersionNumber, "vGetVersionNumber");
  jstring vVersionNumber((jstring)vEnv->GetStaticObjectField(vFormatTools, vGetVersionNumber));
  bpfJNISanityCheck();
  bpfString vVersion = ConvertString(vVersionNumber, vEnv);
  vEnv->DeleteLocalRef(vVersionNumber);
  bpfJNISanityCheck();
  vEnv->DeleteLocalRef(vFormatTools);
  bpfJNISanityCheck();
  return vVersion;
}


void bpfFileReaderBioformatsImplFactory::GetSupportedBioformatsFormats(std::vector<bpfString>&  aFormatDescriptions, std::vector<std::vector<bpfString>>& aExtensions ) const
{
  auto vEnv = bpfJNI::GetEnv();
  bpfJNISanityCheck();
  //MessageBox(0, "Attach ME!", "", 0);
  
  jclass vDebugTools(vEnv->FindClass("loci/common/DebugTools"));
  bpfJNISanityCheck(vDebugTools, "vDebugTools");
  //call DebugTools.setRootLevel("OFF"); to disable logging
  jmethodID vSetRootLevelId = vEnv->GetStaticMethodID(vDebugTools, "setRootLevel", "(Ljava/lang/String;)V");
  bpfJNISanityCheck(vSetRootLevelId, "setRootLevel");
  jstring vLevel = vEnv->NewStringUTF("OFF");
  vEnv->CallStaticVoidMethod(vDebugTools, vSetRootLevelId, vLevel);
  bpfJNISanityCheck();
  vEnv->DeleteLocalRef(vDebugTools);
  bpfJNISanityCheck();
  vEnv->DeleteLocalRef(vLevel);
  bpfJNISanityCheck();

  // first read out all readers => array of IFormatReader
  // public IFormatReader[] getReaders()
  // use class loci.formats.ImageReader
  jclass vImageReader(vEnv->FindClass("loci/formats/ImageReader"));
  bpfJNISanityCheck(vImageReader, "vImageReader");

  // create new instance of ImageReader
  jmethodID vImageReaderConstructor = vEnv->GetMethodID(vImageReader, "<init>", "()V");
  bpfJNISanityCheck(vImageReaderConstructor, "vImageReaderConstructor");
  jobject vImageReaderObject(vEnv->NewObject(vImageReader, vImageReaderConstructor));
  bpfJNISanityCheck(vImageReaderObject, "vImageReaderObject");

  jmethodID vGetReaders = vEnv->GetMethodID(vImageReader, "getReaders", "()[Lloci/formats/IFormatReader;");
  bpfJNISanityCheck(vGetReaders, "vGetReaders");
  jobjectArray vReaders ((jobjectArray)vEnv->CallObjectMethod(vImageReaderObject, vGetReaders));
  bpfJNISanityCheck(vReaders, "vReaders");
  vEnv->DeleteLocalRef(vImageReader);
  bpfJNISanityCheck();
  vEnv->DeleteLocalRef(vImageReaderObject);
  bpfJNISanityCheck();

  //// use the IFormatReader class to get the methodId, because specific reader is of type IFormatReader
  jclass vIFormatReader(vEnv->FindClass("loci/formats/IFormatReader"));
  bpfJNISanityCheck(vIFormatReader, "vIFormatReader");
  jmethodID vGetSuffixes = vEnv->GetMethodID(vIFormatReader, "getSuffixes", "()[Ljava/lang/String;");
  bpfJNISanityCheck(vGetSuffixes, "vGetSuffixes");
  jmethodID vGetFormat = vEnv->GetMethodID(vIFormatReader, "getFormat", "()Ljava/lang/String;");
  bpfJNISanityCheck(vGetFormat, "vGetFormat");
  
  
  std::map<bpfString, std::vector<bpfString>> vFormatsExtensionsMap;
  // iterate over all readers and get description and file extension
  size_t vArrayLength = vEnv->GetArrayLength(vReaders);
  bpfJNISanityCheck();
  aExtensions.resize(vArrayLength);
  aFormatDescriptions.resize(vArrayLength);
  for (size_t vIndex = 0; vIndex < vArrayLength; ++vIndex) {

    jobject vReader = (jobject)(vEnv->GetObjectArrayElement(vReaders, vIndex));
    bpfJNISanityCheck(vReader, "vReader");

    jobjectArray vSuffixes((jobjectArray)vEnv->CallObjectMethod(vReader, vGetSuffixes));
    bpfJNISanityCheck(vSuffixes, "vSuffixes");
    aExtensions[vIndex] = ConvertArrayOfStrings(vSuffixes, vEnv);
    jstring vFormat((jstring)vEnv->CallObjectMethod(vReader, vGetFormat));
    bpfJNISanityCheck(vFormat, "vFormat");
    aFormatDescriptions[vIndex] = ConvertString(vFormat, vEnv);
    vEnv->DeleteLocalRef(vReader);
    bpfJNISanityCheck();
    vEnv->DeleteLocalRef(vSuffixes);
    bpfJNISanityCheck();
    vEnv->DeleteLocalRef(vFormat);
    bpfJNISanityCheck();

  }

  vEnv->DeleteLocalRef(vIFormatReader);
  bpfJNISanityCheck();
  vEnv->DeleteLocalRef(vReaders);
  bpfJNISanityCheck();

}

void bpfFileReaderBioformatsImplFactory::SetFileReaderImplVersion()
{
  if (mVersion.empty()) {
    mVersion = GetBioformatsVersion();
    bpfFileReaderImpl::SetOriginalFormatFileIOVersion("ImarisFileIOBioFormats " + mVersion);
  }
}

void  bpfFileReaderBioformatsImplFactory::SetSupportedBioformatsFormats()
{
  if (!mFormats.empty()) {
    return;
  }
  std::vector<bpfString> vFormats;
  std::vector<std::vector<bpfString>> vExtensions;
  GetSupportedBioformatsFormats(vFormats, vExtensions);
  for (bpfSize vIndex = 0; vIndex < vFormats.size(); ++vIndex) {
    const auto& vFormat = vFormats[vIndex];
    mFormats.push_back(vFormat);
    mDescriptions[vFormat] = "";
    mExtensions[vFormat] = vExtensions[vIndex];
  }
}

bpfString bpfFileReaderBioformatsImplFactory::GetFormatDescription(const bpfString& aFormat) const
{
  auto vDescriptit = mDescriptions.find(aFormat);

  if (vDescriptit != mDescriptions.end()) {
    return vDescriptit->second;
  }

  return "";
}


std::vector<bpfString> bpfFileReaderBioformatsImplFactory::GetFormatExtensions(const bpfString& aFormat) const
{
  std::vector<bpfString> vFormatExtensions;

  auto vIterator = mExtensions.find(aFormat);

  if (mExtensions.end() != vIterator) {
    vFormatExtensions = vIterator->second;

    for (auto& vFormatExtension : vFormatExtensions) {
      vFormatExtension = bpfToUpper(vFormatExtension);
    }
  }

  return vFormatExtensions;
}


bpfFileReaderImplInterface* bpfFileReaderBioformatsImplFactory::CreateFileReader(const bpfString& aFileName)
{
  return CreateFileReader(aFileName, GetFileFormat(aFileName));
}


bpfFileReaderImpl* CreateReaderImpl(const bpfString& aFileName, const bpfString& aFormatName)
{
  bpfFileReaderImpl* vReaderImpl = nullptr;

  if (!aFileName.empty()) {
    try {
      vReaderImpl = new bpfFileReaderBioformats(aFileName);
      vReaderImpl->ShouldColorRangeBeAdjustedToMinMax();
    }
    catch (...) {
      vReaderImpl = nullptr;
    }
  }
  return vReaderImpl;
}


bpfFileReaderImplInterface* bpfFileReaderBioformatsImplFactory::CreateFileReader(const bpfString& aFileName, const bpfString& aFormatName)
{
  
  SetFileReaderImplVersion();

  bpfFileReaderImpl* vReaderImpl = CreateReaderImpl(aFileName, aFormatName);
  return new bpfFileReaderImplToImplInterface(vReaderImpl);
}


bpfFileReaderBioformatsImplFactory::Iterator bpfFileReaderBioformatsImplFactory::FormatBegin()
{
  SetSupportedBioformatsFormats();
  return mFormats.begin();
}


bpfFileReaderBioformatsImplFactory::Iterator bpfFileReaderBioformatsImplFactory::FormatEnd()
{
  return mFormats.end();
}


bpfString bpfFileReaderBioformatsImplFactory::GetFileFormat(const bpfString& aFileName)
{
  Iterator vFormatIt = mFormats.begin();

  while (vFormatIt != mFormats.end()) {
    bpfFileReaderImpl* vReaderImpl = CreateReaderImpl(aFileName, *vFormatIt);

    if (vReaderImpl != nullptr) {
      delete vReaderImpl;
      return *vFormatIt;
    }

    ++vFormatIt;
  }

  return "";
}


void bpfFileReaderBioformatsImplFactory::AddPluginsFormats(const bpfString& aPluginsPath)
{
}


bpfString bpfFileReaderBioformatsImplFactory::GetVersion() const
{
  std::ostringstream vStream;
  vStream << "Bio-Formats JAR version: " << GetBioformatsVersion() << std::endl;
  vStream << "Imaris Convert Bioformats " << IMARIS_VERSION_MAJOR << "." << IMARIS_VERSION_MINOR << "." << IMARIS_VERSION_PATCH;
  return vStream.str();
}

bpfString bpfFileReaderBioformatsImplFactory::GetDefaultJVMPath()
{
#if defined(_WIN32)
  return "jre\\bin\\server";
#elif defined(__APPLE__)
  return "../Frameworks/jre.app/Contents/Home/lib/server";
#else 
  return "jre/lib/server";
#endif
}
