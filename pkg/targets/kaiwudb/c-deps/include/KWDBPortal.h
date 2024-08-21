// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBPORTAL_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBPORTAL_H_

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "kwdb_common.h"
#include "KWDBDevice.h"
#include "KWDBAlarmInfo.h"
#include "KWDBEventInfo.h"
#include "KWDBLogInfo.h"
#include "KWDBSysInfo.h"
#include "KWDBProcInfo.h"
#include "KWDBHardwareInfo.h"
#include "KWDBQuery.h"
#include "KWDBNamedQuery.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
class KWDBPortal {
 public:
  /**
   * get portal name
   *
   * @return portal_name_
   */
  virtual std::string getPortalName() const = 0;
  /**
   * set lifetime
   *
   * @param lifetime lifetime
   * @param unit     lifetime unit
   */
  virtual ErrorCode setLifetime(const int lifetime, const TimeUnit unit) = 0;

  /**
   * get lifetime
   * @param[out] lifetime
   * @param unit lifetime unit
   * @return errorCode
   */
  virtual ErrorCode getLifetime(float* lifetime, const TimeUnit unit) = 0;

  /**
   * list all the attributes associated with this portal
   * 
   * @param[out] attributes vector of attributes
   * @return errorCode
   */
  virtual ErrorCode listAttributes(std::vector<PairString> *attributes) = 0;

  /**
   * get attribute, fetch metadata information from kwdb under this portal
   * 
   * @param attributeName attribute name
   * @param[out] attribute pointer of attribute
   * @return errorCode
   */
  virtual ErrorCode getAttribute(
    const std::string& attributeName,
    std::string *attribute) = 0;

  /**
   * set attribute, save portal related metadata to kwdb
   * 
   * @param attributeName attribute name
   * @param attributeValue attribute value
   * @return errorCode
   */
  virtual ErrorCode setAttribute(
    const std::string& attributeName,
    const std::string& attributeValue) = 0;

  // API to manage devices under this portal
  /**
   * list all devices under this KWDB portal
   *
   * @param[out] devices vector of pointer of KWDBDevice
   * @return errorCode
   */
  virtual ErrorCode listDevices(std::vector<KWDBDevicePtr> *devices) = 0;

  /**
   * create a new device in this KWDB portal
   *
   * @param deviceName  device name
   * @param[out] device pointer of KWDBDevice
   * @return errorCode
   */
  virtual ErrorCode createDevice(
    const std::string& deviceName,
    std::shared_ptr<KWDBDevice> *device) = 0;

  /**
   * create a new device in this KWDB portal
   *
   * @param deviceName  device name
   * @param activeTime active time, "-1" to using system default setting
   * @param[out] device pointer of KWDBDevice
   * @return errorCode
   */
  virtual ErrorCode createDevice(
    const std::string& deviceName,
    int activeTime,
    std::shared_ptr<KWDBDevice> *device) = 0;

  /**
   * drop device can only be done when no endpoints exists inside
   * also need to invalidate all related named query internal
   *
   * @param device pointer of KWDBDevice
   * @return errorCode
   */
  virtual ErrorCode dropDevice(const KWDBDevicePtr& device) = 0;

  /**
   * look up a device in this KWDB portal
   *
   * @param deviceName device name
   * @param[out] device pointer of KWDBDevice
   * @return errorCode
   */
  virtual ErrorCode lookupDevice(
    const std::string& deviceName,
    std::shared_ptr<KWDBDevice> *device) = 0;

  /**
   * list all the endpoints in this portal
   * shortcuts for endpoint managements
   *
   * @param[out] endpoints vector of pointers of KWDBEndpoint
   * @return errorCode
   */
  virtual ErrorCode listEndpoints(
    std::vector<KWDBEndpointPtr> *endpoints) = 0;  // [read]

  /**
   * list endpoints under one specific device
   * 
   * @param deviceName device name
   * @param[out] endpoints vector of pointers of KWDBEndpoint
   * @return errorCode
   */
  virtual ErrorCode listEndpoints(
    const std::string& deviceName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;  // [read]

  // get predefined system object
  /**
   * get alarm info
   *
   * @return pointer of KWDBAlarmInfo
   */
  virtual std::shared_ptr<KWDBAlarmInfo> getAlarmInfo() = 0;

  /**
   * get event info
   *
   * @return pointer of KWDBEventInfo
   */
  virtual std::shared_ptr<KWDBEventInfo> getEventInfo() = 0;

  /**
   * get log info
   *
   * @return pointer of KWDBLogInfo
   */
  virtual std::shared_ptr<KWDBLogInfo> getLogInfo() = 0;

  /**
   * get system info
   *
   * @return pointer of KWDBSysInfo
   */
  virtual std::shared_ptr<KWDBSysInfo> getSysInfo() = 0;

  /**
   * get process info
   *
   * @return pointer of KWDBProcInfo
   */
  virtual std::shared_ptr<KWDBProcInfo> getProcInfo() = 0;

  /**
   * get hardware info
   *
   * @return pointer of KWDBHardwareInfo
   */
  virtual std::shared_ptr<KWDBHardwareInfo> getHardwareInfo() = 0;

  // query api
  virtual ErrorCode createQuery(
    const std::shared_ptr<QueryBuilder> &builder,
    std::shared_ptr<KWDBQuery> *query) = 0;
  // TODO(Adrian): Change createNamedQuery according to updated DCR
  // namedquery management
  // createNamedQuery need manage privilege on all objects in the query
  /**
   * create KWDBNamedQuery
   *
   * @param name   name
   * @param query  pointer of KWDBQuery
   * @param enable enable/disable
   * @param[out] namequery pointer of KWDBNamedQuery
   * @return errorCode
   */
  virtual ErrorCode createNamedQuery(
    const std::string& name,
    const std::shared_ptr<KWDBQuery>& query,
    bool enable,
    KWDBNamedQueryPtr *namequery) = 0;

  /**
   * list all the KWDBNamedQuery in this portal
   *
   * @param[out] nameQueries vector of KWDBNamedQuery
   * @return errorCode
   */
  virtual ErrorCode listNamedQuery(std::vector<KWDBNamedQueryPtr> *nameQueries) = 0;

  /**
   * drop KWDBNamedQuery
   *
   * @param name name
   * @return errorCode
   */
  virtual ErrorCode dropNamedQuery(const std::string& name) = 0;
};
using KWDBPortalPtr = std::shared_ptr<KWDBPortal>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBPORTAL_H_
