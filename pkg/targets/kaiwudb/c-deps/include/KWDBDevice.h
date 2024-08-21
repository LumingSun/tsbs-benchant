// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBDEVICE_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBDEVICE_H_

#include <any>
#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "kwdb_common.h"
#include "KWDBEndpoint.h"
#include "KWDBQuery.h"
#include "KWDBAlarmInfo.h"
#include "KWDBNamedQuery.h"
#include "KWDBPrecomputing.h"

namespace kwdbts {

/**
 * @brief Tuples is a supported class for KWDBDevice
 * that could send data to server side
 */
class Tuples {
 public:
  virtual ~Tuples() = default;
  /**
   * @brief add a row of data to Tuples
   * 
   * @param oneDataRow should be a byte array, filled with the Timestamp endpoint.value and endpoint.valid in
   *                   this group in the order the endpoints created(also listed). each field should be exactly
   *                   the length of the dataType, and no gap between any 2 fields.
   * @return boolean if addData success or not
   */
  virtual bool addToData(void* oneDataRow) = 0;
  /**
   * @brief add multiple rows of data to Tuples
   *
   * @param numOfRows number of rows for this round
   * @param dataRows multiple oneDataRow
   * @return boolean if addData success or not
   */
  virtual bool addToData(int numOfRows, void* dataRows) = 0;

  /**
   * @brief add data to Tuples
   * 
   * @param dataRows A double vector represents actual data.
   *                 The size of outside vector is the number of rows, and the size of inside vector must be equal
   *                 to the number of fields, which includs Timestamp and endpoint.value and endpoint.valid.
   *                 The endpoints shoud be in the order that they are created or listed.
   *                 An empty std::any indicates that the endpoint has no value.
   *
   * @return boolean if addData success or not
   */
  virtual bool addToData(const std::vector<std::vector<std::any>>& dataRows) = 0;

  virtual void* getData() = 0;

  virtual std::string getErrorMessage() = 0;

  virtual void clear() = 0;
};
using TuplesPtr = std::shared_ptr<Tuples>;

/**
 * @brief KWDBDevice represents a device in the microgrid,
 * which collects data from one to multiple data points,
 * and can generate data streams of multiple collection 
 * points at the same time
 */
class KWDBDevice {
 public:
  /**
   * create should be called after user create/delete endpoints.
   * create will cause the device and endpoints get persisted
   * after create get called, we no longer allow create/delete endpoints for this device.
   * so, please create all the endpoints before call the following method.
   */
  virtual ErrorCode create() = 0;

  virtual std::string getDeviceName() const = 0;
  virtual int getActiveTime() const = 0;

  /**
   * set attribute, save device related metadata to kwdb
   *
   * @param name  attribute name
   * @param attribute attribute value
   * @return ErrorCode
   */
  virtual ErrorCode setAttribute(
    const std::string& name,
    const std::string& attribute) = 0;

  /**
   * get attribute, fetch metadata information from kwdb
   *
   * @param attribute string pointer
   * @param name attribute name
   * @return ErrorCode
   */
  virtual ErrorCode getAttribute(
    const std::string& name,
    std::string *attribute) = 0;

  /**
   * list all the attributes associated with this device
   *
   * @param attributes PairString vector
   * @return ErrorCode
   */
  virtual ErrorCode listAttributes(std::vector<PairString> *attributes) = 0;

  /**
   * create EndpointBuilder
   * 
   * @param name      endpoint name
   * @param dataType  type of data field
   * @return pointer of EndpointBuilder
   */
  virtual KWDBEndpointBuilderPtr createEndpointBuilder() = 0;

  /**
   * @brief 创建点对象 add a Endpoint to device
   * 
   * @param[in] endpoint  KWDBEndpoint对象
   * @return ErrorCode 
   */
  virtual ErrorCode addEndpoint(const KWDBEndpointPtr &endpoint) = 0;

  /**
   * remove an endpoint
   * internally need to invalidate all related namedquery
   *
   * @param endpoint KWDBEndpoint shared pointer
   * @param name endpoint name
   * @return ErrorCode
   */
  virtual ErrorCode lookupEndpoint(
    const std::string& name,
    std::shared_ptr<KWDBEndpoint> *endpoint) = 0;

