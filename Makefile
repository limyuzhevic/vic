# Simple Makefile to compile and test concept formation integration

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -I./src
LDFLAGS = -pthread

# Source files
CORE_SOURCES = src/core/Types/Types.cpp \
    src/core/Config/Config.cpp \
    src/core/Random/Random.cpp \
    src/core/Logger/Logger.cpp \
    src/core/SimulationClock/SimulationClock.cpp

BRAIN_SOURCES = src/brain/Neuron.cpp \
    src/brain/Synapse.cpp \
    src/brain/NeuralPopulation.cpp \
    src/brain/NeuralRegion.cpp \
    src/brain/Brain.cpp

COGNITION_SOURCES = src/cognition/ConceptFormation.cpp \
    src/cognition/NeuralPlanner.cpp

# Object files
CORE_OBJS = $(CORE_SOURCES:.cpp=.o)
BRAIN_OBJS = $(BRAIN_SOURCES:.cpp=.o)
COGNITION_OBJS = $(COGNITION_SOURCES:.cpp=.o)

# Targets
all: test_concept_integration

# Test executable
test_concept_integration: test_concept_integration.o $(CORE_OBJS) $(BRAIN_OBJS) $(COGNITION_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ test_concept_integration.o $(CORE_OBJS) $(BRAIN_OBJS) $(COGNITION_OBJS) $(LDFLAGS)

# Test program
test_concept_integration.o: test/test_concept_integration.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Core objects
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f *.o test_concept_integration
	rm -f src/core/*.o src/brain/*.o src/cognition/*.o

.PHONY: all clean