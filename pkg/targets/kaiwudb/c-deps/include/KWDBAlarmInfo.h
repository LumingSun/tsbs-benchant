// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBALARMINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBALARMINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
// configurable enum
enum class AlarmType {
  ALARM_ENUMERATION,  // 枚举
  ALARM_RECORD,     // 记录
  ALARM_SHIFT,    // 变位
  ALARM_OVERFLOW,   // 越限
  ALARM_PROTECTION,   // 保护
  ALARM_ABNORMITY   // 异常
};

enum AlarmStatus {
  ALARM_CREATION,
  ALARM_AUTORESET,
  ALARM_MANUALRESET
};

enum AlarmLevel {
  ALARM_LEVEL_ONE,
  ALARM_LEVEL_TWO,
  ALARM_LEVEL_THREE
};

// 3 level threshold alarm definition
// clarify with ems team..
// up to 5
template<typename T>
struct KWDBAlarmThreshold {
  T alarmThresholdLevelOne;
  T alarmThresholdLevelTwo;
  T alarmThresholdLevelThree;
  bool saveToAlarmInfo;
};

struct KWDBAlarmParameter {
  // data attributes
  TimeStamp ts_;              // timestamp
  std::string module_name_;    // alarm source module name
  std::string alarm_name_;     // alarm name
  AlarmType type_;            // alarm type
  AlarmLevel level_;          // alarm level
  AlarmStatus status_;        // alarm status
  std::string alarm_value_;    // alarm value
  std::string alarm_content_;  // alarm description
  std::string key1_;          // device
  std::string key1_name_;      // device name
  std::string key2_;          // point
  std::string key2_name_;      // point name
  TimeStamp reset_time_;       // reset time
  int reset_value_;            // reset value
  KWDBAlarmParameter() {}
  KWDBAlarmParameter(TimeStamp ts, const char *moduleName, const char *alarmName, int type,
    int level, int status, const char *alarmValue, const char *alarmContent,
    const char *key1, const char *key1Name, const char *key2, const char *key2Name,
    TimeStamp resetTime, int resetValue)
    : ts_(ts), module_name_(moduleName), alarm_name_(alarmName), type_(static_cast<AlarmType>(type)),
      level_(static_cast<AlarmLevel>(level)), status_(static_cast<AlarmStatus>(status)),
      alarm_value_(alarmValue), alarm_content_(alarmContent), key1_(key1), key1_name_(key1Name),
      key2_(key2), key2_name_(key2Name), reset_time_(resetTime), reset_value_(resetValue) {}
};

// object for Alarm service
// will be automatically created when KWDB service is started
// singleton class
class KWDBAlarmInfo {
 public:
  /**
  * set a new alarm
  *
  * @param parameter alarm KWDBAlarmParameter
  * @return ErrorCode
  */
  virtual ErrorCode setAlarm(const KWDBAlarmParameter& parm) = 0;

  /**
   * query alarm data in a time range
   *
   * @param alarms     KWDBAlarmParameter pointer
   * @param to         to timestamp
   * @param from       from timestamp
   * @param exclude    exclude
   * @param alarmType  alarm type (default: 0)
   * @param alarmLevel alarm level(default: 1)
   * @return KWDBAlarmParameter set
   * @return ErrorCode
   */
  virtual ErrorCode getAlarmInRange(
    std::vector<KWDBAlarmParameter> *alarms,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude,
    AlarmType type = AlarmType::ALARM_ENUMERATION,  // default 0
    AlarmLevel level = ALARM_LEVEL_TWO)  // default 1
    = 0;

  /**
   * query most recent unreset alarm
   *
   * @param alarms KWDBAlarmParameter vector
   * @param key1 key1
   * @param key2 key2
   * @return ErrorCode
   */
  virtual ErrorCode getLatestAlarm(
    std::shared_ptr<KWDBAlarmParameter> *alarm,
    const std::string& key1,
    const std::string& key2) = 0;

  /**
   * reset alarm
   *
   * @param parm KWDBAlarmParameter
   * @return ErrorCode
   */
  virtual ErrorCode resetAlarm(const KWDBAlarmParameter& parm) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBALARMINFO_H_
