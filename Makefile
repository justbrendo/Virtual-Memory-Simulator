# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -std=c99 -g -w

# Output executable name
OUTPUT = main.out

# Source files
SRCS = $(wildcard *.c)

# Build directory
BUILD_DIR = build

# Object files in build directory
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(OUTPUT)

# Link object files to create the executable
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compile .c files to .o files in build directory
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT)

# Run the executable with arguments
run: $(OUTPUT)
	./$(OUTPUT) $(ARGS)

# PHONY targets
.PHONY: all clean run

# Allow passing arguments to the run target
ARGS = random compilador.log 4 128