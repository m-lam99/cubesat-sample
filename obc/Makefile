# Pre-compiler and Compiler flags
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb
PRE_FLAGS := -MMD -MP -IC

# Project directory structure
BIN := .
SRC := .
INC := beagle/include bus/include components/include
MAINFILE := $(SRC)/main.cpp

# Build directories and output
TARGET := $(BIN)/main
BUILD := obj

# Library search directories and flags
EXT_LIB :=
LDFLAGS := -pthread
LDPATHS :=

# Include directories
INC_DIRS := $(INC) $(shell find $(SRC) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Construct build output and dependency filenames
SRCS := $(shell find $(SRC) -name *.cpp)
OBJS := $(subst $(SRC)/,$(BUILD)/,$(addsuffix .o,$(basename $(SRCS))))
DEPS := $(OBJS:.o=.d)

# Run task
run: build
	@echo "Executing..."
	./$(TARGET) $(ARGS)

# Build task
build: clean all

# Main task
all:
	+$(MAKE) -C bus
	+$(MAKE) -C components
	+$(MAKE) -C beagle

# Task producing target from built files
$(TARGET): $(OBJS)
	@echo "Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDPATHS) $(LDFLAGS)

# Compile all cpp files
$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(PRE_FLAGS) $(INC_FLAGS) -c -o $@ $< $(LDPATHS) $(LDFLAGS)

# Clean task
.PHONY: clean
clean:
	@echo "Clearing..."
	rm -rf build

# Include all dependencies
-include $(DEPS)