  /**
   * get the reference of the object instance of an endpoint
   *
   * @param endpointName endpoint name
   * @return ErrorCode
   */
  virtual ErrorCode deleteEndpoint(const std::string& endpointName) = 0;

  /**
   * method to list all endpoints
   *
   * @param[out] endpoints KWDBEndpoint vector 
   * @return ErrorCode
   */
  virtual ErrorCode listEndpoints(std::vector<KWDBEndpointPtr> *endpoints) = 0;

  /**
   * method to list all endpoints in a endpoint group
   *
   * @param endpointGroupName endpoint group name
   * @param[out] endpoints KWDBEndpoint vector 
   * @return ErrorCode
   */
  virtual ErrorCode listEndpointsByGroup(
    const std::string& endpointGroupName,
    std::vector<KWDBEndpointPtr> *endpoints) = 0;

  /**
   * method create a Tuples object for a group,
   * will choose device name as endpointGroupName
   *
   * @param[out] tuples shared_ptr to a Tuples aobject 
   * @return ErrorCode
   */
  virtual ErrorCode createTuples(TuplesPtr *tuples) = 0;

  /**
   * method create a Tuples object for a group
   *
   * @param endpointGroupName endpoint group name.
   * @param[out] tuples shared_ptr to a Tuples aobject 
   * @return ErrorCode
   */
  virtual ErrorCode createTuples(
    const std::string& endpointGroupName,
    TuplesPtr *tuples) = 0;

  /**
   * save data api using tuples.
   * Tuples is a input class for user to prepare inserted data.
   * Tuples will have endpoint group information as well as all the data in
   * serialized format. KWDB will provide a utility function inside Tuples for
   * user to serialize its input data row into KWDB format.\n
   * Please see Tuples class for more details
   *
   * @param tuples data info
   * @return ErrorCode
   */
  virtual ErrorCode saveData(
    const TuplesPtr& tuples) = 0;

  /**
   * save data api using json format
   *
   * @param jsonStringData data info in json format
   * @param endpointGroupName endpoint group name
   * @return ErrorCode
   *
   * @example:
   * Function: save collected data to endpoint group, collection data are 
   * in json string format like the following:
   *  
   * { [
   *   {"timestamp" : 1-2-2002
   *    "points": [
   *              "point1": {
   *                  "value": 123.45,   
   *                  "valid": 1        
   *              },
   *              "point2": {
   *                  "value": 45.67,   
   *                  "valid": 1        
   *              }
   *              ]
   *   },
   *   {"timestamp" : 1-2-2002
   *    "points": [
   *              "point3": {
   *                  "value": 123.45,   
   *                  "valid": 1        
   *              },
   *              "point4": {
   *                  "value": 45.67,   
   *                  "valid": 1        
   *              }
   *              ]
   *   }
   *   ]
   *  }
   * returnValue: STATUS_OK if data saved successfully
   *              STATUS_ERROR if any error happens
   */
  virtual ErrorCode saveData(
    const std::string& jsonStringData,
    const std::string& endpointGroupName) = 0;

  /**
   * create PrecomputingBuilder
   *
   * @return pointer of precomputingBuilder
   */
  virtual KWDBPrecomputingBuilderPtr createPrecomputingBuilder() = 0;

  /**
   * create Precomputing
   * @param pre   pointer of KWDBPrecomputing
   * @return errorCode
   */
  virtual ErrorCode createPrecomputing(KWDBPrecomputingPtr* pre) = 0;

  /**
   * drop Precomputing
   * @param pre   pointer of KWDBPrecomputing
   * @return errorCode
   */
  virtual ErrorCode deletePrecomputing(const std::string& precomputingName) = 0;

  /**
   * show created Precomputing
   * 
   * @return errorCode
   */
  virtual ErrorCode listPrecomputings(std::vector<KWDBPrecomputingPtr> *precomputings) = 0;
};
using KWDBDevicePtr = std::shared_ptr<KWDBDevice>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBDEVICE_H_
