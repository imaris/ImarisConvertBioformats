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


#include "bpOutput.h"

#include "../meta/bpFileTools.h"
#include <exception>

#include <iostream>


bpOutput::bpOutput()
: mOutputFileName(""),
  mSinkFlags(eSinkStdCOut)
{

}


bpOutput::~bpOutput()
{
  if (mFile.is_open()) {
    mFile.close();
  }
}


void bpOutput::Write(const bpString& aMessage)
{
  if (mSinkFlags & eSinkStdCOut) {
    WriteToStdCout(aMessage);
  }

  if (mSinkFlags & eSinkFile) {
    WriteToFile(aMessage);
  }
}


void bpOutput::SetSink(tSinkFlags aSinkFlags)
{
  mSinkFlags = aSinkFlags;
}


void bpOutput::SetOutputFileName(const bpString& aOutputFileName)
{
  if (mOutputFileName != aOutputFileName && mFile.is_open()) {
    mFile.close();
  }
  mOutputFileName = aOutputFileName;
}


void bpOutput::WriteToStdCout(const bpString aContent)
{
  std::cout << aContent << std::flush;
}


void bpOutput::WriteToFile(const bpString aContent)
{
  OpenFile();
  if (mFile.good()) {
    mFile << aContent.c_str();
  }
  else {
    throw std::runtime_error("Unable to write output file " + mOutputFileName);
  }
}


void bpOutput::OpenFile()
{
  if (!mFile.is_open()) {

    bpString vOSFileName = bpFileTools::ConvertSeparators(mOutputFileName);
#ifdef BP_UTF8_FILENAMES
    mFile.open(bpFileTools::FromUtf8Path(vOSFileName).c_str(), std::ofstream::trunc | std::ios::binary);
#else
    mFile.open(vOSFileName.c_str(), std::ofstream::trunc | std::ios::binary);
#endif

  }
}
