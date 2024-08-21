// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBERROR_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBERROR_H_

#include <stdarg.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>
#include <memory>

#include "kwdb_common.h"

namespace kwdbts {
typedef enum KWDBClientErrorCode {
  // ErrorCode for success
  CL_SUCCESS_CODE                  = 0,
  // Public client error codes
  // Start with 13000, end with 13099 to avoid conflicts with Postgre/Server error codes
  CL_START                      = 13000,
  CL_BAD_CONN_ERROR,
  // Data errors
  CL_DATA_TYPE_ERR              = 13010,
  CL_DATA_CASTING_ERR,
  CL_DATA_FORMAT_ERR,
  CL_DATA_NOT_EXIST_ERR,
  CL_EMPTY_DATA,

  // System errors
  CL_BAD_ALLOC_ERR             = 13020,

  // User misusing functions, passing invalid params
  CL_CMD_ERR                    = 13030,
  CL_PARAM_NULL_ERR,
  CL_PARAM_INVALID_ERR,
  CL_PARAM_INVALID_TIMESTAMP_ERR,

  CL_PERMISSION_ERR             = 13040,

  CL_RESOURCE_ERR               = 13050,
  CL_USE_CREATING_ENDPOINT_ERR,

  // Validation errors
  CL_PATH_VALIDATION_ERR        = 13060,
  CL_OBJ_VALIDATION_ERR,

  CL_IO_ERR                     = 13065,

  // Metadata errors
  CL_METADATA_INVALID_ERR       = 13070,
  CL_METADATA_OUT_OF_DATE_ERR,
  CL_METADATA_NOT_CRT_ERR,
  CL_METADATA_MISS_ERR,
  CL_METADATA_ALR_CRT_ERR,

  CL_UNSUPPORTED_OPERATION_ERR  = 13080,

  CL_SYS_ERR                    = 13090,

  CL_LOG_WRT_ERR                 = 13095,
  CL_LOG_RD_ERR,
  CL_LOG_CONFIG_ERR,

  CL_UNKNOWN_ERR                = 13098,
  CL_LAST                       = 13099
} KWDBClientErrorCode;

static const ErrorCode KWDB_SUCCESS_CODE{"00000"};

class KWDBError {
 public:
  virtual ~KWDBError() {}

  // 减少string的构造和析构，或者char，返回完整的全部error
  // 直接从shared memory去掉，通过connection handle去拿所有的error
  // 如果期间做了操作，error stack就不在了（有可能）
  // 有connection pool的时候就有可能出问题
  // 发送errorCode，connection handle（包含二维数组）的下标
  // 接口pass in handle
  // connection handle
  virtual std::string getErrorMessage(const ErrorCode& error) = 0;  // server那边会提供template和token
  virtual std::string getError(const ErrorCode& error) = 0;  // return whole errorcode + message
};
using KWDBErrorPtr = std::shared_ptr<KWDBError>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBERROR_H_
