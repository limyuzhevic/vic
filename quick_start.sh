#!/usr/bin/env bash
# quick_start.sh

# Quick Start Script for NLM Phase 2

echo "=============================================="
echo "NLM Phase 2: Neural Learning Machine - Quick Start"
echo "=============================================="
echo

echo "This script provides a quick start guide for using NLM Phase 2."
echo

echo "Prerequisites:"
echo "  - CMake 3.16 or higher"
echo "  - C++20 compatible compiler (g++ 9+, clang 10+)"

echo

echo "1. Build NLM:"
echo "   mkdir build && cd build"
echo "   cmake -DCMAKE_BUILD_TYPE=Release"
echo "   make -j$(nproc)"
echo

echo "2. Run basic demonstration:"
echo "   ../build/nlm"
echo

echo "3. Explore experiment modes:"
echo "   ../build/nlm --help                    # Show all options"
echo "   ../build/nlm -e learning                # Learning experiment"
echo "   ../build/nlm -e advanced               # Advanced testing suite"
echo "   ../build/nlm -e stdp                    # STDP verification"
echo

echo "4. Get started with configuration:"
echo "   ../build/nlm --set neuron_count=1000    # Custom configuration"
echo "   ../build/nlm -c my_config.cfg          # Load config file"
echo "   ../build/nlm -o results.json           # Save output"
echo

echo "5. Performance optimization:"
echo "   ../build/nlm --profile                  # Enable profiling"
echo "   ../build/nlm --debug                    # Enable debug mode"
echo "   ../build/nlm --threads 8               # Set thread count"
echo

echo "For detailed documentation, visit:"
echo "   https://github.com/nlm-project/nlm"
echo

echo "=============================================="
echo "Happy experimenting with NLM Phase 2!"
echo "=============================================="