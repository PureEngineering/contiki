DEFINES+=PROJECT_CONF_H=\"project-conf.h\"

all: 6lbr-client

ifeq ($(TARGET),)
  TARGET=srf06-cc26xx
endif

ifeq ($(BOARD),)
  BOARD=sensortag/cc2650
endif

CONTIKI_WITH_IPV6 = 1

CONTIKI=../../..
include $(CONTIKI)/Makefile.include
