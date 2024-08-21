// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
#ifndef KWDBTS_CLIENT_INCLUDE_DATA_TYPE_H_
#define KWDBTS_CLIENT_INCLUDE_DATA_TYPE_H_

#include <string>
#include <cstddef>
#include <map>

#include "kwdb_common.h"
#include "pg_data_type.h"

namespace kwdbts {

typedef uint32_t ColumnLength;

// the enumation value is the same with server
enum DataType {
  TYPE_TIMESTAMP = 0,
  TYPE_SMALLINT = 1,
  TYPE_INT = 2,
  TYPE_BIGINT = 3,
  TYPE_FLOAT = 4,
  TYPE_DOUBLE = 5,
  TYPE_BOOL = 6,
  TYPE_CHAR = 7,  // 字节型字符串
  TYPE_BINARY = 8,
  TYPE_NCHAR = 9,  // 字符型字符串，每个字符4字节
  TYPE_VARCHAR = 10,
  TYPE_NVARCHAR = 11,  // 变长存储的字符型字符串，每个字符4字节
  TYPE_VARBINARY = 12,
  TYPE_UNKNOWN
};

enum TimeUnit {
  MINUTE,
  HOUR,
  DAY,
  YEAR
};

std::string getDataTypeName(DataType type);

struct EndpointDataType {
  DataType type;
  uint32_t length;
  EndpointDataType() : type(DataType::TYPE_UNKNOWN), length(0) {}
  EndpointDataType(DataType t, uint32_t l) : type(t), length(l) {}
  explicit EndpointDataType(DataType t) : type(t), length(0) {}
};

struct ColumnType {
  std::string name;  // 构造，析构和复制都有成本
  DataType type;
  uint32_t length;
  ColumnType(const std::string &n, DataType t, uint32_t l) :
    name(n),
    type(t),
    length(l) {}
  ColumnType(const char *n, DataType t, uint32_t l) :
    name(n),
    type(t),
    length(l) {}
  ColumnType() : type(DataType::TYPE_UNKNOWN), length(0) {}
  ColumnType(DataType t, uint32_t l) : type(t), length(l) {}
};

}  // namespace kwdbts

#endif  // KWDBTS_CLIENT_INCLUDE_DATA_TYPE_H_
