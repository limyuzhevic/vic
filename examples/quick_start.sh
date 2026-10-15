#!/bin/bash

# Quick Start Script for NLM Enhanced Benchmarking System
# This script provides quick access to the enhanced benchmarking capabilities

echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
echo "║                        NLM Enhanced Benchmarking Quick Start                  ║"
echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
echo ""
echo "This script demonstrates the enhanced benchmarking system for NLM."
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Building NLM..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    cd ..
    echo "Build completed."
else
    echo "Build directory exists. Assuming project is already built."
fi

echo ""
echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
echo "║                           QUICK START OPTIONS                                 ║"
echo "╠════════════════════════════════════════════════════════════════════════════════════════════════════════╣"
echo "║ 1. Quick Benchmark Demonstration                                            ║"
echo "║ 2. Comprehensive Benchmark Demonstration                                   ║"
echo "║ 3. Configuration-based Benchmark                                            ║"
echo "║ 4. System Integration Demonstration                                        ║"
echo "║ 5. Custom Configuration                                                   ║"
echo "║ 6. Export Results                                                           ║"
echo "║ 7. View Examples and Documentation                                        ║"
echo "║ 8. Exit                                                                    ║"
echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
echo ""
read -p "Select an option (1-8): " option

case $option in
    1)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                     QUICK BENCHMARK DEMONSTRATION                          ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Running quick benchmark with minimal monitoring..."
        echo ""
        ./nlm_benchmark_demonstration --quick
        ;;
    2)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                   COMPREHENSIVE BENCHMARK DEMONSTRATION                       ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Running comprehensive benchmark with full monitoring and analysis..."
        echo ""
        ./nlm_benchmark_demonstration --comprehensive
        ;;
    3)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                  CONFIGURATION-BASED BENCHMARK                               ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Running configuration-based benchmark using examples/ directory..."
        echo ""
        echo "Available configuration files:"
        ls -la examples/*.cfg
        echo ""
        read -p "Enter configuration file path (or leave blank for default): " config_file
        if [ -z "$config_file" ]; then
            config_file="examples/quick_benchmark.cfg"
        fi
        if [ -f "$config_file" ]; then
            ./nlm_benchmark_demonstration --config "$config_file"
        else
            echo "Configuration file not found: $config_file"
            echo "Using default configuration..."
            ./nlm_benchmark_demonstration
        fi
        ;;
    4)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                  SYSTEM INTEGRATION DEMONSTRATION                             ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Demonstrating integration with existing NLM systems..."
        echo ""
        ./nlm_benchmark_demonstration --mode integration
        ;;
    5)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                      CUSTOM CONFIGURATION                                   ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Creating custom benchmark configuration..."
        echo ""
        cat > custom_benchmark.cfg << EOF
# Custom Benchmark Configuration
benchmark_type = 0
target_steps_per_second = 8000.0
target_memory_mb = 1024.0
warmup_steps = 150
measurement_steps = 1500
num_threads = 2
monitor_memory = true
monitor_cpu = true
monitor_events = true
monitor_fragmentation = true
export_results = true
export_path = results/custom
export_format = both
compute_statistics = true
compute_regression = true
EOF
        echo "Custom configuration created: custom_benchmark.cfg"
        echo ""
        ./nlm_benchmark_demonstration --config custom_benchmark.cfg
        ;;
    6)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                            EXPORT RESULTS                                   ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Exporting results to different formats..."
        echo ""
        echo "1. Running quick benchmark and exporting to CSV"
        ./nlm_benchmark_demonstration --quick 2>&1 | tail -20
        echo ""
        echo "2. Exporting results to JSON format (simulated)"
        echo "Note: JSON export requires implementation of JSON export functionality"
        echo ""
        echo "To see actual export functionality, refer to the code in src/benchmark/export/"
        ;;
    7)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                          EXAMPLES AND DOCUMENTATION                           ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Available examples:"
        echo "1. Quick benchmark (examples/quick_benchmark.cfg)"
        echo "2. Micro benchmark (examples/micro_benchmark.cfg)"
        echo "3. Large benchmark (examples/large_benchmark.cfg)"
        echo ""
        echo "Documentation files:"
        echo "- BENCHMARKING.md: Comprehensive benchmarking guide"
        echo "- README.md: Main project documentation"
        echo "- docs/PERFORMANCE.md: Enhanced performance documentation"
        echo ""
        echo "For more information, see:"
        echo "- BENCHMARKING.md for detailed usage instructions"
        echo "- README.md for project overview"
        echo "- src/benchmark/ for framework details"
        echo ""
        echo "To view examples, you can run:"
        echo "  cat examples/quick_benchmark.cfg"
        echo "  cat examples/micro_benchmark.cfg"
        echo "  cat examples/large_benchmark.cfg"
        ;;
    8)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
        echo "║                                 EXITING                                    ║"
        echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
        echo ""
        echo "Thank you for using NLM Enhanced Benchmarking System!"
        echo "For more information, visit the documentation files."
        exit 0
        ;;
    *)
        echo "Invalid option selected. Please run the script again."
        ;;
esac

# Print final message
echo ""
echo "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗"
echo "║                          BENCHMARKING COMPLETE                                ║"
echo "╚════════════════════════════════════════════════════════════════════════════════════════════════════════╝"
echo ""
echo "For more information and examples, refer to:"
echo "- BENCHMARKING.md: Comprehensive benchmarking guide"
echo "- README.md: Main project documentation"
echo "- examples/: Configuration file examples"
echo "- src/benchmark/: Framework source code"
echo ""
echo "The enhanced benchmarking system provides comprehensive performance"
echo "analysis while maintaining full backward compatibility with the"
echo "original benchmark.cpp implementation."