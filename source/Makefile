CXX = g++
CXXFLAGS = -Wall -O3 -std=c++17
LDFLAGS =

# All .cpp files and corresponding .o files
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Define special objects
RUN_TEST_MAIN = run_test.o
TEST_MAIN     = test.o

# Object files without the main files
COMMON_OBJS = $(filter-out $(RUN_TEST_MAIN) $(TEST_MAIN), $(OBJ_FILES))

# Targets
EXECUTABLES = clean run_test test

# Default target
all: $(EXECUTABLES)

run_test: $(RUN_TEST_MAIN) $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

test: $(TEST_MAIN) $(COMMON_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLES)
