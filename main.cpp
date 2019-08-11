#include<iostream>
#include<string>
using namespace std;


#include"producer.h"
#include"consumer.h"


int main()
{
    /*
     * 卡夫卡消息生产者类使用范例
    CKafkaProducer* Kafkapr = new CKafkaProducer("localhost:9092", "jyxTest", 0);
    Kafkapr->Start();
    Kafkapr->Send("Hello world! I am kafka producer");
    */


    //卡夫卡消息消费者类使用范例
    CKafkaConsumer::RetMsg   msg;
    CKafkaConsumer* pConsum = new CKafkaConsumer("localhost:9092", "jyxTest","", RD_KAFKA_OFFSET_END);
    pConsum->Start();
    while(true)
    {
        pConsum->Consume(500,msg);
        if (msg.m_errCode == RdKafka::ERR_NO_ERROR)
            cout<<msg.m_msg<<endl;
    }
    pConsum->Stop();
    return 0;
};

