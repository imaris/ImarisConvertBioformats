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


#include "bpFileTools.h"

#include "bpUtils.h"

#include "../src/bpWriterCommonHeaders.h"
#include ICImsUtils_h


#include <iostream>
#include <fstream>


#if defined(_WIN32)
#include <windows.h>
#include <stdio.h>
#else
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#endif // _WIN32


#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#pragma GCC system_header

#elif defined _MSC_VER
#pragma warning(push, 1)

#endif

#ifndef Q_MOC_RUN
#include "boost/filesystem.hpp"
#endif

#if defined _MSC_VER
#pragma warning(pop)
#endif


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


static bpString PathToString(const fs::path& aPath) {
#ifdef BP_UTF8_FILENAMES
  return bpFileTools::ToUtf8Path(aPath.wstring());
#else
  return aPath.string();
#endif
}


bpString bpFileTools::GetPath(const bpString& aFileName)
{
  bpSize vPos = aFileName.find_last_of("\\/");
  if (vPos != bpString::npos) {
    if (IsParentDir(aFileName)) {
      bpString vFileName = aFileName.substr(0, aFileName.length() - 3);
      vPos = vFileName.find_last_of("\\/");
      if (vPos != bpString::npos) {
        return vFileName.substr(0, vPos);
      }
    }
    else {
      return aFileName.substr(0, vPos);
    }
  }
  return "";
}


bpString bpFileTools::GetPathFile(const bpString& aFileName)
{
  if (IsCurrentDir(aFileName)) {
    return aFileName.substr(0, aFileName.length() - 1);
  }
  if (IsParentDir(aFileName)) {
    bpString vFileName = aFileName.substr(0, aFileName.length() - 3);
    bpSize vPosSlash = vFileName.find_last_of("\\/");
    if (vPosSlash != bpString::npos) {
      return vFileName.substr(0, vPosSlash + 1);
    }
    else {
      return "";
    }
  }
  bpSize vPosDot = aFileName.find_last_of(".");
  if (vPosDot != bpString::npos) {
    bpSize vPosSlash = aFileName.find_last_of("\\/");
    if (vPosSlash != bpString::npos) {
      if (vPosSlash > vPosDot) {
        return aFileName;
      }
    }
    return aFileName.substr(0, vPosDot);
  }
  return aFileName;
}


bpString bpFileTools::GetFile(const bpString& aFileName)
{
  if (IsParentDir(aFileName) || IsCurrentDir(aFileName)) {
    return bpString("");
  }

  bpSize vPosSlash = aFileName.find_last_of("\\/");
  bpSize vPosDot = aFileName.find_last_of(".");
  if (vPosSlash != bpString::npos) {
    if (vPosDot != bpString::npos) {
      if (vPosSlash < vPosDot) {
        return aFileName.substr(vPosSlash + 1, vPosDot - vPosSlash - 1);
      }
    }
    return aFileName.substr(vPosSlash + 1);
  }
  if (vPosDot != bpString::npos) {
    return aFileName.substr(0, vPosDot);
  }
  return aFileName;
}


bpString bpFileTools::GetFileExt(const bpString& aFileName)
{
  fs::path vPath = aFileName;

  return PathToString(vPath.filename());
}


bpString bpFileTools::GetExt(const bpString& aFileName)
{
  if (IsParentDir(aFileName)) {
    return bpString("");
  }

  bpSize vPosDot = aFileName.find_last_of(".");
  if (vPosDot == bpString::npos) {
    return "";
  }
  bpSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash != bpString::npos) {
    if (vPosSlash > vPosDot) {
      return "";
    }
  }
  return aFileName.substr(vPosDot + 1);
}


bpString bpFileTools::GetVolume(const bpString& aFileName)
{
  bpSize vPosColon = aFileName.find(":");
  if (vPosColon != bpString::npos) {
    return aFileName.substr(0, vPosColon + 1);
  }
  if ((aFileName.find("//") == 0) ||
      (aFileName.find("\\\\") == 0)) {
    bpString vString = aFileName.substr(2);
    bpSize vPosSlash = vString.find_first_of("\\/");
    if (vPosSlash != bpString::npos) {
      return aFileName.substr(0, 2) + vString.substr(0, vPosSlash);
    }
  }
  return "";
}


