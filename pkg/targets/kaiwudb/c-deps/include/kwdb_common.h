// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDB_COMMON_H_
#define KWDBTS_CLIENT_INCLUDE_KWDB_COMMON_H_

#include <cstdint>
#include <string>
#include <memory>
#include <utility>
#include <unordered_map>

#include "data_type.h"

namespace kwdbts {
using ID = int32_t;
using UUID = int64_t;
using TimeStamp = uint64_t;
using Time = uint32_t;
using ErrorCode = std::string;
constexpr size_t MAX_NAME_LEN = 127;
using PairString = std::pair<std::string, std::string>;

enum Exclude {
  BOTH = 0,
  FROM = 1,
  TO = 2,
  NONE = 3
};

enum Aggregation {
  AGG_MAX = 0,
  AGG_MIN = 1,
  AGG_AVG = 2,
  AGG_FIRST = 3,
  AGG_LAST = 4,
  AGG_SUM = 5,
  AGG_STDDEV = 6,
  AGG_COUNT = 7,
  AGG_MEDIAN = 8,
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDB_COMMON_H_
