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


#include "bpfFileReaderBioformats.h"

#include <limits>
#include "bpfTypesUtils.h"


bpfFileReaderBioformats::bpfFileReaderBioformats(const bpfString& aFilename)
  : bpfFileReaderImpl(aFilename), mBlockNumber(0)
{
  // TODO: probably ConvertSeparators not needed after testing
  mFileName = bpfFileTools::ConvertSeparators(aFilename);
  //mFileName = aFilename;  

  // initialize bioformats file reader
  InitializeFileReader();
}


bpfFileReaderBioformats::~bpfFileReaderBioformats() 
{
  auto vEnv = bpfJNI::GetEnv();
  
  //// ImageReader.close(), return void
  //jmethodID vClose = vEnv->GetMethodID(mImageReaderClass, "close", "()V");
  //bpfJNISanityCheck(vClose, "vClose");
  //vEnv->CallObjectMethod(mImageReaderObject, vClose);
  //bpfJNISanityCheck();
  
  // DeleteGlobalRef
  vEnv->DeleteGlobalRef(mImageReaderObject);
  bpfJNISanityCheck();
  vEnv->DeleteGlobalRef(mImageReaderClass);
  bpfJNISanityCheck();
  vEnv->DeleteGlobalRef(mMetadataObject);
  bpfJNISanityCheck();
  vEnv->DeleteGlobalRef(mMetadataClass);
  bpfJNISanityCheck();
}

void bpfFileReaderBioformats::HandleMetadataOptions(JNIEnv* aEnv)
{
  jclass vDynamicMetadataOptionsClass(aEnv->FindClass("loci/formats/in/DynamicMetadataOptions"));
  bpfJNISanityCheck(vDynamicMetadataOptionsClass, "vDynamicMetadataOptionsClass");

  jmethodID vDynamicMetadataOptionsConstructor = aEnv->GetMethodID(vDynamicMetadataOptionsClass, "<init>", "()V");
  bpfJNISanityCheck(vDynamicMetadataOptionsConstructor, "vDynamicMetadataOptionsConstructor");
  jobject vDynamicMetadataOptionsObject(aEnv->NewObject(vDynamicMetadataOptionsClass, vDynamicMetadataOptionsConstructor));
  bpfJNISanityCheck(vDynamicMetadataOptionsObject, "vDynamicMetadataOptionsObject");
  //mImageReaderObject = vEnv->NewGlobalRef(vImageReaderObject);

  jclass vMetadataLevelClass(aEnv->FindClass("loci/formats/in/MetadataLevel"));
  bpfJNISanityCheck(vMetadataLevelClass, "vMetadataLevelClass");

  jfieldID vLevelField = aEnv->GetStaticFieldID(vMetadataLevelClass, "MINIMUM", "Lloci/formats/in/MetadataLevel;");
  bpfJNISanityCheck(vLevelField, "vLevelField");
  jobject vLevelObject = aEnv->GetStaticObjectField(vMetadataLevelClass, vLevelField);
  bpfJNISanityCheck(vLevelObject, "vLevelObject");

  jmethodID vSetMetadataLevel = aEnv->GetMethodID(vDynamicMetadataOptionsClass, "setMetadataLevel", "(Lloci/formats/in/MetadataLevel;)V");
  bpfJNISanityCheck(vSetMetadataLevel, "vSetMetadataLevel");
  aEnv->CallVoidMethod(vDynamicMetadataOptionsObject, vSetMetadataLevel, vLevelObject);
  //bpfJNISanityCheck(vLevelField, "vLevelField");

  /* jmethodID public void setMetadataLevel
  public void setMetadataLevel(MetadataLevel level
  */

  jmethodID vSetMetadataOptions = aEnv->GetMethodID(mImageReaderClass, "setMetadataOptions", "(Lloci/formats/in/MetadataOptions;)V");
  bpfJNISanityCheck(vSetMetadataOptions, "vSetMetadataOptions");
  aEnv->CallVoidMethod(mImageReaderObject, vSetMetadataOptions, vDynamicMetadataOptionsObject);


  //setOriginalMetadataPopulated(boolean populate) {
  jmethodID vSetOriginalMetadataPopulated = aEnv->GetMethodID(mImageReaderClass, "setOriginalMetadataPopulated", "(Z)V");
  bpfJNISanityCheck(vSetOriginalMetadataPopulated, "vSetOriginalMetadataPopulated");
  aEnv->CallVoidMethod(mImageReaderObject, vSetOriginalMetadataPopulated, 0);
  bpfJNISanityCheck();
}

void bpfFileReaderBioformats::TryOpenAsSeries(JNIEnv* aEnv)
{
  bpfJNISanityCheck();

  jstring vFilename = aEnv->NewStringUTF(mFileName.c_str());

  //loci.common.Location;
  jclass vLocationClass(aEnv->FindClass("loci/common/Location"));
  bpfJNISanityCheck(vLocationClass, "vLocationClass");
  // create new instance of Location
  jmethodID vLocationConstructor = aEnv->GetMethodID(vLocationClass, "<init>", "(Ljava/lang/String;)V");
  bpfJNISanityCheck(vLocationConstructor, "vLocationConstructor");
  jobject vLocationObject(aEnv->NewObject(vLocationClass, vLocationConstructor, vFilename));
  bpfJNISanityCheck(vLocationObject, "vLocationObject");

  //get filename pattern
  jclass vFilePattern(aEnv->FindClass("loci/formats/FilePattern"));
  bpfJNISanityCheck(vFilePattern, "vFilePattern");
  jmethodID vFilePatternConstructor = aEnv->GetMethodID(vFilePattern, "<init>", "(Lloci/common/Location;)V");
  bpfJNISanityCheck(vFilePatternConstructor, "vFilePatternConstructor");
  jobject vFilePatternObject(aEnv->NewObject(vFilePattern, vFilePatternConstructor, vLocationObject));
  bpfJNISanityCheck(vFilePatternObject, "vFilePatternObject");


  jmethodID vGetPattern = aEnv->GetMethodID(vFilePattern, "getPattern", "()Ljava/lang/String;");
  bpfJNISanityCheck(vGetPattern, "vGetPattern");
  jstring vPattern = (jstring)aEnv->CallObjectMethod(vFilePatternObject, vGetPattern);
  //bpfJNISanityCheck();
  bpfString vStringPattern = "";
  if ((void*)vPattern) {
    vStringPattern = aEnv->GetStringUTFChars(vPattern, 0);
  }

  bpfSize vNumberFiles = 0;
  if (!vStringPattern.empty()) {
    jmethodID vGetFiles = aEnv->GetMethodID(vFilePattern, "getFiles", "()[Ljava/lang/String;");
    bpfJNISanityCheck(vGetFiles, "vGetFiles");
    jobjectArray vFiles((jobjectArray)aEnv->CallObjectMethod(vFilePatternObject, vGetFiles));
    //bpfJNISanityCheck();
    if (vFiles) {
      vNumberFiles = aEnv->GetArrayLength(vFiles);
    }
  }


  if (vNumberFiles > 1) {
    
    //loci.formats.FileStitcher
    jclass vFileStitcherClass(aEnv->FindClass("loci/formats/FileStitcher"));
    bpfJNISanityCheck(vFileStitcherClass, "vFileStitcherClass");
    mImageReaderClass = (jclass)aEnv->NewGlobalRef(vFileStitcherClass);

    // create new instance of FileStitcher
    jmethodID vFileStitcherConstructor = aEnv->GetMethodID(vFileStitcherClass, "<init>", "(Lloci/formats/IFormatReader;Z)V");
    bpfJNISanityCheck(vFileStitcherConstructor, "vFileStitcherConstructor");
    jobject vFileStitcherObject(aEnv->NewObject(vFileStitcherClass, vFileStitcherConstructor, mImageReaderObject, JNI_TRUE));
    bpfJNISanityCheck(vFileStitcherObject, "vFileStitcherObject");
    mImageReaderObject = aEnv->NewGlobalRef(vFileStitcherObject);
    jmethodID vSetId = aEnv->GetMethodID(mImageReaderClass, "setId", "(Ljava/lang/String;)V");
    bpfJNISanityCheck(vSetId, "vSetId");
    aEnv->CallVoidMethod(mImageReaderObject, vSetId, vPattern);
    bpfJNISanityCheck();
  }

}

