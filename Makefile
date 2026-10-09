# Simple Makefile for NLM refactoring verification
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -I./src
TARGET = verify_refactoring

SRC = test_refactoring.cpp \
      src/core/TestConfig.cpp \
      src/core/TestHelper.cpp \
      src/core/LearningExperiment.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
