// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBCONFIGMANAGER_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBCONFIGMANAGER_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"
#include "KWDBPubSub.h"
#include "KWDBPrivilege.h"

namespace kwdbts {

/**
 * KWDBConfigManager interface
 * 
 * KWDB agent provides some system configurations for storage, replication,
 * memory, compute engine, sub/pub, etc. User can use KWDBConfigureManager
 * to get/set these system configurations if the user has the permission to
 * do so. The user having a portal sysAdmin privilege can access the system
 * configurations for the portal and the user having universal sysAdmin
 * privilege can access any portals in KWDB. A portal can be located in
 * KWDBT, KWDBE, or KWDBC, so the system config key has its prefix to indicate
 * which location's config, for example, "KWDBT.storage.min_block_size"
 * indicates that this config is for KWDBT. If the location prefix is
 * missing, the config applies to all KWDBT, KWDBE and KWDBC.
 * Some system configurations can take effect immediately and others need
 * to restart the KWDB agent to take effect. Some system configurations will
 * be persistent after they are changed, but others won't. There will be
 * a document about which will take effect immediately and which will be
 * persistent.
 */
class KWDBConfigManager {
 public:
  /**
   * Get a system configuration value
   *
   * @param key key
   * @param[out] sysConfig system configuration
   * @return errorCode
   */
  virtual ErrorCode getSysConfig(
    const std::string& key,
    std::string *sysConfig) = 0;

  /**
   * Set a system configuration value
   *
   * @param key   key
   * @param value system configuration
   * @return errorCode
   */
  virtual ErrorCode setSysConfig(
    const std::string& key,
    const std::string& value) = 0;

  /**
   * Get all the system configurations that current user can access
   *
   * @param[out] sysConfigList vector of all the system configurations
   * @return errorCode
   */
  virtual ErrorCode listSysConfig(std::vector<std::string> *sysConfigList) = 0;

  /**
   * download portal to EMS box, ems box should connect to cloud
   * before it can be used save into local path
   *
   * @param tenantName tenant name
   * @param portalName portal name
   * @param password Password
   * @param ipAddress IP Address could be Null if in unsafe mode
   * @param pathName   the full path name include file name that user wants to export
   * @return errorCode
   */
  virtual ErrorCode exportPortalConfig(
    const std::string& tenantName,
    const std::string& portalName,
    const std::string& password,
    const std::string& ipAddress,
    const std::string& pathName) = 0;
};
using KWDBConfigManagerPtr = std::shared_ptr<KWDBConfigManager>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBCONFIGMANAGER_H_
