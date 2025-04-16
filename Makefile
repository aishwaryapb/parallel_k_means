# Makefile for Parallel K-means Project

# Directories
SRC_DIR := src
BIN_DIR := bin

# Files
MAIN_SRC_FILE := $(SRC_DIR)/k_means.cpp
MAIN_BIN_FILE := $(BIN_DIR)/k_means

IC_SRC_FILE := $(SRC_DIR)/image_compression.cpp
IC_BIN_FILE := $(BIN_DIR)/image_compression

# Compiler and Flags
CXX := clang++
CXXFLAGS := -O2 -g -fopencilk

# Targets
.PHONY: compile run clean

compile:
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(MAIN_BIN_FILE) $(MAIN_SRC_FILE)
	$(CXX) $(CXXFLAGS) -o $(IC_BIN_FILE) $(IC_SRC_FILE)
	@echo "Compilation complete. Binary files created in $(BIN_DIR) directory"

run: compile
	@$(MAIN_BIN_FILE)

run-ic: compile
	@$(IC_BIN_FILE)

clean:
	@rm -rf $(BIN_DIR)
	@rm -f src/data/*.csv
	@rm -f src/images/compressed*
	@echo "Cleaned build artifacts."

