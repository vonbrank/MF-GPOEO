CUDA_DIR = /opt/cuda-11.2.2/
CUDA_INC = $(CUDA_DIR)/include
CUDA_GDK = $(CUDA_DIR)/gdk
CUDA_LIB = $(CUDA_DIR)/lib64
CUDA_STUBS = $(CUDA_DIR)/lib64/stubs

CC = g++
CFLAGS = -Wall -O3 -Wno-unused-parameter
SPECIALFLAGS = -lnvidia-ml -lpthread

SRC = main.cpp PowerManager.cpp mgo_router.cpp daemon_controller.cpp
TARGET = PerfMeasure.bin

JSTP_DIR = ../jstp
JSTP_SOURCES := $(wildcard $(JSTP_DIR)/*.cpp)
JSTP_OBJS := $(JSTP_SOURCES:.cpp=.o)
JSTP_INCLUDE_DIR := $(JSTP_DIR)/include

GLOBAL_INCLUDE_DIR := ../include

all: $(TARGET)

$(TARGET): $(SRC) jstp
	$(CC) $(CFLAGS) -I$(CUDA_INC) -I$(CUDA_GDK)  -I$(JSTP_INCLUDE_DIR) -I$(GLOBAL_INCLUDE_DIR) -L$(CUDA_LIB) -L$(CUDA_STUBS) -lcudart -lcuda $(SPECIALFLAGS) -o $@ $(SRC) $(JSTP_OBJS)

jstp: 
	make -C $(JSTP_DIR)

clean:
	rm -f *.o *.bin
	make -C $(JSTP_DIR) clean