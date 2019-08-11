
#include "consumer.h"
 

CKafkaConsumer::CKafkaConsumer(const std::string& brokers, const std::string& topics, std::string groupid, int64_t offsetMode)
:brokers_(brokers), topics_(topics), groupid_(groupid), offset_(offsetMode)
{
 }
 
 
CKafkaConsumer::~CKafkaConsumer()
{
    if (kafka_consumer_)
        delete kafka_consumer_;
    if (topic_)
        delete topic_;
};
 
bool CKafkaConsumer::Start()
{
    if (m_bRuning)
        return true;

    RdKafka::Conf *conf = nullptr;
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if(!conf)
        return false;
 
    std::string errstr;
    /*设置集群。卡夫卡可以单机运行也可集群运行。kafka集群配置项，在老版本用broker.list配置，新版本用bootstrap.servers。
      集群的配置格式形如 192.168.130.100:9092,192.168.130.200:9092,192.168.130.300:9092，broker间用逗号分隔。
     */
    if (conf->set("bootstrap.servers", brokers_, errstr) != RdKafka::Conf::CONF_OK)
    {
        //fprintf(stderr, "RdKafka conf set brokerlist failed : %s\n", errstr.c_str());
        return false;
    }
 
    /*设置consumer group*/
    if (conf->set("group.id", groupid_, errstr) != RdKafka::Conf::CONF_OK)
    {
        //fprintf(stderr, "RdKafka conf set group.id failed : %s\n", errstr.c_str());
        return false;
    }
 
    std::string strfetch_num = "10240000";
    /*每次从单个分区中拉取消息的最大尺寸*/
    if(conf->set("max.partition.fetch.bytes", strfetch_num, errstr) != RdKafka::Conf::CONF_OK)
    {
        //fprintf(stderr, "RdKafka conf set max.partition failed : %s\n", errstr.c_str());
        return false;
    }
 
    /*创建kafka consumer实例*/
    kafka_consumer_ = RdKafka::Consumer::create(conf, errstr);
    if(!kafka_consumer_)
    {
        //fprintf(stderr, "failed to ceate consumer\n");
        return false;
    }
    delete conf;
 
    RdKafka::Conf *tconf = nullptr;
    /*创建kafka topic的配置*/
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    if(!tconf)
    {
        //fprintf(stderr, "RdKafka create topic conf failed\n");
        return false;
    }
 
    /*kafka + zookeeper,当消息被消费时,会想zk提交当前groupId的consumer消费的offset信息,
    当consumer再次启动将会从此offset开始继续消费.在consumter端配置文件中(或者是
    ConsumerConfig类参数)有个"autooffset.reset"(在kafka 0.8版本中为auto.offset.reset),
    有2个合法的值"largest"/"smallest",默认为"largest",此配置参数表示当此groupId下的消费者,
    在ZK中没有offset值时(比如新的groupId,或者是zk数据被清空),consumer应该从哪个offset开始
    消费.largest表示接受接收最大的offset(即最新消息),smallest表示最小offset,即从topic的
    开始位置消费所有消息.*/
    if(tconf->set("auto.offset.reset", "smallest", errstr) != RdKafka::Conf::CONF_OK)
    {
        //fprintf(stderr, "RdKafka conf set auto.offset.reset failed : %s\n", errstr.c_str());
        return false;
    }
 
    topic_ = RdKafka::Topic::create(kafka_consumer_, topics_, tconf, errstr);
    if(!topic_)
    {
        //fprintf(stderr, "RdKafka create topic failed : %s\n", errstr.c_str());
        return false;
    }
    delete tconf;
 
    RdKafka::ErrorCode resp = kafka_consumer_->start(topic_, partition_, offset_);
    if (resp != RdKafka::ERR_NO_ERROR)
    {
        //fprintf(stderr, "failed to start consumer : %s\n", RdKafka::err2str(resp).c_str());
        return false;
    }
    m_bRuning = true;
    return true;
}
 
 
bool CKafkaConsumer::Consume(int timeout_ms, CKafkaConsumer::RetMsg & ret)
{
    RdKafka::Message *msg = nullptr;
    char buf[1024];
    memset(buf,0,sizeof(buf));

    if(m_bRuning)
    {
        msg = kafka_consumer_->consume(topic_, partition_, timeout_ms);
        kafka_consumer_->poll(0);

        //解析消息
        ret.m_errCode = msg->err();
        ret.m_errMsg = msg->errstr();
        ret.m_lastOffset = msg->offset();
        sprintf(buf,"%.*s\n",  static_cast<int>(msg->len()),  static_cast <const char*>(msg->payload()));
        ret.m_msg = buf;

        delete msg;
        return true;
    }
    else
        return false;
}
 
void CKafkaConsumer::Stop()
{
    m_bRuning = false;
    if(kafka_consumer_){
        kafka_consumer_->stop(topic_, partition_);
        delete kafka_consumer_;
        kafka_consumer_ = nullptr;
    }

    if(topic_){
        delete topic_;
        topic_ = nullptr;
    }

    /*销毁kafka实例*/
    RdKafka::wait_destroyed(5000);
}
