// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
#ifndef KWDBTS_CLIENT_INCLUDE_KWDBPRIVILEGE_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBPRIVILEGE_H_

#include <array>
#include <string>

#include "kwdb_common.h"

namespace kwdbts {

// Privilege enumeration
enum Privilege {
  ADMIN,   // administration privilege
  MANAGE,  // management privileges, include data injection, drop/alter
  READ,     // data access only for analysis
  NUM_PRIVILEGE
};

// Privilege name
static std::array<const std::string, Privilege::NUM_PRIVILEGE> PrivilegeName = {
  "admin",
  "manage",
  "read"
};

// Privilege level
enum class PrivilegeType {
  INSTANCE,
  TENANT,   // tenant level, can see all the portals under this tenant
  PORTAL    // portal level, can see all the devices under this portal
};

class KWDBPrivilege {
 private:
  Privilege privilege_;
  PrivilegeType type_;

 public:
  KWDBPrivilege(Privilege pr, PrivilegeType pt) :
    privilege_(pr), type_(pt) {
  }

  // getter method
  Privilege getPrivilege() const;
  PrivilegeType getPrivilegeType() const;


  // setter method
  ErrorCode setPrivilege(Privilege pri);
  ErrorCode setPrivilegeLevel(PrivilegeType priLeveType);
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBPRIVILEGE_H_
