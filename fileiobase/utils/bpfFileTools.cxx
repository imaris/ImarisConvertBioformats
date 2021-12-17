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


#include "fileiobase/utils/bpfFileTools.h"


#include "fileiobase/exceptions/bpfFileIOException.h"
#include "fileiobase/utils/bpfUtils.h"


#include <iostream>
#include <fstream>
#include <vector>


#if defined(_WIN32)
#include <windows.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#endif // _WIN32



#include "fileiobase/utils/bpBoostFileSystemHeader.h"

namespace fs = boost::filesystem;

/*
 * Test File Path's:
 *
 * "c:/data/image.tif";
 * "c:/data/image";
 * "c:/data/.image";
 * "c:/data/..";
 * "c:/data/.";
 * "c:/data/";
 * "c:/data";
 * "c:/data.ims/image.tif";
 * "c:/data.ims/image";
 * "c:/data.ims/.image";
 * "c:/data.ims/..";
 * "c:/data.ims/.";
 * "c:/data.ims/";
 * "c:/data.ims";
 * "//kontiki/data.ims/image.tif";
 * "//kontiki/data.ims/image";
 * "//kontiki/data.ims/.image";
 * "//kontiki/data.ims/..";
 * "//kontiki/data.ims/.";
 * "//kontiki/data.ims/";
 * "//kontiki/data.ims";
 * "image.tif";
 * "image";
 * ".image";
 * "..";
 * ".";
 * "";
 */


static bpfString PathToString(const fs::path& aPath) {
#ifdef BP_UTF8_FILENAMES
  return bpfFileTools::ToUtf8Path(aPath.wstring());
#else
  return aPath.string();
#endif
}


bpfString bpfFileTools::GetPath(const bpfString& aFileName)
{
  bpfSize vPos = aFileName.find_last_of("\\/");
  if (vPos != bpfString::npos) {
    if (IsParentDir(aFileName)) {
      bpfString vFileName = aFileName.substr(0, aFileName.length() - 3);
      vPos = vFileName.find_last_of("\\/");
      if (vPos != bpfString::npos) {
        return vFileName.substr(0, vPos);
      }
    }
    else {
      return aFileName.substr(0, vPos);
    }
  }
  return "";
}


bpfString bpfFileTools::GetPathFile(const bpfString& aFileName)
{
  if (IsCurrentDir(aFileName)) {
    return aFileName.substr(0, aFileName.length() - 1);
  }
  if (IsParentDir(aFileName)) {
    bpfString vFileName = aFileName.substr(0, aFileName.length() - 3);
    bpfSize vPosSlash = vFileName.find_last_of("\\/");
    if (vPosSlash != bpfString::npos) {
      return vFileName.substr(0, vPosSlash + 1);
    }
    else {
      return "";
    }
  }
  bpfSize vPosDot = aFileName.find_last_of(".");
  if (vPosDot != bpfString::npos) {
    bpfSize vPosSlash = aFileName.find_last_of("\\/");
    if (vPosSlash != bpfString::npos) {
      if (vPosSlash > vPosDot) {
        return aFileName;
      }
    }
    return aFileName.substr(0, vPosDot);
  }
  return aFileName;
}


bpfString bpfFileTools::GetFile(const bpfString& aFileName)
{
  if (IsParentDir(aFileName) || IsCurrentDir(aFileName)) {
    return bpfString("");
  }

  bpfSize vPosSlash = aFileName.find_last_of("\\/");
  bpfSize vPosDot = aFileName.find_last_of(".");
  if (vPosSlash != bpfString::npos) {
    if (vPosDot != bpfString::npos) {
      if (vPosSlash < vPosDot) {
        return aFileName.substr(vPosSlash + 1, vPosDot - vPosSlash - 1);
      }
    }
    return aFileName.substr(vPosSlash + 1);
  }
  if (vPosDot != bpfString::npos) {
    return aFileName.substr(0, vPosDot);
  }
  return aFileName;
}


bpfString bpfFileTools::GetFileExt(const bpfString& aFileName)
{
  fs::path vPath = aFileName;

  return PathToString(vPath.filename());
}


bpfString bpfFileTools::GetExt(const bpfString& aFileName)
{
  if (IsParentDir(aFileName)) {
    return bpfString("");
  }

  bpfSize vPosDot = aFileName.find_last_of(".");
  if (vPosDot == bpfString::npos) {
    return "";
  }
  bpfSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash != bpfString::npos) {
    if (vPosSlash > vPosDot) {
      return "";
    }
  }
  return aFileName.substr(vPosDot + 1);
}


