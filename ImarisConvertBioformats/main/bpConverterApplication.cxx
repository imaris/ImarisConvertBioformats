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


#include "bpConverterApplication.h"

#include "../src/bpConverterVersion.h"
#include "bpConverter.h"
#include "bpLogger.h"

#include "../meta/bpUtils.h"
#include "../meta/bpFileTools.h"
#include "../meta/bpFileReaderImpl.h"
#include "../meta/bpFileReaderFactoryFileIO.h"
#include "../meta/bpFileReaderScene.h"

#include "fileiobase/types/bpfSmartPtr.h"


#include <hdf5.h>

#include <iostream>


void bpConverterApplication::LogMessage(const bpString& aMessage) const
{
  bpLogger::LogInfo(aMessage);
}


void bpConverterApplication::SetColorHint(const bpString& aColorHint)
{
  bpColorHint vColorHint;
  bpFromString(aColorHint, vColorHint);
  bpFileReaderImpl::SetColorHint(vColorHint);
}


void bpConverterApplication::SetDefaultColors(const bpString& aColor0, const bpString& aColor1, const bpString& aColor2, const bpString& aColor3)
{
  auto vColor0 = bpFromString<bpColor>(aColor0);

  if (bpColor(0.0f, 0.0f, 0.0f) == vColor0) {
    vColor0 = bpFileConversionColorsConstant::mDefaultColor0;
  }

  auto vColor1 = bpFromString<bpColor>(aColor1);

  if (bpColor(0.0f, 0.0f, 0.0f) == vColor1) {
    vColor1 = bpFileConversionColorsConstant::mDefaultColor1;
  }

  auto vColor2 = bpFromString<bpColor>(aColor2);

  if (bpColor(0.0f, 0.0f, 0.0f) == vColor2) {
    vColor2 = bpFileConversionColorsConstant::mDefaultColor2;
  }

  auto vColor3 = bpFromString<bpColor>(aColor3);

  if (bpColor(0.0f, 0.0f, 0.0f) == vColor3) {
    vColor3 = bpFileConversionColorsConstant::mDefaultColor3;
  }

  bpFileConversionColors::tDefaultColors vDefaultColors = { vColor0, vColor1, vColor2, vColor3 };
  bpFileReaderImpl::SetDefaultColors(vDefaultColors);
}


bpString bpConverterApplication::GetVersionFullStringRevision() const
{
  return bpString(IMARISCONVERT_APPLICATION_NAME_STR) + " " +
    bpToString(IMARISCONVERT_VERSION_MAJOR) + "." +
    bpToString(IMARISCONVERT_VERSION_MINOR) + "." +
    bpToString(IMARISCONVERT_VERSION_PATCH) +
    IMARISCONVERT_VERSION_BUILD_STR +
    " [" + __DATE__ + "]" +
    " Build " + BP_REVISION_NUMBER_STR +
    " for " + BP_ARCHITECTURE_STR;
}


void bpConverterApplication::PrintCmdLineVersion() const
{
  std::cout << GetVersionFullStringRevision() << std::endl;
}


void bpConverterApplication::PrintInputFormats(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories) const
{
  std::vector<bpString> vFileFormat;
  std::vector<bpString> vFileFormatDescription;
  std::vector<bpString> vFileFormatExtension;

  bpSize vFileFormatLength = 0;

  // collect input format strings
  const bpSharedPtr<bpFileReaderFactory>& vFactory = CreateFileReaderFactory(aFileReaderFactories);
  bpFileReaderFactory::Iterator vFileFormatIt = vFactory->FormatBegin();

  while (vFileFormatIt != vFactory->FormatEnd()) {
    vFileFormatLength = std::max((*vFileFormatIt).length(), vFileFormatLength);
    vFileFormat.push_back(*vFileFormatIt);
    vFileFormatDescription.push_back(vFactory->GetFormatDescription(*vFileFormatIt));
    auto vFormatExtensions = vFactory->GetFormatExtensions(*vFileFormatIt);
    auto vFormatExtension = bpJoin(vFormatExtensions, ";");
    vFileFormatExtension.push_back(vFormatExtension);

    vFileFormatIt++;
  }

  // print input format strings
  std::cout << "Input File Formats are:" << std::endl;
  std::cout << std::endl;
  for (bpSize vFormatIndex = 0; vFormatIndex < vFileFormat.size(); vFormatIndex++) {
    vFileFormat[vFormatIndex].resize(vFileFormatLength, ' ');
    std::cout << "  " + vFileFormat[vFormatIndex] + " " + vFileFormatDescription[vFormatIndex] + " - (" + vFileFormatExtension[vFormatIndex] + ")\n";
  }
  std::cout << std::endl;
}