void bpfFileReaderBioformats::InitializeFileReader()
{
  auto vEnv = bpfJNI::GetEnv();
  bpfJNISanityCheck();

  // use class loci.common.DebugTools
  jclass vDebugTools(vEnv->FindClass("loci/common/DebugTools"));
  bpfJNISanityCheck(vDebugTools, "vDebugTools");
  //call DebugTools.setRootLevel("OFF"); to disable logging
  jmethodID vSetRootLevelId = vEnv->GetStaticMethodID(vDebugTools, "setRootLevel", "(Ljava/lang/String;)V");
  bpfJNISanityCheck(vSetRootLevelId, "setRootLevel");
  jstring vLevel = vEnv->NewStringUTF("OFF");
  vEnv->CallStaticVoidMethod(vDebugTools, vSetRootLevelId, vLevel);
  bpfJNISanityCheck();

  // use class loci.formats.ImageReader
  jclass vImageReader(vEnv->FindClass("loci/formats/ImageReader"));
  bpfJNISanityCheck(vImageReader, "vImageReader");
  mImageReaderClass = (jclass)vEnv->NewGlobalRef(vImageReader);

  // create new instance of ImageReader
  jmethodID vImageReaderConstructor = vEnv->GetMethodID(mImageReaderClass, "<init>", "()V");
  bpfJNISanityCheck(vImageReaderConstructor, "vImageReaderConstructor");
  jobject vImageReaderObject(vEnv->NewObject(mImageReaderClass, vImageReaderConstructor));
  bpfJNISanityCheck(vImageReaderObject, "vImageReaderObject");
  mImageReaderObject = vEnv->NewGlobalRef(vImageReaderObject);


  // Set metadata store

  // use class loci.formats.MetadataTools
  jclass vMetadataTools(vEnv->FindClass("loci/formats/MetadataTools"));
  bpfJNISanityCheck(vMetadataTools, "vMetadataTools");

  // call MetadataTools.createOMEXMLMetadata(), returns IMetadata (this inherits from MetadataStore)
  jmethodID vCreateOMEXMLMetadata = vEnv->GetStaticMethodID(vMetadataTools, "createOMEXMLMetadata", "()Lloci/formats/meta/IMetadata;");
  bpfJNISanityCheck(vCreateOMEXMLMetadata, "vCreateOMEXMLMetadata");
  jobject vMetadataObject(vEnv->CallStaticObjectMethod(vMetadataTools, vCreateOMEXMLMetadata));
  bpfJNISanityCheck(vMetadataObject, "vMetadataObject");
  mMetadataObject = vEnv->NewGlobalRef(vMetadataObject);

  // use class ome.xml.meta.IMetadata
  jclass vIMetadataClassOME(vEnv->FindClass("ome/xml/meta/IMetadata"));
  bpfJNISanityCheck(vIMetadataClassOME, "vIMetadataClassOME");
  mMetadataClass = (jclass)vEnv->NewGlobalRef(vIMetadataClassOME);

  // call ImageReader.setMetadataStore(MetadataStore store), returns void, loci.formats.meta.MetadataStore
  jmethodID vSetMetadataStore = vEnv->GetMethodID(mImageReaderClass, "setMetadataStore", "(Lloci/formats/meta/MetadataStore;)V");
  bpfJNISanityCheck(vSetMetadataStore, "vSetMetadataStore");
  vEnv->CallVoidMethod(mImageReaderObject, vSetMetadataStore, vMetadataObject);
  bpfJNISanityCheck();

  //HandleMetadataOptions(vEnv);

  // set flattened resolution to false, such that resolution levels are not interpreted as datasets
  // call ImageReader.setFlattenedResolutions(boolean flattened), return void
  jmethodID vSetFlattenedResolutions = vEnv->GetMethodID(mImageReaderClass, "setFlattenedResolutions", "(Z)V");
  bpfJNISanityCheck(vSetFlattenedResolutions, "vSetFlattenedResolutions");
  vEnv->CallVoidMethod(mImageReaderObject, vSetFlattenedResolutions, 0);
  bpfJNISanityCheck();

  jstring vFilename = vEnv->NewStringUTF(mFileName.c_str());

  jmethodID vSetId = vEnv->GetMethodID(mImageReaderClass, "setId", "(Ljava/lang/String;)V");
  bpfJNISanityCheck(vSetId, "vSetId");
  vEnv->CallVoidMethod(mImageReaderObject, vSetId, vFilename);
  bpfJNISanityCheck();

  // call ImageReader.getSeriesCount(), returns int
  jmethodID vGetSeriesCount = vEnv->GetMethodID(mImageReaderClass, "getSeriesCount", "()I");
  bpfJNISanityCheck(vGetSeriesCount, "vGetSeriesCount");
  bpfSize vNumberDataSets = static_cast<bpfSize>(vEnv->CallIntMethod(mImageReaderObject, vGetSeriesCount));
  bpfJNISanityCheck();

  if (vNumberDataSets == 1) {
    TryOpenAsSeries(vEnv);
  }  
}



// Methods from interface

std::vector<bpfString> bpfFileReaderBioformats::GetAllFileNames() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getUsedFiles() 
  jmethodID vGetUsedFiles = vEnv->GetMethodID(mImageReaderClass, "getUsedFiles", "()[Ljava/lang/String;");
  bpfJNISanityCheck(vGetUsedFiles, "vGetUsedFiles");
  jobjectArray vUsedFiles( (jobjectArray) vEnv->CallObjectMethod(mImageReaderObject, vGetUsedFiles));
  bpfJNISanityCheck(vUsedFiles, "vUsedFiles");

  std::vector<bpfString> vFileNames;
  vFileNames = ConvertArrayOfStrings(vUsedFiles, vEnv);

  UnlockImageReaderObject(vEnv);
  return vFileNames;
}


std::vector<bpfString> bpfFileReaderBioformats::GetAllFileNamesOfDataSet(const bpfString& aFileName) const
{
  try {
    return GetAllFileNames();
  }
  catch (...) {
    return {};
  }
}


bpfSize bpfFileReaderBioformats::GetNumberOfDataSets() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);  

  // call ImageReader.getSeriesCount(), returns int
  jmethodID vGetSeriesCount = vEnv->GetMethodID(mImageReaderClass, "getSeriesCount", "()I");
  bpfJNISanityCheck(vGetSeriesCount, "vGetSeriesCount");
  jint vSeriesCount(vEnv->CallIntMethod(mImageReaderObject, vGetSeriesCount));
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
  return static_cast<bpfSize>(vSeriesCount);
}

void bpfFileReaderBioformats::SetActiveDataSetIndex(bpfSize aDataSetIndex)
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.setSeries(int no), return void
  jmethodID vSetSeries = vEnv->GetMethodID(mImageReaderClass, "setSeries", "(I)V");
  bpfJNISanityCheck(vSetSeries, "vSetSeries");
  vEnv->CallVoidMethod(mImageReaderObject, vSetSeries, aDataSetIndex);
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
}

bpfSize bpfFileReaderBioformats::GetActiveDataSetIndex() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // public ImageReader.getSeries(), returns int
  jmethodID vGetSeries = vEnv->GetMethodID(mImageReaderClass, "getSeries", "()I");
  bpfJNISanityCheck(vGetSeries, "vGetSeries");
  jint vSeriesIndex(vEnv->CallIntMethod(mImageReaderObject, vGetSeries));
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
  return static_cast<bpfSize>(vSeriesIndex);
}


bpfSize bpfFileReaderBioformats::GetNumberOfResolutions() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getResolutionCount(), returns int
  jmethodID vGetResolutionCount = vEnv->GetMethodID(mImageReaderClass, "getResolutionCount", "()I");
  bpfJNISanityCheck(vGetResolutionCount, "vGetResolutionCount");
  jint vResolutionCount(vEnv->CallIntMethod(mImageReaderObject, vGetResolutionCount));
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
  return static_cast<bpfSize>(vResolutionCount);
}

bpfSize bpfFileReaderBioformats::GetActiveResolutionLevel() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getResolution(), returns int
  jmethodID vGetResolution = vEnv->GetMethodID(mImageReaderClass, "getResolution", "()I");
  bpfJNISanityCheck(vGetResolution, "vGetResolution");
  jint vResolutionIndex(vEnv->CallIntMethod(mImageReaderObject, vGetResolution));
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
  return static_cast<bpfSize>(vResolutionIndex);
}

void bpfFileReaderBioformats::SetActiveResolutionLevel(bpfSize aResolutionLevel)
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.setResolution(int no), returns void
  jmethodID vSetResolution = vEnv->GetMethodID(mImageReaderClass, "setResolution", "(I)V");
  bpfJNISanityCheck(vSetResolution, "vSetResolution");
  vEnv->CallVoidMethod(mImageReaderObject, vSetResolution, (jint) aResolutionLevel);
  bpfJNISanityCheck();

  UnlockImageReaderObject(vEnv);
}


bpfString bpfFileReaderBioformats::GetReaderDescription() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getFormat()
  jmethodID vGetFormat = vEnv->GetMethodID(mImageReaderClass, "getFormat", "()Ljava/lang/String;");
  bpfJNISanityCheck(vGetFormat, "vGetFormat");
  jstring vFormat((jstring) vEnv->CallObjectMethod(mImageReaderObject, vGetFormat));
  bpfJNISanityCheck(vFormat, "vFormat");

  bpfString vFormatString = ConvertString(vFormat, vEnv);

  UnlockImageReaderObject(vEnv);
  return vFormatString;
}


