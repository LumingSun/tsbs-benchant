// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBPROCINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBPROCINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
// configurable enum
enum class ProcStatus {
  PROC_NORMALITY,  // 正常
  PROC_ABNORMITY   // 异常
};

struct KWDBProcParameter {
  // data attributes
  TimeStamp ts_;            // timestamp
  int proc_id_;             // process id
  std::string proc_name_;   // process name
  ProcStatus proc_status_;  // process status
  float running_time_;      // running time
  float memory_used_;       // used size of memory
  float cpu_used_;          // used size of cpu
  float disk_used_;         // used size of disk
  KWDBProcParameter() {}
  KWDBProcParameter(TimeStamp ts, int procId, const char *procName, int procStatus,
    float runningTime, float memoryUsed, float cpuUsed, float diskUsed)
    : ts_(ts), proc_id_(procId), proc_name_(procName), proc_status_(static_cast<ProcStatus>(procStatus)),
      running_time_(runningTime), memory_used_(memoryUsed), cpu_used_(cpuUsed), disk_used_(diskUsed) {}
};

// TODO(yuyue02): change to a better description
// object for process monitor service
// will be automatically created when KWDB service is started
// singleton class
class KWDBProcInfo {
 public:
  // TODO(yuyue02): change to a better description
  // set a new process monitor
  virtual ErrorCode setProc(const KWDBProcParameter& procparm) = 0;

  // TODO(yuyue02): change to a better description
  // query process monitor data in a time range
  virtual ErrorCode getProcInRange(
    std::vector<KWDBProcParameter> *procs,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude) = 0;

  // TODO(yuyue02): change to a better description
  // query most recent process monitor data
  virtual ErrorCode getLatestProc(std::shared_ptr<KWDBProcParameter> *proc) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBPROCINFO_H_