void bpConverterApplication::PrintOutputFormats() const
{
  std::vector<bpString> vFileFormat{ "Imaris5" };
  std::vector<bpString> vFileFormatDescription{ "Bitplane: Imaris 5.5" };
  std::vector<bpString> vFileFormatExtension{ "ims" };

  bpSize vFileFormatLength = 0;
  for (const auto& vFormat : vFileFormat) {
    vFileFormatLength = std::max(vFormat.length(), vFileFormatLength);
  }

  // print output format strings
  std::cout << "Output File Formats are:" << std::endl;
  std::cout << std::endl;
  for (bpSize vFormatIndex = 0; vFormatIndex < vFileFormat.size(); vFormatIndex++) {
    vFileFormat[vFormatIndex].resize(vFileFormatLength, ' ');
    std::cout << "  " + vFileFormat[vFormatIndex] + " " + vFileFormatDescription[vFormatIndex] + " - (" + vFileFormatExtension[vFormatIndex] + ")\n";
  }
  std::cout << std::endl;
}


void bpConverterApplication::PrintExitCodes() const
{
  std::cout << "Exit Codes:" << std::endl;
  std::cout << std::endl;
  std::cout << "  " << IMARIS_CONVERT_EXIT_SUCCESS              << " - Success (no error)" << std::endl;
  std::cout << "  " << IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS    << " - Invalid Arguments" << std::endl;
  std::cout << "  " << IMARIS_CONVERT_EXIT_UKNOWN_FILE          << " - Unknown File Type" << std::endl;
  std::cout << "  " << IMARIS_CONVERT_EXIT_TIMEOUT              << " - Timeout" << std::endl;
  std::cout << "  " << IMARIS_CONVERT_EXIT_ABNORMAL_TERMINATION << " - Abnormal Termination" << std::endl;
  std::cout << std::endl;
}


void bpConverterApplication::PrintCmdLineUsage(const bpString& aProgramName) const
{
  std::cout << "Examples:" << std::endl;
  std::cout << std::endl;
  std::cout << "> " << aProgramName << " -if lif -i \"Leica_serie2.lif\" -ii 1 -o \"Leica_serie2_conv_1.ims\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Reads \"Leica_serie2.lif\" as \"lif\" file. The input format argument, -if is optional." << std::endl;
  std::cout << "If \"Leica_serie2.lif\" contains more than one image the option \"-ii\" specifies the" << std::endl;
  std::cout << "index of the source image (index counting starts with 0)." << std::endl;
  std::cout << std::endl;
  std::cout << "> " << aProgramName << " -i \"retina.ims\" -m \"retina.imi\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Extracts all image meta data of the file. The resulting \"retina.imi\" contains" << std::endl;
  std::cout << "file name(s), image names, desctiptions, image size, channel information etc." << std::endl;
  std::cout << "The data in the output file is structured by XML tags." << std::endl;
  std::cout << std::endl;
  std::cout << "> " << aProgramName << " -i \"retina.ims\" -x \"retina.x\"" << std::endl;
  std::cout << std::endl;
  std::cout << "Reads all voxel data of the file, and calculates a hash-code. The resulting" << std::endl;
  std::cout << "\"retina.x\" contains the voxel-hash-code as XML" << std::endl;
  std::cout << std::endl;
  std::cout << "> " << aProgramName << " -i \"retina.lif\" -ii 2 -t \"retina.tif\" -ts 128" << std::endl;
  std::cout << std::endl;
  std::cout << "If \"retina.lif\" contains more than one image the option \"-ii\" specifies the" << std::endl;
  std::cout << "index of the source image (index counting starts with 0). The output is a" << std::endl;
  std::cout << "compressed tiff-image image with 128 x 128 pixels. If the option \"-ts\" is not" << std::endl;
  std::cout << "specified, the output image has the same size as the original image." << std::endl;
  std::cout << std::endl;
}