std::vector<bpfString> bpfFileReaderBioformats::GetReaderExtension() const
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getSuffixes()
  jmethodID vGetSuffixes = vEnv->GetMethodID(mImageReaderClass, "getSuffixes", "()[Ljava/lang/String;");
  bpfJNISanityCheck(vGetSuffixes, "vGetSuffixes");
  jobjectArray vSuffixes((jobjectArray)vEnv->CallObjectMethod(mImageReaderObject, vGetSuffixes));
  bpfJNISanityCheck(vSuffixes, "vSuffixes");

  std::vector<bpfString> vExtensions;
  vExtensions = ConvertArrayOfStrings(vSuffixes, vEnv);

  UnlockImageReaderObject(vEnv);
  return vExtensions;
}


bpfNumberType bpfFileReaderBioformats::GetDataType()
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  bpfString vPixelTypeString = GetPixelTypeString(vEnv);
  bpfNumberType vDataType = ConvertPixelType(vPixelTypeString, vEnv);

  UnlockImageReaderObject(vEnv);
  return vDataType;
}


std::vector<bpfFileReaderBioformats::Dimension> bpfFileReaderBioformats::GetDimensionSequence()
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getDimensionOrder(), returns e.g. XYCTZ (XY always first), always all 5 dimensions are included
  jmethodID vGetDimensionOrder = vEnv->GetMethodID(mImageReaderClass, "getDimensionOrder", "()Ljava/lang/String;");
  bpfJNISanityCheck(vGetDimensionOrder, "vGetDimensionOrder");
  jstring vDimensionOrder((jstring)vEnv->CallObjectMethod(mImageReaderObject, vGetDimensionOrder));
  bpfJNISanityCheck(vDimensionOrder, "vDimensionOrder");

  bpfString vDimensions = ConvertString(vDimensionOrder, vEnv);
  bool vIsInterleaved = IsInterleaved(vEnv);

  std::vector<Dimension> vDimensionVector;

  if (vIsInterleaved) {
    vDimensionVector.push_back(C);
  }

  for (char& vDimension : vDimensions) {
    if (vDimension == 'X') {
      vDimensionVector.push_back(X);
    }
    else if (vDimension == 'Y') {
      vDimensionVector.push_back(Y);
    }
    else if (vDimension == 'Z') {
      vDimensionVector.push_back(Z);
    }
    else if (vDimension == 'C' && !vIsInterleaved) {
      vDimensionVector.push_back(C);
    }
    else if (vDimension == 'T') {
      vDimensionVector.push_back(T);
    }
  }

  UnlockImageReaderObject(vEnv);
  return vDimensionVector;
}


std::vector<bpfSize> bpfFileReaderBioformats::GetDataSizeV()
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  auto vSeq = GetDimensionSequence();
  std::vector<bpfSize> vSize;

  for (Dimension vDimension : vSeq) {
    if (vDimension == X) {
      bpfSize vSizeX = GetSizeOfDimension("getSizeX", vEnv);
      vSize.push_back(vSizeX);
    }
    else if (vDimension == Y) {
      bpfSize vSizeY = GetSizeOfDimension("getSizeY", vEnv);
      vSize.push_back(vSizeY);
    }
    else if (vDimension == Z) {
      bpfSize vSizeZ = GetSizeOfDimension("getSizeZ", vEnv);
      vSize.push_back(vSizeZ);
    }
    else if (vDimension == T) {
      bpfSize vSizeT = GetSizeOfDimension("getSizeT", vEnv);
      vSize.push_back(vSizeT);
    }
    else if (vDimension == C) {
      bpfSize vSizeC = GetSizeOfDimension("getSizeC", vEnv);
      vSize.push_back(vSizeC);
    }
  }

  UnlockImageReaderObject(vEnv);
  return vSize;
}


std::vector<bpfSize> bpfFileReaderBioformats::GetDataBlockSizeV()
{
  // bioformats blocks are in general XY planes, except if image is interleaved or
  // if RGBChannelCount > 1

  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  auto vSeq = GetDimensionSequence();
  std::vector<bpfSize> vSize;
  for (Dimension vDim : vSeq) {
    if (vDim == X) {
      bpfSize vSizeX = GetSizeOfDimension("getSizeX", vEnv);
      vSize.push_back(vSizeX);
    }
    else if (vDim == Y) {
      bpfSize vSizeY = GetSizeOfDimension("getSizeY", vEnv);
      vSize.push_back(vSizeY);
    }
    else if (vDim == C) {
      bpfSize vRGBChannelCount = GetRGBChannelCount(vEnv);

      if (IsInterleaved(vEnv) || vRGBChannelCount > 1) {
        vSize.push_back(vRGBChannelCount);
      }
      else {
        vSize.push_back(1);
      }

    }
    else {
      vSize.push_back(1);
    }
  }

  UnlockImageReaderObject(vEnv);
  return vSize;
}


void bpfFileReaderBioformats::ReadDataBlock(void* aDataBlockMemory)
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getPixelType()
  jmethodID vGetPixelType = vEnv->GetMethodID(mImageReaderClass, "getPixelType", "()I");
  bpfJNISanityCheck(vGetPixelType, "vGetPixelType");
  jint vPixelType(vEnv->CallIntMethod(mImageReaderObject, vGetPixelType));
  bpfJNISanityCheck();

  // use class loci.formats.FormatTools
  jclass vFormatTools(vEnv->FindClass("loci/formats/FormatTools"));
  bpfJNISanityCheck(vFormatTools, "vFormatTools");

  // call FormatTools.getBytesPerPixel(int pixelType), returns int, is static
  jmethodID vGetBytesPerPixel = vEnv->GetStaticMethodID(vFormatTools, "getBytesPerPixel", "(I)I");
  bpfJNISanityCheck(vGetBytesPerPixel, "vGetBytesPerPixel");
  jint vJBytesPerPixel(vEnv->CallStaticIntMethod(vFormatTools, vGetBytesPerPixel, vPixelType));
  bpfJNISanityCheck();
  bpfSize vBytesPerPixel = static_cast<bpfSize>(vJBytesPerPixel);

  
  jsize vBufferSize = static_cast<jsize>(GetDataBlockNumberOfVoxels() * vBytesPerPixel);
  jint vBlockNumber = static_cast<jint>(mBlockNumber);

  // call ImageReader.openBytes(int no), returns byte[]
  jmethodID vOpenBytes = vEnv->GetMethodID(mImageReaderClass, "openBytes", "(I)[B");
  bpfJNISanityCheck(vOpenBytes, "vOpenBytes");
  jbyteArray vJBlockBytes((jbyteArray)vEnv->CallObjectMethod(mImageReaderObject, vOpenBytes, vBlockNumber));
  bpfJNISanityCheck(vJBlockBytes, "vJBlockBytes");

  // copy bytes from vJBlockBytes array into buffer
  vEnv->GetByteArrayRegion(vJBlockBytes, 0, vBufferSize, (jbyte*) aDataBlockMemory);
  bpfJNISanityCheck();


  // check big endian
  // call ImageReader.isLittleEndian(), return bool
  jmethodID vIsLittleEndian = vEnv->GetMethodID(mImageReaderClass, "isLittleEndian", "()Z");
  bpfJNISanityCheck(vIsLittleEndian, "vIsLittleEndian");
  jboolean vJLittleEndian(vEnv->CallBooleanMethod(mImageReaderObject, vIsLittleEndian));
  bpfJNISanityCheck();

  bool vLittleEndian = static_cast<bool>(vJLittleEndian);

  bpfNumberType vDataType = GetDataType();
  if (!vLittleEndian) {
    if (vDataType == bpfUInt16Type) {
      bpfSize vNumberOfVoxels = GetDataBlockNumberOfVoxels();
      bpfInt16* vDataBlockMemory = static_cast<bpfInt16*>(aDataBlockMemory);
      for (bpfSize vIndex = 0; vIndex < vNumberOfVoxels; vIndex++) {
        bpfSwapVal(vDataBlockMemory++);
      }
    }
    else if (vDataType == bpfFloatType) {
      bpfSize vNumberOfVoxels = GetDataBlockNumberOfVoxels();
      bpfFloat* vDataBlockMemory = static_cast<bpfFloat*>(aDataBlockMemory);
      for (bpfSize vIndex = 0; vIndex < vNumberOfVoxels; vIndex++) {
        bpfSwapVal(vDataBlockMemory++);
      }
    }
  }


  // adjust values to our data types
  // if the original format is signed, we set all negative values to 0
  // get original pixel type 
  bpfString vPixelTypeString = GetPixelTypeString(vEnv);
  
  if (vPixelTypeString == "int8") {
    //BP_TRACE_MSG(1, "bpfFileReaderICS: Converting data from signed 8bit int");
    bpfInt8* dataSigned = (bpfInt8*)aDataBlockMemory;
    bpfInt8* dataEnd = dataSigned + GetDataBlockNumberOfVoxels();
    while (dataSigned != dataEnd) {
      if (*dataSigned < 0) {
        *dataSigned = 0;
      }
      ++dataSigned;
    }
  } 
  else if (vPixelTypeString == "int16") {
    //BP_TRACE_MSG(1, "bpfFileReaderICS: Converting data from signed 16bit int");
    bpfInt16* dataSigned = (bpfInt16*)aDataBlockMemory;
    bpfInt16* dataEnd = dataSigned + GetDataBlockNumberOfVoxels();
    while (dataSigned != dataEnd) {
      if (*dataSigned < 0) {
        *dataSigned = 0;
      }
      ++dataSigned;
    }
  }
  else if (vPixelTypeString == "int32") {
    //BP_TRACE_MSG(1, "bpfFileReaderICS: Converting data from signed 32bit int to float");
    const bpfInt32* dataInt = (bpfInt32*)aDataBlockMemory;
    const bpfInt32* dataEnd = dataInt + GetDataBlockNumberOfVoxels();
    bpfFloat* dataFloat = (bpfFloat*)aDataBlockMemory;
    while (dataInt != dataEnd) {
      *dataFloat = static_cast<bpfFloat>(*dataInt);
      ++dataFloat;
      ++dataInt;
    }
  }
  else if (vPixelTypeString == "uint32") {
    //BP_TRACE_MSG(1, "bpfFileReaderICS: Converting data from signed 32bit int to float");
    const bpfUInt32* dataInt = (bpfUInt32*)aDataBlockMemory;
    const bpfUInt32* dataEnd = dataInt + GetDataBlockNumberOfVoxels();
    bpfFloat* dataFloat = (bpfFloat*)aDataBlockMemory;
    while (dataInt != dataEnd) {
      *dataFloat = static_cast<bpfFloat>(*dataInt);
      ++dataFloat;
      ++dataInt;
    }
  } 
  else if (vPixelTypeString == "double") {
    //BP_TRACE_MSG(1, "bpfFileReaderICS: Converting data from signed 32bit int to float");
    const bpfDouble* dataDouble = (bpfDouble*)aDataBlockMemory;
    const bpfDouble* dataEnd = dataDouble + GetDataBlockNumberOfVoxels();
    bpfFloat* dataFloat = (bpfFloat*)aDataBlockMemory;
    while (dataDouble != dataEnd) {
      *dataFloat = static_cast<bpfFloat>(*dataDouble);
      ++dataFloat;
      ++dataDouble;
    }
  }

  GoToNextDataBlock();

  //bpfJNI::DetachCurrentThreadFromJVM();
  //bpfJNISanityCheck();
  UnlockImageReaderObject(vEnv);
}


