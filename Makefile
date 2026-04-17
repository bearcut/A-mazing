# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/a_mazing

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*

.PHONY: all clean directories