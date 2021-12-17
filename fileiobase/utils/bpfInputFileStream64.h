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


#ifndef __BPF_INPUT_FILE_STREAM64__
#define __BPF_INPUT_FILE_STREAM64__

#include "fileiobase/types/bpfTypes.h"
#include <ios>

class bpfInputFileStream64
{

public:
  typedef bpfInt64 streamsize;
  typedef bpfInt64 streampos;
  typedef bpfInt64 streamoff;

  bpfInputFileStream64();
  explicit bpfInputFileStream64(const char* aFilename, std::ios::open_mode aMode = std::ios::in);
  virtual ~bpfInputFileStream64();

  /**
   * Opens a file whose name is aFilename, associating its content
   * with the stream object to perform input operations on it.
   *
   * If the object already has a file associated (open), the
   * function fails. On failure, the failbit flag is set (which
   * can be checked with member fail).
   *
   * @param aFileName C-string containing the name of the file to be opened.
   */
  void open(const char* aFilename, std::ios::open_mode aMode = std::ios::in);

  /**
   * Closes the file currently associated with the object,
   * disassociating it from the stream.
   *
   * The function fails if no file is currently open with this
   * object. On failure, the failbit internal state flag is set
   * (which can be checked with member fail).
   */
  void close();

  /**
   * Returns true if the stream is currently associated with a
   * file, and false otherwise.
   *
   * The stream is associated with a file if either a previous
   * call to member open succeeded or if the object was
   * successfully constructed using the parameterized constructor,
   * and close has not been called since.
   *
   * @return true if a file is open, false otherwise.
   */
  bool is_open();

  /**
   * Reads a block of data of aBufferLength characters and stores
   * it in the array pointed by aBuffer.
   *
   * If the End-of-File is reached before aBufferLength characters
   * have been read, the array will contain all the elements read
   * until it, and the failbit and eofbit will be set (which can
   * be checked with members fail and eof respectively). Calling
   * member gcount after this function the total number of
   * characters read can be obtained.
   *
   * @param aBuffer Pointer to an allocated block of memory where
   *        the content read will be stored.
   * @param aBufferLength Integer value of type streamsize
   *        representing the size in characters of the block of
   *        data to be read.
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& read(char* aBuffer, streamsize aBufferLength);

  /**
   * Extracts a single character as unformatted input from the
   * stream.
   *
   * If the End-of-File is reached before extracting the character
   * the function sets the eofbit flag.
   *
   * @return The character read.
   */
  int get();

  /**
   * Extracts characters from the stream as unformatted input and
   * stores them into aBuffer as a c-string, until either the
   * extracted character is the delimiting character, or
   * aBufferLength characters have been written to aBuffer
   * (including the terminating null character).
   *
   * The delimiting character is the newline character ('\n').
   *
   * The function will also stop extracting characters if the
   * End-Of-File is reached. If this is reached prematurely
   * (before writing aBufferLength characters),
   * the function sets the eofbit flag.
   *
   * The failbit flag is set if the function extracts no
   * characters, or if the delimiting character is not found once
   * (aBufferLength-1) characters have already been written to
   * aBuffer. Note that if the character that follows those
   * (aBufferLength-1) characters in the input sequence is
   * precisely the delimiting character, it is also extracted and
   * the failbit flag is not set (the extracted sequence was
   * exactly aBufferLength characters long).
   *
   * A null character ('\0') is automatically appended to the
   * written sequence if aBufferLength is greater than zero, even
   * if an empty string is extracted.
   *
   * The number of characters successfully read and stored by this
   * function can be accessed by calling member gcount.
   *
   * @param aBuffer Pointer to an array of characters where
   *                extracted characters are stored as a c-string.
   * @param aBufferLength Maximum number of characters to write to
   *                      aBuffer (including the terminating null
   *                      character). If the function stops
   *                      reading because this limit is reached
   *                      without finding the delimiting
   *                      character, the failbit internal flag is
   *                      set.
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& getline(char* aBuffer, streamsize aBufferLength);

  /**
   * Extracts characters from the stream as unformatted input and
   * stores them into aBuffer as bpfString, until either the
   * extracted character is the delimiting character, or
   * bpfString::max_size() characters have been written to
   * aBuffer (including the terminating null character).
   *
   * The delimiting character is the newline character ('\n').
   *
   * The function will also stop extracting characters if the
   * End-Of-File is reached. If this is reached prematurely
   * (before writing bpfString::max_size() characters),
   * the function sets the eofbit flag.
   *
   * The failbit flag is set if the function extracts no
   * characters, or if the delimiting character is not found once
   * (bpfString::max_size()-1) characters have already been
   * written to aBuffer. Note that if the character that follows
   * those (bpfString::max_size()-1) characters in the input
   * sequence is precisely the delimiting character, it is also
   * extracted and the failbit flag is not set (the extracted
   * sequence was exactly aBufferLength characters long).
   *
   * The number of characters successfully read and stored by this
   * function can be accessed by calling member gcount.
   *
   * @param aBuffer Reference to the buffer where extracted
   *                characters are stored.
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& getline(bpfString& aBuffer);

  /**
   * Returns the absolute position of the get pointer.
   *
   * The get pointer determines the next location in the input
   * sequence to be read by the next input operation.
   *
   * return The number of characters between the beginning of the
   *        input sequence and the current position. Failure is
   *        indicated by returning a value of -1.
   */
  streampos tellg();

