# Minimal test compilation
CXX = g++
CXXFLAGS = -std=c++20 -I. -I./src
LDFLAGS = -std=c++20

TARGET = test_stdp_optimization
SOURCES = test_optimization.cpp src/plasticity/STDP.cpp src/brain/Synapse.cpp src/core/Random/Random.cpp src/core/Types/Types.cpp src/core/Config/Config.cpp src/core/Logger/Logger.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(TARGET)
	rm -f test_optimization.o
	rm -f src/plasticity/STDP.o
	rm -f src/brain/Synapse.o
	rm -f src/core/Random/Random.o
	rm -f src/core/Types/Types.o
	rm -f src/core/Config/Config.o
	rm -f src/core/Logger/Logger.o
