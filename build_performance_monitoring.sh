#!/bin/bash
# Build script for NLM Performance Monitoring System

set -e

echo "=== Building NLM Performance Monitoring System ==="

# Check if we're in the right directory
if [ ! -f "/workspace/app/CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found in /workspace/app"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure CMake with performance monitoring enabled
echo "Configuring CMake..."
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

echo "CMake configuration completed successfully"

# Build the project
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

echo "Build completed successfully"

# Create examples directory
mkdir -p /workspace/app/performance_monitoring_examples

# List performance monitoring files
echo "=== Performance Monitoring Files ==="
find /workspace/app/src/performance_monitoring -name "*.hpp" -o -name "*.cpp" | sort

echo "=== Example Files ==="
find /workspace/app/performance_monitoring_examples -name "*.cpp" -o -name "*.md" | sort

echo "=== Build completed successfully ==="
echo "The NLM Performance Monitoring System has been built and is ready for use."
echo ""
echo "Usage Examples:"
echo "1. Basic Performance Monitoring:"
echo "   ./build/nlm --performance-level=Detailed --performance-enable-dashboard"
echo ""
echo "2. Performance Profiling:"
echo "   ./build/nlm --performance-profiling=CallStack,Memory,Event"
echo ""
echo "3. Performance Analysis:"
echo "   ./build/nlm --performance-analysis=Baseline,Regression,Optimization"
echo ""
echo "4. Performance Dashboard:"
echo "   ./build/nlm --performance-dashboard"
echo ""
echo "5. Performance Reporting:"
echo "   ./build/nlm --performance-report=Detailed,HTML --performance-output=/tmp/performance_report.html"
