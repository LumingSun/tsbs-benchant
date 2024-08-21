// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBLOGINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBLOGINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
// configurable enum
enum LogType {
  LOG_DEBUG,     // 调试
  LOG_RUN,       // 运行
  LOG_OPERATION  // 操作
};

struct KWDBLogParameter {
  // data attributes
  TimeStamp ts_;            // timestamp
  std::string module_name_;  // log source module name
  std::string log_name_;     // log name
  LogType type_;            // log type
  std::string log_content_;  // log description
  KWDBLogParameter() {}
  KWDBLogParameter(TimeStamp ts, const char *moduleName, const char *logName,
    int type, const char *logContent)
    : ts_(ts), module_name_(moduleName), log_name_(logName),
      type_(static_cast<LogType>(type)), log_content_(logContent) {}
};

// TODO(yuyue02): change to a better description
// object for Log service
// will be automatically created when KWDB service is started
// singleton class
class KWDBLogInfo {
 public:
  // TODO(yuyue02): add a description
  // set a new log entry
  virtual ErrorCode setLog(const KWDBLogParameter& logParm) = 0;

  // TODO(yuyue02): add a description
  virtual ErrorCode getLogInRange(
    std::vector<KWDBLogParameter> *logs,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude) = 0;

  // TODO(yuyue02): add a description
  virtual ErrorCode getLatestLog(std::shared_ptr<KWDBLogParameter> *log) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBLOGINFO_H_
