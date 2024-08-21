// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBNAMEDQUERY_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBNAMEDQUERY_H_

#include <string>
#include <memory>

#include "KWDBQuery.h"

namespace kwdbts {
class KWDBNamedQuery;

class NamedQueryBuilder: public virtual QueryBuilder {
 public:
  virtual ~NamedQueryBuilder() {}
  static  std::shared_ptr<NamedQueryBuilder> newNamedQueryBuilder();
  virtual void setName(const std::string& name) = 0;

  // Build named query, returns null_ptr if check fails
  virtual std::shared_ptr<kwdbts::KWDBNamedQuery> buildNamedQuery() = 0;
};

class KWDBNamedQuery: public virtual KWDBQuery {
 public:
  virtual ~KWDBNamedQuery() {}
  virtual std::string getName() = 0;
  virtual bool getValidity() = 0;
};
using KWDBNamedQueryPtr = std::shared_ptr<KWDBNamedQuery>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBNAMEDQUERY_H_