bool bpFileTools::IsCurrentDir(const bpString& aFileName)
{
  return (aFileName.rfind("/.") == aFileName.length() - 2) ||
         (aFileName.rfind("\\.") == aFileName.length() - 2);
}


bpString bpFileTools::GetCurrentDir()
{
#ifdef BP_UTF8_FILENAMES
  return fs::current_path().string();
#else
  const fs::path& vPath = fs::current_path();
  return vPath.string();
#endif
}


void bpFileTools::SetCurrentDir(const bpString& aDirName)
{
#ifdef BP_UTF8_FILENAMES
  return fs::current_path(FromUtf8Path(aDirName));
#else
  return fs::current_path(aDirName);
#endif
}


bool bpFileTools::IsParentDir(const bpString& aFileName)
{
  return (aFileName.rfind("/..") == aFileName.length() - 3) ||
         (aFileName.rfind("\\..") == aFileName.length() - 3);
}


bool bpFileTools::MakeDir(const bpString& aDirName)
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


bool bpFileTools::IsDir(const bpString& aDirName)
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


bool bpFileTools::MakePath(const bpString& aPathName)
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


bool bpFileTools::FileExists(const bpString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::exists(FromUtf8Path(aFileName));
#else
    return fs::exists(aFileName);
#endif
  }
  catch (fs::filesystem_error&) {
    throw std::runtime_error(("File System Error at " + aFileName).c_str());
  }
}


bool bpFileTools::FileRename(const bpString& aFileNameOld, const bpString& aFileNameNew)
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


bool bpFileTools::FileRemove(const bpString& aFileName)
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


bool bpFileTools::IsFileWritable(const bpString& aFileName)
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
    throw std::runtime_error(("File System Error at " + aFileName).c_str());
  }
}


std::list<bpString> bpFileTools::GetFilesOfDirectory(const bpString& aPath)
{
  try {
#ifdef BP_UTF8_FILENAMES
    std::list<bpString> vPathList;
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
    std::list<bpString> vPathList;
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
    throw std::runtime_error(("File System Error at " + aPath).c_str());
  }
}


std::list<bpString> bpFileTools::GetDirectoriesOfDirectory(const bpString& aPath)
{
  try {
#ifdef BP_UTF8_FILENAMES
    std::list<bpString> vPathList;
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
    std::list<bpString> vPathList;
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
    throw std::runtime_error(("File System Error at " + aPath).c_str());
  }
}


bpUInt64 bpFileTools::GetFileSize(const bpString& aFileName)
{
  try {
#ifdef BP_UTF8_FILENAMES
    return fs::file_size(FromUtf8Path(aFileName));
#else
    return fs::file_size(aFileName);
#endif
  }
  catch (fs::filesystem_error& eE) {
    throw std::runtime_error(eE.what());
  }
}


bpString bpFileTools::GetAbsoluteFilePath(const bpString& aFileName)
{
#ifdef BP_UTF8_FILENAMES
  return ToUtf8Path(fs::absolute(FromUtf8Path(aFileName)).wstring());
#else
  return fs::absolute(aFileName).string();
#endif
}


bpString bpFileTools::GetCanonicalDirPath(const bpString& aPath, const bpString& aBase)
{
  bpString vBase = aBase;
  if (vBase.empty()) {
    vBase = GetCurrentDir();
  }
#ifdef BP_UTF8_FILENAMES
  return fs::canonical(FromUtf8Path(aPath), FromUtf8Path(vBase)).string();
#else
  return fs::canonical(aPath, vBase).string();
#endif
}