bpfString bpfFileTools::GetVolume(const bpfString& aFileName)
{
  bpfSize vPosColon = aFileName.find(":");
  if (vPosColon != bpfString::npos) {
    return aFileName.substr(0, vPosColon + 1);
  }
  if ((aFileName.find("//") == 0) ||
      (aFileName.find("\\\\") == 0)) {
    bpfString vString = aFileName.substr(2);
    bpfSize vPosSlash = vString.find_first_of("\\/");
    if (vPosSlash != bpfString::npos) {
      return aFileName.substr(0, 2) + vString.substr(0, vPosSlash);
    }
  }
  return "";
}


bool bpfFileTools::IsCurrentDir(const bpfString& aFileName)
{
  return (aFileName.rfind("/.") == aFileName.length() - 2) ||
         (aFileName.rfind("\\.") == aFileName.length() - 2);
}


bpfString bpfFileTools::GetCurrentDir()
{
#ifdef BP_UTF8_FILENAMES
  return fs::current_path().string();
#else
  const fs::path& vPath = fs::current_path();
  return vPath.string();
#endif
}


void bpfFileTools::SetCurrentDir(const bpfString& aDirName)
{
#ifdef BP_UTF8_FILENAMES
  return fs::current_path(FromUtf8Path(aDirName));
#else
  return fs::current_path(aDirName);
#endif
}


bool bpfFileTools::IsParentDir(const bpfString& aFileName)
{
  return (aFileName.rfind("/..") == aFileName.length() - 3) ||
         (aFileName.rfind("\\..") == aFileName.length() - 3);
}


bool bpfFileTools::MakeDir(const bpfString& aDirName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::create_directory(FromUtf8Path(aDirName, true));
#else
    return fs::create_directory(aDirName);
#endif
  }
  catch (fs::filesystem_error&) {
    return false;
  }
}


bool bpfFileTools::IsDir(const bpfString& aDirName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::is_directory(FromUtf8Path(aDirName));
#else
    return fs::is_directory(aDirName);
#endif
  }
  catch (fs::filesystem_error&) {
    return false;
  }
}


bool bpfFileTools::MakePath(const bpfString& aPathName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::create_directories(FromUtf8Path(aPathName, true));
#else
    return fs::create_directories(aPathName);
#endif
  }
  catch (fs::filesystem_error&) {
    return false;
  }
}


bool bpfFileTools::FileExists(const bpfString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::exists(FromUtf8Path(aFileName));
#else
    return fs::exists(aFileName);
#endif
  }
  catch (fs::filesystem_error&) {
    throw bpfException();
  }
}


bool bpfFileTools::FileRename(const bpfString& aFileNameOld, const bpfString& aFileNameNew)
{
  try {
#ifdef BP_UTF8_FILENAMES
    fs::rename(FromUtf8Path(aFileNameOld), FromUtf8Path(aFileNameNew));
#else
    fs::rename(aFileNameOld, aFileNameNew);
#endif
    return true;
  }
  catch (...) {
    return false;
  }
}


bool bpfFileTools::FileRemove(const bpfString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::remove(FromUtf8Path(aFileName));
#else
    return fs::remove(aFileName);
#endif
  }
  catch (fs::filesystem_error&) {
    return false;
  }
}


bool bpfFileTools::IsFileWritable(const bpfString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    std::ofstream vFileStream(FromUtf8Path(aFileName).c_str(), std::ios::out);
#else
    std::ofstream vFileStream(aFileName.c_str(), std::ios::out);
#endif
    bool vIsWritable = vFileStream.good();
    vFileStream.close();

    return vIsWritable;
  }
  catch (fs::filesystem_error&) {
    throw bpfException();
  }
}


std::list<bpfString> bpfFileTools::GetFilesOfDirectory(const bpfString& aPath)
{
  try {
#ifdef BP_UTF8_FILENAMES
    std::list<bpfString> vPathList;
    std::wstring vPath = FromUtf8Path(aPath);
    if (fs::exists(vPath)) {
      fs::directory_iterator end_itr; // default construction yields past-the-end
      for (fs::directory_iterator itr(vPath); itr != end_itr; ++itr) {
        if (fs::is_regular(itr->status())) {
          vPathList.push_back(PathToString(itr->path().filename()));
        }
      }
    }
    return vPathList;
#else
    std::list<bpfString> vPathList;
    if (fs::exists(aPath)) {
      fs::directory_iterator end_itr; // default construction yields past-the-end
      for (fs::directory_iterator itr(aPath); itr != end_itr; ++itr) {
        if (fs::is_regular_file(itr->status())) {
          vPathList.push_back(itr->path().filename().string());
        }
      }
    }
    return vPathList;
#endif
  }
  catch (fs::filesystem_error&) {
    throw bpfException();
  }
}


