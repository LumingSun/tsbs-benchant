// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBINSTANCE_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBINSTANCE_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kwdb_common.h"
#include "KWDBPortal.h"
#include "KWDBTenant.h"
#include "KWDBQuery.h"
#include "KWDBNamedQuery.h"

namespace kwdbts {

/**
 * KWDBInstance interface
 *
 * A KWDBInstance is a single KWDB cluster deployment with a cluster of KWDB nodes in the cloud
 * and many managed microgrid controllers on edge. It consists of multiple tenants.
 *
 */
class KWDBInstance {
 public:
  virtual ~KWDBInstance() {}

  /**
   * get instance name
   *
   * @return instanceName
   */
  virtual std::string getInstanceName() const = 0;

  /**
   * list all the attributes associated with this instance
   *
   * @param[out] attribute list
   * @return errorCode
   */
  virtual ErrorCode listAttributes(std::vector<PairString> *attributes) = 0;

  /**
   * get attribute, fetch metadata information from kwdb
   *
   * @param attributeName attribute name
   * @param[out] attribute value
   * @return errorCode
   */
  virtual ErrorCode getAttribute(
    const std::string& name,
    std::string *attribute) = 0;

  /**
   * set attribute, save instance related metadata to kwdb
   *
   * @param name  attribute name
   * @param attributeValue attribute value
   * @return errorCode
   */
  virtual ErrorCode setAttribute(
    const std::string& name,
    const std::string& attribute) = 0;

  // API to manage tenants under this KWDBInstance
  /**
   * list all tenants in this KWDB instance
   *
   * @param[out] tenants vector of KWDBTenantPtr
   * @return errorCode
   */
  virtual ErrorCode listTenants(std::vector<KWDBTenantPtr> *tenants) = 0;

  /**
   * create a new tenant in this KWDB instance
   *
   * @param tenantName tenant name
   * @param[out] tenant pointer of KWDBTenant
   * @return ErrorCode
   */
  virtual ErrorCode createTenant(
    const std::string& tenantName,
    KWDBTenantPtr *tenant) = 0;

  /**
   * drop a tenant from KWDB system
   * drop will fail if under the tenant there still exist portal/device/endpoint
   * 
   * @param tenant tenant
   * @return ErrorCode
   */
  virtual ErrorCode dropTenant(const KWDBTenantPtr& tenant) = 0;

  /**
   * look up a tenant in this KWDB instance
   *
   * @param tenantName tenant name
   * @param[out] KWDBTenant
   * @return ErrorCode
   */
  virtual ErrorCode lookupTenant(
    const std::string& tenantName,
    KWDBTenantPtr *tenant) = 0;

  //// The following function will not be supported in v1.0
  /**
   * shortcuts for portal managements
   * this can also be done under KWDBTenant object
   * list all the portals in this instance
   * 
   * @param[out] portals KWDBPortal list
   * @return errorCode
   */
  virtual ErrorCode listPortals(
    std::vector<KWDBPortalPtr> *portals) = 0;  // [read]

  /**
   * list all the portals under one tenant
   * 
   * @param tenantName tenant name
   * @param[out] portals KWDBPortal list
   * @return errorCode
   */
  virtual ErrorCode listPortals(
    const std::string& tenantName,
    std::vector<KWDBPortalPtr> *portals) = 0;

  // shortcuts for device managements
  // list all the devices in this instance
  /**
   * list all the devices in this instance
   *
   * @param[out] KWDBDevice list
   * @return errorCode
   */
  virtual ErrorCode listDevices(
    std::vector<KWDBDevicePtr> *devices) = 0;  // [read ]

  /**
   * list all the devices under one specific tenant
   *
   * @param tenantName tenant name
   * @param[out] KWDBDevice list
   * @return errorCode
   */
  virtual ErrorCode listDevices(
    const std::string& tenantName,
    std::vector<KWDBDevicePtr> *devices) = 0;

  /**
   * list devices under one specific portal
   *
   * @param tenantName tenant name
   * @param portalName portal name
   * @param[out] KWDBDevice list
   * @return errorCode
   */
  virtual ErrorCode listDevices(
    const std::string& tenantName,
    const std::string& portalName,
    std::vector<KWDBDevicePtr> *devices) = 0;

  // shortcuts for endpoint managements
  // list all the endpoints in this instance
  virtual ErrorCode listEndpoints(
    std::vector<KWDBEndpointPtr> *endpoints) = 0;  // [read ]
  // list all the endpoints under one specific tenant
  virtual ErrorCode listEndpoints(
    const std::string& tenantName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;
  // list endpoints under one specific portal
  virtual ErrorCode listEndpoints(
    const std::string& tenantName,
    const std::string& portalName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;
  // list endpoints under one specific device
  virtual ErrorCode listEndpoints(
    const std::string& tenantName,
    const std::string& portalName,
    const std::string& deviceName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;
  //// end of list of functions that will not be supported in v1.0

  /**
   * create KWDBQuery
   *
   * @param builder QueryBuilder
   * @param[out] query shared pointer of KWDBQuery
   * @return errorCode
   */
  virtual ErrorCode createQuery(
    const std::shared_ptr<QueryBuilder> &builder,
    std::shared_ptr<KWDBQuery> *query) = 0;

  /**
   * create KWDBNamedQuery
   *
   * @param name   name
   * @param query  KWDBQuery
   * @param enable enable/disable
   * @param[out] nameQuery shared pointer of KWDBNamedQuery
   * @return errorCode
   */
  // TODO(Adrian): Change createNamedQuery according to updated DCR
  virtual ErrorCode createNamedQuery(
    const std::string& name,
    const std::shared_ptr<KWDBQuery>& query,
    bool enable,
    KWDBNamedQueryPtr *nameQuery) = 0;

  /**
   * list all the KWDBNamedQuery in this instance
   *
   * @param[out] nameQueries vector of KWDBNamedQuery
   * @return errorCode
   */
  virtual ErrorCode listNamedQuery(std::vector<KWDBNamedQueryPtr> *nameQueries) = 0;

  /**
   * drop KWDBNamedQuery
   *
   * @param name name of Named Query
   * @return errorCode
   */
  virtual ErrorCode dropNamedQuery(const std::string& name) = 0;
};
using KWDBInstancePtr = std::shared_ptr<KWDBInstance>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBINSTANCE_H_
