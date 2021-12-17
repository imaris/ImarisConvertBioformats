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


bpfFileReaderBioformatsImplFactory::bpfFileReaderBioformatsImplFactory()
{
  std::vector<bpfString> vExtensions = { "", "1sc", "2fl", "acff", "afi", "afm", "aiix", "aim", "aisf", "al3d", "ali", "am", "amiramesh", "ano", "apl", "arf", "atsf", "avi", "bin", "bip", "bmp", "btf", "c01", "cfg", "ch5", "cif", "companion.ome", "cr2", "crw", "csv", "cxd", "czi", "dat", "dcm", "df3", "dib", "dic", "dicom", "dm2", "dm3", "dm4", "dti", "dv", "dv.log", "env", "eps", "epsi", "ets", "exp", "fake", "fdf", "fff", "ffr", "fits", "flex", "fli", "frm", "fts", "gel", "gif", "grey", "hdf", "hdr", "hed", "his", "htd", "htm", "html", "hx", "i2i", "ics", "ids", "im3", "ima", "img", "ims", "inf", "inr", "ipl", "ipm", "ips", "ipw", "j2k", "j2ki", "j2kr", "jp2", "jpe", "jpeg", "jpf", "jpg", "jpk", "jpx", "klb", "l2d", "labels", "lei", "lif", "liff", "lim", "lms", "log", "lsm", "lut", "map", "mdb", "mea", "mnc", "mng", "mod", "mov", "mrc", "mrcs", "mrw", "msr", "mtb", "mvd2", "naf", "nd", "nd2", "ndpi", "ndpis", "nef", "nhdr", "nii", "nii.gz", "nrrd", "obf", "oib", "oif", "oir", "ome", "ome.btf", "ome.tf2", "ome.tf8", "ome.tif", "ome.tiff", "ome.xml", "par", "pattern", "pbm", "pcoraw", "pct", "pcx", "pgm", "pic", "pict", "png", "pnl", "ppm", "pr3", "ps", "psd", "pst", "pty", "qptiff", "r3d", "r3d.log", "r3d_d3d", "raw", "rec", "res", "scan", "scn", "sdt", "seq", "set", "sif", "sld", "sm2", "sm3", "spc", "spe", "spi", "spl", "st", "stk", "stp", "svs", "sxm", "tf2", "tf8", "tfr", "tga", "thm", "tif", "tif, tiff", "tiff", "tim", "tnb", "top", "txt", "v", "vff", "vms", "vsi", "vws", "wat", "wav", "wlz", "wpi", "xdce", "xlog", "xml", "xqd", "xqf", "xv", "xys", "zfp", "zfr", "zip", "zpo", "zvi" };
  for (bpfString vExtension : vExtensions) {
    mFormats.emplace_back(vExtension);
    mDescriptions[vExtension] = vExtension;
    mExtensions[vExtension] = { vExtension };
  }

  // initialize JNI to read bioformats jar files
  std::string vJVMFolder = GetDefaultJVMPath();
  
  std::string vJarPath = "";
#if defined(_WIN32)
  vJarPath = "fileiobioformats\\bioformats_package.jar";
#else
  vJarPath = "fileiobioformats/bioformats_package.jar";
#endif
  std::vector<std::string> vJars = { vJarPath };

  size_t vMemoryLimitMB = 8000;
  size_t vMemoryLimitJava = (vMemoryLimitMB * 1024 * 1024) * 0.8;
  bpfJNI::Init(vJVMFolder, vJars, vMemoryLimitJava);

  bpfFileReaderImpl::SetOriginalFormatFileIOVersion("ImarisFileIOBioFormats"); // todo: add version number
}


bpfFileReaderBioformatsImplFactory::~bpfFileReaderBioformatsImplFactory()
{
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
  bpfFileReaderImpl* vReaderImpl = CreateReaderImpl(aFileName, aFormatName);
  return new bpfFileReaderImplToImplInterface(vReaderImpl);
}


bpfFileReaderBioformatsImplFactory::Iterator bpfFileReaderBioformatsImplFactory::FormatBegin()
{
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


bpfString bpfFileReaderBioformatsImplFactory::GetVersion()
{
  std::ostringstream vStream;
  vStream << IMARIS_VERSION_MAJOR << "." << IMARIS_VERSION_MINOR << "." << IMARIS_VERSION_PATCH;
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