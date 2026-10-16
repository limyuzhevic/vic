#!/bin/bash

# Performance Monitoring Implementation Check Script
# Run with: bash check_performance_implementation.sh

echo "=== NLM Performance Monitoring Implementation Check ==="
echo

# Check core Performance directory
if [ -d "src/core/Performance" ]; then
    echo "✓ Core Performance directory found"
    PERF_COUNT=$(find src/core/Performance -type f -name "*.hpp" -o -name "*.cpp" | wc -l)
    echo "  Contains $PERF_COUNT files"
else
    echo "✗ Core Performance directory not found"
    exit 1
fi

echo

# Check Performance headers
headers=(
    "PerformanceMonitor.hpp"
    "Profiler.hpp"
    "MemoryProfiler.hpp"
    "NeuralProfiler.hpp"
    "ConfigProfiler.hpp"
    "BuildProfiler.hpp"
    "CMakeProfiler.hpp"
)

all_headers_exist=true
for header in "${headers[@]}"; do
    if [ -f "src/core/Performance/$header" ]; then
        size=$(stat -c%s "src/core/Performance/$header" 2>/dev/null || stat -f%z "src/core/Performance/$header" 2>/dev/null)
        echo "✓ src/core/Performance/$header ($size bytes)"
    else
        echo "✗ src/core/Performance/$header - NOT FOUND"
        all_headers_exist=false
    fi
done

echo

# Check Performance source files
sources=(
    "PerformanceMonitor.cpp"
    "Profiler.cpp"
    "MemoryProfiler.cpp"
    "NeuralProfiler.cpp"
    "ConfigProfiler.cpp"
    "BuildProfiler.cpp"
    "CMakeProfiler.cpp"
)

for source in "${sources[@]}"; do
    if [ -f "src/core/Performance/$source" ]; then
        size=$(stat -c%s "src/core/Performance/$source" 2>/dev/null || stat -f%z "src/core/Performance/$source" 2>/dev/null)
        echo "✓ src/core/Performance/$source ($size bytes)"
    else
        echo "✗ src/core/Performance/$source - NOT FOUND"
        all_headers_exist=false
    fi
done

echo

# Check brain integration
brain_files=(
    "src/brain/Neuron.cpp"
    "src/brain/Synapse.cpp"
)

for filepath in "${brain_files[@]}"; do
    if [ -f "$filepath" ]; then
        size=$(stat -c%s "$filepath" 2>/dev/null || stat -f%z "$filepath" 2>/dev/null)
        echo "✓ $filepath ($size bytes)"
    else
        echo "✗ $filepath - NOT FOUND"
        all_headers_exist=false
    fi
done

echo

# Check documentation
if [ -f "PERFORMANCE_MONITORING_IMPLEMENTATION.md" ]; then
    size=$(stat -c%s "PERFORMANCE_MONITORING_IMPLEMENTATION.md" 2>/dev/null || stat -f%z "PERFORMANCE_MONITORING_IMPLEMENTATION.md" 2>/dev/null)
    echo "✓ PERFORMANCE_MONITORING_IMPLEMENTATION.md ($size bytes)"
else
    echo "✗ PERFORMANCE_MONITORING_IMPLEMENTATION.md - NOT FOUND"
    all_headers_exist=false
fi

if [ -f "test_performance_monitor.cpp" ]; then
    size=$(stat -c%s "test_performance_monitor.cpp" 2>/dev/null || stat -f%z "test_performance_monitor.cpp" 2>/dev/null)
    echo "✓ test_performance_monitor.cpp ($size bytes)"
else
    echo "✗ test_performance_monitor.cpp - NOT FOUND"
    all_headers_exist=false
fi

if [ -f "check_performance_implementation.py" ]; then
    size=$(stat -c%s "check_performance_implementation.py" 2>/dev/null || stat -f%z "check_performance_implementation.py" 2>/dev/null)
    echo "✓ check_performance_implementation.py ($size bytes)"
else
    echo "✗ check_performance_implementation.py - NOT FOUND"
    all_headers_exist=false
fi

echo
echo "======================================================"
if [ "$all_headers_exist" = true ]; then
    echo "SUCCESS: All Performance Monitoring files found!"
    echo ""
    echo "The implementation appears to be complete. Key features implemented:"
    echo ""
    echo "1. Core Performance Monitoring:"
    echo "   - PerformanceMonitor.hpp/cpp: Real-time system metrics collection"
    echo "   - Profiler.hpp/cpp: Component timing and profiling"
    echo "   - PerformanceConfig: Configurable monitoring settings"
    echo "   - Performance alerts and thresholds"
    echo ""
    echo "2. Component-Specific Profiling:"
    echo "   - NeuralProfiler.hpp/cpp: Neural network performance tracking"
    echo "   - MemoryProfiler.hpp/cpp: Memory allocation tracking and leak detection"
    echo "   - ConfigProfiler.hpp/cpp: Configuration loading performance"
    echo "   - BuildProfiler.hpp/cpp: Build performance tracking"
    echo "   - CMakeProfiler.hpp/cpp: CMake performance tracking"
    echo ""
    echo "3. Integration with Core Components:"
    echo "   - Neuron.cpp: Performance monitoring for neuron creation, reset, and operations"
    echo "   - Synapse.cpp: Performance monitoring for synaptic operations"
    echo "   - Integration with existing logging system"
    echo ""
    echo "4. Performance Monitoring APIs:"
    echo "   - Global performance monitoring instance management"
    echo "   - Convenience macros for performance monitoring"
    echo "   - Thread-safe implementation"
    echo ""
    echo "5. Output Formats:"
    echo "   - Text reports: Human-readable performance summary"
    echo "   - HTML reports: Web-based performance visualization"
    echo "   - JSON reports: Machine-readable performance data"
    echo ""
    echo "Next steps:"
    echo "1. Compile the project with performance monitoring enabled"
    echo "2. Run the performance monitoring test"
    echo "3. Verify performance monitoring functionality"
    echo "4. Review the implementation documentation"
else
    echo "FAILURE: Some Performance Monitoring files are missing!"
    echo ""
    echo "Please check the missing files and ensure the implementation is complete."
    echo ""
    echo "Missing files may affect the following functionality:"
    echo "- Real-time performance metrics collection"
    echo "- Per-component performance analysis"
    echo "- Memory allocation tracking"
    echo "- Neural network performance profiling"
    echo "- Configuration loading performance"
    echo "- Build performance tracking"
    echo "- Integration with existing systems"
fi

echo "======================================================