void bpfFileReaderBioformats::GoToDataBlock(bpfSize aBlockNumber)
{
  mBlockNumber = aBlockNumber;
}


void bpfFileReaderBioformats::GoToNextDataBlock()
{
  mBlockNumber++;
}


void bpfFileReaderBioformats::GetExtents(bpfVector3Float& aMin, bpfVector3Float& aMax)
{
  auto vEnv = bpfJNI::GetEnv();

  std::vector<bpfFloat> vScales = GetPixelScales(vEnv);

  aMin[0] = 0 - vScales[0] / 2.0f;
  aMin[1] = 0 - vScales[1] / 2.0f;
  aMin[2] = 0 - vScales[2] / 2.0f;

  aMax[0] = aMin[0] + GetDataSize(X) * vScales[0];
  aMax[1] = aMin[1] + GetDataSize(Y) * vScales[1];
  aMax[2] = aMin[2] + GetDataSize(Z) * vScales[2];
}


bool bpfFileReaderBioformats::ReadThumbnail(std::vector<bpfPackedRGBA>& aThumbnailPixels, bpfSize& aSizeX, bpfSize& aSizeY)
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);

  // call ImageReader.getThumbSizeX(), returns int
  jmethodID vGetThumbSizeX = vEnv->GetMethodID(mImageReaderClass, "getThumbSizeX", "()I");
  bpfJNISanityCheck(vGetThumbSizeX, "vGetThumbSizeX");
  jint vThumbSizeX(vEnv->CallIntMethod(mImageReaderObject, vGetThumbSizeX));
  bpfJNISanityCheck();
  aSizeX = static_cast<bpfSize>(vThumbSizeX);

  // call ImageReader.getThumbSizeY(), returns int
  jmethodID vGetThumbSizeY = vEnv->GetMethodID(mImageReaderClass, "getThumbSizeY", "()I");
  bpfJNISanityCheck(vGetThumbSizeY, "vGetThumbSizeY");
  jint vThumbSizeY(vEnv->CallIntMethod(mImageReaderObject, vGetThumbSizeY));
  bpfJNISanityCheck();
  aSizeY = static_cast<bpfSize>(vThumbSizeY);

  // call ImageReader.getPixelType()
  jmethodID vGetPixelType = vEnv->GetMethodID(mImageReaderClass, "getPixelType", "()I");
  bpfJNISanityCheck(vGetPixelType, "vGetPixelType");
  jint vPixelType(vEnv->CallIntMethod(mImageReaderObject, vGetPixelType));
  bpfJNISanityCheck();

  // use class loci.formats.FormatTools
  jclass vFormatTools(vEnv->FindClass("loci/formats/FormatTools"));
  bpfJNISanityCheck(vFormatTools, "vFormatTools");

  // call FormatTools.getBytesPerPixel(int pixelType), returns int, is static
  jmethodID vGetBytesPerPixel = vEnv->GetStaticMethodID(vFormatTools, "getBytesPerPixel", "(I)I");
  bpfJNISanityCheck(vGetBytesPerPixel, "vGetBytesPerPixel");
  jint vJBytesPerPixel(vEnv->CallStaticIntMethod(vFormatTools, vGetBytesPerPixel, vPixelType));
  bpfJNISanityCheck();
  bpfSize vBytesPerPixel = static_cast<bpfSize>(vJBytesPerPixel);

  bpfSize vNumberOfChannels = GetDataSize(C);
  bpfSize vNumberOfChannelsInBlock = GetDataBlockSize(C);
  bpfSize vNumberOfChannelBlocks = vNumberOfChannels / vNumberOfChannelsInBlock;

  bpfSize vNumberOfVoxels = aSizeX * aSizeY * vNumberOfChannels;
  bpfSize vBufferSize = vNumberOfVoxels * vBytesPerPixel;
  jsize vBlockBufferSizeJ = static_cast<jsize>(aSizeX * aSizeY * vNumberOfChannelsInBlock * vBytesPerPixel);

  std::vector<bpfUInt8> vBufferVec;
  vBufferVec.resize(vBufferSize);
  bpfUInt8* vBufferPointer = vBufferVec.data();

  bpfSize vMiddleSlice = GetDataSize(Z) / 2;

  for (bpfSize vChannelBlockIndex = 0; vChannelBlockIndex < vNumberOfChannelBlocks; vChannelBlockIndex++) {
  
    jint vBlockNumber = GetDataBlockNumber(0, 0, vMiddleSlice, vChannelBlockIndex * vNumberOfChannelsInBlock, 0);

    // call ImageReader.openThumbBytes(int no), returns byte[]
    jmethodID vOpenThumbBytes = vEnv->GetMethodID(mImageReaderClass, "openThumbBytes", "(I)[B");
    bpfJNISanityCheck(vOpenThumbBytes, "vOpenThumbBytes");
    jbyteArray vJThumbBytes((jbyteArray)vEnv->CallObjectMethod(mImageReaderObject, vOpenThumbBytes, vBlockNumber));
    bpfJNISanityCheck(vJThumbBytes, "vJThumbBytes");

    // copy bytes from vJBlockBytes array into buffer
    jsize vChannelOffset = vChannelBlockIndex * vBlockBufferSizeJ;
    vEnv->GetByteArrayRegion(vJThumbBytes, 0, vBlockBufferSizeJ, (jbyte*)vBufferPointer + vChannelOffset);
    bpfJNISanityCheck();
  }

  // map bytes to colors
  bpfNumberType vDataType = GetDataType();
  
  if (vDataType == bpfUInt8Type) {
    bpfUInt8* vUInt8Buffer = vBufferPointer;
    aThumbnailPixels.reserve(vNumberOfVoxels);
    for (bpfSize vIndex = 0; vIndex < vNumberOfVoxels; vIndex++) {
      aThumbnailPixels.push_back(bpfPackedRGBA(vUInt8Buffer[vIndex], vNumberOfChannels > 0 ? vUInt8Buffer[vBlockBufferSizeJ + vIndex] : 0, vNumberOfChannels > 1 ? vUInt8Buffer[vBlockBufferSizeJ * 2 + vIndex] : 0));
    }
  }
    
  if (vDataType == bpfUInt16Type) {
    bpfUInt16* vUInt16Buffer = reinterpret_cast<bpfUInt16*>(vBufferPointer);
    aThumbnailPixels.reserve(vNumberOfVoxels);
    for (bpfSize vIndex = 0; vIndex < vNumberOfVoxels; vIndex++) {
      // todo: adapt range
      aThumbnailPixels.push_back(bpfPackedRGBA(vUInt16Buffer[vIndex] / 255));
    }
  }
 
  if (vDataType == bpfFloatType) {
    bpfFloat* vFloatBuffer = reinterpret_cast<bpfFloat*>(vBufferPointer);
    aThumbnailPixels.reserve(vNumberOfVoxels);
    for (bpfSize vIndex = 0; vIndex < vNumberOfVoxels; vIndex++) {
      // todo: adapt range
      aThumbnailPixels.push_back(bpfPackedRGBA(static_cast<bpfUInt8>(vFloatBuffer[vIndex] * 255)));
    }
  }

  UnlockImageReaderObject(vEnv);
  return true;
}


