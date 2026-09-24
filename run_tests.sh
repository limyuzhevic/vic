#!/bin/bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -I. -I./src -I./python src/core/Config/Config.cpp -c -o Config.o
g++ -std=c++20 -Wall -Wextra -Wpedantic -I. -I./src -I./python src/core/Config/Config.cpp src/tests/test_config.cpp -o test_config
./test_config
