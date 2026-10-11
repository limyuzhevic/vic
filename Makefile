#!/usr/bin/env g++ -std=c++17 -Wall -Wextra -O2

# Compile and test the Phase 2 implementations
# This tests basic compilation and functionality

targets = test_implementation test_runner

clean:
	@echo "Cleaning build artifacts..."
	rm -f test_implementation test_runner *.o

build: test_implementation

test_implementation: test_implementation.cpp
	@echo "Building test implementation..."
	g++ -std=c++17 -Wall -Wextra -O2 test_implementation.cpp -o test_implementation

run: test_implementation
	@echo "Running test implementation..."
	./test_implementation

.PHONY: clean build run