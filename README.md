# kafka
安装范例：https://blog.csdn.net/wqh8522/article/details/79163467

报错解决范例：https://blog.csdn.net/fst438060684/article/details/80662305



基础概念：

        Broker：Kafka集群包含一个或多个服务器，这些服务器就是Broker

        Topic：每条发布到Kafka集群的消息都必须有一个Topic

        Partition：是物理概念上的分区，为了提供系统吞吐率，在物理上每个Topic会分成一个或多个Partition，每个Partition对应一个文件夹

        Producer：消息产生者，负责生产消息并发送到Kafka Broker

        Consumer：消息消费者，向kafka broker读取消息并处理的客户端。

        Consumer Group：每个Consumer属于一个特定的组，组可以用来实现一条消息被组内多个成员消费等功能。

        

配置：

（1）配置文件为config/server.properties.常用需要的配置配置项如下几个：

        broker.id=2

        卡夫卡可以单机部署也可分布式集群部署。一台卡夫卡服务即为一个broker,id值可任意整数，只需保证集群部署时此标记值为互不相同。

        log.dirs=/usr/local/kafka/log

        日志保存目录

        zookeeper.connect=172.17.3.205:2181,172.17.3.206:2181,172.17.3.207:2181,172.17.3.208:2181

        卡夫卡依赖zookeeper进行集群部署管理，此处配置zookeeper集群各个设备的IP。但是zookeeper可以单机模式运行，此时只有一个地址。

        listeners=PLAINTEXT://localhost:9092

        在配置集群的时候，必须设置，不然以后的操作会报找不到leader的错误

        advertised.listeners=PLAINTEXT://localhost:9092

        

        

命令行操作命令（jyxTest为测试创建的topic名）：

  启动运行卡夫卡：

  bin/kafka-server-start.sh -daemon config/server.properties  （运行卡夫卡前必须先运行所依赖的zookeeper。此外启动脚本必须带配置文件路径作为命令行参数）

  启动运行zookeeper:

  bin/zookeeper-server-start.sh -daemon  config/zookeeper.properties   (kafka中有zookeeper启动关闭脚本，但是要先配置好zookeeper.properties)

  查询topic：
  
  bin/kafka-topics.sh --list --zookeeper localhost:2181

  删除topic

  bin/kafka-topics.sh --delete --topic jyxTest --zookeeper zk:2181

  创建启动发布者

  bin/kafka-console-producer.sh --broker-list localhost:9092 --topic jyxTest

  显示消费者

  bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic jyxTest --from-beginning

特别注意点：命令行操作时，如上面示例命令里的localhost可换成具体的IP，但是必须与配置文件里配置项listeners和advertised.listeners保持一致，系统不会自动转换




卡夫卡C++编程：

        使用开源的第三方库librdkafka来访问操作卡夫卡：https://github.com/edenhill/librdkafka。在源码中也有使用范例代码。

        librdkafka库函数解释的说明及范例：https://www.cnblogs.com/swyroc/p/10678379.html

                                        https://www.2cto.com/kf/201807/761748.html

        
