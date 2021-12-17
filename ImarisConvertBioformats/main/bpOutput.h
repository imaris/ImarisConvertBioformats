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


#ifndef __BP_OUTPUT__
#define __BP_OUTPUT__

#include "ImarisWriter/interface/bpConverterTypes.h"

#include <fstream>

/*
* Output wrapper utility to either write to stdout or into a file
*/
class bpOutput
{

public:

  /*
  * Places which define output location
  * Extending them implies extending Write(bpString)
  */
  enum tSink {
    eSinkStdCOut = 0x1,
    eSinkFile   = 0x2
  };

  typedef bpUInt32 tSinkFlags;

  bpOutput();
  ~bpOutput();

  /**
   * Writes out. If no sink is defined output will be written to std::cout.
   *
   * @param aMessage
   */
   void Write(const bpString& aMessage);


  /**
   * Sets where log information is written to. Allows setting multiple sink by doing a
   * bit-wise OR (|) on tLogLevel.
   *
   * @param aSink
   */
  void SetSink(tSinkFlags aSinkFlags);

  /**
   * Set output file.
   * aOverwriteExisting toggles to append to an existing file or overwriting
   *
   * @param aLogFileName
   * @param aOverwriteExisting
   * @return
   */
  void SetOutputFileName(const bpString& aLogFileName);


private:
  void WriteToStdCout(const bpString aLogMessage);
  void WriteToFile(const bpString aLogMessage);
  void OpenFile();

  bpString mOutputFileName;
  tSinkFlags mSinkFlags;
  std::ofstream mFile;

};

#endif // __BP_OUTPUT__