std::list<bpfString> bpfFileTools::GetDirectoriesOfDirectory(const bpfString& aPath)
{
  try {
#ifdef BP_UTF8_FILENAMES
    std::list<bpfString> vPathList;
    std::wstring vPath = FromUtf8Path(aPath);
    if (fs::exists(vPath)) {
      fs::directory_iterator end_itr; // default construction yields past-the-end
      for (fs::directory_iterator itr(vPath); itr != end_itr; ++itr) {
        if (fs::is_directory(itr->status())) {
          vPathList.push_back(itr->path().filename().string());
        }
      }
    }
    return vPathList;
#else
    std::list<bpfString> vPathList;
    if (fs::exists(aPath)) {
      fs::directory_iterator end_itr; // default construction yields past-the-end
      for (fs::directory_iterator itr(aPath); itr != end_itr; ++itr) {
        if (fs::is_directory(itr->status())) {
          vPathList.push_back(itr->path().filename().string());
        }
      }
    }
    return vPathList;
#endif
  }
  catch (fs::filesystem_error&) {
    throw bpfException();
  }
}


bpfUInt64 bpfFileTools::GetFileSize(const bpfString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::file_size(FromUtf8Path(aFileName));
#else
    return fs::file_size(aFileName);
#endif
  }
  catch (fs::filesystem_error& eE) {
    throw bpfException(eE.what());
  }
}


bpfString bpfFileTools::GetAbsoluteFilePath(const bpfString& aFileName)
{
#ifdef BP_UTF8_FILENAMES
  return fs::absolute(FromUtf8Path(aFileName)).string();
#else
  return fs::absolute(aFileName).string();
#endif
}


bpfString bpfFileTools::GetCanonicalDirPath(const bpfString& aPath, const bpfString& aBase)
{
  bpfString vBase = aBase;
  if (vBase.empty()) {
    vBase = GetCurrentDir();
  }
#ifdef BP_UTF8_FILENAMES
  return fs::canonical(FromUtf8Path(aPath), FromUtf8Path(vBase)).string();
#else
  return fs::canonical(aPath, vBase).string();
#endif
}


bpfString bpfFileTools::GetRelativeFileName(const bpfString& aFileNameSrc, const bpfString& aFileNameDst)
{
  if (GetVolume(aFileNameSrc) != GetVolume(aFileNameDst)) {
    return aFileNameDst;
  }

  bpfString vRelativeFileName = "";

  bpfString vPathSrc = GetPath(aFileNameSrc) + GetSeparator();
  bpfString vPathDst = GetPath(aFileNameDst) + GetSeparator();

  bpfSize vPosSlashSrc = vPathSrc.find_first_of("\\/");
  bpfSize vPosSlashDst = vPathDst.find_first_of("\\/");
  while ((vPosSlashSrc != bpfString::npos) &&
         (vPosSlashDst != bpfString::npos)) {
    bpfString vNameSrc = vPathSrc.substr(0, vPosSlashSrc);
    bpfString vNameDst = vPathDst.substr(0, vPosSlashDst);
    vPathSrc = vPathSrc.substr(vPosSlashSrc + 1);
    vPathDst = vPathDst.substr(vPosSlashDst + 1);
    if (vNameSrc != vNameDst) {
      vRelativeFileName = "../" + vRelativeFileName + vNameDst + GetSeparator();
    }
    vPosSlashSrc = vPathSrc.find_first_of("\\/");
    vPosSlashDst = vPathDst.find_first_of("\\/");
  }

  if (vPathSrc == "") {
    vRelativeFileName += vPathDst;
  }

  if (vPathDst == "") {
    bpfSize vPosSlashSrc2 = vPathSrc.find_first_of("\\/");
    while (vPosSlashSrc2 != bpfString::npos) {
      vRelativeFileName = "../" + vRelativeFileName;
      vPathSrc = vPathSrc.substr(vPosSlashSrc2 + 1);
      vPosSlashSrc2 = vPathSrc.find_first_of("\\/");
    }
  }

  vRelativeFileName += GetFileExt(aFileNameDst);

  return vRelativeFileName;
}


bpfString bpfFileTools::GetSeparator()
{
#if defined(_WIN32)
  return "\\";
#else
  return "/";
#endif
}


