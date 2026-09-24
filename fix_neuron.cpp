#include <string>
#include <algorithm>
#include <cmath>

// Function to fix the dt * 1000.0f issue in stepLIF
g++ -std=c++20 -Wall -Wextra -pedantic -I. \
  tests/test_neuron.cpp \
  src/core/Types/Types.cpp \
  src/core/Random/Random.cpp \
  src/brain/Neuron.cpp \
  -o test_neuron 2>&1 | head -50
