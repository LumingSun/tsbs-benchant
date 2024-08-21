// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBUSERMANAGER_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBUSERMANAGER_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "kwdb_common.h"
#include "KWDBPubSub.h"
#include "KWDBPrivilege.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
class KWDBUserManager {
 public:
  // user management by admin at instance level
  // remove password part, return username
  virtual ErrorCode createUser(
    const std::string &userName,
    const std::string &passWord) = 0;
   /**
  * @brief 随即重置密码，如果成功，返回重置后的密码
  * 
  * @param user 用户名
  * @param resetPwd 出参，用于保存重置后的密码
  * @return ErrorCode
  */
  virtual ErrorCode resetPassword(
    const std::string &user,
    std::string *resetPwd) = 0;
   /**
  * @brief 删除用户
  * 
  * @param user 用户名
  * @return ErrorCode
  */
  virtual ErrorCode dropUser(const std::string& user) = 0;

  // password management by owner
  virtual ErrorCode changePassword(
    const std::string &user,
    const std::string& newPassword) = 0;

  // list user name under specified scope universal/tenant/portal
  // used by instance admin to get the whole list of users on this instance
  virtual ErrorCode listUsers(std::vector<std::string> *users) = 0;

  // list users with privileges on a certain object
  // list users with privielge to access the instance, used by the admin of the instance
  virtual ErrorCode listUsersWithInstancePrivilege(std::vector<std::string> *users) = 0;
  // list users with privielge to access the tenant, used by the admin of the tenant or instance
  virtual ErrorCode listUsersWithTenantPrivilege(
    const std::string& tenantName,
    std::vector<std::string> *users) = 0;
  // list users with privielge to access the portal, used by the admin of the portal, tenant or instance
  virtual ErrorCode listUsersWithPortalPrivilege(
    const std::string& tenantName,
    const std::string& portalName,
    std::vector<std::string> *users) = 0;

  // result list of pair<std::string target/TYPE, std::string privilege>
  virtual ErrorCode showUserPrivilege(  // used by the admin of the instance
    const std::string &user,
    std::map<std::string, std::string, std::less<>> *privileges) = 0;

  virtual ErrorCode showUserPrivilege(  // used by the admin of the tenant and instance
    const std::string &user,
    const std::string& tenantName,
    std::map<std::string, std::string, std::less<>> *privileges) = 0;

  virtual ErrorCode showUserPrivilege(  // used by the admin of the portal, tenant and instance
    const std::string &user,
    const std::string& tenaneName,
    const std::string& portalName,
    std::map<std::string, std::string, std::less<>> *privileges) = 0;

  // privilege management API
  // amin/owner can grant/revoke the privilege.
  // grantee inherits grantor's scope
  // targetName format: relative path/name/empty
  // example: tenant1.portal1, tenant1
  // empty means grant to privilege type
  virtual ErrorCode grant(
    const std::vector<std::string>& userNames,
    const KWDBPrivilege& privilege,
    const std::string& targetName) = 0;
  virtual ErrorCode revoke(
    const std::vector<std::string>& userNames,
    const KWDBPrivilege& privilege,
    const std::string& targetName) = 0;
};
using KWDBUserManagerPtr = std::shared_ptr<KWDBUserManager>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBUSERMANAGER_H_
