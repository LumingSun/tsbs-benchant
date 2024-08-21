// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
#ifndef KWDBTS_CLIENT_INCLUDE_KWDBRESULTSET_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBRESULTSET_H_

#include <string>
#include <vector>
#include <memory>

#include "kwdb_common.h"

namespace kwdbts {
class KWDBResultSet {
 public:
  virtual ~KWDBResultSet() = default;

  // getter methods for user to retrieve data and metadata
  virtual std::string getStatus() = 0;
  virtual std::vector<std::string> getHead() = 0;
  virtual std::vector<ColumnType> getColumnMeta() = 0;
  virtual void* getData() = 0;
  virtual int getNumberOfRows() = 0;

  // implement Iterator interface， 模拟resultSet的概念
  virtual bool hasNext() = 0;  // 调用server的next
  virtual const void* next() = 0;  // 不能修改或释放返回值
};
using KWDBResultSetPtr = std::shared_ptr<KWDBResultSet>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBRESULTSET_H_
