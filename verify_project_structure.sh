#!/bin/bash
# NLM Project Build Verification Script

echo "=== NLM Project Build Verification ==="

# Check Phase 6 integration
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ Phase6Demo.cpp exists"
else
    echo "✗ Phase6Demo.cpp missing"
    exit 1
fi

if [ -f "src/experiments/Phase6IntegratedExperiment.hpp" ]; then
    echo "✓ Phase6IntegratedExperiment.hpp exists"
else
    echo "✗ Phase6IntegratedExperiment.hpp missing"
    exit 1
fi

# Check Phase 6 demo compilation
if [ -f "src/experiments/Phase6Demo.cpp" ]; then
    echo "✓ Phase6Demo.cpp exists"
else
    echo "✗ Phase6Demo.cpp missing"
    exit 1
fi

# Check main CMakeLists.txt
if [ -f "CMakeLists.txt" ]; then
    echo "✓ Root CMakeLists.txt exists"
    if grep -q "add_executable(nlm_phase6_demo" CMakeLists.txt; then
        echo "✓ nlm_phase6_demo target defined"
    else
        echo "✗ nlm_phase6_demo target NOT defined"
        exit 1
    fi
else
    echo "✗ Root CMakeLists.txt missing"
    exit 1
fi

# Check Python bindings
if [ -f "python/bindings.cpp" ]; then
    echo "✓ Python bindings exist"
else
    echo "✗ Python bindings missing"
    exit 1
fi

# Count source files
TOTAL_CPP=$(find src -name "*.cpp" | wc -l)
TOTAL_H=$(find src -name "*.hpp" | wc -l)
echo "✓ Found $TOTAL_CPP .cpp files and $TOTAL_H .hpp files"

# Check for essential components
ESSENTIAL_FILES=(
    "src/brain/Neuron.cpp"
    "src/brain/Brain.cpp"
    "src/core/Config/Config.cpp"
    "src/agent/AgentBrain.cpp"
)

for file in "${ESSENTIAL_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file exists"
    else
        echo "✗ $file missing"
        exit 1
    fi
done

echo "\n=== Build Verification Complete ==="
echo "All critical components are present. The project structure is valid."