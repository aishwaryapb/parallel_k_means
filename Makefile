# Makefile for Parallel K-means Project

# Directories
SRC_DIR := src
BIN_DIR := bin

# Files
SRC_FILE := $(SRC_DIR)/k_means.cpp
BIN_FILE := $(BIN_DIR)/k_means

# Compiler and Flags
CXX := clang++
CXXFLAGS := -O2 -g -fopencilk

# Targets
.PHONY: compile run clean

compile:
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN_FILE) $(SRC_FILE)
	@echo "Compilation complete. Binary created at $(BIN_FILE)"

run: compile
	@$(BIN_FILE)

clean:
	@rm -rf $(BIN_DIR)
	@rm src/*.csv
	@echo "Cleaned build artifacts."

