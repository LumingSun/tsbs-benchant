// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBSYSINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBSYSINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
struct KWDBSysParameter {
  // data attributes
  TimeStamp ts_;          // timestamp
  int64_t running_time_;  // running time
  float memory_total_;    // total memory
  float memory_used_;     // used memory
  float cpu_core_;        // cpu core number
  float cpu_load_;        // cpu load in 15 mins
  float flash_total_;     // total size of flash
  float flash_used_;      // used size of flash
  float disk_total_;      // total size of disk
  float disk_used_;       // used size of disk
  KWDBSysParameter() {}
  KWDBSysParameter(TimeStamp ts, int64_t runningTime, float memoryTotal, float memoryUsed,
    float cpuCore, float cpuLoad, float flashTotal, float flashUsed, float diskTotal, float diskUsed)
    : ts_(ts), running_time_(runningTime), memory_total_(memoryTotal), memory_used_(memoryUsed),
      cpu_core_(cpuCore), cpu_load_(cpuLoad), flash_total_(flashTotal), flash_used_(flashUsed),
      disk_total_(diskTotal), disk_used_(diskUsed) {}
};

// TODO(yuyue02): change to a better description
// object for system monitor service
// will be automatically created when KWDB service is started
// singleton class
class KWDBSysInfo {
 public:
  // TODO(yuyue02): change to a better description
  // set a new system monitor
  virtual ErrorCode setSys(const KWDBSysParameter& sysparm) = 0;

  // TODO(yuyue02): change to a better description
  // query system monitor data in a time range
  virtual ErrorCode getSysInRange(
    std::vector<KWDBSysParameter> *sys,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude) = 0;

  // TODO(yuyue02): change to a better description
  // query most recent system monitor data
  virtual ErrorCode getLatestSys(std::shared_ptr<KWDBSysParameter> *sys) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBSYSINFO_H_
