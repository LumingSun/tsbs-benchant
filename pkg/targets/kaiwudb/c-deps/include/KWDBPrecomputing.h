// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
#ifndef KWDBTS_CLIENT_INCLUDE_KWDBPRECOMPUTING_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBPRECOMPUTING_H_

#include <array>
#include <string>
#include <memory>

#include "kwdb_common.h"

namespace kwdbts {
class KWDBPrecomputing;
/**
 * Precomputing Builder struct
 * <p>
 * rules for rule:
 * pattern: list of aggregations separated by comma in the form of the following
 * aggregation(source_columnId) as target_columnId
 * for example: sum(1) as 1, ave(1) as 2
 * internal pre computing builder
 * </p>
 *
 */
class KWDBPrecomputingBuilder{
 public:
  virtual ~KWDBPrecomputingBuilder() {}
  virtual void setPrecomputingName(const std::string& precomputingName) = 0;
  virtual void setInterval(const std::string& interval) = 0;
  virtual void setRule(const std::string& rule) = 0;

  virtual std::shared_ptr<kwdbts::KWDBPrecomputing> build() = 0;

  // clear all elements in KWDBPrecomputing
  virtual void clear() = 0;
};
using KWDBPrecomputingBuilderPtr = std::shared_ptr<KWDBPrecomputingBuilder>;

/**
 * KWDBPrecomputing is an object that can be used to specify certain aggregation operation
 * on existing devices. the result of the aggregation results will be saved in a separate
 * virtual device. The main purpose of this precomputing result is to let KWDB compiler to
 * expedite the KWDBQuery processing
 * 
 */
class KWDBPrecomputing {
 public:
  virtual ~KWDBPrecomputing() {}
  /**
   * create KWDBPrecomputing
   */
  virtual ErrorCode create() = 0;

  virtual std::string getPrecomputingName() = 0;

  virtual std::string getInterval() = 0;

  virtual std::string getRule() = 0;
};
using KWDBPrecomputingPtr = std::shared_ptr<KWDBPrecomputing>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBPRECOMPUTING_H_
