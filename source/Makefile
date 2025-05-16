CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3

# Default config (can override with `make CONFIG=K64N256`)
CONFIG ?= K64N128

# Source files (excluding test.cpp)
SRC_FILES = $(filter-out test.cpp, $(wildcard *.cpp))
OBJS = $(SRC_FILES:.cpp=.o)

# Executable name
TARGET = run_test

# Include paths
INCLUDES = -I.

# Default rule
all: clean consts.h $(TARGET)

# Symlink or copy the correct consts file
consts.h:
	cp consts_$(CONFIG).h consts.h

# Compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -DCONFIG_$(CONFIG) -c $< -o $@

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean
clean:
	rm -f $(OBJS) $(TARGET) consts.h