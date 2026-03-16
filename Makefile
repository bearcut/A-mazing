# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
# LDFLAGS = -lm  # Uncomment if you end up using the math.h library (<math.h>)

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Target Executable
# Note: Dropped the .exe extension for Linux conventions
TARGET = $(BIN_DIR)/a_mazing

# Find all .c files in src/ and map them to .o files in build/
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: directories $(TARGET)

# Linking the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compiling source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build and bin directories exist before compiling
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*

.PHONY: all clean directories
