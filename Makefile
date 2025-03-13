CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -I./include
CXXFLAGS = $(CFLAGS)
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

C_SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CPP_SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

C_OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SRC_FILES))
CPP_OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC_FILES))

OBJ_FILES = $(C_OBJ_FILES) $(CPP_OBJ_FILES)
TARGETS = $(BUILD_DIR)/client $(BUILD_DIR)/redis

all: setup $(TARGETS)

setup:
	mkdir -p $(BUILD_DIR)

# Compiling client
$(BUILD_DIR)/client: $(BUILD_DIR)/client.o $(BUILD_DIR)/utils.o
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compiling redis
$(BUILD_DIR)/redis: $(BUILD_DIR)/redis.o $(BUILD_DIR)/utils.o
	$(CXX) $^ -o $@ $(LDFLAGS)

# C Object Files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# C++ Object Files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all setup clean
