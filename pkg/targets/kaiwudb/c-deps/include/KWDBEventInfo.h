// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBEVENTINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBEVENTINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述

// configurable enum
enum EventType {
  DISPATCHEVENT,    // 调度事件
  ALARMEVENT,       // 告警事件
  PROTECTIONEVENT   // 保护事件
};

struct KWDBEventParameter {
  // data attributes
  TimeStamp ts_;              // timestamp
  std::string module_name_;    // event source module name
  std::string event_name_;     // event name
  EventType type_;            // event type
  std::string event_content_;  // event description
  KWDBEventParameter() {}
  KWDBEventParameter(TimeStamp ts, const char *moduleName, const char *eventName,
    int type, const char *eventContent)
    : ts_(ts), module_name_(moduleName), event_name_(eventName), type_(static_cast<EventType>(type)),
      event_content_(eventContent) {}
};

// TODO(yuyue02): change to a better description
// will be automatically created when KWDBE service is started
// singleton class
class KWDBEventInfo {
 public:
  /**
   * set a new event
   *
   * @param eventParm event KWDBEventParameter
   * @return ErrorCode
   */
  virtual ErrorCode setEvent(const KWDBEventParameter& eventParm) = 0;

  /**
   * query event data in a time range
   *
   * @param events  KWDBEventParameter vector
   * @param to      to timestamp
   * @param from    from timestamp
   * @param exclude exclude
   * @return ErrorCode
   */
  virtual ErrorCode getEventInRange(
    std::vector<KWDBEventParameter> *events,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude) = 0;

  /**
   * query most recent event
   * 
   * @param events  KWDBEventParameter shared pointer
   * @return ErrorCode
   */
  virtual ErrorCode getLatestEvent(std::shared_ptr<KWDBEventParameter> *event) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBEVENTINFO_H_
