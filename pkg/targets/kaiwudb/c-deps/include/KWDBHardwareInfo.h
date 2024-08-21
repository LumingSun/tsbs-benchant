// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBHARDWAREINFO_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBHARDWAREINFO_H_

#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
struct KWDBHardwareParameter {
  // data attributes
  TimeStamp ts_;              // timestamp
  std::string device_sn_;     // device sn
  std::string device_type_;   // device type
  int lan_number_;            // lan number
  std::string lan_status_;    // lan status
  int usb_number_;            // usb number
  std::string usb_status_;    // usb status
  int can_number_;            // can number
  std::string can_status_;    // can status
  int rs485_number_;          // rs485 number
  std::string rs485_status_;  // rs485 status
  int di_number_;             // di number
  std::string di_status_;     // di status
  int do_number_;             // do number
  std::string do_status_;     // do status
  std::string gps_status_;    // gps status
  std::string hdmi_status_;   // hdmi status
  KWDBHardwareParameter() {}
  KWDBHardwareParameter(TimeStamp ts, const char *deviceSn, const char *deviceType,
    int lanNumber, const char *lanStatus, int usbNumber, const char *usbStatus,
    int canNumber, const char *canStatus, int rs485Number, const char *rs485Status,
    int diNumber, const char *diStatus, int doNumber, const char *doStatus,
    const char *gpsStatus, const char *hdmiStatus)
    : ts_(ts), device_sn_(deviceSn), device_type_(deviceType), lan_number_(lanNumber), lan_status_(lanStatus),
      usb_number_(usbNumber), usb_status_(usbStatus), can_number_(canNumber), can_status_(canStatus),
      rs485_number_(rs485Number), rs485_status_(rs485Status), di_number_(diNumber), di_status_(diStatus),
      do_number_(doNumber), do_status_(doStatus), gps_status_(gpsStatus), hdmi_status_(hdmiStatus) {}
};

// TODO(yuyue02): change to a better description
// KWDBHardwareInfo
// object for hardware monitor service
// will be automatically created when KWDB service is started
// singleton class
class KWDBHardwareInfo {
 public:
  /**
   * set a new hardware
   *
   * @param parameter hardware parameter
   * @return ErrorCode
   */
  virtual ErrorCode setHardware(const KWDBHardwareParameter& hardwareparm) = 0;


  /**
   * query hardware data in a time range
   * 
   * @param hardwares KWDBHardwareParameter vector
   * @param to      to timestamp
   * @param from    from timestamp
   * @param exclude exclude
   * @return ErrorCode
   */
  virtual ErrorCode getHardwareInRange(
    std::vector<KWDBHardwareParameter> *hardwares,
    TimeStamp to,
    TimeStamp from,
    Exclude exclude) = 0;

  /**
   * query most recent hardware
   * 
   * @param hardware KWDBHardwareParameter shared pointer
   * @return ErrorCode
   */
  virtual ErrorCode getLatestHardware(
    std::shared_ptr<KWDBHardwareParameter> *hardware) = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBHARDWAREINFO_H_
