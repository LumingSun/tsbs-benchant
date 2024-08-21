// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBCONNECTION_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBCONNECTION_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"
#include "KWDBError.h"
#include "KWDBInstance.h"
#include "KWDBTenant.h"
#include "KWDBPortal.h"
#include "KWDBUserManager.h"
#include "KWDBConfigManager.h"
#include "KWDBAttribute.h"
#include "KWDBUserManager.h"
#include "KWDBPubSub.h"
#include "KWDBPrivilege.h"
#include "libpq-fe.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
class KWDBConnection {
 public:
  // connect to KWDB server
  static ErrorCode connect(
    const std::string &URL,
    const std::string &user,
    const std::string &passWord,
    std::shared_ptr<KWDBConnection> *KWDBConn);

  virtual ErrorCode close() = 0;

  virtual ~KWDBConnection() {}
  // 建立链接的时候会收到memory segment key
  // 有key直接用，如没有去kwbase取
  // get current Instance
  virtual ErrorCode getCurInstance(std::shared_ptr<KWDBInstance> *instance) = 0;
  // get current tenant
  virtual ErrorCode getCurTenant(std::shared_ptr<KWDBTenant> *tenant) = 0;
  // get current portal
  virtual ErrorCode getCurPortal(std::shared_ptr<KWDBPortal> *portal) = 0;
  // get current user name
  virtual ErrorCode getCurUser(std::string *user) = 0;
  // get value of session variables like "application_name"
  virtual ErrorCode getSessionVariable(const std::string &name, std::string *value) = 0;

  // always KWBase
  virtual ErrorCode setCurInstance(const std::string&) = 0;
  // set session current tenant
  virtual ErrorCode setCurTenant(const std::string&) = 0;
  // set session current portal
  virtual ErrorCode setCurPortal(const std::string&) = 0;
  // set session current user
  virtual ErrorCode setCurUser(const std::string&) = 0;
  // set value of session variables like "application_name"
  virtual ErrorCode setSessionVariable(const std::string &name, const std::string &value) = 0;

  virtual KWDBUserManagerPtr getUserManager() = 0;
  // get Config manager
  virtual KWDBConfigManagerPtr getConfigManager() = 0;
  // get Attribute manager
  virtual KWDBAttributeManagerPtr getAttributeManager() = 0;
  // get pubsub service
  virtual KWDBPubSubPtr getPubSub() = 0;
  // get kwdb error
  virtual KWDBErrorPtr getKWDBError() = 0;
  // get kwbase connection
  virtual PGconn* getKwbaseConn() = 0;
};
using KWDBConnectionPtr = std::shared_ptr<KWDBConnection>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBCONNECTION_H_