void bpConverterApplication::PrintCmdLineHelp(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories, const bpString& aProgramName) const
{
  bpString vProgramName = bpFileTools::GetFileExt(aProgramName);

  std::cout << std::endl;

  PrintCmdLineVersion();

  std::cout << "Usage: " << vProgramName << " -if Imaris3 -i \"retina.ims\" -of OmeXml -o \"retina.ome\"" << std::endl;
  std::cout << std::endl;
  std::cout << "  Argument Name:                   Argument Description:             Argument Value:" << std::endl;
  std::cout << "  -h   |--help                     This help screen                  -" << std::endl;
  std::cout << "  -v   |--version                  Print Version                     -" << std::endl;
  std::cout << "  -i   |--input                    Input File Name                   (required: filename)" << std::endl;
  std::cout << "  -li  |--inputlist                Input File Contains List of Files (default: no)" << std::endl;
  std::cout << "  -if  |--inputformat              Input File Format                 (default: autodetect)" << std::endl;
  std::cout << "  -ii  |--inputindex               Input File Image Index            (files with multiple images, default: 0)" << std::endl;
  std::cout << "  -ic  |--inputcrop                Crop Input File Image             (default: do not crop - MinX,MaxX,MinY,MaxY,MinZ,MaxZ,MinC,MaxC,MinT,MaxT" << std::endl;
  std::cout << "                                                                     0 defaults to min or max, resp.)" << std::endl;
  std::cout << "  -il  |--inputlayout              Apply layout to file series       (default: no layout - filename" << std::endl;
  std::cout << "  -vs  |--voxelsize                Set Voxel Size                    (default: empty - read from file)" << std::endl;
  std::cout << "  -vx  |--voxelsizex               Set Voxel Size in X dimension     (default: empty - read from file)" << std::endl;
  std::cout << "  -vy  |--voxelsizey               Set Voxel Size in Y dimension     (default: empty - read from file)" << std::endl;
  std::cout << "  -vz  |--voxelsizez               Set Voxel Size in Z dimension     (default: empty - read from file)" << std::endl;
  std::cout << "  -o   |--output                   Output File Name                  (default: empty - do not generate)" << std::endl;
  std::cout << "  -t   |--thumbnail                Thumbnail File Name               (TIFF image, use thumbnail arguments multiple times for multiple thumbnails)" << std::endl;
  std::cout << "  -tb  |--tbackground              Thumbnail Background Color        (#RRGGBBAA, default: system window color)" << std::endl;
  std::cout << "  -tm  |--tmode                    Thumbnail Mode                    (default: Automatic - Slice|MiddleSlice|MaxIntensity|MinIntensity|Automatic)" << std::endl;
  std::cout << "  -ts  |--tsize                    Thumbnail Image Size X and Y      (default: size of input image)" << std::endl;
  std::cout << "  -tl  |--tlimit                   Thumbnail Image Size Max X,Y      (default: 1024, or input image size if smaller)" << std::endl;
  std::cout << "  -th  |--ttimepoint               Thumbnail Image Time point        (default: 0)" << std::endl;
  std::cout << "  -tz  |--tSliceIndexZ             Thumbnail Image z slice           (default: 0)" << std::endl;
  std::cout << "  -tf  |--tformat                  Thumbnail Image output format     (default: png - tiff|jpeg (75:1 quality) )" << std::endl;
  std::cout << "  -to  |--timeout                  Timeout in seconds                (default: no timeout)" << std::endl;
  std::cout << "  -ti  |--tointerval               Throughput output interval in ms  (default: 0 - no throughput output)" << std::endl;
  std::cout << "  -a   |--allfiles                 Show Attached Files               (default: empty - do not generate. -a [filename])" << std::endl;
  std::cout << "  -m   |--metadata                 Show Image Meta Data              (default: empty - do not generate. -m [filename])" << std::endl;
  std::cout << "  -x   |--voxelhash                Show Voxel Hash Code              (default: empty - do not generate. -x [filename])" << std::endl;
  std::cout << "  -d   |--descriptors              Show Image Descriptors            (default: empty - do not generate. -d [filename])" << std::endl;
  std::cout << "  -xs  |--vblocksort               Block reading sequence            (n|t - default is t, n=no, t=time)" << std::endl; // could enhance it to any combination of x|y|z|c|t
  std::cout << "  -l   |--log                      Log into file                     (default: to stdout - filename|\"none\")" << std::endl;
  std::cout << "  -lp  |--logprogress              Log R/W progress to stdout        (default: do not log progress)" << std::endl;
  std::cout << "  -nt  |--nthreads                 Set number of compression threads (default: 8)" << std::endl;
  std::cout << "  -c   |--compression              Compression level                 (default: 2)" << std::endl;
  std::cout << "  -frp |--filereaderplugins        File Reader Plugins Path          (default: empty - no plugins)" << std::endl;
  std::cout << "  -dcl |--defaultcolorlist         Default color list                (4 #RRGGBB colors that apply to first, second, third and other channels)" << std::endl;
  std::cout << "  -fsdx|--fileseriesdelimitersx    File series delimiters X          (X delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << "  -fsdy|--fileseriesdelimitersy    File series delimiters Y          (Y delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << "  -fsdz|--fileseriesdelimitersz    File series delimiters Z          (Z delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << "  -fsdc|--fileseriesdelimitersc    File series delimiters C          (C delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << "  -fsdt|--fileseriesdelimiterst    File series delimiters T          (T delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << "  -fsdf|--fileseriesdelimitersf    File series delimiters F          (F delimiters to apply to configurable file formats, colon separated)" << std::endl;
  std::cout << std::endl;

  PrintInputFormats(aFileReaderFactories);
  PrintOutputFormats();
  PrintExitCodes();

  PrintCmdLineUsage(vProgramName);
}


bpSharedPtr<bpFileReaderFactory> bpConverterApplication::CreateFileReaderFactory(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories) const
{
  auto vFileReaderFactory = bpfMakeSharedPtr<bpFileReaderFactoryFileIO>();

  for (const auto vFileReaderImplFactory : aFileReaderFactories) {
    vFileReaderFactory->AddFactoryImpl(vFileReaderImplFactory);
}

 
  return vFileReaderFactory;
}



int bpConverterApplication::Execute(const std::vector<bpSharedPtr<bpfFileReaderImplFactoryBase>>& aFileReaderFactories, const std::vector<bpString>& aArguments)
{
  auto vFileReaderFactory = CreateFileReaderFactory(aFileReaderFactories);
  bpConverter vConverter{ vFileReaderFactory };

  //
  // parse the command line arguments
  //

  for (bpSize vArgIndex = 1; vArgIndex < aArguments.size(); ++vArgIndex) {

    bpString vArgName(aArguments[vArgIndex]);

    //
    // the following options do not require an argument
    // -m -a -x are allowed to have an arg or not
    //

    if (vArgName == "-h" || vArgName == "-help" || vArgName == "--help") {
      PrintCmdLineHelp(aFileReaderFactories, aArguments[0]);
      exit(IMARIS_CONVERT_EXIT_SUCCESS);
    }
    else if (vArgName == "-v" || vArgName == "-version" || vArgName == "--version") {
      PrintCmdLineVersion();
      exit(IMARIS_CONVERT_EXIT_SUCCESS);
    }
    else if (vArgName == "-f" || vArgName == "-formats" || vArgName == "--formats") {
      vConverter.SetPrintSupportedFormats(true);
      continue;
    }
    if (vArgName == "-m" || vArgName == "-metadata" || vArgName == "--metadata") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetDoMetaDataCalculation(true);
        continue;
      }
    }
    if (vArgName == "-ma" || vArgName == "-metadataforarena" || vArgName == "--metadataforarena") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetDoMetaDataForArenaCalculation(true);
        continue;
      }
    }
    else if (vArgName == "-a" || vArgName == "-allfiles" || vArgName == "--allfiles") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetDoAllFilesNamesCalculation(true);
        continue;
      }
    }
    else if (vArgName == "-x" || vArgName == "-voxelhash" || vArgName == "--voxelhash") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetDoVoxelHashCalculated(true);
        continue;
      }
    }
    else if (vArgName == "-d" || vArgName == "-descriptors" || vArgName == "--descriptors") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetDoImageDescriptorsCalculation(true);
        continue;
      }
    }
    else if (vArgName == "-mi" || vArgName == "-inputlist" || vArgName == "--inputlist") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetInputList(true);
        continue;
      }
    }
    else if (vArgName == "-lp" || vArgName == "-logprogress" || vArgName == "--logprogress") {
      if (IsAtomicArgument(vArgIndex, aArguments)) {
        vConverter.SetEnableLogProgress(true);
        continue;
      }
    }

    //
    // the following options require an argument
    //

    ++vArgIndex;
    if (vArgIndex == aArguments.size()) {
      std::cerr << "Option " << vArgName << " requires an argument. Use --help for details" << std::endl;
      exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
    }

    bpString vArgValue(aArguments[vArgIndex]);
    if (vArgName == "-i" || vArgName == "-input" || vArgName == "--input") {
      vConverter.SetInputFileName(vArgValue, vArgName);
      std::vector<bpString> vInputFileNames;
      for (; vArgIndex + 1 < aArguments.size() && !bpStartsWith(aArguments[vArgIndex + 1], "-"); ++vArgIndex) {
        vInputFileNames.push_back(aArguments[vArgIndex + 1]);
      }
      vConverter.SetAdditionalInputFileNames(vInputFileNames);
    }
    else if (vArgName == "-if" || vArgName == "-inputformat" || vArgName == "--inputformat") {
      vConverter.SetInputFileFormat(vArgValue, vArgName);
    }
    else if (vArgName == "-ii" || vArgName == "-inputindex" || vArgName == "--inputindex") {
      vConverter.SetInputFileImageIndex(bpFromString<bpSize>(vArgValue));
    }
    else if (vArgName == "-ic" || vArgName == "-inputcrop" || vArgName == "--inputcrop") {
      std::vector<bpSize> vMinMaxCropSizes;
      bpFromString<bpSize>(vArgValue, vMinMaxCropSizes, ",");
      vConverter.SetInputFileCropSizes(vMinMaxCropSizes);
    }
    else if (vArgName == "-il" || vArgName == "-inputlayout" || vArgName == "--inputlayout") {
      vConverter.SetInputSeriesLayoutFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-vs" || vArgName == "-voxelsize" || vArgName == "--voxelsize") {
      vConverter.SetInputVoxelSize(bpFromString<bpVector3Float>(vArgValue));
    }
    else if (vArgName == "-vsx" || vArgName == "-voxelsizex" || vArgName == "--voxelsizex") {
      vConverter.SetInputVoxelSizeX(bpFromString<bpFloat>(vArgValue));
    }
    else if (vArgName == "-vsy" || vArgName == "-voxelsizey" || vArgName == "--voxelsizey") {
      vConverter.SetInputVoxelSizeY(bpFromString<bpFloat>(vArgValue));
    }
    else if (vArgName == "-vsz" || vArgName == "-voxelsizez" || vArgName == "--voxelsizez") {
      vConverter.SetInputVoxelSizeZ(bpFromString<bpFloat>(vArgValue));
    }
    else if (vArgName == "-o" || vArgName == "-output" || vArgName == "--output") {
      vConverter.SetOutputFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-of" || vArgName == "-outputformat" || vArgName == "--outputformat") {
      	//todo: get rid of this argument from calling applications
    }
    else if (vArgName == "-t" || vArgName == "-thumbnail" || vArgName == "--thumbnail") {
      vConverter.AddThumbnail(vArgValue, nullptr);
    }
    else if (vArgName == "-tb" || vArgName == "-tbackground" || vArgName == "--tbackground") {
      vConverter.SetThumbnailBackground(vArgValue, vArgName);
    }
    else if (vArgName == "-tm" || vArgName == "-tmode" || vArgName == "--tmode") {
      vConverter.SetThumbnailMode(vArgValue, vArgName);
    }
    else if (vArgName == "-ts" || vArgName == "-tsize" || vArgName == "--tsize") {
      vConverter.SetThumbnailImageSize(bpFromString<bpSize>(vArgValue));
    }
    else if (vArgName == "-th" || vArgName == "-ttimepoint" || vArgName == "--ttimepoint") {
      vConverter.SetThumbnailTimeIndex(bpFromString<bpInt32>(vArgValue));
    }
    else if (vArgName == "-tz" || vArgName == "-tSliceIndexZ" || vArgName == "--tSliceIndexZ") {
      vConverter.SetThumbnailSliceIndexZ(bpFromString<bpInt32>(vArgValue));
    }
    else if (vArgName == "-tl" || vArgName == "-tlimit" || vArgName == "--tlimit") {
      vConverter.SetThumbnailMaxSizeXY(bpFromString<bpSize>(vArgValue));
    }
    else if (vArgName == "-tf" || vArgName == "-tformat" || vArgName == "--tformat") {
      vConverter.SetThumbnailOutputFormat(vArgValue);
    }
    else if (vArgName == "-to" || vArgName == "-timeout" || vArgName == "--timeout") {
      bpFloat vTimeout = bpFromString<bpFloat>(vArgValue);
      if (vTimeout > 0.0f) {
        mTimeout.Start(vTimeout, IMARIS_CONVERT_EXIT_TIMEOUT);
      }
    }
    else if (vArgName == "-ti" || vArgName == "-tointerval" || vArgName == "--tointerval") {
      bpUInt32 vOutputInterval = bpFromString<bpUInt32>(vArgValue);
      vConverter.SetThroughputOutputInterval(vOutputInterval);
    }
    else if (vArgName == "-ps" || vArgName == "-datacachesize" || vArgName == "--datacachesize") {
      // old argument: ignore
    }
    else if (vArgName == "-a" || vArgName == "-allfiles" || vArgName == "--allfiles") {
      vConverter.SetAllFilesFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-m" || vArgName == "-metadata" || vArgName == "--metadata") {
      vConverter.SetMetaDataFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-ma" || vArgName == "-metadataforarena" || vArgName == "--metadataforarena") {
      vConverter.SetMetaDataForArenaFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-x" || vArgName == "-voxelhash" || vArgName == "--voxelhash") {
      vConverter.SetVoxelHashFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-d" || vArgName == "-descriptors" || vArgName == "--descriptors") {
      vConverter.SetImageDescriptorsFileName(vArgValue, vArgName);
    }
    else if (vArgName == "-l" || vArgName == "-log" || vArgName == "--log") {
      vConverter.SetLogFile(vArgValue, vArgName);
    }
    else if (vArgName == "-xs" || vArgName == "-vblocksort" || vArgName == "--vblocksort") {
      vConverter.SetVoxelHashBlockSort(vArgValue, vArgName);
    }
    else if (vArgName == "-nt" || vArgName == "-nthreads" || vArgName == "--nthreads") {
      vConverter.SetNumberOfThreads(bpFromString<bpSize>(vArgValue));
    }
    else if (vArgName == "-c" || vArgName == "-compression" || vArgName == "--compression") {
      vConverter.SetCompressionAlgorithmType(static_cast<bpConverterTypes::tCompressionAlgorithmType>(bpFromString<bpSize>(vArgValue)));
    }
    else if (vArgName == "-frp" || vArgName == "-filereaderplugins" || vArgName == "--filereaderplugins") {
      vFileReaderFactory->SetPluginsPath(vArgValue);
    }
    else if (vArgName == "-dcl" || vArgName == "-defaultcolorlist" || vArgName == "--defaultcolorlist") {
      if (vArgIndex + 3 < aArguments.size()) {
        auto vColor0 = aArguments[vArgIndex];
        auto vColor1 = aArguments[vArgIndex + 1];
        auto vColor2 = aArguments[vArgIndex + 2];
        auto vColor3 = aArguments[vArgIndex + 3];
        SetDefaultColors(vColor0, vColor1, vColor2, vColor3);
        vArgIndex += 3;
      }
    }
    else if (vArgName == "-fsdx" || vArgName == "-fileseriesdelimitersx" || vArgName == "--fileseriesdelimitersx") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eX, vArgValue);
    }
    else if (vArgName == "-fsdy" || vArgName == "-fileseriesdelimitersy" || vArgName == "--fileseriesdelimitersy") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eY, vArgValue);
    }
    else if (vArgName == "-fsdz" || vArgName == "-fileseriesdelimitersz" || vArgName == "--fileseriesdelimitersz") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eZ, vArgValue);
    }
    else if (vArgName == "-fsdc" || vArgName == "-fileseriesdelimitersc" || vArgName == "--fileseriesdelimitersc") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eC, vArgValue);
    }
    else if (vArgName == "-fsdt" || vArgName == "-fileseriesdelimiterst" || vArgName == "--fileseriesdelimiterst") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eT, vArgValue);
    }
    else if (vArgName == "-fsdf" || vArgName == "-fileseriesdelimitersf" || vArgName == "--fileseriesdelimitersf") {
      vConverter.SetFileSeriesDimensionDelimiters(bpFileSeriesDelimiters::tDimension::eF, vArgValue);
    }
    else {
      std::cerr << "Option " << vArgName << " not understood. Use --help for details" << std::endl;
      exit(IMARIS_CONVERT_EXIT_INVALID_ARGUMENTS);
    }
  }

  bpLogger::LogInfo("Starting ImarisConvert with arguments: " + bpJoin(aArguments, " "));

  //
  // check parameters integrity
  //

  vConverter.CheckParameters();

  H5dont_atexit();

  //
  // finally it's time to run the conversion
  //

  bool vSuccess = vConverter.Execute();
  // force a new line and flush. (boost::process on MAC relays on that...)
  std::cout << std::endl << std::flush;

  // return "0" if success, "2" if fail (file type not known)
  return vSuccess ? IMARIS_CONVERT_EXIT_SUCCESS : IMARIS_CONVERT_EXIT_UKNOWN_FILE;
}


bool bpConverterApplication::IsAtomicArgument(bpSize aArgIndex, const std::vector<bpString>& aArguments)
{
  return aArgIndex == aArguments.size() - 1 || (aArgIndex < aArguments.size() - 1 && bpStartsWith(aArguments[aArgIndex + 1], "-"));
}
