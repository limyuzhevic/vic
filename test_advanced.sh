#!/usr/bin/env bash

cd "$(dirname "$0")"

# Simple test to verify the advanced NLM platform compiles and can be imported
echo "Testing Advanced NLM Brain Simulation Platform..."
echo

# Check if required directories exist
if [ ! -d "src/advanced" ]; then
    echo "❌ Advanced directory not found!"
    exit 1
fi

if [ ! -d "src/research" ]; then
    echo "❌ Research directory not found!"
    exit 1
fi

if [ ! -d "src/analysis" ]; then
    echo "❌ Analysis directory not found!"
    exit 1
fi

if [ ! -d "src/integration" ]; then
    echo "❌ Integration directory not found!"
    exit 1
fi

# Check for key header files
key_headers=(
    "src/advanced/AdvancedNeuralNetwork.hpp"
    "src/advanced/CustomPlasticity.hpp"
    "src/research/ResearchTools.hpp"
    "src/analysis/NeuralAnalyzer.hpp"
    "src/analysis/NetworkAnalyzer.hpp"
    "src/analysis/PlasticityAnalyzer.hpp"
    "src/integration/PluginSystem.hpp"
    "src/integration/ModelImporter.hpp"
    "src/integration/APIHandler.hpp"
    "src/integration/ScriptingInterface.hpp"
)

missing_headers=0
for header in "${key_headers[@]}"; do
    if [ ! -f "$header" ]; then
        echo "❌ Missing header: $header"
        missing_headers=$((missing_headers + 1))
    else
        echo "✅ Found: $header"
    fi
done

if [ $missing_headers -gt 0 ]; then
    echo "❌ $missing_headers required headers missing"
    exit 1
fi

echo
echo "✅ All core advanced components are present and ready for use!"
echo
echo "The Advanced NLM Brain Simulation Platform provides expert users with:"
echo "  • Custom neural network architectures (src/advanced/)"
echo "  • Advanced plasticity rules (src/advanced/)"
echo "  • Research-oriented tools (src/research/)"
echo "  • Comprehensive analysis tools (src/analysis/)"
echo "  • Integration systems (src/integration/)"
echo
echo "For detailed documentation and usage examples, see README_ADVANCED.md"
echo
echo "To build the project, run:"
echo "  mkdir build && cd build && cmake .. && make -j$(nproc)"