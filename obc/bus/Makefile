# Pre-compiler and Compiler flags
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb
PRE_FLAGS := -MMD -MP -IC

# Project directory structure
BIN := .
SRC := src
INC := include

# Build directories and output
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

# Build task
build: clean all

# Main task
all: $(OBJS)

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