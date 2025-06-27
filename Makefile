CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3

# Default config (can override with `make CONFIG=K64N256`)
CONFIG ?= K64N80

# Source files (excluding test.cpp)
SRC_DIR = source
SRC_FILES = $(filter-out $(SRC_DIR)/test.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(SRC_FILES:.cpp=.o)

# Executable name
TARGET = run_test

# Include paths
INCLUDES = -I.

# Default rule
all: clean consts.h $(TARGET)

# Symlink or copy the correct consts file
consts.h:
	cp $(SRC_DIR)/consts_$(CONFIG).h $(SRC_DIR)/consts.h

# Compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DCONFIG_$(CONFIG) -c $< -o $@

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean
clean:
	rm -f $(OBJS) $(SRC_DIR)/$(TARGET) $(SRC_DIR)/consts.h