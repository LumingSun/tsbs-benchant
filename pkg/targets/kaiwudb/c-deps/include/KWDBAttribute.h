// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBATTRIBUTE_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBATTRIBUTE_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "kwdb_common.h"
#include "KWDBPubSub.h"
#include "KWDBPrivilege.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述

enum class AttributeScope {
  INSTANCE = 0,
  TENANT = 1,
  PORTAL = 2,
  DEVICE = 3,
  ENDPOINT = 4,
  UN = 5
};

// KAPAttributeManager 管理KWDB用户对象的属性，属性的定义为KWDBAttribute：
class KWDBAttribute {
 private:
  std::string name_;
  AttributeScope scope_;
  std::string description_;
  DataType data_type_;

 public:
  KWDBAttribute(
    const std::string& name, AttributeScope scope,
    const std::string& desc, DataType type) :
    name_(name), scope_(scope), description_(desc), data_type_(type) {}
  // getter method
  std::string getName() const;
  AttributeScope getScope() const;
  std::string getDescription() const;
  DataType getDatatype() const;

// setter method might not need for now
};

// KAPAttributeManager 管理KWDB用户对象的属性，属性的定义为KWDBAttribute：
class KWDBAttributeManager {
 public:
  virtual ErrorCode listAttributes(
    AttributeScope scope,
    std::vector<KWDBAttribute> *attributes) = 0;
  virtual ErrorCode addAttribute(KWDBAttribute newAttribute) = 0;
};
using KWDBAttributeManagerPtr = std::shared_ptr<KWDBAttributeManager>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBATTRIBUTE_H_