bpfString bpfFileTools::AppendSeparator(const bpfString& aFileName)
{
  bpfSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash == bpfString::npos) {
    return aFileName + GetSeparator();
  }
  else if (vPosSlash + 1 != aFileName.length()) {
    return aFileName + GetSeparator();
  }
  else {
    return aFileName;
  }
}


bpfString bpfFileTools::RemoveSeparator(const bpfString& aFileName)
{
  bpfSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash != bpfString::npos) {
    if (vPosSlash + 1 == aFileName.length()) {
      return aFileName.substr(0, vPosSlash);
    }
  }
  return aFileName;
}


bpfString bpfFileTools::ReduceToSimpleSeparators(const bpfString& aFileName)
{
  if (aFileName.size() < 2) {
    return aFileName;
  }

  bpfString vFileName = aFileName;
  bpfSize vSeparatorPos;
  // remove double backslashes, search from the second position because
  // we do not want to remove a preceding '\\' (e.g. in "\\kontiki2\development\Data")
  vSeparatorPos = vFileName.find("\\\\", 1); //
  while (vSeparatorPos != bpfString::npos) {
    vFileName = vFileName.substr(0, vSeparatorPos) + vFileName.substr(vSeparatorPos + 1, vFileName.length() - vSeparatorPos - 1);
    vSeparatorPos = vFileName.find("\\\\", 1);
  }
  // remove double slashes
  vSeparatorPos = vFileName.find("//", 1);
  while (vSeparatorPos != bpfString::npos) {
    vFileName = vFileName.substr(0, vSeparatorPos) + vFileName.substr(vSeparatorPos + 1, vFileName.length() - vSeparatorPos - 1);
    vSeparatorPos = vFileName.find("//", 1);
  }
  return vFileName;
}

bpfString bpfFileTools::ReplaceAllOccurrences(const bpfString& aString, const bpfString& aOld, const bpfString& aNew)
{
  if (aOld == "") {
    return aString;
  }
  bpfString vString = aString;
  bpfSize vPosition = vString.find(aOld);
  while (vPosition != bpfString::npos) {
    vString.replace(vPosition, aOld.length(), aNew);
    vPosition = vString.find(aOld, vPosition + aNew.length());
  }
  return vString;
}

bpfString bpfFileTools::ConvertSeparatorsToUnix(const bpfString& aPath)
{
  return ReplaceAllOccurrences(aPath, "\\", "/");
}

bpfString bpfFileTools::ConvertSeparatorsToWin(const bpfString& aPath)
{
  return ReplaceAllOccurrences(aPath, "/", "\\");
}

bpfString bpfFileTools::ConvertSeparators(const bpfString& aPath)
{
  bpfString vPath = aPath;
#if defined(_WIN32)
  vPath = ConvertSeparatorsToWin(aPath);
#else
  vPath = ConvertSeparatorsToUnix(aPath);
#endif
  return vPath;
}


bpfString bpfFileTools::CheckFileExtension(const bpfString& aPath, const bpfString& aDefaultExtension)
{
  bpfString vPath = aPath;
  bpfString vPathExtension = bpfFileTools::GetExt(aPath);
  if (vPathExtension.empty()) {
    // there is no extension, add one...
    vPath += ".";
    vPath += aDefaultExtension;
  }
  return vPath;
}


bool bpfFileTools::IsEqual(const bpfString& aPath1, const bpfString& aPath2)
{
  bpfString vPath1 = ConvertSeparators(aPath1);
  bpfString vPath2 = ConvertSeparators(aPath2);
  vPath1 = RemoveSeparator(vPath1);
  vPath2 = RemoveSeparator(vPath2);
  return (vPath1 == vPath2);
}


bpfTimeInfo bpfFileTools::GetFileCreationDate(const bpfString& aFilename)
{

  bpfTimeInfo vCreationDate;
#ifdef BP_UTF8_FILENAMES
  std::time_t vTime = fs::last_write_time(FromUtf8Path(aFilename));
#else
  std::time_t vTime = fs::last_write_time(aFilename);
#endif
  std::tm* vLocalTime = std::localtime(&vTime);
  vCreationDate.SetDate(bpfTimeInfo::ToJulianDay(1900+vLocalTime->tm_year, 1+vLocalTime->tm_mon, vLocalTime->tm_mday));
  vCreationDate.SetTime(((vLocalTime->tm_hour*60 + vLocalTime->tm_min)*60 + vLocalTime->tm_sec)*1000);
  return vCreationDate;
}

bool bpfFileTools::IsAbsolutePath(const bpfString& aPath)
{
#ifdef BP_UTF8_FILENAMES
  fs::path vPath(bpfFromUtf8(aPath));
#else
  fs::path vPath(aPath);
#endif
  return vPath.is_absolute();
}

