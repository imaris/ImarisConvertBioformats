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


#ifndef __BP_LOGGER__
#define __BP_LOGGER__

#include "ImarisWriter/interface/bpConverterTypes.h"

#include <set>
#include <map>


class bpLoggerSink;

class bpLogger
{
public:

  enum tLogLevel {
    eLogLevelTrace,
    eLogLevelDebug,
    eLogLevelInfo,
    eLogLevelWarning,
    eLogLevelError,
    eLogLevelNone
  };

  enum tSink {
    eSinkStdCOut    = 0x1,
    eSinkFile       = 0x2
  };

  typedef bpUInt32 tSinkFlags;
  typedef std::vector<tLogLevel> tLogLevels;

  /**
   * Writes log information. If no sink is defined output will be written to std::cout.
   * Default log level written to sink is eLogLevelInfo that means if you provide a log level
   * below eLogLevelInfo nothing is written to sink.
   *
   * @param aLogLevel
   * @param aMessage
   */
  static void Log(tLogLevel aLogLevel, const bpString& aMessage);

  static void LogTrace(const bpString& aMessage);
  static void LogDebug(const bpString& aMessage);
  static void LogInfo(const bpString& aMessage);
  static void LogWarning(const bpString& aMessage);
  static void LogError(const bpString& aMessage);

  /**
   * Sets where log information is written to. Allows setting multiple sink by doing a
   * bit-wise OR (|) on tLogLevel.
   *
   * @param aSink
   */
  static void SetSink(tSinkFlags aSinkFlags);

  /**
   * Sets the minimumlog level. Every log level above starting from the minimum log level
   * is written to sink.
   *
   * @param aLogLevel The single aLogLevel to set as minimum.
   */
  static void SetMinimumLogLevel(tLogLevel aLogLevel);
  static void SetMinimumLogLevel(const bpString& aLogLevel);

  /**
   * Set where log file is. If no file name is specified, logger is writing to same directory
   * as the executable is with name [executable].log
   *
   * @param aLogFileName
   * @return
   */
  static void SetLogFileName(const bpString& aLogFileName);

  /**
   * Installs a custom message handler meaning all log messages are passed to this function
   * in addition to the other defined sinks (cout, file).
   *
   * @param aEnabled
   */
  static void InstallSink(const bpString& aSinkName, bpLoggerSink* aSink);

  static tLogLevels GetLogLevels();
  static bpString GetStringOfLogLevel(tLogLevel);

  /**
   * Recording messages (in RAM)
   */
  static void EnableMessageRecording(tLogLevel aLogLevel);
  static void DisableMessageRecording(tLogLevel aLogLevel);
  static void DisableMessageRecording();
  static const std::vector<bpString>& GetRecordedMessages(tLogLevel aLogLevel);
  static void ClearRecordedMessages(tLogLevel aLogLevel);
  static void ClearRecordedMessages();

  virtual ~bpLogger();

private:
  static bpLogger* GetInstance();

  bpLogger();

  void LogImpl(tLogLevel aLogLevel, const bpString& aMessage);
  bpString LogLevelToString(tLogLevel aLogLevel);
  void LogToStdCout(const bpString& aLogMessage);
  void LogToFile(const bpString& aLogMessage);

  bpUInt64 GetProcessId() const;
  bpUInt64 GetThreadId() const;

  bpString mLogFileName;
  tLogLevel mLogLevel;
  tSinkFlags mSinkFlags;
  std::map<bpString, bpLoggerSink*> mSinks;

  // message recording
  typedef std::vector<bpString> tLogMessages;
  std::set<tLogLevel> mRecordedLogLevels;
  std::map<tLogLevel, tLogMessages> mRecordedLogMessages;
};

/**
 * Interface for classes that want to serve as a sink.
 *
 * @brief The bpLoggerSink class
 */
class bpLoggerSink {
public:
  virtual void Log(bpLogger::tLogLevel aLogLevel, const bpString& aLogMessage) = 0;
};

#endif // __BP_LOGGER__


