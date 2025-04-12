# Define directories
SRC_DIR=src
BIN_DIR=bin

# Define source file and output file
SRC_FILE=$(SRC_DIR)/k_means.cpp
BIN_FILE=$(BIN_DIR)/k_means

# Default target
all: compile

# Compile target
compile: $(BIN_FILE)

$(BIN_FILE): $(SRC_FILE)
	clang++ -O2 -g -fopencilk -o $@ $<

# Run target
run: $(BIN_FILE)

# Clean target
clean:
	rm -f $(BIN_FILE)
	rm $(SRC_DIR)/*.csv

.PHONY: all compile run clean
