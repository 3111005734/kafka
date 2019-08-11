#include<iostream>
#include<string>
using namespace std;
#include"producer.h"


int main()
{
    //KafkaProducerClient* KafkaprClient_ = new KafkaProducerClient("localhost:9092", "test", 0);
 
    KafkaProducer* Kafkapr_ = new KafkaProducer("localhost:9092", "jyxTest", 0);
    Kafkapr_->Init();
    Kafkapr_->Send("Hello world! I am kafka producer");


    return 0;
};