bpfColorInfo bpfFileReaderBioformats::ReadColorInfo(bpfSize aChannel)
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);
  LockMetadataObject(vEnv);

  // Get channel base color

  tOptionalColor vChannelColor = GetChannelColor(aChannel, vEnv);
  if (vChannelColor) {
    UnlockImageReaderObject(vEnv);
    UnlockMetadataObject(vEnv);
    return bpfColorInfo(*vChannelColor);
  }
  

  // Get color table (if no base color available)

  jmethodID vIsIndexed = vEnv->GetMethodID(mImageReaderClass, "isIndexed", "()Z");
  bpfJNISanityCheck(vIsIndexed, "vIsIndexed");
  jboolean vIndexed(vEnv->CallBooleanMethod(mImageReaderObject, vIsIndexed));
  bpfJNISanityCheck();

  if (vIndexed && GetDataType() == bpfUInt8Type) {

    std::vector<bpfColor> vTable = GetColorTable8Bit(vEnv);

    if (!vTable.empty()) {
      UnlockImageReaderObject(vEnv);
      UnlockMetadataObject(vEnv);
      return bpfColorInfo(vTable);
    }
  }

  if (vIndexed && GetDataType() == bpfUInt16Type) {

    std::vector<bpfColor> vTable = GetColorTable16Bit(vEnv);

    if (!vTable.empty()) {
      UnlockImageReaderObject(vEnv);
      UnlockMetadataObject(vEnv);
      return bpfColorInfo(vTable);
    }
  }


  // Get default colors (if no base color and no color table available)

  return bpfFileReaderImpl::ReadColorInfo(aChannel);
}


bool bpfFileReaderBioformats::ShouldColorRangeBeAdjustedToMinMax()
{
  bpfNumberType vDataType = GetDataType();
  if (vDataType == bpfUInt16Type || vDataType == bpfFloatType) {
    return true;
  }
  return false;
}


bpfSectionContainer bpfFileReaderBioformats::ReadParametersImpl()
{
  auto vEnv = bpfJNI::GetEnv();
  LockImageReaderObject(vEnv);
  LockMetadataObject(vEnv);

  bpfSectionContainer vSectionContainer;
  bpfParameterSection* vCoreMetadataSection = vSectionContainer.CreateSection("Core Metadata");
  MapCoreMetadata(vCoreMetadataSection, vEnv);

  bpfParameterSection* vOriginalMetadataSection = vSectionContainer.CreateSection("Original Metadata");
  MapOriginalMetadata(vOriginalMetadataSection, vEnv);

  bpfParameterSection* vSeriesMetadataSection = vSectionContainer.CreateSection("Series Metadata");
  MapSeriesMetadata(vSeriesMetadataSection, vEnv);

 
  // image name for current image in dataset
  jint vIndex = GetActiveDataSetIndex();

  // call MetadataRetrieve .getImageName(int imageIndex), returns String
  jmethodID vGetImageName = vEnv->GetMethodID(mMetadataClass, "getImageName", "(I)Ljava/lang/String;");
  bpfJNISanityCheck(vGetImageName, "vGetImageName");
  jstring vJImageName((jstring)vEnv->CallObjectMethod(mMetadataObject, vGetImageName, vIndex));
  bpfJNISanityCheck(vJImageName, "vJImageName");

  bpfString vImageName = ConvertString(vJImageName, vEnv);
  bpfParameterSection* vImageSection = vSectionContainer.CreateSection("Image");
  vImageSection->SetParameter("Name", vImageName);


  // add channel parameters for colors

  bpfSize vChannelSize = GetEffectiveSizeC(vEnv);

  for (bpfSize vChannel = 0; vChannel < vChannelSize; vChannel++) {
    bpfParameterSection* channelSection = vSectionContainer.CreateSection("Channel " + bpfToString(vChannel));

    // call getChannelPinholeSize(int imageIndex, int channelIndex), returns Length
    jmethodID vGetChannelPinholeSize = vEnv->GetMethodID(mMetadataClass, "getChannelPinholeSize", "(II)Lome/units/quantity/Length;");
    bpfJNISanityCheck(vGetChannelPinholeSize, "vGetChannelPinholeSize");
    jobject vJPinholeSize(vEnv->CallObjectMethod(mMetadataObject, vGetChannelPinholeSize, vIndex, vChannel));
    bpfJNISanityCheck();
    if (vJPinholeSize) {
      bpfFloat vPinholeSize = GetFloatFromLength(vJPinholeSize, vEnv);
      channelSection->SetParameter("LSMPinhole", bpfToString(vPinholeSize));
    }

    // call getChannelEmissionWavelength(int imageIndex, int channelIndex), returns Length
    jmethodID vGetChannelEmissionWavelength = vEnv->GetMethodID(mMetadataClass, "getChannelEmissionWavelength", "(II)Lome/units/quantity/Length;");
    bpfJNISanityCheck(vGetChannelEmissionWavelength, "vGetChannelEmissionWavelength");
    jobject vJEmissionWavelength(vEnv->CallObjectMethod(mMetadataObject, vGetChannelEmissionWavelength, vIndex, vChannel));
    bpfJNISanityCheck();
    if (vJEmissionWavelength) {
      bpfFloat vEmissionWavelength = GetFloatFromLength(vJEmissionWavelength, vEnv);
      channelSection->SetParameter("LSMEmissionWavelength", bpfToString(vEmissionWavelength));
    }    

    // call Length	getChannelExcitationWavelength(int imageIndex, int channelIndex)
    jmethodID vGetChannelExcitationWavelength = vEnv->GetMethodID(mMetadataClass, "getChannelExcitationWavelength", "(II)Lome/units/quantity/Length;");
    bpfJNISanityCheck(vGetChannelExcitationWavelength, "vGetChannelExcitationWavelength");
    jobject vJExcitationWavelength(vEnv->CallObjectMethod(mMetadataObject, vGetChannelExcitationWavelength, vIndex, vChannel));
    bpfJNISanityCheck();
    if (vJExcitationWavelength) {
      bpfFloat vExcitationWavelength = GetFloatFromLength(vJExcitationWavelength, vEnv);
      channelSection->SetParameter("LSMExcitationWavelength", bpfToString(vExcitationWavelength));
    }
  }

  UnlockImageReaderObject(vEnv);
  UnlockMetadataObject(vEnv);
  return vSectionContainer;
}



// Local methods

bpfString bpfFileReaderBioformats::ConvertString(jstring aString, JNIEnv* aEnv) const
{
  const char *vChars = aEnv->GetStringUTFChars(aString, 0);
  bpfJNISanityCheck();
  bpfString vString = vChars;
  aEnv->ReleaseStringUTFChars(aString, vChars);
  bpfJNISanityCheck();

  return vString;
}


std::vector<bpfString> bpfFileReaderBioformats::ConvertArrayOfStrings(jobjectArray aArrayOfStrings, JNIEnv* aEnv) const
{
  std::vector<bpfString> vStringVector;

  size_t vArrayLength = aEnv->GetArrayLength(aArrayOfStrings);
  bpfJNISanityCheck();

  for (size_t i = 0; i < vArrayLength; i++) {
    jstring vJavaString = (jstring)(aEnv->GetObjectArrayElement(aArrayOfStrings, i));
    bpfJNISanityCheck(vJavaString, "vJavaString");
    const char *vChars = aEnv->GetStringUTFChars(vJavaString, 0);
    bpfJNISanityCheck();
    bpfString vString = vChars;
    vStringVector.push_back(vString);
    aEnv->ReleaseStringUTFChars(vJavaString, vChars);
    bpfJNISanityCheck();
  }

  return vStringVector;
}


