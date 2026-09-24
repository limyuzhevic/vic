#!/bin/bash
g++ -std=c++20 -Wall -Wextra -pedantic -I. tests/test_neuron.cpp \
  src/core/Types/Types.cpp src/core/Config/Config.cpp \
  src/core/Random/Random.cpp src/brain/Neuron.cpp \
  -o test_neuron 2>&1
