#!/bin/bash

# Test script to check if the NLM framework compiles and runs

echo "=== NLM Framework Build Check ==="

# Check for required files
if [ ! -f "pyproject.toml" ]; then
    echo "ERROR: pyproject.toml not found"
    exit 1
fi

# Check for C++ files in src directory
if [ ! -d "src" ]; then
    echo "ERROR: src directory not found"
    exit 1
fi

# Count files
CPP_FILES=$(find src -name "*.cpp" | wc -l)
HEADER_FILES=$(find src -name "*.hpp" | wc -l)
TEST_FILES=$(find tests -name "*.cpp" | wc -l)

echo "C++ source files: $CPP_FILES"
echo "C++ header files: $HEADER_FILES"
echo "Test files: $TEST_FILES"

# Check for critical missing files
CRITICAL_FILES=(
    "src/plasticity/PlasticityRule.cpp"
    "src/plasticity/PlasticityRule.hpp"
    "src/brain/ExperimentRunner.cpp"
)

for file in "${CRITICAL_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file exists"
    else
        echo "✗ $file MISSING"
    fi
done

# Check for duplicate implementations
if [ -f "src/plasticity/Hebbian.hpp" ] && [ -f "src/plasticity/PlasticityRule.hpp" ]; then
    echo "WARNING: Duplicate Hebbian implementations detected!"
    echo "  - src/plasticity/Hebbian.hpp (real implementation)"
    echo "  - PlasticityRule.hpp (placeholder implementation)"
fi

# Check for key functionality
if [ ! -f "src/brain/Brain.cpp" ]; then
    echo "ERROR: src/brain/Brain.cpp missing - critical implementation"
    exit 1
fi

echo ""
echo "=== Build Requirements Analysis ==="
echo "1. CMakeLists.txt: Should be present for building"
echo "2. Python bindings: pyproject.toml suggests pybind11"
echo "3. RandomGenerator: Essential for initialization"
echo "4. SpikeSystem: Referenced but not implemented"

echo ""
echo "=== Missing Core Components ==="
MISSING_COMPONENTS=(
    "SpikeSystem implementation"
    "SpikeEvent and DelayedSpikeEvent classes"
    "CheckpointManager"
    "Neuromodulator base class"
    "Novelty, Dopamine, Curiosity implementations"
    "PredictionError implementation"
    "NeuralAssociativeMemory"
    "NeuralPlanner"
    "ConceptFormation"
    "AttentionalSelection"
    "DevelopmentSystem"
)

for component in "${MISSING_COMPONENTS[@]}"; do
    echo "  - $component"
done

echo ""
echo "=== Key Issues ==="
echo "1. Duplicate Hebbian implementations causing confusion"
echo "2. Missing ExperimentRunner.cpp implementation"
echo "3. Empty AntiHebbianRule and BCMRule classes"
echo "4. Brain.cpp references undefined NeuralRegion methods"
echo "5. No actual neural simulation functionality"

echo ""
echo "=== Recommendation ==="
echo "1. Remove duplicate Hebbian implementations (keep real one)"
echo "2. Implement missing ExperimentRunner.cpp"
echo "3. Fill in AntiHebbianRule and BCMRule"
echo "4. Fix NeuralRegion implementation to match Brain.cpp usage"

echo ""
echo "=== Current Status ==="
echo "This codebase has significant gaps and appears to be in early development."
echo "Many classes are declared but not fully implemented."
exit 1
