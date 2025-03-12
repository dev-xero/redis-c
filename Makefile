CC = g++
CFLAGS = -Wall -Wextra -I./include
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

TARGETS = $(BUILD_DIR)/client $(BUILD_DIR)/redis

all: setup $(TARGETS)

setup:
	mkdir -p $(BUILD_DIR)

# Compiling client
$(BUILD_DIR)/client: $(BUILD_DIR)/client.o $(BUILD_DIR)/utils.o
	$(CC) $^ -o $@ $(LDFLAGS)

# Compiling redis
$(BUILD_DIR)/redis: $(BUILD_DIR)/redis.o $(BUILD_DIR)/utils.o
	$(CC) $^ -o $@ $(LDFLAGS)

# Generic object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all setup clean
