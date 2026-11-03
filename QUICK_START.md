#!/usr/bin/bash

# Quick Start Script for NLM Enhancements
# This script demonstrates the new unified interface and advanced features

echo "========================================"
echo "NLM Enhancement Quick Start"
echo "========================================"

echo "\n1. Building the Enhanced NLM System:"
echo "   mkdir build"
echo "   cd build"
echo "   cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "   make -j4"

echo "\n2. Testing the Unified Interface:"
echo "   ./nlm --phase 6 --neurons 500 --help"

echo "\n3. Running Phase 6 Integration Demo:"
echo "   ./nlm --phase 6 --neurons 1000 --verbose"

echo "\n4. Running Phase 2 Basic Demo:"
echo "   ./nlm --phase 2 --checkpoint"

echo "\n5. Advanced CLI Usage:"
echo "   nlm_cli.py run --phase 6 --neurons 2000"
echo "   nlm_cli.py batch 10 --phases 2 3 4 5 6"
echo "   nlm_cli.py experiment cognitive_learning"

echo "\n6. Python API Examples:"
echo "   import pynlm"
echo "   config = pynlm.createDefaultConfig()"
echo "   brain = pynlm.createBrain(config)"
echo "   brain.initialize()"
echo "   brain.step(0)"

echo "\n7. Key Improvements:"
echo "   ✓ Unified CLI interface for all phases"
echo "   ✓ Advanced batch processing and scripting"
echo "   ✓ Comprehensive documentation and examples"

echo "\n======================================="
echo "Enhancements Complete!"
echo "========================================"
