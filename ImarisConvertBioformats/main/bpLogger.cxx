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


#include "bpLogger.h"

#include "../src/bpWriterCommonHeaders.h"
//#include "common/time/bpTimeInfo.h"
#include "../meta/bpFileTools.h"
#include ICImsUtils_h

#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>

#include <boost/algorithm/string.hpp>


#if defined(_WIN32)
#include <process.h>
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/syscall.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#endif


bpLogger::bpLogger()
  : mLogFileName(""),
    mLogLevel(bpLogger::eLogLevelWarning),
    mSinkFlags(bpLogger::eSinkStdCOut)
{
}


bpLogger::~bpLogger()
{
}


bpLogger* bpLogger::GetInstance()
{
  static bpLogger vLogger;
  return &vLogger;
}


static bpString GetCurrentFilesystemTime()
{
  std::chrono::system_clock::time_point vTimePoint = std::chrono::system_clock::now();
  time_t vTime = std::chrono::system_clock::to_time_t(vTimePoint);
  tm* vLocalTime = localtime(&vTime);
  return bpImsUtils::DateTimeToString(1900 + vLocalTime->tm_year, 1 + vLocalTime->tm_mon, vLocalTime->tm_mday, vLocalTime->tm_hour, vLocalTime->tm_min, vLocalTime->tm_sec);
}

void bpLogger::Log(bpLogger::tLogLevel aLogLevel, const bpString& aMessage)
{
  GetInstance()->LogImpl(aLogLevel, aMessage);
}

void bpLogger::LogImpl(bpLogger::tLogLevel aLogLevel, const bpString& aMessage)
{
  if (mLogLevel == bpLogger::eLogLevelNone || aLogLevel < mLogLevel) {
    return;
  }

  std::ostringstream vLogString;
  vLogString << GetCurrentFilesystemTime() << "\t" << GetProcessId() << "\t" << GetThreadId() << "\t"<< LogLevelToString(aLogLevel) << "\t" << aMessage << std::endl;

  if (mSinkFlags & eSinkStdCOut) {
    LogToStdCout(vLogString.str());
  }

  if (mSinkFlags & eSinkFile) {
    LogToFile(vLogString.str());
  }

  std::map<bpString, bpLoggerSink*>::const_iterator vIt = mSinks.begin();
  while (vIt !=  mSinks.end()) {
    bpLoggerSink* vSink = (*vIt).second;
    vSink->Log(aLogLevel, vLogString.str());
    ++vIt;
  }

  if (mRecordedLogLevels.find(aLogLevel) != mRecordedLogLevels.end()) {
    mRecordedLogMessages[aLogLevel].push_back(vLogString.str());
  }
}

void bpLogger::LogTrace(const bpString& aMessage)
{
  Log(bpLogger::eLogLevelTrace, aMessage);
}

void bpLogger::LogDebug(const bpString& aMessage)
{
  Log(bpLogger::eLogLevelDebug, aMessage);
}

void bpLogger::LogInfo(const bpString& aMessage)
{
  Log(bpLogger::eLogLevelInfo, aMessage);
}


void bpLogger::LogWarning(const bpString& aMessage)
{
  Log(bpLogger::eLogLevelWarning, aMessage);
}


void bpLogger::LogError(const bpString& aMessage)
{
  Log(bpLogger::eLogLevelError, aMessage);
}


void bpLogger::SetSink(tSinkFlags aSinkFlags)
{
  GetInstance()->mSinkFlags = aSinkFlags;
}


void bpLogger::SetMinimumLogLevel(tLogLevel aLogLevel)
{
  GetInstance()->mLogLevel = aLogLevel;
}


void bpLogger::SetMinimumLogLevel(const bpString& aLogLevel)
{
  if (boost::iequals(aLogLevel, "ERROR")) {
    SetMinimumLogLevel(bpLogger::eLogLevelError);
  }
  else if (boost::iequals(aLogLevel, "NONE")) {
    SetMinimumLogLevel(bpLogger::eLogLevelNone);
  }
  else if (boost::iequals(aLogLevel, "WARNING")) {
    SetMinimumLogLevel(bpLogger::eLogLevelWarning);
  }
  else if (boost::iequals(aLogLevel, "DEBUG")) {
    SetMinimumLogLevel(bpLogger::eLogLevelDebug);
  }
  else if (boost::iequals(aLogLevel, "TRACE")) {
    SetMinimumLogLevel(bpLogger::eLogLevelTrace);
  }
  else {
    SetMinimumLogLevel(bpLogger::eLogLevelInfo);
  }
}



