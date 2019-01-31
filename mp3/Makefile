CROSS = 
CC = $(CROSS)gcc
CXX = $(CROSS)g++
SRC_PATH = src
LIB = -L ./lib
INCLUDE = -I ./include
SRCS = $(wildcard $(SRC_PATH)/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
VERSION = 1.0.0.1
TARGET = find_$(VERSION)
$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(INCLUDE) $(LIB)
$(OBJS):%.o:%.c
	$(CC) $< -c -o $@ $(INCLUDE) $(LIB)
.PHONY:clean
clean:
	rm -rf $(SRC_PATH)/*.o $(TARGET)
