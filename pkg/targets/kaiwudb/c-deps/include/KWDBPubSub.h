// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

#ifndef KWDBTS_CLIENT_INCLUDE_KWDBPUBSUB_H_
#define KWDBTS_CLIENT_INCLUDE_KWDBPUBSUB_H_

#include <utility>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "KWDBEndpoint.h"
#include "kwdb_common.h"

namespace kwdbts {

// TODO(yuyue02): 加入class简述与函数描述
class KWDBPubSub {
 public:
  /**
   * @brief Create a Topic object
   * 
   * @param topicName the name of topic
   * @param topicSpec a json format of topic
   * @return ErrorCode
   * 
   * @example topicSpec Json format:
   * {
   *     deviceName: device1,   // tenant1.portal1.device1, or portal1.device1, device1 (qualify with current tenant and portal)
   *     endpointName: endpoint1,
   *     startTime: timestamp1,      // optional field
   *     endTime: timestamp2,        // optional field
   *     rules: [
   *         {
   *             computingRules: { 
   *                 state: 0,      // optional field
   *                 upperBound: 13.64,      // optional field
   *                 excludeRule: 0 (both), 1 (upper included), 2 (lower included), 3 (none)      // optional field
   *                 windowSize: 3,000ms,      // optional field
   *                 windowOffset: 2,000ms,      // optional field
   *                 //MATCHING(90) means [ # of (raw_data matching conditions) / # of total_data ] >= 90%
   *                 Aggregation: LAST/AVG/SUM/MATCHING(90)/etc       // come along with windowSize (windowOffset is still optional)
   *             },
   *             pubRules: {
   *                 repeatPub: 200,000(ms/s)   // suppress the pub for, optional, or the number of occurences, like 10 times
   *             }
   *         },            
   *         {
   *             computingRules: { 
   *                 state: 1,
   *                 upperBound: 23,
   *                 lowerBound: 13.64,
   *                 excludeRule: 0 (both), 1 (upper included), 2 (lower included), 3 (none)
   *                 windowSize: 3000ms,
   *                 windowOffset: 2000ms,
   *                 Aggregation: LAST/AVG/SUM/MATCHING(60)/etc 
   *             },
   *             pubRules: {
   *                 repeatPub: 200000(ms/s) / 10
   *             }
   *         },
   *         {
   *             computingRules: { 
   *                 state: 2,
   *                 upperBound: 35.71,
   *                 lowerBound: 23,
   *                 excludeRule: 0 (both), 1 (upper included), 2 (lower included), 3 (none)
   *                 windowSize: 4000ms,
   *                 windowOffset: 2500ms,
   *                 Aggregation: LAST/AVG/SUM/MATCHING(90)/etc 
   *             },
   *             pubRules: {
   *                 repeatPub: 300000(ms/s) / 10
   *             }
   *         },
   *     ]
   * }
   * 
   */
  virtual ErrorCode createTopic(const std::string& topicName, const std::string& topicSpec) = 0;

  /**
   * delete Topic
   * 
   * @param topicName the name of topic
   * @return ErrorCode 
   */
  virtual ErrorCode deleteTopic(const std::string& topicName) = 0;

  // subscribe API
  virtual ErrorCode subscribe(const std::string& topic) = 0;
  virtual ErrorCode unSubscribe(const std::string& topic) = 0;

  /*
   * get next message
   * output format：
   * {
   *  "time": unix时间戳,
   *  "datas":[
   *       {
   *      "device": "devname1”,
   *      "point":"pointname1”,
   *      "value”:1.234
   *       },
   *       {
   *      "device": "devname1”,
   *      "point":"pointname2”,
   *      "value”:2.345
   *       },
   *       ...
   *      ]
   * }
   */
  // virtual std::string getNextMessage(const std::string& topic) = 0;
  virtual ErrorCode getNextMessage(const std::string& topic, std::string *json) = 0;

  virtual std::string cleanUp() = 0;
};
using KWDBPubSubPtr = std::shared_ptr<KWDBPubSub>;

}  // namespace kwdbts
#endif  // KWDBTS_CLIENT_INCLUDE_KWDBPUBSUB_H_