bpString bpFileTools::GetRelativeFileName(const bpString& aFileNameSrc, const bpString& aFileNameDst)
{
  if (GetVolume(aFileNameSrc) != GetVolume(aFileNameDst)) {
    return aFileNameDst;
  }

  bpString vRelativeFileName = "";

  bpString vPathSrc = GetPath(aFileNameSrc) + GetSeparator();
  bpString vPathDst = GetPath(aFileNameDst) + GetSeparator();

  bpSize vPosSlashSrc = vPathSrc.find_first_of("\\/");
  bpSize vPosSlashDst = vPathDst.find_first_of("\\/");
  while ((vPosSlashSrc != bpString::npos) &&
         (vPosSlashDst != bpString::npos)) {
    bpString vNameSrc = vPathSrc.substr(0, vPosSlashSrc);
    bpString vNameDst = vPathDst.substr(0, vPosSlashDst);
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
    bpSize vPosSlashSrc2 = vPathSrc.find_first_of("\\/");
    while (vPosSlashSrc2 != bpString::npos) {
      vRelativeFileName = "../" + vRelativeFileName;
      vPathSrc = vPathSrc.substr(vPosSlashSrc2 + 1);
      vPosSlashSrc2 = vPathSrc.find_first_of("\\/");
    }
  }

  vRelativeFileName += GetFileExt(aFileNameDst);

  return vRelativeFileName;
}


bpString bpFileTools::GetSeparator()
{
#if defined(_WIN32)
  return "\\";
#else
  return "/";
#endif
}


bpString bpFileTools::AppendSeparator(const bpString& aFileName)
{
  bpSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash == bpString::npos) {
    return aFileName + GetSeparator();
  }
  else if (vPosSlash + 1 != aFileName.length()) {
    return aFileName + GetSeparator();
  }
  else {
    return aFileName;
  }
}


bpString bpFileTools::RemoveSeparator(const bpString& aFileName)
{
  bpSize vPosSlash = aFileName.find_last_of("\\/");
  if (vPosSlash != bpString::npos) {
    if (vPosSlash + 1 == aFileName.length()) {
      return aFileName.substr(0, vPosSlash);
    }
  }
  return aFileName;
}


bpString bpFileTools::ReduceToSimpleSeparators(const bpString& aFileName)
{
  if (aFileName.size() < 2) {
    return aFileName;
  }

  bpString vFileName = aFileName;
  bpSize vSeparatorPos;
  // remove double backslashes, search from the second position because
  // we do not want to remove a preceding '\\' (e.g. in "\\kontiki2\development\Data")
  vSeparatorPos = vFileName.find("\\\\", 1); //
  while (vSeparatorPos != bpString::npos) {
    vFileName = vFileName.substr(0, vSeparatorPos) + vFileName.substr(vSeparatorPos + 1, vFileName.length() - vSeparatorPos - 1);
    vSeparatorPos = vFileName.find("\\\\", 1);
  }
  // remove double slashes
  vSeparatorPos = vFileName.find("//", 1);
  while (vSeparatorPos != bpString::npos) {
    vFileName = vFileName.substr(0, vSeparatorPos) + vFileName.substr(vSeparatorPos + 1, vFileName.length() - vSeparatorPos - 1);
    vSeparatorPos = vFileName.find("//", 1);
  }
  return vFileName;
}

bpString bpFileTools::ReplaceAllOccurrences(const bpString& aString, const bpString& aOld, const bpString& aNew)
{
  if (aOld == "") {
    return aString;
  }
  bpString vString = aString;
  bpSize vPosition = vString.find(aOld);
  while (vPosition != bpString::npos) {
    vString.replace(vPosition, aOld.length(), aNew);
    vPosition = vString.find(aOld, vPosition + aNew.length());
  }
  return vString;
}

bpString bpFileTools::ConvertSeparatorsToUnix(const bpString& aPath)
{
  return ReplaceAllOccurrences(aPath, "\\", "/");
}

bpString bpFileTools::ConvertSeparatorsToWin(const bpString& aPath)
{
  return ReplaceAllOccurrences(aPath, "/", "\\");
}

bpString bpFileTools::ConvertSeparators(const bpString& aPath)
{
  bpString vPath = aPath;
#if defined(_WIN32)
  vPath = ConvertSeparatorsToWin(aPath);
#else
  vPath = ConvertSeparatorsToUnix(aPath);
#endif
  return vPath;
}


bpString bpFileTools::CheckFileExtension(const bpString& aPath, const bpString& aDefaultExtension)
{
  bpString vPath = aPath;
  bpString vPathExtension = bpFileTools::GetExt(aPath);
  if (vPathExtension.empty()) {
    // there is no extension, add one...
    vPath += ".";
    vPath += aDefaultExtension;
  }
  return vPath;
}


