DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

all: cc26xx-web-demo

ifeq ($(TARGET),)
  TARGET=srf06-cc26xx
endif

ifeq ($(BOARD),)
  BOARD=canary/cc2650
endif

PROJECT_SOURCEFILES += cetic-6lbr-client.c mqtt-client.c
PROJECT_SOURCEFILES += pb_encode.c pb_common.c sensor_message.pb.c pb_decode.c
CONTIKI_WITH_IPV6 = 1

APPS += mqtt

CONTIKI=../../..
include $(CONTIKI)/Makefile.include
