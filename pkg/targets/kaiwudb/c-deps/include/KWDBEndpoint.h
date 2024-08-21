// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBENDPOINT_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBENDPOINT_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kwdb_common.h"
#include "KWDBQuery.h"
#include "KWDBNamedQuery.h"

namespace kwdbts {
class KWDBEndpoint;
class KWDBEndpointBuilder{
 public:
  virtual ~KWDBEndpointBuilder() {}
  virtual void setKWDBEndpointName(const std::string& endpointName) = 0;
  virtual void setKWDBEndpointDataType(const EndpointDataType& dataType) = 0;
  virtual void setKWDBEndpointGroupName(const std::string& endpointGroupName) = 0;
  virtual void setValidity(bool validity) = 0;
  virtual void setNullable(bool nullable) = 0;
  virtual std::shared_ptr<kwdbts::KWDBEndpoint> build() = 0;
  virtual void clear() = 0;
};
using KWDBEndpointBuilderPtr = std::shared_ptr<KWDBEndpointBuilder>;

/**
 * @brief KWDBEndpoint logically represents to a specific
 * data collection point, and physically represents 
 * to a specific sensor.
 * KWDBEndpoint is used to manage basic data computing services
 * such as data saving, query, data flow service, etc.
 * It is the smallest data management unit in the
 * computing model of the KWDB.
 */
class KWDBEndpoint {
 public:
  /**
   * get endpoint name
   *
   * @return string of endpoint name
   */
  virtual std::string getEndpointName() const = 0;

  virtual std::string getEndpointGroupName() const = 0;
  /**
   * get endpoint data type
   *
   * @return EndpointDataType of endpoint data type
   */
  virtual EndpointDataType getEndpointDataType() const = 0;

  virtual bool getValidity() const = 0;

  /**
   * set attribute, save endpoint related metadata to kwdb
   *
   * @param name  attribute name
   * @param value attribute value
   * @return ErrorCode
   */
  virtual ErrorCode setAttribute(
    const std::string& name,
    const std::string& attribute) = 0;

  /**
   * get attribute, fetch metadata information from kwdb
   *
   * @param name attribute name
   * @param[out] attribute attribute value
   * @return errorCode
   */
  virtual ErrorCode getAttribute(
    const std::string& name,
    std::string *attribute) = 0;

  /**
   * list all the attributes associated with this endpoint
   *
   * @param[out] attributes attribute list
   * @return errorCode
   */
  virtual ErrorCode listAttributes(std::vector<PairString> *attributes) = 0;

  /**
   * batch import data
   * clarify details from ems team
   * needs to clarify the format of batch data
   *
   * @param inputData input data
   * @return errorCode
   */
  virtual ErrorCode importDataBatch(const std::string& inputData) = 0;

  /**
   * batch export data
   * clarify requirement from ems team
   *
   * @param[out] outputData data path
   * @return errorCode
   */
  virtual ErrorCode exportDataBatch(std::string *outputData) = 0;

  /**
   * get most recent endpoint data
   *
   * @param[out] res KWDBResultSetPtr
   */
  virtual ErrorCode getLatestData(KWDBResultSetPtr *res) = 0;

  /**
   * get last minute data
   *
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastMinuteData(KWDBResultSetPtr *res) = 0;

  /**
   * get last minute aggregation data
   *
   * @param aggregations aggregations
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastMinuteAgg(
    const std::vector<Aggregation>& aggregations,
    KWDBResultSetPtr *res) = 0;

  /**
   * get last hour data
   *
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastHourData(KWDBResultSetPtr *res) = 0;

  /**
   * get last hour aggregation data
   *
   * @param aggregations aggregations
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastHourAgg(
    const std::vector<Aggregation>& aggregations,
    KWDBResultSetPtr *res) = 0;

  /**
   * get last day data
   *
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastDayData(KWDBResultSetPtr *res) = 0;

  /**
   * get last day aggregation data
   *
   * @param aggregations aggregations
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastDayAgg(
    const std::vector<Aggregation>& aggregations,
    KWDBResultSetPtr *res) = 0;

  /**
   * get last month data
   *
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastMonthData(KWDBResultSetPtr *res) = 0;

  /**
   * get last month aggregation data
   *
   * @param aggregations aggregations
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getLastMonthAgg(
    const std::vector<Aggregation>& aggregations,
    KWDBResultSetPtr *res) = 0;

  /**
   * get data for certain range
   *
   * @param from     from timestamp
   * @param to       to timestamp
   * @param exclude  exclude (FROM/TO/NONE/BOTH)
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getDataForRange(
    TimeStamp from,
    TimeStamp to,
    Exclude exclude,
    KWDBResultSetPtr *res) = 0;

  virtual ErrorCode getDataForRange(
    const std::string &from,
    const std::string &to,
    Exclude exclude,
    KWDBResultSetPtr *res) = 0;

  /**
   * get aggregation for certain range
   *
   * @param aggregations aggregations
   * @param from         from timestamp
   * @param to           to timestamp
   * @param exclude      exclude (FROM/TO/NONE/BOTH)
   * @param[out] res KWDBResultSetPtr
   * @return errorCode
   */
  virtual ErrorCode getAggForRange(
    const std::vector<Aggregation>& aggregations,
    TimeStamp from,
    TimeStamp to,
    Exclude exclude,
    KWDBResultSetPtr *res) = 0;

  virtual ErrorCode getAggForRange(
    const std::vector<Aggregation>& aggregations,
    const std::string &from,
    const std::string &to,
    Exclude exclude,
    KWDBResultSetPtr *res) = 0;

  /**
   * get data for certain range with specific interval
   *
   * @param from     from timestamp
   * @param to       to timestamp
   * @param exclude  exclude (FROM/TO/NONE/BOTH)
   * @param[out] res KWDBResultSetPtr
   * @param interval     interval(unit: ms)
   * @return errorCode
   */
  virtual ErrorCode getDataForRangeWithInterval(
    TimeStamp from,  // from time
    TimeStamp to,   // to time
    Exclude exclude,
    KWDBResultSetPtr *res,
    Time interval = 15) = 0;  // unit second. default 15s

  virtual ErrorCode getDataForRangeWithInterval(
    const std::string &from,  // from time
    const std::string &to,   // to time
    Exclude exclude,
    KWDBResultSetPtr *res,
    Time interval = 15) = 0;  // unit second. default 15s

  /**
   * get aggregation for certain range with specific interval
   *
   * @param aggregations aggregations
   * @param from         from timestamp
   * @param to           to timestamp
   * @param exclude      exclude (FROM/TO/NONE/BOTH)
   * @param[out] res KWDBResultSetPtr
   * @param interval     interval(unit: ms)
   * @return errorCode
   */ 
  virtual ErrorCode getAggForRangeWithInterval(
    const std::vector<Aggregation>& aggregations,
    TimeStamp from,
    TimeStamp to,
    Exclude exclude,
    KWDBResultSetPtr *res,
    Time interval = 15) = 0;  // unit second. default 15s

  virtual ErrorCode getAggForRangeWithInterval(
    const std::vector<Aggregation>& aggregations,
    const std::string &from,
    const std::string &to,
    Exclude exclude,
    KWDBResultSetPtr *res,
    Time interval = 15) = 0;  // unit second. default 15s
};
using KWDBEndpointPtr = std::shared_ptr<KWDBEndpoint>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBENDPOINT_H_
