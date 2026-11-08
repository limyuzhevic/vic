#!/bin/bash

# NLM Enhanced Demo Framework - Quick Start Script
# This script provides examples of how to use the new enhanced demo features

echo "=== NLM Enhanced Demo Framework - Quick Start ==="
echo

echo "This script demonstrates the new enhanced demo features:"
echo

echo "1. Basic Demo Run:"
echo "   ./nlm_enhanced_phase6_demo"
echo

echo "2. Enhanced Demo with Configuration:"
echo "   ./nlm_enhanced_phase6_demo --config demo_config.json --neuron-count 1000"
echo

echo "3. Demo with JSON Output:"
echo "   ./nlm_enhanced_phase6_demo --json-output --output results.json"
echo

echo "4. Demo with Progress Tracking:"
echo "   ./nlm_enhanced_phase6_demo --verbose --progress-tracking"
echo

echo "5. Demo with Metrics Collection:"
echo "   ./nlm_enhanced_phase6_demo --enable-performance-monitoring --output metrics.csv"
echo

echo "6. Help and Usage Information:"
echo "   ./nlm_enhanced_phase6_demo --help"
echo

echo "7. Original Phase 6 Demo (for backward compatibility):"
echo "   ./nlm_phase6_demo"
echo

echo "8. Benchmark Demo:"
echo "   ./nlm_benchmark"
echo

echo "=== Key Features Available ==="
echo

echo "• Modular Test Framework: Run specific tests or complete suites"
echo "• Advanced CLI: 20+ command-line options for customization"
echo "• JSON Configuration: Load configurations from JSON files"
echo "• Multiple Output Formats: Text, JSON, CSV, HTML"
echo "• Progress Tracking: Real-time progress bars and status displays"
echo "• Metrics Collection: Comprehensive performance monitoring"
echo "• Error Recovery: Robust error handling and retry mechanisms"
echo "• Extensible Architecture: Plugin-style for new demos"
echo "• Backward Compatibility: Original demos still work"
echo "• Performance Benchmarking: Detailed performance analysis"
echo "• Configuration Management: Merge JSON and CLI configurations"
echo "• Advanced Logging: Configurable logging with multiple levels"
echo "• Output Customization: Flexible output formatting"
echo

echo "=== Configuration Examples ==="
echo

echo "Create custom configuration file:"
echo "cat > my_config.json << 'EOF'"
echo "{
  \"configuration\": {
    \"neuron_count\": 1000,
    \"max_steps\": 5000,
    \"enable_checkpointing\": true
  },
  \"demo_settings\": {
    \"verbose_logging\": true,
    \"json_output\": true,
    \"output_file\": \"my_results.json\"
  }
}"
echo "EOF"
echo

echo "Run with custom configuration:"
echo "./nlm_enhanced_phase6_demo --config my_config.json"
echo

echo "=== Build Instructions ==="
echo

echo "Standard build:"
echo "mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
echo

echo "Build with all features:"
echo "mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON && make -j4"
echo

echo "=== Running Demos ==="
echo

echo "Enhanced Phase 6 Demo (NEW):"
echo "./nlm_enhanced_phase6_demo"
echo

echo "Original Phase 6 Demo (UNCHANGED):"
echo "./nlm_phase6_demo"
echo

echo "Benchmark Demo:"
echo "./nlm_benchmark"
echo

echo "=== Performance Monitoring ==="
echo

echo "Enable performance monitoring:"
echo "./nlm_enhanced_phase6_demo --enable-performance-monitoring --benchmark-iterations 10"
echo

echo "Collect metrics and save to file:"
echo "./nlm_enhanced_phase6_demo --json-output --output performance_metrics.json"
echo

echo "=== Advanced Features ==="
echo

echo "Enable checkpointing for long-running demos:"
echo "./nlm_enhanced_phase6_demo --enable-checkpoint --enable-replay"
echo

echo "Enable development features for experimentation:"
echo "./nlm_enhanced_phase6_demo --enable-development"
echo

echo "Run with specific test filter:"
echo "./nlm_enhanced_phase6_demo --test-filter "plasticity"
echo

echo "=== Output Formats ==="
echo

echo "Text output (default):"
echo "./nlm_enhanced_phase6_demo"
echo

echo "JSON output:"
echo "./nlm_enhanced_phase6_demo --json-output"
echo

echo "CSV output:"
echo "./nlm_enhanced_phase6_demo --output-format csv --output results.csv"
echo

echo "HTML output:"
echo "./nlm_enhanced_phase6_demo --output-format html --output report.html"
echo

echo "=== Error Handling ==="
echo

echo "Run with error recovery enabled:"
echo "./nlm_enhanced_phase6_demo --enable-error-recovery --max-retry-attempts 3"
echo

echo "Get detailed error information:"
echo "./nlm_enhanced_phase6_demo --verbose --log-level debug"
echo

echo "=== Help and Documentation ==="
echo

echo "View help information:"
echo "./nlm_enhanced_phase6_demo --help"
echo

echo "View complete documentation:"
echo "cat docs/ENHANCEMENT_SUMMARY.md"
echo

echo "View framework documentation:"
echo "cat docs/ARCHITECTURE.md"
echo

echo "=== Quick Usage Summary ==="
echo

echo "1. Basic: ./nlm_enhanced_phase6_demo"
echo "2. Configure: ./nlm_enhanced_phase6_demo --config my_config.json"
echo "3. Monitor: ./nlm_enhanced_phase6_demo --verbose --json-output"
echo "4. Benchmark: ./nlm_enhanced_phase6_demo --enable-performance-monitoring"
echo "5. Help: ./nlm_enhanced_phase6_demo --help"
echo

echo "=== The enhanced demo framework provides ==="
echo

echo "✓ 20+ command-line options for customization"
echo "✓ JSON configuration support with file loading"
echo "✓ Multiple output formats (Text, JSON, CSV, HTML)"
echo "✓ Real-time progress tracking and visualization"
echo "✓ Comprehensive metrics collection and analysis"
echo "✓ Robust error handling and recovery mechanisms"
echo "✓ Modular architecture for easy extension"
echo "✓ Backward compatibility with existing demos"
echo "✓ Performance benchmarking and optimization"
echo "✓ Configuration merging (CLI + JSON)"
echo "✓ Advanced logging with multiple levels"
echo "✓ Progress bar and status line displays"