bool bpFileTools::IsEqual(const bpString& aPath1, const bpString& aPath2)
{
  bpString vPath1 = ConvertSeparators(aPath1);
  bpString vPath2 = ConvertSeparators(aPath2);
  vPath1 = RemoveSeparator(vPath1);
  vPath2 = RemoveSeparator(vPath2);
  return (vPath1 == vPath2);
}


bpString bpFileTools::GetFileCreationDate(const bpString& aFilename)
{
#ifdef BP_UTF8_FILENAMES
  std::time_t vTime = fs::last_write_time(FromUtf8Path(aFilename));
#else
  std::time_t vTime = fs::last_write_time(aFilename);
#endif
  std::tm* vLocalTime = std::localtime(&vTime);
  return bpImsUtils::DateTimeToString(1900+vLocalTime->tm_year, 1 + vLocalTime->tm_mon, vLocalTime->tm_mday, vLocalTime->tm_hour, vLocalTime->tm_min, vLocalTime->tm_sec);
}

bool bpFileTools::IsAbsolutePath(const bpString& aPath)
{
#ifdef BP_UTF8_FILENAMES
  fs::path vPath(bpFromUtf8(aPath));
#else
  fs::path vPath(aPath);
#endif
  return vPath.is_absolute();
}

bool bpFileTools::IsRelativePath(const bpString& aPath)
{
  return !IsAbsolutePath(aPath);
}

bpString bpFileTools::ResolveDots(const bpString& aAbsolutePath)
{
  std::vector<bpString> vElements = bpSplit(aAbsolutePath, GetSeparator(), false, false);
  vElements.erase(std::remove(vElements.begin(), vElements.end(), "."), vElements.end());
  std::vector<bpString>::iterator vIt;
  while ((vIt = std::find(vElements.begin(), vElements.end(), "..")) != vElements.end()) {
    if (vIt == vElements.begin()) {
      throw std::runtime_error(("bpFileTools::ResolveDots: Invalid absolute path: " + aAbsolutePath).c_str());
    }
    vIt = vElements.erase(vIt);
    vElements.erase(std::prev(vIt));
  }
  return bpJoin(vElements, GetSeparator());
}

bpString bpFileTools::NormalizeAbsolutePath(const bpString& aAbsolutePath)
{
  return ResolveDots(ReduceToSimpleSeparators(ConvertSeparators(aAbsolutePath)));
}

bpString bpFileTools::AddExtendedPathPrefix(const bpString& aAbsolutePath, bool aDirCreation)
{
#if defined(_WIN32)
  if (aAbsolutePath.empty()) {
    return aAbsolutePath;
  }
  bpString vPath = NormalizeAbsolutePath(aAbsolutePath);
  static bpSize vMaxCharacters = 260 - 1;
  static bpSize vMaxDirCreationCharacters = 260 - 12 - 1;
  if (bpFromUtf8(vPath).length() > (aDirCreation ? vMaxDirCreationCharacters : vMaxCharacters)) {
    bpString vExtendedLengthPathPrefix = "\\\\?\\";
    if (!bpStartsWith(vPath, vExtendedLengthPathPrefix)) {
      bpString vDoubleBackSlashesPrefix = "\\\\";
      if (bpStartsWith(vPath, vDoubleBackSlashesPrefix)) {
        vPath = "UNC\\" + vPath.substr(vDoubleBackSlashesPrefix.length(), vPath.length() - vDoubleBackSlashesPrefix.length());
      }
      return vExtendedLengthPathPrefix + vPath;
    }
  }
  return vPath;
#endif
  return aAbsolutePath;
}

bpString bpFileTools::RemoveExtendedPathPrefix(const bpString& aPath)
{
  bpString vPath = bpReplace(aPath, "\\\\?\\UNC\\", "\\\\");
  return bpReplace(vPath, "\\\\?\\", "");
}

bpString bpFileTools::ToUtf8Path(const std::wstring& aPath)
{
  return RemoveExtendedPathPrefix(bpToUtf8(aPath));
}

std::wstring bpFileTools::FromUtf8Path(const bpString& aPath, bool aDirCreation)
{
  return bpFromUtf8(IsRelativePath(aPath) ? aPath : AddExtendedPathPrefix(aPath, aDirCreation));
}