bool bpfFileTools::IsRelativePath(const bpfString& aPath)
{
  return !IsAbsolutePath(aPath);
}

bpfString bpfFileTools::ResolveDots(const bpfString& aAbsolutePath)
{
  std::vector<bpfString> vElements = bpfSplit(aAbsolutePath, GetSeparator(), false, false);
  vElements.erase(std::remove(vElements.begin(), vElements.end(), "."), vElements.end());
  std::vector<bpfString>::iterator vIt;
  while ((vIt = std::find(vElements.begin(), vElements.end(), "..")) != vElements.end()) {
    if (vIt == vElements.begin()) {
      throw bpfException("bpFileTools::ResolveDots: Invalid absolute path: " + aAbsolutePath);
    }
    vIt = vElements.erase(vIt);
    vElements.erase(std::prev(vIt));
  }
  return bpfJoin(vElements, GetSeparator());
}

bpfString bpfFileTools::NormalizeAbsolutePath(const bpfString& aAbsolutePath)
{
  return ResolveDots(ReduceToSimpleSeparators(ConvertSeparators(aAbsolutePath)));
}

bpfString bpfFileTools::AddExtendedPathPrefix(const bpfString& aAbsolutePath, bool aDirCreation)
{
#if defined(_WIN32)
  if (aAbsolutePath.empty()) {
    return aAbsolutePath;
  }
  bpfString vPath = NormalizeAbsolutePath(aAbsolutePath);
  static bpfSize vMaxCharacters = 260 - 1;
  static bpfSize vMaxDirCreationCharacters = 260 - 12 - 1;
  if (bpfFromUtf8(vPath).length() > (aDirCreation ? vMaxDirCreationCharacters : vMaxCharacters)) {
    bpfString vExtendedLengthPathPrefix = "\\\\?\\";
    if (!bpfStartsWith(vPath, vExtendedLengthPathPrefix)) {
      bpfString vDoubleBackSlashesPrefix = "\\\\";
      if (bpfStartsWith(vPath, vDoubleBackSlashesPrefix)) {
        vPath = "UNC\\" + vPath.substr(vDoubleBackSlashesPrefix.length(), vPath.length() - vDoubleBackSlashesPrefix.length());
      }
      return vExtendedLengthPathPrefix + vPath;
    }
  }
  return vPath;
#endif
  return aAbsolutePath;
}

bpfString bpfFileTools::RemoveExtendedPathPrefix(const bpfString& aPath)
{
  bpfString vPath = bpfReplace(aPath, "\\\\?\\UNC\\", "\\\\");
  return bpfReplace(vPath, "\\\\?\\", "");
}

bpfString bpfFileTools::ToUtf8Path(const std::wstring& aPath)
{
  return RemoveExtendedPathPrefix(bpfToUtf8(aPath));
}

std::wstring bpfFileTools::FromUtf8Path(const bpfString& aPath, bool aDirCreation)
{
  return bpfFromUtf8(IsRelativePath(aPath) ? aPath : AddExtendedPathPrefix(aPath, aDirCreation));
}

void bpfFileTools::ReadFileIntoString(const bpfString& aFilename, bpfString& aFileContent)
{
  if (!FileExists(aFilename)) {
    throw bpfFileException("bpFileTools::ReadFileIntoString:: File " + aFilename + " not found");
  }

#ifdef BP_UTF8_FILENAMES
  std::ifstream vInputFile(FromUtf8Path(aFilename));
#else
  std::ifstream vInputFile(aFilename.c_str());
#endif

  if (!vInputFile) {
    throw bpfFileException("bpFileTools::ReadFileIntoString:: Could not read file " + aFilename);
  }
  vInputFile.seekg(0, vInputFile.end);
  std::streamoff vLength = vInputFile.tellg();
  if (vLength < 0) {
    throw bpfFileException("bpFileTools::ReadFileIntoString:: Could not read file " + aFilename);
  }
  if (vLength == 0) {
    aFileContent = "";
    return;
  }
  vInputFile.seekg(0, vInputFile.beg);
  aFileContent.resize(static_cast<bpfSize>(vLength));
  vInputFile.read(&aFileContent[0], vLength);
  if (!vInputFile) {
    // If couldn't read all characters, resize the string with the number of read characters
    aFileContent.resize(vInputFile.gcount());
  }
  vInputFile.close();

  bpfSize vPos = aFileContent.find('\0');
  if (vPos != bpfString::npos) {
    aFileContent.resize(vPos);
  }
}