  /**
   * Sets the position of the get pointer.
   *
   * The get pointer determines the next location to be read in
   * the source associated to the stream.
   *
   * @param aPosition The new position in the stream buffer.
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& seekg(streampos aPosition);

  /**
   * Move the position of the get pointer, ignoring aCount bytes.
   *
   * The get pointer determines the next location to be read in
   * the source associated to the stream.
   *
   * @param aCount The number of bytes to be ignored.
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& ignore(streamsize aCount);

  /**
   * Sets the position of the get pointer.
   *
   * The get pointer determines the next location to be read in
   * the source associated to the stream.
   *
   * @param aOffset The offset to be applied relative to an
   *        absolute position specified in the aDirection
   *        parameter
   * @param aDirection Seeking direction. An absolute position
   *        from where the offset parameter off is applied.
   *        - ios_base::beg beginning of the stream buffer
   *        - ios_base::cur current position in the stream buffer
   *        - ios_base::end end of the stream buffer
   *
   * @return The functions return *this.
   */
  bpfInputFileStream64& seekg(streamoff aOffset, std::ios::seekdir aDirection);

  /**
   * Returns the number of characters extracted by the last input
   * operation performed on the object.
   *
   * The input operations that modify the value returned by this
   * function are those performed by the following member
   * functions: get, getline, ignore, peek, read, readsome,
   * putback and unget. Notice though, that peek, putback and
   * unget do not extract characters. So gcount will always return
   * zero after a call to any of these.
   *
   * @return The number of characters extracted by the last input
   *         operation.
   */
  streamsize gcount() const;

  /**
   * The function returns true if none of the stream's error flags
   * (eofbit, failbit and badbit) are set.
   *
   * @return true if none of the stream's state flags are set.
   *         false if any of the stream's state flags are set
   *         (badbit, eofbit or failbit).
   */
  bool good() const;

  /**
   * Returns true if the badbit error state flag is set for the
   * stream.
   *
   * @return true if the stream's badbit error state flag is set.
   *         false otherwise.
   */
  bool bad() const;

  /**
   * The function returns true if either the failbit or the
   * badbit is set.
   *
   * @return true if badbit and/or failbit are set. false
   *         otherwise.
   */
  bool fail() const;

  /**
   * Returns whether an error flag is set (either failbit or
   * badbit).
   *
   * Notice that this function does not return the same as member
   * good, but the opposite of member fail.
   *
   * @return true if none of failbit or badbit is set. false
   *         otherwise.
   */
  operator bool() const;

  /**
   * The function returns true if the eofbit stream's error flag
   * has been set by a previous i/o operation.
   *
   * @return true if eofbit is set. false otherwise.
   */
  bool eof() const;

  /**
   * Modifies the current error state value by combining the error
   * state flags passed as argument with those currently set for
   * the stream.
   *
   * Any error bitflag already set is not cleared. If you want to
   * modify the state flags without keeping the current value, use
   * clear() instead.
   *
   * @param aState An object of type ios_base::iostate that can
   *        take as value any combination of the following state
   *        flag member constants:
   *        - eofbit: End-Of-File reached while performing an
   *          extracting operation on an input stream.
   *        - failbit: The last input operation failed because of
   *          an error related to the internal logic of the
   *          operation itself.
   *        - badbit: Error due to the failure of an input
   *          operation on the stream buffer.
   *        - goodbit: No error. Represents the absence of all the
   *          above (the value zero).
   */
  void setstate(std::ios::iostate aState);

  /**
   * Sets a new value for the error control state.
   *
   * All the bits in the control state are replaced by the new
   * ones; The value existing before the call has no effect. If
   * the function is called with goodbit as argument (which is the
   * default value) all error flags are cleared.
   *
   * @param aState An object of type ios_base::iostate that can
   *        take as value any combination of the following state
   *        flag member constants:
   *        - eofbit: End-Of-File reached while performing an
   *          extracting operation on an input stream.
   *        - failbit: The last input operation failed because of
   *          an error related to the internal logic of the
   *          operation itself.
   *        - badbit: Error due to the failure of an input
   *          operation on the stream buffer.
   *        - goodbit: No error. Represents the absence of all the
   *          above (the value zero).
   */
  void clear(std::ios::iostate aState = std::ios::goodbit);

private:

  void ReadFromFile(bpfChar* aBuffer, streamsize aBufferLength);
  streamsize ReadFromCache(bpfChar* aBuffer, streamsize aBufferLength);
  bool EndOfFileReached();

#if defined(_WIN32)
  bpfInt32 mFile; // the associated file to be read
#else
  FILE* mFile; // the associated file to be read
#endif

  std::ios::iostate mState;
  std::ios::iostate mStateMask;
  streamsize mActualRead; // # of bytes read

  streamsize mCacheSize;
  bpfChar* mCachePosition;
  bpfChar* mCache;
  streamsize mCacheStartPosition;
  static const streamsize mMaxCacheSize = 1024 * 1024;
};

#endif //__BPF_INPUT_FILE_STREAM64__
