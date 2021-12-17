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


#include "fileiobase/utils/bpfInputFileStream64.h"

#if defined(_WIN32)
#include "fileiobase/utils/bpfFileTools.h"
#include "fileiobase/utils/bpfUtils.h"
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <fstream>
#endif

bpfInputFileStream64::bpfInputFileStream64()
  : mFile(0),
    mActualRead(0),
    mStateMask(std::ios::eofbit | std::ios::failbit | std::ios::badbit),
    mCacheSize(0),
    mCachePosition(nullptr),
    mCache(nullptr),
    mCacheStartPosition(0)
{
  mCache = new bpfChar[mMaxCacheSize];
  mCachePosition = mCache;
  clear();
}


bpfInputFileStream64::bpfInputFileStream64(const char* aFilename, std::ios::open_mode aMode)
  : mFile(0),
    mActualRead(0),
    mStateMask(std::ios::eofbit | std::ios::failbit | std::ios::badbit),
    mCacheSize(0),
    mCachePosition(nullptr),
    mCache(nullptr),
    mCacheStartPosition(0)
{
  mCache = new bpfChar[mMaxCacheSize];
  mCachePosition = mCache;
  clear();
  open(aFilename, aMode);
}

bpfInputFileStream64::~bpfInputFileStream64()
{
  delete []mCache;
  close();
}