bpfFloat bpfFileReaderBioformats::GetFloatFromLength(jobject aLength, JNIEnv* aEnv)
{
  // use class ome.units.quantity.Length
  jclass vLengthClass(aEnv->FindClass("ome/units/quantity/Length"));
  bpfJNISanityCheck(vLengthClass, "vLengthClass");

  // call Length.value(), returns java/lang/Number
  jmethodID vGetValue = aEnv->GetMethodID(vLengthClass, "value", "()Ljava/lang/Number;");
  bpfJNISanityCheck(vGetValue, "vGetValue");
  jobject vLengthValue(aEnv->CallObjectMethod(aLength, vGetValue));
  bpfJNISanityCheck(vLengthValue, "vLengthValue");

  // use class java/lang/Float (implements Number type)
  jclass vFloatClass(aEnv->FindClass("java/lang/Float"));
  bpfJNISanityCheck(vFloatClass, "vFloatClass");

  // call Float.floatValue(), returns float
  jmethodID vGetFloatValue = aEnv->GetMethodID(vFloatClass, "floatValue", "()F");
  bpfJNISanityCheck(vGetFloatValue, "vGetFloatValue");
  jfloat vJFloatValue(aEnv->CallFloatMethod(vLengthValue, vGetFloatValue));
  bpfJNISanityCheck();

  bpfFloat vFloatValue = static_cast<bpfFloat>(vJFloatValue);
  return vFloatValue;
}


bpfSize bpfFileReaderBioformats::GetSizeOfDimension(const bpfString& aMethodName, JNIEnv* aEnv)
{
  // call ImageReader.getSizeX(), getSizeY(), getSizeZ(), getSizeC(), getSizeT()
  jmethodID vGetSizeOfDim = aEnv->GetMethodID(mImageReaderClass, aMethodName.c_str(), "()I");
  bpfJNISanityCheck(vGetSizeOfDim, "vGetSizeOfDim");
  jint vSizeOfDim(aEnv->CallIntMethod(mImageReaderObject, vGetSizeOfDim));
  bpfJNISanityCheck();
  return static_cast<bpfSize>(vSizeOfDim);
}


bpfSize bpfFileReaderBioformats::GetEffectiveSizeC(JNIEnv* aEnv)
{
  // call ImageReader.getEffectiveSizeC(), returns int
  jmethodID vGetEffectiveSizeC = aEnv->GetMethodID(mImageReaderClass, "getEffectiveSizeC", "()I");
  bpfJNISanityCheck(vGetEffectiveSizeC, "vGetEffectiveSizeC");
  jint vEffectiveSizeC(aEnv->CallIntMethod(mImageReaderObject, vGetEffectiveSizeC));
  bpfJNISanityCheck();
  return static_cast<bpfSize>(vEffectiveSizeC);
}


std::vector<bpfFloat> bpfFileReaderBioformats::GetPixelScales(JNIEnv* aEnv)
{
  bpfFloat vScaleX = GetPixelsPhysicalSize("getPixelsPhysicalSizeX", aEnv);
  bpfFloat vScaleY = GetPixelsPhysicalSize("getPixelsPhysicalSizeY", aEnv);
  bpfFloat vScaleZ;

  if (GetDataSize(Z) > 1) {
    vScaleZ = GetPixelsPhysicalSize("getPixelsPhysicalSizeZ", aEnv);
  }
  else {
    vScaleZ = (vScaleX + vScaleY) / 2.0f;
  }

  std::vector<bpfFloat> vScales = { vScaleX , vScaleY, vScaleZ };
  return vScales;
}


bpfFloat bpfFileReaderBioformats::GetPixelsPhysicalSize(const bpfString& aMethodName, JNIEnv* aEnv)
{
  LockMetadataObject(aEnv);

  // call IMetadata.getPixelsPhysicalSizeX(int imageIndex), return ome.units.quantity.Length
  jmethodID vGetPixelsPhysicalSize = aEnv->GetMethodID(mMetadataClass, aMethodName.c_str(), "(I)Lome/units/quantity/Length;");
  bpfJNISanityCheck(vGetPixelsPhysicalSize, "vGetPixelsPhysicalSize");
  jobject vPixelsPhysicalSize(aEnv->CallObjectMethod(mMetadataObject, vGetPixelsPhysicalSize, 0));
  try {
    bpfJNISanityCheck(vPixelsPhysicalSize, "vPixelsPhysicalSize");
  }
  catch (...) {
    UnlockMetadataObject(aEnv);
    return 1;
  }

  bpfFloat vScale = GetFloatFromLength(vPixelsPhysicalSize, aEnv);

  UnlockMetadataObject(aEnv);
  return vScale != 0 ? vScale : 1;
}


bpfString bpfFileReaderBioformats::GetPixelsPhysicalSizeString(const bpfString& aMethodName, JNIEnv* aEnv)
{
  // call IMetadata.getPixelsPhysicalSizeX(int imageIndex), return ome.units.quantity.Length
  jmethodID vGetPixelsPhysicalSize = aEnv->GetMethodID(mMetadataClass, aMethodName.c_str(), "(I)Lome/units/quantity/Length;");
  bpfJNISanityCheck(vGetPixelsPhysicalSize, "vGetPixelsPhysicalSize");
  jobject vPixelsPhysicalSize(aEnv->CallObjectMethod(mMetadataObject, vGetPixelsPhysicalSize, 0));
  bpfJNISanityCheck();

  if (vPixelsPhysicalSize) {
    bpfFloat vScale = GetFloatFromLength(vPixelsPhysicalSize, aEnv);
    return bpfToString(vScale);
  }
  else {
    return "";
  }
}


bpfNumberType bpfFileReaderBioformats::ConvertPixelType(bpfString aPixelType, JNIEnv* aEnv)
{
  if (aPixelType == "int8" || aPixelType == "uint8") {
    return bpfUInt8Type;
  }
  else if (aPixelType == "int16" || aPixelType == "uint16") {
    return bpfUInt16Type;
  }
  else if (aPixelType == "float" || aPixelType == "int32" || aPixelType == "uint32" || aPixelType == "double") {
    return bpfFloatType;
  }
  else {
    return bpfNoType;
  }
}


bpfString bpfFileReaderBioformats::GetPixelTypeString(JNIEnv* aEnv)
{
  // call ImageReader.getPixelType()
  jmethodID vGetPixelType = aEnv->GetMethodID(mImageReaderClass, "getPixelType", "()I");
  bpfJNISanityCheck(vGetPixelType, "vGetPixelType");
  jint vPixelTypeInt(aEnv->CallIntMethod(mImageReaderObject, vGetPixelType));
  bpfJNISanityCheck();

  // use class loci.formats.FormatTools
  jclass vFormatTools(aEnv->FindClass("loci/formats/FormatTools"));
  bpfJNISanityCheck(vFormatTools, "vFormatTools");

  // call FormatTools.getPixelTypeString(int pixelType)
  jmethodID vGetPixelTypeString = aEnv->GetStaticMethodID(vFormatTools, "getPixelTypeString", "(I)Ljava/lang/String;");
  bpfJNISanityCheck(vGetPixelTypeString, "vGetPixelTypeString");
  jstring vPixelTypeString((jstring)aEnv->CallStaticObjectMethod(vFormatTools, vGetPixelTypeString, vPixelTypeInt));
  bpfJNISanityCheck(vPixelTypeString, "vPixelTypeString");

  bpfString vPixelType = ConvertString(vPixelTypeString, aEnv);
  return vPixelType;
}


bpfFileReaderBioformats::tOptionalColor bpfFileReaderBioformats::GetChannelColor(bpfSize aChannel, JNIEnv* aEnv)
{
  bpfSize vEffectiveSizeC = GetEffectiveSizeC(aEnv);

  if (aChannel < vEffectiveSizeC) {
    jint vDataSetIndex = GetActiveDataSetIndex();

    // call MetadataRetrieve.getChannelColor(int imageIndex, int channelIndex), returns ome.xml.model.primitives.Color
    jmethodID vGetChannelColor = aEnv->GetMethodID(mMetadataClass, "getChannelColor", "(II)Lome/xml/model/primitives/Color;");
    bpfJNISanityCheck(vGetChannelColor, "vGetChannelColor");
    jobject vJChannelColor(aEnv->CallObjectMethod(mMetadataObject, vGetChannelColor, vDataSetIndex, aChannel));
    bpfJNISanityCheck();

    if (vJChannelColor) {
      bpfColor vChannelColor = ConvertColor(vJChannelColor, aEnv);
      return bpfMakeUniquePtr<bpfColor>(vChannelColor);
    }
  }

  return tOptionalColor();
}


