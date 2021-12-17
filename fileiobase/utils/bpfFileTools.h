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


#ifndef __BP_FFILE_TOOLS__
#define __BP_FFILE_TOOLS__

#include "fileiobase/types/bpfTimeInfo.h"
#include "fileiobase/types/bpfTypes.h"

#include "fileiobase/utils/bpfInputFileStream64.h"

#include <list>


/**
 * Provides file and directory access/manipulation functions.
 *
 * \ingroup utils
 */
namespace bpfFileTools
{

/**
 * Strip File Name and File Extension. "c:/data/image.tif" =>
 * "c:/data"
 */
bpfString GetPath(const bpfString& aFileName);

/**
 * Strip File Extension.
 * "c:/data/image.tif" => "c:/data/image"
 */
bpfString GetPathFile(const bpfString& aFileName);

/**
 * Strip File Path and File Extension.
 * "c:/data/image.tif" => "image"
 */
bpfString GetFile(const bpfString& aFileName);

/**
 * Strip File Path.
 * "c:/data/image.tif" => "image.tif"
 */
bpfString GetFileExt(const bpfString& aFileName);

/**
 * Strip File Path and File Name.
 * "c:/data/image.tif" => "tif"
 */
bpfString GetExt(const bpfString& aFileName);

/**
 * Get the volume drive letter
 * "c:/data/image.tif" => "c:"
 * "//server/data/image.tif" => "//server"
 */
bpfString GetVolume(const bpfString& aFileName);

/**
 * Check if the directory is referred.
 * true for "c:/data/."
 */
bool IsCurrentDir(const bpfString& aFileName);

/**
 * Get current directory
 */
bpfString GetCurrentDir();

/**
 * Change current directory to given directoy
 */
void SetCurrentDir(const bpfString& aDirName);

/**
 * Check if a parent directory is referred.
 * true for "c:/data/.."
 */
bool IsParentDir(const bpfString& aFileName);

/**
 * Create a new directory in current directory.
 * true if success
 */
bool MakeDir(const bpfString& aDirName);

/**
 * Check if directory exists.
 */
bool IsDir(const bpfString& aDirName);

/**
 * Creates the directory path aPathName. The function will create
 * all parent directories necessary to create the directory.
 *
 * Returns true if successful; otherwise returns false.
 */
bool MakePath(const bpfString& aPathName);

/**
 * Returns true, if the file exists
 */
bool FileExists(const bpfString& aFileName);

/**
 * Renames an existing file. Returns "false", if it didn't work.
 */
bool FileRename(const bpfString& aFileNameOld, const bpfString& aFileNameNew);

/**
 * Removes an existing file. Returns "false", if it didn't work.
 */
bool FileRemove(const bpfString& aFileName);

/**
 * Returns true, if the file is writable
 */
bool IsFileWritable(const bpfString& aFileName);

/**
 * Returns a list of files located in a specified directory.
 * (filename and extension without path name "image1.tif", "image2.tif", etc)
 */
std::list<bpfString> GetFilesOfDirectory(const bpfString& aPath);
std::list<bpfString> GetDirectoriesOfDirectory(const bpfString& aPath);

/**
 * Return size of a file.
 */
bpfUInt64 GetFileSize(const bpfString& aFileName);

/**
 * Get the full path of a file
 * "image.tif" => "c:/data/image.tif"
 */
bpfString GetAbsoluteFilePath(const bpfString& aFileName);

/**
 * Transform a directory path into a canonical path if it isn't already.
 * This resolves symbolic links, dot and dot-dot elements.
 *
 * "c:/data/../data/images" => "c:/data/images"
 */
bpfString GetCanonicalDirPath(const bpfString& aPath, const bpfString& aBase = "");

/**
 * Get the relative path from a source file path to a destination file
 * src: "c:/data/images/image1.tif" dst: "c:/data/image2.tif"        => "../image2.tif"
 * src: "c:/data/image1.tif"        dst: "c:/data/images/image2.tif" => "images/image2.tif"
 * src: "c:/data/image2.tif"        dst: "d:/data/image2.tif"        => "d:/data/image2.tif"
 */
bpfString GetRelativeFileName(const bpfString& aFileNameSrc, const bpfString& aFileNameDst);

/**
 * Returns the path separator depending on the OS.
 * "\" for Windows
 */
bpfString GetSeparator();

/**
 * Appends a separator if there isn't yet a separator.
 * "c:/data" => "c:/data/"
 * "c:/data/" => "c:/data/"
 */
bpfString AppendSeparator(const bpfString& aFileName);

/**
 * Removes a trailing separator.
 * "c:/data" => "c:/data"
 * "c:/data/" => "c:/data"
 */
bpfString RemoveSeparator(const bpfString& aFileName);

/**
 * Removes multiple occurences of separator and replaces them by
 * just one.
 * "c:/data/to//file.ims" => "c:/data/to/file.ims"
 * "c:\data\to\\file.ims" => "c:\data\to\file.ims"
 * It does not replace the first two \\ or // because it looks
 * for double \\ starting from position 1 of the string.
 * "\\kontiki2\development" => "\\kontiki2\development"
 *
 * @param aFileName The string possibly containing double
 *                  (back)slashes
 * @return bpfString aFileName where multiple occurences of
 *         (back)slashes have been replaced by just one
 */
bpfString ReduceToSimpleSeparators(const bpfString& aFileName);

/**
 * Returns pathName with the '/' or '\' separators converted to separators
 * that are appropriate for the underlying operating system.
 * On windows: "c:/data/images/image1.tif" => "c:\data\images\image1.tif"
 *             "c:\data\images\image1.tif" => "c:\data\images\image1.tif"
 *
 * @param aPath a path to be converted
 * @return the converted path
 */
bpfString ConvertSeparators(const bpfString& aPath);

bpfString ReplaceAllOccurrences(const bpfString& aString, const bpfString& aOld, const bpfString& aNew);
bpfString ConvertSeparatorsToUnix(const bpfString& aPath);
bpfString ConvertSeparatorsToWin(const bpfString& aPath);

/**
 * Check if aPath has a file extension. If not aDefaultExtension
 * is appended
 *
 * "c:/data/image", "tif") => "c:/data/image.tif"
 * "c:/data/image.tif", "tif") => "c:/data/image.tif"
 * "c:/data/image.tif", "ims") => "c:/data/image.tif"
 *
 * @param aPath The path to test
 * @param aDefaultExtension the extension to add
 *
 * @return bpfString the checked path
 */
bpfString CheckFileExtension(const bpfString& aPath, const bpfString& aDefaultExtension);

/**
 * Check if two paths are identical. The paths may contain
 * the name of a file.
 *
 * @param aPath1  First path to compare
 * @param aPath2  Second path to compare
 *
 * @return true, if the paths are identical (regardless of their separator types).
 */
bool IsEqual(const bpfString& aPath1, const bpfString& aPath2);


/**
* Get creation date of file, implemented for unices and windows platforms
*/
bpfTimeInfo GetFileCreationDate(const bpfString& aPath);

/**
* Check if the given path is an absolute path.
*/
bool IsAbsolutePath(const bpfString& aPath);

/**
* Check if the given path is a relative path.
*/
bool IsRelativePath(const bpfString& aPath);

/**
* Resolves dot and dot-dot elements.
*
* "c:/data/../data/images" => "c:/data/images"
* "c:/data/./images" => "c:/data/images"
* "c:/data/../images" => "c:/images"
*
*/
bpfString ResolveDots(const bpfString& aAbsolutePath);

/**
* Normalizes given absolute path:
*  - Converts separators
*  - Reduces to simple separators
*  - Resolves dot and dot-dot elements
*/
bpfString NormalizeAbsolutePath(const bpfString& aAbsolutePath);

/**
* Extended Path Prefix is needed to enable long paths support for windows API
*
* @param aDirCreation  Define if the returned path will be used to create a directory
*
* On Windows: normalize path, and add prefix if path longer than MAX_PATH
*    - If aDirCreation: MAX_PATH = 260 - 12 - 1
*    - Otherwise:       MAX_PATH = 260 - 1
* On Other operating systems, does nothing (return aAbsolutePath)
*
* See Windows API documentation for detailed information:
* https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx#maxpath
*/
bpfString AddExtendedPathPrefix(const bpfString& aAbsolutePath, bool aDirCreation = false);
bpfString RemoveExtendedPathPrefix(const bpfString& aAbsolutePath);

/**
* Creat unicode path from utf8 path for windows operating system file access.
*
* @param aDirCreation  Define if the returned path will be used to create a directory.
*                      This has an impact for extended path compatibility on windows operating system.
*/
std::wstring FromUtf8Path(const bpfString& aPath, bool aDirCreation = false);
bpfString ToUtf8Path(const std::wstring& aPath);

void ReadFileIntoString(const bpfString& aFilename, bpfString& aFileContent);

}
#endif