void bpfInputFileStream64::open(const char* aFilename, std::ios::open_mode aMode)
{
  // opening a file is only allowed once
  if (is_open()) {
    setstate(std::ios::failbit);
    return;
  }

  // open file
#if defined(_WIN32)
  int vMode = _O_TEXT;
  if (aMode & std::ios::binary) {
    vMode = _O_BINARY;
  }
#ifdef BP_UTF8_FILENAMES
  mFile = _wopen(bpfFileTools::FromUtf8Path(aFilename).c_str(), vMode | _O_RDONLY);
#else
  mFile = _open(aFilename, vMode | _O_RDONLY);
#endif
  if (mFile == -1) {
#else
  if (aMode & std::ios::binary) {
    mFile = fopen(aFilename, "rb");
  }
  else {
    mFile = fopen(aFilename, "r");
  }
  if (mFile == nullptr) {
#endif
    setstate(std::ios::failbit);
    return;
  }

  // file has been opened fine
  setstate(std::ios::goodbit);
}


void bpfInputFileStream64::close()
{
  // failbit is set if one tries to close a non-open file
  if (!is_open()) {
    setstate(std::ios::failbit);
    return;
  }

  // close file
#if defined(_WIN32)
  _close(mFile);
#else
  fclose(mFile);
#endif
  // clean up
  mFile = 0;
  clear();
}


bool bpfInputFileStream64::is_open()
{
#if defined(_WIN32)
  if (mFile != 0 && mFile != -1) {
#else
  if (mFile != nullptr) {
#endif
    return true;
  }
  else {
    return false;
  }
}


void bpfInputFileStream64::ReadFromFile(bpfChar* aBuffer, streamsize aBufferLength)
{
#if defined(_WIN32)
  mActualRead = 0;
  streamsize vTryRead = 0;
  streamsize vMaxIntLength = INT_MAX;
  while (aBufferLength > vMaxIntLength) {
    vTryRead = _read(mFile, aBuffer + mActualRead, static_cast<bpfUInt32>(vMaxIntLength));
    if (vTryRead == -1) {
      setstate(std::ios::badbit);
      mActualRead = -1;
      return;
    }
    aBufferLength -= vTryRead;
    mActualRead += vTryRead;
  }
  vTryRead = _read(mFile, aBuffer + mActualRead, static_cast<bpfUInt32>(aBufferLength));
  if (vTryRead == -1) {
    setstate(std::ios::badbit);
    mActualRead = -1;
    return;
  }
  mActualRead += vTryRead;
#else
  mActualRead = fread(aBuffer, sizeof(bpfChar), aBufferLength, mFile);
  if (mActualRead < aBufferLength) {
    if (ferror(mFile)) {
      setstate(std::ios::badbit);
      mActualRead = -1;
    }
  }
#endif
}


bpfInputFileStream64::streamsize bpfInputFileStream64::ReadFromCache(bpfChar* aBuffer, streamsize aBufferLength)
{
  if (mCacheSize <= 0) {
    return 0;
  }

  streamsize vNumberOfBytesRead = aBufferLength;
  if (aBufferLength > mCacheSize) {
    vNumberOfBytesRead = mCacheSize;
  }
  memcpy(aBuffer, mCachePosition, static_cast<bpfSize>(vNumberOfBytesRead));
  mCachePosition += vNumberOfBytesRead;
  mCacheSize -= vNumberOfBytesRead;

  return vNumberOfBytesRead;
}


bool bpfInputFileStream64::EndOfFileReached()
{
#if defined(_WIN32)
  return (_eof(mFile) == 1);
#else
  return (feof(mFile) != 0);
#endif
}


bpfInputFileStream64& bpfInputFileStream64::read(char* aBuffer, streamsize aBufferLength)
{
  if (!is_open()) {
    setstate(std::ios::badbit);
  }
  else {
    // first, get bytes from the buffer
    mActualRead = 0;
    streamsize vNumberOfBytesRead = ReadFromCache(aBuffer, aBufferLength);
    aBufferLength -= vNumberOfBytesRead;

    if (aBufferLength > 0) {
      // if we need more bytes we will have to read from file
      if (EndOfFileReached()) {
        // if we already arrived at the end of the file, raise flags
        setstate(std::ios::failbit | std::ios::eofbit);
      }
      else if (aBufferLength > mMaxCacheSize) {
        // if we want to read more than what the stream buffer can contain
        // we directly try to read all the remaining bytes
        ReadFromFile(&(aBuffer[vNumberOfBytesRead]), aBufferLength);
        if (mActualRead > -1) {
          vNumberOfBytesRead += mActualRead;
          if (mActualRead < aBufferLength) {
            setstate(std::ios::failbit);
            if(mActualRead == 0 || EndOfFileReached()) {
              setstate(std::ios::eofbit);
            }
          }
        }
      }
      else {
        // we simply fill the buffer with new bytes
        // then copy them to the destination
#if defined(_WIN32)
        streamsize vCacheStartPosition = _telli64(mFile);
#else
        streamsize vCacheStartPosition = ftello(mFile);
#endif
        ReadFromFile(mCache, mMaxCacheSize);
        if (mActualRead > -1) {
          mCacheStartPosition = vCacheStartPosition;
          if (mActualRead < aBufferLength) {
            // we couldn't read all the bytes we wanted to read
            memcpy(&(aBuffer[vNumberOfBytesRead]), mCache, static_cast<bpfSize>(mActualRead));
            vNumberOfBytesRead += mActualRead;
            mCachePosition = &(mCache[mActualRead]);
            setstate(std::ios::failbit);
            if (mActualRead == 0 || EndOfFileReached()) {
              setstate(std::ios::eofbit);
            }
          }
          else {
            // we managed to read enough bytes
            memcpy(&(aBuffer[vNumberOfBytesRead]), mCache, static_cast<bpfSize>(aBufferLength));
            mCacheSize = mActualRead - aBufferLength;
            mCachePosition = &(mCache[aBufferLength]);
            vNumberOfBytesRead += aBufferLength;
          }
        }
      }
    }
    if (mActualRead != -1) {
      mActualRead = vNumberOfBytesRead;
    }
  }
  return *this;
}


int bpfInputFileStream64::get()
{
  char vBuffer = '\0';
  read(&vBuffer, 1);
  if (eof()) {
    vBuffer = EOF;
  }
  return (int) vBuffer;
}


bpfInputFileStream64& bpfInputFileStream64::getline(char* aBuffer, streamsize aBufferLength)
{
  streamsize vCharIndex;
  int vChar = '\0';

  // backup the current error flags so that we can play with flags as much as we want here.
  // we'll retrieve them later
  std::ios::iostate vStateBackup = mState;
  clear();

  for (vCharIndex = 0; vCharIndex < aBufferLength; ++vCharIndex) {
    vChar = get();
    if (!fail() && vChar != '\n') {
      aBuffer[vCharIndex] = (char) vChar;
    }
    else {
      break;
    }
  }

  if (eof() && vCharIndex > 0) {
    // if we reached the end of file after reading at least one character
    // we remove the failbit flag that read has added.
    std::ios::iostate vNewState = std::ios::eofbit;
    if (bad()) {
      vNewState |= std::ios::badbit;
    }
    clear(vNewState);
  }

  if (vCharIndex == aBufferLength) {
    // we haven't found any end of line.
    // set the failbit and jump back one step.
    vCharIndex--;
    seekg(-1, std::ios::cur);
    setstate(std::ios::failbit);
  }

  // add back any potential error flag that may have existed before
  // as we are not supposed to have cleared them.
  setstate(vStateBackup);

  // terminate string with a null character
  aBuffer[vCharIndex] = '\0';

  mActualRead = vCharIndex;

  if (vChar == '\n') {
    ++mActualRead;
  }

  return *this;
}


bpfInputFileStream64& bpfInputFileStream64::getline(bpfString& aBuffer)
{
  static const streamsize vBufferSize = 1024;
  char vBuffer[vBufferSize];

  streamsize vNumberCharactersRead = 0;
  aBuffer = "";

  // backup the current error flags so that we can play with flags as much as we want here.
  // we'll retrieve them later.
  std::ios::iostate vStateBackup = mState;
  clear();

  while (bpfString::size_type(vNumberCharactersRead + vBufferSize) <= aBuffer.max_size()) {
    getline(vBuffer, vBufferSize);
    aBuffer.append(vBuffer);
    vNumberCharactersRead += gcount();
    if (eof() || bad()) {
      // stop reading if we've reached the end of the file
      // or if anything unexpected happened.
      break;
    }
    if (fail()) {
      // fail means that we haven't found any end of line yet.
      // reset flags and keep reading.
      clear();
    }
    else {
      // if there is no error flag, we've found an end of line
      // so we stop reading.
      break;
    }
  }

  if (bpfString::size_type(vNumberCharactersRead + vBufferSize) > aBuffer.max_size()) {
    // we haven't found any end of line so we set the fail bit.
    setstate(std::ios::failbit);
  }

  // add back any potential error flag that may have existed before
  // as we are not supposed to have cleared them.
  setstate(vStateBackup);

  mActualRead = vNumberCharactersRead;

  return *this;
}


bpfInputFileStream64::streampos bpfInputFileStream64::tellg()
{
  if(!is_open() || fail()) {
    setstate(std::ios::failbit);
    return -1;
  }

  streampos vPosition = 0;
#if defined(_WIN32)
  vPosition = _telli64(mFile);
#else
  vPosition = ftello(mFile);
#endif

  if(vPosition >= 0) {
    // re-adjust to take the cache into account
    vPosition -= mCacheSize;
  }

  return vPosition;
}


bpfInputFileStream64& bpfInputFileStream64::seekg(bpfInputFileStream64::streampos aPosition)
{
  return seekg(aPosition, std::ios::beg);
}


bpfInputFileStream64& bpfInputFileStream64::ignore(bpfInputFileStream64::streamsize aCount)
{
  return seekg(aCount, std::ios::cur);
}


bpfInputFileStream64& bpfInputFileStream64::seekg(bpfInputFileStream64::streamoff aOffset, std::ios::seekdir aDirection)
{
  if (!is_open() || fail()) {
    setstate(std::ios::failbit);
    return *this;
  }

  bpfInt32 vSeekDir = SEEK_SET;
  if (aDirection == std::ios::beg) {
    vSeekDir = SEEK_SET;
  }
  else if (aDirection == std::ios::cur) {
    if (aOffset == 0) {
      // no need to continue if we do a seek on the current position
      // stopping now prevents the cache from being emptied
      return *this;
    }
    // move within the cache if the offset is small enough
    if ( (aOffset > 0 && aOffset <= mCacheSize)
        || (aOffset < 0 && mCachePosition + aOffset >= mCache) ) {
      mCachePosition += aOffset;
      mCacheSize -= aOffset;
      return *this;
    }

    // re-adjust the offset to take the cache into account
    // before around in the file
    if (aOffset > mCacheSize) {
      vSeekDir = SEEK_CUR;
      aOffset -= mCacheSize;
    } else {
      aOffset += mCachePosition - mCache;
      aOffset += mCacheStartPosition;
      vSeekDir = SEEK_SET;
    }

  }
  else if (aDirection == std::ios::end) {
    vSeekDir = SEEK_END;
  }

  bpfInt64 vActualPos = 0;
#if defined(_WIN32)
  vActualPos = _lseeki64(mFile, aOffset, vSeekDir);
#else
  vActualPos = fseeko(mFile, aOffset, vSeekDir);
#endif
  if (vActualPos == -1) {
    setstate(std::ios::failbit);
  }

  // reset stream buffer
  mCacheSize = 0;
  mCachePosition = mCache;

  return *this;
}


bpfInputFileStream64::streamsize bpfInputFileStream64::gcount() const
{
  return mActualRead;
}


bool bpfInputFileStream64::good() const
{
  return (mState == std::ios::goodbit);
}


bool bpfInputFileStream64::bad() const
{
  return (mState == std::ios::badbit);
}


bool bpfInputFileStream64::fail() const
{
  return mState & std::ios::failbit || mState & std::ios::badbit;
}


bpfInputFileStream64::operator bool() const
{
  return !fail();
}


bool bpfInputFileStream64::eof() const
{
  return (mState & std::ios::eofbit) != 0;
}


void bpfInputFileStream64::setstate(std::ios::iostate aState)
{
  mState = mState | aState;
}


void bpfInputFileStream64::clear(std::ios::iostate aState)
{
  mState = aState & mStateMask;
}