bpfColor bpfFileReaderBioformats::ConvertColor(jobject aJColor, JNIEnv* aEnv)
{
  // use class ome.xml.model.primitives.Color
  jclass vColorClass(aEnv->FindClass("ome/xml/model/primitives/Color"));
  bpfJNISanityCheck(vColorClass, "vColorClass");

  // call Color.getRed(), returns int
  jmethodID vGetRed = aEnv->GetMethodID(vColorClass, "getRed", "()I");
  bpfJNISanityCheck(vGetRed, "vGetRed");
  jint vJRed(aEnv->CallIntMethod(aJColor, vGetRed));
  bpfJNISanityCheck();

  // call Color.getGreen(), returns int
  jmethodID vGetGreen = aEnv->GetMethodID(vColorClass, "getGreen", "()I");
  bpfJNISanityCheck(vGetGreen, "vGetGreen");
  jint vJGreen(aEnv->CallIntMethod(aJColor, vGetGreen));
  bpfJNISanityCheck();

  // call Color.getBlue(), returns int
  jmethodID vGetBlue = aEnv->GetMethodID(vColorClass, "getBlue", "()I");
  bpfJNISanityCheck(vGetBlue, "vGetBlue");
  jint vJBlue(aEnv->CallIntMethod(aJColor, vGetBlue));
  bpfJNISanityCheck();

  bpfColor vColor(vJRed / 255.0f, vJGreen / 255.0f, vJBlue / 255.0f);
  return vColor;
}


std::vector<bpfColor> bpfFileReaderBioformats::GetColorTable8Bit(JNIEnv* aEnv)
{
  std::vector<bpfColor> vTable;

  jmethodID vGet8BitLookupTable = aEnv->GetMethodID(mImageReaderClass, "get8BitLookupTable", "()[[B");
  bpfJNISanityCheck(vGet8BitLookupTable, "vGet8BitLookupTable");
  jobjectArray vLookupTable((jobjectArray)aEnv->CallObjectMethod(mImageReaderObject, vGet8BitLookupTable));
  bpfJNISanityCheck();

  if (vLookupTable) {
    size_t vArrayLength = aEnv->GetArrayLength(vLookupTable);
    bpfJNISanityCheck();

    bpfFloat vMax = (std::numeric_limits<bpfUInt8>::max)();

    if (vArrayLength == 3) {
      // red
      jbyteArray vByteArrayRed = (jbyteArray)(aEnv->GetObjectArrayElement(vLookupTable, 0));
      bpfJNISanityCheck(vByteArrayRed, "vByteArrayRed");
      size_t vByteArrayLength = aEnv->GetArrayLength(vByteArrayRed);
      jbyte *vByteElementsRed = aEnv->GetByteArrayElements(vByteArrayRed, 0);
      bpfJNISanityCheck();

      // green
      jbyteArray vByteArrayGreen = (jbyteArray)(aEnv->GetObjectArrayElement(vLookupTable, 1));
      bpfJNISanityCheck(vByteArrayGreen, "vByteArrayGreen");
      jbyte *vByteElementsGreen = aEnv->GetByteArrayElements(vByteArrayGreen, 0);
      bpfJNISanityCheck();

      // blue
      jbyteArray vByteArrayBlue = (jbyteArray)(aEnv->GetObjectArrayElement(vLookupTable, 2));
      bpfJNISanityCheck(vByteArrayBlue, "vByteArrayBlue");
      jbyte *vByteElementsBlue = aEnv->GetByteArrayElements(vByteArrayBlue, 0);
      bpfJNISanityCheck();

      for (size_t i = 0; i < vByteArrayLength; i++) {
        bpfColor vRGBColor((bpfUInt8)vByteElementsRed[i] / vMax, (bpfUInt8)vByteElementsGreen[i] / vMax, (bpfUInt8)vByteElementsBlue[i] / vMax);
        vTable.push_back(vRGBColor);
      }

      aEnv->ReleaseByteArrayElements(vByteArrayRed, vByteElementsRed, 0);
      bpfJNISanityCheck();
      aEnv->ReleaseByteArrayElements(vByteArrayGreen, vByteElementsGreen, 0);
      bpfJNISanityCheck();
      aEnv->ReleaseByteArrayElements(vByteArrayBlue, vByteElementsBlue, 0);
      bpfJNISanityCheck();
    }
  }

  return vTable;
}


std::vector<bpfColor> bpfFileReaderBioformats::GetColorTable16Bit(JNIEnv* aEnv)
{
  std::vector<bpfColor> vTable;

  // call ImageReader.get16BitLookupTable(), returns short[][]
  jmethodID vGet16BitLookupTable = aEnv->GetMethodID(mImageReaderClass, "get16BitLookupTable", "()[[S");
  bpfJNISanityCheck(vGet16BitLookupTable, "vGet16BitLookupTable");
  jobjectArray vLookupTable((jobjectArray)aEnv->CallObjectMethod(mImageReaderObject, vGet16BitLookupTable));
  bpfJNISanityCheck();

  if (vLookupTable) {
    size_t vArrayLength = aEnv->GetArrayLength(vLookupTable);
    bpfJNISanityCheck();

    bpfFloat vMax = (std::numeric_limits<bpfUInt16>::max)();

    if (vArrayLength == 3) {
      // red
      jshortArray vShortArrayRed = (jshortArray)(aEnv->GetObjectArrayElement(vLookupTable, 0));
      bpfJNISanityCheck(vShortArrayRed, "vShortArrayRed");
      size_t vShortArrayLength = aEnv->GetArrayLength(vShortArrayRed);
      jshort *vShortElementsRed = aEnv->GetShortArrayElements(vShortArrayRed, 0);
      bpfJNISanityCheck();

      // green
      jshortArray vShortArrayGreen = (jshortArray)(aEnv->GetObjectArrayElement(vLookupTable, 1));
      bpfJNISanityCheck(vShortArrayGreen, "vShortArrayGreen");
      jshort *vShortElementsGreen = aEnv->GetShortArrayElements(vShortArrayGreen, 0);
      bpfJNISanityCheck();

      // blue
      jshortArray vShortArrayBlue = (jshortArray)(aEnv->GetObjectArrayElement(vLookupTable, 2));
      bpfJNISanityCheck(vShortArrayBlue, "vShortArrayBlue");
      jshort *vShortElementsBlue = aEnv->GetShortArrayElements(vShortArrayBlue, 0);
      bpfJNISanityCheck();

      bpfSize vTableSize = 256;
      
      for (size_t i = 0; i < vTableSize; i++) {
        bpfSize j = i * (vShortArrayLength - 1) / (vTableSize - 1);
        bpfColor vRGBColor((bpfUInt16)vShortElementsRed[j] / vMax, (bpfUInt16)vShortElementsGreen[j] / vMax, (bpfUInt16)vShortElementsBlue[j] / vMax);
        vTable.push_back(vRGBColor);
      }

      aEnv->ReleaseShortArrayElements(vShortArrayRed, vShortElementsRed, 0);
      bpfJNISanityCheck();
      aEnv->ReleaseShortArrayElements(vShortArrayGreen, vShortElementsGreen, 0);
      bpfJNISanityCheck();
      aEnv->ReleaseShortArrayElements(vShortArrayBlue, vShortElementsBlue, 0);
      bpfJNISanityCheck();
    }
  }

  return vTable;
}


bpfSize bpfFileReaderBioformats::GetRGBChannelCount(JNIEnv* aEnv)
{
  // call ImageReader.getRGBChannelCount() 
  jmethodID vGetRGBChannelCount = aEnv->GetMethodID(mImageReaderClass, "getRGBChannelCount", "()I");
  bpfJNISanityCheck(vGetRGBChannelCount, "vGetRGBChannelCount");
  jint vJRGBChannelCount(aEnv->CallIntMethod(mImageReaderObject, vGetRGBChannelCount));
  bpfJNISanityCheck();
  bpfSize vRGBChannelCount = static_cast<bpfSize>(vJRGBChannelCount);

  return vRGBChannelCount;
}


bool bpfFileReaderBioformats::IsInterleaved(JNIEnv* aEnv)
{
  // call  ImageReader.isInterleaved(), returns boolean
  jmethodID vGetIsInterleaved = aEnv->GetMethodID(mImageReaderClass, "isInterleaved", "()Z");
  bpfJNISanityCheck(vGetIsInterleaved, "vGetIsInterleaved");
  jboolean vIsInterleaved(aEnv->CallBooleanMethod(mImageReaderObject, vGetIsInterleaved));
  bpfJNISanityCheck();

  return vIsInterleaved;
}