void bpLogger::SetLogFileName(const bpString& aLogFileName)
{
  GetInstance()->mLogFileName = aLogFileName;
}


void bpLogger::InstallSink(const bpString& aSinkName, bpLoggerSink* aSink)
{
  if (GetInstance()->mSinks.find(aSinkName) == GetInstance()->mSinks.end()) {
    GetInstance()->mSinks[aSinkName] = aSink;
  }
  else {
    throw std::runtime_error(("bpLogger::InstallSink: sink '" + aSinkName + "'' is already registered.").c_str());
  }
}

bpLogger::tLogLevels bpLogger::GetLogLevels()
{
  bpLogger::tLogLevels vLogLevels{ bpLogger::eLogLevelNone, bpLogger::eLogLevelError, eLogLevelWarning, eLogLevelInfo, eLogLevelDebug, eLogLevelTrace };
  return vLogLevels;
}


bpString bpLogger::GetStringOfLogLevel(bpLogger::tLogLevel aLogLevel)
{
  return GetInstance()->LogLevelToString(aLogLevel);
}


bpString bpLogger::LogLevelToString(bpLogger::tLogLevel aLogLevel)
{
  switch (aLogLevel) {
  case eLogLevelTrace:
    return "TRACE";
    break;
  case eLogLevelDebug:
    return "DEBUG";
    break;
  case eLogLevelWarning:
    return "WARNING";
    break;
  case eLogLevelError:
    return "ERROR";
    break;
  case eLogLevelNone:
    return "NONE";
    break;
  case eLogLevelInfo:
  default:
    return "INFO";
    break;
  }
}


void bpLogger::LogToStdCout(const bpString& aLogMessage)
{
  std::cout << aLogMessage << std::flush;
}


void bpLogger::LogToFile(const bpString& aLogMessage)
{
  bpString vLogFileName = bpFileTools::ConvertSeparators(GetInstance()->mLogFileName);
  if (vLogFileName.empty()) {
    bpString vProcessName = "BitplaneApplications";
    vLogFileName = bpFileTools::ConvertSeparators(vProcessName + ".log");
  }

  std::ofstream vFile;
#ifdef BP_UTF8_FILENAMES
  vFile.open(bpFileTools::FromUtf8Path(vLogFileName).c_str(), std::ofstream::app | std::ios::binary);
#else
  vFile.open(vLogFileName.c_str(), std::ofstream::app | std::ios::binary);
#endif
  if (vFile.good()) {
    vFile << aLogMessage.c_str();
    vFile.close();
  }
}


void bpLogger::EnableMessageRecording(tLogLevel aLogLevel)
{
  GetInstance()->mRecordedLogLevels.insert(aLogLevel);
}


void bpLogger::DisableMessageRecording(tLogLevel aLogLevel)
{
  GetInstance()->mRecordedLogLevels.erase(aLogLevel);
}


void bpLogger::DisableMessageRecording()
{
  GetInstance()->mRecordedLogLevels.clear();
}


const std::vector<bpString>& bpLogger::GetRecordedMessages(tLogLevel aLogLevel)
{
  return GetInstance()->mRecordedLogMessages[aLogLevel];
}


void bpLogger::ClearRecordedMessages(tLogLevel aLogLevel)
{
  return GetInstance()->mRecordedLogMessages[aLogLevel].clear();
}


void bpLogger::ClearRecordedMessages()
{
  return GetInstance()->mRecordedLogMessages.clear();
}


bpUInt64 bpLogger::GetProcessId() const
{
#if defined(_WIN32)
  return _getpid();
#else
  return getpid();
#endif
}


bpUInt64 bpLogger::GetThreadId() const
{
#if defined(_WIN32)
  return GetCurrentThreadId();
#elif defined(__APPLE__)
  return syscall(SYS_thread_selfid);
#elif defined(__linux__)
  return gettid();
#endif
}
