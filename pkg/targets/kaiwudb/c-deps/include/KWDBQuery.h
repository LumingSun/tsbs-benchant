// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBQUERY_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBQUERY_H_

#include <string>
#include <memory>

#include "KWDBResultSet.h"

namespace kwdbts {
class KWDBQuery;
class QueryBuilder {
 public:
  virtual ~QueryBuilder() {}
  // Setting parameters for KWDBQuery
  static  std::shared_ptr<QueryBuilder> newQueryBuilder();

  virtual void setSelectList(const std::string& selectList) = 0;
  virtual void setAggregationList(const std::string& aggregationList) = 0;
  virtual void setUnionAllFrom(const std::string& unionAllFrom) = 0;
  virtual void setWhere(const std::string& where) = 0;
  virtual void setGroupBy(const std::string& groupBy) = 0;
  virtual void setOrderBy(const std::string& orderBy) = 0;
  virtual void setLimit(const std::string& limit) = 0;
  virtual void setSatisfying(const std::string& satisfy) = 0;

  // Perform syntax check and build KWDBQuery, returns null_ptr if check fails
  virtual std::shared_ptr<kwdbts::KWDBQuery> build() = 0;
  // clear all elements in KWDBQuery
  virtual void clear() = 0;
};

class KWDBQuery {
 public:
  virtual ~KWDBQuery() {}
  // Execute the query and return the result
  virtual KWDBResultSetPtr execute() = 0;

  // Get the string sql statement
  virtual std::string getSqlStatement() = 0;
};

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBQUERY_H_
