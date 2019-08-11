
#include "producer.h"
 
CKafkaProducer::CKafkaProducer(const string &brokers, const string &topics, int nPpartition /*= 1*/)
    : m_bRun(true), m_strTopics(topics), m_strBroker(brokers), m_nPpartition(nPpartition)
{
}
 
CKafkaProducer::~CKafkaProducer()
{
    Stop();
}
 
bool CKafkaProducer::Start()
{
    string errstr = "";
    Stop();
 
    // Create configuration objects
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
 
    /*Set configuration properties,设置broker list。即设置卡夫卡集群*/
    if (conf->set("metadata.broker.list", m_strBroker, errstr) != RdKafka::Conf::CONF_OK)
    {
        //std::cerr << "RdKafka conf set brokerlist failed :" << errstr.c_str() << endl;
        return false;
    }

    /* Set delivery report callback */
    //conf->set("dr_cb", &m_producerDeliveryReportCallBack, errstr);
    conf->set("event_cb", &m_producerEventCallBack, errstr);
 
    // Create producer using accumulated global configuration.
    m_pProducer = RdKafka::Producer::create(conf, errstr);
    if (!m_pProducer)
    {
        //std::cerr << "Failed to create producer: " << errstr << std::endl;
        return false;
    }
    
    // Create topic handle
    m_pTopic = RdKafka::Topic::create(m_pProducer, m_strTopics, tconf, errstr);
    if (!m_pTopic)
    {
        //std::cerr << "Failed to create topic: " << errstr << std::endl;
        return false;
    }
    return true;
}


bool CKafkaProducer::Send(const string &msg)
{
    if (!m_bRun)
        return false;
    /*
     * Produce message
    */
    RdKafka::ErrorCode resp = m_pProducer->produce(m_pTopic, m_nPpartition,
                                                   RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                                                   const_cast<char *>(msg.c_str()), msg.size(),
                                                   NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR)
    {
        //std::cerr << "Produce failed: " << RdKafka::err2str(resp) << std::endl;
        return false;
    }
 
    m_pProducer->poll(0);
 
    /* Wait for messages to be delivered */  //firecat add
    while (m_bRun && m_pProducer->outq_len() > 0) 
    {
        m_pProducer->poll(1000);
    }

    return true;
}
 
void CKafkaProducer::Stop()
{
    if (m_pTopic)
        delete m_pTopic;
    if (m_pProducer)
        delete m_pProducer;
}


