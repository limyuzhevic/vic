#!/bin/bash
# Try to compile just test_neuron with fixes
g++ -std=c++20 -Wall -Wextra -pedantic -I. \
  -I./src \
  tests/test_neuron.cpp \
  src/core/Types/Types.cpp \
  src/core/Random/Random.cpp \
  src/brain/Neuron.cpp \
  -o test_neuron 2>&1 | head -100
