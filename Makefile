.PHONY: default clean


TARGET := kafka
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS))

#INCLUDE= -I /3rdsoft/boost_1_68_0/ 
LIBS=  -lrdkafka -lrdkafka++


all:$(TARGET)
%.o:%.cpp
	g++ --std=c++0x -Wall -g -c $<  
$(TARGET):$(OBJS)   
	g++ --std=c++0x -Wall -g -o $@ $^  $(LIBS)


clean:
	rm -rf ./*o
	rm -rf ./$(TARGET)
