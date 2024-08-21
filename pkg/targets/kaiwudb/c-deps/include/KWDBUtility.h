// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBUTILITY_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBUTILITY_H_

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
class KWDBUtility {
 public:
  // get service docs, including os, system, cluster status, log file etc.
  virtual std::string getServiceDoc() = 0;
};
using KWDBUtilityPtr = std::shared_ptr<KWDBUtility>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBUTILITY_H_