void bpfFileReaderBioformats::MapCoreMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv)
{
  // call ImageReader.getBitsPerPixel(), returns int
  jmethodID vGetBitsPerPixel = aEnv->GetMethodID(mImageReaderClass, "getBitsPerPixel", "()I");
  bpfJNISanityCheck(vGetBitsPerPixel, "vGetBitsPerPixel");
  jint vBitsPerPixel(aEnv->CallIntMethod(mImageReaderObject, vGetBitsPerPixel));
  bpfJNISanityCheck();
  aParameterSection->SetParameter("BitsPerPixel", bpfToString((int)vBitsPerPixel));

  // call ImageReader.getDimensionOrder(), returns java.lang.String
  jmethodID vGetDimensionOrder = aEnv->GetMethodID(mImageReaderClass, "getDimensionOrder", "()Ljava/lang/String;");
  bpfJNISanityCheck(vGetDimensionOrder, "vGetDimensionOrder");
  jstring vJDimensionOrder((jstring)aEnv->CallObjectMethod(mImageReaderObject, vGetDimensionOrder));
  bpfJNISanityCheck(vJDimensionOrder, "vJDimensionOrder");

  bpfString vDimensionOrder = ConvertString(vJDimensionOrder, aEnv);
  aParameterSection->SetParameter("DimensionOrder", vDimensionOrder);

  // call ImageReader.isLittleEndian(), return boolean
  jmethodID vIsLittleEndian = aEnv->GetMethodID(mImageReaderClass, "isLittleEndian", "()Z");
  bpfJNISanityCheck(vIsLittleEndian, "vIsLittleEndian");
  jboolean vJLittleEndian(aEnv->CallBooleanMethod(mImageReaderObject, vIsLittleEndian));
  bpfJNISanityCheck();
  bool vLittleEndian = vJLittleEndian ? true : false;
  aParameterSection->SetParameter("LittleEndian", bpfToString(vLittleEndian));

  bool vIsInterleaved = IsInterleaved(aEnv) ? true : false;
  aParameterSection->SetParameter("IsInterleaved", bpfToString(vIsInterleaved));

  bpfString vPixelType = GetPixelTypeString(aEnv);
  aParameterSection->SetParameter("PixelType", vPixelType);

  bpfSize vSizeX = GetSizeOfDimension("getSizeX", aEnv);
  aParameterSection->SetParameter("SizeX", bpfToString(vSizeX));

  bpfSize vSizeY = GetSizeOfDimension("getSizeY", aEnv);
  aParameterSection->SetParameter("SizeY", bpfToString(vSizeY));

  bpfSize vSizeZ = GetSizeOfDimension("getSizeZ", aEnv);
  aParameterSection->SetParameter("SizeZ", bpfToString(vSizeZ));

  bpfSize vSizeT = GetSizeOfDimension("getSizeT", aEnv);
  aParameterSection->SetParameter("SizeT", bpfToString(vSizeT));

  bpfSize vSizeC = GetSizeOfDimension("getSizeC", aEnv);
  aParameterSection->SetParameter("SizeC", bpfToString(vSizeC));
}


void bpfFileReaderBioformats::MapOriginalMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv)
{
  // call ImageReader.getGlobalMetadata(), returns Hashtable<String,Object>
  jmethodID vGetGlobalMetadata = aEnv->GetMethodID(mImageReaderClass, "getGlobalMetadata", "()Ljava/util/Hashtable;");
  bpfJNISanityCheck(vGetGlobalMetadata, "vGetGlobalMetadata");
  jobject vGlobalMetadata(aEnv->CallObjectMethod(mImageReaderObject, vGetGlobalMetadata));
  bpfJNISanityCheck(vGlobalMetadata, "vGlobalMetadata");

  MapMetadataParameters(vGlobalMetadata, aParameterSection, aEnv);
}


void bpfFileReaderBioformats::MapSeriesMetadata(bpfParameterSection* aParameterSection, JNIEnv* aEnv)
{
  // ImageReader.getSeriesMetadata(), returns java.util.Hashtable<java.lang.String, java.lang.Object>
  jmethodID vGetSeriesMetadata = aEnv->GetMethodID(mImageReaderClass, "getSeriesMetadata", "()Ljava/util/Hashtable;");
  bpfJNISanityCheck(vGetSeriesMetadata, "vGetSeriesMetadata");
  jobject vSeriesMetadata(aEnv->CallObjectMethod(mImageReaderObject, vGetSeriesMetadata));
  bpfJNISanityCheck(vSeriesMetadata, "vSeriesMetadata");

  MapMetadataParameters(vSeriesMetadata, aParameterSection, aEnv);
}


void bpfFileReaderBioformats::MapMetadataParameters(jobject aMetadata, bpfParameterSection* aParameterSection, JNIEnv* aEnv)
{
  // use class java.utils.Hashtable<K,V>
  jclass vJHashtable(aEnv->FindClass("java/util/Hashtable"));
  bpfJNISanityCheck(vJHashtable, "vJHashtable");

  // map hashtable content to map
  // get Hashtable.keySet(), then use Set.toArray() to get array with all keys
  // then iterate through keys and use use Hashtable.get(Object key), returns value
  // convert returned object into string with toString method

  // call Hashtable.keySet(), returns Set<K>, java.util.Set<E>
  jmethodID vGetKeySet = aEnv->GetMethodID(vJHashtable, "keySet", "()Ljava/util/Set;");
  bpfJNISanityCheck(vGetKeySet, "vGetKeySet");
  jobject vGlobalMetadataKeySet(aEnv->CallObjectMethod(aMetadata, vGetKeySet));
  bpfJNISanityCheck(vGlobalMetadataKeySet, "vGlobalMetadataKeySet");

  // use class java.util.Set<E>
  jclass vJSet(aEnv->FindClass("java/util/Set"));
  bpfJNISanityCheck(vJSet, "vJSet");

  // call Set.toArray(), returns Object[] (keySet is array of Strings for Global Metadata)
  jmethodID vToArray = aEnv->GetMethodID(vJSet, "toArray", "()[Ljava/lang/Object;");
  bpfJNISanityCheck(vToArray, "vToArray");
  jobjectArray vGlobalMetadataKeyArray((jobjectArray)aEnv->CallObjectMethod(vGlobalMetadataKeySet, vToArray));
  bpfJNISanityCheck(vGlobalMetadataKeyArray, "vGlobalMetadataKeyArray");

  // use class java.lang.Object
  jclass vObjectClass(aEnv->FindClass("java/lang/Object"));
  bpfJNISanityCheck(vObjectClass, "vObjectClass");

  bpfSize vArrayLength = aEnv->GetArrayLength(vGlobalMetadataKeyArray);
  bpfJNISanityCheck();
  for (bpfSize vIndex = 0; vIndex < vArrayLength; vIndex++) {
    jstring vGlobalMetadataKey = (jstring)aEnv->GetObjectArrayElement(vGlobalMetadataKeyArray, vIndex);
    bpfJNISanityCheck(vGlobalMetadataKey, "vGlobalMetadataKey");

    bpfString vKey = ConvertString(vGlobalMetadataKey, aEnv);

    // call Hashtable.get(Object key), returns V (java Object)
    jmethodID vGetMetadataValue = aEnv->GetMethodID(vJHashtable, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    bpfJNISanityCheck(vGetMetadataValue, "vGetMetadataValue");
    jobject vGlobalMetadataValue(aEnv->CallObjectMethod(aMetadata, vGetMetadataValue, vGlobalMetadataKey));
    bpfJNISanityCheck(vGlobalMetadataValue, "vGlobalMetadataValue");

    // call Object.toString(), returns String
    jmethodID vToString = aEnv->GetMethodID(vObjectClass, "toString", "()Ljava/lang/String;");
    bpfJNISanityCheck(vToString, "vToString");
    jstring vMetadataValueString((jstring)aEnv->CallObjectMethod(vGlobalMetadataValue, vToString));
    bpfJNISanityCheck(vMetadataValueString, "vMetadataValueString");

    bpfString vValue = ConvertString(vMetadataValueString, aEnv);

    aParameterSection->SetParameter(vKey, vValue);
  }
}


void bpfFileReaderBioformats::LockImageReaderObject(JNIEnv* aEnv) const 
{
  //jint vIsFree = -1;
  //while (vIsFree != 0) {
  //  vIsFree = aEnv->MonitorEnter(mImageReaderObject);
  //  bpfJNISanityCheck();
  //}
}

void bpfFileReaderBioformats::UnlockImageReaderObject(JNIEnv* aEnv) const
{
  //aEnv->MonitorExit(mImageReaderObject);
  //bpfJNISanityCheck();
}

void bpfFileReaderBioformats::LockMetadataObject(JNIEnv* aEnv) const
{
  //jint vMetadataIsFree = -1;
  //while (vMetadataIsFree != 0) {
  //  vMetadataIsFree = aEnv->MonitorEnter(mMetadataObject);
  //  bpfJNISanityCheck();
  //}  
}

void bpfFileReaderBioformats::UnlockMetadataObject(JNIEnv* aEnv) const
{
  //aEnv->MonitorExit(mMetadataObject);
  //bpfJNISanityCheck();
}