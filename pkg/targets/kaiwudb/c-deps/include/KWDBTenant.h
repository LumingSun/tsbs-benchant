// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBTENANT_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBTENANT_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kwdb_common.h"
#include "KWDBPortal.h"
#include "KWDBQuery.h"
#include "KWDBNamedQuery.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
class KWDBTenant {
 public:
  // getter method
  virtual std::string getTenantName() const = 0;
  // API to manage attributes associated with this tenant
  virtual ErrorCode listAttributes(std::vector<PairString> *attributes) = 0;
  virtual ErrorCode getAttribute(
    const std::string& attributeName,
    std::string *attribute) = 0;
  virtual ErrorCode setAttribute(
    const std::string& attributeName,
    const std::string& attributeValue) = 0;

  // API to manage portals under this KWDBTenant
  virtual ErrorCode listPortals(std::vector<KWDBPortalPtr>*) = 0;
  virtual ErrorCode createPortal(
    const std::string& portalName,
    std::shared_ptr<KWDBPortal> *portal) = 0;
  // drop portal can only succeed
  // when no devices/endpoints are under this portal
  // otherwise the dropPortal will fail
  virtual ErrorCode dropPortal(const KWDBPortalPtr& portal) = 0;
  virtual ErrorCode lookupPortal(
    const std::string& portalName,
    std::shared_ptr<KWDBPortal> *portal) = 0;

  // shortcuts for device managements
  // list all the devices in this tenant
  virtual ErrorCode listDevices(
    std::vector<KWDBDevicePtr> *devices) = 0;
  // list all the devices under one specific portal
  virtual ErrorCode listDevices(
    const std::string& portalName,
    std::vector<KWDBDevicePtr> *devices) = 0;

  // shortcuts for endpoint managements
  // list all the endpoints in this tenant
  virtual ErrorCode listEndpoints(
    std::vector<KWDBEndpointPtr> *endpoints) = 0;
  // list endpoints under one specific portal
  virtual ErrorCode listEndpoints(
    const std::string& portalName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;
  // list endpoints under one specific device
  virtual ErrorCode listEndpoints(
    const std::string& portalName,
    const std::string& deviceName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;

  // query api
  virtual ErrorCode createQuery(
    const std::shared_ptr<QueryBuilder> &builder,
    std::shared_ptr<KWDBQuery> *query) = 0;
  // TODO(Adrian): Change createNamedQuery according to updated DCR
  // named query api
  // createNamedQuery need manage privilege on all objects in the query
  virtual ErrorCode createNamedQuery(
    const std::string& name,
    const std::shared_ptr<KWDBQuery>& query,
    bool enable,
    KWDBNamedQueryPtr *nameQuery) = 0;
  virtual ErrorCode listNamedQuery(std::vector<KWDBNamedQueryPtr> *nameQueries) = 0;
  virtual ErrorCode dropNamedQuery(const std::string& name) = 0;
};
using KWDBTenantPtr = std::shared_ptr<KWDBTenant>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBTENANT_H_
