SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
LOCAL_INCLUDE_DIR := include 
GLOBAL_INCLUDE_DIR := ../include


CC := g++
CFLAGS := -c -Wall

all: $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -I$(LOCAL_INCLUDE_DIR) -I$(GLOBAL_INCLUDE_DIR) $< -o $@

clean:
	rm -f $(OBJECTS)