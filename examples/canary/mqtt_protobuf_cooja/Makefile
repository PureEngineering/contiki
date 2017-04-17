DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

all: cc26xx-web-demo 

PROJECT_SOURCEFILES += cetic-6lbr-client.c mqtt-client.c 
PROJECT_SOURCEFILES += pb_encode.c pb_common.c sensor_message.pb.c pb_decode.c
CONTIKI_WITH_IPV6 = 1

CFLAGS += -DIEEE_ADDR_NODE_ID=$(NODEID) -DPROJECT_CONF_H=\"project-conf.h\"

APPS += mqtt

CONTIKI=../../..


CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
