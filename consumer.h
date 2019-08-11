#ifndef KAFKACOMSUMER_H_
#define KAFKACOMSUMER_H_

#include <memory>
#include <getopt.h>
#include <csignal>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;
#include "librdkafka/rdkafkacpp.h"
#include "librdkafka/rdkafka.h" 

class CKafkaConsumer
{
public:
    struct RetMsg{
        /*错误码可能值为如下几种:RdKafka::ERR__TIMED_OUT、RdKafka::ERR_NO_ERROR、RdKafka::ERR__PARTITION_EOF、
         * RdKafka::ERR__UNKNOWN_TOPIC、RdKafka::ERR__UNKNOWN_PARTITION
        */
        int m_errCode;
        int64_t m_lastOffset;
        string m_msg;
        string m_errMsg;
    };

public:
    /*消息队列里可能已有旧消息，入参offsetMode控制对旧消息的处理。可选值为：RD_KAFKA_OFFSET_BEGINNING、RD_KAFKA_OFFSET_END、RD_KAFKA_OFFSET_STORED等
      卡夫卡可以单机运行也可集群运行。集群的配置格式形如 192.168.130.100:9092,192.168.130.200:9092,192.168.130.300:9092，broker间用逗号分隔。
    */
    CKafkaConsumer(const std::string& brokers, const std::string& topics, std::string groupid, int64_t offsetMode = RD_KAFKA_OFFSET_END);
    
    virtual ~CKafkaConsumer();
 
    bool Start();

    /*  librdkafka底层函数:consume()消费一个消息、consume_batch() - 消费一个或多个消息、consume_callback()消费本地队列中的所有消息且每一个都调用回调函数
        本接口使用底层consume()从消息队列获取一条消息，用ret返回。
    */
    bool Consume(int timeout_ms, CKafkaConsumer::RetMsg & ret);

    void Stop();

private: 
    std::string brokers_;
    std::string topics_;
    std::string groupid_;
 
    bool m_bRuning = false;
    int64_t last_offset_ = 0;
    RdKafka::Consumer *kafka_consumer_ = nullptr;   
    RdKafka::Topic    *topic_          = nullptr;
    int64_t           offset_;
    int32_t           partition_       = 0;
     
};

#endif
